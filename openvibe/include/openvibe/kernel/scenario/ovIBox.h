#pragma once

#include "ovIAttributable.h"
#include "../../ovCObservable.h"

#include <vector>

namespace OpenViBE
{
	namespace Kernel
	{
		enum EBoxEventMessageType
		{
			SettingValueUpdate,
			SettingChange,
			SettingDelete,
			SettingAdd,
			SettingsReorder,
			SettingsAllChange
		};

		enum EBoxInterfacorType
		{
			Setting,
			Input,
			Output
		};

		class OV_API BoxEventMessage
		{
		public:
			EBoxEventMessageType m_Type;
			int m_FirstIndex  = 0;
			int m_SecondIndex = 0;
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
			virtual CIdentifier getIdentifier() const = 0;
			/**
			 * \brief Gets the display name of this box
			 * \return The name of this OpenViBE box.
			 */
			virtual CString getName() const = 0;
			/**
			 * \brief Gets the algorithm class identifier
			 * \return This box' algorithm class identifier
			 */
			virtual CIdentifier getAlgorithmClassIdentifier() const = 0;
			/**
			 * \brief Changes the identifier of this box
			 * \param identifier [in] : The new identifier
			 *        this box should take.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setIdentifier(const CIdentifier& identifier) = 0;
			/**
			 * \brief Renames this box
			 * \param name [in] : The name this box should take
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setName(const CString& name) = 0;
			/**
			 * \brief Changes the algorithm identifier of this box
			 * \param algorithmClassID [in] : The new algorithm
			 *        identifier this box should take.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setAlgorithmClassIdentifier(const CIdentifier& algorithmClassID) = 0;


			/**
			 * \brief Requests for a suggested identifier. If it is already used in the box or
			 * if it is OV_UndefinedIdentifier, a news random one is proposed uniq in the scope of settings.
			 * \param identifier [in] : the requested identifier
			 * \return the proposed identifier
			 */
			virtual CIdentifier getUnusedSettingIdentifier(const CIdentifier& identifier = OV_UndefinedIdentifier) const = 0;

			/**
			 * \brief Requests for a suggested identifier. If it is already used in the box or
			 * if it is OV_UndefinedIdentifier, a news random one is proposed uniq in the scope of the inputs
			 * of the given map object.
			 * \param identifier [in] : the requested identifier
			 * \return the proposed identifier
			 */
			virtual CIdentifier getUnusedInputIdentifier(const CIdentifier& identifier = OV_UndefinedIdentifier) const = 0;

			/**
			 * \brief Requests for a suggested identifier. If it is already used in the box or
			 * if it is OV_UndefinedIdentifier, a news random one is proposed uniq in the scope of the outputs
			 * of the given map object.
			 * \param identifier [in] : the requested identifier
			 * \return the proposed identifier
			 */
			virtual CIdentifier getUnusedOutputIdentifier(const CIdentifier& identifier = OV_UndefinedIdentifier) const = 0;

			//@}
			/** \name Initialisation from prototypes etc... */
			//@{

			/**
			 * \brief Initializes the box from box algorithm descriptor
			 * \param algorithmClassID [in] : The new algorithm
			 *        identifier this box should take.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Resets the box and initializes its input/output/settings
			 * according to the box algorithm descriptor
			 */
			virtual bool initializeFromAlgorithmClassIdentifier(const CIdentifier& algorithmClassID) = 0;
			/**
			 * \brief Initializes the box from an already existing box
			 * \param existingBox [in] : The existing box.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Resets the box and initializes its input/output/settings
			 * according to the existing box.
			 */
			virtual bool initializeFromExistingBox(const IBox& existingBox) = 0;


			virtual bool addInterfacor(const EBoxInterfacorType interfacorType, const CString& newName, const CIdentifier& typeID, const CIdentifier& identifier, bool shouldNotify = true) = 0;
			virtual bool removeInterfacor(const EBoxInterfacorType interfacorType, const uint32_t index, const bool shouldNotify = true) = 0;

