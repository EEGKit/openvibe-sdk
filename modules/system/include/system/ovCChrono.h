#pragma once

#include "defines.h"

namespace System
{
	class System_API CChrono final
	{
	public:

		CChrono();
		~CChrono();

		bool reset(uint32_t ui32StepCount);

		bool stepIn();
		bool stepOut();

		uint64_t getTotalStepInDuration() const;
		uint64_t getTotalStepOutDuration() const;
		uint64_t getAverageStepInDuration() const;
		uint64_t getAverageStepOutDuration() const;
		double getStepInPercentage() const;
		double getStepOutPercentage() const;

		bool hasNewEstimation();

	private:

		uint64_t* m_pStepInTime  = nullptr;
		uint64_t* m_pStepOutTime = nullptr;
		uint32_t m_ui32StepCount = 0;
		uint32_t m_ui32StepIndex = 0;
		bool m_bIsInStep         = false;
		bool m_bHasNewEstimation = false;

		uint64_t m_ui64TotalStepInTime  = 0;
		uint64_t m_ui64TotalStepOutTime = 0;
	};
} // namespace System
