#include "../ovkTKernelObject.h"

#include "ovkCScenario.h"
#include "ovkCBoxUpdater.h"
#include "../ovkCObjectVisitorContext.h"

#include "ovkCLink.h"
#include "ovkTBox.hpp"

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;


namespace
{	
	struct aior_inf_bior
	{
		inline bool operator() (SIORequest a , SIORequest b)
		{
			return a.m_index < b.m_index;
		}
	};
	
	struct asr_inf_bsr
	{
		inline bool operator() (SSettingRequest a , SSettingRequest b)
		{
			return a.m_index < b.m_index;
		}
	};
};

//___________________________________________________________________//
//                                                                   //

CBoxUpdater::CBoxUpdater(CScenario& scenario, IBox *sourceBox)	
: TKernelObject<IBoxUpdater>(scenario.getKernelContext()),
m_kernelBox(NULL),
m_updatedBox(NULL),
m_initialized(false)
{	
	m_scenario = &scenario;
	m_sourceBox = sourceBox;
	
	// initialize kernel box reference	
	if (m_sourceBox->getAlgorithmClassIdentifier() == OVP_ClassId_BoxAlgorithm_Metabox)
	{
			CString l_sMetaboxIdentifier = m_sourceBox->getAttributeValue(OVP_AttributeId_Metabox_Identifier);
			OpenViBE::CIdentifier metaboxId;
			metaboxId.fromString(l_sMetaboxIdentifier);
			CString l_sMetaboxScenarioPath(this->getKernelContext().getMetaboxManager().getMetaboxFilePath(metaboxId));
            
			if (l_sMetaboxIdentifier == CString(""))
			{
				OV_WARNING_K("Failed to find metabox with id " << l_sMetaboxIdentifier);
				return;
			}
            
			// We are going to copy the template scenario, flatten it and then copy all
			// Note that copy constructor for IScenario does not exist
			CIdentifier l_oMetaboxScenarioTemplateIdentifier;

			this->getKernelContext().getScenarioManager().importScenarioFromFile(
			                l_oMetaboxScenarioTemplateIdentifier,
			                OV_ScenarioImportContext_SchedulerMetaboxImport,
			                l_sMetaboxScenarioPath);
						
			CScenario* l_rMetaboxScenarioInstance = dynamic_cast<CScenario*>(&(this->getKernelContext().getScenarioManager().getScenario(l_oMetaboxScenarioTemplateIdentifier)));
		    l_rMetaboxScenarioInstance->setAlgorithmClassIdentifier(OVP_ClassId_BoxAlgorithm_Metabox);
		    m_kernelBox = l_rMetaboxScenarioInstance;
	}
	else
	{
		CBox * kernelBox = new CBox(scenario.getKernelContext());			
		kernelBox->initializeFromAlgorithmClassIdentifierNoInit(m_sourceBox->getAlgorithmClassIdentifier());
		m_kernelBox = kernelBox;
	}		
	
	// initialize updated box
	m_updatedBox = new CBox(m_scenario->getKernelContext());
	
	m_updatedBox->setIdentifier(m_sourceBox->getIdentifier());
	m_updatedBox->setName(m_sourceBox->getName());
	
	// initialize updated box attribute to kernel ones
	CIdentifier attributeIdentifier;
	while ((attributeIdentifier = m_kernelBox->getNextAttributeIdentifier(attributeIdentifier)) != OV_UndefinedIdentifier)
	{
		CString attributeValue = m_kernelBox->getAttributeValue(attributeIdentifier);
		m_updatedBox->addAttribute(attributeIdentifier, attributeValue);
	}
	
	// initialize supported types to kernel ones
	if (m_sourceBox->getAlgorithmClassIdentifier() != OVP_ClassId_BoxAlgorithm_Metabox)
	{
		m_updatedBox->setSupportTypeFromAlgorithmIdentifier(m_kernelBox->getAlgorithmClassIdentifier());
	}
	// should not be done before adding IO elements so the box listener is never called
	// updatedBox->setAlgorithmClassIdentifier(m_kernelBox->getAlgorithmClassIdentifier());
	m_initialized = true;
}


CBoxUpdater::~CBoxUpdater(void)
{
	if (!m_kernelBox || !m_updatedBox)
	{
		return;
	}
	
	if (m_kernelBox->getAlgorithmClassIdentifier() != OVP_ClassId_BoxAlgorithm_Metabox)
	{
		// do not manage destruction of metaboxes (done by metabox manager)
		delete m_kernelBox;				
	}
	
	delete m_updatedBox;
	
}

bool CBoxUpdater::checkForSupportedTypesToBeUpdated()
{
	//check for supported inputs diff
	for (auto& type: m_sourceBox->getInputSupportTypes())
	{
		if (!m_kernelBox->hasInputSupport(type))
		{
			return true;
		}
	}
	for (auto& type: m_kernelBox->getInputSupportTypes())
	{
		if (!m_sourceBox->hasInputSupport(type))
		{
			return true;
		}
	}
	
	//check for supported outputs diff
	for (auto& type: m_sourceBox->getOutputSupportTypes())
	{
		if (!m_kernelBox->hasOutputSupport(type))
		{
			return true;
		}
	}
	for (auto& type: m_kernelBox->getOutputSupportTypes())
	{
		if (!m_sourceBox->hasOutputSupport(type))
		{
			return true;
		}
	}
	return false;
}

