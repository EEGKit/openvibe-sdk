#pragma once

#include "../ovkTKernelObject.h"

#include "ovkTAttributable.h"
#include "ovkTBox.hpp"

#include <vector>
#include <map>
#include <memory>

namespace OpenViBE
{
	namespace Kernel
	{
		typedef TBox<OpenViBE::Kernel::IBox> CBox;
		class CComment;
		class CMetadata;
		class CLink;

		class CScenario final : public TBox<OpenViBE::Kernel::IScenario>
		{
		public:

			CScenario(const OpenViBE::Kernel::IKernelContext& kernelContext, const OpenViBE::CIdentifier& identifier);
			~CScenario(void);

			bool clear(void);
			bool merge(
				const OpenViBE::Kernel::IScenario& scenario,
				OpenViBE::Kernel::IScenario::IScenarioMergeCallback* scenarioMergeCallback,
				bool mergeSettings,
				bool shouldPreserveIdentifies);

			OpenViBE::CIdentifier getNextBoxIdentifier(const OpenViBE::CIdentifier& previousIdentifier) const;
			bool isBox(const OpenViBE::CIdentifier& boxIdentifier) const;
			const OpenViBE::Kernel::IBox* getBoxDetails(const OpenViBE::CIdentifier& boxIdentifier) const;
			OpenViBE::Kernel::IBox* getBoxDetails(const OpenViBE::CIdentifier& boxIdentifier);
			bool addBox(OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::CIdentifier& suggestedBoxIdentifier);
			bool addBox(OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::Kernel::IBox& box, const OpenViBE::CIdentifier& suggestedBoxIdentifier);
			bool addBox(OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::CIdentifier& boxAlgorithmIdentifier, const OpenViBE::CIdentifier& suggestedBoxIdentifier);
			bool addBox(OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::Plugins::IBoxAlgorithmDesc& boxAlgorithmDesc, const OpenViBE::CIdentifier& suggestedBoxIdentifier);
			bool removeBox(const OpenViBE::CIdentifier& boxIdentifier);

			OpenViBE::CIdentifier getNextCommentIdentifier(const OpenViBE::CIdentifier& previousIdentifier) const;
			bool isComment(const OpenViBE::CIdentifier& commentIdentifier) const;
			const OpenViBE::Kernel::IComment* getCommentDetails(const OpenViBE::CIdentifier& commentIdentifier) const;
			OpenViBE::Kernel::IComment* getCommentDetails(const OpenViBE::CIdentifier& commentIdentifier);
			bool addComment(OpenViBE::CIdentifier& commentIdentifier, const OpenViBE::CIdentifier& suggestedCommentIdentifier);
			bool addComment(OpenViBE::CIdentifier& commentIdentifier, const OpenViBE::Kernel::IComment& rComment, const OpenViBE::CIdentifier& suggestedCommentIdentifier);
			bool removeComment(const OpenViBE::CIdentifier& commentIdentifier);

			OpenViBE::CIdentifier getNextMetadataIdentifier(const OpenViBE::CIdentifier& previousIdentifier) const;
			bool isMetadata(const OpenViBE::CIdentifier& metadataIdentifier) const;
			const OpenViBE::Kernel::IMetadata* getMetadataDetails(const OpenViBE::CIdentifier& metadataIdentifier) const;
			OpenViBE::Kernel::IMetadata* getMetadataDetails(const OpenViBE::CIdentifier& metadataIdentifier);
			bool addMetadata(OpenViBE::CIdentifier& metadataIdentifier, const OpenViBE::CIdentifier& suggestedMetadataIdentifier);
			bool removeMetadata(const OpenViBE::CIdentifier& metadataIdentifier);

			OpenViBE::CIdentifier getNextLinkIdentifier(const OpenViBE::CIdentifier& previousIdentifier) const;

			OpenViBE::CIdentifier getNextLinkIdentifierFromBox(const OpenViBE::CIdentifier& previousIdentifier, const OpenViBE::CIdentifier& boxIdentifier) const;
			OpenViBE::CIdentifier getNextLinkIdentifierFromBoxOutput(const OpenViBE::CIdentifier& previousIdentifier, const OpenViBE::CIdentifier& boxIdentifier, const uint32_t outputIndex) const;
			OpenViBE::CIdentifier getNextLinkIdentifierToBox(const OpenViBE::CIdentifier& previousIdentifier, const OpenViBE::CIdentifier& boxIdentifier) const;
			OpenViBE::CIdentifier getNextLinkIdentifierToBoxInput(const OpenViBE::CIdentifier& previousIdentifier, const OpenViBE::CIdentifier& boxIdentifier, const uint32_t inputInex) const;
			bool isLink(const OpenViBE::CIdentifier& boxIdentifier) const;

