#ifndef __OpenViBE_Kernel_Scenario_IScenario_H__
#define __OpenViBE_Kernel_Scenario_IScenario_H__

#include "ovIBox.h"

namespace OpenViBE
{
	class IMemoryBuffer;

	namespace Plugins
	{
		class IBoxAlgorithmDesc;
	}

	namespace Kernel
	{
		class ILink;
		class IComment;
		class IMetadata;
		class IConfigurationManager;

		/**
		 * \class IScenario
		 * \author Yann Renard (IRISA/INRIA)
		 * \date 2006-08-16
		 * \brief A static OpenViBE scenario
		 * \ingroup Group_Scenario
		 * \ingroup Group_Kernel
		 *
		 * This class is a static scenario description.
		 * It is used to manipulate an OpenViBE
		 * box/connection collection...
		 *
		 * \todo Add meta information for this scenario
		 */
		class OV_API IScenario : public OpenViBE::Kernel::IBox
		{
		public:
			class IScenarioMergeCallback
			{
			public:
				virtual void process(OpenViBE::CIdentifier& rOriginalIdentifier, OpenViBE::CIdentifier& rNewIdentifier) = 0;
			};

			/** \name General purpose functions */
			//@{

			/**
			 * \brief Clears the scenario
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean clear(void)=0;

			/**
			 * \brief Merges this scenario with an other existing scenario
			 * \param rScenario A reference to the scenario to merge this scenario with
			 * \param pScenarioCallback A callback that will be called for each merged element and will be passed the old and new identifiers
			 * \param bMergeSettings When true, the settings from the source scenario will also be merged
			 * \param bPreserveIdentifiers When true, the element identifiers from the source scenario will be preserved.
			 * \retval true In case of success
			 * \retval false In case of error
			 * \note The \c bPreservedIdentifiers setting is only reliable when the destination scenario is empty.
			 */
			virtual OpenViBE::boolean merge(
			        const OpenViBE::Kernel::IScenario& rScenario,
			        OpenViBE::Kernel::IScenario::IScenarioMergeCallback* pScenarioMergeCallback,
			        OpenViBE::boolean bMergeSettings,
			        OpenViBE::boolean bPreserveIdentifiers) = 0;

			//@}
			/** \name Box management */
			//@{

			/**
			 * \brief Gets next box identifier
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding box
			 * \return The identifier of the next box in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first box
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextBoxIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const=0;
			/**
			 * \brief Tests whether a given identifier is a box or not
			 * \param rBoxIdentifier [in] : the identifier to test
			 * \return \e true if the identified object is a box
			 * \return \e false if the identified object is not a box
			 * \note Requesting a bad identifier returns \e false
			 */
			virtual OpenViBE::boolean isBox(
				const OpenViBE::CIdentifier& rBoxIdentifier) const=0;
			/**
			 * \brief Gets the details for a specific box
			 * \param rBoxIdentifier [in] : The identifier
			 *        of the box which details should be
			 *        sent.
			 * \return The box details
			 */
			virtual const OpenViBE::Kernel::IBox* getBoxDetails(
				const OpenViBE::CIdentifier& rBoxIdentifier) const=0;
			/// \copydoc getBoxDetails(const OpenViBE::CIdentifier&)const
			virtual OpenViBE::Kernel::IBox* getBoxDetails(
				const OpenViBE::CIdentifier& rBoxIdentifier)=0;
			/**
			 * \brief Adds a new box in the scenario
			 * \param rBoxIdentifier [out] : The identifier of the created box
			 * \param rSuggestedBoxIdentifier [in] : a suggestion for the new
			 *        box identifier. If this specific identifier is not
			 *        yet used, this scenario might use it. If the identifier
			 *        is already used or \c OV_UndefinedIdentifier is passed,
			 *        then a random unused identifier will be used.
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rBoxIdentifier remains unchanged.
			 * \note This produces an empty and unconfigured box !
			 */
			virtual OpenViBE::boolean addBox(
				OpenViBE::CIdentifier& rBoxIdentifier,
				const OpenViBE::CIdentifier& rSuggestedBoxIdentifier)=0;
			/**
			 * \brief Adds a new box in the scenario based on an existing box
			 * \param rBoxIdentifier [out] : The identifier of the created box
			 * \param rBox [in] : the box to copy in this scenario
			 * \param rSuggestedBoxIdentifier [in] : a suggestion for the new
			 *        box identifier. If this specific identifier is not
			 *        yet used, this scenario might use it. If the identifier
			 *        is already used or \c OV_UndefinedIdentifier is passed,
			 *        then a random unused identifier will be used.
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rBoxIdentifier remains unchanged.
			 */
			virtual OpenViBE::boolean addBox(
				OpenViBE::CIdentifier& rBoxIdentifier,
				const OpenViBE::Kernel::IBox& rBox,
				const OpenViBE::CIdentifier& rSuggestedBoxIdentifier)=0;
			/**
			 * \brief Adds a new box in the scenario
			 * \param rBoxIdentifier [out] : The identifier of the created box
			 * \param rBoxAlgorithmClassIdentifier [in] : The
			 *        class identifier of the algorithm for
			 *        this box
			 * \param rSuggestedBoxIdentifier [in] : a suggestion for the new
			 *        box identifier. If this specific identifier is not
			 *        yet used, this scenario might use it. If the identifier
			 *        is already used or \c OV_UndefinedIdentifier is passed,
			 *        then a random unused identifier will be used.
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rBoxIdentifier remains unchanged.
			 * \note This function prepares the box according
			 *       to its algorithm class identifier !
			 */
			virtual OpenViBE::boolean addBox(
				OpenViBE::CIdentifier& rBoxIdentifier,
				const OpenViBE::CIdentifier& rBoxAlgorithmClassIdentifier,
				const OpenViBE::CIdentifier& rSuggestedBoxIdentifier)=0;

