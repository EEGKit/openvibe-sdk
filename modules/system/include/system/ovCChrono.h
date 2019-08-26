#pragma once

#include "defines.h"

namespace System
{
	class System_API CChrono
	{
	public:

		CChrono();
		virtual ~CChrono();

		virtual bool reset(uint32_t ui32StepCount);

		virtual bool stepIn();
		virtual bool stepOut();

		virtual uint64_t getTotalStepInDuration() const;
		virtual uint64_t getTotalStepOutDuration() const;
		virtual uint64_t getAverageStepInDuration() const;
		virtual uint64_t getAverageStepOutDuration() const;
		virtual double getStepInPercentage() const;
		virtual double getStepOutPercentage() const;

		virtual bool hasNewEstimation();

	private:

		uint64_t* m_pStepInTime = nullptr;
		uint64_t* m_pStepOutTime = nullptr;
		uint32_t m_ui32StepCount = 0;
		uint32_t m_ui32StepIndex = 0;
		bool m_bIsInStep = false;
		bool m_bHasNewEstimation = false;

		uint64_t m_ui64TotalStepInTime = 0;
		uint64_t m_ui64TotalStepOutTime = 0;
	};
} // namespace System
