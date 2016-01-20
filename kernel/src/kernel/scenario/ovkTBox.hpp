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

#include "lepton/Lepton.h"
#include <vector>
#include <string>
#include <iostream>

namespace OpenViBE
{
	namespace Kernel
	{
		template<class T>
		class TBox : public TAttributable < TKernelObject < T > >
		{
		public:

			TBox(const OpenViBE::Kernel::IKernelContext& rKernelContext)
			    :TAttributable< TKernelObject <T> >(rKernelContext)
			    ,m_pOwnerScenario(NULL)
			    ,m_pBoxAlgorithmDescriptor(NULL)
			    ,m_pBoxListener(NULL)
			    ,m_bIsNotifyingDescriptor(false)
			    ,m_bIsNotificationActive(true)
			    ,m_oIdentifier(OV_UndefinedIdentifier)
			    ,m_oAlgorithmClassIdentifier(OV_UndefinedIdentifier)
			    ,m_oProcessingUnitIdentifier(OV_UndefinedIdentifier)
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

			virtual OpenViBE::CIdentifier getProcessingUnitIdentifier(void) const
			{
				return m_oProcessingUnitIdentifier;
			}

			virtual OpenViBE::boolean setIdentifier(const OpenViBE::CIdentifier& rIdentifier)
			{
				if(m_oIdentifier!=OV_UndefinedIdentifier)
				{
					return false;
				}
				if(rIdentifier==OV_UndefinedIdentifier)
				{
					return false;
				}
				m_oIdentifier=rIdentifier;

				this->notify(BoxModification_IdentifierChanged);

				return true;
			}

			virtual OpenViBE::boolean setName(const OpenViBE::CString& sName)
			{
				m_sName=sName;

				this->notify(BoxModification_NameChanged);

				return true;
			}

