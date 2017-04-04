#ifndef __OpenViBEKernel_Kernel_Player_CPlayerContext_H__
#define __OpenViBEKernel_Kernel_Player_CPlayerContext_H__

#include "ovkCBoxAlgorithmLogManager.h"
#include "../ovkTKernelObject.h"
#include <memory>

namespace OpenViBE
{
	namespace Kernel
	{
		class CSimulatedBox;

		class CPlayerContext : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IPlayerContext>
		{
		public:

			CPlayerContext(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::CSimulatedBox* pSimulatedBox);
			virtual ~CPlayerContext();

			virtual OpenViBE::boolean sendSignal(
				const OpenViBE::CMessageSignal& rMessageSignal);
			virtual OpenViBE::boolean sendMessage(
				const OpenViBE::CMessageEvent& rMessageEvent,
				const OpenViBE::CIdentifier& rTargetIdentifier);
			virtual OpenViBE::boolean sendMessage(
				const OpenViBE::CMessageEvent& rMessageEvent,
				const OpenViBE::CIdentifier* pTargetIdentifier,
				const OpenViBE::uint32 ui32TargetIdentifierCount);

			virtual OpenViBE::uint64 getCurrentTime(void) const;
			virtual OpenViBE::uint64 getCurrentLateness(void) const;
			virtual OpenViBE::float64 getCurrentCPUUsage(void) const;
			virtual OpenViBE::float64 getCurrentFastForwardMaximumFactor(void) const;

			virtual OpenViBE::boolean stop(void);
			virtual OpenViBE::boolean pause(void);
			virtual OpenViBE::boolean play(void);
			virtual OpenViBE::boolean forward(void);
			virtual OpenViBE::Kernel::EPlayerStatus getStatus(void) const;

			//@}

			virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager(void) const;
			virtual OpenViBE::Kernel::IConfigurationManager& getConfigurationManager(void) const;
			virtual OpenViBE::Kernel::ILogManager& getLogManager(void) const;
			virtual OpenViBE::Kernel::IErrorManager& getErrorManager(void) const;
			virtual OpenViBE::Kernel::IScenarioManager& getScenarioManager(void) const;
			virtual OpenViBE::Kernel::ITypeManager& getTypeManager(void) const;
			bool canCreatePluginObject(const OpenViBE::CIdentifier& pluginIdentifier) const;
			OpenViBE::Plugins::IPluginObject* createPluginObject(const OpenViBE::CIdentifier& pluginIdentifier) const;
			bool releasePluginObject(OpenViBE::Plugins::IPluginObject* pluginObject) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IPlayerContext>, OVK_ClassId_Kernel_Player_PlayerContext)

		private:

			OpenViBE::Kernel::CSimulatedBox& m_rSimulatedBox;
			OpenViBE::Kernel::IPluginManager& m_rPluginManager;
			OpenViBE::Kernel::IAlgorithmManager& m_rAlgorithmManager;
			OpenViBE::Kernel::IConfigurationManager& m_rConfigurationManager;
			OpenViBE::Kernel::ILogManager& m_rLogManager;
			OpenViBE::Kernel::IErrorManager& m_rErrorManager;
			OpenViBE::Kernel::IScenarioManager& m_rScenarioManager;
			OpenViBE::Kernel::ITypeManager& m_rTypeManager;
			mutable OpenViBE::Kernel::CBoxAlgorithmLogManager m_BoxLogManager;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Player_CPlayerContext_H__
