#include "ovkCPluginManager.h"
#include "ovkCPluginModule.h"

#include "../../ovk_tools.h"

#include <fs/IEntryEnumerator.h>
#include <fs/Files.h>

#include <system/ovCMemory.h>

#include <cstdio>
#include <iostream>
#include <map>

#include "../../tools/ovkSBoxProto.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace std;

namespace OpenViBE
{
	namespace Kernel
	{
		class CPluginManagerEntryEnumeratorCallBack : public TKernelObject < IObject >, public FS::IEntryEnumeratorCallBack
		{
		public:

			CPluginManagerEntryEnumeratorCallBack(const IKernelContext& rKernelContext, vector<IPluginModule*>& rPluginModule, map<IPluginObjectDesc*, IPluginModule*>& rPluginObjectDesc, bool& haveAllPluginsLoadedCorrectly)
				:TKernelObject < IObject >(rKernelContext)
				,m_rPluginManager(rKernelContext.getPluginManager())
				,m_rPluginModule(rPluginModule)
				,m_rPluginObjectDesc(rPluginObjectDesc)
			    ,m_HaveAllPluginsLoadedCorrectly(haveAllPluginsLoadedCorrectly)
			{
			}

			virtual FS::boolean callback(
				FS::IEntryEnumerator::IEntry& rEntry,
				FS::IEntryEnumerator::IAttributes& rAttributes)
			{
				vector<IPluginModule*>::iterator i;
				for(i=m_rPluginModule.begin(); i!=m_rPluginModule.end(); ++i)
				{
					CString l_sPluginModuleName;
					if(!(*i)->getFileName(l_sPluginModuleName))
					{
						return true;
					}

					if(FS::Files::equals(rEntry.getName(), (const char*)l_sPluginModuleName))
					{
						OV_WARNING_K("Module [" << CString(rEntry.getName()) << "] has already been loaded");
						return true;
					}
				}

				IPluginModule* l_pPluginModule=new CPluginModule(this->getKernelContext());
				CString l_sLoadError;
				if(!l_pPluginModule->load(rEntry.getName(), &l_sLoadError))
				{
					delete l_pPluginModule;
					OV_WARNING_K("File [" << CString(rEntry.getName()) << "] is not a plugin module (error:" << l_sLoadError << ")");
					m_HaveAllPluginsLoadedCorrectly = false;
					return true;
				}

				if(!l_pPluginModule->initialize())
				{
					l_pPluginModule->uninitialize();
					l_pPluginModule->unload();
					delete l_pPluginModule;
					OV_WARNING_K("Module [" << CString(rEntry.getName()) << "] did not initialize correctly");
					m_HaveAllPluginsLoadedCorrectly = false;
					return true;
				}

				bool l_bPluginObjectDescAdded=false;
				uint32 l_ui32Index=0;
				uint32 l_ui32Count=0;
				IPluginObjectDesc* l_pPluginObjectDesc=NULL;
				while(l_pPluginModule->getPluginObjectDescription(l_ui32Index, l_pPluginObjectDesc))
				{
					bool l_bFound=false;

					for (auto pluginObjectDesc : m_rPluginObjectDesc)
					{
						if (pluginObjectDesc.first->getClassIdentifier() == l_pPluginObjectDesc->getClassIdentifier())
						{
							OV_WARNING_K("Duplicate plugin object descriptor class identifier [" << pluginObjectDesc.first->getName() << "] and [" << l_pPluginObjectDesc->getName() << "]... second one is ignored");
							l_bFound = true;
							break;
						}
					}

					if(!l_bFound)
					{
						if(!l_bPluginObjectDescAdded)
						{
							m_rPluginModule.push_back(l_pPluginModule);
							l_bPluginObjectDescAdded=true;
						}
						m_rPluginObjectDesc[l_pPluginObjectDesc]=l_pPluginModule;
						l_ui32Count++;
					}
					l_ui32Index++;
					l_pPluginObjectDesc=NULL;
				}

				OV_WARNING_UNLESS_K(
					l_bPluginObjectDescAdded,
					"No 'plugin object descriptor' found from [" << CString(rEntry.getName()) << "] even if it looked like a plugin module\n"
				);

				this->getLogManager() << LogLevel_Info << "Added " << l_ui32Count << " plugin object descriptor(s) from [" << CString(rEntry.getName()) << "]\n";

				return true;
			}