			virtual OpenViBE::boolean setAlgorithmClassIdentifier(const OpenViBE::CIdentifier& rAlgorithmClassIdentifier)
			{
				m_oAlgorithmClassIdentifier=rAlgorithmClassIdentifier;

				if(rAlgorithmClassIdentifier != OVP_ClassId_BoxAlgorithm_Metabox && !this->getKernelContext().getPluginManager().canCreatePluginObject(rAlgorithmClassIdentifier))
				{
					this->getLogManager() << LogLevel_Warning << "Box algorithm descriptor not found " << rAlgorithmClassIdentifier << "\n";
					return false;
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

				this->notify(BoxModification_AlgorithmClassIdentifierChanged);

				return true;
			}

			virtual OpenViBE::boolean setProcessingUnitIdentifier(const OpenViBE::CIdentifier& rProcessingUnitIdentifier)
			{
				m_oProcessingUnitIdentifier=rProcessingUnitIdentifier;

				this->notify(BoxModification_ProcessingUnitChanged);

				return true;
			}

			virtual OpenViBE::boolean initializeFromAlgorithmClassIdentifier(const OpenViBE::CIdentifier& rAlgorithmClassIdentifier)
			{
				this->disableNotification();

				const Plugins::IBoxAlgorithmDesc* l_pBoxAlgorithmDesc=dynamic_cast<const Plugins::IBoxAlgorithmDesc*>(this->getKernelContext().getPluginManager().getPluginObjectDescCreating(rAlgorithmClassIdentifier));
				if(!l_pBoxAlgorithmDesc)
				{
					this->getLogManager() << LogLevel_Warning << "Algorithm descriptor not found " << rAlgorithmClassIdentifier << "\n";

					this->enableNotification();

					return false;
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

			OpenViBE::boolean initializeFromBoxAlgorithmDesc(const OpenViBE::Plugins::IBoxAlgorithmDesc& rBoxAlgorithmDesc)
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

			virtual OpenViBE::boolean initializeFromExistingBox(const OpenViBE::Kernel::IBox& rExistingBox)
			{
				uint32 i;

				this->disableNotification();

				this->clearBox();
				this->setName(rExistingBox.getName());
				this->setAlgorithmClassIdentifier(rExistingBox.getAlgorithmClassIdentifier());

				for(i=0; i<rExistingBox.getInputCount(); i++)
				{
					CIdentifier l_oType;
					CString l_sName;
					rExistingBox.getInputType(i, l_oType);
					rExistingBox.getInputName(i, l_sName);
					addInput(l_sName, l_oType);
				}

				for(i=0; i<rExistingBox.getMessageInputCount(); i++)
				{
					CString l_sName;
					rExistingBox.getMessageInputName(i, l_sName);
					addMessageInput(l_sName);
				}

				for(i=0; i<rExistingBox.getOutputCount(); i++)
				{
					CIdentifier l_oType;
					CString l_sName;
					rExistingBox.getOutputType(i, l_oType);
					rExistingBox.getOutputName(i, l_sName);
					addOutput(l_sName, l_oType);
				}

				for(i=0; i<rExistingBox.getMessageOutputCount(); i++)
				{
					CString l_sName;
					rExistingBox.getMessageOutputName(i, l_sName);
					addMessageOutput(l_sName);
				}

				for(i=0; i<rExistingBox.getSettingCount(); i++)
				{
					CIdentifier l_oType;
					CString l_sName;
					CString l_sValue;
					CString l_sDefaultValue;
					rExistingBox.getSettingType(i, l_oType);
					rExistingBox.getSettingName(i, l_sName);
					rExistingBox.getSettingValue(i, l_sValue);
					rExistingBox.getSettingDefaultValue(i, l_sDefaultValue);
					addSetting(l_sName, l_oType, l_sDefaultValue);
					setSettingValue(i, l_sValue);
				}

				CIdentifier l_oIdentifier=rExistingBox.getNextAttributeIdentifier(OV_UndefinedIdentifier);
				while(l_oIdentifier!=OV_UndefinedIdentifier)
				{
					this->addAttribute(l_oIdentifier, rExistingBox.getAttributeValue(l_oIdentifier));
					l_oIdentifier=rExistingBox.getNextAttributeIdentifier(l_oIdentifier);
				}

				this->enableNotification();

				this->notify(BoxModification_Initialized);

				return true;
			}

			virtual OpenViBE::boolean addInput(const OpenViBE::CString& sName, const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				CInput i;
				i.m_sName=sName;
				i.m_oTypeIdentifier=rTypeIdentifier;
				m_vInput.push_back(i);

				uint32 l_ui32Index=m_vInput.size()-1;

				this->notify(BoxModification_InputAdded, l_ui32Index);

				if(l_ui32Index < m_vInput.size())
				{
					CIdentifier l_oNewTypeIdentifier=m_vInput[l_ui32Index].m_oTypeIdentifier;
					if(!this->getTypeManager().isStream(l_oNewTypeIdentifier))
					{
						if(l_oNewTypeIdentifier==OV_UndefinedIdentifier)
						{
							this->getLogManager() << LogLevel_Warning << "Box '" << getName() << "' input '" << sName << "' maps to OV_UndefinedIdentifier. Please configure the box.\n";
						}
						else
						{
							this->getLogManager() << LogLevel_Warning << "While adding input '" << sName << "' to box '" << getName() << "', unknown stream type identifier " << l_oNewTypeIdentifier << "\n";
						}
					}
				}

				return true;
			}

			virtual OpenViBE::boolean removeInput(const OpenViBE::uint32 ui32InputIndex)
			{
				CIdentifier l_oIdentifier;
				size_t i;

				if(ui32InputIndex >= m_vInput.size())
				{
					return false;
				}

				while((l_oIdentifier=m_pOwnerScenario->getNextLinkIdentifierToBoxInput(l_oIdentifier, m_oIdentifier, ui32InputIndex))!=OV_UndefinedIdentifier)
				{
					m_pOwnerScenario->disconnect(l_oIdentifier);
				}

				// $$$
				// The way the links are removed here
				// is not correct because they are all
				// collected and then all removed. In case
				// the box listener callback on box removal,
				// the nextcoming links would potentially be
				// invalid
				// $$$
				// This comment should be fixed as isLink is called before diconnecting // YARE 2014 09 11
				std::vector < CIdentifier > l_vLinksToRemove;
				std::vector < std::pair < std::pair < uint64, uint32 >, std::pair < uint64, uint32 > > > l_vLink;
				while((l_oIdentifier=m_pOwnerScenario->getNextLinkIdentifierToBox(l_oIdentifier, m_oIdentifier))!=OV_UndefinedIdentifier)
				{
					ILink* l_pLink=m_pOwnerScenario->getLinkDetails(l_oIdentifier);
					if(l_pLink->getTargetBoxInputIndex()>ui32InputIndex)
					{
						std::pair < std::pair < uint64, uint32 >, std::pair < uint64, uint32 > > l;
						l.first.first=l_pLink->getSourceBoxIdentifier().toUInteger();
						l.first.second=l_pLink->getSourceBoxOutputIndex();
						l.second.first=l_pLink->getTargetBoxIdentifier().toUInteger();
						l.second.second=l_pLink->getTargetBoxInputIndex();
						l_vLink.push_back(l);
						l_vLinksToRemove.push_back(l_oIdentifier);
					}
				}
				for(i=0; i<l_vLinksToRemove.size(); i++)
				{
					if(m_pOwnerScenario->isLink(l_vLinksToRemove[i]))
					{
						m_pOwnerScenario->disconnect(l_vLinksToRemove[i]);
					}
				}

				// This reorganizes scenario links
				std::vector < std::pair < uint32, std::pair < uint64, uint32 > > > l_vScenarioLink;
				for(i=0; i<m_pOwnerScenario->getInputCount(); i++)
				{
					std::pair < uint32, std::pair < uint64, uint32 > > l;
					CIdentifier l_oBoxIdentier;
					uint32 l_ui32BoxConnectorIndex=uint32(-1);
					m_pOwnerScenario->getScenarioInputLink(i, l_oBoxIdentier, l_ui32BoxConnectorIndex);
					if(l_oBoxIdentier == m_oIdentifier)
					{
						if(l_ui32BoxConnectorIndex > ui32InputIndex)
						{
							l.first = i;
							l.second.first = l_oBoxIdentier.toUInteger();
							l.second.second = l_ui32BoxConnectorIndex;
							l_vScenarioLink.push_back(l);
						}
						if(l_ui32BoxConnectorIndex >= ui32InputIndex)
						{
							m_pOwnerScenario->removeScenarioInputLink(i, l_oBoxIdentier, l_ui32BoxConnectorIndex);
						}
					}
				}

				// Erases actual input
				m_vInput.erase(m_vInput.begin()+ui32InputIndex);

				// Reconnects box links
				for(i=0; i<l_vLink.size(); i++)
				{
					m_pOwnerScenario->connect(
					            l_oIdentifier,
					            l_vLink[i].first.first,
					            l_vLink[i].first.second,
					            l_vLink[i].second.first,
					            l_vLink[i].second.second-1,
					            OV_UndefinedIdentifier);
				}

				// Reconnects scenario links
				for(i=0; i<l_vScenarioLink.size(); i++)
				{
					m_pOwnerScenario->setScenarioInputLink(
					    l_vScenarioLink[i].first,
					    l_vScenarioLink[i].second.first,
					    l_vScenarioLink[i].second.second-1);
				}

				this->notify(BoxModification_InputRemoved, ui32InputIndex);

				return true;
			}

			virtual OpenViBE::uint32 getInputCount(void) const
			{
				return m_vInput.size();
			}

			virtual OpenViBE::boolean getInputType( const OpenViBE::uint32 ui32InputIndex, OpenViBE::CIdentifier& rTypeIdentifier) const
			{
				if(ui32InputIndex>=m_vInput.size())
				{
					return false;
				}
				rTypeIdentifier=m_vInput[ui32InputIndex].m_oTypeIdentifier;
				return true;
			}

			virtual OpenViBE::boolean getInputName(const OpenViBE::uint32 ui32InputIndex, OpenViBE::CString& rName) const
			{
				if(ui32InputIndex>=m_vInput.size())
				{
					return false;
				}
				rName=m_vInput[ui32InputIndex].m_sName;
				return true;
			}

			virtual OpenViBE::boolean setInputType(const OpenViBE::uint32 ui32InputIndex, const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				if(!this->getTypeManager().isStream(rTypeIdentifier))
				{
					this->getLogManager() << LogLevel_Warning << "While changing box '" << getName() << "' input type, unknown stream type identifier " << rTypeIdentifier << "\n";
				}

				if(ui32InputIndex>=m_vInput.size())
				{
					return false;
				}
				m_vInput[ui32InputIndex].m_oTypeIdentifier=rTypeIdentifier;

				this->notify(BoxModification_InputTypeChanged, ui32InputIndex);

				return true;
			}

			virtual OpenViBE::boolean setInputName(const OpenViBE::uint32 ui32InputIndex, const OpenViBE::CString& rName)
			{
				if(ui32InputIndex>=m_vInput.size())
				{
					return false;
				}
				m_vInput[ui32InputIndex].m_sName=rName;

				this->notify(BoxModification_InputNameChanged, ui32InputIndex);

				return true;
			}

			//messages input
			virtual OpenViBE::boolean addMessageInput(const OpenViBE::CString& sName)
			{
				//this->getLogManager() << LogLevel_Fatal << "adding message input named "<< sName << "for box "<< m_sName << "\n";
				CMessageInput l_oMessageInput;
				l_oMessageInput.m_sName = sName;
				m_vMessageInput.push_back(l_oMessageInput);

				this->notify(BoxModification_MessageInputAdded, m_vMessageInput.size()-1);

				return true;
			}

			virtual OpenViBE::boolean removeMessageInput(const OpenViBE::uint32 ui32InputIndex)
			{
				CIdentifier l_oIdentifier;
				size_t i;

				if(ui32InputIndex >= m_vMessageInput.size())
				{
					return false;
				}

				while((l_oIdentifier=m_pOwnerScenario->getNextMessageLinkIdentifierToBoxInput(l_oIdentifier, m_oIdentifier, ui32InputIndex))!=OV_UndefinedIdentifier)
				{
					m_pOwnerScenario->disconnectMessage(l_oIdentifier);
				}

				// $$$
				// The way the links are removed here
				// is not correct because they are all
				// collected and then all removed. In case
				// the box listener callback on box removal,
				// the nextcoming links would potentially be
				// invalid
				std::vector < CIdentifier > l_vMessageLinksToRemove;
				std::vector < std::pair < std::pair < uint64, uint32 >, std::pair < uint64, uint32 > > > l_vMessageLink;
				while((l_oIdentifier=m_pOwnerScenario->getNextMessageLinkIdentifierToBox(l_oIdentifier, m_oIdentifier))!=OV_UndefinedIdentifier)
				{
					ILink* l_pLink=m_pOwnerScenario->getMessageLinkDetails(l_oIdentifier);
					if(l_pLink->getTargetBoxInputIndex()>ui32InputIndex)
					{
						std::pair < std::pair < uint64, uint32 >, std::pair < uint64, uint32 > > l;
						l.first.first=l_pLink->getSourceBoxIdentifier().toUInteger();
						l.first.second=l_pLink->getSourceBoxOutputIndex();
						l.second.first=l_pLink->getTargetBoxIdentifier().toUInteger();
						l.second.second=l_pLink->getTargetBoxInputIndex();
						l_vMessageLink.push_back(l);
						l_vMessageLinksToRemove.push_back(l_oIdentifier);
					}
				}

				for(i=0; i<l_vMessageLinksToRemove.size(); i++)
				{
					m_pOwnerScenario->disconnectMessage(l_vMessageLinksToRemove[i]);
				}

				m_vMessageInput.erase(m_vMessageInput.begin()+ui32InputIndex);

				for(i=0; i<l_vMessageLink.size(); i++)
				{
					m_pOwnerScenario->connectMessage(
					            l_oIdentifier,
					            l_vMessageLink[i].first.first,
					            l_vMessageLink[i].first.second,
					            l_vMessageLink[i].second.first,
					            l_vMessageLink[i].second.second-1,
					            OV_UndefinedIdentifier);
				}

				this->notify(BoxModification_MessageInputRemoved, ui32InputIndex);

				return true;
			}

			virtual OpenViBE::uint32 getMessageInputCount(void) const
			{
				//this->getLogManager() << LogLevel_Fatal << "box "<< m_sName << " has " << (uint64)m_vMessageInput.size() << " message input\n";
				return m_vMessageInput.size();
			}

			virtual OpenViBE::boolean getMessageInputName(const OpenViBE::uint32 ui32InputIndex, OpenViBE::CString& rName) const
			{
				if(ui32InputIndex>=m_vMessageInput.size())
				{
					return false;
				}
				rName=m_vMessageInput[ui32InputIndex].m_sName;
				return true;
			}

			virtual OpenViBE::boolean setMessageInputName(const OpenViBE::uint32 ui32InputIndex, const OpenViBE::CString& rName)
			{
				if(ui32InputIndex>=m_vMessageInput.size())
				{
					return false;
				}
				m_vMessageInput[ui32InputIndex].m_sName=rName;

				this->notify(BoxModification_MessageInputNameChanged, ui32InputIndex);

				return true;
			}

			//message output
			virtual OpenViBE::boolean addMessageOutput(const OpenViBE::CString& sName)
			{
				//this->getLogManager() << LogLevel_Fatal << "adding message Output named "<< sName << "for box "<< m_sName << "\n";
				CMessageOutput l_oMessageOutput;
				l_oMessageOutput.m_sName = sName;
				m_vMessageOutput.push_back(l_oMessageOutput);

				this->notify(BoxModification_MessageOutputAdded, m_vMessageOutput.size()-1);

				return true;
			}

			virtual OpenViBE::boolean removeMessageOutput(const OpenViBE::uint32 ui32OutputIndex)
			{
				CIdentifier l_oIdentifier;
				size_t i;

				if(ui32OutputIndex >= m_vMessageOutput.size())
				{
					return false;
				}

				while((l_oIdentifier=m_pOwnerScenario->getNextMessageLinkIdentifierFromBoxOutput(l_oIdentifier, m_oIdentifier, ui32OutputIndex))!=OV_UndefinedIdentifier)
				{
					m_pOwnerScenario->disconnectMessage(l_oIdentifier);
				}

				// $$$
				// The way the links are removed here
				// is not correct because they are all
				// collected and then all removed. In case
				// the box listener callback on box removal,
				// the nextcoming links would potentially be
				// invalid
				std::vector < CIdentifier > l_vMessageLinksToRemove;
				std::vector < std::pair < std::pair < uint64, uint32 >, std::pair < uint64, uint32 > > > l_vMessageLink;
				while((l_oIdentifier=m_pOwnerScenario->getNextMessageLinkIdentifierFromBox(l_oIdentifier, m_oIdentifier))!=OV_UndefinedIdentifier)
				{
					ILink* l_pLink=m_pOwnerScenario->getMessageLinkDetails(l_oIdentifier);
					if(l_pLink->getSourceBoxOutputIndex()>ui32OutputIndex)
					{
						std::pair < std::pair < uint64, uint32 >, std::pair < uint64, uint32 > > l;
						l.first.first=l_pLink->getSourceBoxIdentifier().toUInteger();
						l.first.second=l_pLink->getSourceBoxOutputIndex();
						l.second.first=l_pLink->getTargetBoxIdentifier().toUInteger();
						l.second.second=l_pLink->getTargetBoxInputIndex();
						l_vMessageLink.push_back(l);
						l_vMessageLinksToRemove.push_back(l_oIdentifier);
					}
				}

				for(i=0; i<l_vMessageLinksToRemove.size(); i++)
				{
					m_pOwnerScenario->disconnectMessage(l_vMessageLinksToRemove[i]);
				}

				m_vMessageOutput.erase(m_vMessageOutput.begin()+ui32OutputIndex);

				for(i=0; i<l_vMessageLink.size(); i++)
				{
					m_pOwnerScenario->connectMessage(
					            l_oIdentifier,
					            l_vMessageLink[i].first.first,
					            l_vMessageLink[i].first.second-1,
					            l_vMessageLink[i].second.first,
					            l_vMessageLink[i].second.second,
					            OV_UndefinedIdentifier);
				}

				this->notify(BoxModification_MessageOutputRemoved, ui32OutputIndex);

				return true;
			}

			virtual OpenViBE::uint32 getMessageOutputCount(void) const
			{
				//this->getLogManager() << LogLevel_Fatal << "box "<< m_sName << " has " << (uint64)m_vMessageOutput.size() << " message Output\n";
				return m_vMessageOutput.size();
			}
			virtual OpenViBE::boolean getMessageOutputName(const OpenViBE::uint32 ui32InputIndex, OpenViBE::CString& rName) const
			{
				if(ui32InputIndex>=m_vMessageOutput.size())
				{
					return false;
				}
				rName=m_vMessageOutput[ui32InputIndex].m_sName;
				return true;
			}

			virtual OpenViBE::boolean setMessageOutputName(const OpenViBE::uint32 ui32InputIndex, const OpenViBE::CString& rName)
			{
				if(ui32InputIndex>=m_vMessageOutput.size())
				{
					return false;
				}
				m_vMessageOutput[ui32InputIndex].m_sName=rName;

				this->notify(BoxModification_MessageOutputNameChanged, ui32InputIndex);

				return true;
			}

			//

			virtual OpenViBE::boolean addOutput(const OpenViBE::CString& sName, const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				COutput o;
				o.m_sName=sName;
				o.m_oTypeIdentifier=rTypeIdentifier;
				m_vOutput.push_back(o);

				uint32 l_ui32Index=m_vOutput.size()-1;

				this->notify(BoxModification_OutputAdded, l_ui32Index);

				if(l_ui32Index < m_vOutput.size())
				{
					CIdentifier l_oNewTypeIdentifier=m_vOutput[l_ui32Index].m_oTypeIdentifier;
					if(!this->getTypeManager().isStream(l_oNewTypeIdentifier))
					{
						if(l_oNewTypeIdentifier==OV_UndefinedIdentifier)
						{
							this->getLogManager() << LogLevel_Warning << "Box '" << getName() << "' output '" << sName << "' maps to OV_UndefinedIdentifier. Please configure the box.\n";
						}
						else
						{
							this->getLogManager() << LogLevel_Warning << "While adding output '" << sName << "' to box '" << getName() << "', unknown stream type identifier " << l_oNewTypeIdentifier << "\n";
						}
					}
				}


				return true;
			}

			virtual OpenViBE::boolean removeOutput(const OpenViBE::uint32 ui32OutputIndex)
			{
				CIdentifier l_oIdentifier;
				size_t i;

				if(ui32OutputIndex >= m_vOutput.size())
				{
					return false;
				}

				while((l_oIdentifier=m_pOwnerScenario->getNextLinkIdentifierFromBoxOutput(l_oIdentifier, m_oIdentifier, ui32OutputIndex))!=OV_UndefinedIdentifier)
				{
					m_pOwnerScenario->disconnect(l_oIdentifier);
				}

				// $$$
				// The way the links are removed here
				// is not correct because they are all
				// collected and then all removed. In case
				// the box listener callback on box removal,
				// the nextcoming links would potentially be
				// invalid
				// $$$
				// This comment should be fixed as isLink is called before diconnecting // YARE 2014 09 11
				std::vector < CIdentifier > l_vLinksToRemove;
				std::vector < std::pair < std::pair < uint64, uint32 >, std::pair < uint64, uint32 > > > l_vLink;
				while((l_oIdentifier=m_pOwnerScenario->getNextLinkIdentifierFromBox(l_oIdentifier, m_oIdentifier))!=OV_UndefinedIdentifier)
				{
					ILink* l_pLink=m_pOwnerScenario->getLinkDetails(l_oIdentifier);
					if(l_pLink->getSourceBoxOutputIndex()>ui32OutputIndex)
					{
						std::pair < std::pair < uint64, uint32 >, std::pair < uint64, uint32 > > l;
						l.first.first=l_pLink->getSourceBoxIdentifier().toUInteger();
						l.first.second=l_pLink->getSourceBoxOutputIndex();
						l.second.first=l_pLink->getTargetBoxIdentifier().toUInteger();
						l.second.second=l_pLink->getTargetBoxInputIndex();
						l_vLink.push_back(l);
						l_vLinksToRemove.push_back(l_oIdentifier);
					}
				}
				for(i=0; i<l_vLinksToRemove.size(); i++)
				{
					if(m_pOwnerScenario->isLink(l_vLinksToRemove[i]))
					{
						m_pOwnerScenario->disconnect(l_vLinksToRemove[i]);
					}
				}

				// This reorganizes scenario links
				std::vector < std::pair < uint32, std::pair < uint64, uint32 > > > l_vScenarioLink;
				for(i=0; i<m_pOwnerScenario->getOutputCount(); i++)
				{
					std::pair < uint32, std::pair < uint64, uint32 > > l;
					CIdentifier l_oBoxIdentier;
					uint32 l_ui32BoxConnectorIndex=uint32(-1);
					m_pOwnerScenario->getScenarioOutputLink(i, l_oBoxIdentier, l_ui32BoxConnectorIndex);
					if(l_oBoxIdentier == m_oIdentifier)
					{
						if(l_ui32BoxConnectorIndex > ui32OutputIndex)
						{
							l.first = i;
							l.second.first = l_oBoxIdentier.toUInteger();
							l.second.second = l_ui32BoxConnectorIndex;
							l_vScenarioLink.push_back(l);
						}
						if(l_ui32BoxConnectorIndex >= ui32OutputIndex)
						{
							m_pOwnerScenario->removeScenarioOutputLink(i, l_oBoxIdentier, l_ui32BoxConnectorIndex);
						}
					}
				}

				// Erases actual output
				m_vOutput.erase(m_vOutput.begin()+ui32OutputIndex);

				// Reconnects box links
				for(i=0; i<l_vLink.size(); i++)
				{
					m_pOwnerScenario->connect(
					            l_oIdentifier,
					            l_vLink[i].first.first,
					            l_vLink[i].first.second-1,
					            l_vLink[i].second.first,
					            l_vLink[i].second.second,
					            OV_UndefinedIdentifier);
				}

				// Reconnects scenario links
				for(i=0; i<l_vScenarioLink.size(); i++)
				{
					m_pOwnerScenario->setScenarioOutputLink(
					    l_vScenarioLink[i].first,
					    l_vScenarioLink[i].second.first,
					    l_vScenarioLink[i].second.second-1);
				}

				this->notify(BoxModification_OutputRemoved, ui32OutputIndex);

				return true;
			}

			virtual OpenViBE::uint32 getOutputCount(void) const
			{
				return m_vOutput.size();
			}

			virtual OpenViBE::boolean getOutputType(const OpenViBE::uint32 ui32OutputIndex, OpenViBE::CIdentifier& rTypeIdentifier) const
			{
				if(ui32OutputIndex>=m_vOutput.size())
				{
					return false;
				}
				rTypeIdentifier=m_vOutput[ui32OutputIndex].m_oTypeIdentifier;
				return true;
			}

			virtual OpenViBE::boolean getOutputName(const OpenViBE::uint32 ui32OutputIndex, OpenViBE::CString& rName) const
			{
				if(ui32OutputIndex>=m_vOutput.size())
				{
					return false;
				}
				rName=m_vOutput[ui32OutputIndex].m_sName;
				return true;
			}

			virtual OpenViBE::boolean setOutputType(const OpenViBE::uint32 ui32OutputIndex, const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				if(!this->getTypeManager().isStream(rTypeIdentifier))
				{
					this->getLogManager() << LogLevel_Warning << "While changing box '" << getName() << "' output type, unknown stream type identifier " << rTypeIdentifier << "\n";
				}

				if(ui32OutputIndex>=m_vOutput.size())
				{
					return false;
				}
				m_vOutput[ui32OutputIndex].m_oTypeIdentifier=rTypeIdentifier;

				this->notify(BoxModification_OutputTypeChanged, ui32OutputIndex);

				return true;
			}

			virtual OpenViBE::boolean setOutputName(const OpenViBE::uint32 ui32OutputIndex, const OpenViBE::CString& rName)
			{
				if(ui32OutputIndex>=m_vOutput.size())
				{
					return false;
				}
				m_vOutput[ui32OutputIndex].m_sName=rName;

				this->notify(BoxModification_OutputNameChanged, ui32OutputIndex);

				return true;
			}

			virtual OpenViBE::boolean addSetting(const OpenViBE::CString& sName, const OpenViBE::CIdentifier& rTypeIdentifier, const OpenViBE::CString& sDefaultValue)
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
							uint64 l_ui64Value=0;
							this->getTypeManager().getEnumerationEntry(rTypeIdentifier, 0, l_sValue, l_ui64Value);

							// Find if the default value string actually is an identifier
							CIdentifier l_oIdentifier;
							l_oIdentifier.fromString(sDefaultValue);

							// Finally, if it is an identifier, then a name should be found
							// from the type manager !
							CString l_sCandidateValue=this->getTypeManager().getEnumerationEntryNameFromValue(rTypeIdentifier, l_oIdentifier.toUInteger());
							if(l_sCandidateValue!=CString(""))
							{
								l_sValue=l_sCandidateValue;
							}
						}
					}
				}

