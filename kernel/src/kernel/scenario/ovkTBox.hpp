#pragma once

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
#include <memory>

namespace
{
	std::map<OpenViBE::Kernel::BoxInterfacorType, OpenViBE::CString> g_InterfacorTypeToName = {
		{ OpenViBE::Kernel::BoxInterfacorType::Setting, "Setting" },
		{ OpenViBE::Kernel::BoxInterfacorType::Input, "Input" },
		{ OpenViBE::Kernel::BoxInterfacorType::Output, "Output" }
	};
	//This class is used to set up the restriction of a stream type for input and output. Each box comes with a
	// decriptor that call functions describe in IBoxProto for intialize the CBox object.
	// This implementation is derived from CBoxProto, to benefit from
	// the implementation of the stream restriction mecanism but neutralizes all other initialization function.
	class CBoxProtoRestriction final : public OpenViBE::Kernel::CBoxProto
	{
	public:

		CBoxProtoRestriction(const OpenViBE::Kernel::IKernelContext& ctx, OpenViBE::Kernel::IBox& box): CBoxProto(ctx, box) {}

		bool addInput(const OpenViBE::CString& /*name*/, const OpenViBE::CIdentifier& /*typeID*/,
					  const OpenViBE::CIdentifier& /*identifier*/ = OV_UndefinedIdentifier, const bool /*notify*/ = true) override 
		{
			return true;
		}

		bool addOutput(const OpenViBE::CString& /*name*/, const OpenViBE::CIdentifier& /*typeID*/,
					   const OpenViBE::CIdentifier& /*identifier*/ = OV_UndefinedIdentifier, const bool /*notify*/ = true) override
		{
			return true;
		}

		bool addSetting(const OpenViBE::CString& /*name*/, const OpenViBE::CIdentifier& /*typeID*/, const OpenViBE::CString& /*defaultValue*/,
						const bool /*modifiable*/ = false, const OpenViBE::CIdentifier& /*identifier*/ = OV_UndefinedIdentifier, const bool /*notify*/ = true) override { return true; }

		bool addFlag(const OpenViBE::Kernel::EBoxFlag /*boxFlag*/) override { return true; }
		bool addFlag(const OpenViBE::CIdentifier& /*identifier*/) override { return true; }
	};

	class CInterfacor
	{
	public:
		CInterfacor() { }

		CInterfacor(const CInterfacor& other)
			: m_sName(other.m_sName), m_oTypeIdentifier(other.m_oTypeIdentifier), m_oIdentifier(other.m_oIdentifier), m_bDeprecated(other.m_bDeprecated) { }

		CInterfacor(const OpenViBE::CString& name, const OpenViBE::CIdentifier& idType, const OpenViBE::CIdentifier& id)
			: m_sName(name), m_oTypeIdentifier(idType), m_oIdentifier(id) {}

		OpenViBE::CString m_sName;
		OpenViBE::CIdentifier m_oTypeIdentifier = OV_UndefinedIdentifier;
		OpenViBE::CIdentifier m_oIdentifier     = OV_UndefinedIdentifier;
		bool m_bDeprecated                      = false;
	};

	class CInputOutput : public CInterfacor
	{
	public:
		CInputOutput() { }

		CInputOutput(const CInputOutput& i) : CInterfacor(i) { this->m_bDeprecated = false; }

		CInputOutput(const OpenViBE::CString& name, const OpenViBE::CIdentifier& idType, const OpenViBE::CIdentifier& id) : CInterfacor(name, idType, id) { }
	};

	class CSetting : public CInterfacor
	{
	public:
		CSetting() { }

		CSetting(const CSetting& s) : CInterfacor(s), m_sDefaultValue(s.m_sDefaultValue), m_sValue(s.m_sValue), m_bMod(s.m_bMod)
		{
			this->m_bDeprecated = false;
		}

		CSetting(const OpenViBE::CString& name, const OpenViBE::CIdentifier& idType, const OpenViBE::CIdentifier& id, const OpenViBE::CString& defaultValue,
				 const bool modifiable)
			: CInterfacor(name, idType, id), m_sDefaultValue(defaultValue), m_sValue(defaultValue), m_bMod(modifiable) { }

		OpenViBE::CString m_sDefaultValue;
		OpenViBE::CString m_sValue;
		bool m_bMod = false;
	};
} // namespace


namespace OpenViBE
{
	namespace Kernel
	{
		template <class T>
		class TBox : public TAttributable<TKernelObject<T>>
		{
		public:

			explicit TBox(const IKernelContext& ctx)
				: TAttributable<TKernelObject<T>>(ctx), m_oIdentifier(OV_UndefinedIdentifier), m_oAlgorithmClassIdentifier(OV_UndefinedIdentifier)
			{
				for (auto i : { Input, Output, Setting })
				{
					m_Interfacors[i]                 = std::vector<std::shared_ptr<CInterfacor>>();
					m_InterfacorIdentifierToIndex[i] = std::map<CIdentifier, uint32_t>();
					m_InterfacorNameToIndex[i]       = std::map<CString, uint32_t>();
				}
			}

			virtual ~TBox()
			{
				if (m_pBoxAlgorithmDescriptor && m_pBoxListener)
				{
					CBoxListenerContext l_oContext(this->getKernelContext(), *this, 0xffffffff);
					m_pBoxListener->uninitialize(l_oContext);
					m_pBoxAlgorithmDescriptor->releaseBoxListener(m_pBoxListener);
				}
			}

			virtual void setOwnerScenario(IScenario* pOwnerScenario) { m_pOwnerScenario = pOwnerScenario; }
			virtual CIdentifier getIdentifier() const { return m_oIdentifier; }
			virtual CString getName() const { return m_sName; }
			virtual CIdentifier getAlgorithmClassIdentifier() const { return m_oAlgorithmClassIdentifier; }

			virtual bool setIdentifier(const CIdentifier& identifier)
			{
				OV_ERROR_UNLESS_KRF(m_oIdentifier == OV_UndefinedIdentifier, "Trying to overwrite an already set indentifier", ErrorType::BadCall);
				OV_ERROR_UNLESS_KRF(identifier != OV_UndefinedIdentifier, "Trying to set an undefined identifier", ErrorType::BadArgument);

				m_oIdentifier = identifier;
				this->notify(BoxModification_IdentifierChanged);

				return true;
			}

			virtual bool setName(const CString& name)
			{
				m_sName = name;
				this->notify(BoxModification_NameChanged);
				return true;
			}

			virtual bool setAlgorithmClassIdentifier(const CIdentifier& algorithmClassID)
			{
				// We need to set the box algorithm identifier in any case. This is because OpenViBE should be able to load
				// a scenario with non-existing boxes and save it without modifying them.
				m_oAlgorithmClassIdentifier = algorithmClassID;

				if (!(algorithmClassID == OVP_ClassId_BoxAlgorithm_Metabox || this
																					   ->getKernelContext().getPluginManager().canCreatePluginObject(
																						   algorithmClassID)))
				{
					//					OV_WARNING_K("Box algorithm descriptor not found " << algorithmClassID.toString());
					return true;
				}

				if (m_pBoxAlgorithmDescriptor && m_pBoxListener)
				{
					CBoxListenerContext l_oContext(this->getKernelContext(), *this, 0xffffffff);
					m_pBoxListener->uninitialize(l_oContext);
					m_pBoxAlgorithmDescriptor->releaseBoxListener(m_pBoxListener);
				}

				const Plugins::IPluginObjectDesc* l_pPluginObjectDescriptor = this
																			  ->getKernelContext().getPluginManager().getPluginObjectDescCreating(
																				  algorithmClassID);
				m_pBoxAlgorithmDescriptor = dynamic_cast<const Plugins::IBoxAlgorithmDesc*>(l_pPluginObjectDescriptor);

				if (m_pBoxAlgorithmDescriptor)
				{
					m_pBoxListener = m_pBoxAlgorithmDescriptor->createBoxListener();
					if (m_pBoxListener)
					{
						CBoxListenerContext l_oContext(this->getKernelContext(), *this, 0xffffffff);
						m_pBoxListener->initialize(l_oContext);
					}
				}

				if (algorithmClassID != OVP_ClassId_BoxAlgorithm_Metabox)
				{
					//We use the neutralized version of CBoxProto to just initialize the stream restriction mecanism
					CBoxProtoRestriction oTempProto(this->getKernelContext(), *this);
					m_pBoxAlgorithmDescriptor->getBoxPrototype(oTempProto);
				}

				this->notify(BoxModification_AlgorithmClassIdentifierChanged);

				return true;
			}

