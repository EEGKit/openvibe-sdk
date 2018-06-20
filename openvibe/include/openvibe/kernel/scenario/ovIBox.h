#ifndef __OpenViBE_Kernel_Scenario_IBox_H__
#define __OpenViBE_Kernel_Scenario_IBox_H__

#include "ovIAttributable.h"
#include "../../ovCObservable.h"

#include <vector>

namespace OpenViBE
{
	namespace Kernel
	{

		enum BoxEventMessageType{
			SettingValueUpdate,
			SettingChange,
			SettingDelete,
			SettingAdd,
			SettingsReorder,
			SettingsAllChange
		};
		
		class OV_API BoxEventMessage{
		public:
			BoxEventMessageType m_eType;
			OpenViBE::int32 m_i32FirstIndex;
			OpenViBE::int32 m_i32SecondIndex;
		};

		/**
		 * \class IBox
		 * \author Yann Renard (IRISA/INRIA)
		 * \date 2006-08-16
		 * \brief Complete OpenViBE box interface
		 * \ingroup Group_Scenario
		 * \ingroup Group_Kernel
		 *
		 * This interface can be used in order to fully describe an
		 * OpenViBE black box. It describes its identification values,
		 * its inputs, its outputs and its settings.
		 */
		class OV_API IBox : public OpenViBE::Kernel::IAttributable, public OpenViBE::CObservable
		{
		public:

			/** \name Box naming and identification */
			//@{

			/**
			 * \brief Gets the identifier of this box
			 * \return The identifier of this OpenViBE box.
			 */
			virtual OpenViBE::CIdentifier getIdentifier(void) const=0;
			/**
			 * \brief Gets the display name of this box
			 * \return The name of this OpenViBE box.
			 */
			virtual OpenViBE::CString getName(void) const=0;
			/**
			 * \brief Gets the algorithm class identifier
			 * \return This box' algorithm class identifier
			 */
			virtual OpenViBE::CIdentifier getAlgorithmClassIdentifier(void) const=0;
			/**
			 * \brief Changes the identifier of this box
			 * \param rIdentifier [in] : The new identifier
			 *        this box should take.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setIdentifier(
				const OpenViBE::CIdentifier& rIdentifier)=0;
			/**
			 * \brief Renames this box
			 * \param sName [in] : The name this box should take
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setName(
				const OpenViBE::CString& rsName)=0;
			/**
			 * \brief Changes the algorithm identifier of this box
			 * \param rAlgorithmClassIdentifier [in] : The new algorithm
			 *        identifier this box should take.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setAlgorithmClassIdentifier(
				const OpenViBE::CIdentifier& rAlgorithmClassIdentifier)=0;
				
			
			/**
			 * \brief Requests for a suggested identifier. If it is already used in the box or
			 * if it is OV_UndefinedIdentifier, a news random one is proposed uniq in the scope of settings.
			 * \param oIdentifier [in] : the requested identifier
			 * \return the proposed identifier
			 */
			virtual CIdentifier getUnusedSettingIdentifier(const CIdentifier& rIdentifier = OV_UndefinedIdentifier) const=0;
			
			/**
			 * \brief Requests for a suggested identifier. If it is already used in the box or
			 * if it is OV_UndefinedIdentifier, a news random one is proposed uniq in the scope of the inputs 
			 * of the given map object.
			 * \param oIdentifier [in] : the requested identifier
			 * \return the proposed identifier
			 */
			virtual CIdentifier getUnusedInputIdentifier(const CIdentifier& rIdentifier = OV_UndefinedIdentifier) const=0;
			
			/**
			 * \brief Requests for a suggested identifier. If it is already used in the box or
			 * if it is OV_UndefinedIdentifier, a news random one is proposed uniq in the scope of the outputs 
			 * of the given map object.
			 * \param oIdentifier [in] : the requested identifier
			 * \return the proposed identifier
			 */
			virtual CIdentifier getUnusedOutputIdentifier(const CIdentifier& rIdentifier = OV_UndefinedIdentifier) const=0;
			
			/**
			 * \brief Combine string bytes values with a seed identifier in order to generate a new identifier.
			 * \param seedId   The seed identifier
			 * \param strValue The string value
			 * \retval A new identifier
			 */	
			virtual OpenViBE::CIdentifier combineIdentifierWithString(const OpenViBE::CIdentifier& seedId, const OpenViBE::CString &strValue) const=0;
			
					
			//@}
			/** \name Initialisation from prototypes etc... */
			//@{

