#include "ovkCLink.h"
#include "ovkCScenario.h"

#include "../ovkCObjectVisitorContext.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;

//___________________________________________________________________//
//                                                                   //

CLink::CLink(const IKernelContext& ctx, CScenario& ownerScenario)
	: TAttributable<TKernelObject<ILink>>(ctx), m_ownerScenario(ownerScenario), m_id(OV_UndefinedIdentifier), m_srcBoxID(OV_UndefinedIdentifier),
	  m_dstBoxID(OV_UndefinedIdentifier) {}

bool CLink::initializeFromExistingLink(const ILink& link)
{
	m_id             = link.getIdentifier();
	m_srcBoxID       = link.getSourceBoxIdentifier();
	m_dstBoxID       = link.getTargetBoxIdentifier();
	m_srcBoxOutputID = link.getSourceBoxOutputIdentifier();
	m_dstBoxInputID  = link.getTargetBoxInputIdentifier();
	m_srcOutputIdx   = link.getSourceBoxOutputIndex();
	m_dstInputIdx    = link.getTargetBoxInputIndex();
	return true;
}


//___________________________________________________________________//
//                                                                   //

bool CLink::setIdentifier(const CIdentifier& identifier)
{
	m_id = identifier;
	return true;
}

CIdentifier CLink::getIdentifier() const { return m_id; }

//___________________________________________________________________//
//                                                                   //

bool CLink::setSource(const CIdentifier& boxId, const size_t boxOutputIdx, const CIdentifier boxOutputID)
{
	m_srcBoxID       = boxId;
	m_srcOutputIdx   = boxOutputIdx;
	m_srcBoxOutputID = boxOutputID;
	return true;
}

bool CLink::setTarget(const CIdentifier& boxId, const size_t boxInputIdx, const CIdentifier boxInputID)
{
	m_dstBoxID      = boxId;
	m_dstInputIdx   = boxInputIdx;
	m_dstBoxInputID = boxInputID;
	return true;
}

bool CLink::getSource(CIdentifier& boxId, size_t& boxOutputIdx, CIdentifier& boxOutputID) const
{
	boxId        = m_srcBoxID;
	boxOutputIdx = m_srcOutputIdx;
	boxOutputID  = m_srcBoxOutputID;
	return true;
}

CIdentifier CLink::getSourceBoxIdentifier() const { return m_srcBoxID; }
size_t CLink::getSourceBoxOutputIndex() const { return m_srcOutputIdx; }
CIdentifier CLink::getSourceBoxOutputIdentifier() const { return m_srcBoxOutputID; }

bool CLink::getTarget(CIdentifier& dstBoxID, size_t& boxInputIndex, CIdentifier& dstBoxInputID) const
{
	dstBoxID      = m_dstBoxID;
	boxInputIndex = m_dstInputIdx;
	dstBoxInputID = m_dstBoxInputID;
	return true;
}

CIdentifier CLink::getTargetBoxIdentifier() const { return m_dstBoxID; }
size_t CLink::getTargetBoxInputIndex() const { return m_dstInputIdx; }
CIdentifier CLink::getTargetBoxInputIdentifier() const { return m_dstBoxInputID; }

//___________________________________________________________________//
//                                                                   //

bool CLink::acceptVisitor(IObjectVisitor& visitor)
{
	CObjectVisitorContext context(getKernelContext());
	return visitor.processBegin(context, *this) && visitor.processEnd(context, *this);
}