			virtual OpenViBE::boolean addBox(
			        OpenViBE::CIdentifier& rBoxIdentifier,
			        const OpenViBE::Plugins::IBoxAlgorithmDesc& rBoxAlgorithmDesc,
			        const OpenViBE::CIdentifier& rSuggestedBoxIdentifier)=0;
			/**
			 * \brief Removes a box of the scenario
			 * \param rBoxIdentifier [in] : The box identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \note Each link related to this box is also removed
			 */
			virtual OpenViBE::boolean removeBox(
				const OpenViBE::CIdentifier& rBoxIdentifier)=0;

			//@}
			/** \name Connection management */
			//@{

			/**
			 * \brief Gets next link identifier
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding link
			 * \return The identifier of the next link in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first link
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextLinkIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const=0;

			/**
			 * \brief Gets next link identifier from fixed box
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding link
			 * \param rBoxIdentifier [in] : The box identifier
			 *        which the link should end to
			 * \return The identifier of the next link in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first link
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextLinkIdentifierFromBox(
				const OpenViBE::CIdentifier& rPreviousIdentifier,
				const OpenViBE::CIdentifier& rBoxIdentifier) const=0;

			/**
			 * \brief Gets next link identifier from fixed box output
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding link
			 * \param rBoxIdentifier [in] : The box identifier
			 *        which the link should end to
			 * \param ui32OutputIndex [in] : The input index
			 *        which the link should end to
			 * \return The identifier of the next link in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first link
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextLinkIdentifierFromBoxOutput(
				const OpenViBE::CIdentifier& rPreviousIdentifier,
				const OpenViBE::CIdentifier& rBoxIdentifier,
				const OpenViBE::uint32 ui32OutputIndex) const=0;

			/**
			 * \brief Gets next link identifier from fixed box
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding link
			 * \param rBoxIdentifier [in] : The box identifier
			 *        which the link should start from
			 * \return The identifier of the next link in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first link
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextLinkIdentifierToBox(
				const OpenViBE::CIdentifier& rPreviousIdentifier,
				const OpenViBE::CIdentifier& rBoxIdentifier) const=0;

			/**
			 * \brief Gets next link identifier from fixed box input
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding link
			 * \param rBoxIdentifier [in] : The box identifier
			 *        which the link should start from
			 * \param ui32InputInex [in] : The input index
			 *        which the link should start from
			 * \return The identifier of the next link in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first link
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextLinkIdentifierToBoxInput(
				const OpenViBE::CIdentifier& rPreviousIdentifier,
				const OpenViBE::CIdentifier& rBoxIdentifier,
				const OpenViBE::uint32 ui32InputInex) const=0;

			/**
			 * \brief Tests whether a given identifier is a link or not
			 * \param rIdentifier [in] : the identifier to test
			 * \return \e true if the identified object is a link
			 * \return \e false if the identified object is not a link
			 * \note Requesting a bad identifier returns \e false
			 */
			virtual OpenViBE::boolean isLink(
				const OpenViBE::CIdentifier& rIdentifier) const=0;

