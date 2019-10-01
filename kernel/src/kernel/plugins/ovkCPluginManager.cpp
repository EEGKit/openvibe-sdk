#include "ovkCPluginManager.h"
#include "ovkCPluginModule.h"

#include "../../ovk_tools.h"

#include <fs/IEntryEnumerator.h>
#include <fs/Files.h>

#include <system/ovCMemory.h>

#include <cinttypes>
#include <cstdio>
#include <iostream>
#include <map>
#include <algorithm>

#include "../../tools/ovkSBoxProto.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;
using namespace std;

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginManagerEntryEnumeratorCallBack final : public TKernelObject<IObject>, public FS::IEntryEnumeratorCallBack
		{
		public:

			CPluginManagerEntryEnumeratorCallBack(const IKernelContext& ctx, vector<IPluginModule*>& rPluginModule,
												  map<IPluginObjectDesc*, IPluginModule*>& rPluginObjectDesc, bool& haveAllPluginsLoadedCorrectly)
				: TKernelObject<IObject>(ctx), m_rPluginManager(ctx.getPluginManager()), m_rPluginModule(rPluginModule),
				  m_rPluginObjectDesc(rPluginObjectDesc), m_HaveAllPluginsLoadedCorrectly(haveAllPluginsLoadedCorrectly) { }

			bool callback(FS::IEntryEnumerator::IEntry& rEntry, FS::IEntryEnumerator::IAttributes& /*rAttributes*/) override
			{
				for (auto& pluginModule : m_rPluginModule)
				{
					CString name;
					if (!pluginModule->getFileName(name)) { return true; }

					if (FS::Files::equals(rEntry.getName(), static_cast<const char*>(name)))
					{
						OV_WARNING_K("Module [" << CString(rEntry.getName()) << "] has already been loaded");
						return true;
					}
				}

				IPluginModule* module = new CPluginModule(this->getKernelContext());
				CString loadError;
				if (!module->load(rEntry.getName(), &loadError))
				{
					delete module;
					OV_WARNING_K("File [" << CString(rEntry.getName()) << "] is not a plugin module (error:" << loadError << ")");
					m_HaveAllPluginsLoadedCorrectly = false;
					return true;
				}

				if (!module->initialize())
				{
					module->uninitialize();
					module->unload();
					delete module;
					OV_WARNING_K("Module [" << CString(rEntry.getName()) << "] did not initialize correctly");
					m_HaveAllPluginsLoadedCorrectly = false;
					return true;
				}

				bool pluginObjectDescAdded = false;
				uint32_t index             = 0;
				uint32_t n                 = 0;
				IPluginObjectDesc* desc    = nullptr;
				while (module->getPluginObjectDescription(index, desc))
				{
					bool found = false;

					for (const auto& pluginObjectDesc : m_rPluginObjectDesc)
					{
						if (pluginObjectDesc.first->getClassIdentifier() == desc->getClassIdentifier())
						{
							OV_WARNING_K("Duplicate plugin object descriptor class identifier [" << pluginObjectDesc.first->getName() 
								<< "] and [" << desc->getName() << "]... second one is ignored");
							found = true;
							break;
						}
					}

					if (!found)
					{
						if (!pluginObjectDescAdded)
						{
							m_rPluginModule.push_back(module);
							pluginObjectDescAdded = true;
						}
						m_rPluginObjectDesc[desc] = module;
						n++;
					}
					index++;
					desc = nullptr;
				}

				OV_WARNING_UNLESS_K(pluginObjectDescAdded,
									"No 'plugin object descriptor' found from [" << CString(rEntry.getName()) << "] even if it looked like a plugin module\n");

				this->getLogManager() << LogLevel_Info << "Added " << n << " plugin object descriptor(s) from [" << CString(rEntry.getName()) << "]\n";

				return true;
			}

			_IsDerivedFromClass_Final_(TKernelObject < IObject >, OV_UndefinedIdentifier)

		protected:

			IPluginManager& m_rPluginManager;
			vector<IPluginModule*>& m_rPluginModule;
			map<IPluginObjectDesc*, IPluginModule*>& m_rPluginObjectDesc;
			bool& m_HaveAllPluginsLoadedCorrectly;
		};
	} // namespace Kernel
} // namespace OpenViBE

CPluginManager::~CPluginManager()
{
	std::unique_lock<std::mutex> lock(m_oMutex);

	for (auto& pluginObjectVector : m_vPluginObject)
	{
		for (auto& pluginObject : pluginObjectVector.second)
		{
			OV_WARNING_K(
				"Trying to release plugin object with class id " << pluginObject->getClassIdentifier() << " and plugin object descriptor " << pluginObjectVector
				.first->getName() << " at plugin manager destruction time");
			pluginObject->release();
		}
	}
	m_vPluginObject.clear();

	for (auto& pluginObjectDesc : m_vPluginObjectDesc) { pluginObjectDesc.first->release(); }
	m_vPluginObjectDesc.clear();

	for (auto k = m_vPluginModule.begin(); k != m_vPluginModule.end(); ++k)
	{
		this->TKernelObject<IPluginManager>::getLogManager() << LogLevel_Trace << "Releasing plugin module with class id " << (*k)->getClassIdentifier() << "\n";
		(*k)->uninitialize();
		delete (*k);
	}
	m_vPluginModule.clear();
}

