#pragma once

#include "ovtkISignalTrialSet.h"

#include <vector>

namespace OpenViBEToolkit
{
	class CSignalTrialSet : public ISignalTrialSet
	{
	public:

		virtual bool addSignalTrial(ISignalTrial& rSignalTrial);
		virtual bool clear();

		virtual uint32_t getSignalTrialCount() const;
		virtual ISignalTrial& getSignalTrial(uint32_t ui32Index) const;

		_IsDerivedFromClass_Final_(OpenViBEToolkit::ISignalTrialSet, OVTK_ClassId_)

	protected:

		mutable std::vector<ISignalTrial*> m_vSignalTrial;
	};

	extern OVTK_API ISignalTrialSet* createSignalTrialSet();
};
