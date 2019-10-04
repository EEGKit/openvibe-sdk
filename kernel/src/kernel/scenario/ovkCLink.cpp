#include "ovkCLink.h"
#include "ovkCScenario.h"

#include "../ovkCObjectVisitorContext.h"

using namespace OpenViBE;
using namespace Kernel;

//___________________________________________________________________//
//                                                                   //

CLink::CLink(const IKernelContext& ctx, CScenario& rOwnerScenario)
	: TAttributable<TKernelObject<ILink>>(ctx)
	  , m_rOwnerScenario(rOwnerScenario)
	  , m_id(OV_UndefinedIdentifier)
	  , m_oSourceBoxID(OV_UndefinedIdentifier)
	  , m_oTargetBoxID(OV_UndefinedIdentifier) {}

bool CLink::initializeFromExistingLink(const ILink& link)
{
	m_id                = link.getIdentifier();
	m_oSourceBoxID       = link.getSourceBoxIdentifier();
	m_oTargetBoxID       = link.getTargetBoxIdentifier();
	m_oSourceBoxOutputID = link.getSourceBoxOutputIdentifier();
	m_oTargetBoxInputID  = link.getTargetBoxInputIdentifier();
	m_ui32SourceOutputIdx      = link.getSourceBoxOutputIndex();
	m_ui32TargetInputIdx       = link.getTargetBoxInputIndex();
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

bool CLink::setSource(const CIdentifier& boxId, const uint32_t boxOutputIdx, const CIdentifier boxOutputID)
{
	m_oSourceBoxID       = boxId;
	m_ui32SourceOutputIdx      = boxOutputIdx;
	m_oSourceBoxOutputID = boxOutputID;
	return true;
}

bool CLink::setTarget(const CIdentifier& boxId, const uint32_t boxInputIdx, const CIdentifier boxInputID)
{
	m_oTargetBoxID      = boxId;
	m_ui32TargetInputIdx      = boxInputIdx;
	m_oTargetBoxInputID = boxInputID;
	return true;
}

bool CLink::getSource(CIdentifier& boxId, uint32_t& boxOutputIdx, CIdentifier& boxOutputID) const
{
	boxId        = m_oSourceBoxID;
	boxOutputIdx = m_ui32SourceOutputIdx;
	boxOutputID  = m_oSourceBoxOutputID;
	return true;
}

CIdentifier CLink::getSourceBoxIdentifier() const { return m_oSourceBoxID; }

uint32_t CLink::getSourceBoxOutputIndex() const { return m_ui32SourceOutputIdx; }

CIdentifier CLink::getSourceBoxOutputIdentifier() const { return m_oSourceBoxOutputID; }

bool CLink::getTarget(CIdentifier& rTargetBoxIdentifier, uint32_t& ui32BoxInputIndex, CIdentifier& rTargetBoxInputIdentifier) const
{
	rTargetBoxIdentifier      = m_oTargetBoxID;
	ui32BoxInputIndex         = m_ui32TargetInputIdx;
	rTargetBoxInputIdentifier = m_oTargetBoxInputID;
	return true;
}

CIdentifier CLink::getTargetBoxIdentifier() const { return m_oTargetBoxID; }

uint32_t CLink::getTargetBoxInputIndex() const { return m_ui32TargetInputIdx; }

CIdentifier CLink::getTargetBoxInputIdentifier() const { return m_oTargetBoxInputID; }

//___________________________________________________________________//
//                                                                   //

bool CLink::acceptVisitor(IObjectVisitor& rObjectVisitor)
{
	CObjectVisitorContext l_oObjectVisitorContext(getKernelContext());
	return rObjectVisitor.processBegin(l_oObjectVisitorContext, *this) && rObjectVisitor.processEnd(l_oObjectVisitorContext, *this);
}
