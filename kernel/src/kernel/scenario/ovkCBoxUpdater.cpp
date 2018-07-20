#include "../ovkTKernelObject.h"

#include "ovkCScenario.h"
#include "ovkCBoxUpdater.h"
#include "../ovkCObjectVisitorContext.h"

#include "ovkCLink.h"
#include "ovkTBox.hpp"

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;


const std::array<CIdentifier, 6> CBoxUpdater::updatableAttributes = {
    OV_AttributeId_Box_FlagCanAddInput,
    OV_AttributeId_Box_FlagCanModifyInput,
    OV_AttributeId_Box_FlagCanAddOutput,
    OV_AttributeId_Box_FlagCanModifyOutput,
    OV_AttributeId_Box_FlagCanAddSetting,
    OV_AttributeId_Box_FlagCanModifySetting
};

CBoxUpdater::CBoxUpdater(CScenario& scenario, IBox* sourceBox)
    : TKernelObject<IKernelObject>(scenario.getKernelContext())
    , m_Scenario(&scenario)
    , m_SourceBox(sourceBox)
    , m_KernelBox(nullptr)
    , m_UpdatedBox(nullptr)
    , m_Initialized(false)
{
	m_OriginalToUpdatedCorrespondence[Input] = std::map<uint32_t, uint32_t>();
	m_OriginalToUpdatedCorrespondence[Output] = std::map<uint32_t, uint32_t>();
	m_OriginalToUpdatedCorrespondence[Setting] = std::map<uint32_t, uint32_t>();
}

CBoxUpdater::~CBoxUpdater(void)
{
	if (!m_KernelBox || !m_UpdatedBox)
	{
		return;
	}

	if (m_KernelBox->getAlgorithmClassIdentifier() != OVP_ClassId_BoxAlgorithm_Metabox)
	{
		// do not manage destruction of metaboxes (done by metabox manager)
		delete m_KernelBox;
	}

	delete m_UpdatedBox;

}

bool CBoxUpdater::initialize()
{
	// initialize kernel box reference
	if (m_SourceBox->getAlgorithmClassIdentifier() == OVP_ClassId_BoxAlgorithm_Metabox)
	{
			CString metaboxIdentifier = m_SourceBox->getAttributeValue(OVP_AttributeId_Metabox_Identifier);
			OV_ERROR_UNLESS_KRF(metaboxIdentifier != CString(""),
			                    "Failed to find metabox with id " << metaboxIdentifier,
			                    ErrorType::BadCall
			                    );

			OpenViBE::CIdentifier metaboxId;
			metaboxId.fromString(metaboxIdentifier);
			CString metaboxScenarioPath(this->getKernelContext().getMetaboxManager().getMetaboxFilePath(metaboxId));

			OV_ERROR_UNLESS_KRF(metaboxScenarioPath != CString(""),
			                    "Metabox scenario is not available for " << m_SourceBox->getName(),
			                    ErrorType::BadCall);


			// We are going to copy the template scenario, flatten it and then copy all
			// Note that copy constructor for IScenario does not exist
			CIdentifier metaboxScenarioTemplateIdentifier;

			this->getKernelContext().getScenarioManager().importScenarioFromFile(
			            metaboxScenarioTemplateIdentifier,
			            OV_ScenarioImportContext_SchedulerMetaboxImport,
			            metaboxScenarioPath);

			CScenario* metaboxScenarioInstance = dynamic_cast<CScenario*>(&(this->getKernelContext().getScenarioManager().getScenario(metaboxScenarioTemplateIdentifier)));
			metaboxScenarioInstance->setAlgorithmClassIdentifier(OVP_ClassId_BoxAlgorithm_Metabox);
			m_KernelBox = metaboxScenarioInstance;
	}
	else
	{
		CBox* kernelBox = new CBox(m_Scenario->getKernelContext());
		kernelBox->initializeFromAlgorithmClassIdentifierNoInit(m_SourceBox->getAlgorithmClassIdentifier());
		m_KernelBox = kernelBox;
	}

	// initialize updated box
	m_UpdatedBox = new CBox(m_Scenario->getKernelContext());

	m_UpdatedBox->setIdentifier(m_SourceBox->getIdentifier());
	m_UpdatedBox->setName(m_SourceBox->getName());

	// initialize updated box attribute to kernel ones
	CIdentifier attributeIdentifier;
	while ((attributeIdentifier = m_KernelBox->getNextAttributeIdentifier(attributeIdentifier)) != OV_UndefinedIdentifier)
	{
		CString attributeValue = m_KernelBox->getAttributeValue(attributeIdentifier);
		m_UpdatedBox->addAttribute(attributeIdentifier, attributeValue);
	}

	// initialize supported types to kernel ones
	if (m_SourceBox->getAlgorithmClassIdentifier() != OVP_ClassId_BoxAlgorithm_Metabox)
	{
		m_UpdatedBox->setSupportTypeFromAlgorithmIdentifier(m_KernelBox->getAlgorithmClassIdentifier());
	}
	// should not be done before adding IO elements so the box listener is never called
	// updatedBox->setAlgorithmClassIdentifier(kernelBox->getAlgorithmClassIdentifier());
	m_Initialized = true;

	m_IsUpdateRequired = false;

	if (this->flaggedForManualUpdate())
	{
		m_IsUpdateRequired = m_Scenario->doesBoxRequireUpdate(m_SourceBox->getIdentifier());
		return true;
	}

	m_IsUpdateRequired |= this->updateInterfacors(Input);
	m_IsUpdateRequired |= this->updateInterfacors(Output);
	m_IsUpdateRequired |= this->updateInterfacors(Setting);
	m_IsUpdateRequired |= this->checkForSupportedTypesToBeUpdated();
	m_IsUpdateRequired |= this->checkForSupportedIOSAttributesToBeUpdated();

	return true;
}


