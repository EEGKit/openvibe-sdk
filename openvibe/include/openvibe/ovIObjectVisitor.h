#ifndef __OpenViBE_IObjectVisitor_H__
#define __OpenViBE_IObjectVisitor_H__

#include "ovIObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		class IObjectVisitorContext;

		class IBox;
		class IComment;
		class IMetadata;
		class ILink;
		class IScenario;
	};

	class OV_API IObjectVisitor : public OpenViBE::IObject
	{
	public:

		virtual bool processBegin(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IBox& rBox) { return true; }
		virtual bool processBegin(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IComment& rComment) { return true; }
		virtual bool processBegin(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IMetadata& rMetadata) { return true; }
		virtual bool processBegin(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::ILink& rLink) { return true; }
		virtual bool processBegin(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IScenario& rScenario) { return true; }

		virtual bool processEnd(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IBox& rBox) { return true; }
		virtual bool processEnd(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IComment& rComment) { return true; }
		virtual bool processEnd(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IMetadata& rMetadata) { return true; }
		virtual bool processEnd(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::ILink& rLink) { return true; }
		virtual bool processEnd(OpenViBE::Kernel::IObjectVisitorContext& rObjectVisitorContext, OpenViBE::Kernel::IScenario& rScenario) { return true; }

		_IsDerivedFromClass_(OpenViBE::IObject, OV_ClassId_ObjectVisitor);
	};
};

#endif // __OpenViBE_IObjectVisitor_H__
