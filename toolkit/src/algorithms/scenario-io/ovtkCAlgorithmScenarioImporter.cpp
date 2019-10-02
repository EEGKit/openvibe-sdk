#include "ovtkCAlgorithmScenarioImporter.h"

#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include <string>
#include <algorithm>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEToolkit;

namespace OpenViBEToolkit
{
	namespace
	{
		typedef struct _SScenarioInput
		{
			CIdentifier m_oIdentifier = OV_UndefinedIdentifier;
			CIdentifier m_typeID;
			CString m_sName;
			CIdentifier m_oLinkedBoxIdentifier;
			uint32_t m_ui32LinkedBoxInputIndex      = OV_Value_UndefinedIndexUInt;
			CIdentifier m_oLinkedBoxInputIdentifier = OV_UndefinedIdentifier;
		} SScenarioInput;

		typedef struct _SScenarioOutput
		{
			CIdentifier m_oIdentifier = OV_UndefinedIdentifier;
			CIdentifier m_typeID;
			CString m_sName;
			CIdentifier m_oLinkedBoxIdentifier;
			uint32_t m_ui32LinkedBoxOutputIndex      = OV_Value_UndefinedIndexUInt;
			CIdentifier m_oLinkedBoxOutputIdentifier = OV_UndefinedIdentifier;
		} SScenarioOutput;

		typedef struct _SInput
		{
			CIdentifier m_oIdentifier = OV_UndefinedIdentifier;
			CIdentifier m_typeID;
			CString m_sName;
		} SInput;

		typedef struct _SOutput
		{
			CIdentifier m_oIdentifier = OV_UndefinedIdentifier;
			CIdentifier m_typeID;
			CString m_sName;
		} SOutput;

		typedef struct _SSetting
		{
			CIdentifier m_typeID;
			CString m_sName;
			CString m_sDefaultValue;
			CString m_sValue;
			bool m_bModifiability     = false;
			CIdentifier m_oIdentifier = OV_UndefinedIdentifier;
		} SSetting;

		typedef struct _SAttribute
		{
			CIdentifier m_oIdentifier;
			CString m_sValue;
		} SAttribute;

		typedef struct _SBox
		{
			CIdentifier m_oIdentifier;
			CIdentifier m_oAlgorithmClassIdentifier;
			CString m_sName;
			std::vector<SInput> m_vInput;
			std::vector<SOutput> m_vOutput;
			std::vector<SSetting> m_vSetting;
			std::vector<SAttribute> m_vAttribute;
		} SBox;

		typedef struct _SComment
		{
			CIdentifier m_oIdentifier;
			CString m_sText;
			std::vector<SAttribute> m_vAttribute;
		} SComment;

		typedef struct _SMetadata
		{
			CIdentifier identifier;
			CIdentifier type;
			CString data;
		} SMetadata;

		typedef struct _SLinkSource
		{
			CIdentifier m_oBoxIdentifier;
			uint32_t m_ui32BoxOutputIndex      = OV_Value_UndefinedIndexUInt;
			CIdentifier m_oBoxOutputIdentifier = OV_UndefinedIdentifier;
		} SLinkSource;

		typedef struct _SLinkTarget
		{
			CIdentifier m_oBoxIdentifier;
			uint32_t m_ui32BoxInputIndex      = OV_Value_UndefinedIndexUInt;
			CIdentifier m_oBoxInputIdentifier = OV_UndefinedIdentifier;
		} SLinkTarget;

		typedef struct _SLink
		{
			CIdentifier m_oIdentifier;
			SLinkSource m_oLinkSource;
			SLinkTarget m_oLinkTarget;
			std::vector<SAttribute> m_vAttribute;
		} SLink;

		typedef struct _SScenario
		{
			std::vector<SSetting> m_vSetting;
			std::vector<SScenarioInput> m_vScenarioInput;
			std::vector<SScenarioOutput> m_vScenarioOutput;
			std::vector<SBox> m_vBox;
			std::vector<SComment> m_vComment;
			std::vector<SMetadata> m_metadata;
			std::vector<SLink> m_vLink;
			std::vector<SAttribute> m_vAttribute;
		} SScenario;
	} // namespace

