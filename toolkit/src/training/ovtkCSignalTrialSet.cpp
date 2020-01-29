#include "ovtkCSignalTrialSet.hpp"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Toolkit;
using namespace std;

bool CSignalTrialSet::addSignalTrial(ISignalTrial& rSignalTrial)
{
	m_signalTrials.push_back(&rSignalTrial);
	return true;
}

bool CSignalTrialSet::clear()
{
	m_signalTrials.clear();
	return true;
}


ISignalTrialSet* OpenViBE::Toolkit::createSignalTrialSet() { return new CSignalTrialSet(); }

void OpenViBE::Toolkit::releaseSignalTrialSet(ISignalTrialSet* signalTrialSet) { delete signalTrialSet; }