			_IsDerivedFromClass_Final_(TKernelObject < IObject >, OV_UndefinedIdentifier)

		protected:

			IPluginManager& m_rPluginManager;
			vector<IPluginModule*>& m_rPluginModule;
			map<IPluginObjectDesc*, IPluginModule*>& m_rPluginObjectDesc;
			bool& m_HaveAllPluginsLoadedCorrectly;
		};
	}
}

CPluginManager::CPluginManager(const IKernelContext& rKernelContext)
	:TKernelObject<IPluginManager>(rKernelContext)
{
}

CPluginManager::~CPluginManager(void)
{
	for (auto& pluginObjectVector : m_vPluginObject)
	{
		for (auto& pluginObject : pluginObjectVector.second)
		{
			OV_WARNING_K("Trying to release plugin object with class id " << pluginObject->getClassIdentifier() << " and plugin object descriptor " << pluginObjectVector.first->getName() << " at plugin manager destruction time");
			pluginObject->release();
		}
	}
	m_vPluginObject.clear();

	for (auto& pluginObjectDesc : m_vPluginObjectDesc)
	{
		pluginObjectDesc.first->release();
	}
	m_vPluginObjectDesc.clear();

	vector < IPluginModule* >::iterator k;
	for(k=m_vPluginModule.begin(); k!=m_vPluginModule.end(); ++k)
	{
		this->getLogManager() << LogLevel_Trace << "Releasing plugin module with class id " << (*k)->getClassIdentifier() << "\n";
		(*k)->uninitialize();
		delete (*k);
	}
	m_vPluginModule.clear();
}

bool CPluginManager::addPluginsFromFiles(
	const CString& rFileNameWildCard)
{
	this->getLogManager() << LogLevel_Info << "Adding plugins from [" << rFileNameWildCard << "]\n";

	bool l_bResult = true;
	bool haveAllPluginsLoadedCorrectly = true;
	CPluginManagerEntryEnumeratorCallBack l_rCB(this->getKernelContext(), m_vPluginModule, m_vPluginObjectDesc, haveAllPluginsLoadedCorrectly);
	FS::IEntryEnumerator* l_pEntryEnumerator=FS::createEntryEnumerator(l_rCB);

	stringstream ss(rFileNameWildCard.toASCIIString());
	string path;

	while(getline(ss, path, ';')) {
		l_bResult &= l_pEntryEnumerator->enumerate(path.c_str());
		if(!l_bResult)
		{
			break;
		}
	}

	l_pEntryEnumerator->release();

	// Just return l_bResult. Error handling is performed within CPluginManagerEntryEnumeratorCallBack.
	return l_bResult && haveAllPluginsLoadedCorrectly;
}

bool CPluginManager::registerPluginDesc(
	const IPluginObjectDesc& rPluginObjectDesc)
{
	m_vPluginObjectDesc [ const_cast < IPluginObjectDesc * > (&rPluginObjectDesc) ] = NULL;
	return true;
}

CIdentifier CPluginManager::getNextPluginObjectDescIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	bool l_bFoundPrevious=(rPreviousIdentifier==OV_UndefinedIdentifier);
	map < IPluginObjectDesc*, IPluginModule* >::const_iterator i;
	for(i=m_vPluginObjectDesc.begin(); i!=m_vPluginObjectDesc.end(); ++i)
	{
		if(!l_bFoundPrevious)
		{
			if(i->first->getClassIdentifier()==rPreviousIdentifier)
			{
				l_bFoundPrevious=true;
			}
		}
		else
		{
			return i->first->getClassIdentifier();
		}
	}
	return OV_UndefinedIdentifier;
}

