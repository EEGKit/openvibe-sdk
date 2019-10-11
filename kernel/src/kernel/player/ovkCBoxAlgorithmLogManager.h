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
				: m_PlayerContext(playerContext), m_LogManager(logManager), m_SimulatedBox(simulatedBox) {}

			bool isActive(ELogLevel level) override { return m_LogManager.isActive(level); }

			bool activate(ELogLevel level, bool active) override { return m_LogManager.activate(level, active); }

			bool activate(ELogLevel eStartLogLevel, ELogLevel eEndLogLevel, bool active) override
			{
				return m_LogManager.activate(eStartLogLevel, eEndLogLevel, active);
			}

			bool activate(bool active) override { return m_LogManager.activate(active); }

			bool addListener(ILogListener* listener) override { return m_LogManager.addListener(listener); }
			bool removeListener(ILogListener* listener) override { return m_LogManager.removeListener(listener); }

			void log(const time64 value) override { m_LogManager.log(value); }
			void log(const uint64_t value) override { m_LogManager.log(value); }
			void log(const uint32_t value) override { m_LogManager.log(value); }
			void log(const int64_t value) override { m_LogManager.log(value); }
			void log(const int value) override { m_LogManager.log(value); }
			void log(const float value) override { m_LogManager.log(value); }
			void log(const double value) override { m_LogManager.log(value); }
			void log(const bool value) override { m_LogManager.log(value); }
			void log(const CString& value) override { m_LogManager.log(value); }
			void log(const std::string& value) override { m_LogManager.log(value); }
			void log(const char* value) override { m_LogManager.log(value); }
			void log(const CIdentifier& value) override { m_LogManager.log(value); }
			void log(const ELogColor value) override { m_LogManager.log(value); }
			void log(const ELogLevel logLevel) override;

			CIdentifier getClassIdentifier() const override { return CIdentifier(); }

		private:
			const IPlayerContext& m_PlayerContext;
			ILogManager& m_LogManager;
			CSimulatedBox& m_SimulatedBox;
		};
	} // namespace Kernel
} // namespace OpenViBE