bool CBoxUpdater::checkForSupportedIOSAttributesToBeUpdated()
{
	std::vector<CIdentifier> attributes = {
	    OV_AttributeId_Box_FlagCanAddInput,
	    OV_AttributeId_Box_FlagCanModifyInput,
	    OV_AttributeId_Box_FlagCanAddOutput,
	    OV_AttributeId_Box_FlagCanModifyOutput,
	    OV_AttributeId_Box_FlagCanAddSetting,
	    OV_AttributeId_Box_FlagCanModifySetting
	};
	
	// check for attributes
	for(auto &attr :attributes)
	{
		if (
		        (m_sourceBox->hasAttribute(attr) && !m_kernelBox->hasAttribute(attr))
		        ||
		        (!m_sourceBox->hasAttribute(attr) && m_kernelBox->hasAttribute(attr))
		        )
		{			
			
			return true;
		}
	}
	return false;
}
 
bool CBoxUpdater::processInputsRequests(bool &inputsToBeUpdated, std::map<CIdentifier,CLink*> &updatedLinks)
{
	inputsToBeUpdated = false;
	// process to inputs add requests
	std::vector<SIORequest> rejectedAddInputs;
	std::vector<SIORequest> rejectedPullInputs;
	std::vector<SIORequest> acceptedAddInputs;
	std::vector<SIORequest> acceptedPullInputs;
	
	// add request and sort result
	bool updatedInputs = false;
	inputsAddRequests(acceptedAddInputs, rejectedAddInputs, updatedInputs);
	std::sort(acceptedAddInputs.begin(),acceptedAddInputs.end(),::aior_inf_bior());
	std::sort(rejectedAddInputs.begin(),rejectedAddInputs.end(),::aior_inf_bior());
	
	// pull request and sort result
	inputsPullRequests(acceptedPullInputs, rejectedPullInputs);
	std::sort(acceptedPullInputs.begin(),acceptedPullInputs.end(),::aior_inf_bior());
	std::sort(rejectedPullInputs.begin(),rejectedPullInputs.end(),::aior_inf_bior());
	
	// when there is conflict of index, give priority to the add requests
	// and remove pull requests
	auto itPull = rejectedPullInputs.begin();
	bool removed = false;
	while (itPull != rejectedPullInputs.end())
	{
		for (auto& acc: acceptedAddInputs)
		{
			if (itPull->m_index == acc.m_index)
			{
				itPull = rejectedPullInputs.erase(itPull);
				removed = true;
				break;
			}
		}
		if (!removed)
		{
			itPull++;
		}
	}
	
	//Build updated box by parsing accepted inputs from add request and rejected inputs from pull request
	auto itAdd = acceptedAddInputs.begin();
	itPull = rejectedPullInputs.begin();
	uint index = 0;
	
	CIdentifier sourceBoxIdentifier = m_sourceBox->getIdentifier();
		
	// add inputs
	while( (itAdd != acceptedAddInputs.end() || itPull != rejectedPullInputs.end()) 
	      && (index < (acceptedAddInputs.size() + rejectedPullInputs.size()))
	      )
	{
		if (itAdd != acceptedAddInputs.end() && itPull != rejectedPullInputs.end())
		{
			OV_ERROR_UNLESS_KRF(itAdd->m_index != itPull->m_index,
			                    "acceptedAddInputs and rejectedPullInputs contains element with the same index = " << index,
			                    ErrorType::BadInput
			                    );
			
			OV_ERROR_UNLESS_KRF((index == itAdd->m_index)||(index == itPull->m_index),
			                    "acceptedAddInputs nor rejectedPullInputs does not contains element with index = " << index,
			                    ErrorType::BadInput
			                    );
		}
		if (itAdd != acceptedAddInputs.end())
		{
			if (index == itAdd->m_index)
			{
				m_updatedBox->addInput(itAdd->m_name,itAdd->m_typeIdentifier,itAdd->m_identifier);
				// fill links to be updated set
				for (auto& l : itAdd->m_linksIdentifiers)
				{
					if (updatedLinks.find(l) == updatedLinks.end())
					{
						updatedLinks[l] = new CLink(m_scenario->getKernelContext(),*m_scenario);
					}
					updatedLinks[l]->setIdentifier(m_scenario->getLinkDetails(l)->getIdentifier());
					updatedLinks[l]->setSource(m_scenario->getLinkDetails(l)->getSourceBoxIdentifier(), 
					                             m_scenario->getLinkDetails(l)->getSourceBoxOutputIndex(), 
					                             m_scenario->getLinkDetails(l)->getSourceBoxOutputIdentifier());
					updatedLinks[l]->setTarget(sourceBoxIdentifier,itAdd->m_index,itAdd->m_identifier);					 
				}
				itAdd++;
			}
		}
		if (itPull != rejectedPullInputs.end())
		{
			if (index == itPull->m_index)
			{
				m_updatedBox->addInput(itPull->m_name,itPull->m_typeIdentifier,itPull->m_identifier);				
				// fill links to be updated set
				for (auto& l : itPull->m_linksIdentifiers)
				{
					if (updatedLinks.find(l) == updatedLinks.end())
					{
						updatedLinks[l] = new CLink(m_scenario->getKernelContext(),*m_scenario);
					}
					updatedLinks[l]->setIdentifier(m_scenario->getLinkDetails(l)->getIdentifier());
					updatedLinks[l]->setSource(m_scenario->getLinkDetails(l)->getSourceBoxIdentifier(), 
					                             m_scenario->getLinkDetails(l)->getSourceBoxOutputIndex(), 
					                             m_scenario->getLinkDetails(l)->getSourceBoxOutputIdentifier());
					updatedLinks[l]->setTarget(sourceBoxIdentifier,itPull->m_index,itPull->m_identifier);					 					
				}
				itPull++;
			}				
		}
		index++;
	}
	
	// At this step, rejected inputs from add request could have been rejected because they are user added inputs. 
	// We must add them if the updated box if CanAddInput is enabled
	if (m_updatedBox->hasAttribute(OV_AttributeId_Box_FlagCanAddInput))
	{
		for (auto it = rejectedAddInputs.begin(); it != rejectedAddInputs.end() ; )
		{			
			if (m_updatedBox->getInputCount() >= m_kernelBox->getInputCount())
			{
				// input was a user added input
				m_updatedBox->addInput(it->m_name,it->m_typeIdentifier,it->m_identifier);
				// fill links to be updated set
				for (auto& l : it->m_linksIdentifiers)
				{
					if (updatedLinks.find(l) == updatedLinks.end())
					{
						updatedLinks[l] = new CLink(m_scenario->getKernelContext(),*m_scenario);
					}
					updatedLinks[l]->setIdentifier(m_scenario->getLinkDetails(l)->getIdentifier());
					updatedLinks[l]->setSource(m_scenario->getLinkDetails(l)->getSourceBoxIdentifier(), 
					                           m_scenario->getLinkDetails(l)->getSourceBoxOutputIndex(), 
					                           m_scenario->getLinkDetails(l)->getSourceBoxOutputIdentifier());
					updatedLinks[l]->setTarget(sourceBoxIdentifier,it->m_index,it->m_identifier);					 				
				}
				it = rejectedAddInputs.erase(it);
			}
			else
			{
				it++;
			}
		}
	}
	else
	{
		// add rejected inputs and set them as deprecated
		for (auto& it : rejectedAddInputs)
		{
			m_updatedBox->addInput(it.m_name,it.m_typeIdentifier,it.m_identifier);
			m_updatedBox->setInputMissingStatus(m_updatedBox->getInputCountWithMissing()-1,true);
			// fill links to be updated set
			for (auto& l : it.m_linksIdentifiers)
			{
				if (updatedLinks.find(l) == updatedLinks.end())
				{
					updatedLinks[l] = new CLink(m_scenario->getKernelContext(),*m_scenario);
				}
				updatedLinks[l]->setIdentifier(m_scenario->getLinkDetails(l)->getIdentifier());
				updatedLinks[l]->setSource(m_scenario->getLinkDetails(l)->getSourceBoxIdentifier(), 
				                             m_scenario->getLinkDetails(l)->getSourceBoxOutputIndex(), 
				                             m_scenario->getLinkDetails(l)->getSourceBoxOutputIdentifier());
				updatedLinks[l]->setTarget(sourceBoxIdentifier,m_updatedBox->getInputCountWithMissing()-1,OV_UndefinedIdentifier);					 
				updatedLinks[l]->setAttributeValue(OV_AttributeId_Link_Invalid, "");
			}
		}
	}
			
	// Box has to be updated if reject add requests or updated inputs or rejected pull requests are pending
	inputsToBeUpdated = rejectedAddInputs.size() || rejectedPullInputs.size() || updatedInputs;
	
	return true;
}