			virtual bool initializeFromAlgorithmClassIdentifier(const CIdentifier& algorithmClassID)
			{
				if (!this->initializeFromAlgorithmClassIdentifierNoInit(algorithmClassID)) { return false; }

				this->notify(BoxModification_Initialized);
				this->notify(BoxModification_DefaultInitialized);
				return true;
			}

			virtual bool initializeFromAlgorithmClassIdentifierNoInit(const CIdentifier& algorithmClassID)
			{
				this->disableNotification();

				const Plugins::IBoxAlgorithmDesc* l_pBoxAlgorithmDesc = dynamic_cast<const Plugins::IBoxAlgorithmDesc*>(this
																														->getKernelContext().getPluginManager().
																														getPluginObjectDescCreating(
																															algorithmClassID));
				if (!l_pBoxAlgorithmDesc)
				{
					this->enableNotification();
					OV_ERROR_KRF("Algorithm descriptor not found " << algorithmClassID.toString(), ErrorType::ResourceNotFound);
				}

				this->clearBox();
				this->setName(l_pBoxAlgorithmDesc->getName());
				this->setAlgorithmClassIdentifier(algorithmClassID);

				CBoxProto l_oBoxProto(this->getKernelContext(), *this);
				l_pBoxAlgorithmDesc->getBoxPrototype(l_oBoxProto);

				if (this->hasAttribute(OV_AttributeId_Box_InitialPrototypeHashValue))
				{
					this->setAttributeValue(
						OV_AttributeId_Box_InitialPrototypeHashValue, this->getPluginManager().getPluginObjectHashValue(algorithmClassID).toString());
				}
				else
				{
					this->addAttribute(
						OV_AttributeId_Box_InitialPrototypeHashValue, this->getPluginManager().getPluginObjectHashValue(algorithmClassID).toString());
				}

				this->enableNotification();

				return true;
			}

			bool initializeFromBoxAlgorithmDesc(const Plugins::IBoxAlgorithmDesc& rBoxAlgorithmDesc)
			{
				this->clearBox();
				this->setName(rBoxAlgorithmDesc.getName());
				this->setAlgorithmClassIdentifier(rBoxAlgorithmDesc.getCreatedClassIdentifier());

				CBoxProto l_oBoxProto(this->getKernelContext(), *this);
				rBoxAlgorithmDesc.getBoxPrototype(l_oBoxProto);

				if (this->hasAttribute(OV_AttributeId_Box_InitialPrototypeHashValue))
				{
					this->setAttributeValue(
						OV_AttributeId_Box_InitialPrototypeHashValue, this->getPluginManager().getPluginObjectHashValue(rBoxAlgorithmDesc).toString());
				}
				else
				{
					this->addAttribute(
						OV_AttributeId_Box_InitialPrototypeHashValue, this->getPluginManager().getPluginObjectHashValue(rBoxAlgorithmDesc).toString());
				}

				this->enableNotification();

				this->notify(BoxModification_Initialized);
				this->notify(BoxModification_DefaultInitialized);
				return true;
			}

			virtual bool initializeFromExistingBox(const IBox& rExistingBox)
			{
				this->disableNotification();
				m_bIsObserverNotificationActive = false;

				this->clearBox();
				this->setName(rExistingBox.getName());
				this->setAlgorithmClassIdentifier(rExistingBox.getAlgorithmClassIdentifier());

				for (const auto interfacorType : { Input, Output, Setting })
				{
					for (uint32_t i = 0; i < rExistingBox.getInterfacorCountIncludingDeprecated(interfacorType); ++i)
					{
						CIdentifier identifier = OV_UndefinedIdentifier;
						CIdentifier type       = OV_UndefinedIdentifier;
						CString name;
						bool isDeprecated;
						rExistingBox.getInterfacorIdentifier(interfacorType, i, identifier);
						rExistingBox.getInterfacorType(interfacorType, i, type);
						rExistingBox.getInterfacorName(interfacorType, i, name);
						rExistingBox.getInterfacorDeprecatedStatus(interfacorType, i, isDeprecated);
						this->addInterfacor(interfacorType, name, type, identifier, true);
						this->setInterfacorDeprecatedStatus(interfacorType, i, isDeprecated);

						if (interfacorType == Setting)
						{
							CString value;
							CString defaultValue;
							bool isModifiable;
							rExistingBox.getSettingValue(i, value);
							rExistingBox.getSettingDefaultValue(i, defaultValue);
							rExistingBox.getSettingMod(i, isModifiable);
							this->setSettingDefaultValue(i, defaultValue);
							this->setSettingValue(i, value);
							this->setSettingMod(i, isModifiable);
							if (isModifiable) { m_vModifiableSettingIndexes.push_back(i); }
						}
					}
				}

				CIdentifier l_oIdentifier = rExistingBox.getNextAttributeIdentifier(OV_UndefinedIdentifier);
				while (l_oIdentifier != OV_UndefinedIdentifier)
				{
					this->addAttribute(l_oIdentifier, rExistingBox.getAttributeValue(l_oIdentifier));
					l_oIdentifier = rExistingBox.getNextAttributeIdentifier(l_oIdentifier);
				}

				CIdentifier l_oStreamTypeIdentifier = OV_UndefinedIdentifier;
				while ((l_oStreamTypeIdentifier = this->getKernelContext().getTypeManager().getNextTypeIdentifier(l_oStreamTypeIdentifier)) !=
					   OV_UndefinedIdentifier)
				{
					if (this->getKernelContext().getTypeManager().isStream(l_oStreamTypeIdentifier))
					{
						//First check if it is a stream
						if (rExistingBox.hasInputSupport(l_oStreamTypeIdentifier)) { this->addInputSupport(l_oStreamTypeIdentifier); }
						if (rExistingBox.hasOutputSupport(l_oStreamTypeIdentifier)) { this->addOutputSupport(l_oStreamTypeIdentifier); }
					}
				}

				this->enableNotification();

				this->notify(BoxModification_Initialized);

				return true;
			}

			//___________________________________________________________________//
			//                                                                   //


			virtual bool addInterfacor(BoxInterfacorType interfacorType, const CString& newName, const CIdentifier& typeID, const CIdentifier& identifier,
									   bool shouldNotify)
			{
				switch (interfacorType)
				{
					case Input:
					case Output:
						OV_ERROR_UNLESS_KRF(this->getTypeManager().isStream(typeID),
											"While adding " << g_InterfacorTypeToName.at(interfacorType) << " '" << newName << "' to box '" << this->getName()
											<< "', unknown stream type identifier " << typeID.toString(),
											ErrorType::BadArgument);
						break;
					case Setting: break;
					default: break;
				}

				uint32_t position = uint32_t(m_Interfacors[interfacorType].size());
				switch (interfacorType)
				{
					case Input:
					case Output:
						m_Interfacors[interfacorType].push_back(std::make_shared<CInputOutput>(newName, typeID, identifier));
						break;
					case Setting:
						m_Interfacors[interfacorType].push_back(std::make_shared<CSetting>(newName, typeID, identifier, "", false));
						break;
					default: break;
				}

				if (identifier != OV_UndefinedIdentifier) { m_InterfacorIdentifierToIndex[interfacorType][identifier] = position; }

				CString uniqueName = this->getUnusedName(m_InterfacorNameToIndex[interfacorType], newName);

				m_Interfacors[interfacorType][position]->m_sName    = uniqueName;
				m_InterfacorNameToIndex[interfacorType][uniqueName] = position;

				OV_ERROR_UNLESS_KRF(m_InterfacorNameToIndex[interfacorType].size() == m_Interfacors[interfacorType].size(),
									"Box " << m_sName << " has corrupted name map storage", ErrorType::BadResourceCreation);

				if (shouldNotify)
				{
					auto newCount = this->getInterfacorCount(interfacorType) - 1;
					switch (interfacorType)
					{
						case Input:
							this->notify(BoxModification_InputAdded, newCount);
							break;
						case Output:
							this->notify(BoxModification_OutputAdded, newCount);
							break;
						case Setting:
							this->notify(BoxModification_SettingAdded, newCount);
							break;
						default: break;
					}
				}
				return true;
			}

			virtual bool removeInterfacor(BoxInterfacorType interfacorType, const uint32_t index, const bool shouldNotify = true)
			{
				switch (interfacorType)
				{
					case Input:
						return this->removeInput(index, shouldNotify);
					case Output:
						return this->removeOutput(index, shouldNotify);
					case Setting:
						return this->removeSetting(index, shouldNotify);
					default: break;
				}
				return false;
			}

