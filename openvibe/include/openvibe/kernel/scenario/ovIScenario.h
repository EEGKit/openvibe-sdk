#pragma once

#include "ovIBox.h"

namespace OpenViBE
{
	class IMemoryBuffer;

	namespace Plugins
	{
		class IBoxAlgorithmDesc;
	} // namespace Plugins

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
		class OV_API IScenario : public IBox
		{
		public:
			class IScenarioMergeCallback
			{
			public:
				virtual void process(CIdentifier& originalIdentifier, CIdentifier& newIdentifier) = 0;
			};

			/** \name General purpose functions */
			//@{

			/**
			 * \brief Clears the scenario
			 * \retval true In case of success.
			 * \retval false In case of error.
			 */
			virtual bool clear() = 0;

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
			virtual bool merge(const IScenario& scenario, IScenarioMergeCallback* scenarioMergeCallback, bool mergeSettings, bool preserveIdentifiers) = 0;

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
			virtual CIdentifier getNextBoxIdentifier(const CIdentifier& previousIdentifier) const = 0;

			/**
			 * \brief Tests whether a given identifier is a box or not
			 * \param boxID the identifier to test
			 * \retval true if the identified object is a box
			 * \retval false if the identified object is not a box
			 * \note Requesting a bad identifier returns \e false
			 */
			virtual bool isBox(const CIdentifier& boxID) const = 0;

			/**
			 * \brief Gets the details for a specific box
			 * \param boxID The identifier of the box which details should be sent.
			 * \return The box details
			 */
			virtual const IBox* getBoxDetails(const CIdentifier& boxID) const = 0;

			/// \copydoc getBoxDetails(const OpenViBE::CIdentifier&)const
			virtual IBox* getBoxDetails(const CIdentifier& boxID) = 0;

			/**
			 * \brief Adds a new box in the scenario
			 * \param[out] boxID The identifier of the created box
			 * \param suggestedBoxIdentifier A suggestion for the new box identifier. If this specific identifier is not
			 *        yet used, this scenario might use it. If the identifier is already used or \c OV_UndefinedIdentifier is passed,
			 *        then a random unused identifier will be used.
			 * \retval true In case of success.
			 * \retval false In case of error. In such case, \c boxID remains unchanged.
			 * \note This produces an empty and unconfigured box!
			 */
			virtual bool addBox(CIdentifier& boxID, const CIdentifier& suggestedBoxIdentifier) = 0;
			/**
			 * \brief Adds a new box in the scenario based on an existing box
			 * \param[out] boxID The identifier of the created box
			 * \param box The box to copy in this scenario
			 * \param suggestedBoxIdentifier a suggestion for the new box identifier. If this specific identifier is not
			 *        yet used, this scenario might use it. If the identifier is already used or \c OV_UndefinedIdentifier is passed,
			 *        then a random unused identifier will be used.
			 * \retval true In case of success.
			 * \retval false In case of error. In such case, \c boxID remains unchanged.
			 */
			virtual bool addBox(CIdentifier& boxID, const IBox& box, const CIdentifier& suggestedBoxIdentifier) = 0;
			/**
			 * \brief Adds a new box in the scenario
			 * \param[out] boxID The identifier of the created box
			 * \param boxAlgorithmClassIdentifier The class identifier of the algorithm for this box
			 * \param suggestedBoxIdentifier a suggestion for the new box identifier. If this specific identifier is not
			 *        yet used, this scenario might use it. If the identifier is already used or \c OV_UndefinedIdentifier is passed,
			 *        then a random unused identifier will be used.
			 * \retval true In case of success.
			 * \retval false In case of error. In such case, \c boxID remains unchanged.
			 * \note This function prepares the box according to its algorithm class identifier !
			 */
			virtual bool addBox(CIdentifier& boxID, const CIdentifier& boxAlgorithmClassIdentifier, const CIdentifier& suggestedBoxIdentifier) = 0;

			// TODO_JL: Doc
			virtual bool addBox(CIdentifier& boxID, const Plugins::IBoxAlgorithmDesc& boxAlgorithmDesc, const CIdentifier& suggestedBoxIdentifier) = 0;

