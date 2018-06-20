#ifndef __OpenViBEKernel_Kernel_Scenario_CBoxUpdater_H__
#define __OpenViBEKernel_Kernel_Scenario_CBoxUpdater_H__

#include "../ovkTKernelObject.h"

#include "ovkCScenario.h"
#include "ovkTBox.hpp"
#include "ovkTAttributable.h"
#include "ovkCBoxProto.h"

#include <map>

namespace OpenViBE
{
	namespace Kernel
	{						
		typedef struct _SIORequest
		{			
			uint m_index = OV_Value_UndefinedIndexUInt;
			CIdentifier m_identifier = OV_UndefinedIdentifier;
			CIdentifier m_typeIdentifier = OV_UndefinedIdentifier;
			CString m_name;
			std::vector<CIdentifier> m_linksIdentifiers;			
		} SIORequest;
		
		typedef struct _SSettingRequest
		{						
			uint m_index = OV_Value_UndefinedIndexUInt;
			CString m_name;
			CIdentifier m_typeIdentifier = OV_UndefinedIdentifier;
			CIdentifier m_identifier = OV_UndefinedIdentifier;
			CString m_defaultValue;                              
            bool m_modifiability = false;
			CString m_value;			
		} SSettingRequest;
				
		class CBoxUpdater : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IBoxUpdater>
		{
		public:
			
			CBoxUpdater(CScenario& scenario, IBox *requestedBox);
			
			virtual ~CBoxUpdater(void);
						
			
			bool isInitialized() const {return m_initialized;};
			
			/**
			 * \brief Build an updated box input prototype according to the result of add and pull requests
			 *		Inputs will be added if they belgon to :
			 *		-	accepted add resuest (from source box to kernel)
			 *		-   rejected pull requests (from source box to kernel)
			 *  CanAdd and CanModify attributes are considered in order to decide to add some rejected request only if they can
			 * belong to the kernel prototype. 
			 *  If not, rejected element is added the end of the prototype and marker as 'Missing'
			 *  Associated links are updated in case of target/source index modification.
			 * 
			 * \param inputsToBeUpdated
			 * \param updatedLinks
			 * \return 
			 */
			bool processInputsRequests(bool &inputsToBeUpdated, std::map<CIdentifier,CLink*> &m_updatedLinks);
			
			
			/**
			 * \brief Build an updated box output prototype according to the result of add and pull requests
			 *		Outputs will be added if they belgon to :
			 *		-	accepted add resuest (from source box to kernel)
			 *		-   rejected pull requests (from source box to kernel)
			 *  CanAdd and CanModify attributes are considered in order to decide to add some rejected request only if they can
			 * belong to the kernel prototype. 
			 *  If not, rejected element is added the end of the prototype and marker as 'Missing'
			 *  Associated links are updated in case of target/source index modification.
			 * 
			 * \param outputsToBeUpdated
			 * \param updatedLinks
			 * \return 
			 */
			bool processOutputsRequests(bool &outputsToBeUpdated, std::map<CIdentifier,CLink*> &m_updatedLinks);
			
			/**
			 * \brief Build an updated box setting prototype according to the result of add and pull requests
			 *		Settings will be added if they belgon to :
			 *		-	accepted add resuest (from source box to kernel)
			 *		-   rejected pull requests (from source box to kernel)
			 *  CanAdd and CanModify attributes are considered in order to decide to add some rejected request only if they can
			 * belong to the kernel prototype. 
			 *  If not, rejected element is added the end of the prototype and marker as 'Missing'
			 *  Associated links are updated in case of target/source index modification.
			 * 
			 * \param settingsToBeUpdated
			 * \param updatedLinks
			 * \return 
			 */
			bool processSettingsRequests(bool &settingsToBeUpdated);
			
			/**
			 * \brief Check if supported type have to be updated between the box to be updated and the kernel
			 * \return		true when at least input or output supported types have to be updated
			 */
			bool checkForSupportedTypesToBeUpdated();
			
			/**
			 * \brief Check if supported inputs, outputs or settings attributes have to be updated between the box to be updated and the kernel
			 * \return		true when at least input, output or settings attributes have to be updated
			 */
			bool checkForSupportedIOSAttributesToBeUpdated();
			
			bool updateIOLinks();
									
			IBox& getUpdatedBox() { return *m_updatedBox;}
			
			const IBox& getKernelBox() const { return *m_kernelBox;}
					
			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IBoxUpdater>, OVK_ClassId_Kernel_Scenario_BoxUpdater)
			
		private:
		