bool CBoxUpdater::processOutputsRequests(bool &outputsToBeUpdated, std::map<CIdentifier,CLink*> &updatedLinks)                  
{
	outputsToBeUpdated = false;
	// process to outputs add requests
	std::vector<SIORequest> rejectedAddOutputs;
	std::vector<SIORequest> rejectedPullOutputs;
	std::vector<SIORequest> acceptedAddOutputs;
	std::vector<SIORequest> acceptedPullOutputs;
	
	// add request and sort result
	bool updatedOutputs = false;
	outputsAddRequests(acceptedAddOutputs, rejectedAddOutputs, updatedOutputs);
	std::sort(acceptedAddOutputs.begin(),acceptedAddOutputs.end(),::aior_inf_bior());
	std::sort(rejectedAddOutputs.begin(),rejectedAddOutputs.end(),::aior_inf_bior());
	
	// pull request and sort result
	outputsPullRequests(acceptedPullOutputs, rejectedPullOutputs);
	std::sort(acceptedPullOutputs.begin(),acceptedPullOutputs.end(),::aior_inf_bior());
	std::sort(rejectedPullOutputs.begin(),rejectedPullOutputs.end(),::aior_inf_bior());
	
	// when there is conflict of index, give priority to the add requests
	// and remove pull requests
	auto itPull = rejectedPullOutputs.begin();
	bool removed = false;
	while (itPull != rejectedPullOutputs.end())
	{
		for (auto& acc: acceptedAddOutputs)
		{
			if (itPull->m_index == acc.m_index)
			{
				itPull = rejectedPullOutputs.erase(itPull);
				removed = true;
				break;
			}
		}
		if (!removed)
		{
			itPull++;
		}
	}
	
	//Build updated box by parsing accepted outputs from add request and rejected outputs from pull request
	CIdentifier sourceBoxIdentifier = m_sourceBox->getIdentifier();
	auto itAdd = acceptedAddOutputs.begin();
	itPull = rejectedPullOutputs.begin();
	uint index = 0;
	// add outputs
	while( (itAdd != acceptedAddOutputs.end() || itPull != rejectedPullOutputs.end()) 
	      && (index < (acceptedAddOutputs.size() + rejectedPullOutputs.size()))
	      )
	{
		if (itAdd != acceptedAddOutputs.end() && itPull != rejectedPullOutputs.end())
		{			
			OV_ERROR_UNLESS_KRF(itAdd->m_index != itPull->m_index,
			                    "acceptedAddOutputs and rejectedPullOutputs contains element with the same index = " << index,
			                    ErrorType::BadOutput
			                    );
			
			OV_ERROR_UNLESS_KRF((index == itAdd->m_index)||(index == itPull->m_index),
			                    "acceptedAddOutputs nor rejectedPullOutputs does not contains element with index = " << index,
			                    ErrorType::BadOutput
			                    );
		}
		if (itAdd != acceptedAddOutputs.end())
		{
			if (index == itAdd->m_index)
			{
				m_updatedBox->addOutput(itAdd->m_name,itAdd->m_typeIdentifier,itAdd->m_identifier);
				// fill links to be updated set
				for (auto& l : itAdd->m_linksIdentifiers)
				{
					if (updatedLinks.find(l) == updatedLinks.end())
					{
						updatedLinks[l] = new CLink(m_scenario->getKernelContext(),*m_scenario);
					}
					updatedLinks[l]->setIdentifier(m_scenario->getLinkDetails(l)->getIdentifier());
					updatedLinks[l]->setTarget(m_scenario->getLinkDetails(l)->getTargetBoxIdentifier(), 
					                             m_scenario->getLinkDetails(l)->getTargetBoxInputIndex(), 
					                             m_scenario->getLinkDetails(l)->getTargetBoxInputIdentifier());
					updatedLinks[l]->setSource(sourceBoxIdentifier,itAdd->m_index,itAdd->m_identifier);					
				}
				itAdd++;
			}
		}
		if (itPull != rejectedPullOutputs.end())
		{
			if (index == itPull->m_index)
			{
				m_updatedBox->addOutput(itPull->m_name,itPull->m_typeIdentifier,itPull->m_identifier);
				// fill links to be updated set
				for (auto& l : itPull->m_linksIdentifiers)
				{
					if (updatedLinks.find(l) == updatedLinks.end())
					{
						updatedLinks[l] = new CLink(m_scenario->getKernelContext(),*m_scenario);
					}
					updatedLinks[l]->setIdentifier(m_scenario->getLinkDetails(l)->getIdentifier());
					updatedLinks[l]->setTarget(m_scenario->getLinkDetails(l)->getTargetBoxIdentifier(), 
					                             m_scenario->getLinkDetails(l)->getTargetBoxInputIndex(), 
					                             m_scenario->getLinkDetails(l)->getTargetBoxInputIdentifier());
					updatedLinks[l]->setSource(sourceBoxIdentifier,itPull->m_index,itPull->m_identifier);					
				}
				itPull++;
			}				
		}
		index++;
	}
	
	// At this step, rejected outputs from add request could have been rejected because they are user added outputs. 
	// We must add them if the updated box if CanAddOutput is enabled
	if (m_updatedBox->hasAttribute(OV_AttributeId_Box_FlagCanAddOutput))
	{
		for (auto it = rejectedAddOutputs.begin(); it != rejectedAddOutputs.end() ; )
		{			
			if (m_updatedBox->getOutputCount() >= m_kernelBox->getOutputCount())
			{
				m_updatedBox->addOutput(it->m_name,it->m_typeIdentifier,it->m_identifier);
				// fill links to be updated set
				for (auto& l : it->m_linksIdentifiers)
				{
					if (updatedLinks.find(l) == updatedLinks.end())
					{
						updatedLinks[l] = new CLink(m_scenario->getKernelContext(),*m_scenario);
					}
					updatedLinks[l]->setIdentifier(m_scenario->getLinkDetails(l)->getIdentifier());
					updatedLinks[l]->setTarget(m_scenario->getLinkDetails(l)->getTargetBoxIdentifier(), 
					                           m_scenario->getLinkDetails(l)->getTargetBoxInputIndex(), 
					                           m_scenario->getLinkDetails(l)->getTargetBoxInputIdentifier());
					updatedLinks[l]->setSource(sourceBoxIdentifier,it->m_index,it->m_identifier);				
				}
				it = rejectedAddOutputs.erase(it);
			}
			else
			{
				it++;
			}
		}
	}
	else
	{
		// add rejected inputs and set them as deprecated
		for (auto& it : rejectedAddOutputs)
		{
			m_updatedBox->addOutput(it.m_name,it.m_typeIdentifier,it.m_identifier);
			m_updatedBox->setOutputMissingStatus(m_updatedBox->getOutputCountWithMissing()-1,true);
			// fill links to be updated set
			for (auto& l : it.m_linksIdentifiers)
			{
				if (updatedLinks.find(l) == updatedLinks.end())
				{
					updatedLinks[l] = new CLink(m_scenario->getKernelContext(),*m_scenario);
				}
				updatedLinks[l]->setIdentifier(m_scenario->getLinkDetails(l)->getIdentifier());
				updatedLinks[l]->setTarget(m_scenario->getLinkDetails(l)->getTargetBoxIdentifier(), 
				                             m_scenario->getLinkDetails(l)->getTargetBoxInputIndex(), 
				                             m_scenario->getLinkDetails(l)->getTargetBoxInputIdentifier());
				updatedLinks[l]->setSource(sourceBoxIdentifier,m_updatedBox->getOutputCountWithMissing()-1,OV_UndefinedIdentifier);					 
				updatedLinks[l]->setAttributeValue(OV_AttributeId_Link_Invalid, "");
			}
		}
	}
	
	// Box has to be updated if reject add requests or updated outputs or rejected pull requests are pending
	outputsToBeUpdated = rejectedAddOutputs.size() || rejectedPullOutputs.size() || updatedOutputs;
	
	return true;
}

