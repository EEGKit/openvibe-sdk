#include "ovkCMetaboxManager.h"
//#include "ovkCPluginModule.h"

#include "../../ovk_tools.h"

#include <fs/IEntryEnumerator.h>
#include <fs/Files.h>

#include <system/ovCMemory.h>

#include <cstdio>
#include <iostream>
#include <map>
#include <algorithm>

#include "../../tools/ovkSBoxProto.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Metabox;
using namespace std;

namespace OpenViBE
{
	namespace Kernel
	{
		class CMetaboxManagerEntryEnumeratorCallBack : public TKernelObject < IObject >, public FS::IEntryEnumeratorCallBack
		{
		public:

			CMetaboxManagerEntryEnumeratorCallBack(const OpenViBE::Kernel::IKernelContext& rKernelContext)
				: TKernelObject < IObject >(rKernelContext)
			{
				m_ui32MetaBoxCount = 0;
			}

			bool callback(FS::IEntryEnumerator::IEntry& rEntry, FS::IEntryEnumerator::IAttributes& rAttributes)
			{
				if(rAttributes.isFile())
				{
					// extract the filename from the path, without parent or extension
					char l_sBuffer[1024];
					std::string l_sFullFileName(rEntry.getName());
					FS::Files::getParentPath(l_sFullFileName.c_str(), l_sBuffer);

					std::size_t l_uiFilenameStartPos = strlen(l_sBuffer) + 1; // where the parent path ends
					std::size_t l_uiFilenameDotPos = l_sFullFileName.rfind('.');
					std::size_t l_uiFilenameLength = l_uiFilenameDotPos - l_uiFilenameStartPos;

					std::string l_sMetaboxIdentifier = l_sFullFileName.substr(l_uiFilenameStartPos, l_uiFilenameLength);
					std::transform(l_sMetaboxIdentifier.begin(), l_sMetaboxIdentifier.end(), l_sMetaboxIdentifier.begin(), ::tolower);
					std::replace(l_sMetaboxIdentifier.begin(), l_sMetaboxIdentifier.end(), ' ', '_');

					std::string l_sConfigurationTokenName = "Metabox_Scenario_Path_For_" + l_sMetaboxIdentifier;
					getKernelContext().getConfigurationManager().createConfigurationToken(l_sConfigurationTokenName.c_str(), l_sFullFileName.c_str());

					OpenViBE::CIdentifier l_oMetaboxScenarioId;
					getKernelContext().getScenarioManager().importScenarioFromFile(l_oMetaboxScenarioId, OVP_ScenarioimportContext_OnLoadMetaboxImport, l_sFullFileName.c_str());
					l_sConfigurationTokenName = "Metabox_Scenario_Hash_For_" + l_sMetaboxIdentifier;

					OpenViBE::Kernel::IScenario& l_rMetaboxScenario = getKernelContext().getScenarioManager().getScenario(l_oMetaboxScenarioId);

					OpenViBE::CIdentifier l_oHash;
					l_oHash.fromString(l_rMetaboxScenario.getAttributeValue(OV_AttributeId_Scenario_MetaboxHash));

					getKernelContext().getConfigurationManager().createConfigurationToken(l_sConfigurationTokenName.c_str(), l_oHash.toString());
					getKernelContext().getScenarioManager().releaseScenario(l_oMetaboxScenarioId);

					m_ui32MetaBoxCount++;

				}
				return true;
			}

			uint32_t resetMetaboxCount(void)
			{
				uint32_t l_ui32ReturnValue = m_ui32MetaBoxCount;
				m_ui32MetaBoxCount = 0;
				return l_ui32ReturnValue;
			}

			_IsDerivedFromClass_Final_(TKernelObject < IObject >, OV_UndefinedIdentifier)
		protected:
			uint32_t m_ui32MetaBoxCount;
		};
	}
}
//namespace OpenViBE
//{
//	namespace Kernel
//	{
//		class CMetaboxManagerEntryEnumeratorCallBack : public TKernelObject < IObject >, public FS::IEntryEnumeratorCallBack
//		{
//		public:

//			CMetaboxManagerEntryEnumeratorCallBack(const IKernelContext& rKernelContext, vector<IPluginModule*>& rPluginModule, map<IPluginObjectDesc*, IPluginModule*>& rPluginObjectDesc, bool& haveAllPluginsLoadedCorrectly)
//				:TKernelObject < IObject >(rKernelContext)
//				,m_rPluginManager(rKernelContext.getPluginManager())
//				,m_rPluginModule(rPluginModule)
//				,m_rPluginObjectDesc(rPluginObjectDesc)
//			    ,m_HaveAllPluginsLoadedCorrectly(haveAllPluginsLoadedCorrectly)
//			{
//			}

//			virtual FS::boolean callback(
//				FS::IEntryEnumerator::IEntry& rEntry,
//				FS::IEntryEnumerator::IAttributes& rAttributes)
//			{
//				vector<IPluginModule*>::iterator i;
//				for(i=m_rPluginModule.begin(); i!=m_rPluginModule.end(); ++i)
//				{
//					CString l_sPluginModuleName;
//					if(!(*i)->getFileName(l_sPluginModuleName))
//					{
//						return true;
//					}

//					if(FS::Files::equals(rEntry.getName(), (const char*)l_sPluginModuleName))
//					{
//						OV_WARNING_K("Module [" << CString(rEntry.getName()) << "] has already been loaded");
//						return true;
//					}
//				}

//				IPluginModule* l_pPluginModule=new CPluginModule(this->getKernelContext());
//				CString l_sLoadError;
//				if(!l_pPluginModule->load(rEntry.getName(), &l_sLoadError))
//				{
//					delete l_pPluginModule;
//					OV_WARNING_K("File [" << CString(rEntry.getName()) << "] is not a plugin module (error:" << l_sLoadError << ")");
//					m_HaveAllPluginsLoadedCorrectly = false;
//					return true;
//				}

//				if(!l_pPluginModule->initialize())
//				{
//					l_pPluginModule->uninitialize();
//					l_pPluginModule->unload();
//					delete l_pPluginModule;
//					OV_WARNING_K("Module [" << CString(rEntry.getName()) << "] did not initialize correctly");
//					m_HaveAllPluginsLoadedCorrectly = false;
//					return true;
//				}

//				bool l_bPluginObjectDescAdded=false;
//				uint32 l_ui32Index=0;
//				uint32 l_ui32Count=0;
//				IPluginObjectDesc* l_pPluginObjectDesc=NULL;
//				while(l_pPluginModule->getPluginObjectDescription(l_ui32Index, l_pPluginObjectDesc))
//				{
//					bool l_bFound=false;

//					for (auto pluginObjectDesc : m_rPluginObjectDesc)
//					{
//						if (pluginObjectDesc.first->getClassIdentifier() == l_pPluginObjectDesc->getClassIdentifier())
//						{
//							OV_WARNING_K("Duplicate plugin object descriptor class identifier [" << pluginObjectDesc.first->getName() << "] and [" << l_pPluginObjectDesc->getName() << "]... second one is ignored");
//							l_bFound = true;
//							break;
//						}
//					}

//					if(!l_bFound)
//					{
//						if(!l_bPluginObjectDescAdded)
//						{
//							m_rPluginModule.push_back(l_pPluginModule);
//							l_bPluginObjectDescAdded=true;
//						}
//						m_rPluginObjectDesc[l_pPluginObjectDesc]=l_pPluginModule;
//						l_ui32Count++;
//					}
//					l_ui32Index++;
//					l_pPluginObjectDesc=NULL;
//				}

//				OV_WARNING_UNLESS_K(
//					l_bPluginObjectDescAdded,
//					"No 'plugin object descriptor' found from [" << CString(rEntry.getName()) << "] even if it looked like a plugin module\n"
//				);

//				this->getLogManager() << LogLevel_Info << "Added " << l_ui32Count << " plugin object descriptor(s) from [" << CString(rEntry.getName()) << "]\n";

//				return true;
//			}

//			_IsDerivedFromClass_Final_(TKernelObject < IObject >, OV_UndefinedIdentifier)

//		protected:

//			IPluginManager& m_rPluginManager;
//			vector<IPluginModule*>& m_rPluginModule;
//			map<IPluginObjectDesc*, IPluginModule*>& m_rPluginObjectDesc;
//			bool& m_HaveAllPluginsLoadedCorrectly;
//		};
//	}
//}

CMetaboxManager::CMetaboxManager(const IKernelContext& rKernelContext)
	: TKernelObject<IMetaboxManager>(rKernelContext)
{
}

CMetaboxManager::~CMetaboxManager(void)
{
//	for (auto& pluginObjectVector : m_vPluginObject)
//	{
//		for (auto& pluginObject : pluginObjectVector.second)
//		{
//			OV_WARNING_K("Trying to release plugin object with class id " << pluginObject->getClassIdentifier() << " and plugin object descriptor " << pluginObjectVector.first->getName() << " at plugin manager destruction time");
//			pluginObject->release();
//		}
//	}
//	m_vPluginObject.clear();

//	for (auto& pluginObjectDesc : m_vPluginObjectDesc)
//	{
//		pluginObjectDesc.first->release();
//	}
//	m_vPluginObjectDesc.clear();

//	vector < IPluginModule* >::iterator k;
//	for(k=m_vPluginModule.begin(); k!=m_vPluginModule.end(); ++k)
//	{
//		this->getLogManager() << LogLevel_Trace << "Releasing plugin module with class id " << (*k)->getClassIdentifier() << "\n";
//		(*k)->uninitialize();
//		delete (*k);
//	}
//	m_vPluginModule.clear();
}

bool CMetaboxManager::addMetaboxFromFiles(const CString& rFileNameWildCard)
{
	getLogManager() << LogLevel_Info << "Adding metaboxes from [" << rFileNameWildCard << "]\n";

//	bool l_bResult = true;
//	bool haveAllPluginsLoadedCorrectly = true;
	CMetaboxManagerEntryEnumeratorCallBack l_rCallback(this->getKernelContext()); //, m_vPluginModule, m_vPluginObjectDesc, haveAllPluginsLoadedCorrectly);
	FS::IEntryEnumerator* entryEnumerator = FS::createEntryEnumerator(l_rCallback);
	stringstream ss(rFileNameWildCard.toASCIIString());
	string path;

	while(getline(ss, path, ';')) {
		std::vector<std::string> extList { "*.xml", "*.mxb", "*.mbb" };
		bool result = true;
		for (auto ext : extList)
		{
			result |= entryEnumerator->enumerate((path + ext).c_str());
		}
		if(result)
		{
			getLogManager() << LogLevel_Info << "Added " << l_rCallback.resetMetaboxCount() << " metaboxes from [" << path.c_str() << "]\n";
		}
	}
	entryEnumerator->release();
	entryEnumerator = nullptr;

//	// Just return l_bResult. Error handling is performed within CMetaboxManagerEntryEnumeratorCallBack.
//	return l_bResult && haveAllPluginsLoadedCorrectly;
	return true;
}

//bool CMetaboxManager::registerPluginDesc(
//	const IPluginObjectDesc& rPluginObjectDesc)
//{
//	m_vPluginObjectDesc [ const_cast < IPluginObjectDesc * > (&rPluginObjectDesc) ] = NULL;
//	return true;
//}

//CIdentifier CMetaboxManager::getNextPluginObjectDescIdentifier(
//	const CIdentifier& rPreviousIdentifier) const
//{
//	bool l_bFoundPrevious=(rPreviousIdentifier==OV_UndefinedIdentifier);
//	map < IPluginObjectDesc*, IPluginModule* >::const_iterator i;
//	for(i=m_vPluginObjectDesc.begin(); i!=m_vPluginObjectDesc.end(); ++i)
//	{
//		if(!l_bFoundPrevious)
//		{
//			if(i->first->getClassIdentifier()==rPreviousIdentifier)
//			{
//				l_bFoundPrevious=true;
//			}
//		}
//		else
//		{
//			return i->first->getClassIdentifier();
//		}
//	}
//	return OV_UndefinedIdentifier;
//}

