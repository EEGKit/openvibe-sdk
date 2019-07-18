#ifndef __OpenViBEToolkit_Offline_CSignalTrialSet_HPP__
#define __OpenViBEToolkit_Offline_CSignalTrialSet_HPP__

#include "ovtkISignalTrialSet.h"

#include <vector>

namespace OpenViBEToolkit
{
	class CSignalTrialSet : public OpenViBEToolkit::ISignalTrialSet
	{
	public:

		virtual bool addSignalTrial(OpenViBEToolkit::ISignalTrial& rSignalTrial);
		virtual bool clear(void);

		virtual uint32_t getSignalTrialCount(void) const;
		virtual OpenViBEToolkit::ISignalTrial& getSignalTrial(uint32_t ui32Index) const;

		_IsDerivedFromClass_Final_(OpenViBEToolkit::ISignalTrialSet, OVTK_ClassId_);

	protected:

		mutable std::vector<OpenViBEToolkit::ISignalTrial*> m_vSignalTrial;
	};

	extern OVTK_API OpenViBEToolkit::ISignalTrialSet* createSignalTrialSet(void);
};

#endif // __OpenViBEToolkit_Offline_CSignalTrialSet_HPP__