bool CBoxUpdater::processSettingsRequests(bool &settingsToBeUpdated)
{
	settingsToBeUpdated = false;
	// process to settings add requests
	std::vector<SSettingRequest> rejectedAddSettings;
	std::vector<SSettingRequest> rejectedPullSettings;
	std::vector<SSettingRequest> acceptedAddSettings;
	std::vector<SSettingRequest> acceptedPullSettings;
	
	// add request and sort result
	bool updatedSettings = false;
	settingsAddRequests(acceptedAddSettings, rejectedAddSettings, updatedSettings);
	std::sort(acceptedAddSettings.begin(),acceptedAddSettings.end(),::asr_inf_bsr());
	std::sort(rejectedAddSettings.begin(),rejectedAddSettings.end(),::asr_inf_bsr());
	
	// pull request and sort result
	settingsPullRequests(acceptedPullSettings, rejectedPullSettings);
	std::sort(acceptedPullSettings.begin(),acceptedPullSettings.end(),::asr_inf_bsr());
	std::sort(rejectedPullSettings.begin(),rejectedPullSettings.end(),::asr_inf_bsr());
	
	// when there is conflict of index, give priority to the add requests
	// and remove pull requests
	auto itPull = rejectedPullSettings.begin();
	bool removed = false;
	while (itPull != rejectedPullSettings.end())
	{
		for (auto& acc: acceptedAddSettings)
		{
			if (itPull->m_index == acc.m_index)
			{
				itPull = rejectedPullSettings.erase(itPull);
				removed = true;
				break;
			}
		}
		if (!removed)
		{
			itPull++;
		}
	}
	
	//Build updated box by parsing accepted settings from add request and rejected settings from pull request
	auto itAdd = acceptedAddSettings.begin();
	itPull = rejectedPullSettings.begin();
	uint index = 0;
	
	// add settings
	while( (itAdd != acceptedAddSettings.end() || itPull != rejectedPullSettings.end()) 
	      && (index < (acceptedAddSettings.size() + rejectedPullSettings.size()))
	      )
	{
		if (itAdd != acceptedAddSettings.end() && itPull != rejectedPullSettings.end())
		{
			OV_ERROR_UNLESS_KRF(itAdd->m_index != itPull->m_index,
			                    "acceptedAddSettings and rejectedPullSettings contains element with the same index = " << index,
			                    ErrorType::BadSetting
			                    );
			
			OV_ERROR_UNLESS_KRF((index == itAdd->m_index)||(index == itPull->m_index),
			                    "acceptedAddSettings nor rejectedPullSettings does not contains element with index = " << index,
			                    ErrorType::BadSetting
			                    );
		}
		if (itAdd != acceptedAddSettings.end())
		{
			if (index == itAdd->m_index)
			{
				m_updatedBox->addSetting(itAdd->m_name,
				                         itAdd->m_typeIdentifier,
				                         itAdd->m_defaultValue,
				                         OV_Value_UndefinedIndexUInt,
				                         itAdd->m_modifiability,
				                         itAdd->m_identifier);
				m_updatedBox->setSettingValue(itAdd->m_index, itAdd->m_value);
				itAdd++;
			}
		}
		if (itPull != rejectedPullSettings.end())
		{
			if (index == itPull->m_index)
			{
				m_updatedBox->addSetting(itPull->m_name,
				                         itPull->m_typeIdentifier,
				                         itPull->m_defaultValue,
				                         OV_Value_UndefinedIndexUInt,
				                         itPull->m_modifiability,
				                         itPull->m_identifier);
				m_updatedBox->setSettingValue(itPull->m_index, itPull->m_value);
				itPull++;
			}				
		}
		index++;
	}
	
	// At this step, rejected settings from add request could have been rejected because they are user added settings. 
	// We must add them if the updated box if CanAddSetting is enabled
	if (m_updatedBox->hasAttribute(OV_AttributeId_Box_FlagCanAddSetting))
	{
		for (auto it = rejectedAddSettings.begin(); it != rejectedAddSettings.end() ; )
		{						
			if (m_updatedBox->getSettingCount() >= m_kernelBox->getSettingCount())
			{
				m_updatedBox->addSetting(it->m_name,
				                         it->m_typeIdentifier,
				                         it->m_defaultValue,
				                         OV_Value_UndefinedIndexUInt,
				                         it->m_modifiability,
				                         it->m_identifier);
				m_updatedBox->setSettingValue(m_updatedBox->getSettingCount()-1, it->m_value);
				it = rejectedAddSettings.erase(it);
			}
			else
			{
				it++;
			}
		}
	}
	else
	{
		// add rejected inputs and set them as deprecated
		for (auto& it : rejectedAddSettings)
		{
			m_updatedBox->addSetting(it.m_name,
			                         it.m_typeIdentifier,
			                         it.m_defaultValue,
			                         OV_Value_UndefinedIndexUInt,
			                         it.m_modifiability,
			                         it.m_identifier);
			m_updatedBox->setSettingMissingStatus(m_updatedBox->getSettingCountWithMissing()-1,true);			
		}
	}
	
	// Box has to be updated if reject add requests or rejected pull requests are pending
	settingsToBeUpdated = rejectedAddSettings.size() || rejectedPullSettings.size() || updatedSettings;
	
	return true;
}

