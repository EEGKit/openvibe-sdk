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
#include <random>

#include "../../tools/ovkSBoxProto.h"
#include "ovp_global_defines.h"

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

			CMetaboxManagerEntryEnumeratorCallBack(const OpenViBE::Kernel::IKernelContext& rKernelContext, CMetaboxManager& metaboxManager)
				: TKernelObject < IObject >(rKernelContext), m_MetaboxManager(metaboxManager)
			{
				m_ui32MetaBoxCount = 0;
			}

			bool callback(FS::IEntryEnumerator::IEntry& rEntry, FS::IEntryEnumerator::IAttributes& rAttributes)
			{
				if(rAttributes.isFile())
				{
					std::string l_sFullFileName(rEntry.getName());

					OpenViBE::CIdentifier scenarioId, metaboxId, metaboxHash, metaboxPrototypeHash;
					getKernelContext().getScenarioManager().importScenarioFromFile(scenarioId, OVP_ScenarioImportContext_OnLoadMetaboxImport, l_sFullFileName.c_str());
					if (scenarioId != OV_UndefinedIdentifier)
					{
						OpenViBE::Kernel::IScenario& metaboxScenario = getKernelContext().getScenarioManager().getScenario(scenarioId);
						bool validId = metaboxId.fromString(metaboxScenario.getAttributeValue(OVP_AttributeId_Metabox_Identifier));
						if (validId && metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_Name) != CString())
						{
							metaboxHash.fromString(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_MetaboxHash));
							m_MetaboxManager.setMetaboxFilePath(metaboxId, CString(l_sFullFileName.c_str()));
							m_MetaboxManager.setMetaboxHash(metaboxId, metaboxHash);

							CMetaboxObjectDesc* metaboxDesc = new CMetaboxObjectDesc(metaboxId.toString().toASCIIString(), metaboxScenario);

							CIdentifier metaboxPrototypeHash = OV_UndefinedIdentifier;
							if (metaboxScenario.hasAttribute(OV_AttributeId_Scenario_MetaboxHash))
							{
								metaboxPrototypeHash.fromString(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_MetaboxHash));
							}
							else
							{
								getKernelContext().getLogManager() << LogLevel_Warning << "The metabox " << metaboxId.toString().toASCIIString() << " has no Hash in the scenario " << l_sFullFileName.c_str() << "\n";
							}

							std::string l_sVirtualBoxIdentifier = std::string(metaboxDesc->getCategory().toASCIIString()) + "/" + std::string(metaboxDesc->getName().toASCIIString());
							m_MetaboxManager.addMetaboxInfo(metaboxId, { *metaboxDesc, l_sVirtualBoxIdentifier.c_str(), l_sFullFileName.c_str(), metaboxPrototypeHash} );
							m_MetaboxManager.getMetaboxObjectDescMap()[metaboxId] = metaboxDesc;

							m_ui32MetaBoxCount++;
						}
					}
					getKernelContext().getScenarioManager().releaseScenario(scenarioId);
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
			CMetaboxManager& m_MetaboxManager;
			uint32_t m_ui32MetaBoxCount;

		};
	}
}

CMetaboxManager::CMetaboxManager(const IKernelContext& rKernelContext)
	: TKernelObject<IMetaboxManager>(rKernelContext)
{
	getScenarioManager().registerScenarioImporter(OVP_ScenarioImportContext_OnLoadMetaboxImport, ".mxb", OVP_GD_ClassId_Algorithm_XMLScenarioImporter);
}

CMetaboxManager::~CMetaboxManager(void)
{
	for (auto desc : m_MetaboxObjectDesc)
	{
		delete desc.second;
	}
}

bool CMetaboxManager::addMetaboxFromFiles(const CString& rFileNameWildCard)
{
	getLogManager() << LogLevel_Info << "Adding metaboxes from [" << rFileNameWildCard << "]\n";

	CMetaboxManagerEntryEnumeratorCallBack l_rCallback(this->getKernelContext(), *this); //, m_vPluginModule, m_vPluginObjectDesc, haveAllPluginsLoadedCorrectly);
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

	return true;
}

CIdentifier CMetaboxManager::getNextMetaboxObjectDescIdentifier(const CIdentifier& rPreviousIdentifier) const
{
	if (!m_MetaboxObjectDesc.size())
	{
		return OV_UndefinedIdentifier;
	}
	if (rPreviousIdentifier == OV_UndefinedIdentifier)
	{
		return m_MetaboxObjectDesc.begin()->first;
	}

	auto result = m_MetaboxObjectDesc.find(rPreviousIdentifier);
	if (result == m_MetaboxObjectDesc.end() || std::next(result, 1) == m_MetaboxObjectDesc.end())
	{
		return OV_UndefinedIdentifier;
	}
	return std::next(result, 1)->first;
}

const OpenViBE::Plugins::IPluginObjectDesc* CMetaboxManager::getMetaboxObjectDesc(const CIdentifier& rClassIdentifier) const
{
	auto result = m_MetaboxObjectDesc.find(rClassIdentifier);
	return result != m_MetaboxObjectDesc.end() ? result->second : nullptr;
}

OpenViBE::CString CMetaboxManager::getMetaboxFilePath(const OpenViBE::CIdentifier& rClassIdentifier) const
{
	auto resultIt = m_MetaboxFilePath.find(rClassIdentifier);
	return resultIt != m_MetaboxFilePath.end() ? resultIt->second : OpenViBE::CString();
}

void CMetaboxManager::setMetaboxFilePath(const OpenViBE::CIdentifier& rClassIdentifier, const CString &filePath)
{
	m_MetaboxFilePath[rClassIdentifier] = filePath;
}

OpenViBE::CIdentifier CMetaboxManager::getMetaboxHash(const OpenViBE::CIdentifier& rClassIdentifier) const
{
	auto resultIt = m_MetaboxHash.find(rClassIdentifier);
	return resultIt != m_MetaboxHash.end() ? resultIt->second : OV_UndefinedIdentifier;
}

void CMetaboxManager::setMetaboxHash(const OpenViBE::CIdentifier& rClassIdentifier, const OpenViBE::CIdentifier& hash)
{
	m_MetaboxHash[rClassIdentifier] = hash;
}
