#include "ovkCLink.h"
#include "ovkCScenario.h"

#include "../ovkCObjectVisitorContext.h"

using namespace OpenViBE;
using namespace Kernel;

//___________________________________________________________________//
//                                                                   //

CLink::CLink(const IKernelContext& rKernelContext, CScenario& rOwnerScenario)
	: TAttributable<TKernelObject<ILink>>(rKernelContext)
	  , m_rOwnerScenario(rOwnerScenario)
	  , m_oIdentifier(OV_UndefinedIdentifier)
	  , m_oSourceBoxIdentifier(OV_UndefinedIdentifier)
	  , m_oTargetBoxIdentifier(OV_UndefinedIdentifier) {}

bool CLink::InitializeFromExistingLink(const ILink& link)
{
	m_oIdentifier                = link.getIdentifier();
	m_oSourceBoxIdentifier       = link.getSourceBoxIdentifier();
	m_oTargetBoxIdentifier       = link.getTargetBoxIdentifier();
	m_oSourceBoxOutputIdentifier = link.getSourceBoxOutputIdentifier();
	m_oTargetBoxInputIdentifier  = link.getTargetBoxInputIdentifier();
	m_ui32SourceOutputIndex      = link.getSourceBoxOutputIndex();
	m_ui32TargetInputIndex       = link.getTargetBoxInputIndex();
	return true;
}


//___________________________________________________________________//
//                                                                   //

bool CLink::setIdentifier(const CIdentifier& rIdentifier)
{
	m_oIdentifier = rIdentifier;
	return true;
}

CIdentifier CLink::getIdentifier() const { return m_oIdentifier; }

//___________________________________________________________________//
//                                                                   //

bool CLink::setSource(const CIdentifier& boxId, const uint32_t boxOutputIdx, const CIdentifier boxOutputID)
{
	m_oSourceBoxIdentifier       = boxId;
	m_ui32SourceOutputIndex      = boxOutputIdx;
	m_oSourceBoxOutputIdentifier = boxOutputID;
	return true;
}

bool CLink::setTarget(const CIdentifier& boxId, const uint32_t boxInputIdx, const CIdentifier boxInputID)
{
	m_oTargetBoxIdentifier      = boxId;
	m_ui32TargetInputIndex      = boxInputIdx;
	m_oTargetBoxInputIdentifier = boxInputID;
	return true;
}

bool CLink::getSource(CIdentifier& boxId, uint32_t& boxOutputIdx, CIdentifier& boxOutputID) const
{
	boxId      = m_oSourceBoxIdentifier;
	boxOutputIdx  = m_ui32SourceOutputIndex;
	boxOutputID = m_oSourceBoxOutputIdentifier;
	return true;
}

CIdentifier CLink::getSourceBoxIdentifier() const { return m_oSourceBoxIdentifier; }

uint32_t CLink::getSourceBoxOutputIndex() const { return m_ui32SourceOutputIndex; }

CIdentifier CLink::getSourceBoxOutputIdentifier() const { return m_oSourceBoxOutputIdentifier; }

bool CLink::getTarget(CIdentifier& rTargetBoxIdentifier, uint32_t& ui32BoxInputIndex, CIdentifier& rTargetBoxInputIdentifier) const
{
	rTargetBoxIdentifier      = m_oTargetBoxIdentifier;
	ui32BoxInputIndex         = m_ui32TargetInputIndex;
	rTargetBoxInputIdentifier = m_oTargetBoxInputIdentifier;
	return true;
}

CIdentifier CLink::getTargetBoxIdentifier() const { return m_oTargetBoxIdentifier; }

uint32_t CLink::getTargetBoxInputIndex() const { return m_ui32TargetInputIndex; }

CIdentifier CLink::getTargetBoxInputIdentifier() const { return m_oTargetBoxInputIdentifier; }

//___________________________________________________________________//
//                                                                   //

bool CLink::acceptVisitor(IObjectVisitor& rObjectVisitor)
{
	CObjectVisitorContext l_oObjectVisitorContext(getKernelContext());
	return rObjectVisitor.processBegin(l_oObjectVisitorContext, *this) && rObjectVisitor.processEnd(l_oObjectVisitorContext, *this);
}
