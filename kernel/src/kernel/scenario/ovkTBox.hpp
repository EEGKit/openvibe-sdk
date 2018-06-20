#ifndef __OpenViBEKernel_Kernel_Scenario_CBox_H__
#define __OpenViBEKernel_Kernel_Scenario_CBox_H__

#include "../ovkTKernelObject.h"

#include "ovkTAttributable.h"
#include "ovkCScenario.h"
#include "ovkCBoxListenerContext.h"
#include "ovkCBoxProto.h"
#include <openvibe/plugins/ovIPluginObjectDesc.h>
#include "../ovkCObjectVisitorContext.h"

#include <openvibe/ov_defines.h>
#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include <system/ovCMath.h>
#include <system/ovCMemory.h>

namespace{
	//This class is used to set up the restriction of a stream type for input and output. Each box comes with a
	// decriptor that call functions describe in IBoxProto for intialize the CBox object.
	// This implementation is derived from CBoxProto, to benefit from
	// the implementation of the stream restriction mecanism but neutralizes all other initialization function.
	class CBoxProtoRestriction : public OpenViBE::Kernel::CBoxProto
	{
	public:

		CBoxProtoRestriction(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::IBox& rBox):
			CBoxProto(rKernelContext, rBox){}

		virtual bool addInput(
			const OpenViBE::CString& rsName,
			const OpenViBE::CIdentifier& rTypeIdentifier,
			const OpenViBE::CIdentifier& oIdentifier = OV_UndefinedIdentifier,
				const OpenViBE::boolean bNotify = true){return true;}

		virtual bool addOutput(
			const OpenViBE::CString& rsName,
			const OpenViBE::CIdentifier& rTypeIdentifier,
			const OpenViBE::CIdentifier& rIdentifier = OV_UndefinedIdentifier,
				const OpenViBE::boolean bNotify = true){return true;}

		virtual bool addSetting(
			const OpenViBE::CString& rsName,
			const OpenViBE::CIdentifier& rTypeIdentifier,
			const OpenViBE::CString& sDefaultValue,
			const bool bModifiable = false,
			const OpenViBE::CIdentifier& rIdentifier = OV_UndefinedIdentifier,
				const OpenViBE::boolean bNotify=true){
			return true;
		}

		virtual bool addFlag(const OpenViBE::Kernel::EBoxFlag eBoxFlag){ return true; }
		virtual bool addFlag(const OpenViBE::CIdentifier& cIdentifierFlag){ return true; }
	};
}


namespace OpenViBE
{
	namespace Kernel
	{
		template<class T>
		class TBox : public TAttributable < TKernelObject < T > >
		{
		public:

			explicit TBox(const OpenViBE::Kernel::IKernelContext& rKernelContext)
			    :TAttributable< TKernelObject <T> >(rKernelContext)
			    ,m_pOwnerScenario(NULL)
			    ,m_pBoxAlgorithmDescriptor(NULL)
			    ,m_pBoxListener(NULL)
			    ,m_bIsNotifyingDescriptor(false)
			    ,m_bIsNotificationActive(true)
				,m_bIsObserverNotificationActive(true)
				,m_oIdentifier(OV_UndefinedIdentifier)
			    ,m_oAlgorithmClassIdentifier(OV_UndefinedIdentifier)
			    ,m_sName("unnamed")
			    
			{
			}

			virtual ~TBox(void)
			{
				if(m_pBoxAlgorithmDescriptor && m_pBoxListener)
				{
					CBoxListenerContext l_oContext(this->getKernelContext(), *this, 0xffffffff);
					m_pBoxListener->uninitialize(l_oContext);
					m_pBoxAlgorithmDescriptor->releaseBoxListener(m_pBoxListener);
				}
			}

			virtual void setOwnerScenario(OpenViBE::Kernel::IScenario* pOwnerScenario)
			{
				m_pOwnerScenario = pOwnerScenario;
			}

			virtual OpenViBE::CIdentifier getIdentifier(void) const
			{
				return m_oIdentifier;
			}

			virtual OpenViBE::CString getName(void) const
			{
				return m_sName;
			}

			virtual OpenViBE::CIdentifier getAlgorithmClassIdentifier(void) const
			{
				return m_oAlgorithmClassIdentifier;
			}

			virtual bool setIdentifier(const OpenViBE::CIdentifier& rIdentifier)
			{
				OV_ERROR_UNLESS_KRF(
					m_oIdentifier == OV_UndefinedIdentifier,
					"Trying to overwrite an already set indentifier",
					ErrorType::BadCall
				);

				OV_ERROR_UNLESS_KRF(
					rIdentifier != OV_UndefinedIdentifier,
					"Trying to set an undefined identifier",
					ErrorType::BadArgument
				);

				m_oIdentifier=rIdentifier;

				this->notify(BoxModification_IdentifierChanged);

				return true;
			}

			virtual bool setName(const OpenViBE::CString& rsName)
			{
				m_sName=rsName;

				this->notify(BoxModification_NameChanged);

				return true;
			}

			virtual bool setAlgorithmClassIdentifier(const OpenViBE::CIdentifier& rAlgorithmClassIdentifier)
			{
				// We need to set the box algorithm identifier in any case. This is because OpenViBE should be able to load
				// a scenario with non-existing boxes and save it without modifying them.
				m_oAlgorithmClassIdentifier=rAlgorithmClassIdentifier;

				if (!(rAlgorithmClassIdentifier == OVP_ClassId_BoxAlgorithm_Metabox || this->getKernelContext().getPluginManager().canCreatePluginObject(rAlgorithmClassIdentifier)))
				{
					OV_WARNING_K("Box algorithm descriptor not found " << rAlgorithmClassIdentifier.toString());
					return true;
				}

				if(m_pBoxAlgorithmDescriptor && m_pBoxListener)
				{
					CBoxListenerContext l_oContext(this->getKernelContext(), *this, 0xffffffff);
					m_pBoxListener->uninitialize(l_oContext);
					m_pBoxAlgorithmDescriptor->releaseBoxListener(m_pBoxListener);
				}

				const Plugins::IPluginObjectDesc* l_pPluginObjectDescriptor=this->getKernelContext().getPluginManager().getPluginObjectDescCreating(rAlgorithmClassIdentifier);
				m_pBoxAlgorithmDescriptor=dynamic_cast<const Plugins::IBoxAlgorithmDesc*>(l_pPluginObjectDescriptor);

				if(m_pBoxAlgorithmDescriptor)
				{
					m_pBoxListener=m_pBoxAlgorithmDescriptor->createBoxListener();
					if(m_pBoxListener)
					{
						CBoxListenerContext l_oContext(this->getKernelContext(), *this, 0xffffffff);
						m_pBoxListener->initialize(l_oContext);
					}
				}

				if (rAlgorithmClassIdentifier != OVP_ClassId_BoxAlgorithm_Metabox)
				{
					//We use the neutralized version of CBoxProto to just initialize the stream restriction mecanism
					CBoxProtoRestriction oTempProto(this->getKernelContext(), *this);
					m_pBoxAlgorithmDescriptor->getBoxPrototype(oTempProto);
				}

				this->notify(BoxModification_AlgorithmClassIdentifierChanged);

				return true;
			}

			virtual bool initializeFromAlgorithmClassIdentifier(const OpenViBE::CIdentifier& rAlgorithmClassIdentifier)
			{
				this->disableNotification();

				const Plugins::IBoxAlgorithmDesc* l_pBoxAlgorithmDesc=dynamic_cast<const Plugins::IBoxAlgorithmDesc*>(this->getKernelContext().getPluginManager().getPluginObjectDescCreating(rAlgorithmClassIdentifier));
				if(!l_pBoxAlgorithmDesc)
				{
					this->enableNotification();
					OV_ERROR_KRF("Algorithm descriptor not found " << rAlgorithmClassIdentifier.toString(), ErrorType::ResourceNotFound);
				}

				this->clearBox();
				this->setName(l_pBoxAlgorithmDesc->getName());
				this->setAlgorithmClassIdentifier(rAlgorithmClassIdentifier);

				CBoxProto l_oBoxProto(this->getKernelContext(), *this);
				l_pBoxAlgorithmDesc->getBoxPrototype(l_oBoxProto);

				if(this->hasAttribute(OV_AttributeId_Box_InitialPrototypeHashValue))
				{
					this->setAttributeValue(OV_AttributeId_Box_InitialPrototypeHashValue, this->getPluginManager().getPluginObjectHashValue(rAlgorithmClassIdentifier).toString());
				}
				else
				{
					this->addAttribute(OV_AttributeId_Box_InitialPrototypeHashValue, this->getPluginManager().getPluginObjectHashValue(rAlgorithmClassIdentifier).toString());
				}

				this->enableNotification();

				this->notify(BoxModification_Initialized);
				this->notify(BoxModification_DefaultInitialized);
				return true;
			}
			
			virtual bool initializeFromAlgorithmClassIdentifierNoInit(const OpenViBE::CIdentifier& rAlgorithmClassIdentifier)
			{
				this->disableNotification();

				const Plugins::IBoxAlgorithmDesc* l_pBoxAlgorithmDesc=dynamic_cast<const Plugins::IBoxAlgorithmDesc*>(this->getKernelContext().getPluginManager().getPluginObjectDescCreating(rAlgorithmClassIdentifier));
				if(!l_pBoxAlgorithmDesc)
				{
					this->enableNotification();
					OV_ERROR_KRF("Algorithm descriptor not found " << rAlgorithmClassIdentifier.toString(), ErrorType::ResourceNotFound);
				}

				this->clearBox();
				this->setName(l_pBoxAlgorithmDesc->getName());
				this->setAlgorithmClassIdentifier(rAlgorithmClassIdentifier);

				CBoxProto l_oBoxProto(this->getKernelContext(), *this);
				l_pBoxAlgorithmDesc->getBoxPrototype(l_oBoxProto);

				if(this->hasAttribute(OV_AttributeId_Box_InitialPrototypeHashValue))
				{
					this->setAttributeValue(OV_AttributeId_Box_InitialPrototypeHashValue, this->getPluginManager().getPluginObjectHashValue(rAlgorithmClassIdentifier).toString());
				}
				else
				{
					this->addAttribute(OV_AttributeId_Box_InitialPrototypeHashValue, this->getPluginManager().getPluginObjectHashValue(rAlgorithmClassIdentifier).toString());
				}

				this->enableNotification();

				return true;
			}

			bool initializeFromBoxAlgorithmDesc(const OpenViBE::Plugins::IBoxAlgorithmDesc& rBoxAlgorithmDesc)
			{
				this->clearBox();
				this->setName(rBoxAlgorithmDesc.getName());
				this->setAlgorithmClassIdentifier(rBoxAlgorithmDesc.getCreatedClassIdentifier());

				CBoxProto l_oBoxProto(this->getKernelContext(), *this);
				rBoxAlgorithmDesc.getBoxPrototype(l_oBoxProto);

				if(this->hasAttribute(OV_AttributeId_Box_InitialPrototypeHashValue))
				{
					this->setAttributeValue(OV_AttributeId_Box_InitialPrototypeHashValue, this->getPluginManager().getPluginObjectHashValue(rBoxAlgorithmDesc).toString());
				}
				else
				{
					this->addAttribute(OV_AttributeId_Box_InitialPrototypeHashValue, this->getPluginManager().getPluginObjectHashValue(rBoxAlgorithmDesc).toString());
				}

				this->enableNotification();

				this->notify(BoxModification_Initialized);
				this->notify(BoxModification_DefaultInitialized);
				return true;
			}