			/**
			 * \brief Removes a box of the scenario
			 * \param boxID The box identifier
			 * \retval true In case of success.
			 * \retval false In case of error.
			 * \note Each link related to this box is also removed
			 */
			virtual bool removeBox(const CIdentifier& boxID) = 0;

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
			virtual CIdentifier getNextLinkIdentifier(const CIdentifier& previousIdentifier) const = 0;

			/**
			 * \brief Gets next link identifier from fixed box
			 * \param previousIdentifier The identifier for the preceeding link
			 * \param boxID The box identifier which the link should end to
			 * \return The identifier of the next link in case of success.
			 * \retval OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c previousIdentifier will cause this function to return the first link identifier.
			 */
			virtual CIdentifier getNextLinkIdentifierFromBox(const CIdentifier& previousIdentifier, const CIdentifier& boxID) const = 0;

			/**
			 * \brief Gets next link identifier from fixed box output
			 * \param previousIdentifier The identifier for the preceeding link
			 * \param boxID The box identifier which the link should end to
			 * \param outputIndex The input index which the link should end to
			 * \return The identifier of the next link in case of success.
			 * \retval OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c previousIdentifier will cause this function to return the first link identifier.
			 */
			virtual CIdentifier getNextLinkIdentifierFromBoxOutput(const CIdentifier& previousIdentifier, const CIdentifier& boxID, uint32_t outputIndex) const = 0;

			/**
			 * \brief Gets next link identifier from fixed box
			 * \param previousIdentifier The identifier for the preceeding link
			 * \param boxID The box identifier which the link should start from
			 * \return The identifier of the next link in case of success.
			 * \retval OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c previousIdentifier will cause this function to return the first link identifier.
			 */
			virtual CIdentifier getNextLinkIdentifierToBox(const CIdentifier& previousIdentifier, const CIdentifier& boxID) const = 0;

			/**
			 * \brief Gets next link identifier from fixed box input
			 * \param previousIdentifier The identifier for the preceeding link
			 * \param boxID The box identifier which the link should start from
			 * \param index The input index which the link should start from
			 * \return The identifier of the next link in case of success.
			 * \retval OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c previousIdentifier will cause this function to return the first link identifier.
			 */
			virtual CIdentifier getNextLinkIdentifierToBoxInput(const CIdentifier& previousIdentifier, const CIdentifier& boxID, uint32_t index) const = 0;

			/**
			 * \brief Tests whether a given identifier is a link or not
			 * \param identifier the identifier to test
			 * \retval true if the identified object is a link
			 * \retval false if the identified object is not a link
			 * \note Requesting a bad identifier returns \e false
			 */
			virtual bool isLink(const CIdentifier& identifier) const = 0;

			/**
			 * \brief Gets the details for a specific link
			 * \param linkIdentifier The identifier of the link which details should be sent.
			 * \return The link details
			 */
			virtual const ILink* getLinkDetails(const CIdentifier& linkIdentifier) const = 0;

			/// \copydoc getLinkDetails(const OpenViBE::CIdentifier&)const
			virtual ILink* getLinkDetails(const CIdentifier& linkIdentifier) = 0;

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
			virtual bool connect(CIdentifier& linkIdentifier, const CIdentifier& sourceBoxIdentifier, uint32_t sourceBoxOutputIndex,
								 const CIdentifier& targetBoxIdentifier, uint32_t targetBoxInputIndex, const CIdentifier& suggestedLinkIdentifier) = 0;

			/**
			 * \brief Creates a connection between two boxes
			 * \param[out] linkIdentifier The created link identifier.
			 * \param sourceBoxIdentifier The source box identifier
			 * \param sourceBoxOutputIdentifier The output identifier for the given source box
			 * \param targetBoxIdentifier The target box identifier
			 * \param targetBoxInputIdentifier The input identifier for the given target box
			 * \param suggestedLinkIdentifier a suggestion for the new link identifier. If this specific identifier is not
			 *        yet used, this scenario might use it. If the identifier is already used or \c OV_UndefinedIdentifier is passed,
			 *        then a random unused identifier will be used.
			 * \retval true In case of success.
			 * \retval false In case of error. In such case, \c linkIdentifier remains unchanged.
			 */
			virtual bool connect(CIdentifier& linkIdentifier, const CIdentifier& sourceBoxIdentifier, const CIdentifier& sourceBoxOutputIdentifier,
								 const CIdentifier& targetBoxIdentifier, const CIdentifier& targetBoxInputIdentifier, const CIdentifier& suggestedLinkIdentifier) = 0;