			virtual uint32_t getInterfacorCount(BoxInterfacorType interfacorType) const
			{
				auto interfacors = m_Interfacors.at(interfacorType);
				return uint32_t(std::count_if(interfacors.begin(), interfacors.end(), [](const std::shared_ptr<CInterfacor>& i) { return !i->m_bDeprecated; }));
			}

			virtual uint32_t getInterfacorCountIncludingDeprecated(BoxInterfacorType interfacorType) const
			{
				return uint32_t(m_Interfacors.at(interfacorType).size());
			}


			virtual bool getInterfacorIdentifier(BoxInterfacorType interfacorType, const uint32_t index, CIdentifier& identifier) const
			{
				identifier = OV_UndefinedIdentifier;
				OV_ERROR_UNLESS_KRF(index < m_Interfacors.at(interfacorType).size(),
									g_InterfacorTypeToName.at(interfacorType) << " index = [" << index << "] is out of range (max index = [" << uint32_t(
										m_Interfacors.at(interfacorType).size() - 1) << "])",
									ErrorType::OutOfBound);

				identifier = m_Interfacors.at(interfacorType)[index]->m_oIdentifier;
				return true;
			}

			virtual bool getInterfacorIndex(BoxInterfacorType interfacorType, const CIdentifier& identifier, uint32_t& index) const
			{
				index   = OV_Value_UndefinedIndexUInt;
				auto it = m_InterfacorIdentifierToIndex.at(interfacorType).find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorIdentifierToIndex.at(interfacorType).end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << " with identifier " << identifier.toString(),
									ErrorType::ResourceNotFound);

				index = it->second;
				return true;
			}

			virtual bool getInterfacorIndex(BoxInterfacorType interfacorType, const CString& name, uint32_t& index) const
			{
				index   = OV_Value_UndefinedIndexUInt;
				auto it = m_InterfacorNameToIndex.at(interfacorType).find(name);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorNameToIndex.at(interfacorType).end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << " with name " << name, ErrorType::ResourceNotFound);

				index = it->second;
				return true;
			}


			virtual bool getInterfacorType(BoxInterfacorType interfacorType, const uint32_t index, CIdentifier& typeID) const
			{
				OV_ERROR_UNLESS_KRF(index < m_Interfacors.at(interfacorType).size(),
									g_InterfacorTypeToName.at(interfacorType) << " index = [" << index << "] is out of range (max index = [" << uint32_t(
										m_Interfacors.at(interfacorType).size() - 1) << "])", ErrorType::OutOfBound);

				typeID = m_Interfacors.at(interfacorType)[index]->m_oTypeIdentifier;
				return true;
			}

			virtual bool getInterfacorType(BoxInterfacorType interfacorType, const CIdentifier& identifier, CIdentifier& typeID) const
			{
				auto it = m_InterfacorIdentifierToIndex.at(interfacorType).find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorIdentifierToIndex.at(interfacorType).end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << " with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->getInterfacorType(interfacorType, it->second, typeID);
			}

			virtual bool getInterfacorType(BoxInterfacorType interfacorType, const CString& name, CIdentifier& typeID) const
			{
				auto it = m_InterfacorNameToIndex.at(interfacorType).find(name);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorNameToIndex.at(interfacorType).end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << " with name " << name, ErrorType::ResourceNotFound);