				CSetting s;
				s.m_sName=sName;
				s.m_oTypeIdentifier=rTypeIdentifier;
				s.m_sDefaultValue=l_sValue;
				s.m_sValue=l_sValue;

				m_vSetting.push_back(s);

				this->notify(BoxModification_SettingAdded, m_vSetting.size()-1);

				return true;
			}

			virtual OpenViBE::boolean removeSetting(const OpenViBE::uint32 ui32SettingIndex)
			{
				uint32 i=0;
				typename std::vector<CSetting>::iterator it=m_vSetting.begin();
				for(i=0; i<ui32SettingIndex && it!=m_vSetting.end(); i++)
				{
					it++;
				}
				if(it==m_vSetting.end())
				{
					return false;
				}
				it=m_vSetting.erase(it);

				this->notify(BoxModification_SettingRemoved, ui32SettingIndex);

				return true;
			}

			virtual OpenViBE::uint32 getSettingCount(void) const
			{
				return m_vSetting.size();
			}

			virtual OpenViBE::boolean hasSettingWithName(const OpenViBE::CString& rName) const
			{
				for (uint32 l_ui32SettingIndex = 0; l_ui32SettingIndex < m_vSetting.size(); l_ui32SettingIndex++)
				{
					if (m_vSetting[l_ui32SettingIndex].m_sName == rName)
					{
						return true;
					}
				}

				return false;
			}

