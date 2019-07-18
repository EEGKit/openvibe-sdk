#ifndef __OpenViBEKernel_Kernel_TParameter_H__
#define __OpenViBEKernel_Kernel_TParameter_H__

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

			TBaseParameter(const OpenViBE::Kernel::IKernelContext& rKernelContext, OpenViBE::Kernel::EParameterType eParameterType, const OpenViBE::CIdentifier& rSubTypeIdentifier = OV_UndefinedIdentifier)
				: IBase(rKernelContext)
				  , m_pParameterRef(0)
				  , m_pValueRef(0)
				  , m_Value(0)
				  , m_eParameterType(eParameterType)
				  , m_oSubTypeIdentifier(rSubTypeIdentifier) { }

			virtual uint64_t getParameterSize(void) const
			{
				return sizeof(IType);
			}

			virtual OpenViBE::Kernel::EParameterType getType(void) const
			{
				return m_eParameterType;
			}

			virtual OpenViBE::CIdentifier getSubTypeIdentifier(void) const
			{
				return m_oSubTypeIdentifier;
			}

			virtual bool clearReferenceTarget(void)
			{
				m_pValueRef     = NULL;
				m_pParameterRef = NULL;
				return true;
			}

			virtual bool getReferenceTarget(OpenViBE::Kernel::IParameter*& pParameterRef) const
			{
				pParameterRef = m_pParameterRef;
				return true;
			}

			virtual bool setReferenceTarget(OpenViBE::Kernel::IParameter* pParameterRef)
			{
				if (m_pValueRef)
				{
					m_pValueRef = NULL;
				}
				m_pParameterRef = pParameterRef;
				return true;
			}

			virtual bool getReferenceTarget(void* pValue) const
			{
				memcpy(&pValue, &m_pValueRef, sizeof(IType*));
				return true;
			}

			virtual bool setReferenceTarget(const void* pValue)
			{
				if (m_pParameterRef)
				{
					m_pParameterRef = NULL;
				}
				memcpy(&m_pValueRef, &pValue, sizeof(IType*));
				return true;
			}

			virtual bool getValue(void* pValue) const
			{
				if (m_pParameterRef)
				{
					return m_pParameterRef->getValue(pValue);
				}
				if (m_pValueRef)
				{
					memcpy(pValue, m_pValueRef, sizeof(IType));
				}
				else
				{
					memcpy(pValue, &m_Value, sizeof(IType));
				}
				return true;
			}

			virtual bool setValue(const void* pValue)
			{
				if (m_pParameterRef)
				{
					return m_pParameterRef->setValue(pValue);
				}
				if (m_pValueRef)
				{
					memcpy(m_pValueRef, pValue, sizeof(IType));
				}
				else
				{
					memcpy(&m_Value, pValue, sizeof(IType));
				}
				return true;
			}

			_IsDerivedFromClass_(IBase, OVK_ClassId_Kernel_ParameterT);

		protected:

			OpenViBE::Kernel::IParameter* m_pParameterRef;
			IType* m_pValueRef;
			IType m_Value;
			OpenViBE::Kernel::EParameterType m_eParameterType;
			OpenViBE::CIdentifier m_oSubTypeIdentifier;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_TParameter_H__