			virtual bool initializeFromExistingBox(const OpenViBE::Kernel::IBox& rExistingBox)
			{
				this->disableNotification();
				m_bIsObserverNotificationActive = false;

				this->clearBox();
				this->setName(rExistingBox.getName());
				this->setAlgorithmClassIdentifier(rExistingBox.getAlgorithmClassIdentifier());

				for(uint32_t i = 0; i < rExistingBox.getInputCountWithMissing(); ++i)
				{
					CIdentifier l_oIdentifier;
					CIdentifier l_oType;
					CString l_sName;
					bool l_bMissing;
					rExistingBox.getInputIdentifier(i, l_oIdentifier);
					rExistingBox.getInputType(i, l_oType);
					rExistingBox.getInputName(i, l_sName);
					rExistingBox.getInputMissingStatus(i, l_bMissing);
					addInput(l_sName, l_oType, l_oIdentifier, true);
					setInputMissingStatus(i, l_bMissing);
				}

				for (uint32_t i = 0; i < rExistingBox.getOutputCountWithMissing(); ++i)
				{
					CIdentifier l_oIdentifier;
					CIdentifier l_oType;
					CString l_sName;
					bool l_bMissing;
					rExistingBox.getOutputIdentifier(i, l_oIdentifier);
					rExistingBox.getOutputType(i, l_oType);
					rExistingBox.getOutputName(i, l_sName);
					rExistingBox.getOutputMissingStatus(i, l_bMissing);							
					addOutput(l_sName, l_oType, l_oIdentifier, true);
					setOutputMissingStatus(i, l_bMissing);
				}

				for (uint32_t i = 0; i < rExistingBox.getSettingCountWithMissing(); ++i)
				{
					CIdentifier l_oType;
					CString l_sName;
					CString l_sValue;
					CString l_sDefaultValue;
					boolean l_bModifiability;
					CIdentifier l_oIdentifier;
					bool l_bMissing;

					rExistingBox.getSettingType(i, l_oType);
					rExistingBox.getSettingName(i, l_sName);
					rExistingBox.getSettingValue(i, l_sValue);
					rExistingBox.getSettingDefaultValue(i, l_sDefaultValue);
					rExistingBox.getSettingMod(i, l_bModifiability);
					rExistingBox.getSettingIdentifier(i, l_oIdentifier);
					rExistingBox.getSettingMissingStatus(i, l_bMissing);
					addSetting(l_sName, l_oType, l_sDefaultValue, OV_Value_UndefinedIndexUInt ,l_bModifiability,l_oIdentifier, true);
					setSettingValue(i, l_sValue);
					setSettingMissingStatus(i, l_bMissing);
				}

				CIdentifier l_oIdentifier=rExistingBox.getNextAttributeIdentifier(OV_UndefinedIdentifier);
				while(l_oIdentifier!=OV_UndefinedIdentifier)
				{
					this->addAttribute(l_oIdentifier, rExistingBox.getAttributeValue(l_oIdentifier));
					l_oIdentifier=rExistingBox.getNextAttributeIdentifier(l_oIdentifier);
				}

				CIdentifier l_oStreamTypeIdentifier = OV_UndefinedIdentifier;
				while((l_oStreamTypeIdentifier=this->getKernelContext().getTypeManager().getNextTypeIdentifier(l_oStreamTypeIdentifier))!=OV_UndefinedIdentifier)
				{
					if(this->getKernelContext().getTypeManager().isStream(l_oStreamTypeIdentifier))
					{
						//First check if it is a stream
						if(rExistingBox.hasInputSupport(l_oStreamTypeIdentifier))
						{
							this->addInputSupport(l_oStreamTypeIdentifier);
						}
						if(rExistingBox.hasOutputSupport(l_oStreamTypeIdentifier))
						{
							this->addOutputSupport(l_oStreamTypeIdentifier);
						}
					}
				}

				this->enableNotification();

				this->notify(BoxModification_Initialized);

				return true;
			}

//___________________________________________________________________//
//                                                                   //

			virtual bool addInput(const OpenViBE::CString& rsName, const OpenViBE::CIdentifier& rTypeIdentifier, const OpenViBE::CIdentifier& rIdentifier, const bool bNotify)
			{
				OV_ERROR_UNLESS_KRF(
					this->getTypeManager().isStream(rTypeIdentifier),
					"While adding input '" << rsName << "' to box '" << getName() << "', unknown stream type identifier " << rTypeIdentifier.toString(),
					ErrorType::BadArgument
				);
				uint32_t position = m_vInput.size();
				m_vInput.push_back(CInput(rsName, rTypeIdentifier, rIdentifier));
				
				
				if (rIdentifier != OV_UndefinedIdentifier)
				{
					// add access by CIdentifier key if defined so that size differs from m_vInput
					m_vInputIdentifierToIndex[rIdentifier] = position;	
				}
				
				// add access by name key (always done so that synchronized with m_vInput 
				OpenViBE::CString newName = this->getUnusedName(m_vInputNameToIndex,rsName);
				m_vInput[position].m_sName = newName;
				m_vInputNameToIndex[newName] = position;
				
				OV_ERROR_UNLESS_KRF(
				            m_vInputNameToIndex.size() == m_vInput.size(),
				            "Box " << m_sName << " has corrupted name map storage",
				            ErrorType::BadResourceCreation
				            );

				if (bNotify)
				{
					this->notify(BoxModification_InputAdded, this->getInputCount() - 1);
				}

				return true;
			}

			virtual bool removeInput(const uint32_t ui32InputIndex, const bool bNotify=true)
			{
				OV_ERROR_UNLESS_KRF(
					ui32InputIndex < m_vInput.size(),
					"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vInput.size() - 1) << "])",
					ErrorType::OutOfBound
					);


				{
					CIdentifier* identifierList = nullptr;
					size_t nbElems = 0;
					m_pOwnerScenario->getLinkIdentifierToBoxInputList(m_oIdentifier, ui32InputIndex, &identifierList, &nbElems);
					for (size_t i = 0; i < nbElems; ++i)
					{
						m_pOwnerScenario->disconnect(identifierList[i]);
					}
					m_pOwnerScenario->releaseIdentifierList(identifierList);
				}

				// $$$
				// The way the links are removed here
				// is not correct because they are all
				// collected and then all removed. In case
				// the box listener callback on box removal,
				// the nextcoming links would potentially be
				// invalid
				std::vector < std::pair < std::pair < uint64_t, uint32_t >, std::pair < uint64_t, uint32_t > > > l_vLink;

				{
					CIdentifier* identifierList = nullptr;
					size_t nbElems = 0;
					m_pOwnerScenario->getLinkIdentifierToBoxList(m_oIdentifier, &identifierList, &nbElems);
					for (size_t i = 0; i < nbElems; ++i)
					{
						CIdentifier l_oIdentifier = identifierList[i];
						ILink* l_pLink = m_pOwnerScenario->getLinkDetails(l_oIdentifier);
						if (l_pLink->getTargetBoxInputIndex() > ui32InputIndex)
						{
							l_vLink.push_back({
								{
									l_pLink->getSourceBoxIdentifier().toUInteger(),
									l_pLink->getSourceBoxOutputIndex()
								},
								{
									l_pLink->getTargetBoxIdentifier().toUInteger(),
									l_pLink->getTargetBoxInputIndex()
								}
							});

							if (m_pOwnerScenario->isLink(l_oIdentifier))
							{
								m_pOwnerScenario->disconnect(l_oIdentifier);
							}
						}
					}
					m_pOwnerScenario->releaseIdentifierList(identifierList);
				}

				// This reorganizes the parent's scenario links if this box is not actually a scenario itself
				if (m_oIdentifier != OV_UndefinedIdentifier)
				{
					std::vector < std::pair < uint32_t, std::pair < uint64_t, uint32_t > > > l_vScenarioLink;
					for (uint32_t scenarioInputIndex = 0; scenarioInputIndex < m_pOwnerScenario->getInputCount(); scenarioInputIndex++)
					{
						CIdentifier l_oBoxIdentifier;
						uint32_t l_ui32BoxConnectorIndex = uint32_t(-1);
						m_pOwnerScenario->getScenarioInputLink(scenarioInputIndex, l_oBoxIdentifier, l_ui32BoxConnectorIndex);
						if (l_oBoxIdentifier == m_oIdentifier)
						{
							if (l_ui32BoxConnectorIndex > ui32InputIndex)
							{
								l_vScenarioLink.push_back({
									scenarioInputIndex,
									{
										l_oBoxIdentifier.toUInteger(),
										l_ui32BoxConnectorIndex
									}
								});
							}
							if (l_ui32BoxConnectorIndex >= ui32InputIndex)
							{
								m_pOwnerScenario->removeScenarioInputLink(scenarioInputIndex, l_oBoxIdentifier, l_ui32BoxConnectorIndex);
							}
						}
					}

					// Reconnects scenario links
					for (const auto& link : l_vScenarioLink)
					{
						m_pOwnerScenario->setScenarioInputLink(
							link.first,
							link.second.first,
							link.second.second - 1);
					}
				}

				OpenViBE::CIdentifier toBeRemovedId = m_vInput[ui32InputIndex].m_oIdentifier;
				OpenViBE::CString toBeRemovedName   = m_vInput[ui32InputIndex].m_sName;
				
				// Erases actual input
				m_vInput.erase(m_vInput.begin() + ui32InputIndex);

				// Reconnects box links
				for (const auto& link : l_vLink)
				{
					CIdentifier newId;
					m_pOwnerScenario->connect(
						newId,
						link.first.first,
						link.first.second,
						link.second.first,
						link.second.second - 1,
						OV_UndefinedIdentifier);
				}
				
				// erase name key				
				auto itName = m_vInputNameToIndex.find(toBeRemovedName);				
				OV_ERROR_UNLESS_KRF(
					itName != m_vInputNameToIndex.end(),
					"No input found with name " << toBeRemovedName,
					ErrorType::ResourceNotFound
					);
				m_vInputNameToIndex.erase(itName);
				
				// erase identifier key if defined
				if (toBeRemovedId != OV_UndefinedIdentifier)
				{
					auto itIdent = m_vInputIdentifierToIndex.find(toBeRemovedId);
					OV_ERROR_UNLESS_KRF(
					            itIdent != m_vInputIdentifierToIndex.end(),
					            "No input found with id " << toBeRemovedId.toString(),
					            ErrorType::ResourceNotFound
					            );
					m_vInputIdentifierToIndex.erase(itIdent);				
				}

				if (bNotify)
				{
					this->notify(BoxModification_InputRemoved, ui32InputIndex);
				}

				return true;
			}

			virtual uint32_t getInputCount(void) const
			{
				uint32_t nbElement = 0;
				for (const auto& input : m_vInput)
				{
					if (!input.m_bMissing)
					{
						nbElement++;
					}					
				}	
				return nbElement;
			}
			
			virtual uint32_t getInputCountWithMissing(void) const
			{
				return static_cast<uint32_t>(m_vInput.size());
			}