			virtual OpenViBE::int32 getSettingIndex(const OpenViBE::CString& rName) const
			{
				for (uint32 l_ui32SettingIndex = 0; l_ui32SettingIndex < m_vSetting.size(); l_ui32SettingIndex++)
				{
					if (m_vSetting[l_ui32SettingIndex].m_sName == rName)
					{
						return static_cast<int32>(l_ui32SettingIndex);
					}
				}

				return -1;
			}

			virtual OpenViBE::boolean getSettingType(const OpenViBE::uint32 ui32SettingIndex, OpenViBE::CIdentifier& rTypeIdentifier) const
			{
				if(ui32SettingIndex>=m_vSetting.size())
				{
					return false;
				}
				rTypeIdentifier=m_vSetting[ui32SettingIndex].m_oTypeIdentifier;
				return true;
			}

			virtual OpenViBE::boolean getSettingName(const OpenViBE::uint32 ui32SettingIndex, OpenViBE::CString& rName) const
			{
				if(ui32SettingIndex>=m_vSetting.size())
				{
					return false;
				}
				rName=m_vSetting[ui32SettingIndex].m_sName;
				return true;
			}

			virtual OpenViBE::boolean getSettingDefaultValue(const OpenViBE::uint32 ui32SettingIndex, OpenViBE::CString& rDefaultValue) const
			{
				if(ui32SettingIndex>=m_vSetting.size())
				{
					return false;
				}
				rDefaultValue=m_vSetting[ui32SettingIndex].m_sDefaultValue;
				return true;
			}

