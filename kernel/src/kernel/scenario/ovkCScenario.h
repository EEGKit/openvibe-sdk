#pragma once

#include "../ovkTKernelObject.h"

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

			CScenario(const IKernelContext& ctx, const CIdentifier& identifier);
			~CScenario() override;

			bool clear() override;
			bool merge(const IScenario& scenario, IScenarioMergeCallback* scenarioMergeCallback, bool mergeSettings, bool shouldPreserveIDs) override;

			CIdentifier getNextBoxIdentifier(const CIdentifier& previousID) const override;
			bool isBox(const CIdentifier& boxID) const override;
			const IBox* getBoxDetails(const CIdentifier& boxID) const override;
			IBox* getBoxDetails(const CIdentifier& boxID) override;
			bool addBox(CIdentifier& boxID, const CIdentifier& suggestedBoxID) override;
			bool addBox(CIdentifier& boxID, const IBox& box, const CIdentifier& suggestedBoxID) override;
			bool addBox(CIdentifier& boxID, const CIdentifier& boxAlgorithmID, const CIdentifier& suggestedBoxID) override;
			bool addBox(CIdentifier& boxID, const Plugins::IBoxAlgorithmDesc& boxAlgorithmDesc, const CIdentifier& suggestedBoxID) override;
			bool removeBox(const CIdentifier& boxID) override;

			CIdentifier getNextCommentIdentifier(const CIdentifier& previousID) const override;
			bool isComment(const CIdentifier& commentID) const override;
			const IComment* getCommentDetails(const CIdentifier& commentID) const override;
			IComment* getCommentDetails(const CIdentifier& commentID) override;
			bool addComment(CIdentifier& commentID, const CIdentifier& suggestedCommentID) override;
			bool addComment(CIdentifier& commentID, const IComment& comment, const CIdentifier& suggestedCommentID) override;
			bool removeComment(const CIdentifier& commentID) override;

			CIdentifier getNextMetadataIdentifier(const CIdentifier& previousID) const override;
			bool isMetadata(const CIdentifier& metadataID) const override;
			const IMetadata* getMetadataDetails(const CIdentifier& metadataID) const override;
			IMetadata* getMetadataDetails(const CIdentifier& metadataID) override;
			bool addMetadata(CIdentifier& metadataID, const CIdentifier& suggestedMetadataID) override;
			bool removeMetadata(const CIdentifier& metadataID) override;

			CIdentifier getNextLinkIdentifier(const CIdentifier& previousID) const override;

			CIdentifier getNextLinkIdentifierFromBox(const CIdentifier& previousID, const CIdentifier& boxID) const override;
			CIdentifier getNextLinkIdentifierFromBoxOutput(const CIdentifier& previousID, const CIdentifier& boxID, uint32_t index) const override;
			CIdentifier getNextLinkIdentifierToBox(const CIdentifier& previousID, const CIdentifier& boxID) const override;
			CIdentifier getNextLinkIdentifierToBoxInput(const CIdentifier& previousID, const CIdentifier& boxID, uint32_t index) const override;
			bool isLink(const CIdentifier& boxID) const override;

			bool setHasIO(bool hasIO) override;
			bool hasIO() const override;
			bool setScenarioInputLink(uint32_t scenarioInputIdx, const CIdentifier& boxID, uint32_t boxInputIdx) override;
			bool setScenarioInputLink(uint32_t scenarioInputIdx, const CIdentifier& boxID, const CIdentifier& boxInputID) override;
			bool setScenarioOutputLink(uint32_t scenarioOutputIdx, const CIdentifier& boxID, uint32_t boxOutputIdx) override;
			bool setScenarioOutputLink(uint32_t scenarioOutputIdx, const CIdentifier& boxID, const CIdentifier& boxOutputID) override;
			bool getScenarioInputLink(uint32_t scenarioInputIdx, CIdentifier& boxID, uint32_t& boxInputIdx) const override;
			bool getScenarioInputLink(uint32_t scenarioInputIdx, CIdentifier& boxID, CIdentifier& boxOutputID) const override;
			bool getScenarioOutputLink(uint32_t scenarioOutputIdx, CIdentifier& boxID, uint32_t& boxOutputIdx) const override;
			bool getScenarioOutputLink(uint32_t scenarioOutputIdx, CIdentifier& boxID, CIdentifier& boxOutputID) const override;
			bool removeScenarioInputLink(uint32_t scenarioInputIdx, const CIdentifier& boxID, uint32_t boxInputIdx) override;
			bool removeScenarioOutputLink(uint32_t scenarioOutputIdx, const CIdentifier& boxID, uint32_t boxOutputIdx) override;

			bool removeScenarioInput(uint32_t index) override;
			bool removeScenarioOutput(uint32_t index) override;

			const ILink* getLinkDetails(const CIdentifier& linkID) const override;
			ILink* getLinkDetails(const CIdentifier& linkID) override;

			bool connect(CIdentifier& linkID, const CIdentifier& srcBoxID, uint32_t srcBoxOutputIdx,
						 const CIdentifier& dstBoxID, uint32_t dstBoxInputIdx, const CIdentifier& suggestedLinkID) override;
			bool connect(CIdentifier& linkID, const CIdentifier& srcBoxID, const CIdentifier& srcBoxOutputID,
						 const CIdentifier& dstBoxID, const CIdentifier& dstBoxInputID, const CIdentifier& suggestedLinkID) override;
			bool disconnect(const CIdentifier& srcBoxID, uint32_t srcBoxOutputIdx, const CIdentifier& dstBoxID, uint32_t dstBoxInputIdx) override;
			bool disconnect(const CIdentifier& srcBoxID, const CIdentifier& srcBoxOutputID, const CIdentifier& dstBoxID,
							const CIdentifier& dstBoxInputID) override;
			bool disconnect(const CIdentifier& linkID) override;

			bool getSourceBoxOutputIndex(const CIdentifier& srcBoxID, const CIdentifier& srcBoxOutputID, uint32_t& srcBoxOutputIdx) override;

			bool getTargetBoxInputIndex(const CIdentifier& dstBoxID, const CIdentifier& dstBoxInputID, uint32_t& dstBoxInputIdx) override;

			bool getSourceBoxOutputIdentifier(const CIdentifier& srcBoxID, const uint32_t& srcBoxOutputIdx, CIdentifier& srcBoxOutputID) override;

			bool getTargetBoxInputIdentifier(const CIdentifier& dstBoxID, const uint32_t& dstBoxInputIdx, CIdentifier& dstBoxInputID) override;

			bool applyLocalSettings() override;
			bool checkSettings(IConfigurationManager* configurationManager) override;

			bool isBoxOutdated(const CIdentifier& boxId);
			bool checkOutdatedBoxes() override;
			bool hasOutdatedBox() override;

			CIdentifier getNextOutdatedBoxIdentifier(const CIdentifier& previousID) const override;

			bool isMetabox() override;

			void getBoxIdentifierList(CIdentifier** listID, size_t* size) const override;
			void getCommentIdentifierList(CIdentifier** listID, size_t* size) const override;
			void getMetadataIdentifierList(CIdentifier** listID, size_t* size) const override;
			void getLinkIdentifierList(CIdentifier** listID, size_t* size) const override;
			void getLinkIdentifierFromBoxList(const CIdentifier& boxID, CIdentifier** listID, size_t* size) const override;
			void getLinkIdentifierFromBoxOutputList(const CIdentifier& boxID, uint32_t index, CIdentifier** listID, size_t* size) const override;
			void getLinkIdentifierToBoxList(const CIdentifier& boxID, CIdentifier** listID, size_t* size) const override;
			void getLinkIdentifierToBoxInputList(const CIdentifier& boxID, uint32_t index, CIdentifier** listID, size_t* size) const override;
			void getOutdatedBoxIdentifierList(CIdentifier** listID, size_t* size) const override;
			void releaseIdentifierList(CIdentifier* listID) const override;


			bool acceptVisitor(IObjectVisitor& objectVisitor) override;

			bool updateBox(const CIdentifier& boxID) override;

			bool containsBoxWithDeprecatedInterfacors() const override;

			bool removeDeprecatedInterfacorsFromBox(const CIdentifier& boxID) override;

			_IsDerivedFromClass_Final_(TBox< OpenViBE::Kernel::IScenario >, OVK_ClassId_Kernel_Scenario_Scenario)

		private:
			CIdentifier getUnusedIdentifier(const CIdentifier& suggestedID) const;

			std::map<CIdentifier, CBox*> m_Boxes;
			std::map<CIdentifier, CComment*> m_Comments;
			std::map<CIdentifier, CMetadata*> m_Metadata;
			std::map<CIdentifier, CLink*> m_Links;
			std::map<CIdentifier, std::shared_ptr<CBox>> m_OutdatedBoxes;
			std::map<EBoxInterfacorType, std::map<CIdentifier, std::map<uint32_t, uint32_t>>> m_UpdatedBoxIOCorrespondence;


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
