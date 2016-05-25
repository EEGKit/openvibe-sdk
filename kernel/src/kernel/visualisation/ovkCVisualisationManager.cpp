#include "ovkCVisualisationTree.h"
#include "ovkCVisualisationManager.h"
#include "../ovkGtkOVCustom.h"

#if defined(TARGET_HAS_ThirdPartyGTK)

#if defined TARGET_OS_Windows
#  include <gdk/gdkwin32.h>
#elif defined TARGET_OS_Linux
#  include <gdk/gdkx.h>
#elif defined TARGET_OS_MacOS
#  define Cursor XCursor
#  include <gdk/gdkx.h>
#  undef Cursor
#else
#endif

#endif

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace OpenViBE::Tools;

#define uint8  OpenViBE::uint8
#define uint16 OpenViBE::uint16
#define uint32 OpenViBE::uint32
#define uint64 OpenViBE::uint64
#define boolean OpenViBE::boolean

CVisualisationManager::CVisualisationManager(const IKernelContext& rKernelContext)
	:TKernelObject<IVisualisationManager>(rKernelContext)
{
}

CVisualisationManager::~CVisualisationManager()
{
}
	
boolean CVisualisationManager::createVisualisationTree(CIdentifier& rVisualisationTreeIdentifier)
{
	//create visualisation tree
	IVisualisationTree* l_pVisualisationTree =
		CKernelObjectFactoryHelper(getKernelContext().getKernelObjectFactory()).createObject<IVisualisationTree*>(OV_ClassId_Kernel_Visualisation_VisualisationTree);

	//generate an identifier for visualisation tree
	rVisualisationTreeIdentifier=getUnusedIdentifier();

	//store pointer to visualisation tree
	m_vVisualisationTree[rVisualisationTreeIdentifier] = l_pVisualisationTree;

	return true;
}

boolean CVisualisationManager::releaseVisualisationTree(const CIdentifier& rVisualisationTreeIdentifier)
{
	map<CIdentifier, IVisualisationTree*>::iterator it = m_vVisualisationTree.find(rVisualisationTreeIdentifier);
	if(it != m_vVisualisationTree.end())
	{
		CKernelObjectFactoryHelper(getKernelContext().getKernelObjectFactory()).releaseObject(it->second);
		m_vVisualisationTree.erase(it);
		return true;
	}

	return false;
}

IVisualisationTree& CVisualisationManager::getVisualisationTree(const CIdentifier& rVisualisationTreeIdentifier)
{
	map<CIdentifier, IVisualisationTree*>::const_iterator it = m_vVisualisationTree.find(rVisualisationTreeIdentifier);
	if(it == m_vVisualisationTree.end())
	{
		this->getLogManager() << LogLevel_Fatal << "Visualisation Tree " << rVisualisationTreeIdentifier << " does not exist !\n";
	}
	return *it->second;
}

boolean CVisualisationManager::enumerateVisualisationTrees(IVisualisationManager::IVisualisationTreeEnum& rCallback) const
{
	map<CIdentifier, IVisualisationTree*>::const_iterator it;
	for(it = m_vVisualisationTree.begin(); it != m_vVisualisationTree.end(); it++)
	{
		if(!rCallback.callback(it->first, *it->second))
		{
			return true;
		}
	}
	return true;
}

boolean CVisualisationManager::setToolbar(const CIdentifier& rVisualisationTreeIdentifier, const CIdentifier& rBoxIdentifier, ::GtkWidget* pToolbar)
{
	IVisualisationTree& l_rVisualisationTree = getVisualisationTree(rVisualisationTreeIdentifier);

	l_rVisualisationTree.setToolbar(rBoxIdentifier, pToolbar);

	return true;
}

boolean CVisualisationManager::setWidget(const CIdentifier& rVisualisationTreeIdentifier, const CIdentifier& rBoxIdentifier, ::GtkWidget* pTopmostWidget)
{
	IVisualisationTree& l_rVisualisationTree = getVisualisationTree(rVisualisationTreeIdentifier);

	l_rVisualisationTree.setWidget(rBoxIdentifier, pTopmostWidget);

	return true;
}

CIdentifier CVisualisationManager::getUnusedIdentifier(void) const
{
	uint64 l_ui64Identifier=(((uint64)rand())<<32)+((uint64)rand());
	CIdentifier l_oResult;
	map<CIdentifier, IVisualisationTree*>::const_iterator it;
	do
	{
		l_oResult=CIdentifier(l_ui64Identifier++);
		it=m_vVisualisationTree.find(l_oResult);
	}
	while(it!=m_vVisualisationTree.end() || l_oResult==OV_UndefinedIdentifier);
	return l_oResult;
}