			virtual OpenViBE::boolean getSettingValue(const OpenViBE::uint32 ui32SettingIndex, OpenViBE::CString& rValue) const
			{
				// This getSettingValue
				if(ui32SettingIndex>=m_vSetting.size())
				{
					return false;
				}
				rValue = m_vSetting[ui32SettingIndex].m_sValue;
				return true;
			}

			virtual OpenViBE::boolean evaluateSettingValue(const OpenViBE::uint32 ui32SettingIndex, OpenViBE::CString& rValue) const
			{
				CIdentifier l_oTypeIdentifier;
				if(!getSettingType(ui32SettingIndex, l_oTypeIdentifier))
				{
					return false;
				}
				// If the token is a numeric value, it may be an arithmetic operation
				if(l_oTypeIdentifier == OV_TypeId_Float || l_oTypeIdentifier == OV_TypeId_Integer)
				{
					// parse and expression with no variables or functions
					double l_dEvaluatedExp = 0;
					try
					{
						l_dEvaluatedExp = Lepton::Parser::parse(std::string(rValue).c_str()).evaluate();
						if(l_oTypeIdentifier == OV_TypeId_Float)
						{
							char l_sTmpValue[64];
							sprintf(l_sTmpValue, "%.17e", l_dEvaluatedExp);
							rValue = l_sTmpValue;
						}
						else
						{
							char l_sTmpValue[64];
							sprintf(l_sTmpValue, "%d", (int) l_dEvaluatedExp);
							rValue = l_sTmpValue;
						}
					}
					catch(...)
					{
						return false;
					}
				}
				return true;
			}

