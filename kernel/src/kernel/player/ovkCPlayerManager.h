#ifndef __OpenViBEKernel_Kernel_Player_CPlayerManager_H__
#define __OpenViBEKernel_Kernel_Player_CPlayerManager_H__

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

			explicit CPlayerManager(const IKernelContext& rKernelContext);

			virtual bool createPlayer(
				CIdentifier& rPlayerIdentifier);
			virtual bool releasePlayer(
				const CIdentifier& rPlayerIdentifier);
			virtual IPlayer& getPlayer(
				const CIdentifier& rPlayerIdentifier);
			virtual CIdentifier getNextPlayerIdentifier(
				const CIdentifier& rPreviousIdentifier) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IPlayerManager>, OVK_ClassId_Kernel_Player_PlayerManager);

		protected:

			virtual CIdentifier getUnusedIdentifier(void) const;

		protected:

			std::map<CIdentifier, CPlayer*> m_vPlayer;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Player_CPlayerManager_H__
