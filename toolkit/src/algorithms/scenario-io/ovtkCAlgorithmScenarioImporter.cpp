#include "ovtkCAlgorithmScenarioImporter.h"

#include <vector>
#include <map>
#include <iostream>
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
			CIdentifier id     = OV_UndefinedIdentifier;
			CIdentifier typeID = OV_UndefinedIdentifier;
			CString name;
			CIdentifier linkedBoxID      = OV_UndefinedIdentifier;
			uint32_t linkedBoxInputIdx   = OV_Value_UndefinedIndexUInt;
			CIdentifier linkedBoxInputID = OV_UndefinedIdentifier;
		} SScenarioInput;

		typedef struct _SScenarioOutput
		{
			CIdentifier id     = OV_UndefinedIdentifier;
			CIdentifier typeID = OV_UndefinedIdentifier;
			CString name;
			CIdentifier linkedBoxID       = OV_UndefinedIdentifier;
			uint32_t linkedBoxOutputIdx   = OV_Value_UndefinedIndexUInt;
			CIdentifier linkedBoxOutputID = OV_UndefinedIdentifier;
		} SScenarioOutput;

		typedef struct _SInput
		{
			CIdentifier id     = OV_UndefinedIdentifier;
			CIdentifier typeID = OV_UndefinedIdentifier;
			CString name;
		} SInput;

		typedef struct _SOutput
		{
			CIdentifier id     = OV_UndefinedIdentifier;
			CIdentifier typeID = OV_UndefinedIdentifier;
			CString name;
		} SOutput;

		typedef struct _SSetting
		{
			CIdentifier typeID = OV_UndefinedIdentifier;
			CString name;
			CString defaultValue;
			CString value;
			bool modifiability = false;
			CIdentifier id     = OV_UndefinedIdentifier;
		} SSetting;

		typedef struct _SAttribute
		{
			CIdentifier id = OV_UndefinedIdentifier;
			CString value;
		} SAttribute;

		typedef struct _SBox
		{
			CIdentifier id               = OV_UndefinedIdentifier;
			CIdentifier algorithmClassID = OV_UndefinedIdentifier;
			CString name;
			std::vector<SInput> inputs;
			std::vector<SOutput> outputs;
			std::vector<SSetting> settings;
			std::vector<SAttribute> attributes;
		} SBox;

		typedef struct _SComment
		{
			CIdentifier id;
			CString text;
			std::vector<SAttribute> attributes;
		} SComment;

		typedef struct _SMetadata
		{
			CIdentifier identifier;
			CIdentifier type;
			CString data;
		} SMetadata;

		typedef struct _SLinkSource
		{
			CIdentifier boxID;
			uint32_t boxOutputIdx   = OV_Value_UndefinedIndexUInt;
			CIdentifier boxOutputID = OV_UndefinedIdentifier;
		} SLinkSource;

		typedef struct _SLinkTarget
		{
			CIdentifier boxID;
			uint32_t boxInputIdx   = OV_Value_UndefinedIndexUInt;
			CIdentifier boxInputID = OV_UndefinedIdentifier;
		} SLinkTarget;

		typedef struct _SLink
		{
			CIdentifier id;
			SLinkSource linkSrc;
			SLinkTarget linkDst;
			std::vector<SAttribute> attributes;
		} SLink;

		typedef struct _SScenario
		{
			std::vector<SSetting> settings;
			std::vector<SScenarioInput> iScenarios;
			std::vector<SScenarioOutput> oScenarios;
			std::vector<SBox> boxes;
			std::vector<SComment> comments;
			std::vector<SMetadata> metadata;
			std::vector<SLink> links;
			std::vector<SAttribute> attributes;
		} SScenario;
	} // namespace

	class CAlgorithmScenarioImporterContext final : public IAlgorithmScenarioImporterContext
	{
	public:

		explicit CAlgorithmScenarioImporterContext(IAlgorithmContext& algorithmCtx) : m_AlgorithmContext(algorithmCtx) { }

		bool processStart(const CIdentifier& identifier) override;
		bool processIdentifier(const CIdentifier& identifier, const CIdentifier& value) override;
		bool processString(const CIdentifier& identifier, const CString& value) override;
		bool processUInteger(const CIdentifier& identifier, uint64_t value) override;
		bool processStop() override;

		_IsDerivedFromClass_Final_(IAlgorithmScenarioImporterContext, OV_UndefinedIdentifier)

		IAlgorithmContext& m_AlgorithmContext;
		SScenario m_SymbolicScenario;
	};
} // namespace OpenViBEToolkit