//___________________________________________________________________//
//                                                                   //
bool CBoxUpdater::inputsAddRequests(std::vector<SIORequest> &acceptedInputs, std::vector<SIORequest> &rejectedInputs, bool &updatedInputs)
{
	acceptedInputs.clear();
	rejectedInputs.clear();
	
	for(uint i = 0; i < m_sourceBox->getInputCount(); ++i)
	{		
		SIORequest request;
		request.m_index = i;
		m_sourceBox->getInputIdentifier(i, request.m_identifier);
		m_sourceBox->getInputType(i, request.m_typeIdentifier);
		m_sourceBox->getInputName(i, request.m_name);
		
		CIdentifier oldType = request.m_typeIdentifier;
		CIdentifier oldIdentifier = request.m_identifier;
		
		CIdentifier* identifierList = nullptr;
		size_t nbElems = 0;		
		m_scenario->getLinkIdentifierToBoxInputList(m_sourceBox->getIdentifier(), i, &identifierList, &nbElems);
		for (size_t j = 0; j < nbElems; ++j)
		{
			request.m_linksIdentifiers.push_back(identifierList[j]);
		}
		
		if (!protoHasInput(*m_kernelBox, request.m_index, request.m_name, request.m_typeIdentifier, request.m_identifier, m_kernelBox->getInputCount()))
		{						
			// kernel prototype does not have input, set as deprecated
			rejectedInputs.push_back(request);
		}
		else
		{
			acceptedInputs.push_back(request);
			if ((oldIdentifier != request.m_identifier) && oldIdentifier == OV_UndefinedIdentifier)
			{
				m_sourceBox->updateInputIdentifier(request.m_index, request.m_identifier);
			}
			updatedInputs |=((i != request.m_index) 
			                   || (oldType != request.m_typeIdentifier)
			                   || ((oldIdentifier != request.m_identifier) && oldIdentifier == OV_UndefinedIdentifier)
			                   );
		}
	}
	return true;
}

