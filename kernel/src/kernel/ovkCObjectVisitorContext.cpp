#include "ovkCObjectVisitorContext.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

CObjectVisitorContext::CObjectVisitorContext(const IKernelContext& rKernelContext)
	: TKernelObject<IObjectVisitorContext>(rKernelContext) {}

CObjectVisitorContext::~CObjectVisitorContext() {}

IAlgorithmManager& CObjectVisitorContext::getAlgorithmManager() const
{
	return TKernelObject<IObjectVisitorContext>::getAlgorithmManager();
}

IConfigurationManager& CObjectVisitorContext::getConfigurationManager() const
{
	return TKernelObject<IObjectVisitorContext>::getConfigurationManager();
}

ITypeManager& CObjectVisitorContext::getTypeManager() const
{
	return TKernelObject<IObjectVisitorContext>::getTypeManager();
}

ILogManager& CObjectVisitorContext::getLogManager() const
{
	return TKernelObject<IObjectVisitorContext>::getLogManager();
}

IErrorManager& CObjectVisitorContext::getErrorManager() const
{
	return TKernelObject<IObjectVisitorContext>::getErrorManager();
}