	class CAlgorithmScenarioImporterContext final : public IAlgorithmScenarioImporterContext
	{
	public:

		explicit CAlgorithmScenarioImporterContext(IAlgorithmContext& algorithmCtx) : m_rAlgorithmContext(algorithmCtx) { }

		bool processStart(const CIdentifier& identifier) override;
		bool processIdentifier(const CIdentifier& identifier, const CIdentifier& value) override;
		bool processString(const CIdentifier& identifier, const CString& value) override;
		bool processUInteger(const CIdentifier& identifier, uint64_t value) override;
		bool processStop() override;

		_IsDerivedFromClass_Final_(IAlgorithmScenarioImporterContext, OV_UndefinedIdentifier)

		IAlgorithmContext& m_rAlgorithmContext;
		SScenario m_oSymbolicScenario;
	};
} // namespace OpenViBEToolkit

bool CAlgorithmScenarioImporter::process()
{
	TParameterHandler<IScenario*> op_pScenario(this->getOutputParameter(OV_Algorithm_ScenarioImporter_OutputParameterId_Scenario));
	IScenario* l_pScenario = op_pScenario;

	OV_ERROR_UNLESS_KRF(l_pScenario, "Output scenario is NULL", OpenViBE::Kernel::ErrorType::BadOutput);

	TParameterHandler<IMemoryBuffer*> ip_pMemoryBuffer(this->getInputParameter(OV_Algorithm_ScenarioImporter_InputParameterId_MemoryBuffer));
	IMemoryBuffer* l_pMemoryBuffer = ip_pMemoryBuffer;

	OV_ERROR_UNLESS_KRF(l_pMemoryBuffer, "Input memory buffer is NULL", OpenViBE::Kernel::ErrorType::BadInput);

	std::map<CIdentifier, CIdentifier> l_vBoxIdMapping;

	CAlgorithmScenarioImporterContext l_oContext(this->getAlgorithmContext());

	OV_ERROR_UNLESS_KRF(this->import(l_oContext, *l_pMemoryBuffer), "Import failed", OpenViBE::Kernel::ErrorType::Internal);

	SScenario& l_rSymbolicScenario = l_oContext.m_oSymbolicScenario;

	// Now build the scenario according to what has been loaded

	for (auto s = l_rSymbolicScenario.m_vSetting.begin(); s != l_rSymbolicScenario.m_vSetting.end(); ++s)
	{
		CIdentifier settingID = s->m_oIdentifier;
		// compute identifier only if it does not exists
		if (settingID == OV_UndefinedIdentifier) { settingID = l_pScenario->getUnusedSettingIdentifier(); }
		l_pScenario->addSetting(s->m_sName, s->m_typeID, s->m_sDefaultValue, OV_Value_UndefinedIndexUInt, false, settingID);
		l_pScenario->setSettingValue(l_pScenario->getSettingCount() - 1, s->m_sValue);
	}


	for (auto b = l_rSymbolicScenario.m_vBox.begin(); b != l_rSymbolicScenario.m_vBox.end(); ++b)
	{
		IBox* l_pBox = nullptr;
		CIdentifier l_oNewBoxID;

		l_pScenario->addBox(l_oNewBoxID, b->m_oIdentifier);
		l_pBox = l_pScenario->getBoxDetails(l_oNewBoxID);
		if (l_pBox)
		{
			l_pBox->setName(b->m_sName);

			for (auto i = b->m_vInput.begin(); i != b->m_vInput.end(); ++i) { l_pBox->addInput(i->m_sName, i->m_typeID, i->m_oIdentifier); }

			for (auto o = b->m_vOutput.begin(); o != b->m_vOutput.end(); ++o) { l_pBox->addOutput(o->m_sName, o->m_typeID, o->m_oIdentifier); }
			for (auto s = b->m_vSetting.begin(); s != b->m_vSetting.end(); ++s)
			{
				const CIdentifier& l_oType = s->m_typeID;
				if (!this->getTypeManager().isRegistered(l_oType) && !(this->getTypeManager().isEnumeration(l_oType)) && (!this
																														   ->getTypeManager().isBitMask(l_oType)
					))
				{
					if (this->getConfigurationManager().expandAsBoolean("${Kernel_AbortScenarioImportOnUnknownSetting}", true))
					{
						OV_ERROR_KRF(
							"The type of the setting " << s->m_sName <<" (" << l_oType.toString() << ") from box " << b->m_sName << " cannot be recognized.",
							OpenViBE::Kernel::ErrorType::BadSetting);
					}
					OV_WARNING_K(
						"The type of the setting " << s->m_sName <<" (" << l_oType.toString() << ") from box " << b->m_sName << " cannot be recognized.");
				}

				l_pBox->addSetting(s->m_sName, s->m_typeID, s->m_sDefaultValue, OV_Value_UndefinedIndexUInt, s->m_bModifiability, s->m_oIdentifier);
				l_pBox->setSettingValue(l_pBox->getSettingCount() - 1, s->m_sValue);
			}
			for (auto a = b->m_vAttribute.begin(); a != b->m_vAttribute.end(); ++a) { l_pBox->addAttribute(a->m_oIdentifier, a->m_sValue); }

			// it is important to set box algorithm at
			// last so the box listener is never called
			l_pBox->setAlgorithmClassIdentifier(b->m_oAlgorithmClassIdentifier);
		}
		l_vBoxIdMapping[b->m_oIdentifier] = l_oNewBoxID;
	}

	for (auto c = l_rSymbolicScenario.m_vComment.begin(); c != l_rSymbolicScenario.m_vComment.end(); ++c)
	{
		IComment* l_pComment = nullptr;
		CIdentifier l_oNewCommentID;

		l_pScenario->addComment(l_oNewCommentID, c->m_oIdentifier);
		l_pComment = l_pScenario->getCommentDetails(l_oNewCommentID);
		if (l_pComment)
		{
			l_pComment->setText(c->m_sText);

			for (auto a = c->m_vAttribute.begin(); a != c->m_vAttribute.end(); ++a) { l_pComment->addAttribute(a->m_oIdentifier, a->m_sValue); }
		}
	}

	for (auto& symbolicMetadata : l_rSymbolicScenario.m_metadata)
	{
		CIdentifier newMetadataIdentifier;
		l_pScenario->addMetadata(newMetadataIdentifier, symbolicMetadata.identifier);
		IMetadata* metadata = l_pScenario->getMetadataDetails(newMetadataIdentifier);
		if (metadata)
		{
			metadata->setType(symbolicMetadata.type);
			metadata->setData(symbolicMetadata.data);
		}
	}

	for (auto l = l_rSymbolicScenario.m_vLink.begin(); l != l_rSymbolicScenario.m_vLink.end(); ++l)
	{
		ILink* link = nullptr;
		CIdentifier l_oNewLinkID;

		uint32_t srcBoxOutputIdx = l->m_oLinkSource.m_ui32BoxOutputIndex;
		uint32_t dstBoxInputIdx  = l->m_oLinkTarget.m_ui32BoxInputIndex;

		CIdentifier srcBoxOutputID = l->m_oLinkSource.m_oBoxOutputIdentifier;
		CIdentifier dstBoxInputID  = l->m_oLinkTarget.m_oBoxInputIdentifier;

		if (srcBoxOutputID != OV_UndefinedIdentifier)
		{
			l_pScenario->getSourceBoxOutputIndex(l_vBoxIdMapping[l->m_oLinkSource.m_oBoxIdentifier], srcBoxOutputID, srcBoxOutputIdx);
		}

		OV_ERROR_UNLESS_KRF(srcBoxOutputIdx != OV_Value_UndefinedIndexUInt, "Output index of the source box could not be found",
							OpenViBE::Kernel::ErrorType::BadOutput);

		if (dstBoxInputID != OV_UndefinedIdentifier)
		{
			l_pScenario->getTargetBoxInputIndex(l_vBoxIdMapping[l->m_oLinkTarget.m_oBoxIdentifier], dstBoxInputID, dstBoxInputIdx);
		}

		OV_ERROR_UNLESS_KRF(dstBoxInputIdx != OV_Value_UndefinedIndexUInt, "Input index of the target box could not be found",
							OpenViBE::Kernel::ErrorType::BadOutput);

		l_pScenario->connect(l_oNewLinkID, l_vBoxIdMapping[l->m_oLinkSource.m_oBoxIdentifier], srcBoxOutputIdx,
							 l_vBoxIdMapping[l->m_oLinkTarget.m_oBoxIdentifier], dstBoxInputIdx, l->m_oIdentifier);

		link = l_pScenario->getLinkDetails(l_oNewLinkID);
		if (link) { for (auto a = l->m_vAttribute.begin(); a != l->m_vAttribute.end(); ++a) { link->addAttribute(a->m_oIdentifier, a->m_sValue); } }
	}

	uint32_t l_ui32ScenarioInputIdx = 0;
	for (auto symbolicScenarioInput : l_rSymbolicScenario.m_vScenarioInput)
	{
		CIdentifier l_oScenarioInputID = symbolicScenarioInput.m_oIdentifier;
		// compute identifier only if it does not exists
		if (l_oScenarioInputID == OV_UndefinedIdentifier) { l_oScenarioInputID = l_pScenario->getUnusedInputIdentifier(); }
		l_pScenario->addInput(symbolicScenarioInput.m_sName, symbolicScenarioInput.m_typeID, l_oScenarioInputID);
		if (symbolicScenarioInput.m_oLinkedBoxIdentifier != OV_UndefinedIdentifier)
		{
			// Only try to set scenario output links from boxes that actually exist
			// This enables the usage of header-only importers
			if (l_rSymbolicScenario.m_vBox.end() != std::find_if(l_rSymbolicScenario.m_vBox.begin(), l_rSymbolicScenario.m_vBox.end(),
																 [&symbolicScenarioInput](SBox box)
																 {
																	 return box.m_oIdentifier == symbolicScenarioInput.m_oLinkedBoxIdentifier;
																 }))
			{
				CIdentifier linkedBoxInputIdentifier = symbolicScenarioInput.m_oLinkedBoxInputIdentifier;
				uint32_t linkedBoxInputIndex     = symbolicScenarioInput.m_ui32LinkedBoxInputIndex;

				if (linkedBoxInputIdentifier != OV_UndefinedIdentifier)
				{
					l_pScenario->getTargetBoxInputIndex(symbolicScenarioInput.m_oLinkedBoxIdentifier, linkedBoxInputIdentifier, linkedBoxInputIndex);
				}

				OV_ERROR_UNLESS_KRF(linkedBoxInputIndex != OV_Value_UndefinedIndexUInt, "Input index of the target box could not be found",
									OpenViBE::Kernel::ErrorType::BadOutput);

				l_pScenario->setScenarioInputLink(l_ui32ScenarioInputIdx, symbolicScenarioInput.m_oLinkedBoxIdentifier, linkedBoxInputIndex);
			}
		}
		l_ui32ScenarioInputIdx++;
	}

	uint32_t l_ui32ScenarioOutputIdx = 0;
	for (auto symbolicScenarioOutput : l_rSymbolicScenario.m_vScenarioOutput)
	{
		CIdentifier l_oScenarioOutputID = symbolicScenarioOutput.m_oIdentifier;
		// compute identifier only if it does not exists
		if (l_oScenarioOutputID == OV_UndefinedIdentifier) { l_oScenarioOutputID = l_pScenario->getUnusedOutputIdentifier(); }
		l_pScenario->addOutput(symbolicScenarioOutput.m_sName, symbolicScenarioOutput.m_typeID, l_oScenarioOutputID);
		if (symbolicScenarioOutput.m_oLinkedBoxIdentifier != OV_UndefinedIdentifier)
		{
			// Only try to set scenario output links from boxes that actually exist
			// This enables the usage of header-only importers
			if (std::any_of(l_rSymbolicScenario.m_vBox.begin(), l_rSymbolicScenario.m_vBox.end(), [&symbolicScenarioOutput](SBox box)
			{
				return box.m_oIdentifier == symbolicScenarioOutput.m_oLinkedBoxIdentifier;
			}))
			{
				CIdentifier linkedBoxOutputIdentifier = symbolicScenarioOutput.m_oLinkedBoxOutputIdentifier;
				uint32_t linkedBoxOutputIndex     = symbolicScenarioOutput.m_ui32LinkedBoxOutputIndex;

				if (linkedBoxOutputIdentifier != OV_UndefinedIdentifier)
				{
					l_pScenario->getSourceBoxOutputIndex(symbolicScenarioOutput.m_oLinkedBoxIdentifier, linkedBoxOutputIdentifier, linkedBoxOutputIndex);
				}

				OV_ERROR_UNLESS_KRF(linkedBoxOutputIndex != OV_Value_UndefinedIndexUInt, "Output index of the target box could not be found",
									OpenViBE::Kernel::ErrorType::BadOutput);
				l_pScenario->setScenarioOutputLink(l_ui32ScenarioOutputIdx, symbolicScenarioOutput.m_oLinkedBoxIdentifier, linkedBoxOutputIndex);
			}
		}
		l_ui32ScenarioOutputIdx++;
	}

	for (auto a = l_rSymbolicScenario.m_vAttribute.begin(); a != l_rSymbolicScenario.m_vAttribute.end(); ++a)
	{
		l_pScenario->addAttribute(a->m_oIdentifier, a->m_sValue);
	}

	if (l_pScenario->checkOutdatedBoxes())
	{
		CIdentifier* listID = nullptr;
		size_t nbElems      = 0;
		l_pScenario->getOutdatedBoxIdentifierList(&listID, &nbElems);
		for (size_t i = 0; i < nbElems; ++i)
		{
			const IBox* l_pBox = l_pScenario->getBoxDetails(listID[i]);
			OV_WARNING_K("Box " << l_pBox->getName() << " [" << l_pBox->getAlgorithmClassIdentifier().toString() << "] should be updated");
		}
		l_pScenario->releaseIdentifierList(listID);
	}

	return true;
}

