#include "ovCStimulationSet.h"

#include <vector>

using namespace OpenViBE;
using namespace std;

namespace OpenViBE
{
	namespace
	{
		class CStimulation
		{
		public:

			CStimulation() : m_ui64Identifier(0), m_ui64Date(0), m_ui64Duration(0) { }

			CStimulation(const uint64_t ui64Identifier, const uint64_t ui64Date, const uint64_t ui64Duration)
				: m_ui64Identifier(ui64Identifier), m_ui64Date(ui64Date), m_ui64Duration(ui64Duration) { }

			uint64_t m_ui64Identifier;
			uint64_t m_ui64Date;
			uint64_t m_ui64Duration;
		};

		class CStimulationSetImpl : public IStimulationSet
		{
		public:

			virtual void clear();

			virtual const uint64_t getStimulationCount() const;
			virtual const uint64_t getStimulationIdentifier(uint64_t ui64StimulationIndex) const;
			virtual const uint64_t getStimulationDate(uint64_t ui64StimulationIndex) const;
			virtual const uint64_t getStimulationDuration(uint64_t ui64StimulationIndex) const;

			virtual bool setStimulationCount(uint64_t ui64StimulationCount);
			virtual bool setStimulationIdentifier(uint64_t ui64StimulationIndex, uint64_t ui64StimulationIdentifier);
			virtual bool setStimulationDate(uint64_t ui64StimulationIndex, uint64_t ui64StimulationDate);
			virtual bool setStimulationDuration(uint64_t ui64StimulationIndex, uint64_t ui64StimulationDuration);

			virtual uint64_t appendStimulation(uint64_t ui64StimulationIdentifier, uint64_t ui64StimulationDate, uint64_t ui64StimulationDuration);
			virtual uint64_t insertStimulation(uint64_t ui64StimulationIndex, uint64_t ui64StimulationIdentifier, uint64_t ui64StimulationDate, uint64_t ui64StimulationDuration);
			virtual bool removeStimulation(uint64_t ui64StimulationIndex);

			_IsDerivedFromClass_Final_(IStimulationSet, OV_ClassId_StimulationSetImpl)

		private:

			vector<CStimulation> m_vStimulation;
		};
	}  // namespace
}  // namespace OpenViBE

// ________________________________________________________________________________________________________________
//

void CStimulationSetImpl::clear() { m_vStimulation.clear(); }

const uint64_t CStimulationSetImpl::getStimulationCount() const { return m_vStimulation.size(); }

const uint64_t CStimulationSetImpl::getStimulationIdentifier(const uint64_t ui64StimulationIndex) const
{
	return m_vStimulation[static_cast<size_t>(ui64StimulationIndex)].m_ui64Identifier;
}

const uint64_t CStimulationSetImpl::getStimulationDate(const uint64_t ui64StimulationIndex) const
{
	return m_vStimulation[static_cast<size_t>(ui64StimulationIndex)].m_ui64Date;
}

const uint64_t CStimulationSetImpl::getStimulationDuration(const uint64_t ui64StimulationIndex) const
{
	return m_vStimulation[static_cast<size_t>(ui64StimulationIndex)].m_ui64Duration;
}

bool CStimulationSetImpl::setStimulationCount(const uint64_t ui64StimulationCount)
{
	m_vStimulation.resize(static_cast<size_t>(ui64StimulationCount));
	return true;
}

bool CStimulationSetImpl::setStimulationIdentifier(const uint64_t ui64StimulationIndex, const uint64_t ui64StimulationIdentifier)
{
	m_vStimulation[static_cast<size_t>(ui64StimulationIndex)].m_ui64Identifier = ui64StimulationIdentifier;
	return true;
}

bool CStimulationSetImpl::setStimulationDate(const uint64_t ui64StimulationIndex, const uint64_t ui64StimulationDate)
{
	m_vStimulation[static_cast<size_t>(ui64StimulationIndex)].m_ui64Date = ui64StimulationDate;
	return true;
}

bool CStimulationSetImpl::setStimulationDuration(const uint64_t ui64StimulationIndex, const uint64_t ui64StimulationDuration)
{
	m_vStimulation[static_cast<size_t>(ui64StimulationIndex)].m_ui64Duration = ui64StimulationDuration;
	return true;
}

