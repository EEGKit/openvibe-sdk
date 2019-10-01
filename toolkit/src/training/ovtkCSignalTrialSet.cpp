#include "ovtkCSignalTrialSet.hpp"

using namespace OpenViBE;
using namespace OpenViBEToolkit;
using namespace std;

bool CSignalTrialSet::addSignalTrial(ISignalTrial& rSignalTrial)
{
	m_vSignalTrial.push_back(&rSignalTrial);
	return true;
}

bool CSignalTrialSet::clear()
{
	m_vSignalTrial.clear();
	return true;
}


ISignalTrialSet* OpenViBEToolkit::createSignalTrialSet() { return new CSignalTrialSet(); }

void OpenViBEToolkit::releaseSignalTrialSet(ISignalTrialSet* pSignalTrialSet) { delete pSignalTrialSet; }
