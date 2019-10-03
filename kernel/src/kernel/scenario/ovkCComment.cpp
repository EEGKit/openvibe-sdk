#include "ovkCComment.h"
#include "ovkCScenario.h"

#include "../ovkCObjectVisitorContext.h"

using namespace std;
using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

//___________________________________________________________________//
//                                                                   //

CComment::CComment(const IKernelContext& ctx, CScenario& rOwnerScenario)
	: TAttributable<TKernelObject<IComment>>(ctx), m_rOwnerScenario(rOwnerScenario), m_text("") {}

CComment::~CComment() {}

//___________________________________________________________________//
//                                                                   //

CIdentifier CComment::getIdentifier() const { return m_id; }

CString CComment::getText() const { return m_text; }

bool CComment::setIdentifier(const CIdentifier& identifier)
{
	if (m_id != OV_UndefinedIdentifier) { return false; }
	if (identifier == OV_UndefinedIdentifier) { return false; }
	m_id = identifier;

	return true;
}

bool CComment::setText(const CString& sText)
{
	m_text = sText;
	return true;
}

//___________________________________________________________________//
//                                                                   //

bool CComment::initializeFromExistingComment(const IComment& rExisitingComment)
{
	m_text = rExisitingComment.getText();

	CIdentifier l_oID = rExisitingComment.getNextAttributeIdentifier(OV_UndefinedIdentifier);
	while (l_oID != OV_UndefinedIdentifier)
	{
		addAttribute(l_oID, rExisitingComment.getAttributeValue(l_oID));
		l_oID = rExisitingComment.getNextAttributeIdentifier(l_oID);
	}

	return true;
}

//___________________________________________________________________//
//                                                                   //

bool CComment::acceptVisitor(IObjectVisitor& rObjectVisitor)
{
	CObjectVisitorContext l_oObjectVisitorContext(getKernelContext());
	return rObjectVisitor.processBegin(l_oObjectVisitorContext, *this) && rObjectVisitor.processEnd(l_oObjectVisitorContext, *this);
}