bool CPluginManager::addPluginsFromFiles(const CString& rFileNameWildCard)
{
	std::unique_lock<std::mutex> lock(m_oMutex);

	this->getLogManager() << LogLevel_Info << "Adding plugins from [" << rFileNameWildCard << "]\n";

	bool res                     = true;
	bool haveAllPluginsLoadedCorrectly = true;
	CPluginManagerEntryEnumeratorCallBack l_rCB(this->getKernelContext(), m_vPluginModule, m_vPluginObjectDesc, haveAllPluginsLoadedCorrectly);
	FS::IEntryEnumerator* l_pEntryEnumerator = createEntryEnumerator(l_rCB);

	stringstream ss(rFileNameWildCard.toASCIIString());
	string path;

	while (getline(ss, path, ';'))
	{
		res &= l_pEntryEnumerator->enumerate(path.c_str());
		if (!res) { break; }
	}

	l_pEntryEnumerator->release();

	// Just return res. Error handling is performed within CPluginManagerEntryEnumeratorCallBack.
	return res && haveAllPluginsLoadedCorrectly;
}

bool CPluginManager::registerPluginDesc(const IPluginObjectDesc& rPluginObjectDesc)
{
	std::unique_lock<std::mutex> lock(m_oMutex);

	m_vPluginObjectDesc[const_cast<IPluginObjectDesc *>(&rPluginObjectDesc)] = nullptr;
	return true;
}

CIdentifier CPluginManager::getNextPluginObjectDescIdentifier(const CIdentifier& previousID) const
{
	std::unique_lock<std::mutex> lock(m_oMutex);

	bool l_bFoundPrevious = (previousID == OV_UndefinedIdentifier);
	for (const auto& elem : m_vPluginObjectDesc)
	{
		if (!l_bFoundPrevious) { if (elem.first->getClassIdentifier() == previousID) { l_bFoundPrevious = true; } }
		else { return elem.first->getClassIdentifier(); }
	}
	return OV_UndefinedIdentifier;
}

CIdentifier CPluginManager::getNextPluginObjectDescIdentifier(const CIdentifier& previousID, const CIdentifier& rBaseClassIdentifier) const
{
	std::unique_lock<std::mutex> lock(m_oMutex);

	bool l_bFoundPrevious = (previousID == OV_UndefinedIdentifier);
	for (const auto& elem : m_vPluginObjectDesc)
	{
		if (!l_bFoundPrevious) { if (elem.first->getClassIdentifier() == previousID) { l_bFoundPrevious = true; } }
		else { if (elem.first->isDerivedFromClass(rBaseClassIdentifier)) { return elem.first->getClassIdentifier(); } }
	}
	return OV_UndefinedIdentifier;
}

bool CPluginManager::canCreatePluginObject(const CIdentifier& classID)
{
	std::unique_lock<std::mutex> lock(m_oMutex);

	//	this->getLogManager() << LogLevel_Debug << "Searching if can build plugin object\n";

	return std::any_of(m_vPluginObjectDesc.begin(), m_vPluginObjectDesc.end(), [classID](const std::pair<IPluginObjectDesc*, IPluginModule*>& v)
	{
		return v.first->getCreatedClass() == classID;
	});
}

const IPluginObjectDesc* CPluginManager::getPluginObjectDesc(const CIdentifier& classID) const
{
	std::unique_lock<std::mutex> lock(m_oMutex);

	//	this->getLogManager() << LogLevel_Debug << "Searching plugin object descriptor\n";

	for (auto& pluginObject : m_vPluginObjectDesc) { if (pluginObject.first->getClassIdentifier() == classID) { return pluginObject.first; } }

	this->getLogManager() << LogLevel_Debug << "Plugin object descriptor class identifier " << classID << " not found\n";
	return nullptr;
}

const IPluginObjectDesc* CPluginManager::getPluginObjectDescCreating(const CIdentifier& classID) const
{
	std::unique_lock<std::mutex> lock(m_oMutex);

	//	this->getLogManager() << LogLevel_Debug << "Searching plugin object descriptor\n";

	auto elem = std::find_if(m_vPluginObjectDesc.begin(), m_vPluginObjectDesc.end(),
							 [classID](const std::pair<IPluginObjectDesc*, IPluginModule*>& v)
							 {
								 return v.first->getCreatedClass() == classID;
							 });
	if (elem != m_vPluginObjectDesc.end()) { return elem->first; }
	this->getLogManager() << LogLevel_Debug << "Plugin object descriptor class identifier " << classID << " not found\n";
	return nullptr;
}

