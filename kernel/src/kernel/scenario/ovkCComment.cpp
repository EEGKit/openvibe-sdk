#include "ovkCComment.h"
#include "ovkCScenario.h"

#include "../ovkCObjectVisitorContext.h"

using namespace std;
using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;

//___________________________________________________________________//
//                                                                   //

CComment::CComment(const IKernelContext& ctx, CScenario& rOwnerScenario)
	: TAttributable<TKernelObject<IComment>>(ctx), m_rOwnerScenario(rOwnerScenario), m_text("") {}

CComment::~CComment() {}

//___________________________________________________________________//
//                                                                   //

CIdentifier CComment::getIdentifier() const { return m_id; }

CString CComment::getText() const { return m_text; }

bool CComment::setIdentifier(const CIdentifier& id)
{
	if (m_id != OV_UndefinedIdentifier || id == OV_UndefinedIdentifier) { return false; }
	m_id = id;
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

	CIdentifier id = rExisitingComment.getNextAttributeIdentifier(OV_UndefinedIdentifier);
	while (id != OV_UndefinedIdentifier)
	{
		addAttribute(id, rExisitingComment.getAttributeValue(id));
		id = rExisitingComment.getNextAttributeIdentifier(id);
	}

	return true;
}

//___________________________________________________________________//
//                                                                   //

bool CComment::acceptVisitor(IObjectVisitor& rObjectVisitor)
{
	CObjectVisitorContext context(getKernelContext());
	return rObjectVisitor.processBegin(context, *this) && rObjectVisitor.processEnd(context, *this);
}
