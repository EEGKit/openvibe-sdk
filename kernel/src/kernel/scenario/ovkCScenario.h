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
			~CScenario() override;

			bool clear() override;
			bool merge(const IScenario& scenario, IScenarioMergeCallback* scenarioMergeCallback, bool mergeSettings, bool shouldPreserveIdentifies) override;

			CIdentifier getNextBoxIdentifier(const CIdentifier& previousIdentifier) const override;
			bool isBox(const CIdentifier& boxID) const override;
			const IBox* getBoxDetails(const CIdentifier& boxID) const override;
			IBox* getBoxDetails(const CIdentifier& boxID) override;
			bool addBox(CIdentifier& boxID, const CIdentifier& suggestedBoxIdentifier) override;
			bool addBox(CIdentifier& boxID, const IBox& box, const CIdentifier& suggestedBoxIdentifier) override;
			bool addBox(CIdentifier& boxID, const CIdentifier& boxAlgorithmIdentifier, const CIdentifier& suggestedBoxIdentifier) override;
			bool addBox(CIdentifier& boxID, const Plugins::IBoxAlgorithmDesc& boxAlgorithmDesc, const CIdentifier& suggestedBoxIdentifier) override;
			bool removeBox(const CIdentifier& boxID) override;

			CIdentifier getNextCommentIdentifier(const CIdentifier& previousIdentifier) const override;
			bool isComment(const CIdentifier& commentIdentifier) const override;
			const IComment* getCommentDetails(const CIdentifier& commentIdentifier) const override;
			IComment* getCommentDetails(const CIdentifier& commentIdentifier) override;
			bool addComment(CIdentifier& commentIdentifier, const CIdentifier& suggestedCommentIdentifier) override;
			bool addComment(CIdentifier& commentIdentifier, const IComment& rComment, const CIdentifier& suggestedCommentIdentifier) override;
			bool removeComment(const CIdentifier& commentIdentifier) override;

			CIdentifier getNextMetadataIdentifier(const CIdentifier& previousIdentifier) const override;
			bool isMetadata(const CIdentifier& metadataIdentifier) const override;
			const IMetadata* getMetadataDetails(const CIdentifier& metadataIdentifier) const override;
			IMetadata* getMetadataDetails(const CIdentifier& metadataIdentifier) override;
			bool addMetadata(CIdentifier& metadataIdentifier, const CIdentifier& suggestedMetadataIdentifier) override;
			bool removeMetadata(const CIdentifier& metadataIdentifier) override;

			CIdentifier getNextLinkIdentifier(const CIdentifier& previousIdentifier) const override;

			CIdentifier getNextLinkIdentifierFromBox(const CIdentifier& previousIdentifier, const CIdentifier& boxID) const override;
			CIdentifier getNextLinkIdentifierFromBoxOutput(const CIdentifier& previousIdentifier, const CIdentifier& boxID, uint32_t outputIdx) const override;
			CIdentifier getNextLinkIdentifierToBox(const CIdentifier& previousIdentifier, const CIdentifier& boxID) const override;
			CIdentifier getNextLinkIdentifierToBoxInput(const CIdentifier& previousIdentifier, const CIdentifier& boxID, uint32_t inputIdx) const override;
			bool isLink(const CIdentifier& boxID) const override;

			bool setHasIO(bool hasIO) override;
			bool hasIO() const override;
			bool setScenarioInputLink(uint32_t scenarioInputIndex, const CIdentifier& boxID, uint32_t boxInputIndex) override;
			bool setScenarioInputLink(uint32_t scenarioInputIndex, const CIdentifier& boxID, const CIdentifier& boxInputIdentifier) override;
			bool setScenarioOutputLink(uint32_t scenarioOutputIndex, const CIdentifier& boxID, uint32_t boxOutputIndex) override;
			bool setScenarioOutputLink(uint32_t scenarioOutputIndex, const CIdentifier& boxID, const CIdentifier& boxOutputID) override;
			bool getScenarioInputLink(uint32_t scenarioInputIndex, CIdentifier& boxID, uint32_t& boxInputIndex) const override;
			bool getScenarioInputLink(uint32_t scenarioInputIndex, CIdentifier& boxID, CIdentifier& boxOutputID) const override;
			bool getScenarioOutputLink(uint32_t scenarioOutputIndex, CIdentifier& boxID, uint32_t& boxOutputIndex) const override;
			bool getScenarioOutputLink(uint32_t scenarioOutputIndex, CIdentifier& boxID, CIdentifier& boxOutputID) const override;
			bool removeScenarioInputLink(uint32_t scenarioInputIndex, const CIdentifier& boxID, uint32_t boxInputIndex) override;
			bool removeScenarioOutputLink(uint32_t scenarioOutputIndex, const CIdentifier& boxID, uint32_t boxOutputIndex) override;

			bool removeScenarioInput(uint32_t index) override;
			bool removeScenarioOutput(uint32_t outputIdx) override;

			const ILink* getLinkDetails(const CIdentifier& linkIdentifier) const override;
			ILink* getLinkDetails(const CIdentifier& linkIdentifier) override;

			bool connect(CIdentifier& linkIdentifier, const CIdentifier& sourceBoxID, uint32_t sourceBoxOutputIndex,
						 const CIdentifier& targetBoxID, uint32_t targetBoxInputIndex, const CIdentifier& suggestedLinkIdentifier) override;
			bool connect(CIdentifier& linkIdentifier, const CIdentifier& sourceBoxID, const CIdentifier& sourceBoxOutputIdentifier,
						 const CIdentifier& targetBoxID, const CIdentifier& targetBoxInputIdentifier, const CIdentifier& suggestedLinkIdentifier) override;
			bool disconnect(const CIdentifier& sourceBoxID, uint32_t sourceBoxOutputIndex, const CIdentifier& targetBoxID, uint32_t targetBoxInputIndex) override;
			bool disconnect(const CIdentifier& sourceBoxID, const CIdentifier& sourceBoxOutputIdentifier, const CIdentifier& targetBoxID, const CIdentifier& targetBoxInputIdentifier) override;
			bool disconnect(const CIdentifier& linkIdentifier) override;

			bool getSourceBoxOutputIndex(const CIdentifier& sourceBoxID, const CIdentifier& sourceBoxOutputIdentifier, uint32_t& sourceBoxOutputIndex) override;

			bool getTargetBoxInputIndex(const CIdentifier& targetBoxID, const CIdentifier& targetBoxInputIdentifier, uint32_t& targetBoxInputIndex) override;

			bool getSourceBoxOutputIdentifier(const CIdentifier& sourceBoxID, const uint32_t& sourceBoxOutputIndex, CIdentifier& sourceBoxOutputIdentifier) override;

			bool getTargetBoxInputIdentifier(const CIdentifier& targetBoxID, const uint32_t& targetBoxInputIndex, CIdentifier& targetBoxInputIdentifier) override;

			bool applyLocalSettings() override;
			bool checkSettings(IConfigurationManager* configurationManager) override;

			bool isBoxOutdated(const CIdentifier& boxId);
			bool checkOutdatedBoxes() override;
			bool hasOutdatedBox() override;

			CIdentifier getNextOutdatedBoxIdentifier(const CIdentifier& previousIdentifier) const override;

			bool isMetabox() override;

			void getBoxIdentifierList(CIdentifier** identifierList, size_t* size) const override;
			void getCommentIdentifierList(CIdentifier** identifierList, size_t* size) const override;
			void getMetadataIdentifierList(CIdentifier** identifierList, size_t* size) const override;
			void getLinkIdentifierList(CIdentifier** identifierList, size_t* size) const override;
			void getLinkIdentifierFromBoxList(const CIdentifier& boxID, CIdentifier** identifierList, size_t* size) const override;
			void getLinkIdentifierFromBoxOutputList(const CIdentifier& boxID, uint32_t outputIdx, CIdentifier** identifierList, size_t* size) const override;
			void getLinkIdentifierToBoxList(const CIdentifier& boxID, CIdentifier** identifierList, size_t* size) const override;
			void getLinkIdentifierToBoxInputList(const CIdentifier& boxID, uint32_t inputIdx, CIdentifier** identifierList, size_t* size) const override;
			void getOutdatedBoxIdentifierList(CIdentifier** identifierList, size_t* size) const override;
			void releaseIdentifierList(CIdentifier* identifierList) const override;


			bool acceptVisitor(IObjectVisitor& objectVisitor) override;

			bool updateBox(const CIdentifier& boxID) override;

			bool containsBoxWithDeprecatedInterfacors() const override;

			bool removeDeprecatedInterfacorsFromBox(const CIdentifier& boxID) override;

			_IsDerivedFromClass_Final_(TBox< OpenViBE::Kernel::IScenario >, OVK_ClassId_Kernel_Scenario_Scenario)

		private:
			CIdentifier getUnusedIdentifier(const CIdentifier& suggestedIdentifier) const;

			std::map<CIdentifier, CBox*> m_Boxes;
			std::map<CIdentifier, CComment*> m_Comments;
			std::map<CIdentifier, CMetadata*> m_Metadata;
			std::map<CIdentifier, CLink*> m_Links;
			std::map<CIdentifier, std::shared_ptr<CBox>> m_OutdatedBoxes;
			std::map<BoxInterfacorType, std::map<CIdentifier, std::map<uint32_t, uint32_t>>> m_UpdatedBoxIOCorrespondence;


			bool m_HasIO = false;

			mutable std::vector<std::pair<CIdentifier, uint32_t>> m_ScenarioInputLinks;
			mutable std::vector<std::pair<CIdentifier, uint32_t>> m_ScenarioOutputLinks;

			// Helper members. These are used for quick lookup of next identifiers for the purpose
			// of the getNextMetadataIdentifier function.
			std::map<CIdentifier, CIdentifier> m_NextMetadataIdentifier;
			CIdentifier m_FirstMetadataIdentifier = OV_UndefinedIdentifier;
		};
	} // namespace Kernel
} // namespace OpenViBE