CIdentifier CPluginManager::getPluginObjectHashValue(const CIdentifier& classID) const
{
	//	std::unique_lock<std::mutex> lock(m_oMutex);

	const IPluginObjectDesc* l_pPluginObjectDesc = this->getPluginObjectDescCreating(classID);
	const IBoxAlgorithmDesc* l_pBoxAlgorithmDesc = dynamic_cast<const IBoxAlgorithmDesc*>(l_pPluginObjectDesc);
	if (l_pBoxAlgorithmDesc)
	{
		SBoxProto l_oBoxPrototype(getKernelContext().getTypeManager());
		l_pBoxAlgorithmDesc->getBoxPrototype(l_oBoxPrototype);
		return l_oBoxPrototype.m_oHash;
	}
	return OV_UndefinedIdentifier;
}

CIdentifier CPluginManager::getPluginObjectHashValue(const IBoxAlgorithmDesc& rBoxAlgorithmDesc) const
{
	std::unique_lock<std::mutex> lock(m_oMutex);

	SBoxProto l_oBoxPrototype(getKernelContext().getTypeManager());
	rBoxAlgorithmDesc.getBoxPrototype(l_oBoxPrototype);
	return l_oBoxPrototype.m_oHash;
}

bool CPluginManager::isPluginObjectFlaggedAsDeprecated(const CIdentifier& classID) const
{
	//	std::unique_lock<std::mutex> lock(m_oMutex);

	const IPluginObjectDesc* l_pPluginObjectDesc = this->getPluginObjectDescCreating(classID);
	const IBoxAlgorithmDesc* l_pBoxAlgorithmDesc = dynamic_cast<const IBoxAlgorithmDesc*>(l_pPluginObjectDesc);
	if (l_pBoxAlgorithmDesc)
	{
		SBoxProto l_oBoxPrototype(getKernelContext().getTypeManager());
		l_pBoxAlgorithmDesc->getBoxPrototype(l_oBoxPrototype);
		return l_oBoxPrototype.m_bIsDeprecated;
	}
	return false;
}

IPluginObject* CPluginManager::createPluginObject(const CIdentifier& classID)
{
	return createPluginObjectT<IPluginObject, IPluginObjectDesc>(classID, nullptr);
}

bool CPluginManager::releasePluginObject(IPluginObject* pPluginObject)
{
	this->getLogManager() << LogLevel_Debug << "Releasing plugin object\n";

	OV_ERROR_UNLESS_KRF(pPluginObject, "Plugin object value is null", OpenViBE::Kernel::ErrorType::BadProcessing);

	{
		std::unique_lock<std::mutex> lock(m_oMutex);

		for (auto& elem : m_vPluginObject)
		{
			auto pluginObjectIt = std::find(elem.second.begin(), elem.second.end(), pPluginObject);
			if (pluginObjectIt != elem.second.end())
			{
				elem.second.erase(pluginObjectIt);
				pPluginObject->release();
				return true;
			}
		}
	}

	OV_ERROR_KRF("Plugin object has not been created by this plugin manager (class id was " << pPluginObject->getClassIdentifier().toString() << ")",
				 ErrorType::ResourceNotFound);
}

IAlgorithm* CPluginManager::createAlgorithm(const CIdentifier& classID, const IAlgorithmDesc** ppAlgorithmDesc)
{
	return createPluginObjectT<IAlgorithm, IAlgorithmDesc>(classID, ppAlgorithmDesc);
}

IAlgorithm* CPluginManager::createAlgorithm(const IAlgorithmDesc& rAlgorithmDesc)
{
	IAlgorithmDesc* l_pAlgorithmDesc = const_cast<IAlgorithmDesc*>(&rAlgorithmDesc);
	IPluginObject* l_pPluginObject   = l_pAlgorithmDesc->create();

	OV_ERROR_UNLESS_KRN(l_pPluginObject,
						"Could not create plugin object from " << rAlgorithmDesc.getName() << " plugin object descriptor",
						ErrorType::BadResourceCreation);

	IAlgorithmDesc* l_pPluginObjectDescT = dynamic_cast<IAlgorithmDesc*>(l_pAlgorithmDesc);
	IAlgorithm* l_pPluginObjectT         = dynamic_cast<IAlgorithm*>(l_pPluginObject);

	OV_ERROR_UNLESS_KRN(l_pPluginObjectDescT && l_pPluginObjectT,
						"Could not downcast plugin object and/or plugin object descriptor for " << rAlgorithmDesc.getName() << " plugin object descriptor",
						ErrorType::BadResourceCreation);

	{
		std::unique_lock<std::mutex> lock(m_oMutex);
		m_vPluginObject[l_pPluginObjectDescT].push_back(l_pPluginObjectT);
	}

	return l_pPluginObjectT;
}