bool CAlgorithmScenarioImporterContext::processStart(const CIdentifier& identifier)
{
	if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_OpenViBEScenario) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Settings) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting) { m_oSymbolicScenario.m_vSetting.push_back(SSetting()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Inputs) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input) { m_oSymbolicScenario.m_vScenarioInput.push_back(SScenarioInput()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Outputs) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output) { m_oSymbolicScenario.m_vScenarioOutput.push_back(SScenarioOutput()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Creator) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_CreatorVersion) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attributes) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute) { m_oSymbolicScenario.m_vAttribute.push_back(SAttribute()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attributes) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute) { m_oSymbolicScenario.m_vBox.back().m_vAttribute.push_back(SAttribute()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attributes) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute) { m_oSymbolicScenario.m_vLink.back().m_vAttribute.push_back(SAttribute()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Boxes) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box) { m_oSymbolicScenario.m_vBox.push_back(SBox()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comments) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment) { m_oSymbolicScenario.m_vComment.push_back(SComment()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attributes) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute)
	{
		m_oSymbolicScenario.m_vComment.back().m_vAttribute.push_back(SAttribute());
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Metadata) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry) { m_oSymbolicScenario.m_metadata.push_back(SMetadata()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Links) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link) { m_oSymbolicScenario.m_vLink.push_back(SLink()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Inputs) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input) { m_oSymbolicScenario.m_vBox.back().m_vInput.push_back(SInput()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Outputs) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output) { m_oSymbolicScenario.m_vBox.back().m_vOutput.push_back(SOutput()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Settings) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting) { m_oSymbolicScenario.m_vBox.back().m_vSetting.push_back(SSetting()); }
		//
	else
	{
		OV_ERROR("(start) Unexpected node identifier " << identifier.toString(), OpenViBE::Kernel::ErrorType::BadArgument, false,
				 m_rAlgorithmContext.getErrorManager(), m_rAlgorithmContext.getLogManager());
	}
	return true;
}

