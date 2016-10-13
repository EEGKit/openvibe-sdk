#include "../ovkTKernelObject.h"

#include "ovkCMetadata.h"
#include "ovkCScenario.h"


#include "../ovkCObjectVisitorContext.h"

using namespace std;
using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

//___________________________________________________________________//
//                                                                   //

CMetadata::CMetadata(const IKernelContext& kernelContext, CScenario& ownerScenario)
	: TKernelObject<IMetadata>(kernelContext)
	, m_ownerScenario(ownerScenario)
	, m_identifier(OV_UndefinedIdentifier)
    , m_type(OV_UndefinedIdentifier)
	, m_data("")
{
}

CMetadata::~CMetadata(void)
{
}

CIdentifier CMetadata::getIdentifier(void) const
{
	return m_identifier;
}

CIdentifier CMetadata::getType(void) const
{
	return m_type;
}

CString CMetadata::getData(void) const
{
	return m_data;
}

bool CMetadata::setIdentifier(const CIdentifier& identifier)
{
	OV_ERROR_UNLESS_KRF(
		m_identifier == OV_UndefinedIdentifier,
		"Metadata [" << m_identifier.toString() << "] in scenario [" << m_ownerScenario.getIdentifier().toString() << "]  already has an identifier.",
		ErrorType::BadCall
	);

	OV_ERROR_UNLESS_KRF(
		identifier != OV_UndefinedIdentifier,
		"Attempted to assign undefined identifier to Metadata in scenario [" << m_ownerScenario.getIdentifier().toString() << "].",
		ErrorType::BadArgument
	);

	m_identifier = identifier;
	return true;
}

bool CMetadata::setType(const CIdentifier& type)
{
	OV_ERROR_UNLESS_KRF(
		type != OV_UndefinedIdentifier,
		"Attempted to assign undefined type to Metadata [" << m_identifier.toString() << "] in scenario [" << m_ownerScenario.getIdentifier().toString() << "].",
		ErrorType::BadArgument
	);

	m_type = type;
	return true;
}

bool CMetadata::setData(const CString& data)
{
	m_data = data;
	return true;
}

//___________________________________________________________________//
//                                                                   //

bool CMetadata::initializeFromExistingMetadata(const IMetadata& existingMetadata)
{
	m_data = existingMetadata.getData();
	m_type = existingMetadata.getType();
	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CMetadata::acceptVisitor(IObjectVisitor& objectVisitor)
{
	CObjectVisitorContext objectVisitorContext(this->getKernelContext());
	return objectVisitor.processBegin(objectVisitorContext, *this) && objectVisitor.processEnd(objectVisitorContext, *this);
}