			virtual uint32_t getInterfacorCount(const EBoxInterfacorType interfacorType) const = 0;
			virtual uint32_t getInterfacorCountIncludingDeprecated(const EBoxInterfacorType interfacorType) const = 0;

			virtual bool getInterfacorIdentifier(const EBoxInterfacorType interfacorType, const uint32_t index, CIdentifier& identifier) const = 0;

			virtual bool getInterfacorIndex(const EBoxInterfacorType interfacorType, const CIdentifier& identifier, uint32_t& index) const = 0;
			virtual bool getInterfacorIndex(const EBoxInterfacorType interfacorType, const CString& name, uint32_t& index) const = 0;

			virtual bool getInterfacorType(const EBoxInterfacorType interfacorType, const uint32_t index, CIdentifier& typeID) const = 0;
			virtual bool getInterfacorType(const EBoxInterfacorType interfacorType, const CIdentifier& identifier, CIdentifier& typeID) const = 0;
			virtual bool getInterfacorType(const EBoxInterfacorType interfacorType, const CString& name, CIdentifier& typeID) const = 0;

			virtual bool getInterfacorName(const EBoxInterfacorType interfacorType, const uint32_t index, CString& name) const = 0;
			virtual bool getInterfacorName(const EBoxInterfacorType interfacorType, const CIdentifier& identifier, CString& name) const = 0;

			virtual bool getInterfacorDeprecatedStatus(const EBoxInterfacorType interfacorType, const uint32_t index, bool& value) const = 0;
			virtual bool getInterfacorDeprecatedStatus(const EBoxInterfacorType interfacorType, const CIdentifier& identifier, bool& value) const = 0;

			virtual bool hasInterfacorWithIdentifier(const EBoxInterfacorType interfacorType, const CIdentifier& identifier) const = 0;
			virtual bool hasInterfacorWithType(const EBoxInterfacorType interfacorType, const uint32_t index, const CIdentifier& typeID) const = 0;
			virtual bool hasInterfacorWithNameAndType(const EBoxInterfacorType interfacorType, const CString& name, const CIdentifier& typeID) const = 0;

			virtual bool setInterfacorType(const EBoxInterfacorType interfacorType, const uint32_t index, const CIdentifier& typeID) = 0;
			virtual bool setInterfacorType(const EBoxInterfacorType interfacorType, const CIdentifier& identifier, const CIdentifier& typeID) = 0;
			virtual bool setInterfacorType(const EBoxInterfacorType interfacorType, const CString& name, const CIdentifier& typeID) = 0;

			virtual bool setInterfacorName(const EBoxInterfacorType interfacorType, const uint32_t index, const CString& name) = 0;
			virtual bool setInterfacorName(const EBoxInterfacorType interfacorType, const CIdentifier& identifier, const CString& name) = 0;

			virtual bool setInterfacorDeprecatedStatus(const EBoxInterfacorType interfacorType, const uint32_t index, const bool value) = 0;
			virtual bool setInterfacorDeprecatedStatus(const EBoxInterfacorType interfacorType, const CIdentifier& identifier, const bool value) = 0;

			virtual bool updateInterfacorIdentifier(const EBoxInterfacorType interfacorType, const uint32_t index, const CIdentifier& newID) = 0;

			virtual bool addInterfacorTypeSupport(const EBoxInterfacorType interfacorType, const CIdentifier& typeID) = 0;
			virtual bool hasInterfacorTypeSupport(const EBoxInterfacorType interfacorType, const CIdentifier& typeID) const = 0;
			//@}
			/** \name Input management */
			//@{

			/**
			 * \brief Adds an input to this box
			 * \param name [in] : The input name
			 * \param typeID [in] : The
			 *        input type identifier
			 * \param identifier [in] : The input identifier
			 * \param notify: if true, activate notification callback (true by default)
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * The input is always added after the last
			 * already existing input.
			 */
			virtual bool addInput(const CString& name, const CIdentifier& typeID, const CIdentifier& identifier = OV_UndefinedIdentifier, const bool notify = true) = 0;

