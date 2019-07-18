#pragma once

#include "ovIAttributable.h"
#include "../../ovCObservable.h"

#include <vector>

namespace OpenViBE
{
	namespace Kernel
	{
		enum BoxEventMessageType
		{
			SettingValueUpdate,
			SettingChange,
			SettingDelete,
			SettingAdd,
			SettingsReorder,
			SettingsAllChange
		};

		enum BoxInterfacorType
		{
			Setting,
			Input,
			Output
		};

		class OV_API BoxEventMessage
		{
		public:
			BoxEventMessageType m_eType;
			int32_t m_i32FirstIndex;
			int32_t m_i32SecondIndex;
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
		class OV_API IBox : public IAttributable, public CObservable
		{
		public:

			/** \name Box naming and identification */
			//@{

			/**
			 * \brief Gets the identifier of this box
			 * \return The identifier of this OpenViBE box.
			 */
			virtual CIdentifier getIdentifier(void) const = 0;
			/**
			 * \brief Gets the display name of this box
			 * \return The name of this OpenViBE box.
			 */
			virtual CString getName(void) const = 0;
			/**
			 * \brief Gets the algorithm class identifier
			 * \return This box' algorithm class identifier
			 */
			virtual CIdentifier getAlgorithmClassIdentifier(void) const = 0;
			/**
			 * \brief Changes the identifier of this box
			 * \param rIdentifier [in] : The new identifier
			 *        this box should take.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setIdentifier(
				const CIdentifier& rIdentifier) = 0;
			/**
			 * \brief Renames this box
			 * \param sName [in] : The name this box should take
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setName(
				const CString& rsName) = 0;
			/**
			 * \brief Changes the algorithm identifier of this box
			 * \param rAlgorithmClassIdentifier [in] : The new algorithm
			 *        identifier this box should take.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setAlgorithmClassIdentifier(
				const CIdentifier& rAlgorithmClassIdentifier) = 0;


			/**
			 * \brief Requests for a suggested identifier. If it is already used in the box or
			 * if it is OV_UndefinedIdentifier, a news random one is proposed uniq in the scope of settings.
			 * \param oIdentifier [in] : the requested identifier
			 * \return the proposed identifier
			 */
			virtual CIdentifier getUnusedSettingIdentifier(const CIdentifier& rIdentifier = OV_UndefinedIdentifier) const = 0;

			/**
			 * \brief Requests for a suggested identifier. If it is already used in the box or
			 * if it is OV_UndefinedIdentifier, a news random one is proposed uniq in the scope of the inputs
			 * of the given map object.
			 * \param oIdentifier [in] : the requested identifier
			 * \return the proposed identifier
			 */
			virtual CIdentifier getUnusedInputIdentifier(const CIdentifier& rIdentifier = OV_UndefinedIdentifier) const = 0;

			/**
			 * \brief Requests for a suggested identifier. If it is already used in the box or
			 * if it is OV_UndefinedIdentifier, a news random one is proposed uniq in the scope of the outputs
			 * of the given map object.
			 * \param oIdentifier [in] : the requested identifier
			 * \return the proposed identifier
			 */
			virtual CIdentifier getUnusedOutputIdentifier(const CIdentifier& rIdentifier = OV_UndefinedIdentifier) const = 0;

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
			virtual bool initializeFromAlgorithmClassIdentifier(
				const CIdentifier& rAlgorithmClassIdentifier) = 0;
			/**
			 * \brief Initializes the box from an already existing box
			 * \param rExisitingBox [in] : The existing box.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Resets the box and initializes its input/output/settings
			 * according to the existing box.
			 */
			virtual bool initializeFromExistingBox(
				const IBox& rExistingBox) = 0;


			virtual bool addInterfacor(BoxInterfacorType interfacorType, const CString& newName, const CIdentifier& typeIdentifier, const CIdentifier& identifier, bool shouldNotify = true) = 0;
			virtual bool removeInterfacor(BoxInterfacorType interfacorType, const uint32_t index, bool shouldNotify = true) = 0;

			virtual uint32_t getInterfacorCount(BoxInterfacorType interfacorType) const = 0;
			virtual uint32_t getInterfacorCountIncludingDeprecated(BoxInterfacorType interfacorType) const = 0;

			virtual bool getInterfacorIdentifier(BoxInterfacorType interfacorType, uint32_t index, CIdentifier& identifier) const = 0;

			virtual bool getInterfacorIndex(BoxInterfacorType interfacorType, const CIdentifier& identifier, uint32_t& index) const = 0;
			virtual bool getInterfacorIndex(BoxInterfacorType interfacorType, const CString& name, uint32_t& index) const = 0;

