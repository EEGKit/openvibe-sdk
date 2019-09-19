#include "ovtkCAlgorithmScenarioExporter.h"
#include <vector>
#include <sstream>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEToolkit;

namespace OpenViBEToolkit
{
	namespace
	{
		class CAlgorithmScenarioExporterHelper
		{
			friend class CAlgorithmScenarioExporter;
		public:

			CAlgorithmScenarioExporterHelper(IAlgorithmContext& context, CAlgorithmScenarioExporter& parent);
			bool exportBox(IMemoryBuffer& memoryBuffer, const IBox& box);
			bool exportComment(IMemoryBuffer& memoryBuffer, const IComment& rComment);
			bool exportMetadata(IMemoryBuffer& memoryBuffer, const IMetadata& metadata);
			bool exportSetting(IMemoryBuffer& memoryBuffer, const IScenario& scenario, uint32_t ui32SettingIndex);
			bool exportInput(IMemoryBuffer& memoryBuffer, const IScenario& scenario, uint32_t index);
			bool exportOutput(IMemoryBuffer& memoryBuffer, const IScenario& scenario, uint32_t outputIdx);
			bool exportLink(IMemoryBuffer& memoryBuffer, const ILink& rLink);
			void exportAttributes(const IAttributable& attributable, IMemoryBuffer& memoryBuffer, const CIdentifier& idAttributes,
								  const CIdentifier& idAttribute, const CIdentifier& idAttributeIdentifier, const CIdentifier& idAttributeValue);


		protected:

			IAlgorithmContext& m_rAlgorithmContext;
			CAlgorithmScenarioExporter& m_parent;
		};
	} // namespace
} // namespace OpenViBEToolkit