			/**
			 * \brief Removes an input for this box
			 * \param index [in] : The index
			 *        of the input to be removed
			 * \param notify manage notify. Is enabled by default
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Inputs coming after the removed input
			 * have their indices changing after this,
			 * they all decrease by 1.
			 */
			virtual bool removeInput(const uint32_t index, const bool notify = true) = 0;

			/**
			 * \brief Gets the number of inputs for this box
			 * \return The number of inputs for this box.
			 */
			virtual uint32_t getInputCount() const = 0;

			/**
			 * \brief Gets an input type identifier by index
			 * \param index [in] : The input index
			 * \param typeID [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c typeID remains unchanged.
			 */
			virtual bool getInputType(const uint32_t index, CIdentifier& typeID) const = 0;

			/**
			 * \brief Gets an input name by index
			 * \param index [in] : The input index
			 * \param name [out] : The name of this input
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c name remains unchanged.
			 */
			virtual bool getInputName(const uint32_t index, CString& name) const = 0;

			/**
			 * \brief Sets an input type identifier by index
			 * \param index [in] : The input index
			 * \param typeID [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setInputType(const uint32_t index, const CIdentifier& typeID) = 0;

			/**
			 * \brief Sets an input name by index
			 * \param index [in] : The input index
			 * \param name [in] : The name of this input
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setInputName(const uint32_t index, const CString& name) = 0;

			//@}
			/** \name Output management */
			//@{

			/**
			 * \brief Adds an output to this box
			 * \param name [in] : The output name
			 * \param typeID [in] : The
			 *        output type idenfitier
			 * \param identifier [in] : The output identifier
			 * \param notify: if true, activate notification callback (true by default)
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * The output is always added after the last
			 * already existing output.
			 */
			virtual bool addOutput(const CString& name, const CIdentifier& typeID, const CIdentifier& identifier = OV_UndefinedIdentifier, const bool notify = true) = 0;

			/**
			 * \brief Removes an output for this box
			 * \param index [in] : The index
			 *        of the output to remove
			 * \param notify manage notify. Is enabled by default
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Outputs coming after the removed output
			 * have their indices changing after this,
			 * they all decrease by 1.
			 */
			virtual bool removeOutput(const uint32_t index, const bool notify = true) = 0;
			/**
			 * \brief Gets the number of outputs for this box
			 * \return The number of outputs for this box.
			 */
			virtual uint32_t getOutputCount() const = 0;

			/**
			 * \brief Gets an output type identifier by index
			 * \param index [in] : The output index
			 * \param typeID [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c typeID remains unchanged.
			 */
			virtual bool getOutputType(const uint32_t index, CIdentifier& typeID) const = 0;

			/**
			 * \brief Gets an output name by index
			 * \param index [in] : The output index
			 * \param name [out] : The name of this output
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c name remains unchanged.
			 */
			virtual bool getOutputName(const uint32_t index, CString& name) const = 0;

			/**
			 * \brief Sets an output type identifier by index
			 * \param index [in] : The output index
			 * \param typeID [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setOutputType(const uint32_t index, const CIdentifier& typeID) = 0;

			/**
			 * \brief Sets an output name by index
			 * \param index [in] : The output index
			 * \param name [in] : The name of this output
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setOutputName(const uint32_t index, const CString& name) = 0;

			//@}
			/** \name Setting management */
			//@{

			/**
			 * \brief Adds a setting to this box
			 * \param name [in] : The setting name
			 * \param typeID [in] : The
			 *        setting type identifier
			 * \param sDefaultValue [in] : The default
			 *        value for this setting
			 * \param index [in] : The index where to
			 *        add the setting
			 * \param bModifiability [in] : true if modifiable setting
			 * \param identifier [in] : The setting identifier
			 * \param notify: if true, activate notification callback (true by default)
			 *
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * The setting is added to the index required.
			 * The default value -1 means that the setting
			 * will be add to the end.
			 */
			virtual bool addSetting(const CString& name, const CIdentifier& typeID, const CString& sDefaultValue, const uint32_t index = OV_Value_UndefinedIndexUInt,
									const bool bModifiability = false, const CIdentifier& identifier = OV_UndefinedIdentifier, const bool notify = true) = 0;

