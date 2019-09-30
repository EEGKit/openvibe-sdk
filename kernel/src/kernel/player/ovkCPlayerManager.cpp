#include "ovkCPlayerManager.h"
#include "ovkCPlayer.h"

#include "../../ovk_tools.h"

#include <system/ovCMath.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace std;

CPlayerManager::CPlayerManager(const IKernelContext& ctx)
	: TKernelObject<IPlayerManager>(ctx) {}

bool CPlayerManager::createPlayer(CIdentifier& rPlayerIdentifier)
{
	rPlayerIdentifier            = getUnusedIdentifier();
	m_vPlayer[rPlayerIdentifier] = new CPlayer(getKernelContext());
	return true;
}

bool CPlayerManager::releasePlayer(const CIdentifier& rPlayerIdentifier)
{
	auto itPlayer = m_vPlayer.find(rPlayerIdentifier);

	OV_ERROR_UNLESS_KRF(itPlayer != m_vPlayer.end(), "Player release failed, identifier :" << rPlayerIdentifier.toString(), ErrorType::ResourceNotFound);

	delete itPlayer->second;
	m_vPlayer.erase(itPlayer);
	return true;
}

IPlayer& CPlayerManager::getPlayer(const CIdentifier& rPlayerIdentifier)
{
	auto itPlayer = m_vPlayer.find(rPlayerIdentifier);

	// use fatal here because the signature does not allow
	// proper checking
	OV_FATAL_UNLESS_K(itPlayer != m_vPlayer.end(), "Trying to retrieve non existing player with id " << rPlayerIdentifier.toString(),
					  ErrorType::ResourceNotFound);

	// use a fatal here because failing to meet this invariant
	// means there is a bug in the manager implementation
	OV_FATAL_UNLESS_K(itPlayer->second, "Null player found for id " << rPlayerIdentifier.toString(), ErrorType::BadValue);

	return *itPlayer->second;
}

CIdentifier CPlayerManager::getNextPlayerIdentifier(const CIdentifier& previousID) const
{
	return getNextIdentifier<CPlayer*>(m_vPlayer, previousID);
}

CIdentifier CPlayerManager::getUnusedIdentifier() const
{
	uint64_t l_ui64Identifier = System::Math::randomUInteger64();
	CIdentifier res;
	map<CIdentifier, CPlayer*>::const_iterator i;
	do
	{
		l_ui64Identifier++;
		res = CIdentifier(l_ui64Identifier);
		i         = m_vPlayer.find(res);
	} while (i != m_vPlayer.end() || res == OV_UndefinedIdentifier);
	return res;
}