			bool setHasIO(const bool hasIO);
			bool hasIO() const;
			bool setScenarioInputLink(const uint32_t scenarioInputIndex, const OpenViBE::CIdentifier& boxIdentifier, const uint32_t boxInputIndex);
			bool setScenarioInputLink(const uint32_t scenarioInputIndex, const OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::CIdentifier& boxInputIdentifier);
			bool setScenarioOutputLink(const uint32_t scenarioOutputIndex, const OpenViBE::CIdentifier& boxIdentifier, const uint32_t boxOutputIndex);
			bool setScenarioOutputLink(const uint32_t scenarioOutputIndex, const OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::CIdentifier& boxOutputIdentifier);
			bool getScenarioInputLink(const uint32_t scenarioInputIndex, OpenViBE::CIdentifier& boxIdentifier, uint32_t& boxInputIndex) const;
			bool getScenarioInputLink(const uint32_t scenarioInputIndex, OpenViBE::CIdentifier& boxIdentifier, OpenViBE::CIdentifier& boxOutputIdentifier) const;
			bool getScenarioOutputLink(const uint32_t scenarioOutputIndex, OpenViBE::CIdentifier& boxIdentifier, uint32_t& boxOutputIndex) const;
			bool getScenarioOutputLink(const uint32_t scenarioOutputIndex, OpenViBE::CIdentifier& boxIdentifier, OpenViBE::CIdentifier& boxOutputIdentifier) const;
			bool removeScenarioInputLink(const uint32_t scenarioInputIndex, const OpenViBE::CIdentifier& boxIdentifier, const uint32_t boxInputIndex);
			bool removeScenarioOutputLink(const uint32_t scenarioOutputIndex, const OpenViBE::CIdentifier& boxIdentifier, const uint32_t boxOutputIndex);

			bool removeScenarioInput(const uint32_t inputIndex);
			bool removeScenarioOutput(const uint32_t outputIndex);

			const OpenViBE::Kernel::ILink* getLinkDetails(const OpenViBE::CIdentifier& linkIdentifier) const;
			OpenViBE::Kernel::ILink* getLinkDetails(const OpenViBE::CIdentifier& linkIdentifier);

			bool connect(
				OpenViBE::CIdentifier& linkIdentifier,
				const OpenViBE::CIdentifier& sourceBoxIdentifier,
				const uint32_t sourceBoxOutputIndex,
				const OpenViBE::CIdentifier& targetBoxIdentifier,
				const uint32_t targetBoxInputIndex,
				const OpenViBE::CIdentifier& suggestedLinkIdentifier);
			bool connect(
				OpenViBE::CIdentifier& linkIdentifier,
				const OpenViBE::CIdentifier& sourceBoxIdentifier,
				const OpenViBE::CIdentifier& sourceBoxOutputIdentifier,
				const OpenViBE::CIdentifier& targetBoxIdentifier,
				const OpenViBE::CIdentifier& targetBoxInputIdentifier,
				const OpenViBE::CIdentifier& suggestedLinkIdentifier);
			bool disconnect(
				const OpenViBE::CIdentifier& sourceBoxIdentifier,
				const uint32_t sourceBoxOutputIndex,
				const OpenViBE::CIdentifier& targetBoxIdentifier,
				const uint32_t targetBoxInputIndex);
			bool disconnect(
				const OpenViBE::CIdentifier& sourceBoxIdentifier,
				const OpenViBE::CIdentifier& sourceBoxOutputIdentifier,
				const OpenViBE::CIdentifier& targetBoxIdentifier,
				const OpenViBE::CIdentifier& targetBoxInputIdentifier);
			bool disconnect(const OpenViBE::CIdentifier& linkIdentifier);

			bool getSourceBoxOutputIndex(
				const OpenViBE::CIdentifier& sourceBoxIdentifier,
				const OpenViBE::CIdentifier& sourceBoxOutputIdentifier,
				uint32_t& sourceBoxOutputIndex);

