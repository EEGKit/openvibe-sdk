#include "ovkCPlayerManager.h"
#include "ovkCPlayer.h"

#include "../../ovk_tools.h"

#include <system/ovCMath.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace std;

bool CPlayerManager::createPlayer(CIdentifier& playerID)
{
	playerID            = getUnusedIdentifier();
	m_vPlayer[playerID] = new CPlayer(getKernelContext());
	return true;
}

bool CPlayerManager::releasePlayer(const CIdentifier& playerID)
{
	auto itPlayer = m_vPlayer.find(playerID);

	OV_ERROR_UNLESS_KRF(itPlayer != m_vPlayer.end(), "Player release failed, identifier :" << playerID.toString(), ErrorType::ResourceNotFound);

	delete itPlayer->second;
	m_vPlayer.erase(itPlayer);
	return true;
}

IPlayer& CPlayerManager::getPlayer(const CIdentifier& playerID)
{
	const auto it = m_vPlayer.find(playerID);

	// use fatal here because the signature does not allow
	// proper checking
	OV_FATAL_UNLESS_K(it != m_vPlayer.end(), "Trying to retrieve non existing player with id " << playerID.toString(), ErrorType::ResourceNotFound);

	// use a fatal here because failing to meet this invariant
	// means there is a bug in the manager implementation
	OV_FATAL_UNLESS_K(it->second, "Null player found for id " << playerID.toString(), ErrorType::BadValue);

	return *it->second;
}

CIdentifier CPlayerManager::getUnusedIdentifier() const
{
	uint64_t id = System::Math::randomUInteger64();
	CIdentifier res;
	map<CIdentifier, CPlayer*>::const_iterator i;
	do
	{
		id++;
		res = CIdentifier(id);
		i   = m_vPlayer.find(res);
	} while (i != m_vPlayer.end() || res == OV_UndefinedIdentifier);
	return res;
}
