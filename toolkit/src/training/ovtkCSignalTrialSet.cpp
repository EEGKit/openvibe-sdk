#include "ovtkCSignalTrialSet.hpp"

using namespace OpenViBE;
using namespace OpenViBEToolkit;
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


ISignalTrialSet* OpenViBEToolkit::createSignalTrialSet() { return new CSignalTrialSet(); }

void OpenViBEToolkit::releaseSignalTrialSet(ISignalTrialSet* pSignalTrialSet) { delete pSignalTrialSet; }
