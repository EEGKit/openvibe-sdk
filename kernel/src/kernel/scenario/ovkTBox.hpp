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
#include <system/ovCMath.h>
#include <system/ovCMemory.h>
#include <memory>

namespace
{
	std::map<OpenViBE::Kernel::EBoxInterfacorType, OpenViBE::CString> g_InterfacorTypeToName = {
		{ OpenViBE::Kernel::EBoxInterfacorType::Setting, "Setting" },
		{ OpenViBE::Kernel::EBoxInterfacorType::Input, "Input" },
		{ OpenViBE::Kernel::EBoxInterfacorType::Output, "Output" }
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
					  const OpenViBE::CIdentifier& /*identifier*/ = OV_UndefinedIdentifier, const bool /*notify*/  = true) override { return true; }

		bool addOutput(const OpenViBE::CString& /*name*/, const OpenViBE::CIdentifier& /*typeID*/,
					   const OpenViBE::CIdentifier& /*identifier*/ = OV_UndefinedIdentifier, const bool /*notify*/  = true) override { return true; }

		bool addSetting(const OpenViBE::CString& /*name*/, const OpenViBE::CIdentifier& /*typeID*/, const OpenViBE::CString& /*defaultValue*/,
						const bool /*modifiable*/  = false, const OpenViBE::CIdentifier& /*identifier*/ = OV_UndefinedIdentifier, const bool /*notify*/  = true) override { return true; }