			virtual OpenViBE::boolean setSettingType(const OpenViBE::uint32 ui32SettingIndex, const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				if(ui32SettingIndex>=m_vSetting.size())
				{
					return false;
				}
				m_vSetting[ui32SettingIndex].m_oTypeIdentifier=rTypeIdentifier;

				this->notify(BoxModification_SettingTypeChanged, ui32SettingIndex);

				return true;
			}

			virtual OpenViBE::boolean setSettingName(const OpenViBE::uint32 ui32SettingIndex, const OpenViBE::CString& rName)
			{
				if(ui32SettingIndex>=m_vSetting.size())
				{
					return false;
				}
				m_vSetting[ui32SettingIndex].m_sName=rName;

				this->notify(BoxModification_SettingNameChanged, ui32SettingIndex);

				return true;
			}

			virtual OpenViBE::boolean setSettingDefaultValue(const OpenViBE::uint32 ui32SettingIndex, const OpenViBE::CString& rDefaultValue)
			{
				if(ui32SettingIndex>=m_vSetting.size())
				{
					return false;
				}
				m_vSetting[ui32SettingIndex].m_sDefaultValue=rDefaultValue;

				this->notify(BoxModification_SettingDefaultValueChanged, ui32SettingIndex);

				return true;
			}

			virtual OpenViBE::boolean setSettingValue(const OpenViBE::uint32 ui32SettingIndex, const OpenViBE::CString& rValue)
			{
				if(ui32SettingIndex>=m_vSetting.size())
				{
					return false;
				}
				m_vSetting[ui32SettingIndex].m_sValue=rValue;

				this->notify(BoxModification_SettingValueChanged, ui32SettingIndex);

				return true;
			}

