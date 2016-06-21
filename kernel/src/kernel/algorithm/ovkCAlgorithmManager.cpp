#include "ovkCAlgorithmManager.h"
#include "ovkCAlgorithm.h"

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
	AlgorithmMap::iterator itAlgorithm;
	for(itAlgorithm=m_vAlgorithm.begin(); itAlgorithm!=m_vAlgorithm.end(); itAlgorithm++)
	{
		CAlgorithm* l_rAlgorithmWrapper = itAlgorithm->second;		
		IAlgorithm& l_rAlgorithm = l_rAlgorithmWrapper->getAlgorithm();
		delete l_rAlgorithmWrapper;
		
		getKernelContext().getPluginManager().releasePluginObject(&l_rAlgorithm);
	}
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
	CAlgorithm* l_pAlgorithmWrapper=new CAlgorithm(getKernelContext(), *l_pAlgorithm, *l_pAlgorithmDesc);
	
	m_vAlgorithm[l_oAlgorithmIdentifier]=l_pAlgorithmWrapper;
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
	CAlgorithm* l_pAlgorithmWrapper=new CAlgorithm(getKernelContext(), *l_pAlgorithm, rAlgorithmDesc);
	m_vAlgorithm[l_oAlgorithmIdentifier]=l_pAlgorithmWrapper;
	return l_oAlgorithmIdentifier;
}


boolean CAlgorithmManager::releaseAlgorithm(
	const CIdentifier& rAlgorithmIdentifier)
{
	AlgorithmMap::iterator itAlgorithm;
	itAlgorithm=m_vAlgorithm.find(rAlgorithmIdentifier);
	if(itAlgorithm==m_vAlgorithm.end())
	{
		getLogManager() << LogLevel_Warning << "Algorithm release failed, identifier " << rAlgorithmIdentifier << "\n";
		return false;
	}
	getLogManager() << LogLevel_Debug << "Releasing algorithm with identifier " << rAlgorithmIdentifier << "\n";
	CAlgorithm* l_rAlgorithmWrapper = itAlgorithm->second;		
	IAlgorithm& l_rAlgorithm=l_rAlgorithmWrapper->getAlgorithm();
	if(l_rAlgorithmWrapper) 
	{
		delete l_rAlgorithmWrapper;
		l_rAlgorithmWrapper = NULL;
	}
	m_vAlgorithm.erase(itAlgorithm);
	getKernelContext().getPluginManager().releasePluginObject(&l_rAlgorithm);
	return true;
}

boolean CAlgorithmManager::releaseAlgorithm(
	IAlgorithmProxy& rAlgorithm)
{
	AlgorithmMap::iterator itAlgorithm;
	for(itAlgorithm=m_vAlgorithm.begin(); itAlgorithm!=m_vAlgorithm.end(); itAlgorithm++)
	{
		CAlgorithm* l_rAlgorithmWrapper = itAlgorithm->second;	
		if((IAlgorithmProxy*)l_rAlgorithmWrapper==&rAlgorithm)
		{
			IAlgorithm& l_rAlgorithm=l_rAlgorithmWrapper->getAlgorithm();
			getLogManager() << LogLevel_Debug << "Releasing algorithm with class id " << l_rAlgorithm.getClassIdentifier() << "\n";
			if(l_rAlgorithmWrapper) 
			{
				delete l_rAlgorithmWrapper;
				l_rAlgorithmWrapper = NULL;
			}
			m_vAlgorithm.erase(itAlgorithm);
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
	AlgorithmMap::const_iterator itAlgorithm;
	itAlgorithm=m_vAlgorithm.find(rAlgorithmIdentifier);
	if(itAlgorithm==m_vAlgorithm.end())
	{
		this->getLogManager() << LogLevel_Fatal << "Algorithm " << rAlgorithmIdentifier << " does not exist !\n";
	}
	return *itAlgorithm->second;
}

CIdentifier CAlgorithmManager::getNextAlgorithmIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	AlgorithmMap::const_iterator itAlgorithm=m_vAlgorithm.begin();

	if(rPreviousIdentifier==OV_UndefinedIdentifier)
	{
		itAlgorithm=m_vAlgorithm.begin();
	}
	else
	{
		itAlgorithm=m_vAlgorithm.find(rPreviousIdentifier);
		if(itAlgorithm==m_vAlgorithm.end())
		{
			return OV_UndefinedIdentifier;
		}
		itAlgorithm++;
	}

	return itAlgorithm!=m_vAlgorithm.end()?itAlgorithm->first:OV_UndefinedIdentifier;
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
		i=m_vAlgorithm.find(l_oResult);
	}
	while(i!=m_vAlgorithm.end() || l_oResult==OV_UndefinedIdentifier);
	return l_oResult;
}
