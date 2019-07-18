#pragma once

#include "../ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class CSimulatedBox;

		class CBoxAlgorithmLogManager final : public ILogManager
		{
		public:
			CBoxAlgorithmLogManager(const IPlayerContext& playerContext, ILogManager& logManager, CSimulatedBox& simulatedBox)
				: m_PlayerContext(playerContext)
				  , m_LogManager(logManager)
				  , m_SimulatedBox(simulatedBox) {}

			bool isActive(ELogLevel eLogLevel) override { return m_LogManager.isActive(eLogLevel); }

			bool activate(ELogLevel eLogLevel, bool bActive) override { return m_LogManager.activate(eLogLevel, bActive); }
			bool activate(ELogLevel eStartLogLevel, ELogLevel eEndLogLevel, bool bActive) override { return m_LogManager.activate(eStartLogLevel, eEndLogLevel, bActive); }
			bool activate(bool bActive) override { return m_LogManager.activate(bActive); }

			bool addListener(ILogListener* listener) override { return m_LogManager.addListener(listener); }
			bool removeListener(ILogListener* listener) override { return m_LogManager.removeListener(listener); }

			void log(const time64 value) override { m_LogManager.log(value); }
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
			void log(const CString& value) override { m_LogManager.log(value); }
			void log(const char* value) override { m_LogManager.log(value); }
			void log(const CIdentifier& value) override { m_LogManager.log(value); }
			void log(const ELogColor value) override { m_LogManager.log(value); }

			void log(ELogLevel logLevel) override;

			CIdentifier getClassIdentifier(void) const override { return CIdentifier(); }

		private:
			const IPlayerContext& m_PlayerContext;
			ILogManager& m_LogManager;
			CSimulatedBox& m_SimulatedBox;
		};
	}
}
