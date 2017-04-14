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

					OpenViBE::CIdentifier l_oMetaboxScenarioId;
					getKernelContext().getScenarioManager().importScenarioFromFile(l_oMetaboxScenarioId, OVP_ScenarioImportContext_OnLoadMetaboxImport, l_sFullFileName.c_str());
					if (l_oMetaboxScenarioId != OV_UndefinedIdentifier)
					{
						OpenViBE::Kernel::IScenario& metaboxScenario = getKernelContext().getScenarioManager().getScenario(l_oMetaboxScenarioId);
						OpenViBE::CIdentifier l_oHash;
						l_oHash.fromString(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_MetaboxHash));

						OpenViBE::CString l_sMetaboxIdentifier(metaboxScenario.getAttributeValue(OVP_AttributeId_Metabox_Identifier));
						OpenViBE::CIdentifier metaboxId;
						metaboxId.fromString(l_sMetaboxIdentifier);
						m_MetaboxManager.setMetaboxFilePath(metaboxId, CString(l_sFullFileName.c_str()));
						m_MetaboxManager.setMetaboxHash(metaboxId, l_oHash);

						CBoxAlgorithmMetaboxDesc l_oMetaboxDesc(l_sMetaboxIdentifier.toASCIIString(), metaboxScenario);

						CIdentifier l_oMetaboxPrototypeHash = OV_UndefinedIdentifier;

						if (metaboxScenario.hasAttribute(OV_AttributeId_Scenario_MetaboxHash))
						{
							l_oMetaboxPrototypeHash.fromString(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_MetaboxHash));
						}
						else
						{
							getKernelContext().getLogManager() << LogLevel_Warning << "The metabox " << l_sMetaboxIdentifier.toASCIIString() << " has no Hash in the scenario " << l_sFullFileName.c_str() << "\n";
						}

						std::string l_sVirtualBoxIdentifier = std::string(l_oMetaboxDesc.getCategory().toASCIIString()) + "/" + std::string(l_oMetaboxDesc.getName().toASCIIString());
						m_MetaboxManager.getMetaboxInfo(l_sMetaboxIdentifier.toASCIIString()).assignMetaboxDesc(l_oMetaboxDesc);
						m_MetaboxManager.getMetaboxInfo(l_sMetaboxIdentifier.toASCIIString()).assignVirtualBoxIdentifier(l_sVirtualBoxIdentifier.c_str());
						m_MetaboxManager.getMetaboxInfo(l_sMetaboxIdentifier.toASCIIString()).assignMetaboxScenarioPath(l_sFullFileName.c_str());
						m_MetaboxManager.getMetaboxInfo(l_sMetaboxIdentifier.toASCIIString()).assignMetaboxHash(l_oMetaboxPrototypeHash);

						m_MetaboxManager.getMetaboxObjectDescMap()[l_sVirtualBoxIdentifier] = &(m_MetaboxManager.getMetaboxInfo(l_sMetaboxIdentifier.toASCIIString()).m_oMetaboxDesc);


						getKernelContext().getScenarioManager().releaseScenario(l_oMetaboxScenarioId);

						m_ui32MetaBoxCount++;
					}

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
}

CMetaboxManager::~CMetaboxManager(void)
{
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

CIdentifier CMetaboxManager::getNextMetaboxObjectDescIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	if (rPreviousIdentifier == OV_UndefinedIdentifier)
	{
		CIdentifier res;
		res.fromString(CString(m_mMetaboxObjectDesc.begin()->first.c_str()));
		return res;
	}

	auto result = m_mMetaboxObjectDesc.find(rPreviousIdentifier.toString().toASCIIString());
	if (result == m_mMetaboxObjectDesc.end() || std::next(result, 1) == m_mMetaboxObjectDesc.end())
	{
		return OV_UndefinedIdentifier;
	}
	CIdentifier res;
	res.fromString(CString(std::next(result, 1)->first.c_str()));
	return res;
}

const OpenViBE::Plugins::IPluginObjectDesc* CMetaboxManager::getMetaboxObjectDesc(const CIdentifier& rClassIdentifier) const
{
	auto result = m_mMetaboxObjectDesc.find(rClassIdentifier.toString().toASCIIString());
	return result != m_mMetaboxObjectDesc.end() ? result->second : nullptr;
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