CIdentifier CPluginManager::getNextPluginObjectDescIdentifier(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rBaseClassIdentifier) const
{
	bool l_bFoundPrevious=(rPreviousIdentifier==OV_UndefinedIdentifier);
	map < IPluginObjectDesc*, IPluginModule* >::const_iterator i;
	for(i=m_vPluginObjectDesc.begin(); i!=m_vPluginObjectDesc.end(); ++i)
	{
		if(!l_bFoundPrevious)
		{
			if(i->first->getClassIdentifier()==rPreviousIdentifier)
			{
				l_bFoundPrevious=true;
			}
		}
		else
		{
			if(i->first->isDerivedFromClass(rBaseClassIdentifier))
			{
				return i->first->getClassIdentifier();
			}
		}
	}
	return OV_UndefinedIdentifier;
}

bool CPluginManager::canCreatePluginObject(
	const CIdentifier& rClassIdentifier)
{
//	this->getLogManager() << LogLevel_Debug << "Searching if can build plugin object\n";

	map < IPluginObjectDesc*, IPluginModule* >::const_iterator i;
	for(i=m_vPluginObjectDesc.begin(); i!=m_vPluginObjectDesc.end(); ++i)
	{
		if(i->first->getCreatedClass()==rClassIdentifier)
		{
			return true;
		}
	}

	return false;
}

const IPluginObjectDesc* CPluginManager::getPluginObjectDesc(
	const CIdentifier& rClassIdentifier) const
{
//	this->getLogManager() << LogLevel_Debug << "Searching plugin object descriptor\n";

	map < IPluginObjectDesc*, IPluginModule* >::const_iterator i;
	for(i=m_vPluginObjectDesc.begin(); i!=m_vPluginObjectDesc.end(); ++i)
	{
		if(i->first->getClassIdentifier()==rClassIdentifier)
		{
			return i->first;
		}
	}

	this->getLogManager() << LogLevel_Debug << "Plugin object descriptor class identifier " << rClassIdentifier << " not found\n";
	return NULL;
}

const IPluginObjectDesc* CPluginManager::getPluginObjectDescCreating(
	const CIdentifier& rClassIdentifier) const
{
//	this->getLogManager() << LogLevel_Debug << "Searching plugin object descriptor\n";

	map < IPluginObjectDesc*, IPluginModule* >::const_iterator i;
	for(i=m_vPluginObjectDesc.begin(); i!=m_vPluginObjectDesc.end(); ++i)
	{
		if(i->first->getCreatedClass()==rClassIdentifier)
		{
			return i->first;
		}
	}

	this->getLogManager() << LogLevel_Debug << "Plugin object descriptor class identifier " << rClassIdentifier << " not found\n";
	return NULL;

}

CIdentifier CPluginManager::getPluginObjectHashValue(
	const CIdentifier& rClassIdentifier) const
{
	const IPluginObjectDesc* l_pPluginObjectDesc=this->getPluginObjectDescCreating(rClassIdentifier);
	const IBoxAlgorithmDesc* l_pBoxAlgorithmDesc=dynamic_cast<const IBoxAlgorithmDesc*>(l_pPluginObjectDesc);
	if(l_pBoxAlgorithmDesc)
	{
		SBoxProto l_oBoxPrototype(getKernelContext().getTypeManager());
		l_pBoxAlgorithmDesc->getBoxPrototype(l_oBoxPrototype);
		return l_oBoxPrototype.m_oHash;
	}
	return OV_UndefinedIdentifier;
}

CIdentifier CPluginManager::getPluginObjectHashValue(const IBoxAlgorithmDesc& rBoxAlgorithmDesc) const
{
	SBoxProto l_oBoxPrototype(getKernelContext().getTypeManager());
	rBoxAlgorithmDesc.getBoxPrototype(l_oBoxPrototype);
	return l_oBoxPrototype.m_oHash;
}