bool CAlgorithmScenarioImporterContext::processIdentifier(const CIdentifier& identifier, const CIdentifier& value)
{
	if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Identifier) { m_oSymbolicScenario.m_vSetting.back().m_oIdentifier = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_TypeIdentifier)
	{
		m_oSymbolicScenario.m_vSetting.back().m_typeID = value;
	}

	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_Identifier)
	{
		m_oSymbolicScenario.m_vScenarioInput.back().m_oIdentifier = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_TypeIdentifier)
	{
		m_oSymbolicScenario.m_vScenarioInput.back().m_typeID = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxIdentifier)
	{
		m_oSymbolicScenario.m_vScenarioInput.back().m_oLinkedBoxIdentifier = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxInputIdentifier)
	{
		m_oSymbolicScenario.m_vScenarioInput.back().m_oLinkedBoxInputIdentifier = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_Identifier)
	{
		m_oSymbolicScenario.m_vScenarioOutput.back().m_oIdentifier = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_TypeIdentifier)
	{
		m_oSymbolicScenario.m_vScenarioOutput.back().m_typeID = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxIdentifier)
	{
		m_oSymbolicScenario.m_vScenarioOutput.back().m_oLinkedBoxIdentifier = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxOutputIdentifier)
	{
		m_oSymbolicScenario.m_vScenarioOutput.back().m_oLinkedBoxOutputIdentifier = value;
	}

	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute_Identifier)
	{
		m_oSymbolicScenario.m_vBox.back().m_vAttribute.back().m_oIdentifier = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Identifier) { m_oSymbolicScenario.m_vBox.back().m_oIdentifier = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_AlgorithmClassIdentifier)
	{
		m_oSymbolicScenario.m_vBox.back().m_oAlgorithmClassIdentifier = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_Identifier)
	{
		m_oSymbolicScenario.m_vBox.back().m_vInput.back().m_oIdentifier = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_TypeIdentifier)
	{
		m_oSymbolicScenario.m_vBox.back().m_vInput.back().m_typeID = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_Identifier)
	{
		m_oSymbolicScenario.m_vBox.back().m_vOutput.back().m_oIdentifier = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_TypeIdentifier)
	{
		m_oSymbolicScenario.m_vBox.back().m_vOutput.back().m_typeID = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Identifier)
	{
		m_oSymbolicScenario.m_vBox.back().m_vSetting.back().m_oIdentifier = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_TypeIdentifier)
	{
		m_oSymbolicScenario.m_vBox.back().m_vSetting.back().m_typeID = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Identifier) { m_oSymbolicScenario.m_vComment.back().m_oIdentifier = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute_Identifier)
	{
		m_oSymbolicScenario.m_vComment.back().m_vAttribute.back().m_oIdentifier = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Identifier) { m_oSymbolicScenario.m_metadata.back().identifier = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Type) { m_oSymbolicScenario.m_metadata.back().type = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute_Identifier)
	{
		m_oSymbolicScenario.m_vLink.back().m_vAttribute.back().m_oIdentifier = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Identifier) { m_oSymbolicScenario.m_vLink.back().m_oIdentifier = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxIdentifier)
	{
		m_oSymbolicScenario.m_vLink.back().m_oLinkSource.m_oBoxIdentifier = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxOutputIdentifier)
	{
		m_oSymbolicScenario.m_vLink.back().m_oLinkSource.m_oBoxOutputIdentifier = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxIdentifier)
	{
		m_oSymbolicScenario.m_vLink.back().m_oLinkTarget.m_oBoxIdentifier = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxInputIdentifier)
	{
		m_oSymbolicScenario.m_vLink.back().m_oLinkTarget.m_oBoxInputIdentifier = value;
	}


	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute_Identifier)
	{
		m_oSymbolicScenario.m_vAttribute.back().m_oIdentifier = value;
	}
	else
	{
		OV_ERROR("(id) Unexpected node identifier " << identifier.toString(),
				 OpenViBE::Kernel::ErrorType::BadArgument, false, m_rAlgorithmContext.getErrorManager(), m_rAlgorithmContext.getLogManager());
	}
	return true;
}