			/**
			 * \brief Deletes a connection between two boxes
			 * \param sourceBoxIdentifier The source box identifier
			 * \param sourceBoxOutputIndex The output index for the given source box
			 * \param targetBoxIdentifier The target box identifier
			 * \param targetBoxInputIndex The input index for the given target box
			 * \retval true In case of success.
			 * \retval false In case of error.
			 */
			virtual bool disconnect(const CIdentifier& sourceBoxIdentifier, uint32_t sourceBoxOutputIndex, const CIdentifier& targetBoxIdentifier, uint32_t targetBoxInputIndex) = 0;

			/**
			 * \brief Deletes a connection between two boxes
			 * \param sourceBoxIdentifier The source box identifier
			 * \param sourceBoxOutputIdentifier The output identifier for the given source box
			 * \param targetBoxIdentifier The target box identifier
			 * \param targetBoxInputIdentifier The input identifier for the given target box
			 * \retval true In case of success.
			 * \retval false In case of error.
			 */
			virtual bool disconnect(const CIdentifier& sourceBoxIdentifier, const CIdentifier& sourceBoxOutputIdentifier, const CIdentifier& targetBoxIdentifier, const CIdentifier& targetBoxInputIdentifier) = 0;

			/**
			 * \brief Deletes a connection between two boxes
			 * \param[out] linkIdentifier The identifier for the link to be deleted
			 * \retval true In case of success.
			 * \retval false In case of error.
			 */
			virtual bool disconnect(const CIdentifier& linkIdentifier) = 0;

			/**
			 * \brief Get the output index of a source, for a specific box
			 * \param[in] sourceBoxIdentifier The source box identifier
			 * \param[in] sourceBoxOutputIdentifier The output identifier for the given source box
			 * \param[out] sourceBoxOutputIndex The output index for the given source box
			 * \retval true in case of success.
			 */
			virtual bool getSourceBoxOutputIndex(const CIdentifier& sourceBoxIdentifier, const CIdentifier& sourceBoxOutputIdentifier, uint32_t& sourceBoxOutputIndex) = 0;

			/**
			 * \brief Get the input index of a target, for a specific box
			 * \param[in] sourceBoxIdentifier The target box identifier
			 * \param[in] sourceBoxOutputIdentifier The input identifier for the given target box
			 * \param[out] sourceBoxOutputIndex The input index for the given target box
			 * \retval true in case of success.
			 */
			virtual bool getTargetBoxInputIndex(const CIdentifier& targetBoxIdentifier, const CIdentifier& targetBoxInputIdentifier, uint32_t& targetBoxInputIndex) = 0;

			/**
			 * \brief  Get the output identifier of a source, for a specific box
			 * \param sourceBoxIdentifier The source box identifier
			 * \param sourceBoxOutputIndex The output index for the given source box
			 * \param sourceBoxOutputIdentifier The output identifier for the given source box
			 * \retval true in case of success.
			 */
			virtual bool getSourceBoxOutputIdentifier(const CIdentifier& sourceBoxIdentifier, const uint32_t& sourceBoxOutputIndex, CIdentifier& sourceBoxOutputIdentifier) = 0;

			/**
			 * \brief  Get the input identifier of a target, for a specific box
			 * \param targetBoxIdentifier The target box identifier
			 * \param targetBoxInputIndex The input index for the given target box
			 * \param targetBoxInputIdentifier The input identifier for the given target box
			 * \retval true in case of success.
			 */
			virtual bool getTargetBoxInputIdentifier(const CIdentifier& targetBoxIdentifier, const uint32_t& targetBoxOutputIndex, CIdentifier& targetBoxOutputIdentifier) = 0;

			//@}
			/** \name Scenario Input/Output and MetaBox management */
			//@{

			virtual bool setHasIO(bool hasIO) = 0;
			virtual bool hasIO() const = 0;