			/**
			 * \brief Initializes the box from box algorithm descriptor
			 * \param rAlgorithmClassIdentifier [in] : The new algorithm
			 *        identifier this box should take.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Resets the box and initializes its input/output/settings
			 * according to the box algorithm descriptor
			 */
			virtual OpenViBE::boolean initializeFromAlgorithmClassIdentifier(
				const OpenViBE::CIdentifier& rAlgorithmClassIdentifier)=0;
			/**
			 * \brief Initializes the box from an already existing box
			 * \param rExisitingBox [in] : The existing box.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Resets the box and initializes its input/output/settings
			 * according to the existing box.
			 */
			virtual OpenViBE::boolean initializeFromExistingBox(
				const OpenViBE::Kernel::IBox& rExistingBox)=0;

			//@}
			/** \name Input management */
			//@{

			/**
			 * \brief Adds an input to this box
			 * \param sName [in] : The input name
			 * \param rTypeIdentifier [in] : The
			 *        input type identifier
			 * \param oIdentifier [in] : The input identifier
			 * \param bNotify: if true, activate notification callback (true by default)
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * The input is always added after the last
			 * already existing input.
			 */
			virtual OpenViBE::boolean addInput(
				const OpenViBE::CString& rsName,
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CIdentifier& oIdentifier = OV_UndefinedIdentifier,
				const OpenViBE::boolean bNotify=true)=0;
			/**
			 * \brief Removes an input for this box
			 * \param ui32InputIndex [in] : The index
			 *        of the input to be removed
			 * \param bNotify manage notify. Is enabled by default
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Inputs coming after the removed input
			 * have their indices changing after this,
			 * they all decrease by 1.
			 */
			virtual OpenViBE::boolean removeInput(
				const OpenViBE::uint32 ui32InputIndex,
				const bool bNotify=true)=0;
			/**
			 * \brief Gets the number of inputs for this box
			 * \return The number of inputs for this box.
			 */
			virtual OpenViBE::uint32 getInputCount(void) const=0;
			
			/**
			 * \brief Gets the number of inputs, included deprecated ones, for this box
			 * \return The number of inputs for this box.
			 */
			virtual OpenViBE::uint32 getInputCountWithMissing(void) const=0;
			
			
			/**
			 * \brief Gets input identifier from index
			 * \param ui32InputIndex [in] : The input index
			 * \param rIdentifier [out] : The identifier of this input
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rName remains unchanged.
			 */
			virtual OpenViBE::boolean getInputIdentifier(
				const OpenViBE::uint32 ui32InputIndex,
				OpenViBE::CIdentifier& rIdentifier) const=0;
			
			/**
			 * \brief Gets input index from identifier
			 * \param rIdentifier [in] : The identifier of this input
			 * \param ui32InputIndex [out] : The input index
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rName remains unchanged.
			 */
			virtual OpenViBE::boolean getInputIndex(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32& ui32InputIndex) const=0;
			
			/**
			 * \brief Gets an input type identifier by index
			 * \param ui32InputIndex [in] : The input index
			 * \param rTypeIdentifier [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rTypeIdentifier remains unchanged.
			 */
			virtual OpenViBE::boolean getInputType(
				const OpenViBE::uint32 ui32InputIndex,
				OpenViBE::CIdentifier& rTypeIdentifier) const=0;
				
				
			/**
			 * \brief Gets an input type identifier by identifier
			 * \param rInputIdentifier [in] : The input identifier
			 * \param rTypeIdentifier [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rTypeIdentifier remains unchanged.
			 */
			virtual OpenViBE::boolean getInputType(
				const OpenViBE::CIdentifier& rInputIdentifier,
				OpenViBE::CIdentifier& rTypeIdentifier) const=0;
				
			/**
			 * \brief Gets an input type identifier by name
			 * \param rsInputName [in] : The input name
			 * \param rTypeIdentifier [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rTypeIdentifier remains unchanged.
			 */
			virtual OpenViBE::boolean getInputType(
				const OpenViBE::CString rsInputName,
				OpenViBE::CIdentifier& rTypeIdentifier) const=0;
				
			/**
			 * \brief Gets an input name by index
			 * \param ui32InputIndex [in] : The input index
			 * \param rName [out] : The name of this input
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rName remains unchanged.
			 */
			virtual OpenViBE::boolean getInputName(
				const OpenViBE::uint32 ui32InputIndex,
				OpenViBE::CString& rName) const=0;
				
			/**
			 * \brief Gets an input name by identifier
			 * \param rInputIdentifier [in] : The input identifier
			 * \param rName [out] : The name of this input
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rName remains unchanged.
			 */
			virtual OpenViBE::boolean getInputName(
				const OpenViBE::CIdentifier& rInputIdentifier,
				OpenViBE::CString& rName) const=0;
				