#define exportAttributesMacro(exporter, attributable, memoryBuffer, AttributableName) \
	do { \
	(exporter).exportAttributes(attributable, memoryBuffer, \
	OVTK_Algorithm_ScenarioExporter_NodeId_##AttributableName##_Attributes, \
	OVTK_Algorithm_ScenarioExporter_NodeId_##AttributableName##_Attribute, \
	OVTK_Algorithm_ScenarioExporter_NodeId_##AttributableName##_Attribute_Identifier, \
	OVTK_Algorithm_ScenarioExporter_NodeId_##AttributableName##_Attribute_Value); \
	} while (0)

void CAlgorithmScenarioExporterHelper::exportAttributes(const IAttributable& attributable, IMemoryBuffer& memoryBuffer, const CIdentifier& idAttributes,
														const CIdentifier& idAttribute, const CIdentifier& idAttributeIdentifier,
														const CIdentifier& idAttributeValue)
{
	if (attributable.getNextAttributeIdentifier(OV_UndefinedIdentifier) != OV_UndefinedIdentifier)
	{
		CIdentifier attributeIdentifier;
		m_parent.exportStart(memoryBuffer, idAttributes);
		while ((attributeIdentifier = attributable.getNextAttributeIdentifier(attributeIdentifier)) != OV_UndefinedIdentifier)
		{
			// do not export attributes that are used only in the designer for bookkeeping
			if ((attributeIdentifier != OV_AttributeId_Box_ToBeUpdated) && (attributeIdentifier != OV_AttributeId_Box_PendingDeprecatedInterfacors))
			{
				m_parent.exportStart(memoryBuffer, idAttribute);
				m_parent.exportIdentifier(memoryBuffer, idAttributeIdentifier, attributeIdentifier);
				m_parent.exportString(memoryBuffer, idAttributeValue, attributable.getAttributeValue(attributeIdentifier));
				m_parent.exportStop(memoryBuffer);
			}
		}
		m_parent.exportStop(memoryBuffer);
	}
}


bool CAlgorithmScenarioExporter::process()
{
	CAlgorithmScenarioExporterHelper l_oHelper(this->getAlgorithmContext(), *this);
	CMemoryBuffer l_oTemporaryMemoryBuffer;

	// preallocates 1 Mbytes
	l_oTemporaryMemoryBuffer.reserve(1024 * 1024);

	TParameterHandler<IScenario*> ip_pScenario(this->getInputParameter(OV_Algorithm_ScenarioExporter_InputParameterId_Scenario));
	IScenario* l_pScenario = ip_pScenario;

	OV_ERROR_UNLESS_KRF(l_pScenario, "Input scenario is NULL", OpenViBE::Kernel::ErrorType::BadInput);

	TParameterHandler<IMemoryBuffer*> op_pMemoryBuffer(this->getOutputParameter(OV_Algorithm_ScenarioExporter_OutputParameterId_MemoryBuffer));
	IMemoryBuffer* l_pMemoryBuffer = op_pMemoryBuffer;

	OV_ERROR_UNLESS_KRF(l_pMemoryBuffer, "Output memory buffer is NULL", OpenViBE::Kernel::ErrorType::BadOutput);

	this->exportStart(l_oTemporaryMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_OpenViBEScenario);
	this->exportString(l_oTemporaryMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_FormatVersion, CString("2"));
	this->exportString(l_oTemporaryMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Creator, this->getConfigurationManager().expand("${Application_Name}"));
	this->exportString(l_oTemporaryMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_CreatorVersion,
					   this->getConfigurationManager().expand("${Application_Version}"));
	this->exportStart(l_oTemporaryMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Settings);
	for (uint32_t l_ui32SettingIndex = 0; l_ui32SettingIndex < l_pScenario->getSettingCount(); l_ui32SettingIndex++)
	{
		l_oHelper.exportSetting(l_oTemporaryMemoryBuffer, *l_pScenario, l_ui32SettingIndex);
	}
	this->exportStop(l_oTemporaryMemoryBuffer);

	this->exportStart(l_oTemporaryMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Inputs);
	for (uint32_t l_ui32InputIndex = 0; l_ui32InputIndex < l_pScenario->getInputCount(); l_ui32InputIndex++)
	{
		l_oHelper.exportInput(l_oTemporaryMemoryBuffer, *l_pScenario, l_ui32InputIndex);
	}
	this->exportStop(l_oTemporaryMemoryBuffer);

	this->exportStart(l_oTemporaryMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Outputs);
	for (uint32_t l_ui32OutputIndex = 0; l_ui32OutputIndex < l_pScenario->getOutputCount(); l_ui32OutputIndex++)
	{
		l_oHelper.exportOutput(l_oTemporaryMemoryBuffer, *l_pScenario, l_ui32OutputIndex);
	}
	this->exportStop(l_oTemporaryMemoryBuffer);

	this->exportStart(l_oTemporaryMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Boxes);
	{
		CIdentifier* listID = nullptr;
		size_t nbElems      = 0;
		l_pScenario->getBoxIdentifierList(&listID, &nbElems);
		for (size_t i = 0; i < nbElems; ++i) { l_oHelper.exportBox(l_oTemporaryMemoryBuffer, *l_pScenario->getBoxDetails(listID[i])); }
		l_pScenario->releaseIdentifierList(listID);
	}
	this->exportStop(l_oTemporaryMemoryBuffer);

	this->exportStart(l_oTemporaryMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Links);
	{
		CIdentifier* listID = nullptr;
		size_t nbElems      = 0;
		l_pScenario->getLinkIdentifierList(&listID, &nbElems);
		for (size_t i = 0; i < nbElems; ++i)
		{
			const ILink* l_pLink = l_pScenario->getLinkDetails(listID[i]);
			// do not export invalid links
			if (!l_pLink->hasAttribute(OV_AttributeId_Link_Invalid)) { l_oHelper.exportLink(l_oTemporaryMemoryBuffer, *l_pLink); }
		}
		l_pScenario->releaseIdentifierList(listID);
	}
	this->exportStop(l_oTemporaryMemoryBuffer);

	this->exportStart(l_oTemporaryMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Comments);
	{
		CIdentifier* listID = nullptr;
		size_t nbElems      = 0;
		l_pScenario->getCommentIdentifierList(&listID, &nbElems);
		for (size_t i = 0; i < nbElems; ++i) { l_oHelper.exportComment(l_oTemporaryMemoryBuffer, *l_pScenario->getCommentDetails(listID[i])); }
		l_pScenario->releaseIdentifierList(listID);
	}
	this->exportStop(l_oTemporaryMemoryBuffer);

	this->exportStart(l_oTemporaryMemoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Metadata);
	{
		CIdentifier* listID = nullptr;
		size_t nbElems      = 0;
		l_pScenario->getMetadataIdentifierList(&listID, &nbElems);
		for (size_t i = 0; i < nbElems; ++i) { l_oHelper.exportMetadata(l_oTemporaryMemoryBuffer, *l_pScenario->getMetadataDetails(listID[i])); }
		l_pScenario->releaseIdentifierList(listID);
	}
	this->exportStop(l_oTemporaryMemoryBuffer);

	exportAttributesMacro(l_oHelper, *l_pScenario, l_oTemporaryMemoryBuffer, Scenario);

	this->exportStop(l_oTemporaryMemoryBuffer);

	l_pMemoryBuffer->setSize(0, true);
	l_pMemoryBuffer->append(l_oTemporaryMemoryBuffer);

	return true;
}

CAlgorithmScenarioExporterHelper::CAlgorithmScenarioExporterHelper(IAlgorithmContext& context, CAlgorithmScenarioExporter& parent)
	: m_rAlgorithmContext(context)
	  , m_parent(parent) {}

bool CAlgorithmScenarioExporterHelper::exportBox(IMemoryBuffer& memoryBuffer, const IBox& box)
{
	m_parent.exportStart(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box);
	m_parent.exportIdentifier(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Identifier, box.getIdentifier());
	m_parent.exportString(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Name, box.getName());
	m_parent.exportIdentifier(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_AlgorithmClassIdentifier, box.getAlgorithmClassIdentifier());

	if (box.getInputCount() != 0)
	{
		m_parent.exportStart(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Inputs);
		for (uint32_t i = 0; i < box.getInputCount(); i++)
		{
			CIdentifier inputID;
			CIdentifier inputTypeID;
			CString inputName;
			box.getInputType(i, inputTypeID);
			box.getInputName(i, inputName);
			box.getInterfacorIdentifier(Input, i, inputID);

			m_parent.exportStart(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input);
			if (inputID != OV_UndefinedIdentifier)
			{
				m_parent.exportIdentifier(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_Identifier, inputID);
			}
			m_parent.exportIdentifier(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_TypeIdentifier, inputTypeID);
			m_parent.exportString(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_Name, inputName);
			m_parent.exportStop(memoryBuffer);
		}
		m_parent.exportStop(memoryBuffer);
	}

	if (box.getOutputCount() != 0)
	{
		m_parent.exportStart(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Outputs);
		for (uint32_t i = 0; i < box.getOutputCount(); i++)
		{
			CIdentifier outputID;
			CIdentifier outputTypeID;
			CString outputName;
			box.getOutputType(i, outputTypeID);
			box.getOutputName(i, outputName);
			box.getInterfacorIdentifier(Output, i, outputID);

			m_parent.exportStart(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output);
			if (outputID != OV_UndefinedIdentifier)
			{
				m_parent.exportIdentifier(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_Identifier, outputID);
			}
			m_parent.exportIdentifier(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_TypeIdentifier, outputTypeID);
			m_parent.exportString(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_Name, outputName);
			m_parent.exportStop(memoryBuffer);
		}
		m_parent.exportStop(memoryBuffer);
	}

	if (box.getSettingCount() != 0)
	{
		m_parent.exportStart(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Settings);
		for (uint32_t i = 0; i < box.getSettingCount(); i++)
		{
			CIdentifier l_oSettingIdentifier;
			CIdentifier l_oSettingTypeIdentifier;
			CString l_sSettingName;
			CString l_sDefaultValue;
			CString l_sValue;
			bool l_bModifiability;
			box.getInterfacorIdentifier(Setting, i, l_oSettingIdentifier);
			box.getSettingType(i, l_oSettingTypeIdentifier);
			box.getSettingName(i, l_sSettingName);
			box.getSettingDefaultValue(i, l_sDefaultValue);
			box.getSettingValue(i, l_sValue);
			box.getSettingMod(i, l_bModifiability);
			CString l_sModifiability;
			if (l_bModifiability) l_sModifiability = CString("true");
			else l_sModifiability                  = CString("false");
			m_parent.exportStart(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting);
			if (l_oSettingIdentifier != OV_UndefinedIdentifier) // do not export identifier if not defined
			{
				m_parent.exportIdentifier(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Identifier, l_oSettingIdentifier);
			}
			m_parent.exportIdentifier(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_TypeIdentifier, l_oSettingTypeIdentifier);
			m_parent.exportString(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Name, l_sSettingName);
			m_parent.exportString(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_DefaultValue, l_sDefaultValue);
			m_parent.exportString(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Value, l_sValue);
			m_parent.exportString(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Modifiability, l_sModifiability);
			m_parent.exportStop(memoryBuffer);
		}
		m_parent.exportStop(memoryBuffer);
	}

	exportAttributesMacro((*this), box, memoryBuffer, Box);

	m_parent.exportStop(memoryBuffer);

	return true;
}

bool CAlgorithmScenarioExporterHelper::exportComment(IMemoryBuffer& memoryBuffer, const IComment& rComment)
{
	m_parent.exportStart(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Comment);
	m_parent.exportIdentifier(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Identifier, rComment.getIdentifier());
	m_parent.exportString(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Text, rComment.getText());

	exportAttributesMacro((*this), rComment, memoryBuffer, Comment);

	m_parent.exportStop(memoryBuffer);

	return true;
}

bool CAlgorithmScenarioExporterHelper::exportMetadata(IMemoryBuffer& memoryBuffer, const IMetadata& metadata)
{
	m_parent.exportStart(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry);
	m_parent.exportIdentifier(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Identifier, metadata.getIdentifier());
	m_parent.exportIdentifier(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Type, metadata.getType());
	m_parent.exportString(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Data, metadata.getData());

	m_parent.exportStop(memoryBuffer);

	return true;
}

bool CAlgorithmScenarioExporterHelper::exportLink(IMemoryBuffer& memoryBuffer, const ILink& rLink)
{
	CIdentifier l_oSourceBoxIdentifier;
	CIdentifier l_oTargetBoxIdentifier;
	uint32_t l_ui32SourceBoxOutputIndex = OV_Value_UndefinedIndexUInt;
	uint32_t l_ui32TargetBoxInputIndex  = OV_Value_UndefinedIndexUInt;
	CIdentifier l_oSourceBoxOutputIdentifier;
	CIdentifier l_oTargetBoxInputIdentifier;

	rLink.getSource(l_oSourceBoxIdentifier, l_ui32SourceBoxOutputIndex, l_oSourceBoxOutputIdentifier);
	rLink.getTarget(l_oTargetBoxIdentifier, l_ui32TargetBoxInputIndex, l_oTargetBoxInputIdentifier);

	m_parent.exportStart(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link);
	m_parent.exportIdentifier(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Identifier, rLink.getIdentifier());
	m_parent.exportStart(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source);
	m_parent.exportIdentifier(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxIdentifier, l_oSourceBoxIdentifier);
	if (l_oSourceBoxOutputIdentifier != OV_UndefinedIdentifier)
	{
		m_parent.exportIdentifier(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxOutputIdentifier, l_oSourceBoxOutputIdentifier);
	}
	else { m_parent.exportUInteger(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxOutputIndex, l_ui32SourceBoxOutputIndex); }
	m_parent.exportStop(memoryBuffer);
	m_parent.exportStart(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target);
	m_parent.exportIdentifier(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxIdentifier, l_oTargetBoxIdentifier);
	if (l_oTargetBoxInputIdentifier != OV_UndefinedIdentifier)
	{
		m_parent.exportIdentifier(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxInputIdentifier, l_oTargetBoxInputIdentifier);
	}
	else { m_parent.exportUInteger(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxInputIndex, l_ui32TargetBoxInputIndex); }
	m_parent.exportStop(memoryBuffer);

	exportAttributesMacro((*this), rLink, memoryBuffer, Link);

	m_parent.exportStop(memoryBuffer);

	return true;
}

bool CAlgorithmScenarioExporterHelper::exportSetting(IMemoryBuffer& memoryBuffer, const IScenario& scenario, uint32_t ui32SettingIndex)
{
	CIdentifier l_oSettingIdentifier;
	CIdentifier l_oSettingTypeIdentifier;
	CString l_sSettingName;
	CString l_sDefaultValue;
	CString l_sValue;


	scenario.getSettingName(ui32SettingIndex, l_sSettingName);
	scenario.getInterfacorIdentifier(Setting, ui32SettingIndex, l_oSettingIdentifier);
	scenario.getSettingType(ui32SettingIndex, l_oSettingTypeIdentifier);
	scenario.getSettingValue(ui32SettingIndex, l_sValue);
	scenario.getSettingDefaultValue(ui32SettingIndex, l_sDefaultValue);

	m_parent.exportStart(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting);
	m_parent.exportIdentifier(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Identifier, l_oSettingIdentifier);
	m_parent.exportIdentifier(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_TypeIdentifier, l_oSettingTypeIdentifier);
	m_parent.exportString(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Name, l_sSettingName);
	m_parent.exportString(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_DefaultValue, l_sDefaultValue);
	m_parent.exportString(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Value, l_sValue);
	m_parent.exportStop(memoryBuffer);

	return true;
}

bool CAlgorithmScenarioExporterHelper::exportInput(IMemoryBuffer& memoryBuffer, const IScenario& scenario, uint32_t index)
{
	CIdentifier inputID;
	CIdentifier inputTypeID;
	CString inputName;
	CIdentifier linkedBoxID;
	uint32_t linkedBoxInputIdx;
	CIdentifier linkedBoxInputID;

	scenario.getInterfacorIdentifier(Input, index, inputID);
	scenario.getInputType(index, inputTypeID);
	scenario.getInputName(index, inputName);
	scenario.getScenarioInputLink(index, linkedBoxID, linkedBoxInputIdx);
	scenario.getScenarioInputLink(index, linkedBoxID, linkedBoxInputID);

	m_parent.exportStart(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input);

	m_parent.exportIdentifier(memoryBuffer,OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_Identifier, inputID);
	m_parent.exportIdentifier(memoryBuffer,OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_TypeIdentifier, inputTypeID);
	m_parent.exportString(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_Name, inputName);
	m_parent.exportIdentifier(memoryBuffer,OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxIdentifier, linkedBoxID);
	if (linkedBoxInputID != OV_UndefinedIdentifier)
	{
		m_parent.exportIdentifier(memoryBuffer,OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxInputIdentifier, linkedBoxInputID);
	}
	else
	{
		m_parent.exportUInteger(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxInputIndex, uint64_t(linkedBoxInputIdx));
	}

	m_parent.exportStop(memoryBuffer);

	return true;
}

bool CAlgorithmScenarioExporterHelper::exportOutput(IMemoryBuffer& memoryBuffer, const IScenario& scenario, uint32_t outputIdx)
{
	CIdentifier outputID;
	CIdentifier outputTypeID;
	CString outputName;
	CIdentifier linkedBoxID;
	uint32_t linkedBoxOutputIdx;
	CIdentifier linkedBoxOutputID;

	scenario.getInterfacorIdentifier(Output, outputIdx, outputID);
	scenario.getOutputType(outputIdx, outputTypeID);
	scenario.getOutputName(outputIdx, outputName);
	scenario.getScenarioOutputLink(outputIdx, linkedBoxID, linkedBoxOutputIdx);
	scenario.getScenarioOutputLink(outputIdx, linkedBoxID, linkedBoxOutputID);

	m_parent.exportStart(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output);

	m_parent.exportIdentifier(memoryBuffer,OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_Identifier, outputID);
	m_parent.exportIdentifier(memoryBuffer,OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_TypeIdentifier, outputTypeID);
	m_parent.exportString(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_Name, outputName);
	m_parent.exportIdentifier(memoryBuffer,OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxIdentifier, linkedBoxID);
	if (linkedBoxOutputID != OV_UndefinedIdentifier)
	{
		m_parent.exportIdentifier(memoryBuffer,OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxOutputIdentifier, linkedBoxOutputID);
	}
	else
	{
		m_parent.exportUInteger(memoryBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxOutputIndex,
								 uint64_t(linkedBoxOutputIdx));
	}

	m_parent.exportStop(memoryBuffer);

	return true;
}
