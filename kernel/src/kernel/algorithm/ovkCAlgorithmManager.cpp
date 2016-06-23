#include "ovkCAlgorithmManager.h"
#include "ovkCAlgorithmProxy.h"

#include <system/ovCMath.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;
using namespace std;

CAlgorithmManager::CAlgorithmManager(const IKernelContext& rKernelContext)
	:TKernelObject<IAlgorithmManager>(rKernelContext)
{
}

CAlgorithmManager::~CAlgorithmManager(void)
{
	for(auto& algorithm : m_vAlgorithms)
	{
		CAlgorithmProxy* l_pAlgorithmProxy = algorithm.second;
		IAlgorithm& l_rAlgorithm = l_pAlgorithmProxy->getAlgorithm();
		delete l_pAlgorithmProxy;
		
		getKernelContext().getPluginManager().releasePluginObject(&l_rAlgorithm);
	}
	
	m_vAlgorithms.clear();
}

CIdentifier CAlgorithmManager::createAlgorithm(
	const CIdentifier& rAlgorithmClassIdentifier)
{
	const IAlgorithmDesc* l_pAlgorithmDesc=NULL;
	IAlgorithm* l_pAlgorithm=getKernelContext().getPluginManager().createAlgorithm(rAlgorithmClassIdentifier, &l_pAlgorithmDesc);
	if(!l_pAlgorithm || !l_pAlgorithmDesc)
	{
		getLogManager() << LogLevel_Warning << "Algorithm creation failed, class identifier :" << rAlgorithmClassIdentifier << "\n";
		return OV_UndefinedIdentifier;
	}

	getLogManager() << LogLevel_Debug << "Creating algorithm with class identifier " << rAlgorithmClassIdentifier << "\n";

	CIdentifier l_oAlgorithmIdentifier=getUnusedIdentifier();
	CAlgorithmProxy* l_pAlgorithmProxy=new CAlgorithmProxy(getKernelContext(), *l_pAlgorithm, *l_pAlgorithmDesc);
	
	m_vAlgorithms[l_oAlgorithmIdentifier]=l_pAlgorithmProxy;
	return l_oAlgorithmIdentifier;
}

CIdentifier CAlgorithmManager::createAlgorithm(
		const IAlgorithmDesc& rAlgorithmDesc)
{
	IAlgorithm* l_pAlgorithm=getKernelContext().getPluginManager().createAlgorithm(rAlgorithmDesc);
	if(!l_pAlgorithm)
	{
		this->getLogManager() << LogLevel_Warning << "Algorithm creation failed, class identifier :" << rAlgorithmDesc.getClassIdentifier() << "\n";
		return OV_UndefinedIdentifier;
	}

	getLogManager() << LogLevel_Debug << "Creating algorithm with class identifier " << rAlgorithmDesc.getClassIdentifier() << "\n";

	CIdentifier l_oAlgorithmIdentifier=getUnusedIdentifier();
	CAlgorithmProxy* l_pAlgorithmProxy=new CAlgorithmProxy(getKernelContext(), *l_pAlgorithm, rAlgorithmDesc);
	m_vAlgorithms[l_oAlgorithmIdentifier]=l_pAlgorithmProxy;
	return l_oAlgorithmIdentifier;
}


boolean CAlgorithmManager::releaseAlgorithm(
	const CIdentifier& rAlgorithmIdentifier)
{
	AlgorithmMap::iterator itAlgorithm = m_vAlgorithms.find(rAlgorithmIdentifier);
	if(itAlgorithm==m_vAlgorithms.end())
	{
		getLogManager() << LogLevel_Warning << "Algorithm release failed, identifier " << rAlgorithmIdentifier << "\n";
		return false;
	}
	getLogManager() << LogLevel_Debug << "Releasing algorithm with identifier " << rAlgorithmIdentifier << "\n";
	CAlgorithmProxy* l_pAlgorithmProxy = itAlgorithm->second;
	if(l_pAlgorithmProxy) 
	{		
		IAlgorithm& l_rAlgorithm=l_pAlgorithmProxy->getAlgorithm();

		delete l_pAlgorithmProxy;
		l_pAlgorithmProxy = NULL;
		
		getKernelContext().getPluginManager().releasePluginObject(&l_rAlgorithm);
	}
	m_vAlgorithms.erase(itAlgorithm);

	return true;
}

boolean CAlgorithmManager::releaseAlgorithm(
	IAlgorithmProxy& rAlgorithm)
{	
	for(auto& algorithm : m_vAlgorithms)
	{
		CAlgorithmProxy* l_pAlgorithmProxy = algorithm.second;
		if(l_pAlgorithmProxy==&rAlgorithm)
		{
			IAlgorithm& l_rAlgorithm=l_pAlgorithmProxy->getAlgorithm();
			getLogManager() << LogLevel_Debug << "Releasing algorithm with class id " << l_rAlgorithm.getClassIdentifier() << "\n";

			delete l_pAlgorithmProxy;
			l_pAlgorithmProxy = NULL;
				
			m_vAlgorithms.erase(algorithm.first);
			getKernelContext().getPluginManager().releasePluginObject(&l_rAlgorithm);
			return true;
		}
	}
	
	getLogManager() << LogLevel_Warning << "Algorithm release failed\n";
	return false;
}

IAlgorithmProxy& CAlgorithmManager::getAlgorithm(
	const CIdentifier& rAlgorithmIdentifier)
{
	AlgorithmMap::const_iterator itAlgorithm = m_vAlgorithms.find(rAlgorithmIdentifier);
	if(itAlgorithm==m_vAlgorithms.end())
	{
		this->getLogManager() << LogLevel_Fatal << "Algorithm " << rAlgorithmIdentifier << " does not exist !\n";
	}
	return *itAlgorithm->second;
}

CIdentifier CAlgorithmManager::getNextAlgorithmIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	AlgorithmMap::const_iterator itAlgorithm=m_vAlgorithms.begin();

	if(rPreviousIdentifier==OV_UndefinedIdentifier)
	{
		itAlgorithm=m_vAlgorithms.begin();
	}
	else
	{
		itAlgorithm=m_vAlgorithms.find(rPreviousIdentifier);
		if(itAlgorithm==m_vAlgorithms.end())
		{
			return OV_UndefinedIdentifier;
		}
		itAlgorithm++;
	}

	return itAlgorithm!=m_vAlgorithms.end()?itAlgorithm->first:OV_UndefinedIdentifier;
}

CIdentifier CAlgorithmManager::getUnusedIdentifier(void) const
{
	uint64 l_ui64Identifier=System::Math::randomUInteger64();
	CIdentifier l_oResult;
	AlgorithmMap::const_iterator i;
	do
	{
		l_ui64Identifier++;
		l_oResult=CIdentifier(l_ui64Identifier);
		i=m_vAlgorithms.find(l_oResult);
	}
	while(i!=m_vAlgorithms.end() || l_oResult==OV_UndefinedIdentifier);
	return l_oResult;
}
