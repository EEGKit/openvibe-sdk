#include "ovkCAlgorithmManager.h"
#include "ovkCAlgorithmProxy.h"

#include <system/ovCMath.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;
using namespace std;

CAlgorithmManager::CAlgorithmManager(const IKernelContext& ctx) : TKernelObject<IAlgorithmManager>(ctx) {}

CAlgorithmManager::~CAlgorithmManager()
{
	std::unique_lock<std::mutex> lock(m_oMutex);

	for (auto& algorithm : m_vAlgorithms)
	{
		CAlgorithmProxy* algorithmProxy = algorithm.second;
		IAlgorithm& tmpAlgorithm        = algorithmProxy->getAlgorithm();
		delete algorithmProxy;

		getKernelContext().getPluginManager().releasePluginObject(&tmpAlgorithm);
	}

	m_vAlgorithms.clear();
}

CIdentifier CAlgorithmManager::createAlgorithm(const CIdentifier& algorithmClassID)
{
	const IAlgorithmDesc* algorithmDesc = nullptr;
	IAlgorithm* algorithm               = getKernelContext().getPluginManager().createAlgorithm(algorithmClassID, &algorithmDesc);

	OV_ERROR_UNLESS_KRU(algorithm && algorithmDesc, "Algorithm creation failed, class identifier :" << algorithmClassID.toString(),
						ErrorType::BadResourceCreation);

	getLogManager() << LogLevel_Debug << "Creating algorithm with class identifier " << algorithmClassID << "\n";

	CIdentifier algorithmId         = getUnusedIdentifier();
	CAlgorithmProxy* algorithmProxy = new CAlgorithmProxy(getKernelContext(), *algorithm, *algorithmDesc);

	{
		std::unique_lock<std::mutex> lock(m_oMutex);
		m_vAlgorithms[algorithmId] = algorithmProxy;
	}

	return algorithmId;
}

CIdentifier CAlgorithmManager::createAlgorithm(const IAlgorithmDesc& rAlgorithmDesc)
{
	std::unique_lock<std::mutex> lock(m_oMutex);

	IAlgorithm* algorithm = getKernelContext().getPluginManager().createAlgorithm(rAlgorithmDesc);

	OV_ERROR_UNLESS_KRU(algorithm, "Algorithm creation failed, class identifier :" << rAlgorithmDesc.getClassIdentifier().toString(),
						ErrorType::BadResourceCreation);

	getLogManager() << LogLevel_Debug << "Creating algorithm with class identifier " << rAlgorithmDesc.getClassIdentifier() << "\n";

	CIdentifier algorithmId         = getUnusedIdentifier();
	CAlgorithmProxy* algorithmProxy = new CAlgorithmProxy(getKernelContext(), *algorithm, rAlgorithmDesc);
	m_vAlgorithms[algorithmId]      = algorithmProxy;
	return algorithmId;
}


bool CAlgorithmManager::releaseAlgorithm(const CIdentifier& rAlgorithmIdentifier)
{
	std::unique_lock<std::mutex> lock(m_oMutex);

	const auto itAlgorithm = m_vAlgorithms.find(rAlgorithmIdentifier);

	OV_ERROR_UNLESS_KRF(itAlgorithm != m_vAlgorithms.end(),
						"Algorithm release failed, identifier :" << rAlgorithmIdentifier.toString(),
						ErrorType::ResourceNotFound);

	getLogManager() << LogLevel_Debug << "Releasing algorithm with identifier " << rAlgorithmIdentifier << "\n";
	CAlgorithmProxy* algorithmProxy = itAlgorithm->second;
	if (algorithmProxy)
	{
		IAlgorithm& algorithm = algorithmProxy->getAlgorithm();

		delete algorithmProxy;
		algorithmProxy = nullptr;

		getKernelContext().getPluginManager().releasePluginObject(&algorithm);
	}
	m_vAlgorithms.erase(itAlgorithm);

	return true;
}

bool CAlgorithmManager::releaseAlgorithm(IAlgorithmProxy& rAlgorithm)
{
	std::unique_lock<std::mutex> lock(m_oMutex);

	bool result = false;
	for (auto& algorithm : m_vAlgorithms)
	{
		CAlgorithmProxy* algorithmProxy = algorithm.second;
		if (algorithmProxy == &rAlgorithm)
		{
			IAlgorithm& tmpAlgorithm = algorithmProxy->getAlgorithm();
			getLogManager() << LogLevel_Debug << "Releasing algorithm with class id " << tmpAlgorithm.getClassIdentifier() << "\n";

			delete algorithmProxy;
			algorithmProxy = nullptr;

			m_vAlgorithms.erase(algorithm.first);
			getKernelContext().getPluginManager().releasePluginObject(&tmpAlgorithm);
			result = true;
			break;
		}
	}

	OV_ERROR_UNLESS_KRF(result, "Algorithm release failed", ErrorType::ResourceNotFound);

	return result;
}

IAlgorithmProxy& CAlgorithmManager::getAlgorithm(const CIdentifier& rAlgorithmIdentifier)
{
	std::unique_lock<std::mutex> lock(m_oMutex);

	const auto itAlgorithm = m_vAlgorithms.find(rAlgorithmIdentifier);

	OV_FATAL_UNLESS_K(itAlgorithm != m_vAlgorithms.end(), "Algorithm " << rAlgorithmIdentifier << " does not exist !", ErrorType::ResourceNotFound);

	return *itAlgorithm->second;
}

CIdentifier CAlgorithmManager::getNextAlgorithmIdentifier(const CIdentifier& previousID) const
{
	std::unique_lock<std::mutex> lock(m_oMutex);

	auto itAlgorithm = m_vAlgorithms.begin();
	if (previousID != OV_UndefinedIdentifier)
	{
		itAlgorithm = m_vAlgorithms.find(previousID);
		if (itAlgorithm == m_vAlgorithms.end()) { return OV_UndefinedIdentifier; }
		++itAlgorithm;
	}
	return (itAlgorithm != m_vAlgorithms.end() ? itAlgorithm->first : OV_UndefinedIdentifier);
}

CIdentifier CAlgorithmManager::getUnusedIdentifier() const
{
	std::unique_lock<std::mutex> lock(m_oMutex);

	uint64_t identifier = CIdentifier::random().toUInteger();
	CIdentifier result;

	AlgorithmMap::const_iterator i;
	do
	{
		identifier++;
		result = CIdentifier(identifier);
		i      = m_vAlgorithms.find(result);
	} while (i != m_vAlgorithms.end() || result == OV_UndefinedIdentifier);

	return result;
}