IBoxAlgorithm* CPluginManager::createBoxAlgorithm(const CIdentifier& classID, const IBoxAlgorithmDesc** ppBoxAlgorithmDesc)
{
	return createPluginObjectT<IBoxAlgorithm, IBoxAlgorithmDesc>(classID, ppBoxAlgorithmDesc);
}

template <class IPluginObjectT, class IPluginObjectDescT>
IPluginObjectT* CPluginManager::createPluginObjectT(const CIdentifier& classID, const IPluginObjectDescT** ppPluginObjectDescT)
{
	std::unique_lock<std::mutex> lock(m_oMutex);

	if (ppPluginObjectDescT) { *ppPluginObjectDescT = nullptr; }

	CIdentifier l_oSubstitutionTokenIdentifier;
	char l_sSubstitutionTokenName[1024];
	uint64_t l_ui64SourceClassIdentifier = classID.toUInteger();
	uint64_t l_ui64TargetClassIdentifier = l_ui64SourceClassIdentifier;
	sprintf(l_sSubstitutionTokenName, "Kernel_PluginSubstitution_%0" PRIx64, l_ui64SourceClassIdentifier);
	if ((l_oSubstitutionTokenIdentifier = this->getConfigurationManager().lookUpConfigurationTokenIdentifier(l_sSubstitutionTokenName)) !=
		OV_UndefinedIdentifier)
	{
		CString l_sSubstitutionTokenValue = this->getConfigurationManager().getConfigurationTokenValue(l_oSubstitutionTokenIdentifier);
		l_sSubstitutionTokenValue         = this->getConfigurationManager().expand(l_sSubstitutionTokenValue);

		try { l_ui64TargetClassIdentifier = std::stoull(l_sSubstitutionTokenValue.toASCIIString(), nullptr, 16); }
		catch (const std::invalid_argument& exception)
		{
			OV_ERROR_KRN("Received exception while converting class identifier from string to number: " << exception.what(), ErrorType::BadArgument);
		}
		catch (const std::out_of_range& exception)
		{
			OV_ERROR_KRN("Received exception while converting class identifier from string to number: " << exception.what(), ErrorType::OutOfBound);
		}
	}
	if (l_ui64TargetClassIdentifier != l_ui64SourceClassIdentifier)
	{
		this->getLogManager() << LogLevel_Trace << "Substituting plugin class identifier " << CIdentifier(l_ui64SourceClassIdentifier) <<
				" with new class identifier " << CIdentifier(l_ui64TargetClassIdentifier) << "\n";
	}
	else
	{
		this->getLogManager() << LogLevel_Debug << "Not substitute plugin found for class identifier " << CIdentifier(l_ui64SourceClassIdentifier) <<
				" (configuration token name was " << CString(l_sSubstitutionTokenName) << ")\n";
	}

	IPluginObjectDesc* l_pPluginObjectDesc = nullptr;
	for (auto i = m_vPluginObjectDesc.begin(); i != m_vPluginObjectDesc.end(); ++i)
	{
		if (i->first->getCreatedClass() == CIdentifier(l_ui64TargetClassIdentifier)) { l_pPluginObjectDesc = i->first; }
	}

	OV_ERROR_UNLESS_KRN(l_pPluginObjectDesc,
						"Did not find the plugin object descriptor with requested class identifier " << CIdentifier(l_ui64SourceClassIdentifier).toString() <<
						" in registered plugin object descriptors",
						ErrorType::BadResourceCreation);

	IPluginObject* l_pPluginObject = l_pPluginObjectDesc->create();

	OV_ERROR_UNLESS_KRN(l_pPluginObject,
						"Could not create plugin object from " << l_pPluginObjectDesc->getName() << " plugin object descriptor",
						ErrorType::BadResourceCreation);

	IPluginObjectDescT* l_pPluginObjectDescT = dynamic_cast<IPluginObjectDescT*>(l_pPluginObjectDesc);
	IPluginObjectT* l_pPluginObjectT         = dynamic_cast<IPluginObjectT*>(l_pPluginObject);

	OV_ERROR_UNLESS_KRN(l_pPluginObjectDescT && l_pPluginObjectT,
						"Could not downcast plugin object and/or plugin object descriptor for " << l_pPluginObjectDesc->getName() <<
						" plugin object descriptor",
						ErrorType::BadResourceCreation);

	if (ppPluginObjectDescT) { *ppPluginObjectDescT = l_pPluginObjectDescT; }

	m_vPluginObject[l_pPluginObjectDescT].push_back(l_pPluginObjectT);

	return l_pPluginObjectT;
}