			/**
			 * \brief Add request for inputs from source box to kernel box
			 * \param acceptedInputs [out] std::vector	: accepted requests
			 * \param rejectedInputs [out] std::vector	: rejected requests
			 * \param updatedInputs	 [out] bool			: true when input indexes/identifiers have been updated
			 * \return 
			 */
			bool inputsAddRequests(std::vector<SIORequest> &acceptedInputs, std::vector<SIORequest> &rejectedInputs, bool &updatedInputs);
			/**
			 * \brief Pull request for inputs from source box to kernel box
			 * \param acceptedInputs [out] std::vector	: accepted requests
			 * \param rejectedInputs [out] std::vector	: rejected requests
			 * \param updatedInputs	 [out] bool			: true when input indexes/identifiers have been updated
			 * \return 
			 */
			bool inputsPullRequests(std::vector<SIORequest> &acceptedInputs, std::vector<SIORequest> &rejectedInputs);
			
			/**
			 * \brief Add request for outputs from source box to kernel box
			 * \param acceptedOutputs [out] std::vector	: accepted requests
			 * \param rejectedOutputs [out] std::vector	: rejected requests
			 * \param updatedOutputs	 [out] bool			: true when output indexes/identifiers have been updated
			 * \return 
			 */
			bool outputsAddRequests(std::vector<SIORequest> &acceptedOuputs, std::vector<SIORequest> &rejectedOuputs, bool &updatedOutputs);
			
			/**
			 * \brief Pull request for outputs from source box to kernel box
			 * \param acceptedOutputs [out] std::vector	: accepted requests
			 * \param rejectedOutputs [out] std::vector	: rejected requests
			 * \param updatedOutputs	 [out] bool			: true when output indexes/identifiers have been updated
			 * \return 
			 */
			bool outputsPullRequests(std::vector<SIORequest> &acceptedOuputs, std::vector<SIORequest> &rejectedOuputs);
			
			/**
			 * \brief Add request for settings from source box to kernel box
			 * \param acceptedSettings [out] std::vector	: accepted requests
			 * \param rejectedSettings [out] std::vector	: rejected requests
			 * \param updatedSettings	 [out] bool			: true when setting indexes/identifiers have been updated
			 * \return 
			 */
			bool settingsAddRequests(std::vector<SSettingRequest> &acceptedSettings, std::vector<SSettingRequest> &rejectedSettings, bool &updatedSettings);
			
			/**
			 * \brief Pull request for settings from source box to kernel box
			 * \param acceptedSettings [out] std::vector	: accepted requests
			 * \param rejectedSettings [out] std::vector	: rejected requests
			 * \param updatedSettings	 [out] bool			: true when setting indexes/identifiers have been updated
			 * \return 
			 */
			bool settingsPullRequests(std::vector<SSettingRequest> &acceptedSettings, std::vector<SSettingRequest> &rejectedSettings);
		
			/**
			 * \brief Tells if the prototype of the given box contains setting handled by index, name and / or identifier according to the fact that they are defined or not.
			 * \param box		the box reference
			 * \param index		index of the element to be found
			 * \param name		name of the element to be found
			 * \param typeIdentifier	identifier of the type of element to be found
			 * \param suggestedIdentifier		indentifier of the element to be found
			 * \param initialSourceSettingsCount  size of the original prototype
			 * \return   true when element has been found
			 */
			static bool protoHasSetting(const IBox &box, uint &index, CString &name , CIdentifier &typeIdentifier, CIdentifier &suggestedIdentifier, const unsigned int initialSourceInputsCount);
			
			/**
			 * \brief Tells if the prototype of the given box contains input handled by index, name and / or identifier according to the fact that they are defined or not.
			 * \param box		the box reference
			 * \param index		index of the element to be found
			 * \param name		name of the element to be found
			 * \param typeIdentifier	identifier of the type of element to be found
			 * \param suggestedIdentifier		indentifier of the element to be found
			 * \param initialSourceInputsCount  size of the original prototype
			 * \return   true when element has been found
			 */
			static bool protoHasInput(const IBox &box, uint &index, CString &name , CIdentifier &typeIdentifier, CIdentifier &suggestedIdentifier, const unsigned int initialSourceOutputsCount);
			
			/**
			 * \brief Tells if the prototype of the given box contains output handled by index, name and / or identifier according to the fact that they are defined or not.
			 * \param box		the box reference
			 * \param index		index of the element to be found
			 * \param name		name of the element to be found
			 * \param typeIdentifier	identifier of the type of element to be found
			 * \param suggestedIdentifier		indentifier of the element to be found
			 * \param initialSourceOutputsCount  size of the original prototype
			 * \return   true when element has been found
			 */
			static bool protoHasOutput(const IBox &box, uint &index, CString &name , CIdentifier &typeIdentifier, CIdentifier &suggestedIdentifier, const unsigned int initialSourceSettingsCount);				
			
			// pointer to the parent scenario
			CScenario* m_scenario;
			// pointer to the original box to be updated
			IBox* m_sourceBox;
			// pointer to the kernel box
			const IBox* m_kernelBox;
			// pointer to the updated box. This box will be used to update the prototype of the original box
			IBox* m_updatedBox;
			// true when updater has been initialized
			bool m_initialized;			
			
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Scenario_CBoxUpdater_H__
