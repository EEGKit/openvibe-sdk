#include "ovkCLink.h"
#include "ovkCScenario.h"

#include "../ovkCObjectVisitorContext.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;

//___________________________________________________________________//
//                                                                   //

CLink::CLink(const IKernelContext& rKernelContext, CScenario& rOwnerScenario)
	: TAttributable<TKernelObject<ILink>>(rKernelContext)
	  , m_rOwnerScenario(rOwnerScenario)
	  , m_oIdentifier(OV_UndefinedIdentifier)
	  , m_oSourceBoxIdentifier(OV_UndefinedIdentifier)
	  , m_oTargetBoxIdentifier(OV_UndefinedIdentifier)
	  , m_ui32SourceOutputIndex(0)
	  , m_ui32TargetInputIndex(0) {}

bool CLink::InitializeFromExistingLink(const ILink& l)
{
	m_oIdentifier                = l.getIdentifier();
	m_oSourceBoxIdentifier       = l.getSourceBoxIdentifier();
	m_oTargetBoxIdentifier       = l.getTargetBoxIdentifier();
	m_oSourceBoxOutputIdentifier = l.getSourceBoxOutputIdentifier();
	m_oTargetBoxInputIdentifier  = l.getTargetBoxInputIdentifier();
	m_ui32SourceOutputIndex      = l.getSourceBoxOutputIndex();
	m_ui32TargetInputIndex       = l.getTargetBoxInputIndex();
	return true;
}


//___________________________________________________________________//
//                                                                   //

bool CLink::setIdentifier(
	const CIdentifier& rIdentifier)
{
	m_oIdentifier = rIdentifier;
	return true;
}

CIdentifier CLink::getIdentifier(void) const
{
	return m_oIdentifier;
}

//___________________________________________________________________//
//                                                                   //

bool CLink::setSource(
	const CIdentifier& rBoxIdentifier,
	const uint32_t ui32BoxOutputIndex,
	const CIdentifier rBoxOutputIdentifier)
{
	m_oSourceBoxIdentifier       = rBoxIdentifier;
	m_ui32SourceOutputIndex      = ui32BoxOutputIndex;
	m_oSourceBoxOutputIdentifier = rBoxOutputIdentifier;
	return true;
}

bool CLink::setTarget(
	const CIdentifier& rBoxIdentifier,
	const uint32_t ui32BoxInputIndex,
	const CIdentifier rBoxInputIdentifier)
{
	m_oTargetBoxIdentifier      = rBoxIdentifier;
	m_ui32TargetInputIndex      = ui32BoxInputIndex;
	m_oTargetBoxInputIdentifier = rBoxInputIdentifier;
	return true;
}

bool CLink::getSource(
	CIdentifier& rBoxIdentifier,
	uint32_t& ui32BoxOutputIndex,
	CIdentifier& rBoxOuputIdentifier) const
{
	rBoxIdentifier      = m_oSourceBoxIdentifier;
	ui32BoxOutputIndex  = m_ui32SourceOutputIndex;
	rBoxOuputIdentifier = m_oSourceBoxOutputIdentifier;
	return true;
}

CIdentifier CLink::getSourceBoxIdentifier(void) const
{
	return m_oSourceBoxIdentifier;
}

uint32_t CLink::getSourceBoxOutputIndex(void) const
{
	return m_ui32SourceOutputIndex;
}

CIdentifier CLink::getSourceBoxOutputIdentifier(void) const
{
	return m_oSourceBoxOutputIdentifier;
}

bool CLink::getTarget(
	CIdentifier& rTargetBoxIdentifier,
	uint32_t& ui32BoxInputIndex,
	CIdentifier& rTargetBoxInputIdentifier) const
{
	rTargetBoxIdentifier      = m_oTargetBoxIdentifier;
	ui32BoxInputIndex         = m_ui32TargetInputIndex;
	rTargetBoxInputIdentifier = m_oTargetBoxInputIdentifier;
	return true;
}

CIdentifier CLink::getTargetBoxIdentifier(void) const
{
	return m_oTargetBoxIdentifier;
}

uint32_t CLink::getTargetBoxInputIndex(void) const
{
	return m_ui32TargetInputIndex;
}

CIdentifier CLink::getTargetBoxInputIdentifier(void) const
{
	return m_oTargetBoxInputIdentifier;
}

//___________________________________________________________________//
//                                                                   //

bool CLink::acceptVisitor(
	IObjectVisitor& rObjectVisitor)
{
	CObjectVisitorContext l_oObjectVisitorContext(getKernelContext());
	return rObjectVisitor.processBegin(l_oObjectVisitorContext, *this) && rObjectVisitor.processEnd(l_oObjectVisitorContext, *this);
}
