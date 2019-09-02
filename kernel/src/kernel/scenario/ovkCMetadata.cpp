#include "../ovkTKernelObject.h"

#include "ovkCMetadata.h"
#include "ovkCScenario.h"


#include "../ovkCObjectVisitorContext.h"

using namespace std;
using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

//___________________________________________________________________//
//                                                                   //

CMetadata::CMetadata(const IKernelContext& kernelContext, CScenario& ownerScenario)
	: TKernelObject<IMetadata>(kernelContext)
	  , m_OwnerScenario(ownerScenario)
	  , m_Identifier(OV_UndefinedIdentifier)
	  , m_Type(OV_UndefinedIdentifier)
	  , m_Data("") {}

CMetadata::~CMetadata() {}

CIdentifier CMetadata::getIdentifier() const { return m_Identifier; }

CIdentifier CMetadata::getType() const { return m_Type; }

CString CMetadata::getData() const { return m_Data; }

bool CMetadata::setIdentifier(const CIdentifier& identifier)
{
	OV_ERROR_UNLESS_KRF(m_Identifier == OV_UndefinedIdentifier,
						"Metadata [" << m_Identifier.toString() << "] in scenario [" << m_OwnerScenario.getIdentifier().toString() <<
						"]  already has an identifier.",
						ErrorType::BadCall);

	OV_ERROR_UNLESS_KRF(identifier != OV_UndefinedIdentifier,
						"Attempted to assign undefined identifier to Metadata in scenario [" << m_OwnerScenario.getIdentifier().toString() << "].",
						ErrorType::BadArgument);

	m_Identifier = identifier;
	return true;
}

bool CMetadata::setType(const CIdentifier& typeID)
{
	OV_ERROR_UNLESS_KRF(typeID != OV_UndefinedIdentifier,
						"Attempted to assign undefined typeID to Metadata [" << m_Identifier.toString() << "] in scenario [" << m_OwnerScenario.getIdentifier().
						toString() << "].",
						ErrorType::BadArgument);

	m_Type = typeID;
	return true;
}

bool CMetadata::setData(const CString& data)
{
	m_Data = data;
	return true;
}

//___________________________________________________________________//
//                                                                   //

bool CMetadata::initializeFromExistingMetadata(const IMetadata& existingMetadata)
{
	m_Data = existingMetadata.getData();
	m_Type = existingMetadata.getType();
	return true;
}

//___________________________________________________________________//
//                                                                   //

bool CMetadata::acceptVisitor(IObjectVisitor& objectVisitor)
{
	CObjectVisitorContext objectVisitorContext(this->getKernelContext());
	return objectVisitor.processBegin(objectVisitorContext, *this) && objectVisitor.processEnd(objectVisitorContext, *this);
}