bool CBoxUpdater::outputsAddRequests(std::vector<SIORequest> &acceptedOutputs, std::vector<SIORequest> &rejectedOutputs, bool &updatedOutputs)
{
	acceptedOutputs.clear();
	rejectedOutputs.clear();
	
	for(uint i = 0; i < m_sourceBox->getOutputCount(); ++i)
	{		
		SIORequest request;
		request.m_index = i;
		m_sourceBox->getOutputIdentifier(i, request.m_identifier);
		m_sourceBox->getOutputType(i, request.m_typeIdentifier);
		m_sourceBox->getOutputName(i, request.m_name);
		
		CIdentifier oldType = request.m_typeIdentifier;
		CIdentifier oldIdentifier = request.m_identifier;
		
		CIdentifier* identifierList = nullptr;
		size_t nbElems = 0;		
		m_scenario->getLinkIdentifierFromBoxOutputList(m_sourceBox->getIdentifier(), i, &identifierList, &nbElems);
		for (size_t j = 0; j < nbElems; ++j)
		{
			request.m_linksIdentifiers.push_back(identifierList[j]);
		}
		
		if (!protoHasOutput(*m_kernelBox, request.m_index, request.m_name, request.m_typeIdentifier, request.m_identifier,m_kernelBox->getOutputCount()))
		{						
			// kernel prototype does not have Output, set as deprecated
			rejectedOutputs.push_back(request);
		}
		else
		{
			acceptedOutputs.push_back(request);
			if ((oldIdentifier != request.m_identifier) && oldIdentifier == OV_UndefinedIdentifier)
			{
				m_sourceBox->updateOutputIdentifier(request.m_index, request.m_identifier);
			}
			updatedOutputs |=((i != request.m_index) 
			                   || (oldType != request.m_typeIdentifier)
			                   || ((oldIdentifier != request.m_identifier) && oldIdentifier == OV_UndefinedIdentifier)
			                   );
		}
	}
	return true;
}