bool CAlgorithmScenarioImporter::process()
{
	TParameterHandler<IScenario*> op_pScenario(this->getOutputParameter(OV_Algorithm_ScenarioImporter_OutputParameterId_Scenario));
	IScenario* scenario = op_pScenario;

	OV_ERROR_UNLESS_KRF(scenario, "Output scenario is NULL", OpenViBE::Kernel::ErrorType::BadOutput);

	TParameterHandler<IMemoryBuffer*> ip_pMemoryBuffer(this->getInputParameter(OV_Algorithm_ScenarioImporter_InputParameterId_MemoryBuffer));
	IMemoryBuffer* memoryBuffer = ip_pMemoryBuffer;

	OV_ERROR_UNLESS_KRF(memoryBuffer, "Input memory buffer is NULL", OpenViBE::Kernel::ErrorType::BadInput);

	std::map<CIdentifier, CIdentifier> boxIdMapping;

	CAlgorithmScenarioImporterContext context(this->getAlgorithmContext());

	OV_ERROR_UNLESS_KRF(this->import(context, *memoryBuffer), "Import failed", OpenViBE::Kernel::ErrorType::Internal);

	SScenario& symbolicScenario = context.m_SymbolicScenario;

	// Now build the scenario according to what has been loaded

	for (auto s = symbolicScenario.settings.begin(); s != symbolicScenario.settings.end(); ++s)
	{
		CIdentifier settingID = s->id;
		// compute identifier only if it does not exists
		if (settingID == OV_UndefinedIdentifier) { settingID = scenario->getUnusedSettingIdentifier(); }
		scenario->addSetting(s->name, s->typeID, s->defaultValue, OV_Value_UndefinedIndexUInt, false, settingID);
		scenario->setSettingValue(scenario->getSettingCount() - 1, s->value);
	}


	for (auto b = symbolicScenario.boxes.begin(); b != symbolicScenario.boxes.end(); ++b)
	{
		IBox* box = nullptr;
		CIdentifier newBoxID;

		scenario->addBox(newBoxID, b->id);
		box = scenario->getBoxDetails(newBoxID);
		if (box)
		{
			box->setName(b->name);

			for (auto i = b->inputs.begin(); i != b->inputs.end(); ++i) { box->addInput(i->name, i->typeID, i->id); }

			for (auto o = b->outputs.begin(); o != b->outputs.end(); ++o) { box->addOutput(o->name, o->typeID, o->id); }
			for (auto s = b->settings.begin(); s != b->settings.end(); ++s)
			{
				const CIdentifier& type = s->typeID;
				if (!this->getTypeManager().isRegistered(type) && !(this->getTypeManager().isEnumeration(type)) && (!this->getTypeManager().isBitMask(type)))
				{
					if (this->getConfigurationManager().expandAsBoolean("${Kernel_AbortScenarioImportOnUnknownSetting}", true))
					{
						OV_ERROR_KRF("The type of the setting " << s->name <<" (" << type.toString() << ") from box " << b->name << " cannot be recognized.",
									 OpenViBE::Kernel::ErrorType::BadSetting);
					}
					OV_WARNING_K("The type of the setting " << s->name <<" (" << type.toString() << ") from box " << b->name << " cannot be recognized.");
				}

				box->addSetting(s->name, s->typeID, s->defaultValue, OV_Value_UndefinedIndexUInt, s->modifiability, s->id);
				box->setSettingValue(box->getSettingCount() - 1, s->value);
			}
			for (auto a = b->attributes.begin(); a != b->attributes.end(); ++a) { box->addAttribute(a->id, a->value); }

			// it is important to set box algorithm at
			// last so the box listener is never called
			box->setAlgorithmClassIdentifier(b->algorithmClassID);
		}
		boxIdMapping[b->id] = newBoxID;
	}

	for (auto c = symbolicScenario.comments.begin(); c != symbolicScenario.comments.end(); ++c)
	{
		IComment* comment = nullptr;
		CIdentifier newCommentID;

		scenario->addComment(newCommentID, c->id);
		comment = scenario->getCommentDetails(newCommentID);
		if (comment)
		{
			comment->setText(c->text);

			for (auto a = c->attributes.begin(); a != c->attributes.end(); ++a) { comment->addAttribute(a->id, a->value); }
		}
	}

	for (auto& symbolicMetadata : symbolicScenario.metadata)
	{
		CIdentifier newMetadataIdentifier;
		scenario->addMetadata(newMetadataIdentifier, symbolicMetadata.identifier);
		IMetadata* metadata = scenario->getMetadataDetails(newMetadataIdentifier);
		if (metadata)
		{
			metadata->setType(symbolicMetadata.type);
			metadata->setData(symbolicMetadata.data);
		}
	}

	for (auto l = symbolicScenario.links.begin(); l != symbolicScenario.links.end(); ++l)
	{
		ILink* link = nullptr;
		CIdentifier newLinkID;

		uint32_t srcBoxOutputIdx = l->linkSrc.boxOutputIdx;
		uint32_t dstBoxInputIdx  = l->linkDst.boxInputIdx;

		CIdentifier srcBoxOutputID = l->linkSrc.boxOutputID;
		CIdentifier dstBoxInputID  = l->linkDst.boxInputID;

		if (srcBoxOutputID != OV_UndefinedIdentifier) { scenario->getSourceBoxOutputIndex(boxIdMapping[l->linkSrc.boxID], srcBoxOutputID, srcBoxOutputIdx); }

		OV_ERROR_UNLESS_KRF(srcBoxOutputIdx != OV_Value_UndefinedIndexUInt, "Output index of the source box could not be found", OpenViBE::Kernel::ErrorType::BadOutput);

		if (dstBoxInputID != OV_UndefinedIdentifier) { scenario->getTargetBoxInputIndex(boxIdMapping[l->linkDst.boxID], dstBoxInputID, dstBoxInputIdx); }

		OV_ERROR_UNLESS_KRF(dstBoxInputIdx != OV_Value_UndefinedIndexUInt, "Input index of the target box could not be found", OpenViBE::Kernel::ErrorType::BadOutput);

		scenario->connect(newLinkID, boxIdMapping[l->linkSrc.boxID], srcBoxOutputIdx, boxIdMapping[l->linkDst.boxID], dstBoxInputIdx, l->id);

		link = scenario->getLinkDetails(newLinkID);
		if (link) { for (auto a = l->attributes.begin(); a != l->attributes.end(); ++a) { link->addAttribute(a->id, a->value); } }
	}

	uint32_t scenarioInputIdx = 0;
	for (auto symbolicScenarioInput : symbolicScenario.iScenarios)
	{
		CIdentifier scenarioInputID = symbolicScenarioInput.id;
		// compute identifier only if it does not exists
		if (scenarioInputID == OV_UndefinedIdentifier) { scenarioInputID = scenario->getUnusedInputIdentifier(); }
		scenario->addInput(symbolicScenarioInput.name, symbolicScenarioInput.typeID, scenarioInputID);
		if (symbolicScenarioInput.linkedBoxID != OV_UndefinedIdentifier)
		{
			// Only try to set scenario output links from boxes that actually exist
			// This enables the usage of header-only importers
			if (symbolicScenario.boxes.end() != std::find_if(symbolicScenario.boxes.begin(), symbolicScenario.boxes.end(), [&symbolicScenarioInput](SBox box) { return box.id == symbolicScenarioInput.linkedBoxID; }))
			{
				CIdentifier linkedBoxInputIdentifier = symbolicScenarioInput.linkedBoxInputID;
				uint32_t linkedBoxInputIndex         = symbolicScenarioInput.linkedBoxInputIdx;

				if (linkedBoxInputIdentifier != OV_UndefinedIdentifier) { scenario->getTargetBoxInputIndex(symbolicScenarioInput.linkedBoxID, linkedBoxInputIdentifier, linkedBoxInputIndex); }

				OV_ERROR_UNLESS_KRF(linkedBoxInputIndex != OV_Value_UndefinedIndexUInt, "Input index of the target box could not be found", OpenViBE::Kernel::ErrorType::BadOutput);

				scenario->setScenarioInputLink(scenarioInputIdx, symbolicScenarioInput.linkedBoxID, linkedBoxInputIndex);
			}
		}
		scenarioInputIdx++;
	}

	uint32_t scenarioOutputIdx = 0;
	for (auto symbolicScenarioOutput : symbolicScenario.oScenarios)
	{
		CIdentifier scenarioOutputID = symbolicScenarioOutput.id;
		// compute identifier only if it does not exists
		if (scenarioOutputID == OV_UndefinedIdentifier) { scenarioOutputID = scenario->getUnusedOutputIdentifier(); }
		scenario->addOutput(symbolicScenarioOutput.name, symbolicScenarioOutput.typeID, scenarioOutputID);
		if (symbolicScenarioOutput.linkedBoxID != OV_UndefinedIdentifier)
		{
			// Only try to set scenario output links from boxes that actually exist
			// This enables the usage of header-only importers
			if (std::any_of(symbolicScenario.boxes.begin(), symbolicScenario.boxes.end(), [&symbolicScenarioOutput](SBox box) { return box.id == symbolicScenarioOutput.linkedBoxID; }))
			{
				CIdentifier linkedBoxOutputIdentifier = symbolicScenarioOutput.linkedBoxOutputID;
				uint32_t linkedBoxOutputIndex         = symbolicScenarioOutput.linkedBoxOutputIdx;

				if (linkedBoxOutputIdentifier != OV_UndefinedIdentifier) { scenario->getSourceBoxOutputIndex(symbolicScenarioOutput.linkedBoxID, linkedBoxOutputIdentifier, linkedBoxOutputIndex); }

				OV_ERROR_UNLESS_KRF(linkedBoxOutputIndex != OV_Value_UndefinedIndexUInt, "Output index of the target box could not be found", OpenViBE::Kernel::ErrorType::BadOutput);
				scenario->setScenarioOutputLink(scenarioOutputIdx, symbolicScenarioOutput.linkedBoxID, linkedBoxOutputIndex);
			}
		}
		scenarioOutputIdx++;
	}

	for (auto a = symbolicScenario.attributes.begin(); a != symbolicScenario.attributes.end(); ++a) { scenario->addAttribute(a->id, a->value); }

	if (scenario->checkOutdatedBoxes())
	{
		CIdentifier* listID = nullptr;
		size_t nbElems      = 0;
		scenario->getOutdatedBoxIdentifierList(&listID, &nbElems);
		for (size_t i = 0; i < nbElems; ++i)
		{
			const IBox* box = scenario->getBoxDetails(listID[i]);
			OV_WARNING_K("Box " << box->getName() << " [" << box->getAlgorithmClassIdentifier().toString() << "] should be updated");
		}
		scenario->releaseIdentifierList(listID);
	}

	return true;
}