				return this->getInterfacorType(interfacorType, it->second, typeID);
			}


			virtual bool getInterfacorName(BoxInterfacorType interfacorType, const uint32_t index, CString& name) const
			{
				OV_ERROR_UNLESS_KRF(index < m_Interfacors.at(interfacorType).size(),
									g_InterfacorTypeToName.at(interfacorType) << " index = [" << index << "] is out of range (max index = [" << uint32_t(
										m_Interfacors.at(interfacorType).size() - 1) << "])",
									ErrorType::OutOfBound);

				name = m_Interfacors.at(interfacorType)[index]->m_sName;
				return true;
			}

			virtual bool getInterfacorName(BoxInterfacorType interfacorType, const CIdentifier& identifier, CString& name) const
			{
				auto it = m_InterfacorIdentifierToIndex.at(interfacorType).find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorIdentifierToIndex.at(interfacorType).end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << " with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->getInputName(it->second, name);
			}

			virtual bool getInterfacorDeprecatedStatus(BoxInterfacorType interfacorType, const uint32_t index, bool& value) const
			{
				if (index >= m_Interfacors.at(interfacorType).size()) { OV_WARNING_K("DUH"); }
				OV_ERROR_UNLESS_KRF(index < m_Interfacors.at(interfacorType).size(),
									g_InterfacorTypeToName.at(interfacorType) << " index = [" << index << "] is out of range (max index = [" << uint32_t(
										m_Interfacors.at(interfacorType).size() - 1) << "])",
									ErrorType::OutOfBound);

				value = m_Interfacors.at(interfacorType)[index]->m_bDeprecated;
				return true;
			}

			virtual bool getInterfacorDeprecatedStatus(BoxInterfacorType interfacorType, const CIdentifier& identifier, bool& value) const
			{
				auto it = m_InterfacorIdentifierToIndex.at(interfacorType).find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorIdentifierToIndex.at(interfacorType).end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << " with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->getInterfacorDeprecatedStatus(interfacorType, it->second, value);
			}

			virtual bool hasInterfacorWithIdentifier(BoxInterfacorType interfacorType, const CIdentifier& identifier) const
			{
				return m_InterfacorIdentifierToIndex.at(interfacorType).find(identifier) != m_InterfacorIdentifierToIndex.at(interfacorType).end();
			}

			virtual bool hasInterfacorWithNameAndType(BoxInterfacorType interfacorType, const CString& name, const CIdentifier& typeID) const
			{
				return m_InterfacorNameToIndex.at(interfacorType).find(name) != m_InterfacorNameToIndex.at(interfacorType).end();
			}

			virtual bool hasInterfacorWithType(BoxInterfacorType interfacorType, const uint32_t index, const CIdentifier& typeID) const
			{
				if (index < this->getInterfacorCount(interfacorType))
				{
					CIdentifier type = OV_UndefinedIdentifier;
					this->getInterfacorType(interfacorType, index, type);
					return (type == typeID);
				}
				return false;
			}

			virtual bool setInterfacorType(BoxInterfacorType interfacorType, const uint32_t index, const CIdentifier& typeID)
			{
				switch (interfacorType)
				{
					case Input:
					case Output:
						OV_ERROR_UNLESS_KRF(this->getTypeManager().isStream(typeID),
											"While changing box '" << this->getName() << "' " << g_InterfacorTypeToName.at(interfacorType) <<
											" type, unknown stream type identifier " << typeID.toString(),
											ErrorType::BadArgument);
						break;
					case Setting:
						break;
				}

				OV_ERROR_UNLESS_KRF(index < m_Interfacors.at(interfacorType).size(),
									g_InterfacorTypeToName.at(interfacorType) << " index = [" << index << "] is out of range (max index = [" << uint32_t(
										m_Interfacors.at(interfacorType).size() - 1) << "])",
									ErrorType::OutOfBound);

				if (m_Interfacors[interfacorType][index]->m_oTypeIdentifier == typeID) { return true; }

				m_Interfacors[interfacorType][index]->m_oTypeIdentifier = typeID;

				switch (interfacorType)
				{
					case Input:
						this->notify(BoxModification_InputTypeChanged, index);
						break;
					case Output:
						this->notify(BoxModification_OutputTypeChanged, index);
						break;
					case Setting:
						this->notify(BoxModification_SettingTypeChanged, index);
						this->notifySettingChange(SettingChange, index);
						break;
				}

				return true;
			}

			virtual bool setInterfacorType(BoxInterfacorType interfacorType, const CIdentifier& identifier, const CIdentifier& typeID)
			{
				auto it = m_InterfacorIdentifierToIndex[interfacorType].find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorIdentifierToIndex[interfacorType].end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << " with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->setInterfacorType(interfacorType, it->second, typeID);
			}

			virtual bool setInterfacorType(BoxInterfacorType interfacorType, const CString& name, const CIdentifier& typeID)
			{
				auto it = m_InterfacorNameToIndex[interfacorType].find(name);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorNameToIndex[interfacorType].end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << " with name " << name, ErrorType::ResourceNotFound);

				return this->setInterfacorType(interfacorType, it->second, typeID);
			}


			virtual bool setInterfacorName(BoxInterfacorType interfacorType, const uint32_t index, const CString& newName)
			{
				OV_ERROR_UNLESS_KRF(index < m_Interfacors[interfacorType].size(),
									g_InterfacorTypeToName.at(interfacorType) << " index = [" << index << "] is out of range (max index = [" << uint32_t(
										m_Interfacors[interfacorType].size() - 1) << "])",
									ErrorType::OutOfBound);

				if (m_Interfacors[interfacorType][index]->m_sName == newName)
				{
					// no change, don't bother notifying
					return true;
				}

				// remove entry from name key map
				auto it = m_InterfacorNameToIndex[interfacorType].find(m_Interfacors[interfacorType][index]->m_sName);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorNameToIndex[interfacorType].end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << " with name " << m_Interfacors[interfacorType][index]->
									m_sName,
									ErrorType::ResourceNotFound);
				m_InterfacorNameToIndex[interfacorType].erase(it);

				// check for duplicated name key and update if necessary
				CString uniqueName                                  = this->getUnusedName(m_InterfacorNameToIndex[interfacorType], newName);
				m_InterfacorNameToIndex[interfacorType][uniqueName] = index;
				m_Interfacors[interfacorType][index]->m_sName       = uniqueName;

				OV_ERROR_UNLESS_KRF(m_InterfacorNameToIndex[interfacorType].size() == m_Interfacors[interfacorType].size(),
									"Box " << m_sName << " has corrupted name map storage", ErrorType::BadResourceCreation);

				switch (interfacorType)
				{
					case Input:
						this->notify(BoxModification_InputNameChanged, index);
						break;
					case Output:
						this->notify(BoxModification_OutputNameChanged, index);
						break;
					case Setting:
						this->notify(BoxModification_SettingNameChanged, index);
						break;
				}

				return true;
			}

			virtual bool setInterfacorName(BoxInterfacorType interfacorType, const CIdentifier& identifier, const CString& newName)
			{
				auto it = m_InterfacorIdentifierToIndex[interfacorType].find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorIdentifierToIndex[interfacorType].end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << " with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->setInterfacorName(interfacorType, it->second, newName);
			}

			virtual bool setInterfacorDeprecatedStatus(BoxInterfacorType interfacorType, const uint32_t index, bool newValue)
			{
				OV_ERROR_UNLESS_KRF(index < m_Interfacors[interfacorType].size(),
									g_InterfacorTypeToName.at(interfacorType) << " index = [" << index << "] is out of range (max index = [" << uint32_t(
										m_Interfacors[interfacorType].size() - 1) << "])",
									ErrorType::OutOfBound);

				m_Interfacors[interfacorType][index]->m_bDeprecated = newValue;

				return true;
			}

			virtual bool setInterfacorDeprecatedStatus(BoxInterfacorType interfacorType, const CIdentifier& identifier, bool newValue)
			{
				auto it = m_InterfacorIdentifierToIndex[interfacorType].find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorIdentifierToIndex[interfacorType].end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << "  with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->setInterfacorDeprecatedStatus(interfacorType, it->second, newValue);
			}

			//___________________________________________________________________//
			//                                                                   //

			virtual bool addInput(const CString& name, const CIdentifier& typeID, const CIdentifier& identifier, const bool bNotify)
			{
				return this->addInterfacor(Input, name, typeID, identifier, bNotify);
			}

			virtual bool removeInput(const uint32_t index, const bool bNotify = true)
			{
				OV_ERROR_UNLESS_KRF(index < m_Interfacors[Input].size(),
									"Input index = [" << index << "] is out of range (max index = [" << uint32_t(m_Interfacors[Input].size() - 1) << "])",
									ErrorType::OutOfBound);


				{
					CIdentifier* listID = nullptr;
					size_t nbElems      = 0;
					m_pOwnerScenario->getLinkIdentifierToBoxInputList(m_oIdentifier, index, &listID, &nbElems);
					for (size_t i = 0; i < nbElems; ++i) { m_pOwnerScenario->disconnect(listID[i]); }
					m_pOwnerScenario->releaseIdentifierList(listID);
				}

				// $$$
				// The way the links are removed here
				// is not correct because they are all
				// collected and then all removed. In case
				// the box listener callback on box removal,
				// the nextcoming links would potentially be
				// invalid
				std::vector<std::pair<std::pair<uint64_t, uint32_t>, std::pair<uint64_t, uint32_t>>> l_vLink;

				{
					CIdentifier* listID = nullptr;
					size_t nbElems      = 0;
					m_pOwnerScenario->getLinkIdentifierToBoxList(m_oIdentifier, &listID, &nbElems);
					for (size_t i = 0; i < nbElems; ++i)
					{
						CIdentifier l_oIdentifier = listID[i];
						ILink* l_pLink            = m_pOwnerScenario->getLinkDetails(l_oIdentifier);
						if (l_pLink->getTargetBoxInputIndex() > index)
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

							if (m_pOwnerScenario->isLink(l_oIdentifier)) { m_pOwnerScenario->disconnect(l_oIdentifier); }
						}
					}
					m_pOwnerScenario->releaseIdentifierList(listID);
				}

				// This reorganizes the parent's scenario links if this box is not actually a scenario itself
				if (m_oIdentifier != OV_UndefinedIdentifier)
				{
					std::vector<std::pair<uint32_t, std::pair<uint64_t, uint32_t>>> l_vScenarioLink;
					for (uint32_t scenarioInputIdx = 0; scenarioInputIdx < m_pOwnerScenario->getInterfacorCount(Input); scenarioInputIdx++)
					{
						CIdentifier l_oBoxIdentifier     = OV_UndefinedIdentifier;
						uint32_t l_ui32BoxConnectorIndex = uint32_t(-1);
						m_pOwnerScenario->getScenarioInputLink(scenarioInputIdx, l_oBoxIdentifier, l_ui32BoxConnectorIndex);
						if (l_oBoxIdentifier == m_oIdentifier)
						{
							if (l_ui32BoxConnectorIndex > index)
							{
								l_vScenarioLink.push_back({
									scenarioInputIdx,
									{
										l_oBoxIdentifier.toUInteger(),
										l_ui32BoxConnectorIndex
									}
								});
							}
							if (l_ui32BoxConnectorIndex >= index)
							{
								m_pOwnerScenario->removeScenarioInputLink(scenarioInputIdx, l_oBoxIdentifier, l_ui32BoxConnectorIndex);
							}
						}
					}

					// Reconnects scenario links
					for (const auto& link : l_vScenarioLink) { m_pOwnerScenario->setScenarioInputLink(link.first, link.second.first, link.second.second - 1); }
				}

				CIdentifier toBeRemovedId = m_Interfacors[Input][index]->m_oIdentifier;
				CString toBeRemovedName   = m_Interfacors[Input][index]->m_sName;

				// Erases actual input
				m_Interfacors[Input].erase(m_Interfacors[Input].begin() + index);

				// Reconnects box links
				for (const auto& link : l_vLink)
				{
					CIdentifier newId = OV_UndefinedIdentifier;
					m_pOwnerScenario->connect(newId, link.first.first, link.first.second, link.second.first, link.second.second - 1, OV_UndefinedIdentifier);
				}

				// erase name key
				auto itName = m_InterfacorNameToIndex[Input].find(toBeRemovedName);
				OV_ERROR_UNLESS_KRF(itName != m_InterfacorNameToIndex[Input].end(), "No input found with name " << toBeRemovedName,
									ErrorType::ResourceNotFound);
				m_InterfacorNameToIndex[Input].erase(itName);

				// erase identifier key if defined
				if (toBeRemovedId != OV_UndefinedIdentifier)
				{
					auto itIdent = m_InterfacorIdentifierToIndex[Input].find(toBeRemovedId);
					OV_ERROR_UNLESS_KRF(itIdent != m_InterfacorIdentifierToIndex[Input].end(), "No input found with id " << toBeRemovedId.toString(),
										ErrorType::ResourceNotFound);
					m_InterfacorIdentifierToIndex[Input].erase(itIdent);
				}

				if (bNotify) { this->notify(BoxModification_InputRemoved, index); }

				return true;
			}

			virtual uint32_t getInputCount() const { return this->getInterfacorCount(Input); }

			virtual bool getInputType(const uint32_t index, CIdentifier& typeID) const { return this->getInterfacorType(Input, index, typeID); }

			virtual bool getInputName(const uint32_t index, CString& rName) const { return this->getInterfacorName(Input, index, rName); }

			virtual bool getInputName(const CIdentifier& rInputIdentifier, CString& rName) const
			{
				return this->getInterfacorName(Input, rInputIdentifier, rName);
			}

			virtual bool setInputType(const uint32_t index, const CIdentifier& typeID) { return this->setInterfacorType(Input, index, typeID); }

			virtual bool setInputName(const uint32_t index, const CString& rName) { return this->setInterfacorName(Input, index, rName); }

			//___________________________________________________________________//
			//                                                                   //

			virtual bool addOutput(const CString& name, const CIdentifier& typeID, const CIdentifier& identifier, const bool bNotify)
			{
				return this->addInterfacor(Output, name, typeID, identifier, bNotify);
			}

			virtual bool removeOutput(const uint32_t index, const bool bNotify = true)
			{
				OV_ERROR_UNLESS_KRF(index < m_Interfacors[Output].size(),
									"Output index = [" << index << "] is out of range (max index = [" << uint32_t(m_Interfacors[Output].size() - 1) << "])",
									ErrorType::OutOfBound);

				std::vector<std::pair<std::pair<uint64_t, uint32_t>, std::pair<uint64_t, uint32_t>>> l_vLink;

				if (m_pOwnerScenario)
				{
					CIdentifier* listID = nullptr;
					size_t nbElems      = 0;

					m_pOwnerScenario->getLinkIdentifierFromBoxOutputList(m_oIdentifier, index, &listID, &nbElems);
					for (size_t i = 0; i < nbElems; ++i) { m_pOwnerScenario->disconnect(listID[i]); }
					m_pOwnerScenario->releaseIdentifierList(listID);

					// $$$
					// The way the links are removed here is not correct because they are all collected and then all removed. In case
					// the box listener callback on box removal, the nextcoming links would potentially be invalid
					{
						listID  = nullptr;
						nbElems = 0;
						m_pOwnerScenario->getLinkIdentifierFromBoxOutputList(m_oIdentifier, index, &listID, &nbElems);
						for (size_t i = 0; i < nbElems; ++i)
						{
							const CIdentifier& cur_id = listID[i];
							ILink* l_pLink            = m_pOwnerScenario->getLinkDetails(cur_id);
							if (l_pLink->getSourceBoxOutputIndex() > index)
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
								if (m_pOwnerScenario->isLink(cur_id)) { m_pOwnerScenario->disconnect(cur_id); }
							}
						}
						m_pOwnerScenario->releaseIdentifierList(listID);
					}

					// This reorganizes the parent's scenario links if this box is not actually a scenario
					if (m_oIdentifier != OV_UndefinedIdentifier)
					{
						std::vector<std::pair<uint32_t, std::pair<uint64_t, uint32_t>>> l_vScenarioLink;
						for (uint32_t scenarioOutputIdx = 0; scenarioOutputIdx < m_pOwnerScenario->getOutputCount(); scenarioOutputIdx++)
						{
							CIdentifier l_oBoxIdentier       = OV_UndefinedIdentifier;
							uint32_t l_ui32BoxConnectorIndex = uint32_t(-1);
							m_pOwnerScenario->getScenarioOutputLink(scenarioOutputIdx, l_oBoxIdentier, l_ui32BoxConnectorIndex);
							if (l_oBoxIdentier == m_oIdentifier)
							{
								if (l_ui32BoxConnectorIndex > index)
								{
									l_vScenarioLink.push_back({ scenarioOutputIdx, { l_oBoxIdentier.toUInteger(), l_ui32BoxConnectorIndex } });
								}
								if (l_ui32BoxConnectorIndex >= index)
								{
									m_pOwnerScenario->removeScenarioOutputLink(scenarioOutputIdx, l_oBoxIdentier, l_ui32BoxConnectorIndex);
								}
							}
						}

						// Reconnects scenario links
						for (const auto& link : l_vScenarioLink)
						{
							m_pOwnerScenario->setScenarioOutputLink(link.first, link.second.first, link.second.second - 1);
						}
					}
				}
				CIdentifier toBeRemovedId = m_Interfacors.at(Output)[index]->m_oIdentifier;
				CString toBeRemovedName   = m_Interfacors.at(Output)[index]->m_sName;

				// Erases actual output
				m_Interfacors[Output].erase(m_Interfacors.at(Output).begin() + index);

				// Reconnects box links
				if (m_pOwnerScenario)
				{
					for (const auto& link : l_vLink)
					{
						CIdentifier newId = OV_UndefinedIdentifier;
						m_pOwnerScenario->connect(newId, link.first.first, link.first.second - 1, link.second.first, link.second.second,
												  OV_UndefinedIdentifier);
					}
				}


				// erase name key
				auto itName = m_InterfacorNameToIndex.at(Output).find(toBeRemovedName);
				OV_ERROR_UNLESS_KRF(itName != m_InterfacorNameToIndex.at(Output).end(), "No output found with name " << toBeRemovedName,
									ErrorType::ResourceNotFound);
				m_InterfacorNameToIndex.at(Output).erase(itName);

				// erase identifier key if defined
				if (toBeRemovedId != OV_UndefinedIdentifier)
				{
					auto itIdent = m_InterfacorIdentifierToIndex.at(Output).find(toBeRemovedId);
					OV_ERROR_UNLESS_KRF(itIdent != m_InterfacorIdentifierToIndex.at(Output).end(), "No output found with id " << toBeRemovedId.toString(),
										ErrorType::ResourceNotFound);

					m_InterfacorIdentifierToIndex.at(Output).erase(itIdent);
				}

				if (bNotify) { this->notify(BoxModification_OutputRemoved, index); }

				return true;
			}

			virtual uint32_t getOutputCount() const { return this->getInterfacorCount(Output); }

			virtual bool getOutputType(const uint32_t index, CIdentifier& typeID) const { return this->getInterfacorType(Output, index, typeID); }

			virtual bool getOutputName(const uint32_t index, CString& rName) const { return this->getInterfacorName(Output, index, rName); }

			virtual bool setOutputType(const uint32_t index, const CIdentifier& typeID) { return this->setInterfacorType(Output, index, typeID); }

			virtual bool setOutputName(const uint32_t index, const CString& rName) { return this->setInterfacorName(Output, index, rName); }

			virtual bool addInterfacorTypeSupport(BoxInterfacorType interfacorType, const CIdentifier& typeID)
			{
				if (interfacorType == Input) { m_vSupportInputType.push_back(typeID); }
				else if (interfacorType == Output) { m_vSupportOutputType.push_back(typeID); }

				return false;
			}

			virtual bool hasInterfacorTypeSupport(BoxInterfacorType interfacorType, const CIdentifier& typeID) const
			{
				if (interfacorType == Input)
				{
					if (m_vSupportInputType.empty()) { return true; }

					for (size_t i = 0; i < m_vSupportInputType.size(); ++i) { if (m_vSupportInputType[i] == typeID) { return true; } }
				}
				else if (interfacorType == Output)
				{
					//If there is no type specify, we allow all
					if (m_vSupportOutputType.empty()) { return true; }

					for (size_t i = 0; i < m_vSupportOutputType.size(); ++i) { if (m_vSupportOutputType[i] == typeID) { return true; } }
				}
				else
				{
					// Settings are always all supported, in a way
					return true;
				}

				return false;
			}

			virtual bool addInputSupport(const CIdentifier& typeID) { return this->addInterfacorTypeSupport(Input, typeID); }

			virtual bool hasInputSupport(const CIdentifier& typeID) const { return this->hasInterfacorTypeSupport(Input, typeID); }

			virtual bool addOutputSupport(const CIdentifier& typeID) { return this->addInterfacorTypeSupport(Output, typeID); }

			virtual bool hasOutputSupport(const CIdentifier& typeID) const { return this->hasInterfacorTypeSupport(Output, typeID); }

			virtual bool setSupportTypeFromAlgorithmIdentifier(const CIdentifier& typeID)
			{
				const Plugins::IPluginObjectDesc* l_pPluginObjectDescriptor = this->getKernelContext().getPluginManager().getPluginObjectDescCreating(typeID);
				const Plugins::IBoxAlgorithmDesc* l_pBoxAlgorithmDescriptor = dynamic_cast<const Plugins::IBoxAlgorithmDesc*>(l_pPluginObjectDescriptor);

				OV_ERROR_UNLESS_KRF(l_pBoxAlgorithmDescriptor, "Tried to initialize with an unregistered algorithm", ErrorType::Internal);

				//We use the neutralized version of CBoxProto to just initialize the stream restriction mecanism
				CBoxProtoRestriction oTempProto(this->getKernelContext(), *this);
				l_pBoxAlgorithmDescriptor->getBoxPrototype(oTempProto);
				return true;
			}

			virtual std::vector<CIdentifier> getInputSupportTypes() const { return m_vSupportInputType; }
			virtual std::vector<CIdentifier> getOutputSupportTypes() const { return m_vSupportOutputType; }
			virtual void clearOutputSupportTypes() { m_vSupportOutputType.clear(); }
			virtual void clearInputSupportTypes() { m_vSupportInputType.clear(); }

		private:
			CIdentifier getUnusedInterfacorIdentifier(BoxInterfacorType interfacorType, const CIdentifier& suggestedIdentifier = OV_UndefinedIdentifier) const
			{
				uint64_t identifier = System::Math::randomUInteger64();
				if (suggestedIdentifier != OV_UndefinedIdentifier) { identifier = suggestedIdentifier.toUInteger(); }

				CIdentifier resultIdentifier = OV_UndefinedIdentifier;
				std::map<CIdentifier, uint32_t>::const_iterator it;
				do
				{
					resultIdentifier = CIdentifier(identifier);
					it               = m_InterfacorIdentifierToIndex.at(interfacorType).find(resultIdentifier);
					identifier++;
				} while (it != m_InterfacorIdentifierToIndex.at(interfacorType).end() || resultIdentifier == OV_UndefinedIdentifier);
				return resultIdentifier;
			}

		public:

			CIdentifier getUnusedSettingIdentifier(const CIdentifier& suggestedIdentifier = OV_UndefinedIdentifier) const
			{
				return this->getUnusedInterfacorIdentifier(Setting);
			}

			CIdentifier getUnusedInputIdentifier(const CIdentifier& suggestedIdentifier = OV_UndefinedIdentifier) const
			{
				return this->getUnusedInterfacorIdentifier(Input);
			}

			CIdentifier getUnusedOutputIdentifier(const CIdentifier& suggestedIdentifier = OV_UndefinedIdentifier) const
			{
				return this->getUnusedInterfacorIdentifier(Output);
			}

			virtual bool addSetting(const CString& name, const CIdentifier& typeID, const CString& sDefaultValue, const uint32_t index,
									const bool bModifiability, const CIdentifier& identifier, const bool bNotify)
			{
				CString l_sValue(sDefaultValue);
				if (this->getTypeManager().isEnumeration(typeID))
				{
					if (this->getTypeManager().getEnumerationEntryValueFromName(typeID, sDefaultValue) == OV_UndefinedIdentifier)
					{
						if (this->getTypeManager().getEnumerationEntryCount(typeID) != 0)
						{
							// get value to the first enum entry
							// and eventually correct this after
							uint64_t l_ui64Value = 0;
							this->getTypeManager().getEnumerationEntry(typeID, 0, l_sValue, l_ui64Value);

							// Find if the default value string actually is an identifier, otherwise just keep the zero index name as default.
							CIdentifier l_oDefaultValueIdentifier = OV_UndefinedIdentifier;
							l_oDefaultValueIdentifier.fromString(sDefaultValue);

							// Finally, if it is an identifier, then a name should be found
							// from the type manager ! Otherwise l_sValue is left to the default.
							CString l_sCandidateValue = this->getTypeManager().getEnumerationEntryNameFromValue(typeID, l_oDefaultValueIdentifier.toUInteger());
							if (l_sCandidateValue != CString("")) { l_sValue = l_sCandidateValue; }
						}
					}
				}

				CSetting s;
				s.m_sName           = name;
				s.m_oTypeIdentifier = typeID;
				s.m_sDefaultValue   = l_sValue;
				s.m_sValue          = l_sValue;
				s.m_bMod            = bModifiability;
				s.m_oIdentifier     = identifier;

				uint32_t l_ui32Index = index;


				uint32_t l_ui32InsertLocation;

				if (index == OV_Value_UndefinedIndexUInt || index == uint32_t(m_Interfacors[Setting].size()))
				{
					m_Interfacors[Setting].push_back(std::make_shared<CSetting>(s));
					l_ui32InsertLocation = (uint32_t(m_Interfacors[Setting].size())) - 1;
				}
				else
				{
					OV_ERROR_UNLESS_KRF(index <= uint32_t(m_Interfacors[Setting].size()),
										"Tried to push '" << name << "' to slot " << index << " with the array size being " << uint32_t(m_Interfacors[Setting]
											.size()),
										ErrorType::OutOfBound);

					auto l_it = m_Interfacors[Setting].begin();
					l_it += l_ui32Index;
					m_Interfacors[Setting].insert(l_it, std::make_shared<CSetting>(s));
					l_ui32InsertLocation = index;
				}

				if (s.m_oIdentifier != OV_UndefinedIdentifier)
				{
					// add access by CIdentifier key if defined so that size differs from m_vSetting
					m_InterfacorIdentifierToIndex[Setting][s.m_oIdentifier] = l_ui32InsertLocation;
				}
				// add access by name key (always done so that synchronized with m_vSetting
				CString newName                                       = this->getUnusedName(m_InterfacorNameToIndex.at(Setting), s.m_sName);
				m_Interfacors[Setting][l_ui32InsertLocation]->m_sName = newName;
				m_InterfacorNameToIndex[Setting][newName]             = l_ui32InsertLocation;

				OV_ERROR_UNLESS_KRF(m_InterfacorNameToIndex.at(Setting).size() == m_Interfacors[Setting].size(),
									"Box " << m_sName << " has corrupted name map storage", ErrorType::BadResourceCreation);

				//if this setting is modifiable, keep its index
				if (bModifiability) { m_vModifiableSettingIndexes.push_back(l_ui32Index); }

				this->getLogManager() << LogLevel_Debug
						<< "Pushed '" << m_Interfacors.at(Setting)[l_ui32InsertLocation]->m_sName << "' : '"
						<< std::static_pointer_cast<CSetting>(m_Interfacors.at(Setting)[l_ui32InsertLocation])->m_sValue
						<< "' to slot " << l_ui32InsertLocation << " with the array size now " << int(m_Interfacors.at(Setting).size()) << "\n";

				if (bNotify)
				{
					this->notify(BoxModification_SettingAdded, l_ui32InsertLocation);
					this->notifySettingChange(SettingAdd, l_ui32InsertLocation);
				}

				return true;
			}

			virtual bool removeSetting(const uint32_t index, const bool bNotify = true)
			{
				auto it = m_Interfacors[Setting].begin() + index;
				OV_ERROR_UNLESS_KRF(it != m_Interfacors[Setting].end(), "No setting found at index " << index, ErrorType::ResourceNotFound);

				CIdentifier toBeRemovedId = m_Interfacors[Setting][index]->m_oIdentifier;
				CString toBeRemovedName   = m_Interfacors[Setting][index]->m_sName;

				it = m_Interfacors[Setting].erase(it);

				//update the modifiable setting indexes
				for (auto it2 = m_vModifiableSettingIndexes.begin(); it2 != m_vModifiableSettingIndexes.end();)
				{
					if (*it2 == index) { it2 = m_vModifiableSettingIndexes.erase(it2); }
					else if (*it2 > index)
					{
						*it2 -= 1;
						++it2;
					}
				}

				// erase name key
				auto itName = m_InterfacorNameToIndex.at(Setting).find(toBeRemovedName);
				OV_ERROR_UNLESS_KRF(itName != m_InterfacorNameToIndex.at(Setting).end(), "No setting found with name " << toBeRemovedName,
									ErrorType::ResourceNotFound);
				m_InterfacorNameToIndex.at(Setting).erase(itName);

				// erase identifier key if defined
				if (toBeRemovedId != OV_UndefinedIdentifier)
				{
					auto itIdent = m_InterfacorIdentifierToIndex.at(Setting).find(toBeRemovedId);
					OV_ERROR_UNLESS_KRF(itIdent != m_InterfacorIdentifierToIndex.at(Setting).end(), "No setting found with id " << toBeRemovedId.toString(),
										ErrorType::ResourceNotFound);
					m_InterfacorIdentifierToIndex.at(Setting).erase(itIdent);
				}

				if (bNotify)
				{
					this->notify(BoxModification_SettingRemoved, index);
					this->notifySettingChange(SettingDelete, index);
				}

				return true;
			}

			virtual uint32_t getSettingCount() const { return this->getInterfacorCount(Setting); }

			virtual bool hasSettingWithName(const CString& rName) const
			{
				return m_InterfacorNameToIndex.at(Setting).find(rName) != m_InterfacorNameToIndex.at(Setting).end();
			}

			virtual bool getSettingType(const uint32_t index, CIdentifier& typeID) const { return this->getInterfacorType(Setting, index, typeID); }

			virtual bool getSettingName(const uint32_t index, CString& rName) const { return this->getInterfacorName(Setting, index, rName); }

			virtual bool getSettingDefaultValue(const uint32_t index, CString& rDefaultValue) const
			{
				OV_ERROR_UNLESS_KRF(index < m_Interfacors.at(Setting).size(),
									"Setting index = [" << index << "] is out of range (max index = [" << uint32_t(m_Interfacors.at(Setting).size() - 1) <<
									"])",
									ErrorType::OutOfBound);

				rDefaultValue = std::static_pointer_cast<CSetting>(m_Interfacors.at(Setting)[index])->m_sDefaultValue;
				return true;
			}

			virtual bool getSettingDefaultValue(const CIdentifier& identifier, CString& rDefaultValue) const
			{
				auto it = m_InterfacorIdentifierToIndex.at(Setting).find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorIdentifierToIndex.at(Setting).end(), "Failed to find setting with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->getSettingDefaultValue(it->second, rDefaultValue);
			}

			virtual bool getSettingDefaultValue(const CString& name, CString& rDefaultValue) const
			{
				auto it = m_InterfacorNameToIndex.at(Setting).find(name);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorNameToIndex.at(Setting).end(), "Failed to find setting with name " << name,
									ErrorType::ResourceNotFound);

				return this->getSettingDefaultValue(it->second, rDefaultValue);
			}

			virtual bool getSettingValue(const uint32_t index, CString& rValue) const
			{
				OV_ERROR_UNLESS_KRF(index < m_Interfacors.at(Setting).size(),
									"Setting index = [" << index << "] is out of range (max index = [" << uint32_t(m_Interfacors.at(Setting).size() - 1) <<
									"])",
									ErrorType::OutOfBound);

				rValue = std::static_pointer_cast<CSetting>(m_Interfacors.at(Setting)[index])->m_sValue;
				return true;
			}

			virtual bool getSettingValue(const CIdentifier& identifier, CString& rValue) const
			{
				auto it = m_InterfacorIdentifierToIndex.at(Setting).find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorIdentifierToIndex.at(Setting).end(), "Failed to find setting with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->getSettingValue(it->second, rValue);
			}

			virtual bool getSettingValue(const CString& name, CString& rValue) const
			{
				auto it = m_InterfacorNameToIndex.at(Setting).find(name);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorNameToIndex.at(Setting).end(), "Failed to find setting with name " << name,
									ErrorType::ResourceNotFound);

				return this->getSettingValue(it->second, rValue);
			}

			virtual bool getSettingMod(const uint32_t index, bool& rValue) const
			{
				OV_ERROR_UNLESS_KRF(index < m_Interfacors.at(Setting).size(),
									"Setting index = [" << index << "] is out of range (max index = [" << uint32_t(m_Interfacors.at(Setting).size() - 1) <<
									"])",
									ErrorType::OutOfBound);

				rValue = std::static_pointer_cast<CSetting>(m_Interfacors.at(Setting)[index])->m_bMod;
				return true;
			}

			virtual bool getSettingMod(const CIdentifier& identifier, bool& rValue) const
			{
				auto it = m_InterfacorIdentifierToIndex.at(Setting).find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorIdentifierToIndex.at(Setting).end(),
									"Failed to find setting with id " << identifier.toString(), ErrorType::ResourceNotFound);

				return this->getSettingMod(it->second, rValue);
			}

			virtual bool getSettingMod(const CString& name, bool& rValue) const
			{
				auto it = m_InterfacorNameToIndex.at(Setting).find(name);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorNameToIndex.at(Setting).end(),
									"Failed to find setting with name " << name, ErrorType::ResourceNotFound);

				return this->getSettingMod(it->second, rValue);
			}

			virtual bool setSettingType(const uint32_t index, const CIdentifier& typeID) { return this->setInterfacorType(Setting, index, typeID); }

			virtual bool setSettingName(const uint32_t index, const CString& rName) { return this->setInterfacorName(Setting, index, rName); }

			virtual bool setSettingDefaultValue(const uint32_t index, const CString& rDefaultValue)
			{
				OV_ERROR_UNLESS_KRF(index < m_Interfacors.at(Setting).size(),
									"Setting index = [" << index << "] is out of range (max index = [" << uint32_t(m_Interfacors.at(Setting).size() - 1) <<
									"])",
									ErrorType::OutOfBound);

				std::static_pointer_cast<CSetting>(m_Interfacors[Setting][index])->m_sDefaultValue = rDefaultValue;

				this->notify(BoxModification_SettingDefaultValueChanged, index);

				return true;
			}

			virtual bool setSettingDefaultValue(const CIdentifier& identifier, const CString& rDefaultValue)
			{
				auto it = m_InterfacorIdentifierToIndex.at(Setting).find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorIdentifierToIndex.at(Setting).end(), "Failed to find setting with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->setSettingDefaultValue(it->second, rDefaultValue);
			}

			virtual bool setSettingDefaultValue(const CString& name, const CString& rDefaultValue)
			{
				auto it = m_InterfacorNameToIndex.at(Setting).find(name);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorNameToIndex.at(Setting).end(), "Failed to find setting with name " << name,
									ErrorType::ResourceNotFound);
				return this->setSettingDefaultValue(it->second, rDefaultValue);
			}

			virtual bool setSettingValue(const uint32_t index, const CString& rValue, const bool bNotify = true)
			{
				OV_ERROR_UNLESS_KRF(index < m_Interfacors.at(Setting).size(),
									"Setting index = [" << index << "] is out of range (max index = [" << uint32_t(m_Interfacors.at(Setting).size() - 1) <<
									"])",
									ErrorType::OutOfBound);

				auto setting = std::static_pointer_cast<CSetting>(m_Interfacors[Setting][index]);
				if (setting->m_sValue != rValue)
				{
					setting->m_sValue = rValue;

					if (bNotify)
					{
						this->notify(BoxModification_SettingValueChanged, index);
						this->notifySettingChange(SettingValueUpdate, index);
					}
				}

				return true;
			}

			virtual bool setSettingValue(const CIdentifier& identifier, const CString& rValue)
			{
				auto it = m_InterfacorIdentifierToIndex.at(Setting).find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorIdentifierToIndex.at(Setting).end(), "Failed to find setting with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->setSettingValue(it->second, rValue);
			}

			virtual bool setSettingValue(const CString& name, const CString& rValue)
			{
				auto it = m_InterfacorNameToIndex.at(Setting).find(name);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorNameToIndex.at(Setting).end(), "Failed to find setting with name " << name,
									ErrorType::ResourceNotFound);

				return this->setSettingValue(it->second, rValue);
			}

			virtual bool setSettingMod(const uint32_t index, const bool rValue)
			{
				OV_ERROR_UNLESS_KRF(index < m_Interfacors.at(Setting).size(),
									"Setting index = [" << index << "] is out of range (max index = [" << uint32_t(m_Interfacors.at(Setting).size() - 1) <<
									"])",
									ErrorType::OutOfBound);

				std::static_pointer_cast<CSetting>(m_Interfacors[Setting][index])->m_bMod = rValue;

				//this->notify(BoxModification_SettingNameChanged, index);
				return true;
			}

			virtual bool setSettingMod(const CString& name, const bool rValue)
			{
				auto it = m_InterfacorNameToIndex.at(Setting).find(name);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorNameToIndex.at(Setting).end(), "Failed to find setting with name " << name,
									ErrorType::ResourceNotFound);

				return this->setSettingMod(it->second, rValue);
			}

			virtual bool setSettingMod(const CIdentifier& identifier, const bool rValue)
			{
				auto it = m_InterfacorIdentifierToIndex.at(Setting).find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_InterfacorIdentifierToIndex.at(Setting).end(), "Failed to find setting with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->setSettingMod(it->second, rValue);
			}

			virtual bool swapInterfacors(BoxInterfacorType interfacorType, const uint32_t indexA, const uint32_t indexB)
			{
				OV_ERROR_UNLESS_KRF(indexA < m_Interfacors.at(interfacorType).size(),
									g_InterfacorTypeToName.at(interfacorType) << " index = [" << indexA << "] is out of range (max index = [" << uint32_t(
										m_Interfacors.at(interfacorType).size() - 1) << "])",
									ErrorType::OutOfBound);
				OV_ERROR_UNLESS_KRF(indexB < m_Interfacors.at(interfacorType).size(),
									g_InterfacorTypeToName.at(interfacorType) << " index = [" << indexB << "] is out of range (max index = [" << uint32_t(
										m_Interfacors.at(interfacorType).size() - 1) << "])",
									ErrorType::OutOfBound);

				CString nameA;
				CIdentifier identifierA = OV_UndefinedIdentifier;
				CString nameB;
				CIdentifier identifierB = OV_UndefinedIdentifier;

				this->getInterfacorName(interfacorType, indexA, nameA);
				this->getInterfacorIdentifier(interfacorType, indexA, identifierA);
				this->getInterfacorName(interfacorType, indexB, nameB);
				this->getInterfacorIdentifier(interfacorType, indexB, identifierB);

				auto itA = m_Interfacors[interfacorType].begin() + indexA;
				auto itB = m_Interfacors[interfacorType].begin() + indexB;
				// swap settings
				std::iter_swap(itA, itB);
				// update associated maps
				m_InterfacorNameToIndex.at(interfacorType)[nameA]             = indexB;
				m_InterfacorNameToIndex.at(interfacorType)[nameB]             = indexA;
				m_InterfacorIdentifierToIndex.at(interfacorType)[identifierA] = indexB;
				m_InterfacorIdentifierToIndex.at(interfacorType)[identifierB] = indexA;

				return true;
			}

			virtual bool swapSettings(const uint32_t indexA, const uint32_t indexB) { return this->swapInterfacors(Setting, indexA, indexB); }

			virtual bool swapInputs(const uint32_t indexA, const uint32_t indexB) { return this->swapInterfacors(Input, indexA, indexB); }

			virtual bool swapOutputs(const uint32_t indexA, const uint32_t indexB) { return this->swapInterfacors(Output, indexA, indexB); }

			virtual void notifySettingChange(BoxEventMessageType eType, int i32FirstIndex = -1, int i32SecondIndex = -1)
			{
				if (m_bIsObserverNotificationActive)
				{
					BoxEventMessage l_oEvent;
					l_oEvent.m_eType          = eType;
					l_oEvent.m_i32FirstIndex  = i32FirstIndex;
					l_oEvent.m_i32SecondIndex = i32SecondIndex;

					this->setChanged();
					this->notifyObservers(&l_oEvent);
				}
			}

			virtual bool hasModifiableSettings() const
			{
				for (const auto& setting : m_Interfacors.at(Setting)) { if (std::static_pointer_cast<CSetting>(setting)->m_bMod) { return true; } }
				return false;
			}

			virtual uint32_t* getModifiableSettings(uint32_t& rCount) const
			{
				uint32_t* l_pReturn = nullptr;
				rCount              = uint32_t(m_vModifiableSettingIndexes.size());

				return l_pReturn;
			}

			virtual bool updateInterfacorIdentifier(BoxInterfacorType interfacorType, const uint32_t index, const CIdentifier& newID)
			{
				OV_ERROR_UNLESS_KRF(index < m_Interfacors.at(interfacorType).size(),
									g_InterfacorTypeToName.at(interfacorType) << " index = [" << index << "] is out of range (max index = [" << uint32_t(
										m_Interfacors.at(Setting).size() - 1) << "])",
									ErrorType::OutOfBound);

				OV_ERROR_UNLESS_KRF(newID != OV_UndefinedIdentifier, g_InterfacorTypeToName.at(interfacorType) << " identifier can not be undefined",
									ErrorType::BadArgument);

				CIdentifier oldIdentifier = OV_UndefinedIdentifier;
				this->getInterfacorIdentifier(interfacorType, index, oldIdentifier);

				if (oldIdentifier != newID)
				{
					// identifier key update is necessary
					auto it = m_InterfacorIdentifierToIndex.at(interfacorType).find(newID);
					OV_ERROR_UNLESS_KRF(it == m_InterfacorIdentifierToIndex.at(interfacorType).end(),
										"Conflict in " << g_InterfacorTypeToName.at(interfacorType) <<
										" identifiers. An entity with the same identifier exists.",
										ErrorType::ResourceNotFound);
					m_Interfacors[interfacorType][index]->m_oIdentifier  = newID;
					m_InterfacorIdentifierToIndex[interfacorType][newID] = index;
					// remove the old identifier key
					auto itOld = m_InterfacorIdentifierToIndex[interfacorType].find(oldIdentifier);
					if (itOld != m_InterfacorIdentifierToIndex[interfacorType].end()) { m_InterfacorIdentifierToIndex[interfacorType].erase(itOld); }
				}
				return true;
			}

			//*/

			virtual bool acceptVisitor(IObjectVisitor& rObjectVisitor)
			{
				CObjectVisitorContext l_oObjectVisitorContext(this->getKernelContext());
				return rObjectVisitor.processBegin(l_oObjectVisitorContext, *this) && rObjectVisitor.processEnd(l_oObjectVisitorContext, *this);
			}

		protected:

			virtual void clearBox()
			{
				m_pBoxAlgorithmDescriptor   = nullptr;
				m_oAlgorithmClassIdentifier = OV_UndefinedIdentifier;
				m_sName                     = "";
				m_Interfacors[Input].clear();
				m_Interfacors[Output].clear();
				m_Interfacors[Setting].clear();
				m_InterfacorIdentifierToIndex[Input].clear();
				m_InterfacorNameToIndex[Input].clear();
				m_InterfacorIdentifierToIndex[Output].clear();
				m_InterfacorNameToIndex[Output].clear();
				m_InterfacorIdentifierToIndex.at(Setting).clear();
				m_InterfacorNameToIndex.at(Setting).clear();

				this->removeAllAttributes();
			}

			virtual void enableNotification() { m_bIsNotificationActive = true; }
			virtual void disableNotification() { m_bIsNotificationActive = false; }

			virtual void notify(const EBoxModification eBoxModificationType, const uint32_t index)
			{
				if (m_pBoxListener && !m_bIsNotifyingDescriptor && m_bIsNotificationActive)
				{
					CBoxListenerContext l_oContext(this->getKernelContext(), *this, index);
					m_bIsNotifyingDescriptor = true;
					m_pBoxListener->process(l_oContext, eBoxModificationType);
					m_bIsNotifyingDescriptor = false;
				}
			}

			virtual void notify(const EBoxModification eBoxModificationType) { this->notify(eBoxModificationType, 0xffffffff); }

			_IsDerivedFromClass_Final_(TAttributable< TKernelObject <T> >, OVK_ClassId_Kernel_Scenario_Box)

			CString getUnusedName(const std::map<CString, uint32_t>& nameToIndex, const CString& suggestedName) const
			{
				uint32_t idx = 1;
				CString newName;
				auto it = nameToIndex.find(suggestedName);
				do
				{
					newName = suggestedName;
					if (it != nameToIndex.end())
					{
						newName += "(" + CString(std::to_string(idx).c_str()) + ")";
						it = nameToIndex.find(newName);
						idx++;
					}
				} while (it != nameToIndex.end());
				return newName;
			}

			IScenario* m_pOwnerScenario                                 = nullptr;
			const Plugins::IBoxAlgorithmDesc* m_pBoxAlgorithmDescriptor = nullptr;
			Plugins::IBoxListener* m_pBoxListener                       = nullptr;
			bool m_bIsNotifyingDescriptor                               = false;
			bool m_bIsNotificationActive                                = true;
			bool m_bIsObserverNotificationActive                        = true;

			CIdentifier m_oIdentifier               = OV_UndefinedIdentifier;
			CIdentifier m_oAlgorithmClassIdentifier = OV_UndefinedIdentifier;
			CString m_sName                         = "unnamed";

			std::map<BoxInterfacorType, std::map<CIdentifier, uint32_t>> m_InterfacorIdentifierToIndex;
			std::map<BoxInterfacorType, std::map<CString, uint32_t>> m_InterfacorNameToIndex;

			//to avoid having to recheck every setting every time
			//careful to update at each setting modification
			std::vector<uint32_t> m_vModifiableSettingIndexes;

			std::vector<CIdentifier> m_vSupportInputType;
			std::vector<CIdentifier> m_vSupportOutputType;
		private:
			std::map<BoxInterfacorType, std::vector<std::shared_ptr<CInterfacor>>> m_Interfacors;
		};
	} // namespace Kernel
} // namespace OpenViBE
