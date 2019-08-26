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

uint32_t CSignalTrialSet::getSignalTrialCount() const { return uint32_t(m_vSignalTrial.size()); }

ISignalTrial& CSignalTrialSet::getSignalTrial(uint32_t index) const { return *m_vSignalTrial[index]; }

ISignalTrialSet* OpenViBEToolkit::createSignalTrialSet() { return new CSignalTrialSet(); }

void OpenViBEToolkit::releaseSignalTrialSet(ISignalTrialSet* pSignalTrialSet) { delete pSignalTrialSet; }