			virtual OpenViBE::boolean acceptVisitor(OpenViBE::IObjectVisitor& rObjectVisitor)
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
				m_vMessageInput.clear();
				m_vMessageOutput.clear();
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
					:m_sName(i.m_sName)
					,m_oTypeIdentifier(i.m_oTypeIdentifier) { }
				OpenViBE::CString m_sName;
				OpenViBE::CIdentifier m_oTypeIdentifier;
			};

			class COutput
			{
			public:
				COutput(void) { }
				COutput(const COutput& o)
					:m_sName(o.m_sName)
					,m_oTypeIdentifier(o.m_oTypeIdentifier) { }
				OpenViBE::CString m_sName;
				OpenViBE::CIdentifier m_oTypeIdentifier;
			};

			class CSetting
			{
			public:
				CSetting(void) { }
				CSetting(const CSetting& s)
					:m_sName(s.m_sName)
					,m_oTypeIdentifier(s.m_oTypeIdentifier)
					,m_sDefaultValue(s.m_sDefaultValue)
					,m_sValue(s.m_sValue) { }
				OpenViBE::CString m_sName;
				OpenViBE::CIdentifier m_oTypeIdentifier;
				OpenViBE::CString m_sDefaultValue;
				OpenViBE::CString m_sValue;
			};