bool CBoxUpdater::settingsAddRequests(std::vector<SSettingRequest> &acceptedSettings, std::vector<SSettingRequest> &rejectedSettings, bool &updatedSettings)
{
	acceptedSettings.clear();
	rejectedSettings.clear();
	
	for(uint i = 0; i < m_sourceBox->getSettingCount(); ++i)
	{		
		SSettingRequest request;
		request.m_index = i;
		m_sourceBox->getSettingIdentifier(i, request.m_identifier);
		m_sourceBox->getSettingType(i, request.m_typeIdentifier);
		m_sourceBox->getSettingName(i, request.m_name);
		m_sourceBox->getSettingDefaultValue(i, request.m_defaultValue);
		m_sourceBox->getSettingMod(i, request.m_modifiability);
		m_sourceBox->getSettingValue(i, request.m_value);
		
		CIdentifier oldType = request.m_typeIdentifier;
		CIdentifier oldIdentifier = request.m_identifier;
		
		if (!protoHasSetting(*m_kernelBox, request.m_index, request.m_name, request.m_typeIdentifier, request.m_identifier, m_kernelBox->getInputCount()))
		{						
			// kernel prototype does not have Setting, set as deprecated
			rejectedSettings.push_back(request);
		}
		else
		{
			acceptedSettings.push_back(request);
			if ((oldIdentifier != request.m_identifier) && oldIdentifier == OV_UndefinedIdentifier)
			{
				m_sourceBox->updateSettingIdentifier(request.m_index, request.m_identifier);
			}
			updatedSettings |=((i != request.m_index) 
			                   || (oldType != request.m_typeIdentifier)
			                   || ((oldIdentifier != request.m_identifier) && oldIdentifier == OV_UndefinedIdentifier)
			                   );		
			
		}
	}
	return true;
}

bool CBoxUpdater::inputsPullRequests(std::vector<SIORequest> &acceptedInputs, std::vector<SIORequest> &rejectedInputs)
{
	acceptedInputs.clear();
	rejectedInputs.clear();
	
	for(uint i = 0; i < m_kernelBox->getInputCount(); ++i)
	{		
		SIORequest request;
		request.m_index = i;
		m_kernelBox->getInputIdentifier(i, request.m_identifier);
		m_kernelBox->getInputType(i, request.m_typeIdentifier);
		m_kernelBox->getInputName(i, request.m_name);
				
		/*unsigned int  initialInputsCount = static_cast<unsigned int>((m_sourceBox->hasAttribute(OV_AttributeId_Box_InitialInputCount))?
		                                                                 atoi(m_sourceBox->getAttributeValue(OV_AttributeId_Box_InitialInputCount).toASCIIString()):m_sourceBox->getInputCount());
		*/
		
		if (!protoHasInput(*m_sourceBox, request.m_index, request.m_name, request.m_typeIdentifier, request.m_identifier, m_sourceBox->getInputCount()))
		{						
			// kernel prototype does not have input, set as deprecated
			rejectedInputs.push_back(request);
		}
		else
		{
			acceptedInputs.push_back(request);
		}
	}
	return true;
}

bool CBoxUpdater::outputsPullRequests(std::vector<SIORequest> &acceptedOutputs, std::vector<SIORequest> &rejectedOuputs)
{
	acceptedOutputs.clear();
	rejectedOuputs.clear();
	
	for(uint i = 0; i < m_kernelBox->getOutputCount(); ++i)
	{		
		SIORequest request;
		request.m_index = i;
		m_kernelBox->getOutputIdentifier(i, request.m_identifier);
		m_kernelBox->getOutputType(i, request.m_typeIdentifier);
		m_kernelBox->getOutputName(i, request.m_name);
				
		/*unsigned int  initialOutputsCount = static_cast<unsigned int>((m_sourceBox->hasAttribute(OV_AttributeId_Box_InitialOutputCount))?
		                                                                 atoi(m_sourceBox->getAttributeValue(OV_AttributeId_Box_InitialOutputCount).toASCIIString()):m_sourceBox->getOutputCount());
		*/
		if (!protoHasOutput(*m_sourceBox, request.m_index, request.m_name, request.m_typeIdentifier, request.m_identifier,m_sourceBox->getOutputCount()))
		{						
			// kernel prototype does not have Output, set as deprecated
			rejectedOuputs.push_back(request);
		}
		else
		{
			acceptedOutputs.push_back(request);
		}
	}
	return true;
}

bool CBoxUpdater::settingsPullRequests(std::vector<SSettingRequest> &acceptedSettings, std::vector<SSettingRequest> &rejectedSettings)
{
	acceptedSettings.clear();
	rejectedSettings.clear();
	
	for(uint i = 0; i < m_kernelBox->getSettingCount(); ++i)
	{		
		SSettingRequest request;
		request.m_index = i;
		m_kernelBox->getSettingIdentifier(i, request.m_identifier);
		m_kernelBox->getSettingType(i, request.m_typeIdentifier);
		m_kernelBox->getSettingName(i, request.m_name);
		m_kernelBox->getSettingDefaultValue(i, request.m_defaultValue);
		m_kernelBox->getSettingMod(i, request.m_modifiability);
		m_kernelBox->getSettingValue(i, request.m_value);
				
		/*unsigned int  initialSettingsCount = static_cast<unsigned int>((m_sourceBox->hasAttribute(OV_AttributeId_Box_InitialSettingCount))?
		                                                                 atoi(m_sourceBox->getAttributeValue(OV_AttributeId_Box_InitialSettingCount).toASCIIString()):m_sourceBox->getSettingCount());
		
		*/
		if (!protoHasSetting(*m_sourceBox, request.m_index, request.m_name, request.m_typeIdentifier, request.m_identifier, m_sourceBox->getSettingCount()))
		{						
			// kernel prototype does not have Setting, set as deprecated
			rejectedSettings.push_back(request);
		}
		else
		{
			acceptedSettings.push_back(request);
		}
	}
	return true;
}

