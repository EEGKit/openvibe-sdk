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
		typedef TBox<IBox> CBox;
		class CComment;
		class CMetadata;
		class CLink;

		class CScenario final : public TBox<IScenario>
		{
		public:

			CScenario(const IKernelContext& kernelContext, const CIdentifier& identifier);
			~CScenario();

			bool clear();
			bool merge(const IScenario& scenario, IScenarioMergeCallback* scenarioMergeCallback, bool mergeSettings, bool shouldPreserveIdentifies);

			CIdentifier getNextBoxIdentifier(const CIdentifier& previousIdentifier) const;
			bool isBox(const CIdentifier& boxIdentifier) const;
			const IBox* getBoxDetails(const CIdentifier& boxIdentifier) const;
			IBox* getBoxDetails(const CIdentifier& boxIdentifier);
			bool addBox(CIdentifier& boxIdentifier, const CIdentifier& suggestedBoxIdentifier);
			bool addBox(CIdentifier& boxIdentifier, const IBox& box, const CIdentifier& suggestedBoxIdentifier);
			bool addBox(CIdentifier& boxIdentifier, const CIdentifier& boxAlgorithmIdentifier, const CIdentifier& suggestedBoxIdentifier);
			bool addBox(CIdentifier& boxIdentifier, const Plugins::IBoxAlgorithmDesc& boxAlgorithmDesc, const CIdentifier& suggestedBoxIdentifier);
			bool removeBox(const CIdentifier& boxIdentifier);

			CIdentifier getNextCommentIdentifier(const CIdentifier& previousIdentifier) const;
			bool isComment(const CIdentifier& commentIdentifier) const;
			const IComment* getCommentDetails(const CIdentifier& commentIdentifier) const;
			IComment* getCommentDetails(const CIdentifier& commentIdentifier);
			bool addComment(CIdentifier& commentIdentifier, const CIdentifier& suggestedCommentIdentifier);
			bool addComment(CIdentifier& commentIdentifier, const IComment& rComment, const CIdentifier& suggestedCommentIdentifier);
			bool removeComment(const CIdentifier& commentIdentifier);

			CIdentifier getNextMetadataIdentifier(const CIdentifier& previousIdentifier) const;
			bool isMetadata(const CIdentifier& metadataIdentifier) const;
			const IMetadata* getMetadataDetails(const CIdentifier& metadataIdentifier) const;
			IMetadata* getMetadataDetails(const CIdentifier& metadataIdentifier);
			bool addMetadata(CIdentifier& metadataIdentifier, const CIdentifier& suggestedMetadataIdentifier);
			bool removeMetadata(const CIdentifier& metadataIdentifier);

			CIdentifier getNextLinkIdentifier(const CIdentifier& previousIdentifier) const;

			CIdentifier getNextLinkIdentifierFromBox(const CIdentifier& previousIdentifier, const CIdentifier& boxIdentifier) const;
			CIdentifier getNextLinkIdentifierFromBoxOutput(const CIdentifier& previousIdentifier, const CIdentifier& boxIdentifier, uint32_t outputIndex) const;
			CIdentifier getNextLinkIdentifierToBox(const CIdentifier& previousIdentifier, const CIdentifier& boxIdentifier) const;
			CIdentifier getNextLinkIdentifierToBoxInput(const CIdentifier& previousIdentifier, const CIdentifier& boxIdentifier, uint32_t inputInex) const;
			bool isLink(const CIdentifier& boxIdentifier) const;

			bool setHasIO(bool hasIO);
			bool hasIO() const;
			bool setScenarioInputLink(uint32_t scenarioInputIndex, const CIdentifier& boxIdentifier, uint32_t boxInputIndex);
			bool setScenarioInputLink(uint32_t scenarioInputIndex, const CIdentifier& boxIdentifier, const CIdentifier& boxInputIdentifier);
			bool setScenarioOutputLink(uint32_t scenarioOutputIndex, const CIdentifier& boxIdentifier, uint32_t boxOutputIndex);
			bool setScenarioOutputLink(uint32_t scenarioOutputIndex, const CIdentifier& boxIdentifier, const CIdentifier& boxOutputIdentifier);
			bool getScenarioInputLink(uint32_t scenarioInputIndex, CIdentifier& boxIdentifier, uint32_t& boxInputIndex) const;
			bool getScenarioInputLink(uint32_t scenarioInputIndex, CIdentifier& boxIdentifier, CIdentifier& boxOutputIdentifier) const;
			bool getScenarioOutputLink(uint32_t scenarioOutputIndex, CIdentifier& boxIdentifier, uint32_t& boxOutputIndex) const;
			bool getScenarioOutputLink(uint32_t scenarioOutputIndex, CIdentifier& boxIdentifier, CIdentifier& boxOutputIdentifier) const;
			bool removeScenarioInputLink(uint32_t scenarioInputIndex, const CIdentifier& boxIdentifier, uint32_t boxInputIndex);
			bool removeScenarioOutputLink(uint32_t scenarioOutputIndex, const CIdentifier& boxIdentifier, uint32_t boxOutputIndex);

			bool removeScenarioInput(uint32_t inputIndex);
			bool removeScenarioOutput(uint32_t outputIndex);

			const ILink* getLinkDetails(const CIdentifier& linkIdentifier) const;
			ILink* getLinkDetails(const CIdentifier& linkIdentifier);

			bool connect(CIdentifier& linkIdentifier, const CIdentifier& sourceBoxIdentifier, uint32_t sourceBoxOutputIndex,
						 const CIdentifier& targetBoxIdentifier, uint32_t targetBoxInputIndex, const CIdentifier& suggestedLinkIdentifier);
			bool connect(CIdentifier& linkIdentifier, const CIdentifier& sourceBoxIdentifier, const CIdentifier& sourceBoxOutputIdentifier,
						 const CIdentifier& targetBoxIdentifier,  const CIdentifier& targetBoxInputIdentifier, const CIdentifier& suggestedLinkIdentifier);
			bool disconnect(const CIdentifier& sourceBoxIdentifier, uint32_t sourceBoxOutputIndex, const CIdentifier& targetBoxIdentifier, uint32_t targetBoxInputIndex);
			bool disconnect(const CIdentifier& sourceBoxIdentifier, const CIdentifier& sourceBoxOutputIdentifier, const CIdentifier& targetBoxIdentifier, const CIdentifier& targetBoxInputIdentifier);
			bool disconnect(const CIdentifier& linkIdentifier);

			bool getSourceBoxOutputIndex(const CIdentifier& sourceBoxIdentifier, const CIdentifier& sourceBoxOutputIdentifier, uint32_t& sourceBoxOutputIndex);

			bool getTargetBoxInputIndex(const CIdentifier& targetBoxIdentifier, const CIdentifier& targetBoxInputIdentifier, uint32_t& targetBoxInputIndex);

			bool getSourceBoxOutputIdentifier(const CIdentifier& sourceBoxIdentifier, const uint32_t& sourceBoxOutputIndex, CIdentifier& sourceBoxOutputIdentifier);

			bool getTargetBoxInputIdentifier(const CIdentifier& targetBoxIdentifier, const uint32_t& targetBoxInputIndex, CIdentifier& targetBoxInputIdentifier);

			bool applyLocalSettings();
			bool checkSettings(IConfigurationManager* configurationManager);

			bool isBoxOutdated(const CIdentifier& rBoxIdentifier);
			bool checkOutdatedBoxes();
			bool hasOutdatedBox();

			CIdentifier getNextOutdatedBoxIdentifier(const CIdentifier& previousIdentifier) const;

			bool isMetabox();

			void getBoxIdentifierList(CIdentifier** identifierList, size_t* size) const;
			void getCommentIdentifierList(CIdentifier** identifierList, size_t* size) const;
			void getMetadataIdentifierList(CIdentifier** identifierList, size_t* size) const;
			void getLinkIdentifierList(CIdentifier** identifierList, size_t* size) const;
			void getLinkIdentifierFromBoxList(const CIdentifier& boxIdentifier, CIdentifier** identifierList, size_t* size) const;
			void getLinkIdentifierFromBoxOutputList(const CIdentifier& boxIdentifier, uint32_t outputIndex, CIdentifier** identifierList, size_t* size) const;
			void getLinkIdentifierToBoxList(const CIdentifier& boxIdentifier, CIdentifier** identifierList, size_t* size) const;
			void getLinkIdentifierToBoxInputList(const CIdentifier& boxIdentifier, uint32_t inputInex, CIdentifier** identifierList, size_t* size) const;
			void getOutdatedBoxIdentifierList(CIdentifier** identifierList, size_t* size) const;
			void releaseIdentifierList(CIdentifier* identifierList) const;


			bool acceptVisitor(IObjectVisitor& objectVisitor);

			bool updateBox(const CIdentifier& boxIdentifier);

			bool containsBoxWithDeprecatedInterfacors() const;

			bool removeDeprecatedInterfacorsFromBox(const CIdentifier& boxIdentifier);

			_IsDerivedFromClass_Final_(TBox< OpenViBE::Kernel::IScenario >, OVK_ClassId_Kernel_Scenario_Scenario)

		private:
			CIdentifier getUnusedIdentifier(const CIdentifier& suggestedIdentifier) const;

			std::map<CIdentifier, CBox*> m_Boxes;
			std::map<CIdentifier, CComment*> m_Comments;
			std::map<CIdentifier, CMetadata*> m_Metadata;
			std::map<CIdentifier, CLink*> m_Links;
			std::map<CIdentifier, std::shared_ptr<CBox>> m_OutdatedBoxes;
			std::map<BoxInterfacorType, std::map<CIdentifier, std::map<uint32_t, uint32_t>>> m_UpdatedBoxIOCorrespondence;


			bool m_HasIO;

			mutable std::vector<std::pair<CIdentifier, uint32_t>> m_ScenarioInputLinks;
			mutable std::vector<std::pair<CIdentifier, uint32_t>> m_ScenarioOutputLinks;

			// Helper members. These are used for quick lookup of next identifiers for the purpose
			// of the getNextMetadataIdentifier function.
			std::map<CIdentifier, CIdentifier> m_NextMetadataIdentifier;
			CIdentifier m_FirstMetadataIdentifier;
		};
	}  // namespace Kernel
}  // namespace OpenViBE