			virtual bool setScenarioInputLink(uint32_t scenarioInputIndex, const CIdentifier& boxID, uint32_t boxInputIndex) = 0;
			virtual bool setScenarioInputLink(uint32_t scenarioInputIndex, const CIdentifier& boxID, const CIdentifier& boxInputIdentifier) = 0;
			virtual bool setScenarioOutputLink(uint32_t scenarioOutputIndex, const CIdentifier& boxID, uint32_t boxOutputIndex) = 0;
			virtual bool setScenarioOutputLink(uint32_t scenarioOutputIndex, const CIdentifier& boxID, const CIdentifier& boxOutputIdentifier) = 0;

			virtual bool getScenarioInputLink(uint32_t scenarioInputIndex, CIdentifier& boxID, uint32_t& boxInputIndex) const = 0;
			virtual bool getScenarioInputLink(uint32_t scenarioInputIndex, CIdentifier& boxID, CIdentifier& boxInputIdentifier) const = 0;
			virtual bool getScenarioOutputLink(uint32_t scenarioOutputIndex, CIdentifier& boxID, uint32_t& boxOutputIndex) const = 0;
			virtual bool getScenarioOutputLink(uint32_t scenarioOutputIndex, CIdentifier& boxID, CIdentifier& boxOutputIdentifier) const = 0;

			virtual bool removeScenarioInputLink(uint32_t scenarioInputIndex, const CIdentifier& boxID, uint32_t boxInputIndex) = 0;
			virtual bool removeScenarioOutputLink(uint32_t scenarioOutputIndex, const CIdentifier& boxID, uint32_t boxOutputIndex) = 0;

			virtual bool removeScenarioInput(uint32_t index) = 0;
			virtual bool removeScenarioOutput(uint32_t outputIndex) = 0;

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
			virtual CIdentifier getNextCommentIdentifier(const CIdentifier& previousIdentifier) const = 0;

			/**
			 * \brief Tests whether a given identifier is a comment or not
			 * \param commentIdentifier the identifier to test
			 * \retval true if the identified object is a comment
			 * \retval false if the identified object is not a comment
			 * \note Requesting a bad identifier returns \e false
			 */
			virtual bool isComment(const CIdentifier& commentIdentifier) const = 0;

			/**
			 * \brief Gets the details for a specific comment
			 * \param commentIdentifier The identifier of the comment which details should be sent.
			 * \return The comment details
			 */
			virtual const IComment* getCommentDetails(const CIdentifier& commentIdentifier) const = 0;

			/// \copydoc getCommentDetails(const OpenViBE::CIdentifier&)const
			virtual IComment* getCommentDetails(const CIdentifier& commentIdentifier) = 0;

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
			virtual bool addComment(CIdentifier& commentIdentifier, const CIdentifier& suggestedCommentIdentifier) = 0;

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
			virtual bool addComment(CIdentifier& commentIdentifier, const IComment& comment, const CIdentifier& suggestedCommentIdentifier) = 0;
			/**
			 * \brief Removes a comment of the scenario
			 * \param commentIdentifier The comment identifier
			 * \retval true In case of success.
			 * \retval false In case of error.
			 */
			virtual bool removeComment(const CIdentifier& commentIdentifier) = 0;

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
			virtual CIdentifier getNextMetadataIdentifier(const CIdentifier& previousIdentifier) const = 0;

			/**
			 * \param metadataIdentifier The identifier to test
			 * \retval true If the identified object is metadata
			 * \retval false If the identified object is not metadata or when the identifier is not present in the scenario
			 */
			virtual bool isMetadata(const CIdentifier& metadataIdentifier) const = 0;


			/**
			 * \param metadataIdentifier The identifier of a metadata
			 * \return Pointer to object containing metadata details
			 */
			virtual const IMetadata* getMetadataDetails(const CIdentifier& metadataIdentifier) const = 0;

			/// \copydoc getMetadataDetails(const OpenViBE::CIdentifier&)const
			virtual IMetadata* getMetadataDetails(const CIdentifier& metadataIdentifier) = 0;

			/**
			 * \brief Add new metadata in the scenario
			 * \param[out] metadataIdentifier The identifier of the newly created metadata
			 * \param suggestedMetadataIdentifier A suggestion for the new identifier. If the identifier is already used or \c OV_UndefinedIdentifier is passed,
			 *        then a random unused identifier will be used.
			 * \retval true In case of success.
			 * \retval false In case of error. In this case, \c metadataIdentifier remains unchanged.
			 * \note This method creates an empty metadata.
			 */
			virtual bool addMetadata(CIdentifier& metadataIdentifier, const CIdentifier& suggestedMetadataIdentifier) = 0;

