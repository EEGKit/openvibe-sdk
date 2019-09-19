#pragma once

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
	} // namespace kernel

	class OV_API IObjectVisitor : public IObject
	{
	public:

		virtual bool processBegin(Kernel::IObjectVisitorContext& /*rObjectVisitorContext*/, Kernel::IBox& /*box*/) { return true; }
		virtual bool processBegin(Kernel::IObjectVisitorContext& /*rObjectVisitorContext*/, Kernel::IComment& /*rComment*/) { return true; }
		virtual bool processBegin(Kernel::IObjectVisitorContext& /*rObjectVisitorContext*/, Kernel::IMetadata& /*rMetadata*/) { return true; }
		virtual bool processBegin(Kernel::IObjectVisitorContext& /*rObjectVisitorContext*/, Kernel::ILink& /*rLink*/) { return true; }
		virtual bool processBegin(Kernel::IObjectVisitorContext& /*rObjectVisitorContext*/, Kernel::IScenario& /*scenario*/) { return true; }

		virtual bool processEnd(Kernel::IObjectVisitorContext& /*rObjectVisitorContext*/, Kernel::IBox& /*box*/) { return true; }
		virtual bool processEnd(Kernel::IObjectVisitorContext& /*rObjectVisitorContext*/, Kernel::IComment& /*rComment*/) { return true; }
		virtual bool processEnd(Kernel::IObjectVisitorContext& /*rObjectVisitorContext*/, Kernel::IMetadata& /*rMetadata*/) { return true; }
		virtual bool processEnd(Kernel::IObjectVisitorContext& /*rObjectVisitorContext*/, Kernel::ILink& /*rLink*/) { return true; }
		virtual bool processEnd(Kernel::IObjectVisitorContext& /*rObjectVisitorContext*/, Kernel::IScenario& /*scenario*/) { return true; }

		_IsDerivedFromClass_(OpenViBE::IObject, OV_ClassId_ObjectVisitor)
	};
} // namespace OpenViBE