uint64_t CStimulationSetImpl::appendStimulation(const uint64_t ui64StimulationIdentifier, const uint64_t ui64StimulationDate, const uint64_t ui64StimulationDuration)
{
	m_vStimulation.push_back(CStimulation(ui64StimulationIdentifier, ui64StimulationDate, ui64StimulationDuration));
	return m_vStimulation.size() - 1;
}

uint64_t CStimulationSetImpl::insertStimulation(const uint64_t ui64StimulationIndex, const uint64_t ui64StimulationIdentifier, const uint64_t ui64StimulationDate, const uint64_t ui64StimulationDuration)
{
	if (ui64StimulationIndex > m_vStimulation.size()) { return false; }
	if (ui64StimulationIndex == m_vStimulation.size())
	{
		m_vStimulation.push_back(CStimulation(ui64StimulationIdentifier, ui64StimulationDate, ui64StimulationDuration));
	}
	else
	{
		m_vStimulation.insert(m_vStimulation.begin() + static_cast<size_t>(ui64StimulationIndex), CStimulation(ui64StimulationIdentifier, ui64StimulationDate, ui64StimulationDuration));
	}
	return true;
}

bool CStimulationSetImpl::removeStimulation(const uint64_t ui64StimulationIndex)
{
	if (ui64StimulationIndex >= m_vStimulation.size()) { return false; }
	m_vStimulation.erase(m_vStimulation.begin() + static_cast<size_t>(ui64StimulationIndex));
	return true;
}

// ________________________________________________________________________________________________________________
//

CStimulationSet::CStimulationSet() : m_pStimulationSetImpl(NULL) { m_pStimulationSetImpl = new CStimulationSetImpl(); }

CStimulationSet::~CStimulationSet() { delete m_pStimulationSetImpl; }

void CStimulationSet::clear() { m_pStimulationSetImpl->clear(); }

const uint64_t CStimulationSet::getStimulationCount() const
{
	return m_pStimulationSetImpl->getStimulationCount();
}

const uint64_t CStimulationSet::getStimulationIdentifier(const uint64_t ui64StimulationIndex) const
{
	return m_pStimulationSetImpl->getStimulationIdentifier(ui64StimulationIndex);
}

const uint64_t CStimulationSet::getStimulationDate(const uint64_t ui64StimulationIndex) const
{
	return m_pStimulationSetImpl->getStimulationDate(ui64StimulationIndex);
}

const uint64_t CStimulationSet::getStimulationDuration(const uint64_t ui64StimulationIndex) const
{
	return m_pStimulationSetImpl->getStimulationDuration(ui64StimulationIndex);
}

bool CStimulationSet::setStimulationCount(const uint64_t ui64StimulationCount)
{
	return m_pStimulationSetImpl->setStimulationCount(ui64StimulationCount);
}

bool CStimulationSet::setStimulationIdentifier(const uint64_t ui64StimulationIndex, const uint64_t ui64StimulationIdentifier)
{
	return m_pStimulationSetImpl->setStimulationIdentifier(ui64StimulationIndex, ui64StimulationIdentifier);
}

bool CStimulationSet::setStimulationDate(const uint64_t ui64StimulationIndex, const uint64_t ui64StimulationDate)
{
	return m_pStimulationSetImpl->setStimulationDate(ui64StimulationIndex, ui64StimulationDate);
}

bool CStimulationSet::setStimulationDuration(const uint64_t ui64StimulationIndex, const uint64_t ui64StimulationDuration)
{
	return m_pStimulationSetImpl->setStimulationDuration(ui64StimulationIndex, ui64StimulationDuration);
}

uint64_t CStimulationSet::appendStimulation(const uint64_t ui64StimulationIdentifier, const uint64_t ui64StimulationDate, const uint64_t ui64StimulationDuration)
{
	return m_pStimulationSetImpl->appendStimulation(ui64StimulationIdentifier, ui64StimulationDate, ui64StimulationDuration);
}

uint64_t CStimulationSet::insertStimulation(const uint64_t ui64StimulationIndex, const uint64_t ui64StimulationIdentifier, const uint64_t ui64StimulationDate, const uint64_t ui64StimulationDuration)
{
	return m_pStimulationSetImpl->insertStimulation(ui64StimulationIndex, ui64StimulationIdentifier, ui64StimulationDate, ui64StimulationDuration);
}

bool CStimulationSet::removeStimulation(const uint64_t ui64StimulationIndex)
{
	return m_pStimulationSetImpl->removeStimulation(ui64StimulationIndex);
}