bool CAlgorithmScenarioImporterContext::processStart(const CIdentifier& identifier)
{
	if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_OpenViBEScenario) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Settings) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting) { m_SymbolicScenario.settings.push_back(SSetting()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Inputs) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input) { m_SymbolicScenario.iScenarios.push_back(SScenarioInput()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Outputs) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output) { m_SymbolicScenario.oScenarios.push_back(SScenarioOutput()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Creator) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_CreatorVersion) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attributes) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute) { m_SymbolicScenario.attributes.push_back(SAttribute()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attributes) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute) { m_SymbolicScenario.boxes.back().attributes.push_back(SAttribute()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attributes) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute) { m_SymbolicScenario.links.back().attributes.push_back(SAttribute()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Boxes) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box) { m_SymbolicScenario.boxes.push_back(SBox()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comments) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment) { m_SymbolicScenario.comments.push_back(SComment()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attributes) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute) { m_SymbolicScenario.comments.back().attributes.push_back(SAttribute()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Metadata) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry) { m_SymbolicScenario.metadata.push_back(SMetadata()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Links) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link) { m_SymbolicScenario.links.push_back(SLink()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Inputs) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input) { m_SymbolicScenario.boxes.back().inputs.push_back(SInput()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Outputs) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output) { m_SymbolicScenario.boxes.back().outputs.push_back(SOutput()); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Settings) { }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting) { m_SymbolicScenario.boxes.back().settings.push_back(SSetting()); }
		//
	else
	{
		OV_ERROR("(start) Unexpected node identifier " << identifier.toString(), OpenViBE::Kernel::ErrorType::BadArgument, false,
				 m_AlgorithmContext.getErrorManager(), m_AlgorithmContext.getLogManager());
	}
	return true;
}