bool CBoxUpdater::checkForSupportedTypesToBeUpdated()
{
	//check for supported inputs diff
	for (auto& type: m_SourceBox->getInputSupportTypes())
	{
		if (!m_KernelBox->hasInputSupport(type))
		{
			return true;
		}
	}
	for (auto& type: m_KernelBox->getInputSupportTypes())
	{
		if (!m_SourceBox->hasInputSupport(type))
		{
			return true;
		}
	}

	//check for supported outputs diff
	for (auto& type: m_SourceBox->getOutputSupportTypes())
	{
		if (!m_KernelBox->hasOutputSupport(type))
		{
			return true;
		}
	}
	for (auto& type: m_KernelBox->getOutputSupportTypes())
	{
		if (!m_SourceBox->hasOutputSupport(type))
		{
			return true;
		}
	}
	return false;
}

bool CBoxUpdater::checkForSupportedIOSAttributesToBeUpdated()
{
	// check for attributes
	for(auto &attr : CBoxUpdater::updatableAttributes)
	{
		if ((m_SourceBox->hasAttribute(attr) && !m_KernelBox->hasAttribute(attr))
		        || (!m_SourceBox->hasAttribute(attr) && m_KernelBox->hasAttribute(attr)))
		{

			return true;
		}
	}
	return false;
}

