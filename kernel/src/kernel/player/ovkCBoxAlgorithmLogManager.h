#pragma once

#include "../ovkTKernelObject.h"

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


	}
}