//CIdentifier CMetaboxManager::getNextPluginObjectDescIdentifier(
//	const CIdentifier& rPreviousIdentifier,
//	const CIdentifier& rBaseClassIdentifier) const
//{
//	bool l_bFoundPrevious=(rPreviousIdentifier==OV_UndefinedIdentifier);
//	map < IPluginObjectDesc*, IPluginModule* >::const_iterator i;
//	for(i=m_vPluginObjectDesc.begin(); i!=m_vPluginObjectDesc.end(); ++i)
//	{
//		if(!l_bFoundPrevious)
//		{
//			if(i->first->getClassIdentifier()==rPreviousIdentifier)
//			{
//				l_bFoundPrevious=true;
//			}
//		}
//		else
//		{
//			if(i->first->isDerivedFromClass(rBaseClassIdentifier))
//			{
//				return i->first->getClassIdentifier();
//			}
//		}
//	}
//	return OV_UndefinedIdentifier;
//}

//bool CMetaboxManager::canCreatePluginObject(
//	const CIdentifier& rClassIdentifier)
//{
////	this->getLogManager() << LogLevel_Debug << "Searching if can build plugin object\n";

//	map < IPluginObjectDesc*, IPluginModule* >::const_iterator i;
//	for(i=m_vPluginObjectDesc.begin(); i!=m_vPluginObjectDesc.end(); ++i)
//	{
//		if(i->first->getCreatedClass()==rClassIdentifier)
//		{
//			return true;
//		}
//	}

//	return false;
//}

//const IPluginObjectDesc* CMetaboxManager::getPluginObjectDesc(
//	const CIdentifier& rClassIdentifier) const
//{
////	this->getLogManager() << LogLevel_Debug << "Searching plugin object descriptor\n";

//	map < IPluginObjectDesc*, IPluginModule* >::const_iterator i;
//	for(i=m_vPluginObjectDesc.begin(); i!=m_vPluginObjectDesc.end(); ++i)
//	{
//		if(i->first->getClassIdentifier()==rClassIdentifier)
//		{
//			return i->first;
//		}
//	}

//	this->getLogManager() << LogLevel_Debug << "Plugin object descriptor class identifier " << rClassIdentifier << " not found\n";
//	return NULL;
//}

//const IPluginObjectDesc* CMetaboxManager::getPluginObjectDescCreating(
//	const CIdentifier& rClassIdentifier) const
//{
////	this->getLogManager() << LogLevel_Debug << "Searching plugin object descriptor\n";

//	map < IPluginObjectDesc*, IPluginModule* >::const_iterator i;
//	for(i=m_vPluginObjectDesc.begin(); i!=m_vPluginObjectDesc.end(); ++i)
//	{
//		if(i->first->getCreatedClass()==rClassIdentifier)
//		{
//			return i->first;
//		}
//	}

//	this->getLogManager() << LogLevel_Debug << "Plugin object descriptor class identifier " << rClassIdentifier << " not found\n";
//	return NULL;

//}

//CIdentifier CMetaboxManager::getPluginObjectHashValue(
//	const CIdentifier& rClassIdentifier) const
//{
//	const IPluginObjectDesc* l_pPluginObjectDesc=this->getPluginObjectDescCreating(rClassIdentifier);
//	const IBoxAlgorithmDesc* l_pBoxAlgorithmDesc=dynamic_cast<const IBoxAlgorithmDesc*>(l_pPluginObjectDesc);
//	if(l_pBoxAlgorithmDesc)
//	{
//		SBoxProto l_oBoxPrototype;
//		l_pBoxAlgorithmDesc->getBoxPrototype(l_oBoxPrototype);
//		return l_oBoxPrototype.m_oHash;
//	}
//	return OV_UndefinedIdentifier;
//}

//CIdentifier CMetaboxManager::getPluginObjectHashValue(const IBoxAlgorithmDesc& rBoxAlgorithmDesc) const
//{
//	SBoxProto l_oBoxPrototype;
//	rBoxAlgorithmDesc.getBoxPrototype(l_oBoxPrototype);
//	return l_oBoxPrototype.m_oHash;
//}

