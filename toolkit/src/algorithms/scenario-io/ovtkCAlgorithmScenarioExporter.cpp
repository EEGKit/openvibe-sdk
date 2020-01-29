#include "ovtkCAlgorithmScenarioExporter.h"
#include <vector>
#include <sstream>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace /*OpenViBE::*/Toolkit;

namespace OpenViBE
{
	namespace Toolkit
	{
	namespace
	{
		class CAlgorithmScenarioExporterHelper
		{
			friend class CAlgorithmScenarioExporter;
		public:

			CAlgorithmScenarioExporterHelper(IAlgorithmContext& context, CAlgorithmScenarioExporter& parent);
			bool exportBox(IMemoryBuffer& buffer, const IBox& box) const;
			bool exportComment(IMemoryBuffer& buffer, const IComment& comment) const;
			bool exportMetadata(IMemoryBuffer& buffer, const IMetadata& metadata) const;
			bool exportSetting(IMemoryBuffer& buffer, const IScenario& scenario, const size_t index) const;
			bool exportInput(IMemoryBuffer& buffer, const IScenario& scenario, size_t index) const;
			bool exportOutput(IMemoryBuffer& buffer, const IScenario& scenario, size_t index) const;
			bool exportLink(IMemoryBuffer& buffer, const ILink& rLink) const;
			void exportAttributes(const IAttributable& attributable, IMemoryBuffer& buffer, const CIdentifier& idAttributes,
								  const CIdentifier& idAttribute, const CIdentifier& idAttributeID, const CIdentifier& idAttributeValue) const;


		protected:

			IAlgorithmContext& m_algorithmContext;
			CAlgorithmScenarioExporter& m_parent;
		};
	} // namespace
	}  // namespace Toolkit
}  // namespace OpenViBE

