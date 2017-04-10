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

#include <vector>
#include <string>
#include <iostream>

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

		virtual OpenViBE::boolean addInput(
			const OpenViBE::CString& sName,
			const OpenViBE::CIdentifier& rTypeIdentifier){return true;}

		virtual OpenViBE::boolean addOutput(
			const OpenViBE::CString& sName,
			const OpenViBE::CIdentifier& rTypeIdentifier){return true;}

		virtual OpenViBE::boolean addSetting(
			const OpenViBE::CString& sName,
			const OpenViBE::CIdentifier& rTypeIdentifier,
			const OpenViBE::CString& sDefaultValue,
			const OpenViBE::boolean bModifiable = false){return true;}

		virtual OpenViBE::boolean addFlag(
			const OpenViBE::Kernel::EBoxFlag eBoxFlag){return true;}
		virtual OpenViBE::boolean addFlag(const OpenViBE::CString& cStringFlag)
		{
			return false;
		}
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

			virtual OpenViBE::boolean setIdentifier(const OpenViBE::CIdentifier& rIdentifier)
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

			virtual OpenViBE::boolean setName(const OpenViBE::CString& sName)
			{
				m_sName=sName;

				this->notify(BoxModification_NameChanged);

				return true;
			}

			virtual OpenViBE::boolean setAlgorithmClassIdentifier(const OpenViBE::CIdentifier& rAlgorithmClassIdentifier)
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

			virtual OpenViBE::boolean initializeFromAlgorithmClassIdentifier(const OpenViBE::CIdentifier& rAlgorithmClassIdentifier)
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
				m_bIsObserverNotificationActive = false;

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

				for(i=0; i<rExistingBox.getOutputCount(); i++)
				{
					CIdentifier l_oType;
					CString l_sName;
					rExistingBox.getOutputType(i, l_oType);
					rExistingBox.getOutputName(i, l_sName);
					addOutput(l_sName, l_oType);
				}

				for(i=0; i<rExistingBox.getSettingCount(); i++)
				{
					CIdentifier l_oType;
					CString l_sName;
					CString l_sValue;
					CString l_sDefaultValue;
					boolean l_bModifiability;

					rExistingBox.getSettingType(i, l_oType);
					rExistingBox.getSettingName(i, l_sName);
					rExistingBox.getSettingValue(i, l_sValue);
					rExistingBox.getSettingDefaultValue(i, l_sDefaultValue);
					rExistingBox.getSettingMod(i, l_bModifiability);
					addSetting(l_sName, l_oType, l_sDefaultValue, -1 ,l_bModifiability);
					setSettingValue(i, l_sValue);
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

			virtual OpenViBE::boolean addInput(const OpenViBE::CString& sName, const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				OV_ERROR_UNLESS_KRF(
					this->getTypeManager().isStream(rTypeIdentifier),
					"While adding input '" << sName << "' to box '" << getName() << "', unknown stream type identifier " << rTypeIdentifier.toString(),
					ErrorType::BadArgument
				);

				CInput i;
				i.m_sName=sName;
				i.m_oTypeIdentifier=rTypeIdentifier;
				m_vInput.push_back(i);

				this->notify(BoxModification_InputAdded, m_vInput.size()-1);

				return true;
			}

			virtual OpenViBE::boolean removeInput(const OpenViBE::uint32 ui32InputIndex)
			{
				OV_ERROR_UNLESS_KRF(
					ui32InputIndex < m_vInput.size(),
					"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vInput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				CIdentifier l_oIdentifier;

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
				std::vector < CIdentifier > l_vLinksToRemove;
				std::vector < std::pair < std::pair < uint64, uint32 >, std::pair < uint64, uint32 > > > l_vLink;
				while((l_oIdentifier=m_pOwnerScenario->getNextLinkIdentifierToBox(l_oIdentifier, m_oIdentifier))!=OV_UndefinedIdentifier)
				{
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
						l_vLinksToRemove.push_back(l_oIdentifier);
					}
				}
				for (size_t i = 0; i < l_vLinksToRemove.size(); i++)
				{
					if (m_pOwnerScenario->isLink(l_vLinksToRemove[i]))
					{
						m_pOwnerScenario->disconnect(l_vLinksToRemove[i]);
					}
				}

				// This reorganizes the parent's scenario links if this box is not actually a scenario itself
				if (m_oIdentifier != OV_UndefinedIdentifier)
				{
					std::vector < std::pair < uint32, std::pair < uint64, uint32 > > > l_vScenarioLink;
					for(uint32 scenarioInputIndex = 0; scenarioInputIndex < m_pOwnerScenario->getInputCount(); scenarioInputIndex++)
					{
						CIdentifier l_oBoxIdentifier;
						uint32 l_ui32BoxConnectorIndex = uint32(-1);
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
					for(size_t i = 0; i < l_vScenarioLink.size(); i++)
					{
						m_pOwnerScenario->setScenarioInputLink(
						    l_vScenarioLink[i].first,
						    l_vScenarioLink[i].second.first,
						    l_vScenarioLink[i].second.second-1);
					}
				}

				// Erases actual input
				m_vInput.erase(m_vInput.begin()+ui32InputIndex);

				// Reconnects box links
				for (size_t i = 0; i < l_vLink.size(); i++)
				{
					m_pOwnerScenario->connect(
					            l_oIdentifier,
					            l_vLink[i].first.first,
					            l_vLink[i].first.second,
					            l_vLink[i].second.first,
					            l_vLink[i].second.second-1,
					            OV_UndefinedIdentifier);
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
				OV_ERROR_UNLESS_KRF(
					ui32InputIndex < m_vInput.size(),
					"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vInput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rTypeIdentifier=m_vInput[ui32InputIndex].m_oTypeIdentifier;
				return true;
			}

			virtual OpenViBE::boolean getInputName(const OpenViBE::uint32 ui32InputIndex, OpenViBE::CString& rName) const
			{
				OV_ERROR_UNLESS_KRF(
					ui32InputIndex < m_vInput.size(),
					"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vInput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rName=m_vInput[ui32InputIndex].m_sName;
				return true;
			}

			virtual OpenViBE::boolean setInputType(const OpenViBE::uint32 ui32InputIndex, const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				OV_ERROR_UNLESS_KRF(
					this->getTypeManager().isStream(rTypeIdentifier),
					"While changing box '" << getName() << "' input type, unknown stream type identifier " << rTypeIdentifier.toString(),
					ErrorType::BadArgument
				);

				OV_ERROR_UNLESS_KRF(
					ui32InputIndex < m_vInput.size(),
					"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vInput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				m_vInput[ui32InputIndex].m_oTypeIdentifier=rTypeIdentifier;

				this->notify(BoxModification_InputTypeChanged, ui32InputIndex);

				return true;
			}

			virtual OpenViBE::boolean setInputName(const OpenViBE::uint32 ui32InputIndex, const OpenViBE::CString& rName)
			{
				OV_ERROR_UNLESS_KRF(
					ui32InputIndex < m_vInput.size(),
					"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vInput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				m_vInput[ui32InputIndex].m_sName=rName;

				this->notify(BoxModification_InputNameChanged, ui32InputIndex);

				return true;
			}

			//___________________________________________________________________//
			//                                                                   //

			virtual OpenViBE::boolean addOutput(const OpenViBE::CString& sName, const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				OV_ERROR_UNLESS_KRF(
					this->getTypeManager().isStream(rTypeIdentifier),
					"While adding output '" << sName << "' to box '" << getName() << "', unknown stream type identifier " << rTypeIdentifier.toString(),
					ErrorType::BadArgument
				);

				COutput o;
				o.m_sName=sName;
				o.m_oTypeIdentifier=rTypeIdentifier;
				m_vOutput.push_back(o);

				this->notify(BoxModification_OutputAdded, m_vOutput.size()-1);

				return true;
			}

			virtual OpenViBE::boolean removeOutput(const OpenViBE::uint32 ui32OutputIndex)
			{
				OV_ERROR_UNLESS_KRF(
					ui32OutputIndex < m_vOutput.size(),
					"Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vOutput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				CIdentifier l_oIdentifier;

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
				std::vector < CIdentifier > l_vLinksToRemove;
				std::vector < std::pair < std::pair < uint64, uint32 >, std::pair < uint64, uint32 > > > l_vLink;
				while((l_oIdentifier=m_pOwnerScenario->getNextLinkIdentifierFromBox(l_oIdentifier, m_oIdentifier))!=OV_UndefinedIdentifier)
				{
					ILink* l_pLink=m_pOwnerScenario->getLinkDetails(l_oIdentifier);
					if(l_pLink->getSourceBoxOutputIndex()>ui32OutputIndex)
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
						l_vLinksToRemove.push_back(l_oIdentifier);
					}
				}
				for(size_t i=0; i<l_vLinksToRemove.size(); i++)
				{
					if(m_pOwnerScenario->isLink(l_vLinksToRemove[i]))
					{
						m_pOwnerScenario->disconnect(l_vLinksToRemove[i]);
					}
				}

				// This reorganizes the parent's scenario links if this box is not actually a scenario
				if (m_oIdentifier != OV_UndefinedIdentifier)
				{
					std::vector < std::pair < uint32, std::pair < uint64, uint32 > > > l_vScenarioLink;
					for(uint32 scenarioOutputIndex = 0; scenarioOutputIndex < m_pOwnerScenario->getOutputCount(); scenarioOutputIndex++)
					{
						CIdentifier l_oBoxIdentier;
						uint32 l_ui32BoxConnectorIndex = uint32(-1);
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
					for(size_t i=0; i<l_vScenarioLink.size(); i++)
					{
						m_pOwnerScenario->setScenarioOutputLink(
						            l_vScenarioLink[i].first,
						            l_vScenarioLink[i].second.first,
						            l_vScenarioLink[i].second.second-1);
					}
				}


				// Erases actual output
				m_vOutput.erase(m_vOutput.begin()+ui32OutputIndex);

				// Reconnects box links
				for(size_t i=0; i<l_vLink.size(); i++)
				{
					m_pOwnerScenario->connect(
								l_oIdentifier,
								l_vLink[i].first.first,
								l_vLink[i].first.second-1,
								l_vLink[i].second.first,
								l_vLink[i].second.second,
								OV_UndefinedIdentifier);
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
				OV_ERROR_UNLESS_KRF(
					ui32OutputIndex < m_vOutput.size(),
					"Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vOutput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rTypeIdentifier=m_vOutput[ui32OutputIndex].m_oTypeIdentifier;
				return true;
			}

			virtual OpenViBE::boolean getOutputName(const OpenViBE::uint32 ui32OutputIndex, OpenViBE::CString& rName) const
			{
				OV_ERROR_UNLESS_KRF(
					ui32OutputIndex < m_vOutput.size(),
					"Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vOutput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rName=m_vOutput[ui32OutputIndex].m_sName;
				return true;
			}

			virtual OpenViBE::boolean setOutputType(const OpenViBE::uint32 ui32OutputIndex, const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				OV_ERROR_UNLESS_KRF(
					this->getTypeManager().isStream(rTypeIdentifier),
					"While changing box '" << getName() << "' output type, unknown stream type identifier " << rTypeIdentifier.toString(),
					ErrorType::BadArgument
				);

				OV_ERROR_UNLESS_KRF(
					ui32OutputIndex < m_vOutput.size(),
					"Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vOutput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				m_vOutput[ui32OutputIndex].m_oTypeIdentifier=rTypeIdentifier;

				this->notify(BoxModification_OutputTypeChanged, ui32OutputIndex);

				return true;
			}

			virtual OpenViBE::boolean setOutputName(const OpenViBE::uint32 ui32OutputIndex, const OpenViBE::CString& rName)
			{
				OV_ERROR_UNLESS_KRF(
					ui32OutputIndex < m_vOutput.size(),
					"Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vOutput.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				m_vOutput[ui32OutputIndex].m_sName=rName;

				this->notify(BoxModification_OutputNameChanged, ui32OutputIndex);

				return true;
			}



			virtual OpenViBE::boolean addInputSupport(const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				m_vSupportInputType.push_back(rTypeIdentifier);
				return true;
			}

			virtual OpenViBE::boolean hasInputSupport(const OpenViBE::CIdentifier& rTypeIdentifier) const
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

			virtual OpenViBE::boolean addOutputSupport(const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				m_vSupportOutputType.push_back(rTypeIdentifier);
				return true;
			}

			virtual OpenViBE::boolean hasOutputSupport(const OpenViBE::CIdentifier& rTypeIdentifier) const
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

			virtual OpenViBE::boolean setSupportTypeFromAlgorithmIdentifier(const CIdentifier &rTypeIdentifier)
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

			virtual OpenViBE::boolean addSetting(
				const OpenViBE::CString& sName,
				const OpenViBE::CIdentifier& rTypeIdentifier,
				const OpenViBE::CString& sDefaultValue,
				const int32 i32Index,
				const OpenViBE::boolean bModifiability)
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

							// Find if the default value string actually is an identifier, otherwise just keep the zero index name as default.
							CIdentifier l_oIdentifier;
							l_oIdentifier.fromString(sDefaultValue);

							// Finally, if it is an identifier, then a name should be found
							// from the type manager ! Otherwise l_sValue is left to the default.
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
				s.m_bMod=bModifiability;

				int32 l_i32Index = i32Index;

				OV_ERROR_UNLESS_KRF(
					i32Index <= static_cast<int32>(m_vSetting.size()),
					"Tried to push '" << sName << "' to slot " << i32Index << " with the array size being " << static_cast<int32>(m_vSetting.size()),
					ErrorType::OutOfBound
				);

				int32 l_i32InsertLocation;

				if(i32Index < 0 || i32Index == static_cast<int32>(m_vSetting.size()))
				{
					m_vSetting.push_back(s);
					l_i32InsertLocation = (static_cast<int32>(m_vSetting.size()))-1;
				}
				else
				{
					typename std::vector<CSetting>::iterator l_it = m_vSetting.begin();
					l_it += l_i32Index;
					m_vSetting.insert(l_it, s);
					l_i32InsertLocation = i32Index;
				}

				//if this setting is modifiable, keep its index
				if(bModifiability)
				{
					m_vModifiableSettingIndexes.push_back(l_i32Index);
				}

				this->getLogManager() << LogLevel_Debug << "Pushed '" << m_vSetting[l_i32InsertLocation].m_sName << "' : '" << m_vSetting[l_i32InsertLocation].m_sValue << "' to slot " << l_i32InsertLocation << " with the array size now " << static_cast<int32>(m_vSetting.size()) << "\n";

				this->notify(BoxModification_SettingAdded, l_i32InsertLocation);
				this->notifySettingChange(SettingAdd, l_i32InsertLocation);

				return true;
			}

			virtual OpenViBE::boolean removeSetting(const OpenViBE::uint32 ui32SettingIndex)
			{
				uint32 i=0;
				typename std::vector<CSetting>::iterator it=m_vSetting.begin();
				for(i=0; i<ui32SettingIndex && it!=m_vSetting.end(); i++)
				{
					++it;
				}

				OV_ERROR_UNLESS_KRF(
					it != m_vSetting.end(),
					"No setting found at index " << ui32SettingIndex,
					ErrorType::ResourceNotFound
				);

				it=m_vSetting.erase(it);

				//update the modifiable setting indexes
				std::vector<uint32>::iterator it2=m_vModifiableSettingIndexes.begin();
				for (i=0; i<m_vModifiableSettingIndexes.size(); i++)
				{
					if(m_vModifiableSettingIndexes[i]==ui32SettingIndex)
					{
						m_vModifiableSettingIndexes.erase(it2);
					}
					else if(m_vModifiableSettingIndexes[i]>ui32SettingIndex)
					{
						m_vModifiableSettingIndexes[i]-=1;
					}
					++it2;
				}

				this->notify(BoxModification_SettingRemoved, ui32SettingIndex);
				this->notifySettingChange(SettingDelete, ui32SettingIndex);

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
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rTypeIdentifier=m_vSetting[ui32SettingIndex].m_oTypeIdentifier;
				return true;
			}

			virtual OpenViBE::boolean getSettingName(const OpenViBE::uint32 ui32SettingIndex, OpenViBE::CString& rName) const
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rName=m_vSetting[ui32SettingIndex].m_sName;
				return true;
			}

			virtual OpenViBE::boolean getSettingDefaultValue(const OpenViBE::uint32 ui32SettingIndex, OpenViBE::CString& rDefaultValue) const
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rDefaultValue=m_vSetting[ui32SettingIndex].m_sDefaultValue;
				return true;
			}

			virtual OpenViBE::boolean getSettingValue(const OpenViBE::uint32 ui32SettingIndex, OpenViBE::CString& rValue) const
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rValue = m_vSetting[ui32SettingIndex].m_sValue;
				return true;
			}

			virtual OpenViBE::boolean setSettingType(const OpenViBE::uint32 ui32SettingIndex, const OpenViBE::CIdentifier& rTypeIdentifier)
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vSetting.size() - 1) << "])",
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

			virtual OpenViBE::boolean setSettingName(const OpenViBE::uint32 ui32SettingIndex, const OpenViBE::CString& rName)
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				m_vSetting[ui32SettingIndex].m_sName=rName;

				this->notify(BoxModification_SettingNameChanged, ui32SettingIndex);
				this->notifySettingChange(SettingChange, ui32SettingIndex);

				return true;
			}

			virtual OpenViBE::boolean setSettingDefaultValue(const OpenViBE::uint32 ui32SettingIndex, const OpenViBE::CString& rDefaultValue)
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				m_vSetting[ui32SettingIndex].m_sDefaultValue=rDefaultValue;

				this->notify(BoxModification_SettingDefaultValueChanged, ui32SettingIndex);

				return true;
			}

			virtual OpenViBE::boolean setSettingValue(const OpenViBE::uint32 ui32SettingIndex, const OpenViBE::CString& rValue)
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				m_vSetting[ui32SettingIndex].m_sValue=rValue;

				this->notify(BoxModification_SettingValueChanged, ui32SettingIndex);
				this->notifySettingChange(SettingValueUpdate, ui32SettingIndex);

				return true;
			}

			virtual void notifySettingChange(BoxEventMessageType eType, int32 i32FirstIndex = -1, int32 i32SecondIndex = -1)
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

			//*
			virtual OpenViBE::boolean getSettingMod(
					const OpenViBE::uint32 ui32SettingIndex,
					OpenViBE::boolean& rValue) const
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				rValue=m_vSetting[ui32SettingIndex].m_bMod;
				return true;
			}


			virtual OpenViBE::boolean setSettingMod(
					const OpenViBE::uint32 ui32SettingIndex,
					const OpenViBE::boolean rValue)
			{
				OV_ERROR_UNLESS_KRF(
					ui32SettingIndex < m_vSetting.size(),
					"Setting index = [" << ui32SettingIndex << "] is out of range (max index = [" << static_cast<OpenViBE::uint32>(m_vSetting.size() - 1) << "])",
					ErrorType::OutOfBound
				);

				m_vSetting[ui32SettingIndex].m_bMod=rValue;

				//this->notify(BoxModification_SettingNameChanged, ui32SettingIndex);
				return true;
			}

			virtual OpenViBE::boolean hasModifiableSettings(void)const
			{
				uint32 i=0;
				boolean rValue = false;
				while((i<m_vSetting.size())&&(!rValue))
				{
					rValue = m_vSetting[i].m_bMod;
					i++;
				}
				return rValue;
			}

			virtual uint32* getModifiableSettings(uint32& rCount)const
			{
				uint32* l_pReturn = NULL;
				rCount = m_vModifiableSettingIndexes.size();

				return l_pReturn;

			}


//*/

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
					,m_sValue(s.m_sValue)
					,m_bMod(s.m_bMod){ }
				OpenViBE::CString m_sName;
				OpenViBE::CIdentifier m_oTypeIdentifier;
				OpenViBE::CString m_sDefaultValue;
				OpenViBE::CString m_sValue;
				OpenViBE::boolean m_bMod;
			};

			_IsDerivedFromClass_Final_(TAttributable< TKernelObject <T> >, OVK_ClassId_Kernel_Scenario_Box)

		protected:

			OpenViBE::Kernel::IScenario* m_pOwnerScenario;
			const OpenViBE::Plugins::IBoxAlgorithmDesc* m_pBoxAlgorithmDescriptor;
			OpenViBE::Plugins::IBoxListener* m_pBoxListener;
			OpenViBE::boolean m_bIsNotifyingDescriptor;
			OpenViBE::boolean m_bIsNotificationActive;
			OpenViBE::boolean m_bIsObserverNotificationActive;

			OpenViBE::CIdentifier m_oIdentifier;
			OpenViBE::CIdentifier m_oAlgorithmClassIdentifier;
			OpenViBE::CString m_sName;

			std::vector<CInput> m_vInput;
			std::vector<COutput> m_vOutput;
			std::vector<CSetting> m_vSetting;
			//to avoid having to recheck every setting every time
			//careful to update at each setting modification
			std::vector<OpenViBE::uint32> m_vModifiableSettingIndexes;

			std::vector<CIdentifier> m_vSupportInputType;
			std::vector<CIdentifier> m_vSupportOutputType;
		};
	}
}

#endif // __OpenViBEKernel_Kernel_Scenario_CBox_H__
