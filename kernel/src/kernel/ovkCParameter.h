#pragma once

#include "ovkTKernelObject.h"
#include "ovkTParameter.h"

#include <cstring>

namespace OpenViBE
{
	namespace Kernel
	{
#define _parameter_template_instance_simple_type_(_CName_, CType, IType, oClassId) \
			typedef OpenViBE::Kernel::TBaseParameter < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IParameter >, IType > _Base_##_CName_; \
			class _CName_ : public _Base_##_CName_ \
			{ \
			public: \
				_CName_(const OpenViBE::Kernel::IKernelContext& ctx, OpenViBE::Kernel::EParameterType eParameterType, const OpenViBE::CIdentifier& subTypeID = OV_UndefinedIdentifier) \
					:OpenViBE::Kernel::TBaseParameter < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IParameter >, IType >(ctx, eParameterType, subTypeID) \
				{ \
					m_defaultValue = 0; \
					memcpy(&m_value, &m_defaultValue, sizeof(IType)); \
				} \
				_IsDerivedFromClass_Final_(_Base_##_CName_, oClassId) \
			protected: \
				CType m_defaultValue; \
			};

#define _parameter_template_instance_object_(_CName_, CType, IType, oClassId) \
			typedef OpenViBE::Kernel::TBaseParameter < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IParameter >, IType > _Base_##_CName_; \
			class _CName_ : public _Base_##_CName_ \
			{ \
			public: \
				_CName_(const OpenViBE::Kernel::IKernelContext& ctx, OpenViBE::Kernel::EParameterType eParameterType) \
					:OpenViBE::Kernel::TBaseParameter < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IParameter >, IType >(ctx, eParameterType) \
				{ \
					IType defaultValue=&m_defaultValue; \
					memcpy(&m_value, &defaultValue, sizeof(IType)); \
				} \
				_IsDerivedFromClass_Final_(_Base_##_CName_, oClassId) \
			protected: \
				CType m_defaultValue; \
			};

#define _parameter_template_instance_pointer_(_CName_, CType, IType, oClassId) \
			typedef OpenViBE::Kernel::TBaseParameter < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IParameter >, IType > _Base_##_CName_; \
			class _CName_ : public _Base_##_CName_ \
			{ \
			public: \
				_CName_(const OpenViBE::Kernel::IKernelContext& ctx, OpenViBE::Kernel::EParameterType eParameterType) \
					:OpenViBE::Kernel::TBaseParameter < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IParameter >, IType >(ctx, eParameterType) \
				{ \
					m_defaultValue = nullptr; \
					IType defaultValue=&m_defaultValue; \
					memcpy(&m_value, &defaultValue, sizeof(IType)); \
				} \
				_IsDerivedFromClass_Final_(_Base_##_CName_, oClassId) \
			protected: \
				CType m_defaultValue; \
			};

		_parameter_template_instance_simple_type_(CIntegerParameter, int64_t, int64_t, OVK_ClassId_Kernel_IntegerParameter)

		_parameter_template_instance_simple_type_(CUIntegerParameter, uint64_t, uint64_t, OVK_ClassId_Kernel_UIntegerParameter)

		_parameter_template_instance_simple_type_(CEnumerationParameter, uint64_t, uint64_t, OVK_ClassId_Kernel_EnumerationParameter)

		_parameter_template_instance_simple_type_(CBooleanParameter, bool, bool, OVK_ClassId_Kernel_BooleanParameter)

		_parameter_template_instance_simple_type_(CFloatParameter, double, double, OVK_ClassId_Kernel_FloatParameter)

		_parameter_template_instance_object_(CStringParameter, OpenViBE::CString, OpenViBE::CString*, OVK_ClassId_Kernel_StringParameter)

		_parameter_template_instance_object_(CIdentifierParameter, OpenViBE::CIdentifier, OpenViBE::CIdentifier*, OVK_ClassId_Kernel_IdentifierParameter)

		_parameter_template_instance_object_(CMatrixParameter, OpenViBE::CMatrix, OpenViBE::IMatrix*, OVK_ClassId_Kernel_MatrixParameter)

		_parameter_template_instance_object_(CStimulationSetParameter, OpenViBE::CStimulationSet, OpenViBE::IStimulationSet*,
											 OVK_ClassId_Kernel_StimulationSetParameter)

		_parameter_template_instance_object_(CMemoryBufferParameter, OpenViBE::CMemoryBuffer, OpenViBE::IMemoryBuffer*,
											 OVK_ClassId_Kernel_MemoryBufferParameter)

		_parameter_template_instance_object_(CObjectParameter, OpenViBE::CNullObject, OpenViBE::IObject*, OVK_ClassId_Kernel_ObjectParameter)

		_parameter_template_instance_pointer_(CPointerParameter, void*, void*, OVK_ClassId_Kernel_PointerParameter)

#undef _instance_
	} // namespace Kernel
} // namespace OpenViBE
