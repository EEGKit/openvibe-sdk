#include "ovIObserver.h"
#include "ovCObservable.h"


#include "ov_base.h"
#include <iostream>
#include <vector>

using namespace OpenViBE;

struct CObservable::ObserverList
{
	std::vector<IObserver *> m_Vector;
};

CObservable::CObservable() { m_pObserverList = new ObserverList(); }

CObservable::~CObservable() { delete m_pObserverList; }

void CObservable::addObserver(IObserver* o) { m_pObserverList->m_Vector.push_back(o); }

void CObservable::deleteObserver(IObserver* o)
{
	for (std::vector<IObserver *>::iterator it = m_pObserverList->m_Vector.begin(); it != m_pObserverList->m_Vector.end(); ++it)
	{
		if ((*it) == o)
		{
			m_pObserverList->m_Vector.erase(it);
			//We only suppress the first occurence, no need to continue
			return;
		}
	}
}

void CObservable::setChanged() { m_bHasChanged = true; }

void CObservable::clearChanged() { m_bHasChanged = false; }

bool CObservable::hasChanged() { return m_bHasChanged; }

void CObservable::notifyObservers(void* data)
{
	if (m_bHasChanged)
	{
		for (std::vector<IObserver *>::iterator it = m_pObserverList->m_Vector.begin(); it != m_pObserverList->m_Vector.end(); ++it)
		{
			((IObserver *)*it)->update(*this, data);
		}
		m_bHasChanged = false;
	}
}
