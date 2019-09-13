#pragma once

#include "ovkTKernelObject.h"

#include "ovkCParameter.h"

#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class IParameter;

		template <class IBase>
		class TBaseConfigurable : public IBase
		{
		public:

			explicit TBaseConfigurable(const IKernelContext& ctx) : IBase(ctx) { }

			virtual ~TBaseConfigurable()
			{
				auto itParameter = m_vParameter.begin();
				while (itParameter != m_vParameter.end())
				{
					// @FIXME is this really as intended, test the first, delete the second?
					if (itParameter->second.first)
					{
						delete itParameter->second.second;
						itParameter->second.second = nullptr;
					}
					++itParameter;
				}
			}

			virtual CIdentifier getNextParameterIdentifier(const CIdentifier& previousID) const
			{
				return getNextIdentifier<std::pair<bool, IParameter*>>(m_vParameter, previousID);
			}

			virtual IParameter* getParameter(const CIdentifier& rParameterIdentifier)
			{
				auto itParameter = m_vParameter.find(rParameterIdentifier);
				if (itParameter == m_vParameter.end()) { return nullptr; }
				return itParameter->second.second;
			}

			virtual bool setParameter(const CIdentifier& rParameterIdentifier, IParameter& rpParameter)
			{
				this->removeParameter(rParameterIdentifier);

				m_vParameter[rParameterIdentifier] = std::pair<bool, IParameter*>(false, &rpParameter);

				return true;
			}

			virtual IParameter* createParameter(const CIdentifier& rParameterIdentifier, const EParameterType eParameterType,
												const CIdentifier& subTypeID)
			{
				auto itParameter = m_vParameter.find(rParameterIdentifier);
				if (itParameter != m_vParameter.end()) { return nullptr; }

				IParameter* l_pParameter = nullptr;
				switch (eParameterType)
				{
					case ParameterType_UInteger: l_pParameter = new CUIntegerParameter(this->getKernelContext(), eParameterType);
						break;
					case ParameterType_Integer: l_pParameter = new CIntegerParameter(this->getKernelContext(), eParameterType);
						break;
					case ParameterType_Enumeration: l_pParameter = new CEnumerationParameter(this->getKernelContext(), eParameterType, subTypeID);
						break;
					case ParameterType_Boolean: l_pParameter = new CBooleanParameter(this->getKernelContext(), eParameterType);
						break;
					case ParameterType_Float: l_pParameter = new CFloatParameter(this->getKernelContext(), eParameterType);
						break;
					case ParameterType_String: l_pParameter = new CStringParameter(this->getKernelContext(), eParameterType);
						break;
					case ParameterType_Identifier: l_pParameter = new CIdentifierParameter(this->getKernelContext(), eParameterType);
						break;
					case ParameterType_Matrix: l_pParameter = new CMatrixParameter(this->getKernelContext(), eParameterType);
						break;
					case ParameterType_StimulationSet: l_pParameter = new CStimulationSetParameter(this->getKernelContext(), eParameterType);
						break;
					case ParameterType_MemoryBuffer: l_pParameter = new CMemoryBufferParameter(this->getKernelContext(), eParameterType);
						break;
					case ParameterType_Object: l_pParameter = new CObjectParameter(this->getKernelContext(), eParameterType);
						break;
					case ParameterType_None:
					case ParameterType_Pointer: l_pParameter = new CPointerParameter(this->getKernelContext(), eParameterType);
						break;
				}

				if (l_pParameter != nullptr) { m_vParameter[rParameterIdentifier] = std::pair<bool, IParameter*>(true, l_pParameter); }

				return l_pParameter;
			}

			virtual bool removeParameter(const CIdentifier& rParameterIdentifier)
			{
				auto itParameter = m_vParameter.find(rParameterIdentifier);
				if (itParameter == m_vParameter.end()) { return false; }

				if (itParameter->second.first) { delete itParameter->second.second; }
				m_vParameter.erase(itParameter);

				return true;
			}

			_IsDerivedFromClass_Final_(IBase, OVK_ClassId_Kernel_ConfigurableT)

		private:

			std::map<CIdentifier, std::pair<bool, IParameter*>> m_vParameter;
		};
	} // namespace Kernel
} // namespace OpenViBE
