#pragma once

#include "defines.h"

namespace System
{
	class System_API CChrono
	{
	public:

		CChrono(void);
		virtual ~CChrono(void);

		virtual bool reset(uint32_t ui32StepCount);

		virtual bool stepIn(void);
		virtual bool stepOut(void);

		virtual uint64_t getTotalStepInDuration(void) const;
		virtual uint64_t getTotalStepOutDuration(void) const;
		virtual uint64_t getAverageStepInDuration(void) const;
		virtual uint64_t getAverageStepOutDuration(void) const;
		virtual double getStepInPercentage(void) const;
		virtual double getStepOutPercentage(void) const;

		virtual bool hasNewEstimation(void);

	private:

		uint64_t* m_pStepInTime;
		uint64_t* m_pStepOutTime;
		uint32_t m_ui32StepCount;
		uint32_t m_ui32StepIndex;
		bool m_bIsInStep;
		bool m_bHasNewEstimation;

		uint64_t m_ui64TotalStepInTime;
		uint64_t m_ui64TotalStepOutTime;
	};
};
