#include "ovpiCPluginObjectDescEnum.h"

#include <iostream>
#include <fstream>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace std;

CPluginObjectDescEnum::CPluginObjectDescEnum(const IKernelContext& rKernelContext)
	:m_rKernelContext(rKernelContext)
{
}

CPluginObjectDescEnum::~CPluginObjectDescEnum(void)
{
}

// Enumerate plugins by iterating over a user defined list of descriptors, used for metaboxes
boolean CPluginObjectDescEnum::enumeratePluginObjectDesc(std::vector<const IPluginObjectDesc*>& vPluginDescriptors)
{
//	for (auto pPlugin : vPluginDescriptors)
	for (auto it = vPluginDescriptors.cbegin(); it != vPluginDescriptors.cend(); it++)
	{
		auto pPlugin = *it;
		this->callback(*pPlugin);
	}
	return true;
}

boolean CPluginObjectDescEnum::enumeratePluginObjectDesc(vector<uint64> * pAuthorizedPluginsIdentifiers)
{
	CIdentifier l_oIdentifier;
	while((l_oIdentifier=m_rKernelContext.getPluginManager().getNextPluginObjectDescIdentifier(l_oIdentifier))!=OV_UndefinedIdentifier)
	{
		if(pAuthorizedPluginsIdentifiers)
		{
			//if a manifest vector has been provided, we filter plugins by ID
			boolean l_bAuthorized = false;
			for(uint32 i =0; i < pAuthorizedPluginsIdentifiers->size() && !l_bAuthorized; i++)
			{
				CIdentifier l_oAuthorizedId((*pAuthorizedPluginsIdentifiers)[i]);
				l_bAuthorized = (l_oIdentifier == l_oAuthorizedId);
			}
			if(l_bAuthorized && !m_rKernelContext.getPluginManager().isPluginObjectFlaggedAsGhost(m_rKernelContext.getPluginManager().getPluginObjectDesc(l_oIdentifier)->getCreatedClass()))
			{
				this->callback(*m_rKernelContext.getPluginManager().getPluginObjectDesc(l_oIdentifier));
			}
		}
		else
		{
			// default behavior: we take everything except ghosts (they run a pop-up on insertion, and we do not need documenttaion for ghosts).
			if(!m_rKernelContext.getPluginManager().isPluginObjectFlaggedAsGhost(m_rKernelContext.getPluginManager().getPluginObjectDesc(l_oIdentifier)->getCreatedClass()))
			{
				this->callback(*m_rKernelContext.getPluginManager().getPluginObjectDesc(l_oIdentifier));
			}
		}
	}
	return true;
}

boolean CPluginObjectDescEnum::enumeratePluginObjectDesc(
	const CIdentifier& rParentClassIdentifier, vector<uint64> * pAuthorizedPluginsIdentifiers)
{
	CIdentifier l_oIdentifier;
	while((l_oIdentifier=m_rKernelContext.getPluginManager().getNextPluginObjectDescIdentifier(l_oIdentifier, rParentClassIdentifier))!=OV_UndefinedIdentifier)
	{
		if(pAuthorizedPluginsIdentifiers)
		{
			//if an exclude vector has been provided, we filter plugins by ID
			boolean l_bAuthorized = false;
			for(uint32 i =0; i < pAuthorizedPluginsIdentifiers->size() && !l_bAuthorized; i++)
			{
				CIdentifier l_oAuthorizedId((*pAuthorizedPluginsIdentifiers)[i]);
				l_bAuthorized = (l_oIdentifier == l_oAuthorizedId);
			}
			if(l_bAuthorized && !m_rKernelContext.getPluginManager().isPluginObjectFlaggedAsGhost(m_rKernelContext.getPluginManager().getPluginObjectDesc(l_oIdentifier)->getCreatedClass()))
			{
				this->callback(*m_rKernelContext.getPluginManager().getPluginObjectDesc(l_oIdentifier));
			}
		}
		else
		{
			// default behavior: we take everything except ghosts (they run a pop-up on insertion, and we do not need documenttaion for ghosts).
			if(!m_rKernelContext.getPluginManager().isPluginObjectFlaggedAsGhost(m_rKernelContext.getPluginManager().getPluginObjectDesc(l_oIdentifier)->getCreatedClass()))
			{
				this->callback(*m_rKernelContext.getPluginManager().getPluginObjectDesc(l_oIdentifier));
			}
		}
	}
	return true;
}

std::string CPluginObjectDescEnum::transform(const std::string& sInput, const boolean bRemoveSlash)
{
	std::string l_sInput(sInput);
	std::string l_sOutput;
	bool l_bLastWasSeparator=true;

	for(std::string::size_type i=0; i<l_sInput.length(); i++)
	{
		if((l_sInput[i]>='a' && l_sInput[i]<='z') || (l_sInput[i]>='A' && l_sInput[i]<='Z') || (l_sInput[i]>='0' && l_sInput[i]<='9') || (!bRemoveSlash && l_sInput[i]=='/'))
		{
			if(l_sInput[i]=='/')
			{
				l_sOutput+="_";
			}
			else
			{
				if(l_bLastWasSeparator)
				{
					if('a' <= l_sInput[i] && l_sInput[i] <= 'z')
					{
						l_sOutput+=l_sInput[i]+'A'-'a';
					}
					else
					{
						l_sOutput+=l_sInput[i];
					}
				}
				else
				{
					l_sOutput+=l_sInput[i];
				}
			}
			l_bLastWasSeparator=false;
		}
		else
		{
/*
			if(!l_bLastWasSeparator)
			{
				l_sOutput+="_";
			}
*/
			l_bLastWasSeparator=true;
		}
	}
	return l_sOutput;
}
