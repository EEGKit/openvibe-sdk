#pragma once

#include "defines.h"

namespace System
{
	class System_API CChrono
	{
	public:

		CChrono(void);
		virtual ~CChrono(void);

		virtual bool reset(System::uint32 ui32StepCount);

		virtual bool stepIn(void);
		virtual bool stepOut(void);

		virtual System::uint64 getTotalStepInDuration(void) const;
		virtual System::uint64 getTotalStepOutDuration(void) const;
		virtual System::uint64 getAverageStepInDuration(void) const;
		virtual System::uint64 getAverageStepOutDuration(void) const;
		virtual double getStepInPercentage(void) const;
		virtual double getStepOutPercentage(void) const;

		virtual bool hasNewEstimation(void);

	private:

		System::uint64* m_pStepInTime;
		System::uint64* m_pStepOutTime;
		System::uint32 m_ui32StepCount;
		System::uint32 m_ui32StepIndex;
		bool m_bIsInStep;
		bool m_bHasNewEstimation;

		System::uint64 m_ui64TotalStepInTime;
		System::uint64 m_ui64TotalStepOutTime;
	};
};