			/**
			 * \brief Gets an input deprecation by index
			 * \param ui32InputIndex [in] : The input index
			 * \param rValue [out] : The deprecation value of this input
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rValue remains unchanged.
			 */
			virtual OpenViBE::boolean getInputMissingStatus(
				const OpenViBE::uint32 ui32InputIndex,
				bool& rValue) const=0;
				
			/**
			 * \brief Gets an input deprecation by identifier
			 * \param rInputIdentifier [in] : The input identifier
			 * \param rValue [out] : The deprecation value of this input
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rValue remains unchanged.
			 */
			virtual OpenViBE::boolean getInputMissingStatus(
				const OpenViBE::CIdentifier& rInputIdentifier,
				bool& rValue) const=0;
				
				
			
			/**
			 * \brief Tells if input handled by given identifier exists.
			 * \param rInputIdentifier	 the given identifier
			 * \return \e	true if it exists
			 *		   \e   false if it does not exist
			 */
			virtual OpenViBE::boolean hasInputWithIdentifier(const OpenViBE::CIdentifier& rOutputIdentifier)const =0;
			
			/**
			 * \brief Tells if input handled by given index exists with the given type.
			 * \param ui32InputIndex [in] : The input index
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e	true if it exists
			 *		   \e   false if it does not exist
			 */
			virtual OpenViBE::boolean hasInputWithType(
				const OpenViBE::uint32 ui32InputIndex,
				const OpenViBE::CIdentifier& rTypeIdentifier)const =0;
				
			/**
			 * \brief Sets an input type identifier by index
			 * \param ui32InputIndex [in] : The input index
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setInputType(
				const OpenViBE::uint32 ui32InputIndex,
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
				
			/**
			 * \brief Sets an input type identifier by name
			 * \param rsInputName [in] : The input name
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setInputType(
				const OpenViBE::CString& rsInputName,
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
			
			/**
			 * \brief Sets an input type identifier by identifier
			 * \param rInputIndex [in] : The input identifier
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setInputType(
				const OpenViBE::CIdentifier& rInputIdentifier,
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
			
			/**
			 * \brief Sets an input name by index
			 * \param ui32InputIndex [in] : The input index
			 * \param rName [in] : The name of this input
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setInputName(
				const OpenViBE::uint32 ui32InputIndex,
				const OpenViBE::CString& rName)=0;
				
			/**
			 * \brief Sets an input name by identifier
			 * \param rInputIdentifier [in] : The input identifier
			 * \param rName [in] : The name of this input
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setInputName(
				const OpenViBE::CIdentifier& rInputIdentifier,
				const OpenViBE::CString& rName)=0;
				
			/**
			 * \brief Sets input deprecation value by index
			 * \param ui32InputIndex [in] : The input index
			 * \param rValue [in] : The deprecation value of this input
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setInputMissingStatus(
				const OpenViBE::uint32 ui32InputIndex,
				const bool rValue)=0;
				
			/**
			 * \brief Sets input deprecation value by identifier
			 * \param rInputIdentifier [in] : The input identifier
			 * \param rValue [in] : The deprecation value of this input
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setInputMissingStatus(
				const OpenViBE::CIdentifier& rInputIdentifier,
				const bool rValue)=0;
				
			/**
			 * \brief Update identifier of the input handled by index
			 *  Identifier is computed by using name and type of the input. When these attributes have
			 *  changed, it can be usefull to regenerate identifier. The identifier can be suggested if defined.
			 * \param ui32InputIndex	the input index,
			 * \param suggestedIdentifier the suggested identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */	
			virtual bool updateInputIdentifier(const unsigned int ui32InputIndex, const CIdentifier& suggestedIdentifier = OV_UndefinedIdentifier)=0;

			//@}
			/** \name Output management */
			//@{

			/**
			 * \brief Adds an output to this box
			 * \param sName [in] : The output name
			 * \param rTypeIdentifier [in] : The
			 *        output type idenfitier
			 * \param oIdentifier [in] : The output identifier
			 * \param bNotify: if true, activate notification callback (true by default)
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * The output is always added after the last
			 * already existing output.
			 */
			virtual OpenViBE::boolean addOutput(
				const OpenViBE::CString& rsName,
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CIdentifier& rIdentifier = OV_UndefinedIdentifier,
				const OpenViBE::boolean bNotify = true)=0;
			/**
			 * \brief Removes an output for this box
			 * \param ui32OutputIndex [in] : The index
			 *        of the output to remove
			 * \param bNotify manage notify. Is enabled by default
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Outputs coming after the removed output
			 * have their indices changing after this,
			 * they all decrease by 1.
			 */
			virtual OpenViBE::boolean removeOutput(
				const OpenViBE::uint32 ui32OutputIndex,
				const bool bNotify=true)=0;
			/**
			 * \brief Gets the number of outputs for this box
			 * \return The number of outputs for this box.
			 */
			virtual OpenViBE::uint32 getOutputCount(void) const=0;
			