bool CAlgorithmScenarioImporterContext::processIdentifier(const CIdentifier& identifier, const CIdentifier& value)
{
	if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Identifier) { m_SymbolicScenario.settings.back().id = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_TypeIdentifier) { m_SymbolicScenario.settings.back().typeID = value; }

	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_Identifier) { m_SymbolicScenario.iScenarios.back().id = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_TypeIdentifier) { m_SymbolicScenario.iScenarios.back().typeID = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxIdentifier) { m_SymbolicScenario.iScenarios.back().linkedBoxID = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxInputIdentifier) { m_SymbolicScenario.iScenarios.back().linkedBoxInputID = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_Identifier) { m_SymbolicScenario.oScenarios.back().id = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_TypeIdentifier) { m_SymbolicScenario.oScenarios.back().typeID = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxIdentifier) { m_SymbolicScenario.oScenarios.back().linkedBoxID = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxOutputIdentifier) { m_SymbolicScenario.oScenarios.back().linkedBoxOutputID = value; }

	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute_Identifier) { m_SymbolicScenario.boxes.back().attributes.back().id = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Identifier) { m_SymbolicScenario.boxes.back().id = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_AlgorithmClassIdentifier) { m_SymbolicScenario.boxes.back().algorithmClassID = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_Identifier) { m_SymbolicScenario.boxes.back().inputs.back().id = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_TypeIdentifier) { m_SymbolicScenario.boxes.back().inputs.back().typeID = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_Identifier) { m_SymbolicScenario.boxes.back().outputs.back().id = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_TypeIdentifier) { m_SymbolicScenario.boxes.back().outputs.back().typeID = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Identifier) { m_SymbolicScenario.boxes.back().settings.back().id = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_TypeIdentifier) { m_SymbolicScenario.boxes.back().settings.back().typeID = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Identifier) { m_SymbolicScenario.comments.back().id = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute_Identifier) { m_SymbolicScenario.comments.back().attributes.back().id = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Identifier) { m_SymbolicScenario.metadata.back().identifier = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Type) { m_SymbolicScenario.metadata.back().type = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute_Identifier) { m_SymbolicScenario.links.back().attributes.back().id = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Identifier) { m_SymbolicScenario.links.back().id = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxIdentifier) { m_SymbolicScenario.links.back().linkSrc.boxID = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxOutputIdentifier) { m_SymbolicScenario.links.back().linkSrc.boxOutputID = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxIdentifier) { m_SymbolicScenario.links.back().linkDst.boxID = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxInputIdentifier) { m_SymbolicScenario.links.back().linkDst.boxInputID = value; }


	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute_Identifier) { m_SymbolicScenario.attributes.back().id = value; }
	else
	{
		OV_ERROR("(id) Unexpected node identifier " << identifier.toString(),
				 OpenViBE::Kernel::ErrorType::BadArgument, false, m_AlgorithmContext.getErrorManager(), m_AlgorithmContext.getLogManager());
	}
	return true;
}