bool CBoxUpdater::updateInterfacors(BoxInterfacorType interfacorType)
{
	std::vector<InterfacorRequest> interfacors;

	bool updated = false;

	// First add and optionally modify all requests from the Kernel prototype
	uint32_t index = 0;
	while (index < m_KernelBox->getInterfacorCount(interfacorType))
	{
		CIdentifier kTypeIdentifier;
		CIdentifier kIdentifier;
		CString kName;
		m_KernelBox->getInterfacorType(interfacorType, index, kTypeIdentifier);
		m_KernelBox->getInterfacorIdentifier(interfacorType, index, kIdentifier);
		m_KernelBox->getInterfacorName(interfacorType, index, kName);


		InterfacorRequest request;
		request.index = index;
		request.identifier = kIdentifier;
		request.name = kName;
		request.typeIdentifier = kTypeIdentifier;
		request.toBeRemoved = false;

		if (interfacorType == Setting) {
			CString defaultValue;
			CString value;
			bool modifiable;
			m_KernelBox->getSettingDefaultValue(index, defaultValue);
			m_KernelBox->getSettingValue(index, value);
			m_KernelBox->getSettingMod(index, modifiable);
			request.defaultValue = defaultValue;
			request.value = value;
			request.modifiability = modifiable;
		}

		auto indexInBox = this->getInterfacorIndex(interfacorType, *m_SourceBox, kTypeIdentifier, kIdentifier, kName);
		if (indexInBox != OV_Value_UndefinedIndexUInt)
		{
			CIdentifier identifier;
			CString name;

			m_SourceBox->getInterfacorIdentifier(interfacorType, indexInBox, identifier);
			m_SourceBox->getInterfacorName(interfacorType, indexInBox, name);
			updated |= (identifier == kIdentifier && name != kName) || (identifier != kIdentifier && name == kName);

			interfacors.push_back(request);
			m_OriginalToUpdatedCorrespondence[interfacorType][indexInBox] = index;
		}
		else
		{
			interfacors.push_back(request);
			// try to modify the type in the kernel proto to adjust to the inputs
			updated = true;
		}

		++index;
	}

	// Now go throught the inputs we have not yet associated and decide what to do with them
	// As we currently only support boxes with fixed amount of inputs/outputs this always means that the
	// I/O is redundant
	index = 0;
	while (index < m_SourceBox->getInterfacorCount(interfacorType))
	{
		// Skip if the input was handled in the previous step
		if (m_OriginalToUpdatedCorrespondence.at(interfacorType).find(index) != m_OriginalToUpdatedCorrespondence.at(interfacorType).end())
		{
			++index;
			continue;
		}

		CIdentifier sTypeIdentifier;
		CIdentifier sIdentifier;
		CString sName;
		m_SourceBox->getInterfacorType(interfacorType, index, sTypeIdentifier);
		m_SourceBox->getInterfacorIdentifier(interfacorType, index, sIdentifier);
		m_SourceBox->getInterfacorName(interfacorType, index, sName);

		InterfacorRequest request;
		request.index = index;
		request.identifier = sIdentifier;
		request.name = sName;
		request.typeIdentifier = sTypeIdentifier;
		request.toBeRemoved = true;

		if (interfacorType == Setting) {
			CString defaultValue;
			CString value;
			bool modifiable;
			m_SourceBox->getSettingDefaultValue(index, defaultValue);
			m_SourceBox->getSettingValue(index, value);
			m_SourceBox->getSettingMod(index, modifiable);
			request.defaultValue = defaultValue;
			request.value = value;
			request.modifiability = modifiable;
		}

		updated = true;

		interfacors.push_back(request);

		m_OriginalToUpdatedCorrespondence[interfacorType][index] = static_cast<uint32_t>(interfacors.size() - 1);

		++index;
	}

	for (auto& i : interfacors)
	{
		m_UpdatedBox->addInterfacor(interfacorType, i.name, i.typeIdentifier, i.identifier);
		if (interfacorType == Setting)
		{
			auto index = m_UpdatedBox->getInterfacorCountIncludingDeprecated(Setting) - 1;
			m_UpdatedBox->setSettingDefaultValue(index, i.defaultValue);
			m_UpdatedBox->setSettingValue(index, i.value);
			m_UpdatedBox->setSettingMod(index, i.modifiability);
		}
		if (i.toBeRemoved)
		{
			m_UpdatedBox->setInterfacorDeprecatedStatus(interfacorType, m_UpdatedBox->getInterfacorCountIncludingDeprecated(interfacorType) - 1, true);
		}
	}

	return updated;
}

uint32_t CBoxUpdater::getInterfacorIndex(BoxInterfacorType interfacorType, const IBox& box, const CIdentifier& typeIdentifier, const CIdentifier& identifier, const CString& name)
{
	uint32_t index = OV_Value_UndefinedIndexUInt;
	if (identifier != OV_UndefinedIdentifier && box.hasInterfacorWithIdentifier(interfacorType, identifier))
	{
		box.getInterfacorIndex(interfacorType, identifier, index);
	}
	else if (box.hasInterfacorWithNameAndType(interfacorType, name, typeIdentifier))
	{
		box.getInterfacorIndex(interfacorType, name, index);
	}

	return index;
}