			/**
			 * \brief Removes a setting for this box
			 * \param index [in] : The index
			 *        of the setting to remove
			 * \param notify manage notify. Is enabled by default
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Settings coming after the removed setting
			 * have their indices changing after this,
			 * they all decrease by 1.
			 */
			virtual bool removeSetting(const uint32_t index, const bool notify = true) = 0;
			/**
			 * \brief Gets the number of settings for this box
			 * \return The number of settings for this box.
			 */
			virtual uint32_t getSettingCount() const = 0;

			/**
			 * \brief checks if a setting with a given name is present
			 * \param name [in] : Name of the setting
			 * \return true if setting is present, false otherwise
			 */
			virtual bool hasSettingWithName(const CString& name) const = 0;

			/**
			 * \brief Gets a setting type by index
			 * \param index [in] : The setting index
			 * \param typeID [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c typeID remains unchanged.
			 */
			virtual bool getSettingType(const uint32_t index, CIdentifier& typeID) const = 0;

			/**
			 * \brief Gets a setting name by index
			 * \param index [in] : The setting index
			 * \param name [out] : The name of this setting
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c name remains unchanged.
			 */
			virtual bool getSettingName(const uint32_t index, CString& name) const = 0;

			/**
			 * \brief Gets the default setting value by index
			 * \param index [in] : The setting index
			 * \param defaultValue [out] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c defaultValue remains unchanged.
			 */
			virtual bool getSettingDefaultValue(const uint32_t index, CString& defaultValue) const = 0;

			/**
			 * \brief Gets the default setting value by identifier
			 * \param identifier [in] : The setting identifier
			 * \param defaultValue [out] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c defaultValue remains unchanged.
			 */
			virtual bool getSettingDefaultValue(const CIdentifier& identifier, CString& defaultValue) const = 0;

			/**
			* \brief Gets the default setting value by name
			* \param name [in] : The setting name
			* \param defaultValue [out] : The default value
			* \return \e true in case of success.
			* \return \e false in case of error. In such case,
			*         \c defaultValue remains unchanged.
			*/
			virtual bool getSettingDefaultValue(const CString& name, CString& defaultValue) const = 0;

			/**
			 * \brief Gets the setting value by index
			 * \param index [in] : The setting index
			 * \param value [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c value remains unchanged.
			 */
			virtual bool getSettingValue(const uint32_t index, CString& value) const = 0;

			/**
			 * \brief Gets the setting value by identifier
			 * \param identifier [in] : The setting identifier
			 * \param value [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c value remains unchanged.
			 */
			virtual bool getSettingValue(const CIdentifier& identifier, CString& value) const = 0;

			/**
			 * \brief Gets the setting value by name
			 * \param name [in] : The setting name
			 * \param value [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c value remains unchanged.
			 */
			virtual bool getSettingValue(const CString& name, CString& value) const = 0;