			/**
			 * \brief Gets the number of outputs, included deprecated ones, for this box
			 * \return The number of outputs for this box.
			 */
			virtual OpenViBE::uint32 getOutputCountWithMissing(void) const=0;
			
			/**
			 * \brief Gets output identifier from index
			 * \param ui32OutputIndex [in] : The output index
			 * \param rIdentifier [out] : The identifier of this output
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rName remains unchanged.
			 */
			virtual OpenViBE::boolean getOutputIdentifier(
				const OpenViBE::uint32 ui32OutputIndex,
				OpenViBE::CIdentifier& rIdentifier) const=0;
				
			/**
			 * \brief Gets output index from identifier
			 * \param rIdentifier [in] : The identifier of this output
			 * \param ui32OutputIndex [out] : The output index
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rName remains unchanged.
			 */
			virtual OpenViBE::boolean getOutputIndex(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32& ui32OutputIndex) const=0;
			
			/**
			 * \brief Gets an output type identifier by index
			 * \param ui32OutputIndex [in] : The output index
			 * \param rTypeIdentifier [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rTypeIdentifier remains unchanged.
			 */
			virtual OpenViBE::boolean getOutputType(
				const OpenViBE::uint32 ui32OutputIndex,
				OpenViBE::CIdentifier& rTypeIdentifier) const=0;
				
			/**
			 * \brief Gets an output type identifier by identifier
			 * \param rOutputIdentifier [in] : The output identifier
			 * \param rTypeIdentifier [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rTypeIdentifier remains unchanged.
			 */
			virtual OpenViBE::boolean getOutputType(
				const OpenViBE::CIdentifier& rOutputIdentifier,
				OpenViBE::CIdentifier& rTypeIdentifier) const=0;
				
			/**
			 * \brief Gets an output type identifier by name
			 * \param rsOutputName [in] : The output name
			 * \param rTypeIdentifier [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rTypeIdentifier remains unchanged.
			 */
			virtual OpenViBE::boolean getOutputType(
				const OpenViBE::CString rsOutputName,
				OpenViBE::CIdentifier& rTypeIdentifier) const=0;
				
			/**
			 * \brief Gets an output name by index
			 * \param ui32OutputIndex [in] : The output index
			 * \param rName [out] : The name of this output
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rName remains unchanged.
			 */
			virtual OpenViBE::boolean getOutputName(
				const OpenViBE::uint32 ui32OutputIndex,
				OpenViBE::CString& rName) const=0;
				
			/**
			 * \brief Gets an output name by identifier
			 * \param rOutputIdentifier [in] : The output identifier
			 * \param rName [out] : The name of this output
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rName remains unchanged.
			 */
			virtual OpenViBE::boolean getOutputName(
				const OpenViBE::CIdentifier& rOutputIdentifier,
				OpenViBE::CString& rName) const=0;
			/**
			 * \brief Gets an output deprecation by index
			 * \param ui32OutputIndex [in] : The output index
			 * \param rValue [out] : The deprecation value of this output
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rValue remains unchanged.
			 */
			virtual OpenViBE::boolean getOutputMissingStatus(
				const OpenViBE::uint32 ui32OutputIndex,
				bool& rValue) const=0;
				
			/**
			 * \brief Gets an output deprecation by identifier
			 * \param rOutputIdentifier [in] : The output identifier
			 * \param rValue [out] : The deprecation value of this output
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rValue remains unchanged.
			 */
			virtual OpenViBE::boolean getOutputMissingStatus(
				const OpenViBE::CIdentifier& rOutputIdentifier,
				bool& rValue) const=0;
					
			/**
			 * \brief Tells if output handled by given identifier exists.
			 * \param rOutputIdentifier	 the given identifier
			 * \return \e	true if it exists
			 *		   \e   false if it does not exist
			 */
			virtual OpenViBE::boolean hasOutputWithIdentifier(const OpenViBE::CIdentifier& rOutputIdentifier) const=0;
			
