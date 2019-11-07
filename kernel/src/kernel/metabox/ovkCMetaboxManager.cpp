#include "ovkCMetaboxManager.h"

#include <fs/IEntryEnumerator.h>

#include <map>
#include <random>

#include "ovp_global_defines.h"
#include "ovkCMetaboxObjectDesc.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Metabox;
using namespace std;

namespace OpenViBE
{
	namespace Kernel
	{
		class CMetaboxManagerEntryEnumeratorCallBack final : public TKernelObject<IObject>, public FS::IEntryEnumeratorCallBack
		{
		public:

			CMetaboxManagerEntryEnumeratorCallBack(const IKernelContext& ctx, CMetaboxManager& metaboxManager)
				: TKernelObject<IObject>(ctx), m_manager(metaboxManager) { m_n = 0; }

			bool callback(FS::IEntryEnumerator::IEntry& rEntry, FS::IEntryEnumerator::IAttributes& rAttributes) override
			{
				if (rAttributes.isFile())
				{
					const char* fullFileName = rEntry.getName();

					CIdentifier scenarioID, metaboxId, metaboxHash;
					this->getKernelContext().getScenarioManager().
						  importScenarioFromFile(scenarioID, OV_ScenarioImportContext_OnLoadMetaboxImport, fullFileName);
					if (scenarioID != OV_UndefinedIdentifier)
					{
						IScenario& metaboxScenario = this->getKernelContext().getScenarioManager().getScenario(scenarioID);
						const bool isValid         = metaboxId.fromString(metaboxScenario.getAttributeValue(OVP_AttributeId_Metabox_Identifier));
						if (isValid && metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_Name) != CString())
						{
							const bool hasHash = metaboxHash.fromString(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_MetaboxHash));
							if (!hasHash)
							{
								this->getKernelContext().getLogManager() << LogLevel_Warning << "The metabox " << metaboxId.str() <<
										" has no Hash in the scenario " << fullFileName << "\n";
							}
							m_manager.setMetaboxFilePath(metaboxId, CString(fullFileName));
							m_manager.setMetaboxHash(metaboxId, metaboxHash);
							m_manager.setMetaboxObjectDesc(metaboxId, new CMetaboxObjectDesc(metaboxId.str().c_str(), metaboxScenario));
							m_n++;
						}
						else
						{
							this->getKernelContext().getLogManager() << LogLevel_Warning << "The metabox file " << fullFileName <<
									" is missing elements. Please check it.\n";
						}
					}
					this->getKernelContext().getScenarioManager().releaseScenario(scenarioID);
				}
				return true;
			}

			size_t resetMetaboxCount()
			{
				const size_t res = m_n;
				m_n              = 0;
				return res;
			}

			_IsDerivedFromClass_Final_(TKernelObject < IObject >, OV_UndefinedIdentifier)
		protected:
			CMetaboxManager& m_manager;
			size_t m_n;
		};
	} // namespace Kernel
} // namespace OpenViBE

CMetaboxManager::CMetaboxManager(const IKernelContext& ctx) : TKernelObject<IMetaboxManager>(ctx)
{
	this->TKernelObject<IMetaboxManager>::getScenarioManager().registerScenarioImporter(
		OV_ScenarioImportContext_OnLoadMetaboxImport, ".mxb", OVP_GD_ClassId_Algorithm_XMLScenarioImporter);
}

CMetaboxManager::~CMetaboxManager() { for (auto& desc : m_objectDesc) { delete desc.second; } }

bool CMetaboxManager::addMetaboxesFromFiles(const CString& fileNameWildCard)
{
	this->getLogManager() << LogLevel_Info << "Adding metaboxes from [" << fileNameWildCard << "]\n";

	CMetaboxManagerEntryEnumeratorCallBack callBack(this->getKernelContext(), *this); //, m_pluginModules, m_pluginObjectDescs, haveAllPluginsLoadedCorrectly);
	FS::IEntryEnumerator* entryEnumerator = createEntryEnumerator(callBack);
	stringstream ss(fileNameWildCard.toASCIIString());
	string path;
	while (getline(ss, path, ';'))
	{
		bool result = false; // Used to output imported metabox count
		CString ext("");
		while ((ext = this->getScenarioManager().getNextScenarioImporter(OV_ScenarioImportContext_OnLoadMetaboxImport, ext)) != CString(""))
		{
			result |= entryEnumerator->enumerate((path + "*" + ext.toASCIIString()).c_str());
		}
		if (result) { this->getLogManager() << LogLevel_Info << "Added " << callBack.resetMetaboxCount() << " metaboxes from [" << path.c_str() << "]\n"; }
	}
	entryEnumerator->release();

	return true;
}

CIdentifier CMetaboxManager::getNextMetaboxObjectDescIdentifier(const CIdentifier& previousID) const
{
	if (m_objectDesc.empty()) { return OV_UndefinedIdentifier; }
	if (previousID == OV_UndefinedIdentifier) { return m_objectDesc.begin()->first; }

	const auto result = m_objectDesc.find(previousID);
	if (result == m_objectDesc.end() || std::next(result, 1) == m_objectDesc.end()) { return OV_UndefinedIdentifier; }
	return std::next(result, 1)->first;
}

const Plugins::IPluginObjectDesc* CMetaboxManager::getMetaboxObjectDesc(const CIdentifier& metaboxID) const
{
	const auto result = m_objectDesc.find(metaboxID);
	return result != m_objectDesc.end() ? result->second : nullptr;
}

void CMetaboxManager::setMetaboxObjectDesc(const CIdentifier& metaboxID, Plugins::IPluginObjectDesc* metaboxDesc) { m_objectDesc[metaboxID] = metaboxDesc; }

CString CMetaboxManager::getMetaboxFilePath(const CIdentifier& metaboxID) const
{
	const auto resultIt = m_filepath.find(metaboxID);
	return resultIt != m_filepath.end() ? resultIt->second : CString();
}

void CMetaboxManager::setMetaboxFilePath(const CIdentifier& metaboxID, const CString& filePath) { m_filepath[metaboxID] = filePath; }

CIdentifier CMetaboxManager::getMetaboxHash(const CIdentifier& metaboxID) const
{
	const auto resultIt = m_hash.find(metaboxID);
	return resultIt != m_hash.end() ? resultIt->second : OV_UndefinedIdentifier;
}

void CMetaboxManager::setMetaboxHash(const CIdentifier& metaboxID, const CIdentifier& hash) { m_hash[metaboxID] = hash; }
