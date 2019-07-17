#include "ovtkCSignalTrialSet.hpp"

using namespace OpenViBE;
using namespace OpenViBEToolkit;
using namespace std;

bool CSignalTrialSet::addSignalTrial(ISignalTrial& rSignalTrial)
{
	m_vSignalTrial.push_back(&rSignalTrial);
	return true;
}

bool CSignalTrialSet::clear(void)
{
	m_vSignalTrial.clear();
	return true;
}

uint32 CSignalTrialSet::getSignalTrialCount(void) const
{
	return static_cast<uint32>(m_vSignalTrial.size());
}

ISignalTrial& CSignalTrialSet::getSignalTrial(uint32 ui32Index) const
{
	return *m_vSignalTrial[ui32Index];
}

ISignalTrialSet* OpenViBEToolkit::createSignalTrialSet(void)
{
	return new CSignalTrialSet();
}

void OpenViBEToolkit::releaseSignalTrialSet(ISignalTrialSet* pSignalTrialSet)
{
	delete pSignalTrialSet;
}
