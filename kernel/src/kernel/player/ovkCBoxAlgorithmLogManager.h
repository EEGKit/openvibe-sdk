#pragma once

#include "../ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CSimulatedBox;

		class CBoxAlgorithmLogManager final : public OpenViBE::Kernel::ILogManager
		{
		public:
			CBoxAlgorithmLogManager(const OpenViBE::Kernel::IPlayerContext& playerContext, OpenViBE::Kernel::ILogManager& logManager, OpenViBE::Kernel::CSimulatedBox& simulatedBox)
				: m_PlayerContext(playerContext)
				  , m_LogManager(logManager)
				  , m_SimulatedBox(simulatedBox) {}

			bool isActive(OpenViBE::Kernel::ELogLevel eLogLevel) override { return m_LogManager.isActive(eLogLevel); }

			bool activate(OpenViBE::Kernel::ELogLevel eLogLevel, bool bActive) override { return m_LogManager.activate(eLogLevel, bActive); }
			bool activate(OpenViBE::Kernel::ELogLevel eStartLogLevel, OpenViBE::Kernel::ELogLevel eEndLogLevel, bool bActive) override { return m_LogManager.activate(eStartLogLevel, eEndLogLevel, bActive); }
			bool activate(bool bActive) override { return m_LogManager.activate(bActive); }

			bool addListener(OpenViBE::Kernel::ILogListener* listener) override { return m_LogManager.addListener(listener); }
			bool removeListener(OpenViBE::Kernel::ILogListener* listener) override { return m_LogManager.removeListener(listener); }

			void log(const OpenViBE::time64 value) override { m_LogManager.log(value); }
			void log(const uint64_t value) override { m_LogManager.log(value); }
			void log(const uint32_t value) override { m_LogManager.log(value); }
			void log(const uint16_t value) override { m_LogManager.log(value); }
			void log(const uint8_t value) override { m_LogManager.log(value); }
			void log(const int64_t value) override { m_LogManager.log(value); }
			void log(const int32_t value) override { m_LogManager.log(value); }
			void log(const int16_t value) override { m_LogManager.log(value); }
			void log(const int8_t value) override { m_LogManager.log(value); }
			void log(const float value) override { m_LogManager.log(value); }
			void log(const double value) override { m_LogManager.log(value); }
			void log(const bool value) override { m_LogManager.log(value); }
			void log(const OpenViBE::CString& value) override { m_LogManager.log(value); }
			void log(const char* value) override { m_LogManager.log(value); }
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