			virtual bool getInputType(const uint32_t ui32InputIndex, OpenViBE::CIdentifier& rTypeIdentifier) const
			{
				OV_ERROR_UNLESS_KRF(
					ui32InputIndex < m_vInput.size(),
					"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vInput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rTypeIdentifier=m_vInput[ui32InputIndex].m_oTypeIdentifier;
				return true;
			}
			
			virtual bool getInputType(const OpenViBE::CIdentifier& rInputIdentifier, OpenViBE::CIdentifier& rTypeIdentifier) const
			{
				auto it = m_vInputIdentifierToIndex.find(rInputIdentifier);
				OV_ERROR_UNLESS_KRF(
				            it != m_vSettingIdentifierToIndex.end(),
				            "Failed to find input with id " << rInputIdentifier.toString(),
				            ErrorType::ResourceNotFound
				            );
				
				return getInputType(it->second,rTypeIdentifier);
			}
			
			virtual bool getInputType(const OpenViBE::CString rsInputName, OpenViBE::CIdentifier& rTypeIdentifier) const
			{
				auto it = m_vInputNameToIndex.find(rsInputName);
				OV_ERROR_UNLESS_KRF(
				            it != m_vInputNameToIndex.end(),
				            "Failed to find input with name " << rsInputName,
				            ErrorType::ResourceNotFound
				            );
				
				return getInputType(it->second,rTypeIdentifier);
			}
			

			virtual bool getInputName(const uint32_t ui32InputIndex, OpenViBE::CString& rName) const
			{
				OV_ERROR_UNLESS_KRF(
					ui32InputIndex < m_vInput.size(),
					"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vInput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rName=m_vInput[ui32InputIndex].m_sName;
				return true;
			}
			
			virtual bool getInputName(const OpenViBE::CIdentifier& rInputIdentifier, OpenViBE::CString& rName) const
			{
				auto it = m_vInputIdentifierToIndex.find(rInputIdentifier);
				OV_ERROR_UNLESS_KRF(
				            it != m_vSettingIdentifierToIndex.end(),
				            "Failed to find input with id " << rInputIdentifier.toString(),
				            ErrorType::ResourceNotFound
				            );
				
				return getInputName(it->second,rName);
			}
			
			virtual bool getInputIdentifier(const uint32_t ui32InputIndex, OpenViBE::CIdentifier &rIdentifier) const
			{
				rIdentifier = OV_UndefinedIdentifier;
				OV_ERROR_UNLESS_KRF(
					ui32InputIndex < m_vInput.size(),
					"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vInput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rIdentifier=m_vInput[ui32InputIndex].m_oIdentifier;
				return true;
			}
			
			
			virtual bool getInputIndex(const OpenViBE::CIdentifier &rIdentifier,uint32_t& rIndex) const
			{
				rIndex = OV_Value_UndefinedIndexUInt;
				auto it = m_vInputIdentifierToIndex.find(rIdentifier);
				OV_ERROR_UNLESS_KRF(
					it != m_vInputIdentifierToIndex.end(),					
					"Failed to find input with identifier " << rIdentifier.toString(),
					ErrorType::ResourceNotFound
				);
				
				rIndex = it->second;					
				return true;
			}
			
			virtual bool getInputMissingStatus(const uint32_t ui32InputIndex, bool &rvalue) const
			{
				OV_ERROR_UNLESS_KRF(
					ui32InputIndex < m_vInput.size(),
					"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vInput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rvalue = m_vInput[ui32InputIndex].m_bMissing;
				return true;
			}
			
			virtual bool getInputMissingStatus(const OpenViBE::CIdentifier& rInputIdentifier, bool& rvalue) const
			{
				auto it = m_vInputIdentifierToIndex.find(rInputIdentifier);
				OV_ERROR_UNLESS_KRF(
				            it != m_vSettingIdentifierToIndex.end(),
				            "Failed to find input with id " << rInputIdentifier.toString(),
				            ErrorType::ResourceNotFound
				            );
				
				return getInputMissingStatus(it->second, rvalue);
			}
			
			virtual bool hasInputWithIdentifier(const OpenViBE::CIdentifier& rIdentifier) const
			{
				return m_vInputIdentifierToIndex.find(rIdentifier) != m_vInputIdentifierToIndex.end();
			}
			
			virtual bool hasInputWithType(const uint32_t ui32InputIndex, const OpenViBE::CIdentifier& rTypeIdentifier) const
			{
				if (ui32InputIndex < getInputCount())
				{
					CIdentifier type;
					getInputType(ui32InputIndex, type);
					return (type == rTypeIdentifier);
				}
				return false;
			}

			virtual bool setInputType(const uint32_t ui32InputIndex, const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				OV_ERROR_UNLESS_KRF(
					this->getTypeManager().isStream(rTypeIdentifier),
					"While changing box '" << getName() << "' input type, unknown stream type identifier " << rTypeIdentifier.toString(),
					ErrorType::BadArgument
				);

				OV_ERROR_UNLESS_KRF(
					ui32InputIndex < m_vInput.size(),
					"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vInput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				m_vInput[ui32InputIndex].m_oTypeIdentifier=rTypeIdentifier;

				this->notify(BoxModification_InputTypeChanged, ui32InputIndex);

				return true;
			}
			
			virtual bool setInputType(const OpenViBE::CIdentifier& rInputIdentifier, const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				auto it = m_vInputIdentifierToIndex.find(rInputIdentifier);
				OV_ERROR_UNLESS_KRF(
				            it != m_vInputIdentifierToIndex.end(),
				            "Failed to find Input with id " << rInputIdentifier.toString(),
				            ErrorType::ResourceNotFound
				            );
				
				return setInputType(it->second,rTypeIdentifier);
			}
			
			virtual bool setInputType(const OpenViBE::CString& rsInputName, const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				auto it = m_vInputNameToIndex.find(rsInputName);
				OV_ERROR_UNLESS_KRF(
				            it != m_vInputNameToIndex.end(),
				            "Failed to find Input with name " << rsInputName,
				            ErrorType::ResourceNotFound
				            );
				
				return setInputType(it->second,rTypeIdentifier);
			}

			virtual bool setInputName(const uint32_t ui32InputIndex, const OpenViBE::CString& rName)
			{
				OV_ERROR_UNLESS_KRF(
					ui32InputIndex < m_vInput.size(),
					"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vInput.size() - 1) << "])",
					ErrorType::OutOfBound
				);
				
				if(m_vInput[ui32InputIndex].m_sName == rName)
				{
					// no change, don't bother notifying
					return true;
				}
				
				// remove entry from name key map
				auto it = m_vInputNameToIndex.find(m_vInput[ui32InputIndex].m_sName);
				OV_ERROR_UNLESS_KRF(
					it != m_vInputNameToIndex.end(),
					"Failed to find setting with name " << m_vInput[ui32InputIndex].m_sName,
					ErrorType::ResourceNotFound
				);
				m_vInputNameToIndex.erase(it);
								
				// check for duplicated name key and update if necessary					
				OpenViBE::CString newName = this->getUnusedName(m_vInputNameToIndex,rName);
				m_vInputNameToIndex[newName] = ui32InputIndex;	
				m_vInput[ui32InputIndex].m_sName = newName;
				
				OV_ERROR_UNLESS_KRF(m_vInputNameToIndex.size() == m_vInput.size(),
				            "Box " << m_sName << " has corrupted name map storage",
				            ErrorType::BadResourceCreation
				            );
				            
				this->notify(BoxModification_InputNameChanged, ui32InputIndex);

				return true;
			}
			
			virtual bool setInputName(const OpenViBE::CIdentifier& rInputIdentifier, const OpenViBE::CString& rName)
			{
				auto it = m_vInputIdentifierToIndex.find(rInputIdentifier);
				OV_ERROR_UNLESS_KRF(
				            it != m_vInputIdentifierToIndex.end(),
				            "Failed to find Input with id " << rInputIdentifier.toString(),
				            ErrorType::ResourceNotFound
				            );
				
				return setInputName(it->second,rName);
			}
			
			virtual bool setInputMissingStatus(const uint32_t ui32InputIndex, const bool rValue)
			{				
				OV_ERROR_UNLESS_KRF(
					ui32InputIndex < m_vInput.size(),
					"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vInput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				m_vInput[ui32InputIndex].m_bMissing = rValue;

				return true;
			}
			
			virtual bool setInputMissingStatus(const OpenViBE::CIdentifier& rInputIdentifier, const bool rValue)
			{
				auto it = m_vInputIdentifierToIndex.find(rInputIdentifier);
				OV_ERROR_UNLESS_KRF(
				            it != m_vInputIdentifierToIndex.end(),
				            "Failed to find Input with id " << rInputIdentifier.toString(),
				            ErrorType::ResourceNotFound
				            );
				
				return setInputMissingStatus(it->second,rValue);
			}

			//___________________________________________________________________//
			//                                                                   //

			virtual bool addOutput(const OpenViBE::CString& rsName, const OpenViBE::CIdentifier& rTypeIdentifier, const OpenViBE::CIdentifier& rIdentifier, const bool bNotify)
			{
				OV_ERROR_UNLESS_KRF(
					this->getTypeManager().isStream(rTypeIdentifier),
					"While adding output '" << rsName << "' to box '" << getName() << "', unknown stream type identifier " << rTypeIdentifier.toString(),
					ErrorType::BadArgument
				);
				uint32_t position = m_vOutput.size();
				m_vOutput.push_back(COutput(rsName, rTypeIdentifier, rIdentifier));
				
				if (rIdentifier != OV_UndefinedIdentifier)
				{
					// add access by CIdentifier key if defined so that size differs from m_vOutput
					m_vOutputIdentifierToIndex[rIdentifier] = position;	
				}
				
				// add access by name key (always done so that synchronized with m_vOutput 
				OpenViBE::CString newName = this->getUnusedName(m_vOutputNameToIndex,rsName);
				m_vOutput[position].m_sName = newName;
				m_vOutputNameToIndex[newName] = position;
				
				OV_ERROR_UNLESS_KRF(
				            m_vOutputNameToIndex.size() == m_vOutput.size(),
				            "Box " << m_sName << " has corrupted name map storage",
				            ErrorType::BadResourceCreation
				            );

				if (bNotify)
				{
					this->notify(BoxModification_OutputAdded, this->getOutputCount() - 1);
				}

				return true;
			}

			virtual bool removeOutput(const uint32_t ui32OutputIndex, const bool bNotify=true)
			{
				OV_ERROR_UNLESS_KRF(
					ui32OutputIndex < m_vOutput.size(),
					"Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vOutput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				std::vector < std::pair < std::pair < uint64_t, uint32_t >, std::pair < uint64_t, uint32_t > > > l_vLink;

				if (m_pOwnerScenario)
				{
					CIdentifier* identifierList = nullptr;
					size_t nbElems = 0;
					
					m_pOwnerScenario->getLinkIdentifierFromBoxOutputList(m_oIdentifier, ui32OutputIndex, &identifierList, &nbElems);
					for (size_t i = 0; i < nbElems; ++i)
					{
						m_pOwnerScenario->disconnect(identifierList[i]);
					}
					m_pOwnerScenario->releaseIdentifierList(identifierList);
					
					
					
					// $$$
					// The way the links are removed here
					// is not correct because they are all
					// collected and then all removed. In case
					// the box listener callback on box removal,
					// the nextcoming links would potentially be
					// invalid
					
					
					{
						CIdentifier* identifierList = nullptr;
						size_t nbElems = 0;
						m_pOwnerScenario->getLinkIdentifierFromBoxOutputList(m_oIdentifier, ui32OutputIndex, &identifierList, &nbElems);
						for (size_t i = 0; i < nbElems; ++i)
						{
							const OpenViBE::CIdentifier& cur_id = identifierList[i];
							ILink* l_pLink = m_pOwnerScenario->getLinkDetails(cur_id);
							if (l_pLink->getSourceBoxOutputIndex() > ui32OutputIndex)
							{
								l_vLink.push_back({
								                      {
								                          l_pLink->getSourceBoxIdentifier().toUInteger(),
								                          l_pLink->getSourceBoxOutputIndex()
								                      },
								                      {
								                          l_pLink->getTargetBoxIdentifier().toUInteger(),
								                          l_pLink->getTargetBoxInputIndex()
								                      }
								                  });
								if (m_pOwnerScenario->isLink(cur_id))
								{
									m_pOwnerScenario->disconnect(cur_id);
								}
							}
						}
						m_pOwnerScenario->releaseIdentifierList(identifierList);
					}
					
					// This reorganizes the parent's scenario links if this box is not actually a scenario
					if (m_oIdentifier != OV_UndefinedIdentifier)
					{
						std::vector < std::pair < uint32_t, std::pair < uint64_t, uint32_t > > > l_vScenarioLink;
						for(uint32_t scenarioOutputIndex = 0; scenarioOutputIndex < m_pOwnerScenario->getOutputCount(); scenarioOutputIndex++)
						{
							CIdentifier l_oBoxIdentier;
							uint32_t l_ui32BoxConnectorIndex = uint32_t(-1);
							m_pOwnerScenario->getScenarioOutputLink(scenarioOutputIndex, l_oBoxIdentier, l_ui32BoxConnectorIndex);
							if (l_oBoxIdentier == m_oIdentifier)
							{
								if (l_ui32BoxConnectorIndex > ui32OutputIndex)
								{
									l_vScenarioLink.push_back({
									                              scenarioOutputIndex,
									                              {
									                                  l_oBoxIdentier.toUInteger(),
									                                  l_ui32BoxConnectorIndex
									                              }
									                          });
								}
								if (l_ui32BoxConnectorIndex >= ui32OutputIndex)
								{
									m_pOwnerScenario->removeScenarioOutputLink(scenarioOutputIndex, l_oBoxIdentier, l_ui32BoxConnectorIndex);
								}
							}
						}
						
						// Reconnects scenario links
						for(const auto& link : l_vScenarioLink)
						{
							m_pOwnerScenario->setScenarioOutputLink(
							            link.first,
							            link.second.first,
							            link.second.second - 1);
						}
					}
				}
					OpenViBE::CIdentifier toBeRemovedId = m_vOutput[ui32OutputIndex].m_oIdentifier;
					OpenViBE::CString toBeRemovedName   = m_vOutput[ui32OutputIndex].m_sName;
					
					// Erases actual output
					m_vOutput.erase(m_vOutput.begin()+ui32OutputIndex);
					
					// Reconnects box links
					if (m_pOwnerScenario)
					{
						for(const auto& link : l_vLink)
						{
							CIdentifier newId;
							m_pOwnerScenario->connect(
							            newId,
							            link.first.first,
							            link.first.second - 1,
							            link.second.first,
							            link.second.second,
							            OV_UndefinedIdentifier);
						}
					}
				

				// erase name key				
				auto itName = m_vOutputNameToIndex.find(toBeRemovedName);				
				OV_ERROR_UNLESS_KRF(
					itName != m_vOutputNameToIndex.end(),
					"No output found with name " << toBeRemovedName,
					ErrorType::ResourceNotFound
					);
				m_vOutputNameToIndex.erase(itName);
				
				// erase identifier key if defined
				if (toBeRemovedId != OV_UndefinedIdentifier)
				{
					auto itIdent = m_vOutputIdentifierToIndex.find(toBeRemovedId);
					OV_ERROR_UNLESS_KRF(
					            itIdent != m_vOutputIdentifierToIndex.end(),
					            "No output found with id " << toBeRemovedId.toString(),
					            ErrorType::ResourceNotFound
					            );
					m_vOutputIdentifierToIndex.erase(itIdent);				
				}

				if (bNotify)
				{
					this->notify(BoxModification_OutputRemoved, ui32OutputIndex);
				}

				return true;
			}

			virtual uint32_t getOutputCount(void) const
			{
				uint32_t nbElement = 0;
				for (const auto& output : m_vOutput)
				{
					if (!output.m_bMissing)
					{
						nbElement++;
					}					
				}	
				return nbElement;
			}
			
			virtual uint32_t getOutputCountWithMissing(void) const
			{
				return static_cast<uint32_t>(m_vOutput.size());
			}

			virtual bool getOutputType(const uint32_t ui32OutputIndex, OpenViBE::CIdentifier& rTypeIdentifier) const
			{
				OV_ERROR_UNLESS_KRF(
					ui32OutputIndex < m_vOutput.size(),
					"Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vOutput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rTypeIdentifier=m_vOutput[ui32OutputIndex].m_oTypeIdentifier;
				return true;
			}
			
			virtual bool getOutputType(const OpenViBE::CIdentifier& rOutputIdentifier, OpenViBE::CIdentifier& rTypeIdentifier) const
			{
				auto it = m_vOutputIdentifierToIndex.find(rOutputIdentifier);
				OV_ERROR_UNLESS_KRF(
				            it != m_vSettingIdentifierToIndex.end(),
				            "Failed to find Output with id " << rOutputIdentifier.toString(),
				            ErrorType::ResourceNotFound
				            );
				
				return getOutputType(it->second,rTypeIdentifier);
			}
			
			virtual bool getOutputType(const OpenViBE::CString rsOutputName, OpenViBE::CIdentifier& rTypeIdentifier) const
			{
				auto it = m_vOutputNameToIndex.find(rsOutputName);
				OV_ERROR_UNLESS_KRF(
				            it != m_vOutputNameToIndex.end(),
				            "Failed to find Output with name " << rsOutputName,
				            ErrorType::ResourceNotFound
				            );
				
				return getOutputType(it->second,rTypeIdentifier);
			}

			virtual bool getOutputName(const uint32_t ui32OutputIndex, OpenViBE::CString& rName) const
			{
				OV_ERROR_UNLESS_KRF(
					ui32OutputIndex < m_vOutput.size(),
					"Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vOutput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rName=m_vOutput[ui32OutputIndex].m_sName;
				return true;
			}
			
			virtual bool getOutputName(const OpenViBE::CIdentifier& rOutputIdentifier, OpenViBE::CString& rName) const
			{
				auto it = m_vOutputIdentifierToIndex.find(rOutputIdentifier);
				OV_ERROR_UNLESS_KRF(
				            it != m_vSettingIdentifierToIndex.end(),
				            "Failed to find Output with id " << rOutputIdentifier.toString(),
				            ErrorType::ResourceNotFound
				            );
				
				return getOutputName(it->second,rName);
			}
			
			virtual bool getOutputIdentifier(const uint32_t ui32OutputIndex, OpenViBE::CIdentifier &rIdentifier) const
			{
				rIdentifier = OV_UndefinedIdentifier;
				OV_ERROR_UNLESS_KRF(
					ui32OutputIndex < m_vOutput.size(),
					"Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vOutput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rIdentifier=m_vOutput[ui32OutputIndex].m_oIdentifier;
				return true;
			}
			
			virtual bool getOutputIndex(const OpenViBE::CIdentifier &rIdentifier,uint32_t& rIndex) const
			{
				rIndex = OV_Value_UndefinedIndexUInt;
				auto it = m_vOutputIdentifierToIndex.find(rIdentifier);
				OV_ERROR_UNLESS_KRF(
					it != m_vOutputIdentifierToIndex.end(),					
					"Failed to find output with identifier " << rIdentifier.toString(),
					ErrorType::ResourceNotFound
				);
				
				rIndex = it->second;					
				return true;
			}
			
			virtual bool getOutputMissingStatus(const uint32_t ui32OutputIndex, bool &rvalue) const
			{
				OV_ERROR_UNLESS_KRF(
					ui32OutputIndex < m_vOutput.size(),
					"Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vOutput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rvalue = m_vOutput[ui32OutputIndex].m_bMissing;
				return true;
			}
			
			virtual bool getOutputMissingStatus(const OpenViBE::CIdentifier& rOutputIdentifier, bool& rvalue) const
			{
				auto it = m_vOutputIdentifierToIndex.find(rOutputIdentifier);
				OV_ERROR_UNLESS_KRF(
				            it != m_vSettingIdentifierToIndex.end(),
				            "Failed to find Output with id " << rOutputIdentifier.toString(),
				            ErrorType::ResourceNotFound
				            );
				
				return getOutputMissingStatus(it->second, rvalue);
			}
			
			virtual bool hasOutputWithIdentifier(const OpenViBE::CIdentifier& rIdentifier) const
			{
				return m_vOutputIdentifierToIndex.find(rIdentifier) != m_vOutputIdentifierToIndex.end();
			}
			
			virtual bool hasOutputWithType(const uint32_t ui32OutputIndex, const OpenViBE::CIdentifier& rTypeIdentifier) const
			{
				if (ui32OutputIndex < getOutputCount())
				{
					CIdentifier type;
					getOutputType(ui32OutputIndex, type);
					return (type == rTypeIdentifier);
				}
				return false;
			}

			virtual bool setOutputType(const uint32_t ui32OutputIndex, const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				OV_ERROR_UNLESS_KRF(
					this->getTypeManager().isStream(rTypeIdentifier),
					"While changing box '" << getName() << "' output type, unknown stream type identifier " << rTypeIdentifier.toString(),
					ErrorType::BadArgument
				);

				OV_ERROR_UNLESS_KRF(
					ui32OutputIndex < m_vOutput.size(),
					"Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vOutput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				m_vOutput[ui32OutputIndex].m_oTypeIdentifier=rTypeIdentifier;

				this->notify(BoxModification_OutputTypeChanged, ui32OutputIndex);

				return true;
			}
			
			virtual bool setOutputType(const OpenViBE::CIdentifier& rOutputIdentifier, const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				auto it = m_vOutputIdentifierToIndex.find(rOutputIdentifier);
				OV_ERROR_UNLESS_KRF(
				            it != m_vOutputIdentifierToIndex.end(),
				            "Failed to find output with id " << rOutputIdentifier.toString(),
				            ErrorType::ResourceNotFound
				            );
				
				return setOutputType(it->second,rTypeIdentifier);
			}
			
			virtual bool setOutputType(const OpenViBE::CString rsOutputName, const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				auto it = m_vOutputNameToIndex.find(rsOutputName);
				OV_ERROR_UNLESS_KRF(
				            it != m_vOutputNameToIndex.end(),
				            "Failed to find output with name " << rsOutputName,
				            ErrorType::ResourceNotFound
				            );
				
				return setOutputType(it->second,rTypeIdentifier);
			}

			virtual bool setOutputName(const uint32_t ui32OutputIndex, const OpenViBE::CString& rName)
			{
				OV_ERROR_UNLESS_KRF(
				            ui32OutputIndex < m_vOutput.size(),
				            "Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vOutput.size() - 1) << "])",
				            ErrorType::OutOfBound
				            );
				
				if(m_vOutput[ui32OutputIndex].m_sName == rName)
				{
					// no change, don't bother notifying
					return true;
				}
				
				// remove entry from name key map
				auto it = m_vOutputNameToIndex.find(m_vOutput[ui32OutputIndex].m_sName);
				OV_ERROR_UNLESS_KRF(
				            it != m_vOutputNameToIndex.end(),
				            "Failed to find setting with name " << m_vOutput[ui32OutputIndex].m_sName,
				            ErrorType::ResourceNotFound
				            );
				m_vOutputNameToIndex.erase(it);
				
				// check for duplicated name key and update if necessary					
				OpenViBE::CString newName = this->getUnusedName(m_vOutputNameToIndex,rName);
				m_vOutputNameToIndex[newName] = ui32OutputIndex;	
				m_vOutput[ui32OutputIndex].m_sName = newName;
				
				OV_ERROR_UNLESS_KRF(m_vOutputNameToIndex.size() == m_vOutput.size(),
				                    "Box " << m_sName << " has corrupted name map storage",
				                    ErrorType::BadResourceCreation
				                    );
				
				this->notify(BoxModification_OutputNameChanged, ui32OutputIndex);
				
				return true;
			}
			
			virtual bool setOutputName(const OpenViBE::CIdentifier& rOutputIdentifier, const OpenViBE::CString& rName)
			{
				auto it = m_vOutputIdentifierToIndex.find(rOutputIdentifier);
				OV_ERROR_UNLESS_KRF(
				            it != m_vOutputIdentifierToIndex.end(),
				            "Failed to find Output with id " << rOutputIdentifier.toString(),
				            ErrorType::ResourceNotFound
				            );
				
				return setOutputName(it->second,rName);
			}
			
			virtual bool setOutputMissingStatus(const uint32_t ui32OutputIndex, const bool rValue)
			{				
				OV_ERROR_UNLESS_KRF(
					ui32OutputIndex < m_vOutput.size(),
					"Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vOutput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				m_vOutput[ui32OutputIndex].m_bMissing = rValue;

				return true;
			}
			
			virtual bool setOutputMissingStatus(const OpenViBE::CIdentifier& rOutputIdentifier, const bool rValue)
			{
				auto it = m_vOutputIdentifierToIndex.find(rOutputIdentifier);
				OV_ERROR_UNLESS_KRF(
				            it != m_vOutputIdentifierToIndex.end(),
				            "Failed to find Output with id " << rOutputIdentifier.toString(),
				            ErrorType::ResourceNotFound
				            );
				
				return setOutputMissingStatus(it->second,rValue);
			}

			virtual bool addInputSupport(const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				m_vSupportInputType.push_back(rTypeIdentifier);
				return true;
			}

			virtual bool hasInputSupport(const OpenViBE::CIdentifier& rTypeIdentifier) const
			{
				//If there is no type specify, we allow all
				if(m_vSupportInputType.empty())
					return true;

				for(size_t i =0; i < m_vSupportInputType.size(); ++i)
				{
					if(m_vSupportInputType[i] == rTypeIdentifier)
						return true;
				}
				return false;
			}

			virtual bool addOutputSupport(const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				m_vSupportOutputType.push_back(rTypeIdentifier);
				return true;
			}

			virtual bool hasOutputSupport(const OpenViBE::CIdentifier& rTypeIdentifier) const
			{
				//If there is no type specify, we allow all
				if(m_vSupportOutputType.empty())
					return true;

				for(size_t i =0; i < m_vSupportOutputType.size(); ++i)
				{
					if(m_vSupportOutputType[i] == rTypeIdentifier)
						return true;
				}
				return false;
			}

			virtual bool setSupportTypeFromAlgorithmIdentifier(const OpenViBE::CIdentifier &rTypeIdentifier)
			{

				const Plugins::IPluginObjectDesc* l_pPluginObjectDescriptor=this->getKernelContext().getPluginManager().getPluginObjectDescCreating(rTypeIdentifier);
				const Plugins::IBoxAlgorithmDesc *l_pBoxAlgorithmDescriptor=dynamic_cast<const Plugins::IBoxAlgorithmDesc*>(l_pPluginObjectDescriptor);

				OV_ERROR_UNLESS_KRF(
					l_pBoxAlgorithmDescriptor,
					"Tried to initialize with an unregistered algorithm",
					ErrorType::Internal
				);

				//We use the neutralized version of CBoxProto to just initialize the stream restriction mecanism
				CBoxProtoRestriction oTempProto(this->getKernelContext(), *this);
				l_pBoxAlgorithmDescriptor->getBoxPrototype(oTempProto);
				return true;
			}
			
			virtual std::vector<CIdentifier> getInputSupportTypes() const
			{
				return m_vSupportInputType;
			}
			
			virtual std::vector<CIdentifier> getOutputSupportTypes() const
			{
				return m_vSupportOutputType;
			}
			
			virtual void clearOutputSupportTypes()
			{
				m_vSupportOutputType.clear();
			}
			
			virtual void clearInputSupportTypes()
			{
				m_vSupportInputType.clear();
			}

			
			OpenViBE::CIdentifier getUnusedSettingIdentifier(const OpenViBE::CIdentifier& suggestedIdentifier = OV_UndefinedIdentifier) const
			{
				uint64 l_ui64Identifier=System::Math::randomUInteger64();
				if (suggestedIdentifier != OV_UndefinedIdentifier)
				{
					l_ui64Identifier = suggestedIdentifier.toUInteger();
				}
				
				CIdentifier l_oResult;
				std::map<CIdentifier, uint32_t>::const_iterator itSettings;
				do
				{					
					l_oResult=CIdentifier(l_ui64Identifier);
					itSettings=m_vSettingIdentifierToIndex.find(l_oResult);
					l_ui64Identifier++;
				}
				while(itSettings != m_vSettingIdentifierToIndex.end()
					|| l_oResult==OV_UndefinedIdentifier
					);
				return l_oResult;
			}
			
			OpenViBE::CIdentifier getUnusedInputIdentifier(const OpenViBE::CIdentifier& suggestedIdentifier = OV_UndefinedIdentifier) const
			{
				uint64 l_ui64Identifier=System::Math::randomUInteger64();
				if (suggestedIdentifier != OV_UndefinedIdentifier)
				{
					l_ui64Identifier = suggestedIdentifier.toUInteger();
				}
				
				CIdentifier l_oResult;
				std::map<CIdentifier, uint32_t>::const_iterator itInputs;
				do
				{					
					l_oResult=CIdentifier(l_ui64Identifier);
					itInputs=m_vInputIdentifierToIndex.find(l_oResult);
					l_ui64Identifier++;
				}
				while(itInputs != m_vInputIdentifierToIndex.end()
					|| l_oResult==OV_UndefinedIdentifier
					);
				return l_oResult;
			}
			
			OpenViBE::CIdentifier getUnusedOutputIdentifier(const OpenViBE::CIdentifier& suggestedIdentifier = OV_UndefinedIdentifier) const
			{
				uint64 l_ui64Identifier=System::Math::randomUInteger64();
				if (suggestedIdentifier != OV_UndefinedIdentifier)
				{
					l_ui64Identifier = suggestedIdentifier.toUInteger();
				}
				
				CIdentifier l_oResult;
				std::map<CIdentifier, uint32_t>::const_iterator itOutputs;
				do
				{					
					l_oResult=CIdentifier(l_ui64Identifier);
					itOutputs=m_vOutputIdentifierToIndex.find(l_oResult);
					l_ui64Identifier++;
				}
				while(itOutputs != m_vOutputIdentifierToIndex.end()
					|| l_oResult==OV_UndefinedIdentifier
					);
				return l_oResult;
			}
			
			OpenViBE::CIdentifier combineIdentifierWithString(const OpenViBE::CIdentifier& seedId, const OpenViBE::CString &strValue) const
			{
				uint64_t s = seedId.toUInteger();
				uint8_t sTable[sizeof(s)];				
				
				System::Memory::hostToLittleEndian(s, sTable);
				for(uint32_t i = 0 ; i < strValue.length(); i++ )
				{
					uint32_t index = i % sizeof(s);
					sTable[index] = sTable[index] + static_cast<unsigned char>(strValue[i]);
				}
				System::Memory::littleEndianToHost(sTable, &s);
				return OpenViBE::CIdentifier(s);
			}
		
			virtual bool addSetting(
				const OpenViBE::CString& rsName,
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sDefaultValue,
				const uint32_t ui32Index,
				const bool bModifiability,
				const OpenViBE::CIdentifier& rIdentifier,
				const bool bNotify)
			{
				CString l_sValue(sDefaultValue);
				if(this->getTypeManager().isEnumeration(rTypeIdentifier))
				{
					if(this->getTypeManager().getEnumerationEntryValueFromName(rTypeIdentifier, sDefaultValue)==OV_UndefinedIdentifier)
					{
						if(this->getTypeManager().getEnumerationEntryCount(rTypeIdentifier)!=0)
						{
							// get value to the first enum entry
							// and eventually correct this after
							uint64_t l_ui64Value=0;
							this->getTypeManager().getEnumerationEntry(rTypeIdentifier, 0, l_sValue, l_ui64Value);

							// Find if the default value string actually is an identifier, otherwise just keep the zero index name as default.
							CIdentifier l_oDefaultValueIdentifier;
							l_oDefaultValueIdentifier.fromString(sDefaultValue);

							// Finally, if it is an identifier, then a name should be found
							// from the type manager ! Otherwise l_sValue is left to the default.
							CString l_sCandidateValue=this->getTypeManager().getEnumerationEntryNameFromValue(rTypeIdentifier, l_oDefaultValueIdentifier.toUInteger());
							if(l_sCandidateValue!=CString(""))
							{
								l_sValue=l_sCandidateValue;
							}
						}
					}
				}

				CSetting s;
				s.m_sName=rsName;
				s.m_oTypeIdentifier=rTypeIdentifier;
				s.m_sDefaultValue=l_sValue;
				s.m_sValue=l_sValue;
				s.m_bMod=bModifiability;
				s.m_oIdentifier = rIdentifier;

				uint32_t l_ui32Index = ui32Index;

				

				uint32_t l_ui32InsertLocation;

				if(ui32Index == OV_Value_UndefinedIndexUInt || ui32Index == static_cast<uint32_t>(m_vSetting.size()))
				{
					m_vSetting.push_back(s);					
					l_ui32InsertLocation = (static_cast<uint32_t>(m_vSetting.size()))-1;
				}
				else
				{
					OV_ERROR_UNLESS_KRF(
					            ui32Index <= static_cast<uint32_t>(m_vSetting.size()),
					            "Tried to push '" << rsName << "' to slot " << ui32Index << " with the array size being " << static_cast<uint32>(m_vSetting.size()),
					            ErrorType::OutOfBound
					            );
					            
					typename std::vector<CSetting>::iterator l_it = m_vSetting.begin();
					l_it += l_ui32Index;
					m_vSetting.insert(l_it, s);
					l_ui32InsertLocation = ui32Index;
				}
				
				if (s.m_oIdentifier != OV_UndefinedIdentifier)
				{
					// add access by CIdentifier key if defined so that size differs from m_vSetting
					m_vSettingIdentifierToIndex[s.m_oIdentifier] = l_ui32InsertLocation;	
				}			
				// add access by name key (always done so that synchronized with m_vSetting 
				OpenViBE::CString newName = this->getUnusedName(m_vSettingNameToIndex,s.m_sName);
				m_vSetting[l_ui32InsertLocation].m_sName = newName;
				m_vSettingNameToIndex[newName] = l_ui32InsertLocation;
				
				OV_ERROR_UNLESS_KRF(
				            m_vSettingNameToIndex.size() == m_vSetting.size(),
				            "Box " << m_sName << " has corrupted name map storage",
				            ErrorType::BadResourceCreation
				            );

				//if this setting is modifiable, keep its index
				if(bModifiability)
				{
					m_vModifiableSettingIndexes.push_back(l_ui32Index);
				}

				this->getLogManager() << LogLevel_Debug << "Pushed '" << m_vSetting[l_ui32InsertLocation].m_sName << "' : '" << m_vSetting[l_ui32InsertLocation].m_sValue << "' to slot " << l_ui32InsertLocation << " with the array size now " << static_cast<int32_t>(m_vSetting.size()) << "\n";

				if (bNotify)
				{
					this->notify(BoxModification_SettingAdded, l_ui32InsertLocation);
					this->notifySettingChange(SettingAdd, l_ui32InsertLocation);
				}

				return true;
			}

			virtual bool removeSetting(const uint32_t ui32SettingIndex, const bool bNotify=true)
			{
				auto it = m_vSetting.begin() + ui32SettingIndex;
				OV_ERROR_UNLESS_KRF(
					it != m_vSetting.end(),
					"No setting found at index " << ui32SettingIndex,
					ErrorType::ResourceNotFound
					);
					
				OpenViBE::CIdentifier toBeRemovedId = m_vSetting[ui32SettingIndex].m_oIdentifier;
				OpenViBE::CString toBeRemovedName   = m_vSetting[ui32SettingIndex].m_sName;
				
				it=m_vSetting.erase(it);

				//update the modifiable setting indexes
				for (auto it2 = m_vModifiableSettingIndexes.begin(); it2 != m_vModifiableSettingIndexes.end();)
				{
					if (*it2 == ui32SettingIndex)
					{
						it2 = m_vModifiableSettingIndexes.erase(it2);
					}
					else if (*it2 > ui32SettingIndex)
					{
						*it2 -= 1;
						++it2;
					}
				}
				
				// erase name key				
				auto itName = m_vSettingNameToIndex.find(toBeRemovedName);				
				OV_ERROR_UNLESS_KRF(
					itName != m_vSettingNameToIndex.end(),
					"No setting found with name " << toBeRemovedName,
					ErrorType::ResourceNotFound
					);
				m_vSettingNameToIndex.erase(itName);
				
				// erase identifier key if defined
				if (toBeRemovedId != OV_UndefinedIdentifier)
				{
					auto itIdent = m_vSettingIdentifierToIndex.find(toBeRemovedId);
					OV_ERROR_UNLESS_KRF(
					            itIdent != m_vSettingIdentifierToIndex.end(),
					            "No setting found with id " << toBeRemovedId.toString(),
					            ErrorType::ResourceNotFound
					            );
					m_vSettingIdentifierToIndex.erase(itIdent);				
				}
				
				if (bNotify)
				{
					this->notify(BoxModification_SettingRemoved, ui32SettingIndex);
					this->notifySettingChange(SettingDelete, ui32SettingIndex);
				}

				return true;
			}

			virtual uint32_t getSettingCount(void) const
			{
				uint32_t nbElement = 0;
				for (const auto& setting : m_vSetting)
				{
					if (!setting.m_bMissing)
					{
						nbElement++;
					}					
				}	
				return nbElement;			
			}
			
			virtual uint32_t getSettingCountWithMissing(void) const
			{
				return static_cast<uint32_t>(m_vSetting.size());
			}

			virtual bool hasSettingWithName(const OpenViBE::CString& rName) const
			{
				return m_vSettingNameToIndex.find(rName) != m_vSettingNameToIndex.end();
			}
			
			virtual bool hasSettingWithIdentifier(const OpenViBE::CIdentifier& rIdentifier) const
			{
				return m_vSettingIdentifierToIndex.find(rIdentifier) != m_vSettingIdentifierToIndex.end();
			}
			
			virtual bool hasSettingWithType(const uint32_t ui32SettingIndex, const OpenViBE::CIdentifier& rTypeIdentifier) const
			{
				if (ui32SettingIndex < getSettingCount())
				{
					CIdentifier type;
					getSettingType(ui32SettingIndex, type);
					return (type == rTypeIdentifier);
				}
				return false;
			}
			
			virtual bool getSettingIndex(const OpenViBE::CString& rsName, uint32_t& rIndex) const
			{
				auto it = m_vSettingNameToIndex.find(rsName);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingNameToIndex.end(),					
					"Failed to find setting with name " << rsName,
					ErrorType::ResourceNotFound
				);
				
				rIndex = it->second;					
				return true;
			}
			
			virtual bool getSettingIndex(const OpenViBE::CIdentifier& rIdentifier, uint32_t& rIndex) const
			{
				auto it = m_vSettingIdentifierToIndex.find(rIdentifier);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingIdentifierToIndex.end(),					
					"Failed to find setting with id " << rIdentifier.toString(),
					ErrorType::ResourceNotFound
				);
				
				rIndex = it->second;					
				return true;				
			}
						
			virtual bool getSettingType(const uint32_t ui32SettingIndex, OpenViBE::CIdentifier& rTypeIdentifier) const
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rTypeIdentifier=m_vSetting[ui32SettingIndex].m_oTypeIdentifier;
				return true;
			}
			
			virtual bool getSettingType(const OpenViBE::CIdentifier &rIdentifier, OpenViBE::CIdentifier& rTypeIdentifier) const
			{
				auto it = m_vSettingIdentifierToIndex.find(rIdentifier);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingIdentifierToIndex.end(),					
					"Failed to find setting with id " << rIdentifier.toString(),
					ErrorType::ResourceNotFound
				);

				return getSettingType(it->second, rTypeIdentifier);
			}
			
			virtual bool getSettingType(const OpenViBE::CString& rsName, OpenViBE::CIdentifier& rTypeIdentifier) const
			{
				auto it = m_vSettingNameToIndex.find(rsName);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingNameToIndex.end(),					
					"Failed to find setting with name " << rsName,
					ErrorType::ResourceNotFound
				);

				return getSettingType(it->second, rTypeIdentifier);
			}

			virtual bool getSettingName(const uint32_t ui32SettingIndex, OpenViBE::CString& rName) const
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rName=m_vSetting[ui32SettingIndex].m_sName;
				return true;
			}
			
			virtual bool getSettingName(const OpenViBE::CIdentifier& rIdentifier, OpenViBE::CString& rName) const
			{
				auto it = m_vSettingIdentifierToIndex.find(rIdentifier);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingIdentifierToIndex.end(),
					"Failed to find setting with id " << rIdentifier.toString(),
					ErrorType::ResourceNotFound
				);
				
				return getSettingName(it->second,rName);				
			}

			virtual bool getSettingDefaultValue(const uint32_t ui32SettingIndex, OpenViBE::CString& rDefaultValue) const
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);
				
				rDefaultValue=m_vSetting[ui32SettingIndex].m_sDefaultValue;
				return true;
			}
			
