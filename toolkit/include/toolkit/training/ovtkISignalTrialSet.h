#pragma once

#include "../ovtkIObject.h"

namespace OpenViBEToolkit
{
	class ISignalTrial;

	class OVTK_API ISignalTrialSet : public IObject
	{
	public:

		virtual bool addSignalTrial(ISignalTrial& signalTrial) = 0;
		virtual bool clear() = 0;

		virtual size_t getSignalTrialCount() const = 0;
		virtual ISignalTrial& getSignalTrial(const size_t index) const = 0;

		_IsDerivedFromClass_(OpenViBEToolkit::IObject, OVTK_ClassId_)
	};

	extern OVTK_API ISignalTrialSet* createSignalTrialSet();
	extern OVTK_API void releaseSignalTrialSet(ISignalTrialSet* signalTrialSet);
} // namespace OpenViBEToolkit