bool CPluginManager::isPluginObjectFlaggedAsDeprecated(
	const CIdentifier& rClassIdentifier) const
{
	const IPluginObjectDesc* l_pPluginObjectDesc=this->getPluginObjectDescCreating(rClassIdentifier);
	const IBoxAlgorithmDesc* l_pBoxAlgorithmDesc=dynamic_cast<const IBoxAlgorithmDesc*>(l_pPluginObjectDesc);
	if(l_pBoxAlgorithmDesc)
	{
		SBoxProto l_oBoxPrototype(getKernelContext().getTypeManager());
		l_pBoxAlgorithmDesc->getBoxPrototype(l_oBoxPrototype);
		return l_oBoxPrototype.m_bIsDeprecated;
	}
	return false;
}

IPluginObject* CPluginManager::createPluginObject(
	const CIdentifier& rClassIdentifier)
{
	return createPluginObjectT<IPluginObject, IPluginObjectDesc>(rClassIdentifier, NULL);
}

bool CPluginManager::releasePluginObject(
	IPluginObject* pPluginObject)
{
	this->getLogManager() << LogLevel_Debug << "Releasing plugin object\n";

	map < IPluginObjectDesc*, vector < IPluginObject* > >::iterator i;
	vector < IPluginObject* >::iterator j;
	for(i=m_vPluginObject.begin(); i!=m_vPluginObject.end(); ++i)
	{
		for(j=i->second.begin(); j!=i->second.end(); ++j)
		{
			if((*j)==pPluginObject)
			{
				i->second.erase(j);
				pPluginObject->release();
				return true;
			}
		}
	}

	OV_ERROR_KRF(
		"Plugin object has not been created by this plugin manager (class id was " << pPluginObject->getClassIdentifier().toString() << ")",
		ErrorType::ResourceNotFound
	);
}

IAlgorithm* CPluginManager::createAlgorithm(
	const CIdentifier& rClassIdentifier,
	const IAlgorithmDesc** ppAlgorithmDesc)
{
	return createPluginObjectT<IAlgorithm, IAlgorithmDesc>(rClassIdentifier, ppAlgorithmDesc);
}

IAlgorithm* CPluginManager::createAlgorithm(
	const IAlgorithmDesc& rAlgorithmDesc)
{
	IAlgorithmDesc* l_pAlgorithmDesc = const_cast<IAlgorithmDesc*>(&rAlgorithmDesc);
	IPluginObject* l_pPluginObject = l_pAlgorithmDesc->create();

	OV_ERROR_UNLESS_KRN(
		l_pPluginObject,
		"Could not create plugin object from " << rAlgorithmDesc.getName() << " plugin object descriptor",
		ErrorType::BadResourceCreation
	);

	IAlgorithmDesc* l_pPluginObjectDescT=dynamic_cast<IAlgorithmDesc*>(l_pAlgorithmDesc);
	IAlgorithm* l_pPluginObjectT=dynamic_cast<IAlgorithm*>(l_pPluginObject);

	OV_ERROR_UNLESS_KRN(
		l_pPluginObjectDescT && l_pPluginObjectT,
		"Could not downcast plugin object and/or plugin object descriptor for " << rAlgorithmDesc.getName() << " plugin object descriptor",
		ErrorType::BadResourceCreation
	);

	m_vPluginObject[l_pPluginObjectDescT].push_back(l_pPluginObjectT);
	return l_pPluginObjectT;
}

IBoxAlgorithm* CPluginManager::createBoxAlgorithm(
	const CIdentifier& rClassIdentifier,
	const IBoxAlgorithmDesc** ppBoxAlgorithmDesc)
{
	return createPluginObjectT<IBoxAlgorithm, IBoxAlgorithmDesc>(rClassIdentifier, ppBoxAlgorithmDesc);
}