//bool CMetaboxManager::isPluginObjectFlaggedAsDeprecated(
//	const CIdentifier& rClassIdentifier) const
//{
//	const IPluginObjectDesc* l_pPluginObjectDesc=this->getPluginObjectDescCreating(rClassIdentifier);
//	const IBoxAlgorithmDesc* l_pBoxAlgorithmDesc=dynamic_cast<const IBoxAlgorithmDesc*>(l_pPluginObjectDesc);
//	if(l_pBoxAlgorithmDesc)
//	{
//		SBoxProto l_oBoxPrototype;
//		l_pBoxAlgorithmDesc->getBoxPrototype(l_oBoxPrototype);
//		return l_oBoxPrototype.m_bIsDeprecated;
//	}
//	return false;
//}

//IPluginObject* CMetaboxManager::createPluginObject(
//	const CIdentifier& rClassIdentifier)
//{
//	return createPluginObjectT<IPluginObject, IPluginObjectDesc>(rClassIdentifier, NULL);
//}

//bool CMetaboxManager::releasePluginObject(
//	IPluginObject* pPluginObject)
//{
//	this->getLogManager() << LogLevel_Debug << "Releasing plugin object\n";

//	map < IPluginObjectDesc*, vector < IPluginObject* > >::iterator i;
//	vector < IPluginObject* >::iterator j;
//	for(i=m_vPluginObject.begin(); i!=m_vPluginObject.end(); ++i)
//	{
//		for(j=i->second.begin(); j!=i->second.end(); ++j)
//		{
//			if((*j)==pPluginObject)
//			{
//				i->second.erase(j);
//				pPluginObject->release();
//				return true;
//			}
//		}
//	}

//	OV_ERROR_KRF(
//		"Plugin object has not been created by this plugin manager (class id was " << pPluginObject->getClassIdentifier().toString() << ")",
//		ErrorType::ResourceNotFound
//	);
//}

//IAlgorithm* CMetaboxManager::createAlgorithm(
//	const CIdentifier& rClassIdentifier,
//	const IAlgorithmDesc** ppAlgorithmDesc)
//{
//	return createPluginObjectT<IAlgorithm, IAlgorithmDesc>(rClassIdentifier, ppAlgorithmDesc);
//}

//IAlgorithm* CMetaboxManager::createAlgorithm(
//	const IAlgorithmDesc& rAlgorithmDesc)
//{
//	IAlgorithmDesc* l_pAlgorithmDesc = const_cast<IAlgorithmDesc*>(&rAlgorithmDesc);
//	IPluginObject* l_pPluginObject = l_pAlgorithmDesc->create();

//	OV_ERROR_UNLESS_KRN(
//		l_pPluginObject,
//		"Could not create plugin object from " << rAlgorithmDesc.getName() << " plugin object descriptor",
//		ErrorType::BadResourceCreation
//	);

//	IAlgorithmDesc* l_pPluginObjectDescT=dynamic_cast<IAlgorithmDesc*>(l_pAlgorithmDesc);
//	IAlgorithm* l_pPluginObjectT=dynamic_cast<IAlgorithm*>(l_pPluginObject);

//	OV_ERROR_UNLESS_KRN(
//		l_pPluginObjectDescT && l_pPluginObjectT,
//		"Could not downcast plugin object and/or plugin object descriptor for " << rAlgorithmDesc.getName() << " plugin object descriptor",
//		ErrorType::BadResourceCreation
//	);

//	m_vPluginObject[l_pPluginObjectDescT].push_back(l_pPluginObjectT);
//	return l_pPluginObjectT;
//}

//IBoxAlgorithm* CMetaboxManager::createBoxAlgorithm(
//	const CIdentifier& rClassIdentifier,
//	const IBoxAlgorithmDesc** ppBoxAlgorithmDesc)
//{
//	return createPluginObjectT<IBoxAlgorithm, IBoxAlgorithmDesc>(rClassIdentifier, ppBoxAlgorithmDesc);
//}

//template <class IPluginObjectT, class IPluginObjectDescT>
//IPluginObjectT* CMetaboxManager::createPluginObjectT(
//	const CIdentifier& rClassIdentifier,
//	const IPluginObjectDescT** ppPluginObjectDescT)
//{
//	if(ppPluginObjectDescT)
//	{
//		*ppPluginObjectDescT=NULL;
//	}