			virtual bool getInterfacorType(BoxInterfacorType interfacorType, uint32_t index, CIdentifier& typeIdentifier) const = 0;
			virtual bool getInterfacorType(BoxInterfacorType interfacorType, const CIdentifier& identifier, CIdentifier& typeIdentifier) const = 0;
			virtual bool getInterfacorType(BoxInterfacorType interfacorType, const CString& name, CIdentifier& typeIdentifier) const = 0;

			virtual bool getInterfacorName(BoxInterfacorType interfacorType, uint32_t index, CString& name) const = 0;
			virtual bool getInterfacorName(BoxInterfacorType interfacorType, const CIdentifier& identifier, CString& name) const = 0;

			virtual bool getInterfacorDeprecatedStatus(BoxInterfacorType interfacorType, uint32_t index, bool& value) const = 0;
			virtual bool getInterfacorDeprecatedStatus(BoxInterfacorType interfacorType, const CIdentifier& identifier, bool& value) const = 0;

			virtual bool hasInterfacorWithIdentifier(BoxInterfacorType interfacorType, const CIdentifier& identifier) const = 0;
			virtual bool hasInterfacorWithType(BoxInterfacorType interfacorType, uint32_t index, const CIdentifier& typeIdentifier) const = 0;
			virtual bool hasInterfacorWithNameAndType(BoxInterfacorType interfacorType, const CString& name, const CIdentifier& typeIdentifier) const = 0;

			virtual bool setInterfacorType(BoxInterfacorType interfacorType, uint32_t index, const CIdentifier& typeIdentifier) = 0;
			virtual bool setInterfacorType(BoxInterfacorType interfacorType, const CIdentifier& identifier, const CIdentifier& typeIdentifier) = 0;
			virtual bool setInterfacorType(BoxInterfacorType interfacorType, const CString& name, const CIdentifier& typeIdentifier) = 0;

			virtual bool setInterfacorName(BoxInterfacorType interfacorType, uint32_t index, const CString& name) = 0;
			virtual bool setInterfacorName(BoxInterfacorType interfacorType, const CIdentifier& identifier, const CString& name) = 0;

			virtual bool setInterfacorDeprecatedStatus(BoxInterfacorType interfacorType, uint32_t index, bool value) = 0;
			virtual bool setInterfacorDeprecatedStatus(BoxInterfacorType interfacorType, const CIdentifier& identifier, bool value) = 0;

			virtual bool updateInterfacorIdentifier(BoxInterfacorType interfacorType, uint32_t index, const CIdentifier& newIdentifier) = 0;

			virtual bool addInterfacorTypeSupport(BoxInterfacorType interfacorType, const CIdentifier& typeIdentifier) = 0;
			virtual bool hasInterfacorTypeSupport(BoxInterfacorType interfacorType, const CIdentifier& typeIdentifier) const = 0;
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
			virtual bool addInput(
				const CString& rsName,
				const CIdentifier& rTypeIdentifier,
				const CIdentifier& oIdentifier = OV_UndefinedIdentifier,
				const bool bNotify                       = true) = 0;

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
			virtual bool removeInput(
				const uint32_t ui32InputIndex,
				const bool bNotify = true) = 0;

			/**
			 * \brief Gets the number of inputs for this box
			 * \return The number of inputs for this box.
			 */
			virtual uint32_t getInputCount(void) const = 0;

			/**
			 * \brief Gets an input type identifier by index
			 * \param ui32InputIndex [in] : The input index
			 * \param rTypeIdentifier [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rTypeIdentifier remains unchanged.
			 */
			virtual bool getInputType(
				const uint32_t ui32InputIndex,
				CIdentifier& rTypeIdentifier) const = 0;

			/**
			 * \brief Gets an input name by index
			 * \param ui32InputIndex [in] : The input index
			 * \param rName [out] : The name of this input
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rName remains unchanged.
			 */
			virtual bool getInputName(
				const uint32_t ui32InputIndex,
				CString& rName) const = 0;

			/**
			 * \brief Sets an input type identifier by index
			 * \param ui32InputIndex [in] : The input index
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setInputType(
				const uint32_t ui32InputIndex,
				const CIdentifier& rTypeIdentifier) = 0;

			/**
			 * \brief Sets an input name by index
			 * \param ui32InputIndex [in] : The input index
			 * \param rName [in] : The name of this input
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setInputName(
				const uint32_t ui32InputIndex,
				const CString& rName) = 0;

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
			virtual bool addOutput(
				const CString& rsName,
				const CIdentifier& rTypeIdentifier,
				const CIdentifier& rIdentifier = OV_UndefinedIdentifier,
				const bool bNotify                       = true) = 0;
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
			virtual bool removeOutput(
				const uint32_t ui32OutputIndex,
				const bool bNotify = true) = 0;
			/**
			 * \brief Gets the number of outputs for this box
			 * \return The number of outputs for this box.
			 */
			virtual uint32_t getOutputCount(void) const = 0;