template <class IPluginObjectT, class IPluginObjectDescT>
IPluginObjectT* CPluginManager::createPluginObjectT(
	const CIdentifier& rClassIdentifier,
	const IPluginObjectDescT** ppPluginObjectDescT)
{
	if(ppPluginObjectDescT)
	{
		*ppPluginObjectDescT=NULL;
	}

	CIdentifier l_oSubstitutionTokenIdentifier;
	char l_sSubstitutionTokenName[1024];
	uint64 l_ui64SourceClassIdentifier=rClassIdentifier.toUInteger();
	uint64 l_ui64TargetClassIdentifier=l_ui64SourceClassIdentifier;
	::sprintf(l_sSubstitutionTokenName, "Kernel_PluginSubstitution_%0lx", l_ui64SourceClassIdentifier);
	if((l_oSubstitutionTokenIdentifier=this->getConfigurationManager().lookUpConfigurationTokenIdentifier(l_sSubstitutionTokenName))!=OV_UndefinedIdentifier)
	{
		CString l_sSubstitutionTokenValue;
		l_sSubstitutionTokenValue=this->getConfigurationManager().getConfigurationTokenValue(l_oSubstitutionTokenIdentifier);
		l_sSubstitutionTokenValue=this->getConfigurationManager().expand(l_sSubstitutionTokenValue);

		try
		{
			l_ui64TargetClassIdentifier = std::stoull(l_sSubstitutionTokenValue.toASCIIString(), 0, 16);
		}
		catch(const std::invalid_argument& exception)
		{
			OV_ERROR_KRN(
				"Received exception while converting class identifier from string to number: " << exception.what(),
				ErrorType::BadArgument
			);
		}
		catch(const std::out_of_range& exception)
		{
			OV_ERROR_KRN(
				"Received exception while converting class identifier from string to number: " << exception.what(),
				ErrorType::OutOfBound
			);
		}
	}
	if(l_ui64TargetClassIdentifier!=l_ui64SourceClassIdentifier)
	{
		this->getLogManager() << LogLevel_Trace << "Substituting plugin class identifier " << CIdentifier(l_ui64SourceClassIdentifier) << " with new class identifier " << CIdentifier(l_ui64TargetClassIdentifier) << "\n";
	}
	else
	{
		this->getLogManager() << LogLevel_Debug << "Not substitute plugin found for class identifier " << CIdentifier(l_ui64SourceClassIdentifier) << " (configuration token name was " << CString(l_sSubstitutionTokenName) << ")\n";
	}

	IPluginObjectDesc* l_pPluginObjectDesc=NULL;
	map < IPluginObjectDesc*, IPluginModule* >::const_iterator i;
	for(i=m_vPluginObjectDesc.begin(); i!=m_vPluginObjectDesc.end(); ++i)
	{
		if(i->first->getCreatedClass()==CIdentifier(l_ui64TargetClassIdentifier))
		{
			l_pPluginObjectDesc=i->first;
		}
	}

	OV_ERROR_UNLESS_KRN(
		l_pPluginObjectDesc,
		"Did not find the plugin object descriptor with requested class identifier " << CIdentifier(l_ui64SourceClassIdentifier).toString() << " in registered plugin object descriptors",
		ErrorType::BadResourceCreation
	);

	IPluginObject* l_pPluginObject=l_pPluginObjectDesc->create();

	OV_ERROR_UNLESS_KRN(
		l_pPluginObject,
		"Could not create plugin object from " << l_pPluginObjectDesc->getName() << " plugin object descriptor",
		ErrorType::BadResourceCreation
	);

	IPluginObjectDescT* l_pPluginObjectDescT=dynamic_cast<IPluginObjectDescT*>(l_pPluginObjectDesc);
	IPluginObjectT* l_pPluginObjectT=dynamic_cast<IPluginObjectT*>(l_pPluginObject);

	OV_ERROR_UNLESS_KRN(
		l_pPluginObjectDescT && l_pPluginObjectT,
		"Could not downcast plugin object and/or plugin object descriptor for " << l_pPluginObjectDesc->getName() << " plugin object descriptor",
		ErrorType::BadResourceCreation
	);

	if(ppPluginObjectDescT)
	{
		*ppPluginObjectDescT=l_pPluginObjectDescT;
	}

	m_vPluginObject[l_pPluginObjectDescT].push_back(l_pPluginObjectT);
	return l_pPluginObjectT;
}
