#pragma once

#include "../ovkTKernelObject.h"

#include "ovkCScenario.h"

#include <map>
#include <array>

namespace OpenViBE
{
	namespace Kernel
	{
		typedef struct _InterfacorRequest
		{
			uint32_t index         = OV_Value_UndefinedIndexUInt;
			CIdentifier identifier = OV_UndefinedIdentifier;
			CIdentifier typeID     = OV_UndefinedIdentifier;
			CString name;
			bool toBeRemoved;

			// only for settings
			CString defaultValue;
			bool modifiability = false;
			CString value;
		} InterfacorRequest;

		class CBoxUpdater final : public TKernelObject<IKernelObject>
		{
		public:

			CBoxUpdater(CScenario& scenario, IBox* box);
			~CBoxUpdater() override;

			bool initialize();

			const std::map<uint32_t, uint32_t>& getOriginalToUpdatedInterfacorCorrespondence(EBoxInterfacorType interfacorType) const
			{
				return m_OriginalToUpdatedCorrespondence.at(interfacorType);
			}

			IBox& getUpdatedBox() const { return *m_UpdatedBox; }

			bool flaggedForManualUpdate() const
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

			bool isUpdateRequired() const { return m_IsUpdateRequired; }

			static const std::array<CIdentifier, 10> UPDATABLE_ATTRIBUTES;

			_IsDerivedFromClass_Final_(TKernelObject<IKernelObject>, OV_ClassId_Kernel_Scenario_BoxUpdater)

		private:

			static uint32_t getInterfacorIndex(EBoxInterfacorType interfacorType, const IBox& box, const CIdentifier& typeID, const CIdentifier& identifier, const CString& name);
			bool updateInterfacors(EBoxInterfacorType interfacorType);

			/**
			 * \brief Check if supported type have to be updated between the box to be updated and the kernel
			 * \return true when at least input or output supported types have to be updated
			 */
			bool checkForSupportedTypesToBeUpdated() const;

			/**
			 * \brief Check if supported inputs, outputs or settings attributes have to be updated between the box to be updated and the kernel
			 * \return true when at least input, output or settings attributes have to be updated
			 */
			bool checkForSupportedIOSAttributesToBeUpdated() const;

			// pointer to the parent scenario
			CScenario* m_Scenario = nullptr;
			// pointer to the original box to be updated
			IBox* m_SourceBox = nullptr;
			// pointer to the kernel box
			const IBox* m_KernelBox = nullptr;
			// pointer to the updated box. This box will be used to update the prototype of the original box
			IBox* m_UpdatedBox = nullptr;
			// true when updater has been initialized
			bool m_Initialized = false;

			std::map<EBoxInterfacorType, std::map<uint32_t, uint32_t>> m_OriginalToUpdatedCorrespondence;
			bool m_IsUpdateRequired = false;
		};
	} // namespace Kernel
} // namespace OpenViBE
