#pragma once

#include "../ovkTKernelObject.h"

#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CPlayer;

		class CPlayerManager : public TKernelObject<IPlayerManager>
		{
		public:

			explicit CPlayerManager(const IKernelContext& ctx);
			bool createPlayer(CIdentifier& rPlayerIdentifier) override;
			bool releasePlayer(const CIdentifier& rPlayerIdentifier) override;
			IPlayer& getPlayer(const CIdentifier& rPlayerIdentifier) override;
			CIdentifier getNextPlayerIdentifier(const CIdentifier& previousID) const override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IPlayerManager>, OVK_ClassId_Kernel_Player_PlayerManager)

		protected:

			virtual CIdentifier getUnusedIdentifier() const;

			std::map<CIdentifier, CPlayer*> m_vPlayer;
		};
	} // namespace Kernel
} // namespace OpenViBE
