#ifndef __OpenViBEKernel_Kernel_Scenario_CScenario_H__
#define __OpenViBEKernel_Kernel_Scenario_CScenario_H__

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
		class CLink;

		class CScenario : public TBox< OpenViBE::Kernel::IScenario >
		{
		public:

			CScenario(const OpenViBE::Kernel::IKernelContext& rKernelContext, const OpenViBE::CIdentifier& rIdentifier);
			virtual ~CScenario(void);

			virtual OpenViBE::boolean clear(void);
			virtual OpenViBE::boolean merge(
			        const OpenViBE::Kernel::IScenario& rScenario,
			        OpenViBE::Kernel::IScenario::IScenarioMergeCallback* pScenarioMergeCallback,
			        OpenViBE::boolean bMergeSettings);

			virtual OpenViBE::boolean setVisualisationTreeIdentifier(
				const OpenViBE::CIdentifier& rVisualisationTreeIdentifier);
			virtual OpenViBE::CIdentifier getVisualisationTreeIdentifier(void) const;
			virtual const OpenViBE::Kernel::IVisualisationTree& getVisualisationTreeDetails() const;
			virtual OpenViBE::Kernel::IVisualisationTree& getVisualisationTreeDetails();

			virtual OpenViBE::CIdentifier getNextBoxIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const;
			virtual OpenViBE::boolean isBox(
				const OpenViBE::CIdentifier& rBoxIdentifier) const;
			virtual const OpenViBE::Kernel::IBox* getBoxDetails(
				const OpenViBE::CIdentifier& rBoxIdentifier) const;
			virtual OpenViBE::Kernel::IBox* getBoxDetails(
				const OpenViBE::CIdentifier& rBoxIdentifier);
			virtual OpenViBE::boolean addBox(
				OpenViBE::CIdentifier& rBoxIdentifier,
				const OpenViBE::CIdentifier& rSuggestedBoxIdentifier);
			virtual OpenViBE::boolean addBox(
				OpenViBE::CIdentifier& rBoxIdentifier,
				const OpenViBE::Kernel::IBox& rBox,
				const OpenViBE::CIdentifier& rSuggestedBoxIdentifier);
			virtual OpenViBE::boolean addBox(
				OpenViBE::CIdentifier& rBoxIdentifier,
				const OpenViBE::CIdentifier& rBoxAlgorithmIdentifier,
				const OpenViBE::CIdentifier& rSuggestedBoxIdentifier);
			virtual OpenViBE::boolean addBox(
			        OpenViBE::CIdentifier& rBoxIdentifier,
			        const OpenViBE::Plugins::IBoxAlgorithmDesc& rBoxAlgorithmDesc,
			        const OpenViBE::CIdentifier& rSuggestedBoxIdentifier);
			virtual OpenViBE::boolean removeBox(
				const OpenViBE::CIdentifier& rBoxIdentifier);

			virtual OpenViBE::CIdentifier getNextCommentIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const;
			virtual OpenViBE::boolean isComment(
				const OpenViBE::CIdentifier& rCommentIdentifier) const;
			virtual const OpenViBE::Kernel::IComment* getCommentDetails(
				const OpenViBE::CIdentifier& rCommentIdentifier) const;
			virtual OpenViBE::Kernel::IComment* getCommentDetails(
				const OpenViBE::CIdentifier& rCommentIdentifier);
			virtual OpenViBE::boolean addComment(
				OpenViBE::CIdentifier& rCommentIdentifier,
				const OpenViBE::CIdentifier& rSuggestedCommentIdentifier);
			virtual OpenViBE::boolean addComment(
				OpenViBE::CIdentifier& rCommentIdentifier,
				const OpenViBE::Kernel::IComment& rComment,
				const OpenViBE::CIdentifier& rSuggestedCommentIdentifier);
			virtual OpenViBE::boolean removeComment(
				const OpenViBE::CIdentifier& rCommentIdentifier);

			virtual OpenViBE::CIdentifier getNextLinkIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const;

			virtual OpenViBE::CIdentifier getNextLinkIdentifierFromBox(
				const OpenViBE::CIdentifier& rPreviousIdentifier,
				const OpenViBE::CIdentifier& rBoxIdentifier) const;
			virtual OpenViBE::CIdentifier getNextLinkIdentifierFromBoxOutput(
				const OpenViBE::CIdentifier& rPreviousIdentifier,
				const OpenViBE::CIdentifier& rBoxIdentifier,
				const OpenViBE::uint32 ui32OutputIndex) const;
			virtual OpenViBE::CIdentifier getNextLinkIdentifierToBox(
				const OpenViBE::CIdentifier& rPreviousIdentifier,
				const OpenViBE::CIdentifier& rBoxIdentifier) const;
			virtual OpenViBE::CIdentifier getNextLinkIdentifierToBoxInput(
				const OpenViBE::CIdentifier& rPreviousIdentifier,
				const OpenViBE::CIdentifier& rBoxIdentifier,
				const OpenViBE::uint32 ui32InputInex) const;
			virtual OpenViBE::boolean isLink(
				const OpenViBE::CIdentifier& rBoxIdentifier) const;

			virtual OpenViBE::boolean setHasIO(const OpenViBE::boolean bHasIO);
			virtual OpenViBE::boolean hasIO() const;
			virtual OpenViBE::boolean setScenarioInputLink(
			        const OpenViBE::uint32 ui32ScenarioInputIndex,
			        const OpenViBE::CIdentifier& rBoxIdentifier,
			        const OpenViBE::uint32 ui32BoxInputIndex);
			virtual OpenViBE::boolean setScenarioOutputLink(
			        const OpenViBE::uint32 ui32ScenarioOutputIndex,
			        const OpenViBE::CIdentifier& rBoxIdentifier,
			        const OpenViBE::uint32 ui32BoxOutputIndex);
			virtual OpenViBE::boolean getScenarioInputLink(
			        const OpenViBE::uint32 ui32ScenarioInputIndex,
			        OpenViBE::CIdentifier& rBoxIdentifier,
			        OpenViBE::uint32& rBoxInputIndex) const;
			virtual OpenViBE::boolean getScenarioOutputLink(
			        const OpenViBE::uint32 ui32ScenarioOutputIndex,
			        OpenViBE::CIdentifier& rBoxIdentifier,
			        OpenViBE::uint32& rBoxOutputIndex) const;
			virtual OpenViBE::boolean removeScenarioInputLink(
			        const OpenViBE::uint32 ui32ScenarioInputIndex,
			        const OpenViBE::CIdentifier& rBoxIdentifier,
			        const OpenViBE::uint32 ui32BoxInputIndex);
			virtual OpenViBE::boolean removeScenarioOutputLink(
			        const OpenViBE::uint32 ui32ScenarioOutputIndex,
			        const OpenViBE::CIdentifier& rBoxIdentifier,
			        const OpenViBE::uint32 ui32BoxOutputIndex);

			virtual OpenViBE::boolean removeScenarioInput(const uint32 ui32InputIndex);
			virtual OpenViBE::boolean removeScenarioOutput(const uint32 ui32OutputIndex);

			virtual const OpenViBE::Kernel::ILink* getLinkDetails(
				const OpenViBE::CIdentifier& rLinkIdentifier) const;
			virtual OpenViBE::Kernel::ILink* getLinkDetails(
				const OpenViBE::CIdentifier& rLinkIdentifier);

			virtual OpenViBE::boolean connect(
				OpenViBE::CIdentifier& rLinkIdentifier,
				const OpenViBE::CIdentifier& rSourceBoxIdentifier,
				const OpenViBE::uint32 ui32SourceBoxOutputIndex,
				const OpenViBE::CIdentifier& rTargetBoxIdentifier,
				const OpenViBE::uint32 ui32TargetBoxInputIndex,
				const OpenViBE::CIdentifier& rSuggestedLinkIdentifier);
			virtual OpenViBE::boolean disconnect(
				const OpenViBE::CIdentifier& rSourceBoxIdentifier,
				const OpenViBE::uint32 ui32SourceBoxOutputIndex,
				const OpenViBE::CIdentifier& rTargetBoxIdentifier,
				const OpenViBE::uint32 ui32TargetBoxInputIndex);
			virtual OpenViBE::boolean disconnect(
				const OpenViBE::CIdentifier& rLinkIdentifier);

			virtual OpenViBE::boolean applyLocalSettings(void);
			virtual OpenViBE::boolean checkSettings(IConfigurationManager* pConfig);

			virtual OpenViBE::boolean checkNeedsUpdateBox();
			virtual OpenViBE::boolean hasNeedsUpdateBox();
			virtual OpenViBE::CIdentifier getNextNeedsUpdateBoxIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const;

			virtual OpenViBE::boolean isMetabox(void);

			virtual OpenViBE::boolean acceptVisitor(
				OpenViBE::IObjectVisitor& rObjectVisitor);

			_IsDerivedFromClass_Final_(TBox< OpenViBE::Kernel::IScenario >, OVK_ClassId_Kernel_Scenario_Scenario)

		protected:

			virtual OpenViBE::CIdentifier getUnusedIdentifier(const CIdentifier& rSuggestedIdentifier) const;

		protected:

			OpenViBE::CIdentifier m_oIdentifier;
			OpenViBE::CIdentifier m_oVisualisationTreeIdentifier;
			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CBox*> m_vBox;
			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CComment*> m_vComment;
			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CLink*> m_vLink;
			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CBox*> m_vNeedsUpdatesBoxes;

			OpenViBE::boolean m_bHasIO;

			mutable std::vector<std::pair<OpenViBE::CIdentifier, OpenViBE::uint32> > m_vScenarioInputLink;
			mutable std::vector<std::pair<OpenViBE::CIdentifier, OpenViBE::uint32> > m_vScenarioOutputLink;
		};
	}
}

#endif // __OpenViBEKernel_Kernel_Scenario_CScenario_H__
