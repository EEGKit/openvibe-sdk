#pragma once

#include "../ovkTKernelObject.h"

#include "ovkCScenario.h"
#include "ovkTBox.hpp"
#include "ovkTAttributable.h"
#include "ovkCBoxProto.h"

#include <map>
#include <array>

namespace OpenViBE
{
	namespace Kernel
	{
		typedef struct _InterfacorRequest
		{
			uint32_t index = OV_Value_UndefinedIndexUInt;
			CIdentifier identifier = OV_UndefinedIdentifier;
			CIdentifier typeIdentifier = OV_UndefinedIdentifier;
			CString name;
			bool toBeRemoved;

			// only for settings
			CString defaultValue;
			bool modifiability = false;
			CString value;
		} InterfacorRequest;

		class CBoxUpdater : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IKernelObject>
		{
		public:

			CBoxUpdater(CScenario& scenario, IBox *requestedBox);

			virtual ~CBoxUpdater(void);

			bool initialize();

			const std::map<uint32_t, uint32_t>& getOriginalToUpdatedInterfacorCorrespondence(BoxInterfacorType interfacorType) const
			{
				return m_OriginalToUpdatedCorrespondence.at(interfacorType);
			}

			IBox& getUpdatedBox()
			{
				return *m_UpdatedBox;
			}

			bool flaggedForManualUpdate()
			{
				OV_FATAL_UNLESS_K(m_Initialized, "Box Updater is not initialized", ErrorType::BadCall);

				return m_KernelBox->hasAttribute(OV_AttributeId_Box_FlagNeedsManualUpdate)
				        || m_KernelBox->hasAttribute(OV_AttributeId_Box_FlagCanAddInput)
				        || m_KernelBox->hasAttribute(OV_AttributeId_Box_FlagCanModifyInput)
				        || m_KernelBox->hasAttribute(OV_AttributeId_Box_FlagCanAddOutput)
				        || m_KernelBox->hasAttribute(OV_AttributeId_Box_FlagCanModifyOutput)
				        || m_KernelBox->hasAttribute(OV_AttributeId_Box_FlagCanAddSetting)
				        || m_KernelBox->hasAttribute(OV_AttributeId_Box_FlagCanModifySetting);
			}

			bool isUpdateRequired()
			{
				return m_IsUpdateRequired;
			}

			static const std::array<CIdentifier, 10> updatableAttributes;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Scenario_BoxUpdater)

		private:

			static uint32_t getInterfacorIndex(BoxInterfacorType interfacorType, const IBox& box, const CIdentifier& typeIdentifier, const CIdentifier& identifier, const CString& name);
			bool updateInterfacors(BoxInterfacorType interfacorType);

			/**
			 * \brief Check if supported type have to be updated between the box to be updated and the kernel
			 * \return true when at least input or output supported types have to be updated
			 */
			bool checkForSupportedTypesToBeUpdated();

			/**
			 * \brief Check if supported inputs, outputs or settings attributes have to be updated between the box to be updated and the kernel
			 * \return true when at least input, output or settings attributes have to be updated
			 */
			bool checkForSupportedIOSAttributesToBeUpdated();

			// pointer to the parent scenario
			CScenario* m_Scenario;
			// pointer to the original box to be updated
			IBox* m_SourceBox;
			// pointer to the kernel box
			const IBox* m_KernelBox;
			// pointer to the updated box. This box will be used to update the prototype of the original box
			IBox* m_UpdatedBox;
			// true when updater has been initialized
			bool m_Initialized;

			std::map<BoxInterfacorType, std::map<uint32_t, uint32_t>> m_OriginalToUpdatedCorrespondence;
			bool m_IsUpdateRequired;


		};
	};
};

