#pragma once

#include "../ovkTKernelObject.h"

#include "ovkTAttributable.h"
#include "ovkTBox.hpp"

#include <vector>
#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		typedef TBox< OpenViBE::Kernel::IBox > CBox;
		class CComment;
		class CMetadata;
		class CLink;

		class CScenario final : public TBox< OpenViBE::Kernel::IScenario >
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
			OpenViBE::CIdentifier getNextLinkIdentifierFromBoxOutput(const OpenViBE::CIdentifier& previousIdentifier, const OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::uint32 outputIndex) const;
			OpenViBE::CIdentifier getNextLinkIdentifierToBox(const OpenViBE::CIdentifier& previousIdentifier, const OpenViBE::CIdentifier& boxIdentifier) const;
			OpenViBE::CIdentifier getNextLinkIdentifierToBoxInput(const OpenViBE::CIdentifier& previousIdentifier, const OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::uint32 inputInex) const;
			bool isLink(const OpenViBE::CIdentifier& boxIdentifier) const;

			bool setHasIO(const bool hasIO);
			bool hasIO() const;
			bool setScenarioInputLink(const OpenViBE::uint32 scenarioInputIndex, const OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::uint32 boxInputIndex);
			bool setScenarioOutputLink(const OpenViBE::uint32 scenarioOutputIndex, const OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::uint32 boxOutputIndex);
			bool getScenarioInputLink(const OpenViBE::uint32 scenarioInputIndex, OpenViBE::CIdentifier& boxIdentifier, OpenViBE::uint32& boxInputIndex) const;
			bool getScenarioOutputLink(const OpenViBE::uint32 scenarioOutputIndex, OpenViBE::CIdentifier& boxIdentifier, OpenViBE::uint32& boxOutputIndex) const;
			bool removeScenarioInputLink(const OpenViBE::uint32 scenarioInputIndex, const OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::uint32 boxInputIndex);
			bool removeScenarioOutputLink(const OpenViBE::uint32 scenarioOutputIndex, const OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::uint32 boxOutputIndex);

			bool removeScenarioInput(const uint32 inputIndex);
			bool removeScenarioOutput(const uint32 outputIndex);

			const OpenViBE::Kernel::ILink* getLinkDetails(const OpenViBE::CIdentifier& linkIdentifier) const;
			OpenViBE::Kernel::ILink* getLinkDetails(const OpenViBE::CIdentifier& linkIdentifier);

			bool connect(
				OpenViBE::CIdentifier& linkIdentifier,
				const OpenViBE::CIdentifier& sourceBoxIdentifier,
				const OpenViBE::uint32 sourceBoxOutputIndex,
				const OpenViBE::CIdentifier& targetBoxIdentifier,
				const OpenViBE::uint32 targetBoxInputIndex,
				const OpenViBE::CIdentifier& suggestedLinkIdentifier);
			bool disconnect(
				const OpenViBE::CIdentifier& sourceBoxIdentifier,
				const OpenViBE::uint32 sourceBoxOutputIndex,
				const OpenViBE::CIdentifier& targetBoxIdentifier,
				const OpenViBE::uint32 targetBoxInputIndex);
			bool disconnect(const OpenViBE::CIdentifier& linkIdentifier);

			bool applyLocalSettings(void);
			bool checkSettings(IConfigurationManager* configurationManager);

			bool checkNeedsUpdateBox();
			bool hasNeedsUpdateBox();
			OpenViBE::CIdentifier getNextNeedsUpdateBoxIdentifier(const OpenViBE::CIdentifier& previousIdentifier) const;

			bool isMetabox(void);

			bool acceptVisitor(OpenViBE::IObjectVisitor& objectVisitor);

			_IsDerivedFromClass_Final_(TBox< OpenViBE::Kernel::IScenario >, OVK_ClassId_Kernel_Scenario_Scenario)

		private:
			OpenViBE::CIdentifier getUnusedIdentifier(const CIdentifier& suggestedIdentifier) const;

			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CBox*> m_Boxes;
			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CComment*> m_Comments;
			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CMetadata*> m_Metadata;
			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CLink*> m_Links;
			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CBox*> m_BoxesWhichNeedUpdate;

			bool m_HasIO;

			mutable std::vector<std::pair<OpenViBE::CIdentifier, OpenViBE::uint32> > m_ScenarioInputLinks;
			mutable std::vector<std::pair<OpenViBE::CIdentifier, OpenViBE::uint32> > m_ScenarioOutputLinks;

			// Helper members. These are used for quick lookup of next identifiers for the purpose
			// of the getNextMetadataIdentifier function.
			std::map<OpenViBE::CIdentifier, OpenViBE::CIdentifier> m_NextMetadataIdentifier;
			OpenViBE::CIdentifier m_FirstMetadataIdentifier;

		};
	}
}

