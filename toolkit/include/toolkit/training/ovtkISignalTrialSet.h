#pragma once

#include "../ovtkIObject.h"

namespace OpenViBEToolkit
{
	class ISignalTrial;

	class OVTK_API ISignalTrialSet : public IObject
	{
	public:

		virtual bool addSignalTrial(ISignalTrial& rSignalTrial) = 0;
		virtual bool clear(void) = 0;

		virtual uint32_t getSignalTrialCount(void) const = 0;
		virtual ISignalTrial& getSignalTrial(uint32_t ui32Index) const = 0;

		_IsDerivedFromClass_(OpenViBEToolkit::IObject, OVTK_ClassId_);
	};

	extern OVTK_API ISignalTrialSet* createSignalTrialSet(void);
	extern OVTK_API void releaseSignalTrialSet(ISignalTrialSet* pSignalTrialSet);
};