			/**
			 * \brief Tells if output handled by given index exists with the given type.
			 * \param ui32OutputIndex [in] : The output index
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e	true if it exists
			 *		   \e   false if it does not exist
			 */
			virtual OpenViBE::boolean hasOutputWithType(
				const OpenViBE::uint32 ui32OutputIndex,
				const OpenViBE::CIdentifier& rTypeIdentifier)const =0;
			
			/**
			 * \brief Sets an output type identifier by index
			 * \param ui32OutputIndex [in] : The output index
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setOutputType(
				const OpenViBE::uint32 ui32OutputIndex,
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
				
			/**
			 * \brief Sets an output type identifier by identifier
			 * \param rOutputIdentifier [in] : The output identifier
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setOutputType(
				const OpenViBE::CIdentifier& rOutputIdentifier,
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
			
			/**
			 * \brief Sets an output type identifier by name
			 * \param rsOutputName [in] : The output name
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setOutputType(
				const OpenViBE::CString rsOutputName,
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
				
			/**
			 * \brief Sets an output name by index
			 * \param ui32OutputIndex [in] : The output index
			 * \param rName [in] : The name of this output
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setOutputName(
				const OpenViBE::uint32 ui32OutputIndex,
				const OpenViBE::CString& rName)=0;
				
			/**
			 * \brief Sets an output name by identifier
			 * \param rOutputIdentifier [in] : The output index
			 * \param rName [in] : The name of this output
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setOutputName(
				const OpenViBE::CIdentifier& rOutputIdentifier,
				const OpenViBE::CString& rName)=0;

			/**
			 * \brief Sets output deprecation value by index
			 * \param ui32OutputIndex [in] : The output index
			 * \param rValue [in] : The deprecation value of this output
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setOutputMissingStatus(
				const OpenViBE::uint32 ui32OutputIndex,
				const bool rValue)=0;
				
			/**
			 * \brief Sets output deprecation value by identifier
			 * \param rOutputIdentifier [in] : The output identifier
			 * \param rValue [in] : The deprecation value of this output
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setOutputMissingStatus(
				const OpenViBE::CIdentifier& rOutputIdentifier,
				const bool rValue)=0;
			
			/**
			 * \brief Update identifier of the output handled by index
			 *  Identifier is computed by using name and type of the output. When these attributes have
			 *  changed, it can be usefull to regenerate identifier.  The identifier can be suggested if defined.
			 * \param ui32OutputIndex	the output index,
			 * \param suggestedIdentifier the suggested identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */	
			virtual bool updateOutputIdentifier(const unsigned int ui32OutputIndex, const CIdentifier& suggestedIdentifier=OV_UndefinedIdentifier)=0;
			
			//@}
			/** \name Setting management */
			//@{	

			/**
			 * \brief Adds a setting to this box
			 * \param sName [in] : The setting name
			 * \param rTypeIdentifier [in] : The
			 *        setting type identifier
			 * \param sDefaultValue [in] : The default
			 *        value for this setting
			 * \param i32Index [in] : The index where to
			 *        add the setting
			 * \param bModifiability [in] : true if modifiable setting 
			 * \param oIdentifier [in] : The setting identifier
			 * \param bNotify: if true, activate notification callback (true by default)
			 * 
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * The setting is added to the index required.
			 * The default value -1 means that the setting
			 * will be add to the end.
			 */
			virtual OpenViBE::boolean addSetting(
				const OpenViBE::CString& rsName,
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sDefaultValue,
				const OpenViBE::uint32 ui32Index = OV_Value_UndefinedIndexUInt,
				const OpenViBE::boolean bModifiability = false,
				const OpenViBE::CIdentifier& rIdentifier = OV_UndefinedIdentifier,
				const OpenViBE::boolean bNotify=true)=0;
			
			/**
			 * \brief Removes a setting for this box
			 * \param ui32SettingIndex [in] : The index
			 *        of the setting to remove
			 * \param bNotify manage notify. Is enabled by default
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Settings coming after the removed setting
			 * have their indices changing after this,
			 * they all decrease by 1.
			 */
			virtual OpenViBE::boolean removeSetting(
				const OpenViBE::uint32 ui32SettingIndex,
				const OpenViBE::boolean bNotify=true)=0;
			/**
			 * \brief Gets the number of settings for this box
			 * \return The number of settings for this box.
			 */
			virtual OpenViBE::uint32 getSettingCount(void) const=0;
			
			/**
			 * \brief Gets the number of settings, included deprecated ones, for this box
			 * \return The number of settings for this box.
			 */
			virtual OpenViBE::uint32 getSettingCountWithMissing(void) const=0;

