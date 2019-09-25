#pragma once

#include "ovkTKernelObject.h"

#include <iostream>
#include <cstring>

namespace OpenViBE
{
	namespace Kernel
	{
		template <
			class IBase,
			class IType>
		class TBaseParameter : public IBase
		{
		public:

			TBaseParameter(const IKernelContext& ctx, EParameterType eParameterType, const CIdentifier& subTypeID = OV_UndefinedIdentifier)
				: IBase(ctx), m_pValueRef(nullptr), m_Value(0), m_eParameterType(eParameterType), m_oSubTypeIdentifier(subTypeID) { }

			uint64_t getParameterSize() const override { return sizeof(IType); }
			EParameterType getType() const override { return m_eParameterType; }
			CIdentifier getSubTypeIdentifier() const override { return m_oSubTypeIdentifier; }

			bool clearReferenceTarget() override
			{
				m_pValueRef     = NULL;
				m_pParameterRef = nullptr;
				return true;
			}

			bool getReferenceTarget(IParameter*& pParameterRef) const override
			{
				pParameterRef = m_pParameterRef;
				return true;
			}

			bool setReferenceTarget(IParameter* pParameterRef) override
			{
				if (m_pValueRef) { m_pValueRef = NULL; }
				m_pParameterRef = pParameterRef;
				return true;
			}

			bool getReferenceTarget(void* pValue) const override
			{
				memcpy(&pValue, &m_pValueRef, sizeof(IType*));
				return true;
			}

			bool setReferenceTarget(const void* pValue) override
			{
				if (m_pParameterRef) { m_pParameterRef = nullptr; }
				memcpy(&m_pValueRef, &pValue, sizeof(IType*));
				return true;
			}

			bool getValue(void* pValue) const override
			{
				if (m_pParameterRef) { return m_pParameterRef->getValue(pValue); }
				if (m_pValueRef) { memcpy(pValue, m_pValueRef, sizeof(IType)); }
				else { memcpy(pValue, &m_Value, sizeof(IType)); }
				return true;
			}

			bool setValue(const void* pValue) override
			{
				if (m_pParameterRef) { return m_pParameterRef->setValue(pValue); }
				if (m_pValueRef) { memcpy(m_pValueRef, pValue, sizeof(IType)); }
				else { memcpy(&m_Value, pValue, sizeof(IType)); }
				return true;
			}

			_IsDerivedFromClass_(IBase, OVK_ClassId_Kernel_ParameterT)

		protected:

			IParameter* m_pParameterRef = nullptr;
			IType* m_pValueRef          = nullptr;
			IType m_Value;
			EParameterType m_eParameterType;
			CIdentifier m_oSubTypeIdentifier = OV_UndefinedIdentifier;
		};
	} // namespace Kernel
} // namespace OpenViBE
