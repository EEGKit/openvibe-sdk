#ifndef __OpenViBEKernel_Kernel_Player_CPlayerManager_H__
#define __OpenViBEKernel_Kernel_Player_CPlayerManager_H__

#include "../ovkTKernelObject.h"

#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CPlayer;

		class CPlayerManager : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IPlayerManager>
		{
		public:

			explicit CPlayerManager(const OpenViBE::Kernel::IKernelContext& rKernelContext);

			virtual bool createPlayer(
				OpenViBE::CIdentifier& rPlayerIdentifier);
			virtual bool releasePlayer(
				const OpenViBE::CIdentifier& rPlayerIdentifier);
			virtual OpenViBE::Kernel::IPlayer& getPlayer(
				const OpenViBE::CIdentifier& rPlayerIdentifier);
			virtual OpenViBE::CIdentifier getNextPlayerIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IPlayerManager>, OVK_ClassId_Kernel_Player_PlayerManager);

		protected:

			virtual OpenViBE::CIdentifier getUnusedIdentifier(void) const;

		protected:

			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CPlayer*> m_vPlayer;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Player_CPlayerManager_H__