			/**
			 * \brief checks if a setting with a given name is present
			 * \param rName [in] : Name of the setting
			 * \return true if setting is present, false otherwise
			 */
			virtual OpenViBE::boolean hasSettingWithName(
			        const OpenViBE::CString& rName) const = 0;
			        
			 /**
			 * \brief Tells if setting handled by given identifier exists.
			 * \param rSettingIdentifier	 the given identifier
			 * \return \e	true if it exists
			 *		   \e   false if it does not exist
			 */
			virtual OpenViBE::boolean hasSettingWithIdentifier(const OpenViBE::CIdentifier& rSettingIdentifier) const=0;
			
			/**
			 * \brief Tells if setting handled by given index exists with the given type.
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e	true if it exists
			 *		   \e   false if it does not exist
			 */
			virtual OpenViBE::boolean hasSettingWithType(
				const OpenViBE::uint32 ui32SettingIndex,
				const OpenViBE::CIdentifier& rTypeIdentifier)const =0;

			/**
			 * \brief gets the index of a setting with a given name
			 * \param rName [in] : Name of the setting
			 * \return index of the first setting with specified name
			 */
			virtual bool getSettingIndex(
			        const OpenViBE::CString& rName, unsigned int& rIndex) const = 0;
			        
			/**
			 * \brief gets the index of a setting with a given identifier
			 * \param rIdentifier [in] : identifier of the setting
			 * \return index of the first setting with specified name
			 */
			virtual bool getSettingIndex(
			        const OpenViBE::CIdentifier& rIdentifier, unsigned int& rIndex) const = 0;
			/**
			 * \brief Gets a setting type by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rTypeIdentifier [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rTypeIdentifier remains unchanged.
			 */
			virtual OpenViBE::boolean getSettingType(
				const OpenViBE::uint32 ui32SettingIndex,
				OpenViBE::CIdentifier& rTypeIdentifier) const=0;
				
			/**
			 * \brief Gets a setting type by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rTypeIdentifier [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rTypeIdentifier remains unchanged.
			 */
			virtual OpenViBE::boolean getSettingType(
				const OpenViBE::CIdentifier &rIdentifier,
				OpenViBE::CIdentifier& rTypeIdentifier) const=0;
				
			/**
			 * \brief Gets a setting type by name
			 * \param sName [in] : The setting name
			 * \param rTypeIdentifier [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rTypeIdentifier remains unchanged.
			 */
			virtual OpenViBE::boolean getSettingType(
				const OpenViBE::CString &sName,
				OpenViBE::CIdentifier& rTypeIdentifier) const=0;
				
			/**
			 * \brief Gets setting identifier
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rTypeIdentifier [out] : The type identifier
			 * \return \c identifier value.			 
			 */
			virtual bool getSettingIdentifier(
				const OpenViBE::uint32 ui32SettingIndex,
				OpenViBE::CIdentifier& rTypeIdentifier) const=0;
				
			/**
			 * \brief Gets setting identifier by name
			 * \param sName [in] : The setting name
			 * \param rTypeIdentifier [out] : The type identifier
			 * \return \c identifier value.			 
			 */
			virtual bool getSettingIdentifier(
				const OpenViBE::CString& rsName,
				OpenViBE::CIdentifier& rTypeIdentifier) const=0;
								
			/**
			 * \brief Gets a setting name by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rName [out] : The name of this setting
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rName remains unchanged.
			 */
			virtual OpenViBE::boolean getSettingName(
				const OpenViBE::uint32 ui32SettingIndex,
				OpenViBE::CString& rName) const=0;
				
			/**
			 * \brief Gets a setting name by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rName [out] : The name of this setting
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rName remains unchanged.
			 */
			virtual OpenViBE::boolean getSettingName(
				const OpenViBE::CIdentifier &rIdentifier,
				OpenViBE::CString& rName) const=0;
							
			/**
			 * \brief Gets the default setting value by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rDefaultValue [out] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rDefaultValue remains unchanged.
			 */
			virtual OpenViBE::boolean getSettingDefaultValue(
				const OpenViBE::uint32 ui32SettingIndex,
				OpenViBE::CString& rDefaultValue) const=0;
				
		    /**
			 * \brief Gets the default setting value by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rDefaultValue [out] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rDefaultValue remains unchanged.
			 */
			virtual OpenViBE::boolean getSettingDefaultValue(
				const OpenViBE::CIdentifier &rIdentifier,
				OpenViBE::CString& rDefaultValue) const=0;
				