			class CMessageInput
			{
			public:
				CMessageInput(void) { }
				CMessageInput(const CMessageInput& mi)
					:m_sName(mi.m_sName) { }
				OpenViBE::CString m_sName;

			};

			class CMessageOutput
			{
			public:
				CMessageOutput(void) { }
				CMessageOutput(const CMessageOutput& mi)
					:m_sName(mi.m_sName) { }
				OpenViBE::CString m_sName;

			};

			_IsDerivedFromClass_Final_(TAttributable< TKernelObject <T> >, OVK_ClassId_Kernel_Scenario_Box)

		protected:

			OpenViBE::Kernel::IScenario* m_pOwnerScenario;
			const OpenViBE::Plugins::IBoxAlgorithmDesc* m_pBoxAlgorithmDescriptor;
			OpenViBE::Plugins::IBoxListener* m_pBoxListener;
			OpenViBE::boolean m_bIsNotifyingDescriptor;
			OpenViBE::boolean m_bIsNotificationActive;

			OpenViBE::CIdentifier m_oIdentifier;
			OpenViBE::CIdentifier m_oAlgorithmClassIdentifier;
			OpenViBE::CIdentifier m_oProcessingUnitIdentifier;
			OpenViBE::CString m_sName;

			std::vector<CInput> m_vInput;
			std::vector<COutput> m_vOutput;
			std::vector<CSetting> m_vSetting;

			//only the name of the in/output are stored for message socket
			std::vector<CMessageInput> m_vMessageInput;
			std::vector<CMessageOutput> m_vMessageOutput;

		};
	}
}

#endif // __OpenViBEKernel_Kernel_Scenario_CBox_H__