bool CAlgorithmScenarioImporterContext::processString(const CIdentifier& identifier, const CString& value)
{
	if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute_Value) { m_oSymbolicScenario.m_vBox.back().m_vAttribute.back().m_sValue = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Name) { m_oSymbolicScenario.m_vBox.back().m_sName = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_Name) { m_oSymbolicScenario.m_vBox.back().m_vInput.back().m_sName = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_Name) { m_oSymbolicScenario.m_vBox.back().m_vOutput.back().m_sName = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Name) { m_oSymbolicScenario.m_vBox.back().m_vSetting.back().m_sName = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_DefaultValue)
	{
		m_oSymbolicScenario.m_vBox.back().m_vSetting.back().m_sDefaultValue = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Value) { m_oSymbolicScenario.m_vBox.back().m_vSetting.back().m_sValue = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Modifiability)
	{
		m_oSymbolicScenario.m_vBox.back().m_vSetting.back().m_bModifiability = (value == CString("true")) ? true : false;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Text) { m_oSymbolicScenario.m_vComment.back().m_sText = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute_Value)
	{
		m_oSymbolicScenario.m_vComment.back().m_vAttribute.back().m_sValue = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Data) { m_oSymbolicScenario.m_metadata.back().data = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute_Value)
	{
		m_oSymbolicScenario.m_vLink.back().m_vAttribute.back().m_sValue = value;
	}

	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute_Value) { m_oSymbolicScenario.m_vAttribute.back().m_sValue = value; }

	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Name) { m_oSymbolicScenario.m_vSetting.back().m_sName = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_DefaultValue)
	{
		m_oSymbolicScenario.m_vSetting.back().m_sDefaultValue = value;
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Value) { m_oSymbolicScenario.m_vSetting.back().m_sValue = value; }

	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_Name) { m_oSymbolicScenario.m_vScenarioInput.back().m_sName = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_Name) { m_oSymbolicScenario.m_vScenarioOutput.back().m_sName = value; }

	else
	{
		OV_ERROR("(string) Unexpected node identifier " << identifier.toString(), OpenViBE::Kernel::ErrorType::BadArgument,
				 false, m_rAlgorithmContext.getErrorManager(), m_rAlgorithmContext.getLogManager());
	}
	return true;
}

bool CAlgorithmScenarioImporterContext::processUInteger(const CIdentifier& identifier, const uint64_t value)
{
	if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxOutputIndex)
	{
		m_oSymbolicScenario.m_vLink.back().m_oLinkSource.m_ui32BoxOutputIndex = uint32_t(value);
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxInputIndex)
	{
		m_oSymbolicScenario.m_vLink.back().m_oLinkTarget.m_ui32BoxInputIndex = uint32_t(value);
	}

	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxInputIndex)
	{
		m_oSymbolicScenario.m_vScenarioInput.back().m_ui32LinkedBoxInputIndex = uint32_t(value);
	}
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxOutputIndex)
	{
		m_oSymbolicScenario.m_vScenarioOutput.back().m_ui32LinkedBoxOutputIndex = uint32_t(value);
	}

	else
	{
		OV_ERROR("(uint) Unexpected node identifier " << identifier.toString(), OpenViBE::Kernel::ErrorType::BadArgument, false,
				 m_rAlgorithmContext.getErrorManager(), m_rAlgorithmContext.getLogManager());
	}
	return true;
}

bool CAlgorithmScenarioImporterContext::processStop() { return true; }