			virtual bool getSettingDefaultValue(const OpenViBE::CIdentifier& rIdentifier, OpenViBE::CString& rDefaultValue) const
			{
				auto it = m_vSettingIdentifierToIndex.find(rIdentifier);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingIdentifierToIndex.end(),
					"Failed to find setting with id " << rIdentifier.toString(),
					ErrorType::ResourceNotFound
				);
				
				return getSettingDefaultValue(it->second,rDefaultValue);
			}
			
			virtual bool getSettingDefaultValue(const OpenViBE::CString& rsName, OpenViBE::CString& rDefaultValue) const
			{
				auto it = m_vSettingNameToIndex.find(rsName);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingNameToIndex.end(),					
					"Failed to find setting with name " << rsName,
					ErrorType::ResourceNotFound
				);

				return getSettingDefaultValue(it->second, rDefaultValue);
			}

			virtual bool getSettingValue(const uint32_t ui32SettingIndex, OpenViBE::CString& rValue) const
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rValue = m_vSetting[ui32SettingIndex].m_sValue;
				return true;
			}
			
			virtual bool getSettingValue(const OpenViBE::CIdentifier& rIdentifier, OpenViBE::CString& rValue) const
			{	
				auto it = m_vSettingIdentifierToIndex.find(rIdentifier);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingIdentifierToIndex.end(),
					"Failed to find setting with id " << rIdentifier.toString(),
					ErrorType::ResourceNotFound
				);
				
				return getSettingValue(it->second,rValue);
			}
			
			virtual bool getSettingValue(const OpenViBE::CString& rsName, OpenViBE::CString& rValue) const
			{			
				auto it = m_vSettingNameToIndex.find(rsName);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingNameToIndex.end(),					
					"Failed to find setting with name " << rsName,
					ErrorType::ResourceNotFound
				);

				return getSettingValue(it->second,rValue);
			}
			
			virtual bool getSettingIdentifier(const uint32_t ui32SettingIndex, OpenViBE::CIdentifier& rIdentifier) const
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rIdentifier=m_vSetting[ui32SettingIndex].m_oIdentifier;
				return true;
			}
			
			virtual bool getSettingIdentifier(const OpenViBE::CString &sName, OpenViBE::CIdentifier& rIdentifier) const
			{
				auto it = m_vSettingNameToIndex.find(sName);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingNameToIndex.end(),					
					"Failed to find setting with name " << sName,
					ErrorType::ResourceNotFound
				);
				
				return getSettingIdentifier(it->second,rIdentifier);
			}
			
			virtual bool getSettingMod(
					const uint32_t ui32SettingIndex,
					bool& rValue) const
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rValue=m_vSetting[ui32SettingIndex].m_bMod;
				return true;
			}
			
			virtual bool getSettingMod(const OpenViBE::CIdentifier& rIdentifier, bool& rValue) const
			{
				auto it = m_vSettingIdentifierToIndex.find(rIdentifier);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingIdentifierToIndex.end(),
					"Failed to find setting with id " << rIdentifier.toString(),
					ErrorType::ResourceNotFound
				);
				
				return getSettingMod(it->second,rValue);
			}
			
			virtual bool getSettingMod(const OpenViBE::CString& rsName, bool& rValue) const
			{
				auto it = m_vSettingNameToIndex.find(rsName);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingNameToIndex.end(),					
					"Failed to find setting with name " << rsName,
					ErrorType::ResourceNotFound
				);

				return getSettingMod(it->second,rValue);
			}
			
			virtual bool getSettingMissingStatus(const uint32_t ui32SettingIndex,	bool &rValue) const
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rValue=m_vSetting[ui32SettingIndex].m_bMissing;
				return true;
			}
			
			virtual bool getSettingMissingStatus(const OpenViBE::CIdentifier& rIdentifier, bool &rValue) const
			{
				auto it = m_vSettingIdentifierToIndex.find(rIdentifier);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingIdentifierToIndex.end(),
					"Failed to find setting with id " << rIdentifier.toString(),
					ErrorType::ResourceNotFound
				);
				
				return getSettingMissingStatus(it->second,rValue);
			}
			
			virtual bool setSettingType(const uint32_t ui32SettingIndex, const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				if(m_vSetting[ui32SettingIndex].m_oTypeIdentifier == rTypeIdentifier)
				{
					// no change, don't bother notifying
					return true;
				}

				m_vSetting[ui32SettingIndex].m_oTypeIdentifier=rTypeIdentifier;

				this->notify(BoxModification_SettingTypeChanged, ui32SettingIndex);
				this->notifySettingChange(SettingChange, ui32SettingIndex);

				return true;
			}
			
			virtual bool setSettingType(const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				auto it = m_vSettingIdentifierToIndex.find(rIdentifier);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingIdentifierToIndex.end(),
					"Failed to find setting with id " << rIdentifier.toString(),
					ErrorType::ResourceNotFound
				);
								
				return setSettingType(it->second,rTypeIdentifier);
			}
			
			virtual bool setSettingType(const OpenViBE::CString& rsName, const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				auto it = m_vSettingNameToIndex.find(rsName);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingNameToIndex.end(),
					"Failed to find setting with name " << rsName,
					ErrorType::ResourceNotFound
				);
				return setSettingType(it->second,rTypeIdentifier);
			}

			virtual bool setSettingName(const uint32_t ui32SettingIndex, const OpenViBE::CString& rName)
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);
				
				if(m_vSetting[ui32SettingIndex].m_sName == rName)
				{
					// no change, don't bother notifying
					return true;
				}				
				// remove entry from name key map
				auto it = m_vSettingNameToIndex.find(m_vSetting[ui32SettingIndex].m_sName);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingNameToIndex.end(),
					"Failed to find setting with name " << m_vSetting[ui32SettingIndex].m_sName,
					ErrorType::ResourceNotFound
				);
				m_vSettingNameToIndex.erase(it);
								
				// check for duplicated name key and update if necessary					
				OpenViBE::CString newName = this->getUnusedName(m_vSettingNameToIndex,rName);
				m_vSettingNameToIndex[newName] = ui32SettingIndex;	
				m_vSetting[ui32SettingIndex].m_sName = newName;
				
				OV_ERROR_UNLESS_KRF(m_vSettingNameToIndex.size() == m_vSetting.size(),
				            "Box " << m_sName << " has corrupted name map storage",
				            ErrorType::BadResourceCreation
				            );
				            
				this->notify(BoxModification_SettingNameChanged, ui32SettingIndex);
				this->notifySettingChange(SettingChange, ui32SettingIndex);

				return true;
			}
			
			virtual bool setSettingName(const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::CString& rName)
			{
				auto itIdentToIndex = m_vSettingIdentifierToIndex.find(rIdentifier);
				OV_ERROR_UNLESS_KRF(
					itIdentToIndex != m_vSettingIdentifierToIndex.end(),
					"Failed to find setting with id " << rIdentifier.toString(),
					ErrorType::ResourceNotFound
				);
				
				return setSettingName(itIdentToIndex->second,rName);
			}

			virtual bool setSettingDefaultValue(const uint32_t ui32SettingIndex, const OpenViBE::CString& rDefaultValue)
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				m_vSetting[ui32SettingIndex].m_sDefaultValue=rDefaultValue;

				this->notify(BoxModification_SettingDefaultValueChanged, ui32SettingIndex);

				return true;
			}
			
			virtual bool setSettingDefaultValue(const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::CString& rDefaultValue)
			{
				auto it = m_vSettingIdentifierToIndex.find(rIdentifier);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingIdentifierToIndex.end(),
					"Failed to find setting with id " << rIdentifier.toString(),
					ErrorType::ResourceNotFound
				);
				
				return setSettingDefaultValue(it->second,rDefaultValue);
			}
			
			virtual bool setSettingDefaultValue(const OpenViBE::CString& rsName, const OpenViBE::CString& rDefaultValue)
			{
				auto it = m_vSettingNameToIndex.find(rsName);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingNameToIndex.end(),
					"Failed to find setting with name " << rsName,
					ErrorType::ResourceNotFound
				);
				return setSettingDefaultValue(it->second,rDefaultValue);
			}

			virtual bool setSettingValue(const uint32_t ui32SettingIndex, const OpenViBE::CString& rValue, const bool bNotify=true)
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				if (m_vSetting[ui32SettingIndex].m_sValue != rValue)
				{
					m_vSetting[ui32SettingIndex].m_sValue=rValue;

					if (bNotify)
					{
						this->notify(BoxModification_SettingValueChanged, ui32SettingIndex);
						this->notifySettingChange(SettingValueUpdate, ui32SettingIndex);
					}
				}

				return true;
			}
			
			virtual bool setSettingValue(const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::CString& rValue)
			{
				auto it = m_vSettingIdentifierToIndex.find(rIdentifier);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingIdentifierToIndex.end(),
					"Failed to find setting with id " << rIdentifier.toString(),
					ErrorType::ResourceNotFound
				);
				
				return setSettingValue(it->second,rValue);
			}
			
			virtual bool setSettingValue(const OpenViBE::CString& rsName, const OpenViBE::CString& rValue)
			{
				auto it = m_vSettingNameToIndex.find(rsName);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingNameToIndex.end(),
					"Failed to find setting with name " << rsName,
					ErrorType::ResourceNotFound
				);

				return setSettingValue(it->second,rValue);
			}			

			virtual bool setSettingMod(const uint32_t ui32SettingIndex, const bool rValue)
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				m_vSetting[ui32SettingIndex].m_bMod=rValue;

				//this->notify(BoxModification_SettingNameChanged, ui32SettingIndex);
				return true;
			}
			
			virtual bool setSettingMod(const OpenViBE::CString& rsName, const bool rValue)
			{
				auto it = m_vSettingNameToIndex.find(rsName);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingNameToIndex.end(),
					"Failed to find setting with name " << rsName,
					ErrorType::ResourceNotFound
				);

				return setSettingMod(it->second,rValue);
			}
			
			virtual bool setSettingMod(const OpenViBE::CIdentifier& rIdentifier, const bool rValue)
			{
				auto it = m_vSettingIdentifierToIndex.find(rIdentifier);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingIdentifierToIndex.end(),
					"Failed to find setting with id " << rIdentifier.toString(),
					ErrorType::ResourceNotFound
				);

				return setSettingMod(it->second,rValue);
			}
			
			virtual bool setSettingMissingStatus(const uint32_t ui32SettingIndex, const bool rValue)
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				m_vSetting[ui32SettingIndex].m_bMissing = rValue;
				
				return true;
			}
			
			virtual bool setSettingMissingStatus(const OpenViBE::CIdentifier& rIdentifier, const bool rValue)
			{
				auto it = m_vSettingIdentifierToIndex.find(rIdentifier);
				OV_ERROR_UNLESS_KRF(
					it != m_vSettingIdentifierToIndex.end(),
					"Failed to find setting with id " << rIdentifier.toString(),
					ErrorType::ResourceNotFound
				);

				return setSettingMissingStatus(it->second,rValue);
			}
			
			virtual bool swapSettings(uint32_t indexA, uint32_t indexB)
			{
				OV_ERROR_UNLESS_KRF(
					indexA < m_vSetting.size(),
					"Setting index = [" << indexA << "] is out of range (max index = [" << static_cast<uint32_t>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);
				OV_ERROR_UNLESS_KRF(
					indexB < m_vSetting.size(),
					"Setting index = [" << indexB << "] is out of range (max index = [" << static_cast<uint32_t>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);
				
				CString nameA;				
				CIdentifier identifierA;
				CString nameB;				
				CIdentifier identifierB;
				
				getSettingName(indexA, nameA);
				getSettingIdentifier(indexA, identifierA);
				getSettingName(indexB, nameB);
				getSettingIdentifier(indexB, identifierB);
								
				auto itA = m_vSetting.begin()+indexA;
				auto itB = m_vSetting.begin()+indexB;
				// swap settings
				std::iter_swap(itA,itB);
				// update associated maps
				m_vSettingNameToIndex[nameA] = indexB;
				m_vSettingNameToIndex[nameB] = indexA;
				m_vSettingIdentifierToIndex[identifierA] = indexB;
				m_vSettingIdentifierToIndex[identifierB] = indexA;				
				
				return true;
			}
			
			virtual bool swapInputs(uint32_t indexA, uint32_t indexB)
			{
				OV_ERROR_UNLESS_KRF(
					indexA < m_vInput.size(),
					"Input index = [" << indexA << "] is out of range (max index = [" << static_cast<uint32_t>(m_vInput.size() - 1) << "])",
					ErrorType::OutOfBound
				);
				OV_ERROR_UNLESS_KRF(
					indexB < m_vInput.size(),
					"Input index = [" << indexB << "] is out of range (max index = [" << static_cast<uint32_t>(m_vInput.size() - 1) << "])",
					ErrorType::OutOfBound
				);
				
				CString nameA;				
				CIdentifier identifierA;
				CString nameB;				
				CIdentifier identifierB;
				
				getInputName(indexA, nameA);
				getInputIdentifier(indexA, identifierA);
				getInputName(indexB, nameB);
				getInputIdentifier(indexB, identifierB);
								
				auto itA = m_vInput.begin()+indexA;
				auto itB = m_vInput.begin()+indexB;
				// swap Input
				std::iter_swap(itA,itB);
				// update associated maps
				m_vInputNameToIndex[nameA] = indexB;
				m_vInputNameToIndex[nameB] = indexA;
				m_vInputIdentifierToIndex[identifierA] = indexB;
				m_vInputIdentifierToIndex[identifierB] = indexA;				
				
				return true;
			}
			
			virtual bool swapOutputs(uint32_t indexA, uint32_t indexB)
			{
				OV_ERROR_UNLESS_KRF(
					indexA < m_vOutput.size(),
					"Input index = [" << indexA << "] is out of range (max index = [" << static_cast<uint32_t>(m_vOutput.size() - 1) << "])",
					ErrorType::OutOfBound
				);
				OV_ERROR_UNLESS_KRF(
					indexB < m_vOutput.size(),
					"Input index = [" << indexB << "] is out of range (max index = [" << static_cast<uint32_t>(m_vOutput.size() - 1) << "])",
					ErrorType::OutOfBound
				);
				
				CString nameA;				
				CIdentifier identifierA;
				CString nameB;				
				CIdentifier identifierB;
				
				getOutputName(indexA, nameA);
				getOutputIdentifier(indexA, identifierA);
				getOutputName(indexB, nameB);
				getOutputIdentifier(indexB, identifierB);
								
				auto itA = m_vOutput.begin()+indexA;
				auto itB = m_vOutput.begin()+indexB;
				// swap Input
				std::iter_swap(itA,itB);
				// update associated maps
				m_vOutputNameToIndex[nameA] = indexB;
				m_vOutputNameToIndex[nameB] = indexA;
				m_vOutputIdentifierToIndex[identifierA] = indexB;
				m_vOutputIdentifierToIndex[identifierB] = indexA;				
				
				return true;
			}
			
			virtual void notifySettingChange(BoxEventMessageType eType, int32_t i32FirstIndex = -1, int32_t i32SecondIndex = -1)
			{
				if( m_bIsObserverNotificationActive)
				{
					BoxEventMessage l_oEvent;
					l_oEvent.m_eType = eType;
					l_oEvent.m_i32FirstIndex = i32FirstIndex;
					l_oEvent.m_i32SecondIndex = i32SecondIndex;

					this->setChanged();
					this->notifyObservers(&l_oEvent);
				}
			}

			virtual bool hasModifiableSettings(void)const
			{
				for (const auto& setting : m_vSetting)
				{
					if (setting.m_bMod)
					{
						return true;
					}
				}
				return false;
			}

			virtual uint32_t* getModifiableSettings(uint32_t& rCount)const
			{
				uint32_t* l_pReturn = NULL;
				rCount = static_cast<uint32_t>(m_vModifiableSettingIndexes.size());

				return l_pReturn;

			}
			
			virtual bool updateSettingIdentifier(const uint32_t ui32SettingIndex, const CIdentifier& suggestedIdentifier)
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);
				
				OpenViBE::CIdentifier oldIdentifier;
				getSettingIdentifier(ui32SettingIndex,oldIdentifier);
						
				OpenViBE::CIdentifier newIdentifier = (suggestedIdentifier != OV_UndefinedIdentifier)?
				            suggestedIdentifier:
				            combineIdentifierWithString(m_vSetting[ui32SettingIndex].m_oTypeIdentifier,m_vSetting[ui32SettingIndex].m_sName);
				
				if (oldIdentifier != newIdentifier)
				{									
					// identifier key update is necessary
					auto it = m_vSettingIdentifierToIndex.find(newIdentifier);
					OV_ERROR_UNLESS_KRF(
						it == m_vSettingIdentifierToIndex.end(),
						"Conflict in settings identifiers. A setting with the same name and type exists.",
						ErrorType::ResourceNotFound
					);
					m_vSetting[ui32SettingIndex].m_oIdentifier = newIdentifier;
					m_vSettingIdentifierToIndex[newIdentifier] = ui32SettingIndex;
					// remove the old identifier key
					auto itOld = m_vSettingIdentifierToIndex.find(oldIdentifier);
					if (itOld != m_vSettingIdentifierToIndex.end())
					{
						m_vSettingIdentifierToIndex.erase(itOld);
					}					
				}
				return true;
				
			}
			
			virtual bool updateInputIdentifier(const uint32_t ui32InputIndex, const CIdentifier& suggestedIdentifier)
			{
				OV_ERROR_UNLESS_KRF(
					ui32InputIndex < m_vInput.size(),
					"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vInput.size() - 1) << "])",
					ErrorType::OutOfBound
				);
				
				OpenViBE::CIdentifier oldIdentifier;
				getInputIdentifier(ui32InputIndex,oldIdentifier);
				
				OpenViBE::CIdentifier newIdentifier = (suggestedIdentifier != OV_UndefinedIdentifier)?
				            suggestedIdentifier:
				            combineIdentifierWithString(m_vInput[ui32InputIndex].m_oTypeIdentifier,m_vInput[ui32InputIndex].m_sName);
				
				if (oldIdentifier != newIdentifier)
				{									
					// identifier key update is necessary
					auto it = m_vInputIdentifierToIndex.find(newIdentifier);
					OV_ERROR_UNLESS_KRF(
						it == m_vInputIdentifierToIndex.end(),
						"Conflict in Inputs identifiers. A Input with the same name and type exists.",
						ErrorType::ResourceNotFound
					);
					m_vInput[ui32InputIndex].m_oIdentifier = newIdentifier;
					m_vInputIdentifierToIndex[newIdentifier] = ui32InputIndex;
					// remove the old identifier key
					auto itOld = m_vInputIdentifierToIndex.find(oldIdentifier);
					if (itOld != m_vInputIdentifierToIndex.end())
					{
						m_vInputIdentifierToIndex.erase(itOld);
					}					
				}
				return true;
				
			}
			
			virtual bool updateOutputIdentifier(const uint32_t ui32OutputIndex, const CIdentifier& suggestedIdentifier)
			{
				OV_ERROR_UNLESS_KRF(
					ui32OutputIndex < m_vOutput.size(),
					"Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << static_cast<uint32_t>(m_vOutput.size() - 1) << "])",
					ErrorType::OutOfBound
				);
				
				OpenViBE::CIdentifier oldIdentifier;
				getOutputIdentifier(ui32OutputIndex,oldIdentifier);
						
				OpenViBE::CIdentifier newIdentifier = (suggestedIdentifier != OV_UndefinedIdentifier)?
				            suggestedIdentifier:
				            combineIdentifierWithString(m_vOutput[ui32OutputIndex].m_oTypeIdentifier,m_vOutput[ui32OutputIndex].m_sName);
				
				if (oldIdentifier != newIdentifier)
				{									
					// identifier key update is necessary
					auto it = m_vOutputIdentifierToIndex.find(newIdentifier);
					OV_ERROR_UNLESS_KRF(
						it == m_vOutputIdentifierToIndex.end(),
						"Conflict in Outputs identifiers. A Output with the same name and type exists.",
						ErrorType::ResourceNotFound
					);
					m_vOutput[ui32OutputIndex].m_oIdentifier = newIdentifier;
					m_vOutputIdentifierToIndex[newIdentifier] = ui32OutputIndex;
					// remove the old identifier key
					auto itOld = m_vOutputIdentifierToIndex.find(oldIdentifier);
					if (itOld != m_vOutputIdentifierToIndex.end())
					{
						m_vOutputIdentifierToIndex.erase(itOld);
					}					
				}
				return true;
				
			}


