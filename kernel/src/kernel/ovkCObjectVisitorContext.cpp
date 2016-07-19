#include "ovkCObjectVisitorContext.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

CObjectVisitorContext::CObjectVisitorContext(const IKernelContext& rKernelContext)
	:TKernelObject < IObjectVisitorContext >(rKernelContext)
{
}

CObjectVisitorContext::~CObjectVisitorContext(void)
{
}

IAlgorithmManager& CObjectVisitorContext::getAlgorithmManager(void) const
{
	return TKernelObject < IObjectVisitorContext >::getAlgorithmManager();
}

IConfigurationManager& CObjectVisitorContext::getConfigurationManager(void) const
{
	return TKernelObject < IObjectVisitorContext >::getConfigurationManager();
}

ITypeManager& CObjectVisitorContext::getTypeManager(void) const
{
	return TKernelObject < IObjectVisitorContext >::getTypeManager();
}

ILogManager& CObjectVisitorContext::getLogManager(void) const
{
	return TKernelObject < IObjectVisitorContext >::getLogManager();
}

IErrorManager& CObjectVisitorContext::getErrorManager(void) const
{
	return TKernelObject < IObjectVisitorContext >::getErrorManager();
}
