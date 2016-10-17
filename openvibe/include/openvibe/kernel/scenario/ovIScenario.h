#pragma once

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
				virtual void process(OpenViBE::CIdentifier& originalIdentifier, OpenViBE::CIdentifier& newIdentifier) = 0;
			};

			/** \name General purpose functions */
			//@{

			/**
			 * \brief Clears the scenario
			 * \retval true In case of success.
			 * \retval false In case of error.
			 */
			virtual bool clear(void) = 0;

			/**
			 * \brief Merges this scenario with an other existing scenario
			 * \param scenario A reference to the scenario to merge this scenario with
			 * \param scenarioMergaCallback A callback that will be called for each merged element and will be passed the old and new identifiers
			 * \param mergeSettings When true, the settings from the source scenario will also be merged
			 * \param preserveIdentifiers When true, the element identifiers from the source scenario will be preserved.
			 * \retval true In case of success
			 * \retval false In case of error
			 * \note The \c bPreservedIdentifiers setting is only reliable when the destination scenario is empty. As an identifier can only be
			 * preserved when the destination scenario does not contain any elements which use it. In general, this parameter should only be set
			 * to true when cloning a scenario.
			 */
			virtual bool merge(
			        const OpenViBE::Kernel::IScenario& scenario,
			        OpenViBE::Kernel::IScenario::IScenarioMergeCallback* scenarioMergeCallback,
			        bool mergeSettings,
			        bool preserveIdentifiers) = 0;

			//@}
			/** \name Box management */
			//@{

			/**
			 * \brief Gets next box identifier
			 * \param previousIdentifier The identifier
			 *        for the preceeding box
			 * \return The identifier of the next box in case of success.
			 * \retval OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c previousIdentifier
			 *       will cause this function to return the first box
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextBoxIdentifier(const OpenViBE::CIdentifier& previousIdentifier) const = 0;

			/**
			 * \brief Tests whether a given identifier is a box or not
			 * \param boxIdentifier the identifier to test
			 * \retval true if the identified object is a box
			 * \retval false if the identified object is not a box
			 * \note Requesting a bad identifier returns \e false
			 */
			virtual bool isBox(const OpenViBE::CIdentifier& boxIdentifier) const = 0;

			/**
			 * \brief Gets the details for a specific box
			 * \param boxIdentifier The identifier of the box which details should be sent.
			 * \return The box details
			 */
			virtual const OpenViBE::Kernel::IBox* getBoxDetails(const OpenViBE::CIdentifier& boxIdentifier) const = 0;

			/// \copydoc getBoxDetails(const OpenViBE::CIdentifier&)const
			virtual OpenViBE::Kernel::IBox* getBoxDetails(const OpenViBE::CIdentifier& boxIdentifier) = 0;

			/**
			 * \brief Adds a new box in the scenario
			 * \param[out] boxIdentifier The identifier of the created box
			 * \param suggestedBoxIdentifier A suggestion for the new box identifier. If this specific identifier is not
			 *        yet used, this scenario might use it. If the identifier is already used or \c OV_UndefinedIdentifier is passed,
			 *        then a random unused identifier will be used.
			 * \retval true In case of success.
			 * \retval false In case of error. In such case, \c boxIdentifier remains unchanged.
			 * \note This produces an empty and unconfigured box!
			 */
			virtual bool addBox(OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::CIdentifier& suggestedBoxIdentifier) = 0;
			/**
			 * \brief Adds a new box in the scenario based on an existing box
			 * \param[out] boxIdentifier The identifier of the created box
			 * \param box The box to copy in this scenario
			 * \param suggestedBoxIdentifier a suggestion for the new box identifier. If this specific identifier is not
			 *        yet used, this scenario might use it. If the identifier is already used or \c OV_UndefinedIdentifier is passed,
			 *        then a random unused identifier will be used.
			 * \retval true In case of success.
			 * \retval false In case of error. In such case, \c boxIdentifier remains unchanged.
			 */
			virtual bool addBox(OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::Kernel::IBox& box, const OpenViBE::CIdentifier& suggestedBoxIdentifier) = 0;
			/**
			 * \brief Adds a new box in the scenario
			 * \param[out] boxIdentifier The identifier of the created box
			 * \param boxAlgorithmClassIdentifier The class identifier of the algorithm for this box
			 * \param suggestedBoxIdentifier a suggestion for the new box identifier. If this specific identifier is not
			 *        yet used, this scenario might use it. If the identifier is already used or \c OV_UndefinedIdentifier is passed,
			 *        then a random unused identifier will be used.
			 * \retval true In case of success.
			 * \retval false In case of error. In such case, \c boxIdentifier remains unchanged.
			 * \note This function prepares the box according to its algorithm class identifier !
			 */
			virtual bool addBox(OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::CIdentifier& boxAlgorithmClassIdentifier, const OpenViBE::CIdentifier& suggestedBoxIdentifier) = 0;

			// TODO_JL: Doc
			virtual bool addBox(OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::Plugins::IBoxAlgorithmDesc& boxAlgorithmDesc, const OpenViBE::CIdentifier& suggestedBoxIdentifier) = 0;

			/**
			 * \brief Removes a box of the scenario
			 * \param boxIdentifier The box identifier
			 * \retval true In case of success.
			 * \retval false In case of error.
			 * \note Each link related to this box is also removed
			 */
			virtual bool removeBox(const OpenViBE::CIdentifier& boxIdentifier) = 0;

			//@}
			/** \name Connection management */
			//@{

			/**
			 * \brief Gets next link identifier
			 * \param previousIdentifier The identifier for the preceeding link
			 * \return The identifier of the next link in case of success.
			 * \retval OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c previousIdentifier will cause this function to return the first link identifier.
			 */
			virtual OpenViBE::CIdentifier getNextLinkIdentifier(const OpenViBE::CIdentifier& previousIdentifier) const = 0;

			/**
			 * \brief Gets next link identifier from fixed box
			 * \param previousIdentifier The identifier for the preceeding link
			 * \param boxIdentifier The box identifier which the link should end to
			 * \return The identifier of the next link in case of success.
			 * \retval OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c previousIdentifier will cause this function to return the first link identifier.
			 */
			virtual OpenViBE::CIdentifier getNextLinkIdentifierFromBox(const OpenViBE::CIdentifier& previousIdentifier, const OpenViBE::CIdentifier& boxIdentifier) const = 0;

			/**
			 * \brief Gets next link identifier from fixed box output
			 * \param previousIdentifier The identifier for the preceeding link
			 * \param boxIdentifier The box identifier which the link should end to
			 * \param outputIndex The input index which the link should end to
			 * \return The identifier of the next link in case of success.
			 * \retval OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c previousIdentifier will cause this function to return the first link identifier.
			 */
			virtual OpenViBE::CIdentifier getNextLinkIdentifierFromBoxOutput(
				const OpenViBE::CIdentifier& previousIdentifier,
				const OpenViBE::CIdentifier& boxIdentifier,
				const OpenViBE::uint32 outputIndex) const = 0;

			/**
			 * \brief Gets next link identifier from fixed box
			 * \param previousIdentifier The identifier for the preceeding link
			 * \param boxIdentifier The box identifier which the link should start from
			 * \return The identifier of the next link in case of success.
			 * \retval OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c previousIdentifier will cause this function to return the first link identifier.
			 */
			virtual OpenViBE::CIdentifier getNextLinkIdentifierToBox(const OpenViBE::CIdentifier& previousIdentifier, const OpenViBE::CIdentifier& boxIdentifier) const = 0;

			/**
			 * \brief Gets next link identifier from fixed box input
			 * \param previousIdentifier The identifier for the preceeding link
			 * \param boxIdentifier The box identifier which the link should start from
			 * \param inputIndex The input index which the link should start from
			 * \return The identifier of the next link in case of success.
			 * \retval OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c previousIdentifier will cause this function to return the first link identifier.
			 */
			virtual OpenViBE::CIdentifier getNextLinkIdentifierToBoxInput(const OpenViBE::CIdentifier& previousIdentifier, const OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::uint32 inputIndex) const = 0;

			/**
			 * \brief Tests whether a given identifier is a link or not
			 * \param identifier the identifier to test
			 * \retval true if the identified object is a link
			 * \retval false if the identified object is not a link
			 * \note Requesting a bad identifier returns \e false
			 */
			virtual bool isLink(const OpenViBE::CIdentifier& identifier) const = 0;

			/**
			 * \brief Gets the details for a specific link
			 * \param linkIdentifier The identifier of the link which details should be sent.
			 * \return The link details
			 */
			virtual const OpenViBE::Kernel::ILink* getLinkDetails(const OpenViBE::CIdentifier& linkIdentifier) const = 0;

			/// \copydoc getLinkDetails(const OpenViBE::CIdentifier&)const
			virtual OpenViBE::Kernel::ILink* getLinkDetails(const OpenViBE::CIdentifier& linkIdentifier) = 0;

			/**
			 * \brief Creates a connection between two boxes
			 * \param[out] linkIdentifier The created link identifier.
			 * \param sourceBoxIdentifier The source box identifier
			 * \param sourceBoxOutputIndex The output index for the given source box
			 * \param targetBoxIdentifier The target box identifier
			 * \param targetBoxInputIndex The input index for the given target box
			 * \param suggestedLinkIdentifier a suggestion for the new link identifier. If this specific identifier is not
			 *        yet used, this scenario might use it. If the identifier is already used or \c OV_UndefinedIdentifier is passed,
			 *        then a random unused identifier will be used.
			 * \retval true In case of success.
			 * \retval false In case of error. In such case, \c linkIdentifier remains unchanged.
			 */
			virtual bool connect(
				OpenViBE::CIdentifier& linkIdentifier,
				const OpenViBE::CIdentifier& sourceBoxIdentifier,
				const OpenViBE::uint32 sourceBoxOutputIndex,
				const OpenViBE::CIdentifier& targetBoxIdentifier,
				const OpenViBE::uint32 targetBoxInputIndex,
				const OpenViBE::CIdentifier& suggestedLinkIdentifier) = 0;

			/**
			 * \brief Deletes a connection between two boxes
			 * \param sourceBoxIdentifier The source box identifier
			 * \param sourceBoxOutputIndex The output index for the given source box
			 * \param targetBoxIdentifier The target box identifier
			 * \param targetBoxInputIndex The input index for the given target box
			 * \retval true In case of success.
			 * \retval false In case of error.
			 */
			virtual bool disconnect(
				const OpenViBE::CIdentifier& sourceBoxIdentifier,
				const OpenViBE::uint32 sourceBoxOutputIndex,
				const OpenViBE::CIdentifier& targetBoxIdentifier,
				const OpenViBE::uint32 targetBoxInputIndex) = 0;

			/**
			 * \brief Deletes a connection between two boxes
			 * \param[out] linkIdentifier The identifier for the link to be deleted
			 * \retval true In case of success.
			 * \retval false In case of error.
			 */
			virtual bool disconnect(const OpenViBE::CIdentifier& linkIdentifier) = 0;

			//@}
			/** \name Scenario Input/Output and MetaBox management */
			//@{

			virtual bool setHasIO(const bool hasIO) = 0;
			virtual bool hasIO() const = 0;

			virtual bool setScenarioInputLink(const OpenViBE::uint32 scenarioInputIndex, const OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::uint32 boxInputIndex) = 0;

			virtual bool setScenarioOutputLink(const OpenViBE::uint32 scenarioOutputIndex, const OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::uint32 boxOutputIndex) = 0;

			virtual bool getScenarioInputLink(const OpenViBE::uint32 scenarioInputIndex, OpenViBE::CIdentifier& boxIdentifier, OpenViBE::uint32& boxInputIndex) const = 0;

			virtual bool getScenarioOutputLink(const OpenViBE::uint32 scenarioOutputIndex, OpenViBE::CIdentifier& boxIdentifier, OpenViBE::uint32& boxOutputIndex) const = 0;

			virtual bool removeScenarioInputLink(const OpenViBE::uint32 scenarioInputIndex, const OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::uint32 boxInputIndex) = 0;

			virtual bool removeScenarioOutputLink(const OpenViBE::uint32 scenarioOutputIndex, const OpenViBE::CIdentifier& boxIdentifier, const OpenViBE::uint32 boxOutputIndex) = 0;

			virtual bool removeScenarioInput(const uint32 inputIndex) = 0;
			virtual bool removeScenarioOutput(const uint32 outputIndex) = 0;

			//@}
			/** \name Comment management */
			//@{

			/**
			 * \brief Gets next comment identifier
			 * \param previousIdentifier The identifier for the preceeding comment
			 * \return The identifier of the next comment in case of success.
			 * \retval OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c previousIdentifier will cause this function to return the first comment identifier.
			 */
			virtual OpenViBE::CIdentifier getNextCommentIdentifier(const OpenViBE::CIdentifier& previousIdentifier) const = 0;

			/**
			 * \brief Tests whether a given identifier is a comment or not
			 * \param commentIdentifier the identifier to test
			 * \retval true if the identified object is a comment
			 * \retval false if the identified object is not a comment
			 * \note Requesting a bad identifier returns \e false
			 */
			virtual bool isComment(const OpenViBE::CIdentifier& commentIdentifier) const = 0;

			/**
			 * \brief Gets the details for a specific comment
			 * \param commentIdentifier The identifier of the comment which details should be sent.
			 * \return The comment details
			 */
			virtual const OpenViBE::Kernel::IComment* getCommentDetails(const OpenViBE::CIdentifier& commentIdentifier) const = 0;

			/// \copydoc getCommentDetails(const OpenViBE::CIdentifier&)const
			virtual OpenViBE::Kernel::IComment* getCommentDetails(const OpenViBE::CIdentifier& commentIdentifier) = 0;

			/**
			 * \brief Adds a new comment in the scenario
			 * \param[out] commentIdentifier The identifier of the created comment
			 * \param suggestedCommentIdentifier a suggestion for the new comment identifier. If this specific identifier is not
			 *        yet used, this scenario might use it. If the identifier is already used or \c OV_UndefinedIdentifier is passed,
			 *        then a random unused identifier will be used.
			 * \retval true In case of success.
			 * \retval false In case of error. In such case, \c commentIdentifier remains unchanged.
			 * \note This produces an empty and unconfigured comment!
			 */
			virtual bool addComment(OpenViBE::CIdentifier& commentIdentifier, const OpenViBE::CIdentifier& suggestedCommentIdentifier) = 0;

			/**
			 * \brief Adds a new comment in the scenario based on an existing comment
			 * \param[out] commentIdentifier The identifier of the created comment
			 * \param comment the comment to copy in this scenario
			 * \param suggestedCommentIdentifier a suggestion for the new comment identifier. If this specific identifier is not
			 *        yet used, this scenario might use it. If the identifier is already used or \c OV_UndefinedIdentifier is passed,
			 *        then a random unused identifier will be used.
			 * \retval true In case of success.
			 * \retval false In case of error. In such case, \c commentIdentifier remains unchanged.
			 */
			virtual bool addComment(OpenViBE::CIdentifier& commentIdentifier, const OpenViBE::Kernel::IComment& comment, const OpenViBE::CIdentifier& suggestedCommentIdentifier) = 0;
			/**
			 * \brief Removes a comment of the scenario
			 * \param commentIdentifier The comment identifier
			 * \retval true In case of success.
			 * \retval false In case of error.
			 */
			virtual bool removeComment(const OpenViBE::CIdentifier& commentIdentifier) = 0;
			
			//@}
			/** \name Metadata management */
			//@{

			/**
			 * \brief Get next metadata identifier in regards to another
			 * \param previousIdentifier The identifier of the metadata
			 * \retval OV_UndefinedIdentifier In case when metadata with the \c previousIdentifier is not present
			 * \retval OV_UndefinedIdentifier In case when metadata with the \c previousIdentifier is last in the scenario
			 * \return The identifier of the next metadata
			 * \note Giving \c OV_UndefinedIdentifier as \c previousIdentifier will cause this function to return the first metadata identifier.
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
			 * \param metadataIdentifier The metadata identifier
			 * \retval true In case of success.
			 * \retval false In case of error.
			 */
			virtual bool removeMetadata(const OpenViBE::CIdentifier& metadataIdentifier) = 0;
			//@}

			/**
			 * \brief replaces settings of each box by its locally expanded version only expands the $var{} tokens, it leaves others as is
			 */
			virtual bool applyLocalSettings(void) = 0;
			
			/**
			 * \brief Check settings before playing scenario, if the settings are not suitable, stop scenario
			 * and launch a console warning. Only check numeric values in the beginning
			 * \param configurationManager: local configuration manager that can contain the definition of local scenario settings
			 */
			virtual bool checkSettings(IConfigurationManager* configurationManager) = 0;

			/**
			 * \brief Check if boxes in scenario need to be updated. Feed an array with the identifiers
			 * of boxes that need to be updated
			 * \return true if at least one box needs to updated
			 */
			virtual bool checkNeedsUpdateBox() = 0;
			
			/**
			* \brief Gets identifier of next box that needs to be updated
			 * \param previousIdentifier The identifier for the preceeding box that needs updates
			 * \return The identifier of the next box that needs updates in case of success.
			 * \retval OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c previousIdentifier will cause this function to return the first processing unit identifier.
			 * \note Warning: You need to call at least once the function "checkNeedsUpdateBox", before calling this function
			 */
			virtual OpenViBE::CIdentifier getNextNeedsUpdateBoxIdentifier(const OpenViBE::CIdentifier& previousIdentifier) const = 0;

			/**
			 * \brief Indicates if at least one box in scenario need to be updated.
			 * \return true if at least one box needs to updated
			 * \note Warning: You need to call at least once the function "checkNeedsUpdateBox", before calling this function
			 */
			virtual bool hasNeedsUpdateBox() = 0;

			/**
			 * \return true if the scenario is actually a metabox
			 */
			virtual bool isMetabox(void) = 0;

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IAttributable, OV_ClassId_Kernel_Scenario_Scenario)
		};
	}
}

