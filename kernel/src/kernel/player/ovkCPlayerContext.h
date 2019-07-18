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

		class CPlayerContext : public TKernelObject<IPlayerContext>
		{
		public:

			CPlayerContext(const IKernelContext& rKernelContext, CSimulatedBox* pSimulatedBox);
			virtual ~CPlayerContext();

			virtual bool sendSignal(
				const CMessageSignal& rMessageSignal);
			virtual bool sendMessage(
				const CMessageEvent& rMessageEvent,
				const CIdentifier& rTargetIdentifier);
			virtual bool sendMessage(
				const CMessageEvent& rMessageEvent,
				const CIdentifier* pTargetIdentifier,
				const uint32_t ui32TargetIdentifierCount);

			virtual uint64_t getCurrentTime(void) const;
			virtual uint64_t getCurrentLateness(void) const;
			virtual double getCurrentCPUUsage(void) const;
			virtual double getCurrentFastForwardMaximumFactor(void) const;

			virtual bool stop(void);
			virtual bool pause(void);
			virtual bool play(void);
			virtual bool forward(void);
			virtual EPlayerStatus getStatus(void) const;

			//@}

			virtual IAlgorithmManager& getAlgorithmManager(void) const;
			virtual IConfigurationManager& getConfigurationManager(void) const;
			virtual ILogManager& getLogManager(void) const;
			virtual IErrorManager& getErrorManager(void) const;
			virtual IScenarioManager& getScenarioManager(void) const;
			virtual ITypeManager& getTypeManager(void) const;
			bool canCreatePluginObject(const CIdentifier& pluginIdentifier) const;
			Plugins::IPluginObject* createPluginObject(const CIdentifier& pluginIdentifier) const;
			bool releasePluginObject(Plugins::IPluginObject* pluginObject) const;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IPlayerContext>, OVK_ClassId_Kernel_Player_PlayerContext)

		private:

			CSimulatedBox& m_rSimulatedBox;
			IPluginManager& m_rPluginManager;
			IAlgorithmManager& m_rAlgorithmManager;
			IConfigurationManager& m_rConfigurationManager;
			ILogManager& m_rLogManager;
			IErrorManager& m_rErrorManager;
			IScenarioManager& m_rScenarioManager;
			ITypeManager& m_rTypeManager;
			mutable CBoxAlgorithmLogManager m_BoxLogManager;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Player_CPlayerContext_H__