			/**
			 * \brief Gets the details for a specific link
			 * \param rLinkIdentifier [in] : The identifier
			 *        of the link which details should be
			 *        sent.
			 * \return The link details
			 */
			virtual const OpenViBE::Kernel::ILink* getLinkDetails(
				const OpenViBE::CIdentifier& rLinkIdentifier) const=0;
			/// \copydoc getLinkDetails(const OpenViBE::CIdentifier&)const
			virtual OpenViBE::Kernel::ILink* getLinkDetails(
				const OpenViBE::CIdentifier& rLinkIdentifier)=0;

			/**
			 * \brief Creates a connection between two boxes
			 * \param rLinkIdentifier [out] : The created link identifier.
			 * \param rSourceBoxIdentifier [in] : The source box identifier
			 * \param ui32SourceBoxOutputIndex [in] : The output index for the given source box
			 * \param rTargetBoxIdentifier [in] : The target box identifier
			 * \param ui32TargetBoxInputIndex [in] : The input index for the given target box
			 * \param rSuggestedLinkIdentifier [in] : a suggestion for the new
			 *        link identifier. If this specific identifier is not
			 *        yet used, this scenario might use it. If the identifier
			 *        is already used or \c OV_UndefinedIdentifier is passed,
			 *        then a random unused identifier will be used.
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         rLinkIdentifier remains unchanged.
			 */
			virtual OpenViBE::boolean connect(
				OpenViBE::CIdentifier& rLinkIdentifier,
				const OpenViBE::CIdentifier& rSourceBoxIdentifier,
				const OpenViBE::uint32 ui32SourceBoxOutputIndex,
				const OpenViBE::CIdentifier& rTargetBoxIdentifier,
				const OpenViBE::uint32 ui32TargetBoxInputIndex,
				const OpenViBE::CIdentifier& rSuggestedLinkIdentifier)=0;

			/**
			 * \brief Deletes a connection between two boxes
			 * \param rSourceBoxIdentifier [in] : The source box identifier
			 * \param ui32SourceBoxOutputIndex [in] : The output index for the given source box
			 * \param rTargetBoxIdentifier [in] : The target box identifier
			 * \param ui32TargetBoxInputIndex [in] : The input index for the given target box
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean disconnect(
				const OpenViBE::CIdentifier& rSourceBoxIdentifier,
				const OpenViBE::uint32 ui32SourceBoxOutputIndex,
				const OpenViBE::CIdentifier& rTargetBoxIdentifier,
				const OpenViBE::uint32 ui32TargetBoxInputIndex)=0;

			/**
			 * \brief Deletes a connection between two boxes
			 * \param rLinkIdentifier [out] : The identifier for the link to be deleted
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean disconnect(
				const OpenViBE::CIdentifier& rLinkIdentifier)=0;

			//@}
			/** \name Scenario Input/Output and MetaBox management */
			//@{

			virtual OpenViBE::boolean setHasIO(const OpenViBE::boolean bHasIO) = 0;
			virtual OpenViBE::boolean hasIO() const = 0;

			virtual OpenViBE::boolean setScenarioInputLink(
			        const OpenViBE::uint32 ui32ScenarioInputIndex,
			        const OpenViBE::CIdentifier& rBoxIdentifier,
			        const OpenViBE::uint32 ui32BoxInputIndex) = 0;

			virtual OpenViBE::boolean setScenarioOutputLink(
			        const OpenViBE::uint32 ui32ScenarioOutputIndex,
			        const OpenViBE::CIdentifier& rBoxIdentifier,
			        const OpenViBE::uint32 ui32BoxOutputIndex) = 0;

			virtual OpenViBE::boolean getScenarioInputLink(
			        const OpenViBE::uint32 ui32ScenarioInputIndex,
			        OpenViBE::CIdentifier& rBoxIdentifier,
			        OpenViBE::uint32& rBoxInputIndex) const = 0;

