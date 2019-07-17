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
			virtual OpenViBE::CIdentifier getIdentifier(void) const =0;
			/**
			 * \brief Gets the display name of this box
			 * \return The name of this OpenViBE box.
			 */
			virtual OpenViBE::CString getName(void) const =0;
			/**
			 * \brief Gets the algorithm class identifier
			 * \return This box' algorithm class identifier
			 */
			virtual OpenViBE::CIdentifier getAlgorithmClassIdentifier(void) const =0;
			/**
			 * \brief Changes the identifier of this box
			 * \param rIdentifier [in] : The new identifier
			 *        this box should take.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setIdentifier(
				const OpenViBE::CIdentifier& rIdentifier) =0;
			/**
			 * \brief Renames this box
			 * \param sName [in] : The name this box should take
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setName(
				const OpenViBE::CString& rsName) =0;
			/**
			 * \brief Changes the algorithm identifier of this box
			 * \param rAlgorithmClassIdentifier [in] : The new algorithm
			 *        identifier this box should take.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setAlgorithmClassIdentifier(
				const OpenViBE::CIdentifier& rAlgorithmClassIdentifier) =0;


			/**
			 * \brief Requests for a suggested identifier. If it is already used in the box or
			 * if it is OV_UndefinedIdentifier, a news random one is proposed uniq in the scope of settings.
			 * \param oIdentifier [in] : the requested identifier
			 * \return the proposed identifier
			 */
			virtual CIdentifier getUnusedSettingIdentifier(const CIdentifier& rIdentifier = OV_UndefinedIdentifier) const =0;

			/**
			 * \brief Requests for a suggested identifier. If it is already used in the box or
			 * if it is OV_UndefinedIdentifier, a news random one is proposed uniq in the scope of the inputs
			 * of the given map object.
			 * \param oIdentifier [in] : the requested identifier
			 * \return the proposed identifier
			 */
			virtual CIdentifier getUnusedInputIdentifier(const CIdentifier& rIdentifier = OV_UndefinedIdentifier) const =0;

			/**
			 * \brief Requests for a suggested identifier. If it is already used in the box or
			 * if it is OV_UndefinedIdentifier, a news random one is proposed uniq in the scope of the outputs
			 * of the given map object.
			 * \param oIdentifier [in] : the requested identifier
			 * \return the proposed identifier
			 */
			virtual CIdentifier getUnusedOutputIdentifier(const CIdentifier& rIdentifier = OV_UndefinedIdentifier) const =0;

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
				const OpenViBE::CIdentifier& rAlgorithmClassIdentifier) =0;
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
				const OpenViBE::Kernel::IBox& rExistingBox) =0;


			virtual bool addInterfacor(BoxInterfacorType interfacorType, const OpenViBE::CString& newName, const OpenViBE::CIdentifier& typeIdentifier, const OpenViBE::CIdentifier& identifier, bool shouldNotify = true) = 0;
			virtual bool removeInterfacor(BoxInterfacorType interfacorType, const uint32_t index, bool shouldNotify = true) = 0;

			virtual uint32_t getInterfacorCount(BoxInterfacorType interfacorType) const = 0;
			virtual uint32_t getInterfacorCountIncludingDeprecated(BoxInterfacorType interfacorType) const = 0;

			virtual bool getInterfacorIdentifier(BoxInterfacorType interfacorType, uint32_t index, OpenViBE::CIdentifier& identifier) const = 0;

			virtual bool getInterfacorIndex(BoxInterfacorType interfacorType, const OpenViBE::CIdentifier& identifier, uint32_t& index) const = 0;
			virtual bool getInterfacorIndex(BoxInterfacorType interfacorType, const OpenViBE::CString& name, uint32_t& index) const = 0;

			virtual bool getInterfacorType(BoxInterfacorType interfacorType, uint32_t index, OpenViBE::CIdentifier& typeIdentifier) const = 0;
			virtual bool getInterfacorType(BoxInterfacorType interfacorType, const OpenViBE::CIdentifier& identifier, OpenViBE::CIdentifier& typeIdentifier) const = 0;
			virtual bool getInterfacorType(BoxInterfacorType interfacorType, const CString& name, OpenViBE::CIdentifier& typeIdentifier) const = 0;

			virtual bool getInterfacorName(BoxInterfacorType interfacorType, uint32_t index, OpenViBE::CString& name) const = 0;
			virtual bool getInterfacorName(BoxInterfacorType interfacorType, const OpenViBE::CIdentifier& identifier, OpenViBE::CString& name) const = 0;

			virtual bool getInterfacorDeprecatedStatus(BoxInterfacorType interfacorType, uint32_t index, bool& value) const = 0;
			virtual bool getInterfacorDeprecatedStatus(BoxInterfacorType interfacorType, const OpenViBE::CIdentifier& identifier, bool& value) const = 0;

			virtual bool hasInterfacorWithIdentifier(BoxInterfacorType interfacorType, const OpenViBE::CIdentifier& identifier) const = 0;
			virtual bool hasInterfacorWithType(BoxInterfacorType interfacorType, uint32_t index, const OpenViBE::CIdentifier& typeIdentifier) const = 0;
			virtual bool hasInterfacorWithNameAndType(BoxInterfacorType interfacorType, const CString& name, const OpenViBE::CIdentifier& typeIdentifier) const = 0;

			virtual bool setInterfacorType(BoxInterfacorType interfacorType, uint32_t index, const OpenViBE::CIdentifier& typeIdentifier) = 0;
			virtual bool setInterfacorType(BoxInterfacorType interfacorType, const OpenViBE::CIdentifier& identifier, const OpenViBE::CIdentifier& typeIdentifier) = 0;
			virtual bool setInterfacorType(BoxInterfacorType interfacorType, const CString& name, const OpenViBE::CIdentifier& typeIdentifier) = 0;

			virtual bool setInterfacorName(BoxInterfacorType interfacorType, uint32_t index, const OpenViBE::CString& name) = 0;
			virtual bool setInterfacorName(BoxInterfacorType interfacorType, const OpenViBE::CIdentifier& identifier, const OpenViBE::CString& name) = 0;

			virtual bool setInterfacorDeprecatedStatus(BoxInterfacorType interfacorType, uint32_t index, bool value) = 0;
			virtual bool setInterfacorDeprecatedStatus(BoxInterfacorType interfacorType, const OpenViBE::CIdentifier& identifier, bool value) = 0;

			virtual bool updateInterfacorIdentifier(BoxInterfacorType interfacorType, uint32_t index, const CIdentifier& newIdentifier) = 0;

			virtual bool addInterfacorTypeSupport(BoxInterfacorType interfacorType, const OpenViBE::CIdentifier& typeIdentifier) = 0;
			virtual bool hasInterfacorTypeSupport(BoxInterfacorType interfacorType, const OpenViBE::CIdentifier& typeIdentifier) const = 0;
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
				const OpenViBE::CString& rsName,
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CIdentifier& oIdentifier = OV_UndefinedIdentifier,
				const bool bNotify                       = true) =0;

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
				const OpenViBE::uint32 ui32InputIndex,
				const bool bNotify = true) =0;

			/**
			 * \brief Gets the number of inputs for this box
			 * \return The number of inputs for this box.
			 */
			virtual OpenViBE::uint32 getInputCount(void) const =0;

			/**
			 * \brief Gets an input type identifier by index
			 * \param ui32InputIndex [in] : The input index
			 * \param rTypeIdentifier [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rTypeIdentifier remains unchanged.
			 */
			virtual bool getInputType(
				const OpenViBE::uint32 ui32InputIndex,
				OpenViBE::CIdentifier& rTypeIdentifier) const =0;

			/**
			 * \brief Gets an input name by index
			 * \param ui32InputIndex [in] : The input index
			 * \param rName [out] : The name of this input
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rName remains unchanged.
			 */
			virtual bool getInputName(
				const OpenViBE::uint32 ui32InputIndex,
				OpenViBE::CString& rName) const =0;

			/**
			 * \brief Sets an input type identifier by index
			 * \param ui32InputIndex [in] : The input index
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setInputType(
				const OpenViBE::uint32 ui32InputIndex,
				const OpenViBE::CIdentifier& rTypeIdentifier) =0;

			/**
			 * \brief Sets an input name by index
			 * \param ui32InputIndex [in] : The input index
			 * \param rName [in] : The name of this input
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setInputName(
				const OpenViBE::uint32 ui32InputIndex,
				const OpenViBE::CString& rName) =0;

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
				const OpenViBE::CString& rsName,
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CIdentifier& rIdentifier = OV_UndefinedIdentifier,
				const bool bNotify                       = true) =0;
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
				const OpenViBE::uint32 ui32OutputIndex,
				const bool bNotify = true) =0;
			/**
			 * \brief Gets the number of outputs for this box
			 * \return The number of outputs for this box.
			 */
			virtual OpenViBE::uint32 getOutputCount(void) const =0;

			/**
			 * \brief Gets an output type identifier by index
			 * \param ui32OutputIndex [in] : The output index
			 * \param rTypeIdentifier [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rTypeIdentifier remains unchanged.
			 */
			virtual bool getOutputType(
				const OpenViBE::uint32 ui32OutputIndex,
				OpenViBE::CIdentifier& rTypeIdentifier) const =0;

			/**
			 * \brief Gets an output name by index
			 * \param ui32OutputIndex [in] : The output index
			 * \param rName [out] : The name of this output
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rName remains unchanged.
			 */
			virtual bool getOutputName(
				const OpenViBE::uint32 ui32OutputIndex,
				OpenViBE::CString& rName) const =0;

			/**
			 * \brief Sets an output type identifier by index
			 * \param ui32OutputIndex [in] : The output index
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setOutputType(
				const OpenViBE::uint32 ui32OutputIndex,
				const OpenViBE::CIdentifier& rTypeIdentifier) =0;

			/**
			 * \brief Sets an output name by index
			 * \param ui32OutputIndex [in] : The output index
			 * \param rName [in] : The name of this output
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setOutputName(
				const OpenViBE::uint32 ui32OutputIndex,
				const OpenViBE::CString& rName) =0;

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
				const OpenViBE::CString& rsName,
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sDefaultValue,
				const OpenViBE::uint32 ui32Index         = OV_Value_UndefinedIndexUInt,
				const bool bModifiability                = false,
				const OpenViBE::CIdentifier& rIdentifier = OV_UndefinedIdentifier,
				const bool bNotify                       = true) =0;

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
				const OpenViBE::uint32 ui32SettingIndex,
				const bool bNotify = true) =0;
			/**
			 * \brief Gets the number of settings for this box
			 * \return The number of settings for this box.
			 */
			virtual OpenViBE::uint32 getSettingCount(void) const =0;

			/**
			 * \brief checks if a setting with a given name is present
			 * \param rName [in] : Name of the setting
			 * \return true if setting is present, false otherwise
			 */
			virtual bool hasSettingWithName(
				const OpenViBE::CString& rName) const = 0;

			/**
			 * \brief Gets a setting type by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rTypeIdentifier [out] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rTypeIdentifier remains unchanged.
			 */
			virtual bool getSettingType(
				const OpenViBE::uint32 ui32SettingIndex,
				OpenViBE::CIdentifier& rTypeIdentifier) const =0;

			/**
			 * \brief Gets a setting name by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rName [out] : The name of this setting
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rName remains unchanged.
			 */
			virtual bool getSettingName(
				const OpenViBE::uint32 ui32SettingIndex,
				OpenViBE::CString& rName) const =0;

			/**
			 * \brief Gets the default setting value by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rDefaultValue [out] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rDefaultValue remains unchanged.
			 */
			virtual bool getSettingDefaultValue(
				const OpenViBE::uint32 ui32SettingIndex,
				OpenViBE::CString& rDefaultValue) const =0;

			/**
			 * \brief Gets the default setting value by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rDefaultValue [out] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rDefaultValue remains unchanged.
			 */
			virtual bool getSettingDefaultValue(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::CString& rDefaultValue) const =0;

			/**
			* \brief Gets the default setting value by name
			* \param sName [in] : The setting name
			* \param rDefaultValue [out] : The default value
			* \return \e true in case of success.
			* \return \e false in case of error. In such case,
			*         \c rDefaultValue remains unchanged.
			*/
			virtual bool getSettingDefaultValue(
				const OpenViBE::CString& sName,
				OpenViBE::CString& rDefaultValue) const =0;

			/**
			 * \brief Gets the setting value by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rValue remains unchanged.
			 */
			virtual bool getSettingValue(
				const OpenViBE::uint32 ui32SettingIndex,
				OpenViBE::CString& rValue) const =0;

			/**
			 * \brief Gets the setting value by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rValue remains unchanged.
			 */
			virtual bool getSettingValue(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::CString& rValue) const =0;

			/**
			 * \brief Gets the setting value by name
			 * \param sName [in] : The setting name
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error. In such case,
			 *         \c rValue remains unchanged.
			 */
			virtual bool getSettingValue(
				const OpenViBE::CString& sName,
				OpenViBE::CString& rValue) const =0;

			/**
			 * \brief Sets a setting type identifier by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rTypeIdentifier [in] : The type identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingType(
				const OpenViBE::uint32 ui32SettingIndex,
				const OpenViBE::CIdentifier& rTypeIdentifier) =0;

			/**
			 * \brief Sets a setting name by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rName [in] : The name of this setting
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingName(
				const OpenViBE::uint32 ui32SettingIndex,
				const OpenViBE::CString& rName) =0;

			/**
			 * \brief Sets the default setting value by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rDefaultValue [in] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingDefaultValue(
				const OpenViBE::uint32 ui32SettingIndex,
				const OpenViBE::CString& rDefaultValue) =0;

			/**
			 * \brief Sets the default setting value by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rDefaultValue [in] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingDefaultValue(
				const OpenViBE::CIdentifier& rIdentifier,
				const OpenViBE::CString& rDefaultValue) =0;

			/**
			 * \brief Sets the default setting value by name
			 * \param sName [in] : The setting name
			 * \param rDefaultValue [in] : The default value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingDefaultValue(
				const OpenViBE::CString& sName,
				const OpenViBE::CString& rDefaultValue) =0;

			/**
			 * \brief Sets the setting value by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingValue(
				const OpenViBE::uint32 ui32SettingIndex,
				const OpenViBE::CString& rValue,
				const bool bNotify = true) =0;

			/**
			 * \brief Sets the setting value by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingValue(
				const OpenViBE::CIdentifier& rIdentifier,
				const OpenViBE::CString& rValue) =0;

			/**
			 * \brief Sets the setting value by name
			 * \param sName [in] : The setting name
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingValue(
				const OpenViBE::CString& rsName,
				const OpenViBE::CString& rValue) =0;

			/**
			 * \brief Gets the setting modifiability by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool getSettingMod(
				const OpenViBE::uint32 ui32SettingIndex,
				bool& rValue) const =0;

			/**
			 * \brief Gets the setting modifiability by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool getSettingMod(
				const OpenViBE::CIdentifier& rIdentifier,
				bool& rValue) const =0;

			/**
			 * \brief Gets the setting modifiability by name
			 * \param sName [in] : The setting name
			 * \param rValue [out] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool getSettingMod(
				const OpenViBE::CString& sName,
				bool& rValue) const =0;

			/**
			 * \brief Sets the setting modifiability by index
			 * \param ui32SettingIndex [in] : The setting index
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingMod(
				const OpenViBE::uint32 ui32SettingIndex,
				const bool rValue) =0;

			/**
			 * \brief Sets the setting modifiability by identifier
			 * \param rIdentifier [in] : The setting identifier
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingMod(
				const OpenViBE::CIdentifier& rIdentifier,
				const bool rValue) =0;

			/**
			 * \brief Sets the setting modifiability by name
			 * \param sName [in] : The setting name
			 * \param rValue [in] : The value
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSettingMod(
				const OpenViBE::CString& sName,
				const bool rValue) =0;

			virtual bool swapSettings(unsigned int indexA, unsigned int indexB) =0;
			virtual bool swapInputs(unsigned int indexA, unsigned int indexB) =0;
			virtual bool swapOutputs(unsigned int indexA, unsigned int indexB) =0;
			/**
			 * \brief Inform if the box possess a modifiable interface
			 * \return \e true if it does.
			 * \return \e false otherwise.
			 */
			virtual bool hasModifiableSettings(void) const =0;


			virtual OpenViBE::uint32* getModifiableSettings(OpenViBE::uint32& rCount) const =0;


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
				const OpenViBE::CIdentifier& rTypeIdentifier) =0;

			/**
			  * \brief Indicates if a type is support by inputs
			  * \param rTypeIdentifier [in] : The type identifier
			  * \return \e true if type is support.
			  * \return \e false if type isn't support.
			  */
			virtual bool hasInputSupport(
				const OpenViBE::CIdentifier& rTypeIdentifier) const =0;

			/**
			  * \brief Marks this type as supported by outputs
			  * \param rTypeIdentifier [in] : The type identifier
			  * \return \e true in case of success.
			  * \return \e false in case of error.
			  */
			virtual bool addOutputSupport(
				const OpenViBE::CIdentifier& rTypeIdentifier) =0;

			/**
			  * \brief Indicates if a type is support by outputs
			  * \param rTypeIdentifier [in] : The type identifier
			  * \return \e true if type is support.
			  * \return \e false if type isn't support.
			  */
			virtual bool hasOutputSupport(
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
			virtual bool setSupportTypeFromAlgorithmIdentifier(
				const OpenViBE::CIdentifier& rTypeIdentifier) =0;

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