			/**
			 * \brief Remove metadata from the scenario
			 * \param metadataIdentifier The metadata identifier
			 * \retval true In case of success.
			 * \retval false In case of error.
			 */
			virtual bool removeMetadata(const CIdentifier& metadataIdentifier) = 0;
			//@}

			/**
			 * \brief replaces settings of each box by its locally expanded version only expands the $var{} tokens, it leaves others as is
			 */
			virtual bool applyLocalSettings() = 0;

			/**
			 * \brief Check settings before playing scenario, if the settings are not suitable, stop scenario
			 * and launch a console warning. Only check numeric values in the beginning
			 * \param configurationManager: local configuration manager that can contain the definition of local scenario settings
			 */
			virtual bool checkSettings(IConfigurationManager* configurationManager) = 0;

			/**
			 * \brief Check if boxes in scenario need to be updated. Feed an map of updates boxes instances with the identifiers
			 * of outdated boxes
			 * \return true if at least one box needs to updated
			 */
			virtual bool checkOutdatedBoxes() = 0;

			/**
			* \brief Gets identifier of next outdated box
			 * \param previousIdentifier The identifier for the preceeding outdated box
			 * \return The identifier of the next box that needs updates in case of success.
			 * \retval OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c previousIdentifier will cause this function to return the first processing unit identifier.
			 * \note Warning: You need to call at least once the function "checkOutdatedBoxes", before calling this function
			 */
			virtual CIdentifier getNextOutdatedBoxIdentifier(const CIdentifier& previousIdentifier) const = 0;

			/**
			 * \brief Indicates if at least one box in scenario need to be updated.
			 * \return true if at least one box needs to updated
			 * \note Warning: You need to call at least once the function "checkOutdatedBoxes", before calling this function
			 */
			virtual bool hasOutdatedBox() = 0;

			/**
			 * \brief Update the prototypes of the box identified by the given identifier.
			 * \param boxID
			 * \retval true in case of success
			 * \retval false in case of error
			 */
			virtual bool updateBox(const CIdentifier& boxID) = 0;

			/**
			 * \brief Remove deprecated interfacors from the box identified by the given identifier.
			 * \param boxID Id of the box to clean up
			 * \retval true in case of success
			 * \retval false if the box does not exist
			 */
			virtual bool removeDeprecatedInterfacorsFromBox(const CIdentifier& boxID) = 0;

			/**
			 * \brief Check if scenario contains a box with a deprecated interfacor due to an incomplete update
			 * \retval true if a box in the scenario has a deprecated interfacor
			 * \retval false if no box in the scenario contains a deprecated interfacor
			 */
			virtual bool containsBoxWithDeprecatedInterfacors() const = 0;

			/**
			 * \return true if the scenario is actually a metabox
			 */
			virtual bool isMetabox() = 0;

			virtual void getBoxIdentifierList(CIdentifier** identifierList, size_t* size) const = 0;
			virtual void getCommentIdentifierList(CIdentifier** identifierList, size_t* size) const = 0;
			virtual void getMetadataIdentifierList(CIdentifier** identifierList, size_t* size) const = 0;
			virtual void getLinkIdentifierList(CIdentifier** identifierList, size_t* size) const = 0;
			virtual void getLinkIdentifierFromBoxList(const CIdentifier& boxID, CIdentifier** identifierList, size_t* size) const = 0;
			virtual void getLinkIdentifierFromBoxOutputList(const CIdentifier& boxID, uint32_t outputIndex, CIdentifier** identifierList, size_t* size) const = 0;
			virtual void getLinkIdentifierToBoxList(const CIdentifier& boxID, CIdentifier** identifierList, size_t* size) const = 0;
			virtual void getLinkIdentifierToBoxInputList(const CIdentifier& boxID, uint32_t inputInex, CIdentifier** identifierList, size_t* size) const = 0;
			virtual void getOutdatedBoxIdentifierList(CIdentifier** identifierList, size_t* size) const = 0;
			virtual void releaseIdentifierList(CIdentifier* identifierList) const = 0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IAttributable, OV_ClassId_Kernel_Scenario_Scenario)
		};
	} // namespace Kernel
} // namespace OpenViBE