			bool getTargetBoxInputIndex(
				const OpenViBE::CIdentifier& targetBoxIdentifier,
				const OpenViBE::CIdentifier& targetBoxInputIdentifier,
				uint32_t& targetBoxInputIndex);

			bool getSourceBoxOutputIdentifier(
				const OpenViBE::CIdentifier& sourceBoxIdentifier,
				const uint32_t& sourceBoxOutputIndex,
				OpenViBE::CIdentifier& sourceBoxOutputIdentifier);

			bool getTargetBoxInputIdentifier(
				const OpenViBE::CIdentifier& targetBoxIdentifier,
				const uint32_t& targetBoxInputIndex,
				OpenViBE::CIdentifier& targetBoxInputIdentifier);

			bool applyLocalSettings(void);
			bool checkSettings(IConfigurationManager* configurationManager);

			bool isBoxOutdated(const CIdentifier& rBoxIdentifier);
			bool checkOutdatedBoxes();
			bool hasOutdatedBox();

			OpenViBE::CIdentifier getNextOutdatedBoxIdentifier(const OpenViBE::CIdentifier& previousIdentifier) const;

			bool isMetabox(void);

			void getBoxIdentifierList(OpenViBE::CIdentifier** identifierList, size_t* size) const;
			void getCommentIdentifierList(OpenViBE::CIdentifier** identifierList, size_t* size) const;
			void getMetadataIdentifierList(OpenViBE::CIdentifier** identifierList, size_t* size) const;
			void getLinkIdentifierList(OpenViBE::CIdentifier** identifierList, size_t* size) const;
			void getLinkIdentifierFromBoxList(const OpenViBE::CIdentifier& boxIdentifier, OpenViBE::CIdentifier** identifierList, size_t* size) const;
			void getLinkIdentifierFromBoxOutputList(const OpenViBE::CIdentifier& boxIdentifier, const uint32_t outputIndex, OpenViBE::CIdentifier** identifierList, size_t* size) const;
			void getLinkIdentifierToBoxList(const OpenViBE::CIdentifier& boxIdentifier, OpenViBE::CIdentifier** identifierList, size_t* size) const;
			void getLinkIdentifierToBoxInputList(const OpenViBE::CIdentifier& boxIdentifier, const uint32_t inputInex, OpenViBE::CIdentifier** identifierList, size_t* size) const;
			void getOutdatedBoxIdentifierList(OpenViBE::CIdentifier** identifierList, size_t* size) const;
			void releaseIdentifierList(OpenViBE::CIdentifier* identifierList) const;


			bool acceptVisitor(OpenViBE::IObjectVisitor& objectVisitor);

			bool updateBox(const CIdentifier& boxIdentifier);

			bool containsBoxWithDeprecatedInterfacors() const;

			bool removeDeprecatedInterfacorsFromBox(const CIdentifier& boxIdentifier);

			_IsDerivedFromClass_Final_(TBox< OpenViBE::Kernel::IScenario >, OVK_ClassId_Kernel_Scenario_Scenario)

		private:
			OpenViBE::CIdentifier getUnusedIdentifier(const CIdentifier& suggestedIdentifier) const;

			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CBox*> m_Boxes;
			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CComment*> m_Comments;
			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CMetadata*> m_Metadata;
			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CLink*> m_Links;
			std::map<OpenViBE::CIdentifier, std::shared_ptr<OpenViBE::Kernel::CBox>> m_OutdatedBoxes;
			std::map<BoxInterfacorType, std::map<CIdentifier, std::map<uint32_t, uint32_t>>> m_UpdatedBoxIOCorrespondence;


			bool m_HasIO;

			mutable std::vector<std::pair<OpenViBE::CIdentifier, uint32_t>> m_ScenarioInputLinks;
			mutable std::vector<std::pair<OpenViBE::CIdentifier, uint32_t>> m_ScenarioOutputLinks;

			// Helper members. These are used for quick lookup of next identifiers for the purpose
			// of the getNextMetadataIdentifier function.
			std::map<OpenViBE::CIdentifier, OpenViBE::CIdentifier> m_NextMetadataIdentifier;
			OpenViBE::CIdentifier m_FirstMetadataIdentifier;
		};
	}
}