//___________________________________________________________________//
//  

bool CBoxUpdater::protoHasInput(const IBox &box, uint &index, CString &name , CIdentifier &typeIdentifier, CIdentifier &suggestedIdentifier, const unsigned int initialSourceInputsCount)
{
	bool test = (suggestedIdentifier != OV_UndefinedIdentifier)?
	            box.hasInputWithIdentifier(suggestedIdentifier):box.hasInputWithType(index,typeIdentifier);
	
	if (test)
	{
		// input was found
		// update other data in case it has changed
		if (suggestedIdentifier != OV_UndefinedIdentifier)
		{			
			if (!box.hasAttribute(OV_AttributeId_Box_FlagCanModifyInput))
			{
				box.getInputName(suggestedIdentifier, name);
				box.getInputType(suggestedIdentifier, typeIdentifier);  
			}
			box.getInputIndex(suggestedIdentifier, index); 
		}
		else
		{
			if (!box.hasAttribute(OV_AttributeId_Box_FlagCanModifyInput))
			{
				box.getInputName(index, name);
				box.getInputType(index, typeIdentifier);
			}
			box.getInputIdentifier(index, suggestedIdentifier); 
		}            
		return true;
	}			                        
	else
	{
		if (box.hasAttribute(OV_AttributeId_Box_FlagCanModifyInput) && suggestedIdentifier == OV_UndefinedIdentifier)
		{
			
			if ((index < initialSourceInputsCount) && box.hasInputSupport(typeIdentifier))
			{
				// input was belonging to kernel proto but user could have modified its type
				//box.getInputName(index, name);
				box.getInputIdentifier(index, suggestedIdentifier);
				return true;
			}				
		}		
		return false;
	}
}

bool CBoxUpdater::protoHasOutput(const IBox &box, uint &index, CString &name , CIdentifier &typeIdentifier, CIdentifier &suggestedIdentifier, const unsigned int initialSourceOutputsCount)
{
	bool test = (suggestedIdentifier != OV_UndefinedIdentifier)?
	            box.hasOutputWithIdentifier(suggestedIdentifier):box.hasOutputWithType(index,typeIdentifier);
	
	if (test)
	{
		// Output was found
		// update other data in case it has changed
		if (suggestedIdentifier != OV_UndefinedIdentifier)
		{
			if (!box.hasAttribute(OV_AttributeId_Box_FlagCanModifyOutput))
			{
				box.getOutputName(suggestedIdentifier, name);
				box.getOutputType(suggestedIdentifier, typeIdentifier);  
			}
			box.getOutputIndex(suggestedIdentifier, index); 
		}
		else
		{
			if (!box.hasAttribute(OV_AttributeId_Box_FlagCanModifyOutput))
			{
				box.getOutputName(index, name);
				box.getOutputType(index, typeIdentifier);
			}
			box.getOutputIdentifier(index, suggestedIdentifier); 
		}            
		return true;
	}			                        
	else
	{
		if (box.hasAttribute(OV_AttributeId_Box_FlagCanModifyOutput) && suggestedIdentifier == OV_UndefinedIdentifier)
		{
			if ((index < initialSourceOutputsCount) && box.hasOutputSupport(typeIdentifier))
			{
				// output was belonging to kernel proto but user could have modified its type
				//box.getOutputName(index, name);
				box.getOutputIdentifier(index, suggestedIdentifier);
				return true;
			}				
		}		
		return false;
	}
}

bool CBoxUpdater::protoHasSetting(const IBox &box, uint &index, CString &name , CIdentifier &typeIdentifier, CIdentifier &suggestedIdentifier, const unsigned int initialSourceSettingsCount)
{
	bool test = (suggestedIdentifier != OV_UndefinedIdentifier)?
	            box.hasSettingWithIdentifier(suggestedIdentifier):box.hasSettingWithType(index,typeIdentifier);
	
	if (test)
	{
		// Setting was found
		// update other data in case it has changed
		if (suggestedIdentifier != OV_UndefinedIdentifier)
		{
			box.getSettingName(suggestedIdentifier, name);
			box.getSettingType(suggestedIdentifier, typeIdentifier);  
			box.getSettingIndex(suggestedIdentifier, index); 
		}
		else
		{
			box.getSettingName(index, name);
			box.getSettingType(index, typeIdentifier);
			box.getSettingIdentifier(index, suggestedIdentifier); 
		}            
		return true;
	}			                        
	else
	{
		if (box.hasAttribute(OV_AttributeId_Box_FlagCanModifySetting) && suggestedIdentifier == OV_UndefinedIdentifier)
		{
			if (index < initialSourceSettingsCount)
			{
				// setting was belonging to kernel proto but user could have modified its type
				//box.getSettingName(index, name);
				box.getSettingIdentifier(index, suggestedIdentifier);
				return true;
			}				
		}		
		return false;
	}
}

