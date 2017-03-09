#ifndef __OpenViBEKernel_Kernel_Player_CPlayerContext_H__
#define __OpenViBEKernel_Kernel_Player_CPlayerContext_H__

#include "../ovkTKernelObject.h"
#include <memory>

namespace OpenViBE
{
	namespace Kernel
	{
		class CSimulatedBox;

		class CBoxAlgorithmLogManager final : public OpenViBE::Kernel::ILogManager {
		public:
			CBoxAlgorithmLogManager(const OpenViBE::Kernel::IPlayerContext& playerContext, OpenViBE::Kernel::ILogManager& logManager, OpenViBE::Kernel::CSimulatedBox& simulatedBox)
			    : m_PlayerContext(playerContext)
			    , m_LogManager(logManager)
			    , m_SimulatedBox(simulatedBox)
			{}

			bool isActive(OpenViBE::Kernel::ELogLevel eLogLevel) override  { return m_LogManager.isActive(eLogLevel); }

			bool activate(OpenViBE::Kernel::ELogLevel eLogLevel, OpenViBE::boolean bActive) override { return m_LogManager.activate(eLogLevel, bActive); }
			bool activate(OpenViBE::Kernel::ELogLevel eStartLogLevel, OpenViBE::Kernel::ELogLevel eEndLogLevel, OpenViBE::boolean bActive) override { return m_LogManager.activate(eStartLogLevel, eEndLogLevel, bActive); }
			bool activate(OpenViBE::boolean bActive) override { return m_LogManager.activate(bActive); }

			bool addListener(OpenViBE::Kernel::ILogListener* listener) override { return m_LogManager.addListener(listener); }
			bool removeListener(OpenViBE::Kernel::ILogListener* listener) override { return m_LogManager.removeListener(listener); }

			void log(const OpenViBE::time64 value) override { m_LogManager.log(value); }
			void log(const OpenViBE::uint64 value) override { m_LogManager.log(value); }
			void log(const OpenViBE::uint32 value) override { m_LogManager.log(value); }
			void log(const OpenViBE::uint16 value) override { m_LogManager.log(value); }
			void log(const OpenViBE::uint8 value) override { m_LogManager.log(value); }
			void log(const OpenViBE::int64 value) override { m_LogManager.log(value); }
			void log(const OpenViBE::int32 value) override { m_LogManager.log(value); }
			void log(const OpenViBE::int16 value) override { m_LogManager.log(value); }
			void log(const OpenViBE::int8 value) override { m_LogManager.log(value); }
			void log(const OpenViBE::float32 value) override { m_LogManager.log(value); }
			void log(const OpenViBE::float64 value) override { m_LogManager.log(value); }
			void log(const OpenViBE::boolean value) override { m_LogManager.log(value); }
			void log(const OpenViBE::CString& value) override { m_LogManager.log(value); }
			void log(const char* value) override { m_LogManager.log(value);}
			void log(const OpenViBE::CIdentifier& value) override { m_LogManager.log(value); }
			void log(const OpenViBE::Kernel::ELogColor value) override { m_LogManager.log(value); }

			void log(const OpenViBE::Kernel::ELogLevel logLevel) override;

			OpenViBE::CIdentifier getClassIdentifier(void) const override { return OpenViBE::CIdentifier(); }

		private:
			const OpenViBE::Kernel::IPlayerContext& m_PlayerContext;
			OpenViBE::Kernel::ILogManager& m_LogManager;
			OpenViBE::Kernel::CSimulatedBox& m_SimulatedBox;
		};

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