			/**
			 * \brief Gets an output type identifier by index
			 * \param ui32OutputIndex [in] : The output index
			 * \param rTypeIdentifier [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rTypeIdentifier remains unchanged.
			 */
			virtual bool getOutputType(
				const uint32_t ui32OutputIndex,
				CIdentifier& rTypeIdentifier) const = 0;

			/**
			 * \brief Gets an output name by index
			 * \param ui32OutputIndex [in] : The output index
			 * \param rName [out] : The name of this output
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rName remains unchanged.
			 */
			virtual bool getOutputName(
				const uint32_t ui32OutputIndex,
				CString& rName) const = 0;

			/**
			 * \brief Sets an output type identifier by index
			 * \param ui32OutputIndex [in] : The output index
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setOutputType(
				const uint32_t ui32OutputIndex,
				const CIdentifier& rTypeIdentifier) = 0;

			/**
			 * \brief Sets an output name by index
			 * \param ui32OutputIndex [in] : The output index
			 * \param rName [in] : The name of this output
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setOutputName(
				const uint32_t ui32OutputIndex,
				const CString& rName) = 0;

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
			virtual bool addSetting(
				const CString& rsName,
				const CIdentifier& rTypeIdentifier,
				const CString& sDefaultValue,
				const uint32_t ui32Index         = OV_Value_UndefinedIndexUInt,
				const bool bModifiability                = false,
				const CIdentifier& rIdentifier = OV_UndefinedIdentifier,
				const bool bNotify                       = true) = 0;

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
			virtual bool removeSetting(
				const uint32_t ui32SettingIndex,
				const bool bNotify = true) = 0;
			/**
			 * \brief Gets the number of settings for this box
			 * \return The number of settings for this box.
			 */
			virtual uint32_t getSettingCount(void) const = 0;

			/**
			 * \brief checks if a setting with a given name is present
			 * \param rName [in] : Name of the setting
			 * \return true if setting is present, false otherwise
			 */
			virtual bool hasSettingWithName(
				const CString& rName) const = 0;

			/**
			 * \brief Gets a setting type by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rTypeIdentifier [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rTypeIdentifier remains unchanged.
			 */
			virtual bool getSettingType(
				const uint32_t ui32SettingIndex,
				CIdentifier& rTypeIdentifier) const = 0;

			/**
			 * \brief Gets a setting name by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rName [out] : The name of this setting
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rName remains unchanged.
			 */
			virtual bool getSettingName(
				const uint32_t ui32SettingIndex,
				CString& rName) const = 0;

			/**
			 * \brief Gets the default setting value by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rDefaultValue [out] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rDefaultValue remains unchanged.
			 */
			virtual bool getSettingDefaultValue(
				const uint32_t ui32SettingIndex,
				CString& rDefaultValue) const = 0;

			/**
			 * \brief Gets the default setting value by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rDefaultValue [out] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rDefaultValue remains unchanged.
			 */
			virtual bool getSettingDefaultValue(
				const CIdentifier& rIdentifier,
				CString& rDefaultValue) const = 0;

			/**
			* \brief Gets the default setting value by name
			* \param sName [in] : The setting name
			* \param rDefaultValue [out] : The default value
			* \return \e true in case of success.
			* \return \e false in case of error. In such case,
			*         \c rDefaultValue remains unchanged.
			*/
			virtual bool getSettingDefaultValue(
				const CString& sName,
				CString& rDefaultValue) const = 0;

			/**
			 * \brief Gets the setting value by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rValue remains unchanged.
			 */
			virtual bool getSettingValue(
				const uint32_t ui32SettingIndex,
				CString& rValue) const = 0;

			/**
			 * \brief Gets the setting value by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rValue remains unchanged.
			 */
			virtual bool getSettingValue(
				const CIdentifier& rIdentifier,
				CString& rValue) const = 0;

			/**
			 * \brief Gets the setting value by name
			 * \param sName [in] : The setting name
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rValue remains unchanged.
			 */
			virtual bool getSettingValue(
				const CString& sName,
				CString& rValue) const = 0;

