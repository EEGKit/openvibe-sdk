#pragma once

#include "ovtkISignalTrialSet.h"

#include <vector>

namespace OpenViBEToolkit
{
	class CSignalTrialSet : public ISignalTrialSet
	{
	public:
		bool addSignalTrial(ISignalTrial& rSignalTrial) override;
		bool clear() override;
		uint32_t getSignalTrialCount() const override;
		ISignalTrial& getSignalTrial(uint32_t index) const override;

		_IsDerivedFromClass_Final_(OpenViBEToolkit::ISignalTrialSet, OVTK_ClassId_)

	protected:

		mutable std::vector<ISignalTrial*> m_vSignalTrial;
	};

	extern OVTK_API ISignalTrialSet* createSignalTrialSet();
} // namespace OpenViBEToolkit