			 /**
			 * \brief Gets the default setting value by name
			 * \param sName [in] : The setting name
			 * \param rDefaultValue [out] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rDefaultValue remains unchanged.
			 */
			virtual OpenViBE::boolean getSettingDefaultValue(
				const OpenViBE::CString &sName,
				OpenViBE::CString& rDefaultValue) const=0;
				
			/**
			 * \brief Gets the setting value by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rValue remains unchanged.
			 */
			virtual OpenViBE::boolean getSettingValue(
				const OpenViBE::uint32 ui32SettingIndex,
				OpenViBE::CString& rValue) const=0;
				
			/**
			 * \brief Gets the setting value by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rValue remains unchanged.
			 */
			virtual OpenViBE::boolean getSettingValue(
				const OpenViBE::CIdentifier &rIdentifier,
				OpenViBE::CString& rValue) const=0;
			
			/**
			 * \brief Gets the setting value by name
			 * \param sName [in] : The setting name
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rValue remains unchanged.
			 */
			virtual OpenViBE::boolean getSettingValue(
				const OpenViBE::CString &sName,
				OpenViBE::CString& rValue) const=0;
				
			/**
			 * \brief Sets a setting type identifier by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSettingType(
				const OpenViBE::uint32 ui32SettingIndex,
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
				
			/**
			 * \brief Sets a setting type identifier by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSettingType(
				const OpenViBE::CIdentifier &rIdentifier,
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;
			
			/**
			 * \brief Sets a setting type identifier by name
			 * \param sName [in] : The setting name
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSettingType(
				const OpenViBE::CString &sName,
				const OpenViBE::CIdentifier& rTypeIdentifier)=0;	
						
			/**
			 * \brief Sets a setting name by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rName [in] : The name of this setting
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSettingName(
				const OpenViBE::uint32 ui32SettingIndex,
				const OpenViBE::CString& rName)=0;
				
		    /**
			 * \brief Sets a setting name by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rName [in] : The name of this setting
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSettingName(
				const OpenViBE::CIdentifier &rIdentifier,
				const OpenViBE::CString& rName)=0;
				
			/**
			 * \brief Sets the default setting value by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rDefaultValue [in] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSettingDefaultValue(
				const OpenViBE::uint32 ui32SettingIndex,
				const OpenViBE::CString& rDefaultValue)=0;
				
			/**
			 * \brief Sets the default setting value by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rDefaultValue [in] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSettingDefaultValue(
				const OpenViBE::CIdentifier &rIdentifier,
				const OpenViBE::CString& rDefaultValue)=0;
			
			/**
			 * \brief Sets the default setting value by name
			 * \param sName [in] : The setting name
			 * \param rDefaultValue [in] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSettingDefaultValue(
				const OpenViBE::CString &sName,
				const OpenViBE::CString& rDefaultValue)=0;
					
			/**
			 * \brief Sets the setting value by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSettingValue(
				const OpenViBE::uint32 ui32SettingIndex,
				const OpenViBE::CString& rValue,
				const OpenViBE::boolean bNotify=true)=0;
				
			/**
			 * \brief Sets the setting value by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSettingValue(
				const OpenViBE::CIdentifier& rIdentifier,
				const OpenViBE::CString& rValue)=0;
			
			/**
			 * \brief Sets the setting value by name
			 * \param sName [in] : The setting name
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSettingValue(
				const OpenViBE::CString& rsName,
				const OpenViBE::CString& rValue)=0;

			/**
			 * \brief Gets the setting modifiability by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean getSettingMod(
				const OpenViBE::uint32 ui32SettingIndex,
				OpenViBE::boolean& rValue) const=0;
				
			/**
			 * \brief Gets the setting modifiability by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean getSettingMod(
				const OpenViBE::CIdentifier &rIdentifier,
				OpenViBE::boolean& rValue) const=0;
				
			/**
			 * \brief Gets the setting modifiability by name
			 * \param sName [in] : The setting name
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean getSettingMod(
				const OpenViBE::CString &sName,
				OpenViBE::boolean& rValue) const=0;
				
			/**
			 * \brief Sets the setting modifiability by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSettingMod(
				const OpenViBE::uint32 ui32SettingIndex,
				const OpenViBE::boolean rValue)=0;
				
			/**
			 * \brief Sets the setting modifiability by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSettingMod(
				const OpenViBE::CIdentifier &rIdentifier,
				const OpenViBE::boolean rValue)=0;
				
			/**
			 * \brief Sets the setting modifiability by name
			 * \param sName [in] : The setting name
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSettingMod(
				const OpenViBE::CString &sName,
				const OpenViBE::boolean rValue)=0;

			/**
			 * \brief Gets the setting deprecation by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean getSettingMissingStatus(
			        const OpenViBE::uint32 ui32SettingIndex,
			        bool& rValue) const=0;
			
			/**
			 * \brief Gets the setting deprecation by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean getSettingMissingStatus(
			        const OpenViBE::CIdentifier &rIdentifier,
			       bool& rValue) const=0;
			
			/**
			 * \brief Sets the setting deprecation by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSettingMissingStatus(
				const OpenViBE::uint32 ui32SettingIndex,
				const bool rValue)=0;
				
			/**
			 * \brief Sets the setting deprecation by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean setSettingMissingStatus(
				const OpenViBE::CIdentifier &rIdentifier,
				const bool rValue)=0;
									
			virtual bool swapSettings(unsigned int indexA, unsigned int indexB)=0;
			virtual bool swapInputs(unsigned int indexA, unsigned int indexB)=0;
			virtual bool swapOutputs(unsigned int indexA, unsigned int indexB)=0;
			/**
			 * \brief Inform if the box possess a modifiable interface
			 * \return \e true if it does.
			 * \return \e false otherwise.
			 */
			virtual OpenViBE::boolean hasModifiableSettings(void)const=0;