			/**
			 * \brief Sets a setting type identifier by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingType(
				const uint32_t ui32SettingIndex,
				const CIdentifier& rTypeIdentifier) = 0;

			/**
			 * \brief Sets a setting name by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rName [in] : The name of this setting
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingName(
				const uint32_t ui32SettingIndex,
				const CString& rName) = 0;

			/**
			 * \brief Sets the default setting value by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rDefaultValue [in] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingDefaultValue(
				const uint32_t ui32SettingIndex,
				const CString& rDefaultValue) = 0;

			/**
			 * \brief Sets the default setting value by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rDefaultValue [in] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingDefaultValue(
				const CIdentifier& rIdentifier,
				const CString& rDefaultValue) = 0;

			/**
			 * \brief Sets the default setting value by name
			 * \param sName [in] : The setting name
			 * \param rDefaultValue [in] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingDefaultValue(
				const CString& sName,
				const CString& rDefaultValue) = 0;

			/**
			 * \brief Sets the setting value by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingValue(
				const uint32_t ui32SettingIndex,
				const CString& rValue,
				const bool bNotify = true) = 0;

			/**
			 * \brief Sets the setting value by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingValue(
				const CIdentifier& rIdentifier,
				const CString& rValue) = 0;

			/**
			 * \brief Sets the setting value by name
			 * \param sName [in] : The setting name
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingValue(
				const CString& rsName,
				const CString& rValue) = 0;

			/**
			 * \brief Gets the setting modifiability by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool getSettingMod(
				const uint32_t ui32SettingIndex,
				bool& rValue) const = 0;

			/**
			 * \brief Gets the setting modifiability by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool getSettingMod(
				const CIdentifier& rIdentifier,
				bool& rValue) const = 0;

			/**
			 * \brief Gets the setting modifiability by name
			 * \param sName [in] : The setting name
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool getSettingMod(
				const CString& sName,
				bool& rValue) const = 0;

			/**
			 * \brief Sets the setting modifiability by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingMod(
				const uint32_t ui32SettingIndex,
				const bool rValue) = 0;

			/**
			 * \brief Sets the setting modifiability by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingMod(
				const CIdentifier& rIdentifier,
				const bool rValue) = 0;

			/**
			 * \brief Sets the setting modifiability by name
			 * \param sName [in] : The setting name
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingMod(
				const CString& sName,
				const bool rValue) = 0;

			virtual bool swapSettings(unsigned int indexA, unsigned int indexB) = 0;
			virtual bool swapInputs(unsigned int indexA, unsigned int indexB) = 0;
			virtual bool swapOutputs(unsigned int indexA, unsigned int indexB) = 0;
			/**
			 * \brief Inform if the box possess a modifiable interface
			 * \return \e true if it does.
			 * \return \e false otherwise.
			 */
			virtual bool hasModifiableSettings(void) const = 0;


			virtual uint32_t* getModifiableSettings(uint32_t& rCount) const = 0;


			//@}


			/** \name Input/Output management */
			//@{
			/**
			  * \brief Marks this type as supported by inputs
			  * \param rTypeIdentifier [in] : The type identifier
			  * \return \e true in case of success.
			  * \return \e false in case of error.
			  */
			virtual bool addInputSupport(
				const CIdentifier& rTypeIdentifier) = 0;

			/**
			  * \brief Indicates if a type is support by inputs
			  * \param rTypeIdentifier [in] : The type identifier
			  * \return \e true if type is support.
			  * \return \e false if type isn't support.
			  */
			virtual bool hasInputSupport(
				const CIdentifier& rTypeIdentifier) const = 0;

			/**
			  * \brief Marks this type as supported by outputs
			  * \param rTypeIdentifier [in] : The type identifier
			  * \return \e true in case of success.
			  * \return \e false in case of error.
			  */
			virtual bool addOutputSupport(
				const CIdentifier& rTypeIdentifier) = 0;

			/**
			  * \brief Indicates if a type is support by outputs
			  * \param rTypeIdentifier [in] : The type identifier
			  * \return \e true if type is support.
			  * \return \e false if type isn't support.
			  */
			virtual bool hasOutputSupport(
				const CIdentifier& rTypeIdentifier) const = 0;
			//@}

			/**
			 * \brief Set the supported stream type for input and output according
			 * to the restriction of the algorithm whose identifier is given in parameter.
			 * \param rTypeIdentifier [in] : identifier of the algorithm
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \note The supported stream list is not reset.
			 */
			virtual bool setSupportTypeFromAlgorithmIdentifier(
				const CIdentifier& rTypeIdentifier) = 0;

			/**
			 * \brief Get a vector of input supported types
			 * \return the vector of input supported types
			 */
			virtual std::vector<CIdentifier> getInputSupportTypes() const = 0;

			/**
			 * \brief Get a vector of output supported types
			 * \return the vector of output supported types
			 */
			virtual std::vector<CIdentifier> getOutputSupportTypes() const = 0;

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

		typedef IBox IStaticBoxContext;
	};
};