			virtual OpenViBE::boolean getScenarioOutputLink(
			        const OpenViBE::uint32 ui32ScenarioOutputIndex,
			        OpenViBE::CIdentifier& rBoxIdentifier,
			        OpenViBE::uint32& rBoxOutputIndex) const = 0;

			virtual OpenViBE::boolean removeScenarioInputLink(
			        const OpenViBE::uint32 ui32ScenarioInputIndex,
			        const OpenViBE::CIdentifier& rBoxIdentifier,
			        const OpenViBE::uint32 ui32BoxInputIndex) = 0;

			virtual OpenViBE::boolean removeScenarioOutputLink(
			        const OpenViBE::uint32 ui32ScenarioOutputIndex,
			        const OpenViBE::CIdentifier& rBoxIdentifier,
			        const OpenViBE::uint32 ui32BoxOutputIndex) = 0;

			virtual OpenViBE::boolean removeScenarioInput(const uint32 ui32InputIndex) = 0;
			virtual OpenViBE::boolean removeScenarioOutput(const uint32 ui32OutputIndex) = 0;

			//@}
			/** \name Comment management */
			//@{

			/**
			 * \brief Gets next comment identifier
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding comment
			 * \return The identifier of the next comment in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first comment
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextCommentIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const=0;
			/**
			 * \brief Tests whether a given identifier is a comment or not
			 * \param rCommentIdentifier [in] : the identifier to test
			 * \return \e true if the identified object is a comment
			 * \return \e false if the identified object is not a comment
			 * \note Requesting a bad identifier returns \e false
			 */
			virtual OpenViBE::boolean isComment(
				const OpenViBE::CIdentifier& rCommentIdentifier) const=0;
			/**
			 * \brief Gets the details for a specific comment
			 * \param rCommentIdentifier [in] : The identifier
			 *        of the comment which details should be
			 *        sent.
			 * \return The comment details
			 */
			virtual const OpenViBE::Kernel::IComment* getCommentDetails(
				const OpenViBE::CIdentifier& rCommentIdentifier) const=0;
			/// \copydoc getCommentDetails(const OpenViBE::CIdentifier&)const
			virtual OpenViBE::Kernel::IComment* getCommentDetails(
				const OpenViBE::CIdentifier& rCommentIdentifier)=0;
			/**
			 * \brief Adds a new comment in the scenario
			 * \param rCommentIdentifier [out] : The identifier of the created comment
			 * \param rSuggestedCommentIdentifier [in] : a suggestion for the new
			 *        comment identifier. If this specific identifier is not
			 *        yet used, this scenario might use it. If the identifier
			 *        is already used or \c OV_UndefinedIdentifier is passed,
			 *        then a random unused identifier will be used.
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rCommentIdentifier remains unchanged.
			 * \note This produces an empty and unconfigured comment !
			 */
			virtual OpenViBE::boolean addComment(
				OpenViBE::CIdentifier& rCommentIdentifier,
				const OpenViBE::CIdentifier& rSuggestedCommentIdentifier)=0;
			/**
			 * \brief Adds a new comment in the scenario based on an existing comment
			 * \param rCommentIdentifier [out] : The identifier of the created comment
			 * \param rComment [in] : the comment to copy in this scenario
			 * \param rSuggestedCommentIdentifier [in] : a suggestion for the new
			 *        comment identifier. If this specific identifier is not
			 *        yet used, this scenario might use it. If the identifier
			 *        is already used or \c OV_UndefinedIdentifier is passed,
			 *        then a random unused identifier will be used.
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rCommentIdentifier remains unchanged.
			 */
			virtual OpenViBE::boolean addComment(
				OpenViBE::CIdentifier& rCommentIdentifier,
				const OpenViBE::Kernel::IComment& rComment,
				const OpenViBE::CIdentifier& rSuggestedCommentIdentifier)=0;
			/**
			 * \brief Removes a comment of the scenario
			 * \param rCommentIdentifier [in] : The comment identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean removeComment(
				const OpenViBE::CIdentifier& rCommentIdentifier)=0;
			
			//@}
			/** \name Metadata management */
			//@{