			virtual OpenViBE::uint32* getModifiableSettings(OpenViBE::uint32& rCount)const =0;


			/**
			 * \brief Update identifier of the setting handled by index
			 *  Identifier is computed by using name and type of the setting. When these attributes have
			 *  changed, it can be usefull to regenerate identifier. The identifier can be suggested if defined.
			 * \param ui32SettingIndex	the setting index
			 * \param suggestedIdentifier the suggested identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */	
			virtual bool updateSettingIdentifier(const unsigned int ui32SettingIndex, const CIdentifier& suggestedIdentifier = OV_UndefinedIdentifier)=0;
			//@}


			/** \name Input/Output management */
			//@{
			/**
			  * \brief Marks this type as supported by inputs
			  * \param rTypeIdentifier [in] : The type identifier
			  * \return \e true in case of success.
			  * \return \e false in case of error.
			  */
			virtual OpenViBE::boolean addInputSupport(
					const OpenViBE::CIdentifier& rTypeIdentifier)=0;

			/**
			  * \brief Indicates if a type is support by inputs
			  * \param rTypeIdentifier [in] : The type identifier
			  * \return \e true if type is support.
			  * \return \e false if type isn't support.
			  */
			virtual OpenViBE::boolean hasInputSupport(
					const OpenViBE::CIdentifier& rTypeIdentifier) const =0;

			/**
			  * \brief Marks this type as supported by outputs
			  * \param rTypeIdentifier [in] : The type identifier
			  * \return \e true in case of success.
			  * \return \e false in case of error.
			  */
			virtual OpenViBE::boolean addOutputSupport(
					const OpenViBE::CIdentifier& rTypeIdentifier)=0;

			/**
			  * \brief Indicates if a type is support by outputs
			  * \param rTypeIdentifier [in] : The type identifier
			  * \return \e true if type is support.
			  * \return \e false if type isn't support.
			  */
			virtual OpenViBE::boolean hasOutputSupport(
					const OpenViBE::CIdentifier& rTypeIdentifier) const =0;
			//@}

			/**
			 * \brief Set the supported stream type for input and output according
			 * to the restriction of the algorithm whose identifier is given in parameter.
			 * \param rTypeIdentifier [in] : identifier of the algorithm
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \note The supported stream list is not reset.
			 */
			virtual OpenViBE::boolean setSupportTypeFromAlgorithmIdentifier(
					const OpenViBE::CIdentifier& rTypeIdentifier)=0;					
			
			/**
			 * \brief Get a vector of input supported types
			 * \return the vector of input supported types
			 */
			virtual std::vector<OpenViBE::CIdentifier> getInputSupportTypes() const = 0;
			
			/**
			 * \brief Get a vector of output supported types
			 * \return the vector of output supported types
			 */
			virtual std::vector<OpenViBE::CIdentifier> getOutputSupportTypes() const = 0;
			
			/**
			 * \brief clear output supported types list
			 */
			virtual void clearOutputSupportTypes() = 0;
			
			/**
			 * \brief clear input supported types list
			 */
			virtual void clearInputSupportTypes() = 0;
			
			_IsDerivedFromClass_(OpenViBE::Kernel::IAttributable, OV_ClassId_Kernel_Scenario_Box)
		};

		typedef OpenViBE::Kernel::IBox IStaticBoxContext;
	};
};

#endif // __OpenViBE_Kernel_Scenario_IBox_H__