bool CAlgorithmScenarioImporterContext::processString(const CIdentifier& identifier, const CString& value)
{
	if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Attribute_Value) { m_SymbolicScenario.boxes.back().attributes.back().value = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Name) { m_SymbolicScenario.boxes.back().name = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Input_Name) { m_SymbolicScenario.boxes.back().inputs.back().name = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Output_Name) { m_SymbolicScenario.boxes.back().outputs.back().name = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Name) { m_SymbolicScenario.boxes.back().settings.back().name = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_DefaultValue) { m_SymbolicScenario.boxes.back().settings.back().defaultValue = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Value) { m_SymbolicScenario.boxes.back().settings.back().value = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Box_Setting_Modifiability) { m_SymbolicScenario.boxes.back().settings.back().modifiability = (value == CString("true")) ? true : false; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Text) { m_SymbolicScenario.comments.back().text = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Comment_Attribute_Value) { m_SymbolicScenario.comments.back().attributes.back().value = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_MetadataEntry_Data) { m_SymbolicScenario.metadata.back().data = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Attribute_Value) { m_SymbolicScenario.links.back().attributes.back().value = value; }

	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Attribute_Value) { m_SymbolicScenario.attributes.back().value = value; }

	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Name) { m_SymbolicScenario.settings.back().name = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_DefaultValue) { m_SymbolicScenario.settings.back().defaultValue = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Setting_Value) { m_SymbolicScenario.settings.back().value = value; }

	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_Name) { m_SymbolicScenario.iScenarios.back().name = value; }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_Name) { m_SymbolicScenario.oScenarios.back().name = value; }

	else
	{
		OV_ERROR("(string) Unexpected node identifier " << identifier.toString(), OpenViBE::Kernel::ErrorType::BadArgument,
				 false, m_AlgorithmContext.getErrorManager(), m_AlgorithmContext.getLogManager());
	}
	return true;
}

bool CAlgorithmScenarioImporterContext::processUInteger(const CIdentifier& identifier, const uint64_t value)
{
	if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Source_BoxOutputIndex) { m_SymbolicScenario.links.back().linkSrc.boxOutputIdx = uint32_t(value); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Link_Target_BoxInputIndex) { m_SymbolicScenario.links.back().linkDst.boxInputIdx = uint32_t(value); }

	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Input_LinkedBoxInputIndex) { m_SymbolicScenario.iScenarios.back().linkedBoxInputIdx = uint32_t(value); }
	else if (identifier == OVTK_Algorithm_ScenarioExporter_NodeId_Scenario_Output_LinkedBoxOutputIndex) { m_SymbolicScenario.oScenarios.back().linkedBoxOutputIdx = uint32_t(value); }

	else
	{
		OV_ERROR("(uint) Unexpected node identifier " << identifier.toString(), OpenViBE::Kernel::ErrorType::BadArgument, false,
				 m_AlgorithmContext.getErrorManager(), m_AlgorithmContext.getLogManager());
	}
	return true;
}

bool CAlgorithmScenarioImporterContext::processStop() { return true; }