			/**
			 * \brief Get next metadata identifier in regards to another
			 * \param previousIdentifier The identifier of the metadata
			 * \retval OV_UndefinedIdentifier In case when metadata with the \c previousIdentifier is not present
			 * \retval OV_UndefinedIdentifier In case when metadata with the \c previousIdentifier is last in the scenario
			 * \return The identifier of the next metadata
			 * \note Giving \c OV_UndefinedIdentifier as \c previousIdentifier will cause this function to return the first comment identifier.
			 */
			virtual OpenViBE::CIdentifier getNextMetadataIdentifier(const OpenViBE::CIdentifier& previousIdentifier) const = 0;

			/**
			 * \param commentIdentifier The identifier to test
			 * \retval true If the identified object is metadata
			 * \retval false If the identified object is not metadata or when the identifier is not present in the scenario
			 */
			virtual bool isMetadata(const OpenViBE::CIdentifier& metadataIdentifier) const = 0;


			/**
			 * \param metadataIdentifier The identifier of a metadata
			 * \return Pointer to object containing metadata details
			 */
			virtual const OpenViBE::Kernel::IMetadata* getMetadataDetails(const OpenViBE::CIdentifier& metadataIdentifier) const = 0;

			/// \copydoc getMetadataDetails(const OpenViBE::CIdentifier&)const
			virtual OpenViBE::Kernel::IMetadata* getMetadataDetails(const OpenViBE::CIdentifier& metadataIdentifier) = 0;

			/**
			 * \brief Add new metadata in the scenario
			 * \param[out] metadataIdentifier The identifier of the newly created metadata
			 * \param suggestedMetadataIdentifier A suggestion for the new identifier. If the identifier is already used or \c OV_UndefinedIdentifier is passed,
			 *        then a random unused identifier will be used.
			 * \retval true In case of success.
			 * \retval false In case of error. In this case, \c metadataIdentifier remains unchanged.
			 * \note This method creates an empty metadata.
			 */
			virtual bool addMetadata(OpenViBE::CIdentifier& metadataIdentifier, const OpenViBE::CIdentifier& suggestedMetadataIdentifier) = 0;

			/**
			 * \brief Remove metadata from the scenario
			 * \param metadataIdentifier The comment identifier
			 * \retval true In case of success.
			 * \retval false In case of error.
			 */
			virtual bool removeMetadata(const OpenViBE::CIdentifier& metadataIdentifier) = 0;
			//@}

			/**
			 * \brief replaces settings of each box by its locally expanded version
			 * only expands the $var{} tokens, it leaves others as is
			 */
			virtual OpenViBE::boolean applyLocalSettings(void) = 0;
			
			/**
			 * \brief Check settings before playing scenario, if the settings are not suitable, stop scenario
			 * and launch a console warning. Only check numeric values in the beginning
			 * \param pConfig: local configuration manager that can contain the definition of local scenario settings
			 */
			virtual OpenViBE::boolean checkSettings(IConfigurationManager* pConfig) = 0;

			/**
			 * \brief Check if boxes in scenario need to be updated. Feed an array with the identifiers
			 * of boxes that need to be updated
			 * \return true if at least one box needs to updated
			 */
			virtual OpenViBE::boolean checkNeedsUpdateBox() = 0;
			
			/**
			* \brief Gets identifier of next box that needs to be updated
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding box that needs updates
			 * \return The identifier of the next box that needs updates in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first processing unit
			 *       identifier.
			 * \note Warning: You need to call at least once the function "checkNeedsUpdateBox", before calling this function
			 */
			virtual OpenViBE::CIdentifier getNextNeedsUpdateBoxIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const = 0;

			/**
			 * \brief Indicates if at least one box in scenario need to be updated.
			 * \return true if at least one box needs to updated
			 * \note Warning: You need to call at least once the function "checkNeedsUpdateBox", before calling this function
			 */
			virtual OpenViBE::boolean hasNeedsUpdateBox() = 0;

			/**
			 * \return true if the scenario is actually a metabox
			 */
			virtual OpenViBE::boolean isMetabox(void) = 0;

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IAttributable, OV_ClassId_Kernel_Scenario_Scenario)
		};
	};
};

#endif // __OpenViBE_Kernel_Scenario_IScenario_H__