//*/

			virtual bool acceptVisitor(OpenViBE::IObjectVisitor& rObjectVisitor)
			{
				CObjectVisitorContext l_oObjectVisitorContext(this->getKernelContext());
				return rObjectVisitor.processBegin(l_oObjectVisitorContext, *this) && rObjectVisitor.processEnd(l_oObjectVisitorContext, *this);
			}							

		protected:

			virtual void clearBox(void)
			{
				m_pBoxAlgorithmDescriptor=NULL;
				m_oAlgorithmClassIdentifier=OV_UndefinedIdentifier;
				m_sName="";
				m_vInput.clear();
				m_vOutput.clear();
				m_vSetting.clear();
				m_vInputIdentifierToIndex.clear();
				m_vInputNameToIndex.clear();
				m_vOutputIdentifierToIndex.clear();
				m_vOutputNameToIndex.clear();
				m_vSettingIdentifierToIndex.clear();
				m_vSettingNameToIndex.clear();
				
				this->removeAllAttributes();
			}

			virtual void enableNotification(void)
			{
				m_bIsNotificationActive=true;
			}

			virtual void disableNotification(void)
			{
				m_bIsNotificationActive=false;
			}

			virtual void notify(const OpenViBE::Kernel::EBoxModification eBoxModificationType, const OpenViBE::uint32 ui32Index)
			{
				if(m_pBoxListener && !m_bIsNotifyingDescriptor && m_bIsNotificationActive)
				{
					CBoxListenerContext l_oContext(this->getKernelContext(), *this, ui32Index);
					m_bIsNotifyingDescriptor=true;
					m_pBoxListener->process(l_oContext, eBoxModificationType);
					m_bIsNotifyingDescriptor=false;
				}
			}

			virtual void notify(const OpenViBE::Kernel::EBoxModification eBoxModificationType)
			{
				this->notify(eBoxModificationType, 0xffffffff);
			}

		protected:

			class CInput
			{
			public:
				CInput(void) { }
				CInput(const CInput& i)
					: m_sName(i.m_sName)
					, m_oTypeIdentifier(i.m_oTypeIdentifier)
					,m_oIdentifier(i.m_oIdentifier)
					,m_bMissing(i.m_bMissing) { }
				CInput(const OpenViBE::CString& name, 
						const OpenViBE::CIdentifier& idType, 
						const OpenViBE::CIdentifier& id) 
					: m_sName(name)
					, m_oTypeIdentifier(idType)
					, m_oIdentifier(id)
					,m_bMissing(false) { }

				OpenViBE::CString m_sName;
				OpenViBE::CIdentifier m_oTypeIdentifier;
				OpenViBE::CIdentifier m_oIdentifier;
				bool m_bMissing;
			};

			class COutput
			{
			public:
				COutput(void) { }
				COutput(const COutput& o)
					:m_sName(o.m_sName)
					,m_oTypeIdentifier(o.m_oTypeIdentifier)
					,m_oIdentifier(o.m_oIdentifier)
					,m_bMissing(false) { }
				COutput(const OpenViBE::CString& name, 
						const OpenViBE::CIdentifier& idType, 
						const OpenViBE::CIdentifier& id)
					: m_sName(name)
					, m_oTypeIdentifier(idType)
					, m_oIdentifier(id)
					,m_bMissing(false) { }

				OpenViBE::CString m_sName;
				OpenViBE::CIdentifier m_oTypeIdentifier;
				OpenViBE::CIdentifier m_oIdentifier;
				bool m_bMissing;
			};

			class CSetting
			{
			public:
				CSetting(void) { }
				CSetting(const CSetting& s)
					:m_sName(s.m_sName)
					,m_oTypeIdentifier(s.m_oTypeIdentifier)
					,m_sDefaultValue(s.m_sDefaultValue)
					,m_sValue(s.m_sValue)
					,m_oIdentifier(s.m_oIdentifier)
					,m_bMod(s.m_bMod)
					,m_bMissing(false) { }
				OpenViBE::CString m_sName;
				OpenViBE::CIdentifier m_oTypeIdentifier;
				OpenViBE::CString m_sDefaultValue;
				OpenViBE::CString m_sValue;				
				OpenViBE::CIdentifier m_oIdentifier;
				bool m_bMod;
				bool m_bMissing;
			};

			_IsDerivedFromClass_Final_(TAttributable< TKernelObject <T> >, OVK_ClassId_Kernel_Scenario_Box)

		protected:			
			
			OpenViBE::CString getUnusedName(const std::map<OpenViBE::CString, uint32_t> &nameToIndex, const OpenViBE::CString& suggestedName) const
			{
				uint32_t idx = 1;
				OpenViBE::CString  newName;								
				auto it = nameToIndex.find(suggestedName);
				do
				{
					newName = suggestedName;
					if (it != nameToIndex.end())
					{
						newName += "(" + OpenViBE::CString(std::to_string(idx).c_str())+")";
						it = nameToIndex.find(newName);
						idx++;
					}
				}
				while(it != nameToIndex.end());				
				return newName;
			}
		
			OpenViBE::Kernel::IScenario* m_pOwnerScenario;
			const OpenViBE::Plugins::IBoxAlgorithmDesc* m_pBoxAlgorithmDescriptor;
			OpenViBE::Plugins::IBoxListener* m_pBoxListener;
			bool m_bIsNotifyingDescriptor;
			bool m_bIsNotificationActive;
			bool m_bIsObserverNotificationActive;			

			OpenViBE::CIdentifier m_oIdentifier;
			OpenViBE::CIdentifier m_oAlgorithmClassIdentifier;
			OpenViBE::CString m_sName;

			std::vector<CInput> m_vInput;
			std::vector<COutput> m_vOutput;
			std::vector<CSetting> m_vSetting;
			
			std::map<CIdentifier, uint32_t> m_vInputIdentifierToIndex;
			std::map<OpenViBE::CString, uint32_t> m_vInputNameToIndex;
			
			std::map<CIdentifier, uint32_t> m_vOutputIdentifierToIndex;
			std::map<OpenViBE::CString, uint32_t> m_vOutputNameToIndex;
			
			std::map<CIdentifier, uint32_t> m_vSettingIdentifierToIndex;
			std::map<OpenViBE::CString, uint32_t> m_vSettingNameToIndex;
			
			//to avoid having to recheck every setting every time
			//careful to update at each setting modification
			std::vector<uint32_t> m_vModifiableSettingIndexes;

			std::vector<CIdentifier> m_vSupportInputType;
			std::vector<CIdentifier> m_vSupportOutputType;
		};
	}
}

#endif // __OpenViBEKernel_Kernel_Scenario_CBox_H__