//	CIdentifier l_oSubstitutionTokenIdentifier;
//	char l_sSubstitutionTokenName[1024];
//	uint64 l_ui64SourceClassIdentifier=rClassIdentifier.toUInteger();
//	uint64 l_ui64TargetClassIdentifier=l_ui64SourceClassIdentifier;
//	::sprintf(l_sSubstitutionTokenName, "Kernel_PluginSubstitution_%0lx", l_ui64SourceClassIdentifier);
//	if((l_oSubstitutionTokenIdentifier=this->getConfigurationManager().lookUpConfigurationTokenIdentifier(l_sSubstitutionTokenName))!=OV_UndefinedIdentifier)
//	{
//		CString l_sSubstitutionTokenValue;
//		l_sSubstitutionTokenValue=this->getConfigurationManager().getConfigurationTokenValue(l_oSubstitutionTokenIdentifier);
//		l_sSubstitutionTokenValue=this->getConfigurationManager().expand(l_sSubstitutionTokenValue);

//		try
//		{
//			l_ui64TargetClassIdentifier = std::stoull(l_sSubstitutionTokenValue.toASCIIString(), 0, 16);
//		}
//		catch(const std::invalid_argument& exception)
//		{
//			OV_ERROR_KRN(
//				"Received exception while converting class identifier from string to number: " << exception.what(),
//				ErrorType::BadArgument
//			);
//		}
//		catch(const std::out_of_range& exception)
//		{
//			OV_ERROR_KRN(
//				"Received exception while converting class identifier from string to number: " << exception.what(),
//				ErrorType::OutOfBound
//			);
//		}
//	}
//	if(l_ui64TargetClassIdentifier!=l_ui64SourceClassIdentifier)
//	{
//		this->getLogManager() << LogLevel_Trace << "Substituting plugin class identifier " << CIdentifier(l_ui64SourceClassIdentifier) << " with new class identifier " << CIdentifier(l_ui64TargetClassIdentifier) << "\n";
//	}
//	else
//	{
//		this->getLogManager() << LogLevel_Debug << "Not substitute plugin found for class identifier " << CIdentifier(l_ui64SourceClassIdentifier) << " (configuration token name was " << CString(l_sSubstitutionTokenName) << ")\n";
//	}

//	IPluginObjectDesc* l_pPluginObjectDesc=NULL;
//	map < IPluginObjectDesc*, IPluginModule* >::const_iterator i;
//	for(i=m_vPluginObjectDesc.begin(); i!=m_vPluginObjectDesc.end(); ++i)
//	{
//		if(i->first->getCreatedClass()==CIdentifier(l_ui64TargetClassIdentifier))
//		{
//			l_pPluginObjectDesc=i->first;
//		}
//	}

//	OV_ERROR_UNLESS_KRN(
//		l_pPluginObjectDesc,
//		"Did not find the plugin object descriptor with requested class identifier " << CIdentifier(l_ui64SourceClassIdentifier).toString() << " in registered plugin object descriptors",
//		ErrorType::BadResourceCreation
//	);

//	IPluginObject* l_pPluginObject=l_pPluginObjectDesc->create();

//	OV_ERROR_UNLESS_KRN(
//		l_pPluginObject,
//		"Could not create plugin object from " << l_pPluginObjectDesc->getName() << " plugin object descriptor",
//		ErrorType::BadResourceCreation
//	);

//	IPluginObjectDescT* l_pPluginObjectDescT=dynamic_cast<IPluginObjectDescT*>(l_pPluginObjectDesc);
//	IPluginObjectT* l_pPluginObjectT=dynamic_cast<IPluginObjectT*>(l_pPluginObject);

//	OV_ERROR_UNLESS_KRN(
//		l_pPluginObjectDescT && l_pPluginObjectT,
//		"Could not downcast plugin object and/or plugin object descriptor for " << l_pPluginObjectDesc->getName() << " plugin object descriptor",
//		ErrorType::BadResourceCreation
//	);

//	if(ppPluginObjectDescT)
//	{
//		*ppPluginObjectDescT=l_pPluginObjectDescT;
//	}

//	m_vPluginObject[l_pPluginObjectDescT].push_back(l_pPluginObjectT);
//	return l_pPluginObjectT;
//}