			/**
			 * \brief Sets a setting type identifier by index
			 * \param index [in] : The setting index
			 * \param typeID [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingType(const uint32_t index, const CIdentifier& typeID) = 0;

			/**
			 * \brief Sets a setting name by index
			 * \param index [in] : The setting index
			 * \param name [in] : The name of this setting
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingName(const uint32_t index, const CString& name) = 0;

			/**
			 * \brief Sets the default setting value by index
			 * \param index [in] : The setting index
			 * \param defaultValue [in] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingDefaultValue(const uint32_t index, const CString& defaultValue) = 0;

			/**
			 * \brief Sets the default setting value by identifier
			 * \param identifier [in] : The setting identifier
			 * \param defaultValue [in] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingDefaultValue(const CIdentifier& identifier, const CString& defaultValue) = 0;

			/**
			 * \brief Sets the default setting value by name
			 * \param name [in] : The setting name
			 * \param defaultValue [in] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingDefaultValue(const CString& name, const CString& defaultValue) = 0;

			/**
			 * \brief Sets the setting value by index
			 * \param index [in] : The setting index
			 * \param value [in] : The value
			 * \param notify : allow notification
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingValue(const uint32_t index, const CString& value, const bool notify = true) = 0;

			/**
			 * \brief Sets the setting value by identifier
			 * \param identifier [in] : The setting identifier
			 * \param value [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingValue(const CIdentifier& identifier, const CString& value) = 0;

			/**
			 * \brief Sets the setting value by name
			 * \param name [in] : The setting name
			 * \param value [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingValue(const CString& name, const CString& value) = 0;

			/**
			 * \brief Gets the setting modifiability by index
			 * \param index [in] : The setting index
			 * \param value [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool getSettingMod(const uint32_t index, bool& value) const = 0;

			/**
			 * \brief Gets the setting modifiability by identifier
			 * \param identifier [in] : The setting identifier
			 * \param value [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool getSettingMod(const CIdentifier& identifier, bool& value) const = 0;

			/**
			 * \brief Gets the setting modifiability by name
			 * \param name [in] : The setting name
			 * \param value [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool getSettingMod(const CString& name, bool& value) const = 0;

			/**
			 * \brief Sets the setting modifiability by index
			 * \param index [in] : The setting index
			 * \param value [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingMod(const uint32_t index, const bool value) = 0;

			/**
			 * \brief Sets the setting modifiability by identifier
			 * \param identifier [in] : The setting identifier
			 * \param value [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingMod(const CIdentifier& identifier, const bool value) = 0;

			/**
			 * \brief Sets the setting modifiability by name
			 * \param name [in] : The setting name
			 * \param value [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingMod(const CString& name, const bool value) = 0;

			virtual bool swapSettings(const uint32_t indexA, const uint32_t indexB) = 0;
			virtual bool swapInputs(const uint32_t indexA, const uint32_t indexB) = 0;
			virtual bool swapOutputs(const uint32_t indexA, const uint32_t indexB) = 0;
			/**
			 * \brief Inform if the box possess a modifiable interface
			 * \return \e true if it does.
			 * \return \e false otherwise.
			 */
			virtual bool hasModifiableSettings() const = 0;


			virtual uint32_t* getModifiableSettings(uint32_t& rCount) const = 0;


			//@}


			/** \name Input/Output management */
			//@{
			/**
			  * \brief Marks this type as supported by inputs
			  * \param typeID [in] : The type identifier
			  * \return \e true in case of success.
			  * \return \e false in case of error.
			  */
			virtual bool addInputSupport(const CIdentifier& typeID) = 0;

			/**
			  * \brief Indicates if a type is support by inputs
			  * \param typeID [in] : The type identifier
			  * \return \e true if type is support.
			  * \return \e false if type isn't support.
			  */
			virtual bool hasInputSupport(const CIdentifier& typeID) const = 0;

			/**
			  * \brief Marks this type as supported by outputs
			  * \param typeID [in] : The type identifier
			  * \return \e true in case of success.
			  * \return \e false in case of error.
			  */
			virtual bool addOutputSupport(const CIdentifier& typeID) = 0;

			/**
			  * \brief Indicates if a type is support by outputs
			  * \param typeID [in] : The type identifier
			  * \return \e true if type is support.
			  * \return \e false if type isn't support.
			  */
			virtual bool hasOutputSupport(const CIdentifier& typeID) const = 0;
			//@}

			/**
			 * \brief Set the supported stream type for input and output according
			 * to the restriction of the algorithm whose identifier is given in parameter.
			 * \param typeID [in] : identifier of the algorithm
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \note The supported stream list is not reset.
			 */
			virtual bool setSupportTypeFromAlgorithmIdentifier(const CIdentifier& typeID) = 0;

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
	} // namespace Kernel
} // namespace OpenViBE