#define exportAttributesMacro(exporter, attributable, memoryBuffer, AttributableName) \
	do { \
	(exporter).exportAttributes(attributable, memoryBuffer, \
	OVTK_Algorithm_ScenarioExporter_NodeId_##AttributableName##_Attributes, \
	OVTK_Algorithm_ScenarioExporter_NodeId_##AttributableName##_Attribute, \
	OVTK_Algorithm_ScenarioExporter_NodeId_##AttributableName##_Attribute_ID, \
	OVTK_Algorithm_ScenarioExporter_NodeId_##AttributableName##_Attribute_Value); \
	} while (0)

void CAlgorithmScenarioExporterHelper::exportAttributes(const IAttributable& attributable, IMemoryBuffer& buffer, const CIdentifier& idAttributes,
														const CIdentifier& idAttribute, const CIdentifier& idAttributeID,
														const CIdentifier& idAttributeValue) const
{
	if (attributable.getNextAttributeIdentifier(OV_UndefinedIdentifier) != OV_UndefinedIdentifier)
	{
		CIdentifier attributeIdentifier;
		m_parent.exportStart(buffer, idAttributes);
		while ((attributeIdentifier = attributable.getNextAttributeIdentifier(attributeIdentifier)) != OV_UndefinedIdentifier)
		{
			// do not export attributes that are used only in the designer for bookkeeping
			if ((attributeIdentifier != OV_AttributeId_Box_ToBeUpdated) && (attributeIdentifier != OV_AttributeId_Box_PendingDeprecatedInterfacors))
			{
				m_parent.exportStart(buffer, idAttribute);
				m_parent.exportIdentifier(buffer, idAttributeID, attributeIdentifier);
				m_parent.exportString(buffer, idAttributeValue, attributable.getAttributeValue(attributeIdentifier));
				m_parent.exportStop(buffer);
			}
		}
		m_parent.exportStop(buffer);
	}
}


bool CAlgorithmScenarioExporter::process()
{
	const CAlgorithmScenarioExporterHelper helper(this->getAlgorithmContext(), *this);
	CMemoryBuffer tmpBuffer;

	// preallocates 1 Mbytes
	tmpBuffer.reserve(1024 * 1024);

	TParameterHandler<IScenario*> ip_scenario(this->getInputParameter(OV_Algorithm_ScenarioExporter_InputParameterId_Scenario));
	IScenario* scenario = ip_scenario;

	OV_ERROR_UNLESS_KRF(scenario, "Input scenario is NULL", OpenViBE::Kernel::ErrorType::BadInput);

	TParameterHandler<IMemoryBuffer*> op_buffer(this->getOutputParameter(OV_Algorithm_ScenarioExporter_OutputParameterId_MemoryBuffer));
	IMemoryBuffer* buffer = op_buffer;

	OV_ERROR_UNLESS_KRF(buffer, "Output memory buffer is NULL", OpenViBE::Kernel::ErrorType::BadOutput);

	this->exportStart(tmpBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_OpenViBEScenario);
	this->exportString(tmpBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_FormatVersion, CString("2"));
	this->exportString(tmpBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Creator, this->getConfigurationManager().expand("${Application_Name}"));
	this->exportString(tmpBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_CreatorVersion, this->getConfigurationManager().expand("${Application_Version}"));
	this->exportStart(tmpBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Settings);
	for (size_t i = 0; i < scenario->getSettingCount(); ++i) { helper.exportSetting(tmpBuffer, *scenario, i); }
	this->exportStop(tmpBuffer);

	this->exportStart(tmpBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Inputs);
	for (size_t i = 0; i < scenario->getInputCount(); ++i) { helper.exportInput(tmpBuffer, *scenario, i); }
	this->exportStop(tmpBuffer);

	this->exportStart(tmpBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Outputs);
	for (size_t i = 0; i < scenario->getOutputCount(); ++i) { helper.exportOutput(tmpBuffer, *scenario, i); }
	this->exportStop(tmpBuffer);

	this->exportStart(tmpBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Boxes);
	{
		CIdentifier* listID = nullptr;
		size_t nbElems      = 0;
		scenario->getBoxIdentifierList(&listID, &nbElems);
		for (size_t i = 0; i < nbElems; ++i) { helper.exportBox(tmpBuffer, *scenario->getBoxDetails(listID[i])); }
		scenario->releaseIdentifierList(listID);
	}
	this->exportStop(tmpBuffer);

	this->exportStart(tmpBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Links);
	{
		CIdentifier* listID = nullptr;
		size_t nbElems      = 0;
		scenario->getLinkIdentifierList(&listID, &nbElems);
		for (size_t i = 0; i < nbElems; ++i)
		{
			const ILink* link = scenario->getLinkDetails(listID[i]);
			// do not export invalid links
			if (!link->hasAttribute(OV_AttributeId_Link_Invalid)) { helper.exportLink(tmpBuffer, *link); }
		}
		scenario->releaseIdentifierList(listID);
	}
	this->exportStop(tmpBuffer);

	this->exportStart(tmpBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Comments);
	{
		CIdentifier* listID = nullptr;
		size_t nbElems      = 0;
		scenario->getCommentIdentifierList(&listID, &nbElems);
		for (size_t i = 0; i < nbElems; ++i) { helper.exportComment(tmpBuffer, *scenario->getCommentDetails(listID[i])); }
		scenario->releaseIdentifierList(listID);
	}
	this->exportStop(tmpBuffer);

	this->exportStart(tmpBuffer, OVTK_Algorithm_ScenarioExporter_NodeId_Metadata);
	{
		CIdentifier* listID = nullptr;
		size_t nbElems      = 0;
		scenario->getMetadataIdentifierList(&listID, &nbElems);
		for (size_t i = 0; i < nbElems; ++i) { helper.exportMetadata(tmpBuffer, *scenario->getMetadataDetails(listID[i])); }
		scenario->releaseIdentifierList(listID);
	}
	this->exportStop(tmpBuffer);

	exportAttributesMacro(helper, *scenario, tmpBuffer, Scenario);

	this->exportStop(tmpBuffer);

	buffer->setSize(0, true);
	buffer->append(tmpBuffer);

	return true;
}

CAlgorithmScenarioExporterHelper::CAlgorithmScenarioExporterHelper(IAlgorithmContext& context, CAlgorithmScenarioExporter& parent)
	: m_algorithmContext(context), m_parent(parent) {}

bool CAlgorithmScenarioExporterHelper::exportBox(IMemoryBuffer& buffer, const IBox& box) const
{
	m_parent.exportStart(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box);
	m_parent.exportIdentifier(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_ID, box.getIdentifier());
	m_parent.exportString(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Name, box.getName());
	m_parent.exportIdentifier(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_AlgorithmClassIdD, box.getAlgorithmClassIdentifier());

	if (box.getInputCount() != 0)
	{
		m_parent.exportStart(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Inputs);
		for (size_t i = 0; i < box.getInputCount(); ++i)
		{
			CIdentifier inputID;
			CIdentifier inputTypeID;
			CString inputName;
			box.getInputType(i, inputTypeID);
			box.getInputName(i, inputName);
			box.getInterfacorIdentifier(Input, i, inputID);

			m_parent.exportStart(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input);
			if (inputID != OV_UndefinedIdentifier) { m_parent.exportIdentifier(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_ID, inputID); }
			m_parent.exportIdentifier(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_TypeID, inputTypeID);
			m_parent.exportString(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_Name, inputName);
			m_parent.exportStop(buffer);
		}
		m_parent.exportStop(buffer);
	}

	if (box.getOutputCount() != 0)
	{
		m_parent.exportStart(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Outputs);
		for (size_t i = 0; i < box.getOutputCount(); ++i)
		{
			CIdentifier outputID;
			CIdentifier outputTypeID;
			CString outputName;
			box.getOutputType(i, outputTypeID);
			box.getOutputName(i, outputName);
			box.getInterfacorIdentifier(Output, i, outputID);

			m_parent.exportStart(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output);
			if (outputID != OV_UndefinedIdentifier) { m_parent.exportIdentifier(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_ID, outputID); }
			m_parent.exportIdentifier(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_TypeID, outputTypeID);
			m_parent.exportString(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_Name, outputName);
			m_parent.exportStop(buffer);
		}
		m_parent.exportStop(buffer);
	}

	if (box.getSettingCount() != 0)
	{
		m_parent.exportStart(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Settings);
		for (size_t i = 0; i < box.getSettingCount(); ++i)
		{
			CIdentifier settingID;
			CIdentifier settingTypeID;
			CString settingName;
			CString defaultValue;
			CString value;
			bool modifiability;
			box.getInterfacorIdentifier(Setting, i, settingID);
			box.getSettingType(i, settingTypeID);
			box.getSettingName(i, settingName);
			box.getSettingDefaultValue(i, defaultValue);
			box.getSettingValue(i, value);
			box.getSettingMod(i, modifiability);

			CString str;
			if (modifiability) { str = CString("true"); }
			else { str = CString("false"); }

			m_parent.exportStart(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting);
			if (settingID != OV_UndefinedIdentifier) // do not export identifier if not defined
			{
				m_parent.exportIdentifier(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_ID, settingID);
			}
			m_parent.exportIdentifier(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_TypeID, settingTypeID);
			m_parent.exportString(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Name, settingName);
			m_parent.exportString(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_DefaultValue, defaultValue);
			m_parent.exportString(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Value, value);
			m_parent.exportString(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Modifiability, str);
			m_parent.exportStop(buffer);
		}
		m_parent.exportStop(buffer);
	}

	exportAttributesMacro((*this), box, buffer, Box);

	m_parent.exportStop(buffer);

	return true;
}

bool CAlgorithmScenarioExporterHelper::exportComment(IMemoryBuffer& buffer, const IComment& comment) const
{
	m_parent.exportStart(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Comment);
	m_parent.exportIdentifier(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Comment_ID, comment.getIdentifier());
	m_parent.exportString(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Text, comment.getText());

	exportAttributesMacro((*this), comment, buffer, Comment);

	m_parent.exportStop(buffer);

	return true;
}

bool CAlgorithmScenarioExporterHelper::exportMetadata(IMemoryBuffer& buffer, const IMetadata& metadata) const
{
	m_parent.exportStart(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry);
	m_parent.exportIdentifier(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_ID, metadata.getIdentifier());
	m_parent.exportIdentifier(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Type, metadata.getType());
	m_parent.exportString(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Data, metadata.getData());
	m_parent.exportStop(buffer);
	return true;
}

bool CAlgorithmScenarioExporterHelper::exportLink(IMemoryBuffer& buffer, const ILink& rLink) const
{
	CIdentifier srcBoxID;
	CIdentifier dstBoxID;
	size_t srcBoxOutputIdx = size_t(-1);
	size_t dstBoxInputIdx  = size_t(-1);
	CIdentifier srcBoxOutputID;
	CIdentifier dstBoxInputID;

	rLink.getSource(srcBoxID, srcBoxOutputIdx, srcBoxOutputID);
	rLink.getTarget(dstBoxID, dstBoxInputIdx, dstBoxInputID);

	m_parent.exportStart(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link);
	m_parent.exportIdentifier(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_ID, rLink.getIdentifier());
	m_parent.exportStart(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source);
	m_parent.exportIdentifier(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxID, srcBoxID);
	if (srcBoxOutputID != OV_UndefinedIdentifier)
	{
		m_parent.exportIdentifier(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxOutputID, srcBoxOutputID);
	}
	else { m_parent.exportUInteger(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxOutputIdx, srcBoxOutputIdx); }
	m_parent.exportStop(buffer);
	m_parent.exportStart(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target);
	m_parent.exportIdentifier(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxID, dstBoxID);
	if (dstBoxInputID != OV_UndefinedIdentifier)
	{
		m_parent.exportIdentifier(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxInputID, dstBoxInputID);
	}
	else { m_parent.exportUInteger(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxInputIdx, dstBoxInputIdx); }
	m_parent.exportStop(buffer);

	exportAttributesMacro((*this), rLink, buffer, Link);

	m_parent.exportStop(buffer);

	return true;
}

bool CAlgorithmScenarioExporterHelper::exportSetting(IMemoryBuffer& buffer, const IScenario& scenario, const size_t index) const
{
	CIdentifier settingID;
	CIdentifier settingTypeID;
	CString settingName;
	CString defaultValue;
	CString value;


	scenario.getSettingName(index, settingName);
	scenario.getInterfacorIdentifier(Setting, index, settingID);
	scenario.getSettingType(index, settingTypeID);
	scenario.getSettingValue(index, value);
	scenario.getSettingDefaultValue(index, defaultValue);

	m_parent.exportStart(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting);
	m_parent.exportIdentifier(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_ID, settingID);
	m_parent.exportIdentifier(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_TypeID, settingTypeID);
	m_parent.exportString(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Name, settingName);
	m_parent.exportString(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_DefaultValue, defaultValue);
	m_parent.exportString(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Value, value);
	m_parent.exportStop(buffer);

	return true;
}

bool CAlgorithmScenarioExporterHelper::exportInput(IMemoryBuffer& buffer, const IScenario& scenario, const size_t index) const
{
	CIdentifier inputID;
	CIdentifier inputTypeID;
	CString inputName;
	CIdentifier linkedBoxID;
	size_t linkedBoxInputIdx;
	CIdentifier linkedBoxInputID;

	scenario.getInterfacorIdentifier(Input, index, inputID);
	scenario.getInputType(index, inputTypeID);
	scenario.getInputName(index, inputName);
	scenario.getScenarioInputLink(index, linkedBoxID, linkedBoxInputIdx);
	scenario.getScenarioInputLink(index, linkedBoxID, linkedBoxInputID);

	m_parent.exportStart(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input);

	m_parent.exportIdentifier(buffer,OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_ID, inputID);
	m_parent.exportIdentifier(buffer,OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_TypeID, inputTypeID);
	m_parent.exportString(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_Name, inputName);
	m_parent.exportIdentifier(buffer,OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxID, linkedBoxID);
	if (linkedBoxInputID != OV_UndefinedIdentifier)
	{
		m_parent.exportIdentifier(buffer,OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxInputID, linkedBoxInputID);
	}
	else { m_parent.exportUInteger(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxInputIdx, uint64_t(linkedBoxInputIdx)); }

	m_parent.exportStop(buffer);

	return true;
}

bool CAlgorithmScenarioExporterHelper::exportOutput(IMemoryBuffer& buffer, const IScenario& scenario, const size_t index) const
{
	CIdentifier outputID;
	CIdentifier outputTypeID;
	CString outputName;
	CIdentifier linkedBoxID;
	size_t linkedBoxOutputIdx;
	CIdentifier linkedBoxOutputID;

	scenario.getInterfacorIdentifier(Output, index, outputID);
	scenario.getOutputType(index, outputTypeID);
	scenario.getOutputName(index, outputName);
	scenario.getScenarioOutputLink(index, linkedBoxID, linkedBoxOutputIdx);
	scenario.getScenarioOutputLink(index, linkedBoxID, linkedBoxOutputID);

	m_parent.exportStart(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output);

	m_parent.exportIdentifier(buffer,OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_ID, outputID);
	m_parent.exportIdentifier(buffer,OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_TypeID, outputTypeID);
	m_parent.exportString(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_Name, outputName);
	m_parent.exportIdentifier(buffer,OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxID, linkedBoxID);
	if (linkedBoxOutputID != OV_UndefinedIdentifier)
	{
		m_parent.exportIdentifier(buffer,OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxOutputID, linkedBoxOutputID);
	}
	else { m_parent.exportUInteger(buffer, OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxOutputIdx, uint64_t(linkedBoxOutputIdx)); }

	m_parent.exportStop(buffer);

	return true;
}