		bool addFlag(const OpenViBE::Kernel::EBoxFlag /*boxFlag*/) override { return true; }
		bool addFlag(const OpenViBE::CIdentifier& /*identifier*/) override { return true; }
	};

	class CInterfacor
	{
	public:
		CInterfacor() { }

		CInterfacor(const CInterfacor& other)
			: m_name(other.m_name), m_typeID(other.m_typeID), m_id(other.m_id), m_bDeprecated(other.m_bDeprecated) { }

		CInterfacor(const OpenViBE::CString& name, const OpenViBE::CIdentifier& idType, const OpenViBE::CIdentifier& id)
			: m_name(name), m_typeID(idType), m_id(id) {}

		OpenViBE::CString m_name;
		OpenViBE::CIdentifier m_typeID = OV_UndefinedIdentifier;
		OpenViBE::CIdentifier m_id     = OV_UndefinedIdentifier;
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

		CSetting(const CSetting& s) : CInterfacor(s), m_defaultValue(s.m_defaultValue), m_value(s.m_value), m_bMod(s.m_bMod) { this->m_bDeprecated = false; }

		CSetting(const OpenViBE::CString& name, const OpenViBE::CIdentifier& idType, const OpenViBE::CIdentifier& id, const OpenViBE::CString& defaultValue,
				 const bool modifiable)
			: CInterfacor(name, idType, id), m_defaultValue(defaultValue), m_value(defaultValue), m_bMod(modifiable) { }

		OpenViBE::CString m_defaultValue;
		OpenViBE::CString m_value;
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

			explicit TBox(const IKernelContext& ctx) : TAttributable<TKernelObject<T>>(ctx), m_identifier(OV_UndefinedIdentifier), m_algorithmClassID(OV_UndefinedIdentifier)
			{
				for (auto i : { Input, Output, Setting })
				{
					m_interfacors[i]         = std::vector<std::shared_ptr<CInterfacor>>();
					m_interfacorIDToIdx[i]   = std::map<CIdentifier, uint32_t>();
					m_interfacorNameToIdx[i] = std::map<CString, uint32_t>();
				}
			}

			~TBox() override
			{
				if (m_boxAlgorithmDesc && m_boxListener)
				{
					CBoxListenerContext ctx(this->getKernelContext(), *this, 0xffffffff);
					m_boxListener->uninitialize(ctx);
					m_boxAlgorithmDesc->releaseBoxListener(m_boxListener);
				}
			}

			void setOwnerScenario(IScenario* ownerScenario) { m_ownerScenario = ownerScenario; }
			CIdentifier getIdentifier() const override { return m_identifier; }
			CString getName() const override { return m_name; }
			CIdentifier getAlgorithmClassIdentifier() const override { return m_algorithmClassID; }

			bool setIdentifier(const CIdentifier& identifier) override
			{
				OV_ERROR_UNLESS_KRF(m_identifier == OV_UndefinedIdentifier, "Trying to overwrite an already set indentifier", ErrorType::BadCall);
				OV_ERROR_UNLESS_KRF(identifier != OV_UndefinedIdentifier, "Trying to set an undefined identifier", ErrorType::BadArgument);

				m_identifier = identifier;
				this->notify(BoxModification_IdentifierChanged);

				return true;
			}

			bool setName(const CString& name) override
			{
				m_name = name;
				this->notify(BoxModification_NameChanged);
				return true;
			}

			bool setAlgorithmClassIdentifier(const CIdentifier& algorithmClassID) override
			{
				// We need to set the box algorithm identifier in any case. This is because OpenViBE should be able to load
				// a scenario with non-existing boxes and save it without modifying them.
				m_algorithmClassID = algorithmClassID;

				if (!(algorithmClassID == OVP_ClassId_BoxAlgorithm_Metabox || this->getKernelContext().getPluginManager().canCreatePluginObject(algorithmClassID)))
				{
					//					OV_WARNING_K("Box algorithm descriptor not found " << algorithmClassID.toString());
					return true;
				}

				if (m_boxAlgorithmDesc && m_boxListener)
				{
					CBoxListenerContext l_oContext(this->getKernelContext(), *this, 0xffffffff);
					m_boxListener->uninitialize(l_oContext);
					m_boxAlgorithmDesc->releaseBoxListener(m_boxListener);
				}

				const Plugins::IPluginObjectDesc* l_pPluginObjectDescriptor = this
																			  ->getKernelContext().getPluginManager().getPluginObjectDescCreating(
																				  algorithmClassID);
				m_boxAlgorithmDesc = dynamic_cast<const Plugins::IBoxAlgorithmDesc*>(l_pPluginObjectDescriptor);

				if (m_boxAlgorithmDesc)
				{
					m_boxListener = m_boxAlgorithmDesc->createBoxListener();
					if (m_boxListener)
					{
						CBoxListenerContext l_oContext(this->getKernelContext(), *this, 0xffffffff);
						m_boxListener->initialize(l_oContext);
					}
				}

				if (algorithmClassID != OVP_ClassId_BoxAlgorithm_Metabox)
				{
					//We use the neutralized version of CBoxProto to just initialize the stream restriction mecanism
					CBoxProtoRestriction oTempProto(this->getKernelContext(), *this);
					m_boxAlgorithmDesc->getBoxPrototype(oTempProto);
				}

				this->notify(BoxModification_AlgorithmClassIdentifierChanged);

				return true;
			}

			bool initializeFromAlgorithmClassIdentifier(const CIdentifier& algorithmClassID) override
			{
				if (!this->initializeFromAlgorithmClassIdentifierNoInit(algorithmClassID)) { return false; }

				this->notify(BoxModification_Initialized);
				this->notify(BoxModification_DefaultInitialized);
				return true;
			}

			bool initializeFromAlgorithmClassIdentifierNoInit(const CIdentifier& algorithmClassID)
			{
				this->disableNotification();

				const Plugins::IBoxAlgorithmDesc* desc = dynamic_cast<const Plugins::IBoxAlgorithmDesc*>(this->getKernelContext().getPluginManager().getPluginObjectDescCreating(algorithmClassID));
				if (!desc)
				{
					this->enableNotification();
					OV_ERROR_KRF("Algorithm descriptor not found " << algorithmClassID.toString(), ErrorType::ResourceNotFound);
				}

				this->clearBox();
				this->setName(desc->getName());
				this->setAlgorithmClassIdentifier(algorithmClassID);

				CBoxProto proto(this->getKernelContext(), *this);
				desc->getBoxPrototype(proto);

				if (this->hasAttribute(OV_AttributeId_Box_InitialPrototypeHashValue)) { this->setAttributeValue(OV_AttributeId_Box_InitialPrototypeHashValue, this->getPluginManager().getPluginObjectHashValue(algorithmClassID).toString()); }
				else { this->addAttribute(OV_AttributeId_Box_InitialPrototypeHashValue, this->getPluginManager().getPluginObjectHashValue(algorithmClassID).toString()); }

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

			bool initializeFromExistingBox(const IBox& rExistingBox) override
			{
				this->disableNotification();
				m_isObserverNotificationActive = false;

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
							if (isModifiable) { m_modifiableSettingIndexes.push_back(i); }
						}
					}
				}

				CIdentifier l_oID = rExistingBox.getNextAttributeIdentifier(OV_UndefinedIdentifier);
				while (l_oID != OV_UndefinedIdentifier)
				{
					this->addAttribute(l_oID, rExistingBox.getAttributeValue(l_oID));
					l_oID = rExistingBox.getNextAttributeIdentifier(l_oID);
				}

				CIdentifier l_oStreamTypeID = OV_UndefinedIdentifier;
				while ((l_oStreamTypeID = this->getKernelContext().getTypeManager().getNextTypeIdentifier(l_oStreamTypeID)) !=
					   OV_UndefinedIdentifier)
				{
					if (this->getKernelContext().getTypeManager().isStream(l_oStreamTypeID))
					{
						//First check if it is a stream
						if (rExistingBox.hasInputSupport(l_oStreamTypeID)) { this->addInputSupport(l_oStreamTypeID); }
						if (rExistingBox.hasOutputSupport(l_oStreamTypeID)) { this->addOutputSupport(l_oStreamTypeID); }
					}
				}

				this->enableNotification();

				this->notify(BoxModification_Initialized);

				return true;
			}

			//___________________________________________________________________//
			//                                                                   //


			bool addInterfacor(const EBoxInterfacorType interfacorType, const CString& newName, const CIdentifier& typeID, const CIdentifier& identifier, bool shouldNotify) override
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

				const uint32_t position = uint32_t(m_interfacors[interfacorType].size());
				switch (interfacorType)
				{
					case Input:
					case Output:
						m_interfacors[interfacorType].push_back(std::make_shared<CInputOutput>(newName, typeID, identifier));
						break;
					case Setting:
						m_interfacors[interfacorType].push_back(std::make_shared<CSetting>(newName, typeID, identifier, "", false));
						break;
					default: break;
				}

				if (identifier != OV_UndefinedIdentifier) { m_interfacorIDToIdx[interfacorType][identifier] = position; }

				const CString uniqueName = this->getUnusedName(m_interfacorNameToIdx[interfacorType], newName);

				m_interfacors[interfacorType][position]->m_name   = uniqueName;
				m_interfacorNameToIdx[interfacorType][uniqueName] = position;

				OV_ERROR_UNLESS_KRF(m_interfacorNameToIdx[interfacorType].size() == m_interfacors[interfacorType].size(),
									"Box " << m_name << " has corrupted name map storage", ErrorType::BadResourceCreation);

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

			bool removeInterfacor(const EBoxInterfacorType interfacorType, const uint32_t index, const bool shouldNotify = true) override
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

			uint32_t getInterfacorCount(const EBoxInterfacorType interfacorType) const override
			{
				auto interfacors = m_interfacors.at(interfacorType);
				return uint32_t(std::count_if(interfacors.begin(), interfacors.end(), [](const std::shared_ptr<CInterfacor>& i) { return !i->m_bDeprecated; }));
			}

			uint32_t getInterfacorCountIncludingDeprecated(const EBoxInterfacorType interfacorType) const override { return uint32_t(m_interfacors.at(interfacorType).size()); }

			bool getInterfacorIdentifier(const EBoxInterfacorType interfacorType, const uint32_t index, CIdentifier& identifier) const override
			{
				identifier = OV_UndefinedIdentifier;
				OV_ERROR_UNLESS_KRF(index < m_interfacors.at(interfacorType).size(),
									g_InterfacorTypeToName.at(interfacorType) << " index = [" << index << "] is out of range (max index = [" << uint32_t(
										m_interfacors.at(interfacorType).size() - 1) << "])",
									ErrorType::OutOfBound);

				identifier = m_interfacors.at(interfacorType)[index]->m_id;
				return true;
			}

			bool getInterfacorIndex(const EBoxInterfacorType interfacorType, const CIdentifier& identifier, uint32_t& index) const override
			{
				index         = OV_Value_UndefinedIndexUInt;
				const auto it = m_interfacorIDToIdx.at(interfacorType).find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_interfacorIDToIdx.at(interfacorType).end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << " with identifier " << identifier.toString(),
									ErrorType::ResourceNotFound);

				index = it->second;
				return true;
			}

			bool getInterfacorIndex(const EBoxInterfacorType interfacorType, const CString& name, uint32_t& index) const override
			{
				index         = OV_Value_UndefinedIndexUInt;
				const auto it = m_interfacorNameToIdx.at(interfacorType).find(name);
				OV_ERROR_UNLESS_KRF(it != m_interfacorNameToIdx.at(interfacorType).end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << " with name " << name, ErrorType::ResourceNotFound);

				index = it->second;
				return true;
			}

			bool getInterfacorType(const EBoxInterfacorType interfacorType, const uint32_t index, CIdentifier& typeID) const override
			{
				OV_ERROR_UNLESS_KRF(index < m_interfacors.at(interfacorType).size(),
									g_InterfacorTypeToName.at(interfacorType) << " index = [" << index << "] is out of range (max index = [" << uint32_t(
										m_interfacors.at(interfacorType).size() - 1) << "])", ErrorType::OutOfBound);

				typeID = m_interfacors.at(interfacorType)[index]->m_typeID;
				return true;
			}

			bool getInterfacorType(const EBoxInterfacorType interfacorType, const CIdentifier& identifier, CIdentifier& typeID) const override
			{
				const auto it = m_interfacorIDToIdx.at(interfacorType).find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_interfacorIDToIdx.at(interfacorType).end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << " with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->getInterfacorType(interfacorType, it->second, typeID);
			}

			bool getInterfacorType(const EBoxInterfacorType interfacorType, const CString& name, CIdentifier& typeID) const override
			{
				const auto it = m_interfacorNameToIdx.at(interfacorType).find(name);
				OV_ERROR_UNLESS_KRF(it != m_interfacorNameToIdx.at(interfacorType).end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << " with name " << name, ErrorType::ResourceNotFound);

				return this->getInterfacorType(interfacorType, it->second, typeID);
			}


			bool getInterfacorName(const EBoxInterfacorType interfacorType, const uint32_t index, CString& name) const override
			{
				OV_ERROR_UNLESS_KRF(index < m_interfacors.at(interfacorType).size(),
									g_InterfacorTypeToName.at(interfacorType) << " index = [" << index << "] is out of range (max index = [" << uint32_t(
										m_interfacors.at(interfacorType).size() - 1) << "])",
									ErrorType::OutOfBound);

				name = m_interfacors.at(interfacorType)[index]->m_name;
				return true;
			}

			bool getInterfacorName(const EBoxInterfacorType interfacorType, const CIdentifier& identifier, CString& name) const override
			{
				const auto it = m_interfacorIDToIdx.at(interfacorType).find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_interfacorIDToIdx.at(interfacorType).end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << " with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->getInputName(it->second, name);
			}

			bool getInterfacorDeprecatedStatus(const EBoxInterfacorType interfacorType, const uint32_t index, bool& value) const override
			{
				if (index >= m_interfacors.at(interfacorType).size()) { OV_WARNING_K("DUH"); }
				OV_ERROR_UNLESS_KRF(index < m_interfacors.at(interfacorType).size(),
									g_InterfacorTypeToName.at(interfacorType) << " index = [" << index << "] is out of range (max index = [" << uint32_t(
										m_interfacors.at(interfacorType).size() - 1) << "])",
									ErrorType::OutOfBound);

				value = m_interfacors.at(interfacorType)[index]->m_bDeprecated;
				return true;
			}

			bool getInterfacorDeprecatedStatus(const EBoxInterfacorType interfacorType, const CIdentifier& identifier, bool& value) const override
			{
				const auto it = m_interfacorIDToIdx.at(interfacorType).find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_interfacorIDToIdx.at(interfacorType).end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << " with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->getInterfacorDeprecatedStatus(interfacorType, it->second, value);
			}

			bool hasInterfacorWithIdentifier(const EBoxInterfacorType interfacorType, const CIdentifier& identifier) const override { return m_interfacorIDToIdx.at(interfacorType).find(identifier) != m_interfacorIDToIdx.at(interfacorType).end(); }

			bool hasInterfacorWithNameAndType(const EBoxInterfacorType interfacorType, const CString& name, const CIdentifier& /*typeID*/) const override { return m_interfacorNameToIdx.at(interfacorType).find(name) != m_interfacorNameToIdx.at(interfacorType).end(); }

			bool hasInterfacorWithType(const EBoxInterfacorType interfacorType, const uint32_t index, const CIdentifier& typeID) const override
			{
				if (index < this->getInterfacorCount(interfacorType))
				{
					CIdentifier type = OV_UndefinedIdentifier;
					this->getInterfacorType(interfacorType, index, type);
					return (type == typeID);
				}
				return false;
			}


			bool setInterfacorType(const EBoxInterfacorType interfacorType, const uint32_t index, const CIdentifier& typeID) override
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

				OV_ERROR_UNLESS_KRF(index < m_interfacors.at(interfacorType).size(),
									g_InterfacorTypeToName.at(interfacorType) << " index = [" << index << "] is out of range (max index = [" << uint32_t(
										m_interfacors.at(interfacorType).size() - 1) << "])",
									ErrorType::OutOfBound);

				if (m_interfacors[interfacorType][index]->m_typeID == typeID) { return true; }

				m_interfacors[interfacorType][index]->m_typeID = typeID;

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

			bool setInterfacorType(const EBoxInterfacorType interfacorType, const CIdentifier& identifier, const CIdentifier& typeID) override
			{
				const auto it = m_interfacorIDToIdx[interfacorType].find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_interfacorIDToIdx[interfacorType].end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << " with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->setInterfacorType(interfacorType, it->second, typeID);
			}

			bool setInterfacorType(const EBoxInterfacorType interfacorType, const CString& name, const CIdentifier& typeID) override
			{
				const auto it = m_interfacorNameToIdx[interfacorType].find(name);
				OV_ERROR_UNLESS_KRF(it != m_interfacorNameToIdx[interfacorType].end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << " with name " << name, ErrorType::ResourceNotFound);

				return this->setInterfacorType(interfacorType, it->second, typeID);
			}


			bool setInterfacorName(const EBoxInterfacorType interfacorType, const uint32_t index, const CString& newName) override
			{
				OV_ERROR_UNLESS_KRF(index < m_interfacors[interfacorType].size(),
									g_InterfacorTypeToName.at(interfacorType) << " index = [" << index << "] is out of range (max index = [" << uint32_t(
										m_interfacors[interfacorType].size() - 1) << "])",
									ErrorType::OutOfBound);

				if (m_interfacors[interfacorType][index]->m_name == newName)
				{
					// no change, don't bother notifying
					return true;
				}

				// remove entry from name key map
				const auto it = m_interfacorNameToIdx[interfacorType].find(m_interfacors[interfacorType][index]->m_name);
				OV_ERROR_UNLESS_KRF(it != m_interfacorNameToIdx[interfacorType].end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << " with name " << m_interfacors[interfacorType][index]->
									m_name,
									ErrorType::ResourceNotFound);
				m_interfacorNameToIdx[interfacorType].erase(it);

				// check for duplicated name key and update if necessary
				const CString uniqueName                          = this->getUnusedName(m_interfacorNameToIdx[interfacorType], newName);
				m_interfacorNameToIdx[interfacorType][uniqueName] = index;
				m_interfacors[interfacorType][index]->m_name      = uniqueName;

				OV_ERROR_UNLESS_KRF(m_interfacorNameToIdx[interfacorType].size() == m_interfacors[interfacorType].size(),
									"Box " << m_name << " has corrupted name map storage", ErrorType::BadResourceCreation);

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

			bool setInterfacorName(const EBoxInterfacorType interfacorType, const CIdentifier& identifier, const CString& newName) override
			{
				const auto it = m_interfacorIDToIdx[interfacorType].find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_interfacorIDToIdx[interfacorType].end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << " with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->setInterfacorName(interfacorType, it->second, newName);
			}

			bool setInterfacorDeprecatedStatus(const EBoxInterfacorType interfacorType, const uint32_t index, const bool newValue) override
			{
				OV_ERROR_UNLESS_KRF(index < m_interfacors[interfacorType].size(),
									g_InterfacorTypeToName.at(interfacorType) << " index = [" << index << "] is out of range (max index = [" << uint32_t(
										m_interfacors[interfacorType].size() - 1) << "])",
									ErrorType::OutOfBound);

				m_interfacors[interfacorType][index]->m_bDeprecated = newValue;

				return true;
			}

			bool setInterfacorDeprecatedStatus(const EBoxInterfacorType interfacorType, const CIdentifier& identifier, const bool newValue) override
			{
				const auto it = m_interfacorIDToIdx[interfacorType].find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_interfacorIDToIdx[interfacorType].end(),
									"Failed to find " << g_InterfacorTypeToName.at(interfacorType) << "  with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->setInterfacorDeprecatedStatus(interfacorType, it->second, newValue);
			}

			//___________________________________________________________________//
			//                                                                   //

			bool addInput(const CString& name, const CIdentifier& typeID, const CIdentifier& identifier, const bool bNotify) override { return this->addInterfacor(Input, name, typeID, identifier, bNotify); }

			bool removeInput(const uint32_t index, const bool bNotify = true) override
			{
				OV_ERROR_UNLESS_KRF(index < m_interfacors[Input].size(),
									"Input index = [" << index << "] is out of range (max index = [" << uint32_t(m_interfacors[Input].size() - 1) << "])",
									ErrorType::OutOfBound);


				{
					CIdentifier* listID = nullptr;
					size_t nbElems      = 0;
					m_ownerScenario->getLinkIdentifierToBoxInputList(m_identifier, index, &listID, &nbElems);
					for (size_t i = 0; i < nbElems; ++i) { m_ownerScenario->disconnect(listID[i]); }
					m_ownerScenario->releaseIdentifierList(listID);
				}

				// $$$
				// The way the links are removed here
				// is not correct because they are all
				// collected and then all removed. In case
				// the box listener callback on box removal,
				// the nextcoming links would potentially be
				// invalid
				std::vector<std::pair<std::pair<uint64_t, uint32_t>, std::pair<uint64_t, uint32_t>>> links;

				{
					CIdentifier* listID = nullptr;
					size_t nbElems      = 0;
					m_ownerScenario->getLinkIdentifierToBoxList(m_identifier, &listID, &nbElems);
					for (size_t i = 0; i < nbElems; ++i)
					{
						CIdentifier id = listID[i];
						ILink* link    = m_ownerScenario->getLinkDetails(id);
						if (link->getTargetBoxInputIndex() > index)
						{
							links.push_back(std::make_pair(std::make_pair(link->getSourceBoxIdentifier().toUInteger(), link->getSourceBoxOutputIndex()),
														   std::make_pair(link->getTargetBoxIdentifier().toUInteger(), link->getTargetBoxInputIndex())));

							if (m_ownerScenario->isLink(id)) { m_ownerScenario->disconnect(id); }
						}
					}
					m_ownerScenario->releaseIdentifierList(listID);
				}

				// This reorganizes the parent's scenario links if this box is not actually a scenario itself
				if (m_identifier != OV_UndefinedIdentifier)
				{
					std::vector<std::pair<uint32_t, std::pair<uint64_t, uint32_t>>> scenarioLinks;
					for (uint32_t scenarioInputIdx = 0; scenarioInputIdx < m_ownerScenario->getInterfacorCount(Input); scenarioInputIdx++)
					{
						CIdentifier boxID        = OV_UndefinedIdentifier;
						uint32_t boxConnectorIdx = uint32_t(-1);
						m_ownerScenario->getScenarioInputLink(scenarioInputIdx, boxID, boxConnectorIdx);
						if (boxID == m_identifier)
						{
							if (boxConnectorIdx > index) { scenarioLinks.push_back(std::make_pair(scenarioInputIdx, std::make_pair(boxID.toUInteger(), boxConnectorIdx))); }
							if (boxConnectorIdx >= index) { m_ownerScenario->removeScenarioInputLink(scenarioInputIdx, boxID, boxConnectorIdx); }
						}
					}

					// Reconnects scenario links
					for (const auto& link : scenarioLinks) { m_ownerScenario->setScenarioInputLink(link.first, link.second.first, link.second.second - 1); }
				}

				const CIdentifier toBeRemovedId = m_interfacors[Input][index]->m_id;
				CString toBeRemovedName         = m_interfacors[Input][index]->m_name;

				// Erases actual input
				m_interfacors[Input].erase(m_interfacors[Input].begin() + index);

				// Reconnects box links
				for (const auto& link : links)
				{
					CIdentifier newId = OV_UndefinedIdentifier;
					m_ownerScenario->connect(newId, link.first.first, link.first.second, link.second.first, link.second.second - 1, OV_UndefinedIdentifier);
				}

				// erase name key
				const auto itName = m_interfacorNameToIdx[Input].find(toBeRemovedName);
				OV_ERROR_UNLESS_KRF(itName != m_interfacorNameToIdx[Input].end(), "No input found with name " << toBeRemovedName,
									ErrorType::ResourceNotFound);
				m_interfacorNameToIdx[Input].erase(itName);

				// erase identifier key if defined
				if (toBeRemovedId != OV_UndefinedIdentifier)
				{
					const auto itIdent = m_interfacorIDToIdx[Input].find(toBeRemovedId);
					OV_ERROR_UNLESS_KRF(itIdent != m_interfacorIDToIdx[Input].end(), "No input found with id " << toBeRemovedId.toString(),
										ErrorType::ResourceNotFound);
					m_interfacorIDToIdx[Input].erase(itIdent);
				}

				if (bNotify) { this->notify(BoxModification_InputRemoved, index); }

				return true;
			}

			uint32_t getInputCount() const override { return this->getInterfacorCount(Input); }
			bool getInputType(const uint32_t index, CIdentifier& typeID) const override { return this->getInterfacorType(Input, index, typeID); }
			bool getInputName(const uint32_t index, CString& name) const override { return this->getInterfacorName(Input, index, name); }

			bool getInputName(const CIdentifier& rInputIdentifier, CString& name) const { return this->getInterfacorName(Input, rInputIdentifier, name); }

			bool setInputType(const uint32_t index, const CIdentifier& typeID) override { return this->setInterfacorType(Input, index, typeID); }
			bool setInputName(const uint32_t index, const CString& name) override { return this->setInterfacorName(Input, index, name); }

			//___________________________________________________________________//
			//                                                                   //

			bool addOutput(const CString& name, const CIdentifier& typeID, const CIdentifier& identifier, const bool bNotify) override { return this->addInterfacor(Output, name, typeID, identifier, bNotify); }

			bool removeOutput(const uint32_t index, const bool bNotify = true) override
			{
				OV_ERROR_UNLESS_KRF(index < m_interfacors[Output].size(),
									"Output index = [" << index << "] is out of range (max index = [" << uint32_t(m_interfacors[Output].size() - 1) << "])",
									ErrorType::OutOfBound);

				std::vector<std::pair<std::pair<uint64_t, uint32_t>, std::pair<uint64_t, uint32_t>>> links;

				if (m_ownerScenario)
				{
					CIdentifier* listID = nullptr;
					size_t nbElems      = 0;

					m_ownerScenario->getLinkIdentifierFromBoxOutputList(m_identifier, index, &listID, &nbElems);
					for (size_t i = 0; i < nbElems; ++i) { m_ownerScenario->disconnect(listID[i]); }
					m_ownerScenario->releaseIdentifierList(listID);

					// $$$
					// The way the links are removed here is not correct because they are all collected and then all removed. In case
					// the box listener callback on box removal, the nextcoming links would potentially be invalid
					{
						listID  = nullptr;
						nbElems = 0;
						m_ownerScenario->getLinkIdentifierFromBoxOutputList(m_identifier, index, &listID, &nbElems);
						for (size_t i = 0; i < nbElems; ++i)
						{
							const CIdentifier& curID = listID[i];
							ILink* link              = m_ownerScenario->getLinkDetails(curID);
							if (link->getSourceBoxOutputIndex() > index)
							{
								links.push_back(std::make_pair(std::make_pair(link->getSourceBoxIdentifier().toUInteger(), link->getSourceBoxOutputIndex()),
															   std::make_pair(link->getTargetBoxIdentifier().toUInteger(), link->getTargetBoxInputIndex())));
								if (m_ownerScenario->isLink(curID)) { m_ownerScenario->disconnect(curID); }
							}
						}
						m_ownerScenario->releaseIdentifierList(listID);
					}

					// This reorganizes the parent's scenario links if this box is not actually a scenario
					if (m_identifier != OV_UndefinedIdentifier)
					{
						std::vector<std::pair<uint32_t, std::pair<uint64_t, uint32_t>>> scenarioLinks;
						for (uint32_t scenarioOutputIdx = 0; scenarioOutputIdx < m_ownerScenario->getOutputCount(); scenarioOutputIdx++)
						{
							CIdentifier boxID        = OV_UndefinedIdentifier;
							uint32_t boxConnectorIdx = uint32_t(-1);
							m_ownerScenario->getScenarioOutputLink(scenarioOutputIdx, boxID, boxConnectorIdx);
							if (boxID == m_identifier)
							{
								if (boxConnectorIdx > index) { scenarioLinks.push_back(std::make_pair(scenarioOutputIdx, std::make_pair(boxID.toUInteger(), boxConnectorIdx))); }
								if (boxConnectorIdx >= index) { m_ownerScenario->removeScenarioOutputLink(scenarioOutputIdx, boxID, boxConnectorIdx); }
							}
						}

						// Reconnects scenario links
						for (const auto& link : scenarioLinks) { m_ownerScenario->setScenarioOutputLink(link.first, link.second.first, link.second.second - 1); }
					}
				}
				const CIdentifier toBeRemovedId = m_interfacors.at(Output)[index]->m_id;
				CString toBeRemovedName         = m_interfacors.at(Output)[index]->m_name;

				// Erases actual output
				m_interfacors[Output].erase(m_interfacors.at(Output).begin() + index);

				// Reconnects box links
				if (m_ownerScenario)
				{
					for (const auto& link : links)
					{
						CIdentifier newId = OV_UndefinedIdentifier;
						m_ownerScenario->connect(newId, link.first.first, link.first.second - 1, link.second.first, link.second.second,
												 OV_UndefinedIdentifier);
					}
				}


				// erase name key
				const auto itName = m_interfacorNameToIdx.at(Output).find(toBeRemovedName);
				OV_ERROR_UNLESS_KRF(itName != m_interfacorNameToIdx.at(Output).end(), "No output found with name " << toBeRemovedName,
									ErrorType::ResourceNotFound);
				m_interfacorNameToIdx.at(Output).erase(itName);

				// erase identifier key if defined
				if (toBeRemovedId != OV_UndefinedIdentifier)
				{
					const auto itIdent = m_interfacorIDToIdx.at(Output).find(toBeRemovedId);
					OV_ERROR_UNLESS_KRF(itIdent != m_interfacorIDToIdx.at(Output).end(), "No output found with id " << toBeRemovedId.toString(),
										ErrorType::ResourceNotFound);

					m_interfacorIDToIdx.at(Output).erase(itIdent);
				}

				if (bNotify) { this->notify(BoxModification_OutputRemoved, index); }

				return true;
			}

			uint32_t getOutputCount() const override { return this->getInterfacorCount(Output); }
			bool getOutputType(const uint32_t index, CIdentifier& typeID) const override { return this->getInterfacorType(Output, index, typeID); }
			bool getOutputName(const uint32_t index, CString& name) const override { return this->getInterfacorName(Output, index, name); }
			bool setOutputType(const uint32_t index, const CIdentifier& typeID) override { return this->setInterfacorType(Output, index, typeID); }
			bool setOutputName(const uint32_t index, const CString& name) override { return this->setInterfacorName(Output, index, name); }

			bool addInterfacorTypeSupport(const EBoxInterfacorType interfacorType, const CIdentifier& typeID) override
			{
				if (interfacorType == Input) { m_supportInputTypes.push_back(typeID); }
				else if (interfacorType == Output) { m_supportOutputTypes.push_back(typeID); }

				return false;
			}

			bool hasInterfacorTypeSupport(const EBoxInterfacorType interfacorType, const CIdentifier& typeID) const override
			{
				if (interfacorType == Input)
				{
					if (m_supportInputTypes.empty()) { return true; }

					for (size_t i = 0; i < m_supportInputTypes.size(); ++i) { if (m_supportInputTypes[i] == typeID) { return true; } }
				}
				else if (interfacorType == Output)
				{
					//If there is no type specify, we allow all
					if (m_supportOutputTypes.empty()) { return true; }

					for (size_t i = 0; i < m_supportOutputTypes.size(); ++i) { if (m_supportOutputTypes[i] == typeID) { return true; } }
				}
				else
				{
					// Settings are always all supported, in a way
					return true;
				}

				return false;
			}

			bool addInputSupport(const CIdentifier& typeID) override { return this->addInterfacorTypeSupport(Input, typeID); }
			bool hasInputSupport(const CIdentifier& typeID) const override { return this->hasInterfacorTypeSupport(Input, typeID); }
			bool addOutputSupport(const CIdentifier& typeID) override { return this->addInterfacorTypeSupport(Output, typeID); }
			bool hasOutputSupport(const CIdentifier& typeID) const override { return this->hasInterfacorTypeSupport(Output, typeID); }

			bool setSupportTypeFromAlgorithmIdentifier(const CIdentifier& typeID) override
			{
				const Plugins::IPluginObjectDesc* l_pPluginObjectDescriptor = this->getKernelContext().getPluginManager().getPluginObjectDescCreating(typeID);
				const Plugins::IBoxAlgorithmDesc* l_pBoxAlgorithmDescriptor = dynamic_cast<const Plugins::IBoxAlgorithmDesc*>(l_pPluginObjectDescriptor);

				OV_ERROR_UNLESS_KRF(l_pBoxAlgorithmDescriptor, "Tried to initialize with an unregistered algorithm", ErrorType::Internal);

				//We use the neutralized version of CBoxProto to just initialize the stream restriction mecanism
				CBoxProtoRestriction oTempProto(this->getKernelContext(), *this);
				l_pBoxAlgorithmDescriptor->getBoxPrototype(oTempProto);
				return true;
			}

			std::vector<CIdentifier> getInputSupportTypes() const override { return m_supportInputTypes; }
			std::vector<CIdentifier> getOutputSupportTypes() const override { return m_supportOutputTypes; }
			void clearOutputSupportTypes() override { m_supportOutputTypes.clear(); }
			void clearInputSupportTypes() override { m_supportInputTypes.clear(); }

		private:
			CIdentifier getUnusedInterfacorIdentifier(const EBoxInterfacorType interfacorType, const CIdentifier& suggestedIdentifier = OV_UndefinedIdentifier) const
			{
				uint64_t identifier = System::Math::randomUInteger64();
				if (suggestedIdentifier != OV_UndefinedIdentifier) { identifier = suggestedIdentifier.toUInteger(); }

				CIdentifier resultIdentifier = OV_UndefinedIdentifier;
				std::map<CIdentifier, uint32_t>::const_iterator it;
				do
				{
					resultIdentifier = CIdentifier(identifier);
					it               = m_interfacorIDToIdx.at(interfacorType).find(resultIdentifier);
					identifier++;
				} while (it != m_interfacorIDToIdx.at(interfacorType).end() || resultIdentifier == OV_UndefinedIdentifier);
				return resultIdentifier;
			}

		public:

			CIdentifier getUnusedSettingIdentifier(const CIdentifier& /*suggestedID*/ = OV_UndefinedIdentifier) const { return this->getUnusedInterfacorIdentifier(Setting); }

			CIdentifier getUnusedInputIdentifier(const CIdentifier& /*suggestedID*/ = OV_UndefinedIdentifier) const { return this->getUnusedInterfacorIdentifier(Input); }

			CIdentifier getUnusedOutputIdentifier(const CIdentifier& /*suggestedID*/ = OV_UndefinedIdentifier) const { return this->getUnusedInterfacorIdentifier(Output); }

			bool addSetting(const CString& name, const CIdentifier& typeID, const CString& sDefaultValue, const uint32_t index,
							const bool bModifiability, const CIdentifier& identifier, const bool bNotify) override
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
							CIdentifier l_oDefaultValueID = OV_UndefinedIdentifier;
							l_oDefaultValueID.fromString(sDefaultValue);

							// Finally, if it is an identifier, then a name should be found
							// from the type manager ! Otherwise l_sValue is left to the default.
							const CString l_sCandidateValue = this->getTypeManager().getEnumerationEntryNameFromValue(typeID, l_oDefaultValueID.toUInteger());
							if (l_sCandidateValue != CString("")) { l_sValue = l_sCandidateValue; }
						}
					}
				}

				CSetting s;
				s.m_name            = name;
				s.m_typeID = typeID;
				s.m_defaultValue   = l_sValue;
				s.m_value          = l_sValue;
				s.m_bMod            = bModifiability;
				s.m_id     = identifier;

				const uint32_t l_ui32Idx = index;


				uint32_t l_ui32InsertLocation;

				if (index == OV_Value_UndefinedIndexUInt || index == uint32_t(m_interfacors[Setting].size()))
				{
					m_interfacors[Setting].push_back(std::make_shared<CSetting>(s));
					l_ui32InsertLocation = (uint32_t(m_interfacors[Setting].size())) - 1;
				}
				else
				{
					OV_ERROR_UNLESS_KRF(index <= uint32_t(m_interfacors[Setting].size()),
										"Tried to push '" << name << "' to slot " << index << " with the array size being " << uint32_t(m_interfacors[Setting].size()),
										ErrorType::OutOfBound);

					auto l_it = m_interfacors[Setting].begin();
					l_it += l_ui32Idx;
					m_interfacors[Setting].insert(l_it, std::make_shared<CSetting>(s));
					l_ui32InsertLocation = index;
				}

				if (s.m_id != OV_UndefinedIdentifier)
				{
					// add access by CIdentifier key if defined so that size differs from m_settings
					m_interfacorIDToIdx[Setting][s.m_id] = l_ui32InsertLocation;
				}
				// add access by name key (always done so that synchronized with m_settings
				const CString newName                                = this->getUnusedName(m_interfacorNameToIdx.at(Setting), s.m_name);
				m_interfacors[Setting][l_ui32InsertLocation]->m_name = newName;
				m_interfacorNameToIdx[Setting][newName]              = l_ui32InsertLocation;

				OV_ERROR_UNLESS_KRF(m_interfacorNameToIdx.at(Setting).size() == m_interfacors[Setting].size(),
									"Box " << m_name << " has corrupted name map storage", ErrorType::BadResourceCreation);

				//if this setting is modifiable, keep its index
				if (bModifiability) { m_modifiableSettingIndexes.push_back(l_ui32Idx); }

				this->getLogManager() << LogLevel_Debug
						<< "Pushed '" << m_interfacors.at(Setting)[l_ui32InsertLocation]->m_name << "' : '"
						<< std::static_pointer_cast<CSetting>(m_interfacors.at(Setting)[l_ui32InsertLocation])->m_value
						<< "' to slot " << l_ui32InsertLocation << " with the array size now " << int(m_interfacors.at(Setting).size()) << "\n";

				if (bNotify)
				{
					this->notify(BoxModification_SettingAdded, l_ui32InsertLocation);
					this->notifySettingChange(SettingAdd, l_ui32InsertLocation);
				}

				return true;
			}

			bool removeSetting(const uint32_t index, const bool bNotify = true) override
			{
				auto it = m_interfacors[Setting].begin() + index;
				OV_ERROR_UNLESS_KRF(it != m_interfacors[Setting].end(), "No setting found at index " << index, ErrorType::ResourceNotFound);

				const CIdentifier toBeRemovedId = m_interfacors[Setting][index]->m_id;
				CString toBeRemovedName         = m_interfacors[Setting][index]->m_name;

				it = m_interfacors[Setting].erase(it);

				//update the modifiable setting indexes
				for (auto it2 = m_modifiableSettingIndexes.begin(); it2 != m_modifiableSettingIndexes.end();)
				{
					if (*it2 == index) { it2 = m_modifiableSettingIndexes.erase(it2); }
					else if (*it2 > index)
					{
						*it2 -= 1;
						++it2;
					}
				}

				// erase name key
				const auto itName = m_interfacorNameToIdx.at(Setting).find(toBeRemovedName);
				OV_ERROR_UNLESS_KRF(itName != m_interfacorNameToIdx.at(Setting).end(), "No setting found with name " << toBeRemovedName,
									ErrorType::ResourceNotFound);
				m_interfacorNameToIdx.at(Setting).erase(itName);

				// erase identifier key if defined
				if (toBeRemovedId != OV_UndefinedIdentifier)
				{
					const auto itIdent = m_interfacorIDToIdx.at(Setting).find(toBeRemovedId);
					OV_ERROR_UNLESS_KRF(itIdent != m_interfacorIDToIdx.at(Setting).end(), "No setting found with id " << toBeRemovedId.toString(),
										ErrorType::ResourceNotFound);
					m_interfacorIDToIdx.at(Setting).erase(itIdent);
				}

				if (bNotify)
				{
					this->notify(BoxModification_SettingRemoved, index);
					this->notifySettingChange(SettingDelete, index);
				}

				return true;
			}

			uint32_t getSettingCount() const override { return this->getInterfacorCount(Setting); }

			bool hasSettingWithName(const CString& name) const override { return m_interfacorNameToIdx.at(Setting).find(name) != m_interfacorNameToIdx.at(Setting).end(); }

			bool getSettingType(const uint32_t index, CIdentifier& typeID) const override { return this->getInterfacorType(Setting, index, typeID); }

			bool getSettingName(const uint32_t index, CString& name) const override { return this->getInterfacorName(Setting, index, name); }

			bool getSettingDefaultValue(const uint32_t index, CString& rDefaultValue) const override
			{
				OV_ERROR_UNLESS_KRF(index < m_interfacors.at(Setting).size(),
									"Setting index = [" << index << "] is out of range (max index = [" << uint32_t(m_interfacors.at(Setting).size() - 1) <<
									"])",
									ErrorType::OutOfBound);

				rDefaultValue = std::static_pointer_cast<CSetting>(m_interfacors.at(Setting)[index])->m_defaultValue;
				return true;
			}

			bool getSettingDefaultValue(const CIdentifier& identifier, CString& rDefaultValue) const override
			{
				const auto it = m_interfacorIDToIdx.at(Setting).find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_interfacorIDToIdx.at(Setting).end(), "Failed to find setting with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->getSettingDefaultValue(it->second, rDefaultValue);
			}

			bool getSettingDefaultValue(const CString& name, CString& rDefaultValue) const override
			{
				const auto it = m_interfacorNameToIdx.at(Setting).find(name);
				OV_ERROR_UNLESS_KRF(it != m_interfacorNameToIdx.at(Setting).end(), "Failed to find setting with name " << name,
									ErrorType::ResourceNotFound);

				return this->getSettingDefaultValue(it->second, rDefaultValue);
			}

			bool getSettingValue(const uint32_t index, CString& rValue) const override
			{
				OV_ERROR_UNLESS_KRF(index < m_interfacors.at(Setting).size(),
									"Setting index = [" << index << "] is out of range (max index = [" << uint32_t(m_interfacors.at(Setting).size() - 1) <<
									"])",
									ErrorType::OutOfBound);

				rValue = std::static_pointer_cast<CSetting>(m_interfacors.at(Setting)[index])->m_value;
				return true;
			}

			bool getSettingValue(const CIdentifier& identifier, CString& rValue) const override
			{
				const auto it = m_interfacorIDToIdx.at(Setting).find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_interfacorIDToIdx.at(Setting).end(), "Failed to find setting with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->getSettingValue(it->second, rValue);
			}

			bool getSettingValue(const CString& name, CString& rValue) const override
			{
				const auto it = m_interfacorNameToIdx.at(Setting).find(name);
				OV_ERROR_UNLESS_KRF(it != m_interfacorNameToIdx.at(Setting).end(), "Failed to find setting with name " << name,
									ErrorType::ResourceNotFound);

				return this->getSettingValue(it->second, rValue);
			}

			bool getSettingMod(const uint32_t index, bool& rValue) const override
			{
				OV_ERROR_UNLESS_KRF(index < m_interfacors.at(Setting).size(),
									"Setting index = [" << index << "] is out of range (max index = [" << uint32_t(m_interfacors.at(Setting).size() - 1) <<
									"])",
									ErrorType::OutOfBound);

				rValue = std::static_pointer_cast<CSetting>(m_interfacors.at(Setting)[index])->m_bMod;
				return true;
			}

			bool getSettingMod(const CIdentifier& identifier, bool& rValue) const override
			{
				const auto it = m_interfacorIDToIdx.at(Setting).find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_interfacorIDToIdx.at(Setting).end(),
									"Failed to find setting with id " << identifier.toString(), ErrorType::ResourceNotFound);

				return this->getSettingMod(it->second, rValue);
			}

			bool getSettingMod(const CString& name, bool& rValue) const override
			{
				const auto it = m_interfacorNameToIdx.at(Setting).find(name);
				OV_ERROR_UNLESS_KRF(it != m_interfacorNameToIdx.at(Setting).end(),
									"Failed to find setting with name " << name, ErrorType::ResourceNotFound);

				return this->getSettingMod(it->second, rValue);
			}

			bool setSettingType(const uint32_t index, const CIdentifier& typeID) override { return this->setInterfacorType(Setting, index, typeID); }

			bool setSettingName(const uint32_t index, const CString& name) override { return this->setInterfacorName(Setting, index, name); }

			bool setSettingDefaultValue(const uint32_t index, const CString& rDefaultValue) override
			{
				OV_ERROR_UNLESS_KRF(index < m_interfacors.at(Setting).size(),
									"Setting index = [" << index << "] is out of range (max index = [" << uint32_t(m_interfacors.at(Setting).size() - 1) <<
									"])",
									ErrorType::OutOfBound);

				std::static_pointer_cast<CSetting>(m_interfacors[Setting][index])->m_defaultValue = rDefaultValue;

				this->notify(BoxModification_SettingDefaultValueChanged, index);

				return true;
			}

			bool setSettingDefaultValue(const CIdentifier& identifier, const CString& rDefaultValue) override
			{
				const auto it = m_interfacorIDToIdx.at(Setting).find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_interfacorIDToIdx.at(Setting).end(), "Failed to find setting with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->setSettingDefaultValue(it->second, rDefaultValue);
			}

			bool setSettingDefaultValue(const CString& name, const CString& rDefaultValue) override
			{
				const auto it = m_interfacorNameToIdx.at(Setting).find(name);
				OV_ERROR_UNLESS_KRF(it != m_interfacorNameToIdx.at(Setting).end(), "Failed to find setting with name " << name,
									ErrorType::ResourceNotFound);
				return this->setSettingDefaultValue(it->second, rDefaultValue);
			}

			bool setSettingValue(const uint32_t index, const CString& rValue, const bool bNotify = true) override
			{
				OV_ERROR_UNLESS_KRF(index < m_interfacors.at(Setting).size(),
									"Setting index = [" << index << "] is out of range (max index = [" << uint32_t(m_interfacors.at(Setting).size() - 1) <<
									"])",
									ErrorType::OutOfBound);

				auto setting = std::static_pointer_cast<CSetting>(m_interfacors[Setting][index]);
				if (setting->m_value != rValue)
				{
					setting->m_value = rValue;

					if (bNotify)
					{
						this->notify(BoxModification_SettingValueChanged, index);
						this->notifySettingChange(SettingValueUpdate, index);
					}
				}

				return true;
			}

			bool setSettingValue(const CIdentifier& identifier, const CString& rValue) override
			{
				const auto it = m_interfacorIDToIdx.at(Setting).find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_interfacorIDToIdx.at(Setting).end(), "Failed to find setting with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->setSettingValue(it->second, rValue);
			}

			bool setSettingValue(const CString& name, const CString& rValue) override
			{
				const auto it = m_interfacorNameToIdx.at(Setting).find(name);
				OV_ERROR_UNLESS_KRF(it != m_interfacorNameToIdx.at(Setting).end(), "Failed to find setting with name " << name,
									ErrorType::ResourceNotFound);

				return this->setSettingValue(it->second, rValue);
			}

			bool setSettingMod(const uint32_t index, const bool rValue) override
			{
				OV_ERROR_UNLESS_KRF(index < m_interfacors.at(Setting).size(),
									"Setting index = [" << index << "] is out of range (max index = [" << uint32_t(m_interfacors.at(Setting).size() - 1) <<
									"])",
									ErrorType::OutOfBound);

				std::static_pointer_cast<CSetting>(m_interfacors[Setting][index])->m_bMod = rValue;

				//this->notify(BoxModification_SettingNameChanged, index);
				return true;
			}

			bool setSettingMod(const CString& name, const bool rValue) override
			{
				const auto it = m_interfacorNameToIdx.at(Setting).find(name);
				OV_ERROR_UNLESS_KRF(it != m_interfacorNameToIdx.at(Setting).end(), "Failed to find setting with name " << name,
									ErrorType::ResourceNotFound);

				return this->setSettingMod(it->second, rValue);
			}

			bool setSettingMod(const CIdentifier& identifier, const bool rValue) override
			{
				const auto it = m_interfacorIDToIdx.at(Setting).find(identifier);
				OV_ERROR_UNLESS_KRF(it != m_interfacorIDToIdx.at(Setting).end(), "Failed to find setting with id " << identifier.toString(),
									ErrorType::ResourceNotFound);

				return this->setSettingMod(it->second, rValue);
			}

			bool swapInterfacors(const EBoxInterfacorType interfacorType, const uint32_t indexA, const uint32_t indexB)
			{
				OV_ERROR_UNLESS_KRF(indexA < m_interfacors.at(interfacorType).size(),
									g_InterfacorTypeToName.at(interfacorType) << " index = [" << indexA << "] is out of range (max index = [" << uint32_t(
										m_interfacors.at(interfacorType).size() - 1) << "])",
									ErrorType::OutOfBound);
				OV_ERROR_UNLESS_KRF(indexB < m_interfacors.at(interfacorType).size(),
									g_InterfacorTypeToName.at(interfacorType) << " index = [" << indexB << "] is out of range (max index = [" << uint32_t(
										m_interfacors.at(interfacorType).size() - 1) << "])",
									ErrorType::OutOfBound);

				CString nameA;
				CIdentifier identifierA = OV_UndefinedIdentifier;
				CString nameB;
				CIdentifier identifierB = OV_UndefinedIdentifier;

				this->getInterfacorName(interfacorType, indexA, nameA);
				this->getInterfacorIdentifier(interfacorType, indexA, identifierA);
				this->getInterfacorName(interfacorType, indexB, nameB);
				this->getInterfacorIdentifier(interfacorType, indexB, identifierB);

				const auto itA = m_interfacors[interfacorType].begin() + indexA;
				const auto itB = m_interfacors[interfacorType].begin() + indexB;
				// swap settings
				std::iter_swap(itA, itB);
				// update associated maps
				m_interfacorNameToIdx.at(interfacorType)[nameA]     = indexB;
				m_interfacorNameToIdx.at(interfacorType)[nameB]     = indexA;
				m_interfacorIDToIdx.at(interfacorType)[identifierA] = indexB;
				m_interfacorIDToIdx.at(interfacorType)[identifierB] = indexA;

				return true;
			}

			bool swapSettings(const uint32_t indexA, const uint32_t indexB) override { return this->swapInterfacors(Setting, indexA, indexB); }
			bool swapInputs(const uint32_t indexA, const uint32_t indexB) override { return this->swapInterfacors(Input, indexA, indexB); }
			bool swapOutputs(const uint32_t indexA, const uint32_t indexB) override { return this->swapInterfacors(Output, indexA, indexB); }

			void notifySettingChange(const EBoxEventMessageType eType, const int firstIdx = -1, const int secondIdx = -1)
			{
				if (m_isObserverNotificationActive)
				{
					BoxEventMessage l_oEvent;
					l_oEvent.m_Type          = eType;
					l_oEvent.m_FirstIdx  = firstIdx;
					l_oEvent.m_SecondIdx = secondIdx;

					this->setChanged();
					this->notifyObservers(&l_oEvent);
				}
			}

			bool hasModifiableSettings() const override
			{
				for (const auto& setting : m_interfacors.at(Setting)) { if (std::static_pointer_cast<CSetting>(setting)->m_bMod) { return true; } }
				return false;
			}

			uint32_t* getModifiableSettings(uint32_t& rCount) const override
			{
				uint32_t* l_pReturn = nullptr;
				rCount              = uint32_t(m_modifiableSettingIndexes.size());

				return l_pReturn;
			}

			bool updateInterfacorIdentifier(const EBoxInterfacorType interfacorType, const uint32_t index, const CIdentifier& newID) override
			{
				OV_ERROR_UNLESS_KRF(index < m_interfacors.at(interfacorType).size(),
									g_InterfacorTypeToName.at(interfacorType) << " index = [" << index << "] is out of range (max index = [" << uint32_t(
										m_interfacors.at(Setting).size() - 1) << "])",
									ErrorType::OutOfBound);

				OV_ERROR_UNLESS_KRF(newID != OV_UndefinedIdentifier, g_InterfacorTypeToName.at(interfacorType) << " identifier can not be undefined",
									ErrorType::BadArgument);

				CIdentifier oldIdentifier = OV_UndefinedIdentifier;
				this->getInterfacorIdentifier(interfacorType, index, oldIdentifier);

				if (oldIdentifier != newID)
				{
					// identifier key update is necessary
					const auto it = m_interfacorIDToIdx.at(interfacorType).find(newID);
					OV_ERROR_UNLESS_KRF(it == m_interfacorIDToIdx.at(interfacorType).end(),
										"Conflict in " << g_InterfacorTypeToName.at(interfacorType) <<
										" identifiers. An entity with the same identifier exists.",
										ErrorType::ResourceNotFound);
					m_interfacors[interfacorType][index]->m_id = newID;
					m_interfacorIDToIdx[interfacorType][newID]          = index;
					// remove the old identifier key
					const auto itOld = m_interfacorIDToIdx[interfacorType].find(oldIdentifier);
					if (itOld != m_interfacorIDToIdx[interfacorType].end()) { m_interfacorIDToIdx[interfacorType].erase(itOld); }
				}
				return true;
			}

			//*/

			bool acceptVisitor(IObjectVisitor& rObjectVisitor) override
			{
				CObjectVisitorContext l_oObjectVisitorContext(this->getKernelContext());
				return rObjectVisitor.processBegin(l_oObjectVisitorContext, *this) && rObjectVisitor.processEnd(l_oObjectVisitorContext, *this);
			}

		protected:

			void clearBox()
			{
				m_boxAlgorithmDesc = nullptr;
				m_algorithmClassID = OV_UndefinedIdentifier;
				m_name             = "";
				m_interfacors[Input].clear();
				m_interfacors[Output].clear();
				m_interfacors[Setting].clear();
				m_interfacorIDToIdx[Input].clear();
				m_interfacorNameToIdx[Input].clear();
				m_interfacorIDToIdx[Output].clear();
				m_interfacorNameToIdx[Output].clear();
				m_interfacorIDToIdx.at(Setting).clear();
				m_interfacorNameToIdx.at(Setting).clear();

				this->removeAllAttributes();
			}

			void enableNotification() { m_isNotificationActive = true; }
			void disableNotification() { m_isNotificationActive = false; }

			void notify(const EBoxModification eBoxModificationType, const uint32_t index)
			{
				if (m_boxListener && !m_isNotifyingDesc && m_isNotificationActive)
				{
					CBoxListenerContext l_oContext(this->getKernelContext(), *this, index);
					m_isNotifyingDesc = true;
					m_boxListener->process(l_oContext, eBoxModificationType);
					m_isNotifyingDesc = false;
				}
			}

			void notify(const EBoxModification boxModification) { this->notify(boxModification, 0xffffffff); }

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

			IScenario* m_ownerScenario                           = nullptr;
			const Plugins::IBoxAlgorithmDesc* m_boxAlgorithmDesc = nullptr;
			Plugins::IBoxListener* m_boxListener                 = nullptr;
			bool m_isNotifyingDesc                               = false;
			bool m_isNotificationActive                          = true;
			bool m_isObserverNotificationActive                  = true;

			CIdentifier m_identifier       = OV_UndefinedIdentifier;
			CIdentifier m_algorithmClassID = OV_UndefinedIdentifier;
			CString m_name                 = "unnamed";

			std::map<EBoxInterfacorType, std::map<CIdentifier, uint32_t>> m_interfacorIDToIdx;
			std::map<EBoxInterfacorType, std::map<CString, uint32_t>> m_interfacorNameToIdx;

			//to avoid having to recheck every setting every time
			//careful to update at each setting modification
			std::vector<uint32_t> m_modifiableSettingIndexes;

			std::vector<CIdentifier> m_supportInputTypes;
			std::vector<CIdentifier> m_supportOutputTypes;
		private:
			std::map<EBoxInterfacorType, std::vector<std::shared_ptr<CInterfacor>>> m_interfacors;
		};
	} // namespace Kernel
} // namespace OpenViBE
