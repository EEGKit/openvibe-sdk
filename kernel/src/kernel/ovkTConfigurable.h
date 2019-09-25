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

			~TBaseConfigurable() override
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

			CIdentifier getNextParameterIdentifier(const CIdentifier& previousID) const override
			{
				return getNextIdentifier<std::pair<bool, IParameter*>>(m_vParameter, previousID);
			}

			IParameter* getParameter(const CIdentifier& parameterID) override
			{
				const auto it = m_vParameter.find(parameterID);
				if (it == m_vParameter.end()) { return nullptr; }
				return it->second.second;
			}

			bool setParameter(const CIdentifier& parameterID, IParameter& parameter) override
			{
				this->removeParameter(parameterID);

				m_vParameter[parameterID] = std::pair<bool, IParameter*>(false, &parameter);

				return true;
			}

			IParameter* createParameter(const CIdentifier& parameterID, const EParameterType parameterType, const CIdentifier& subTypeID) override
			{
				const auto it = m_vParameter.find(parameterID);
				if (it != m_vParameter.end()) { return nullptr; }

				IParameter* parameter = nullptr;
				switch (parameterType)
				{
					case ParameterType_UInteger: parameter = new CUIntegerParameter(this->getKernelContext(), parameterType);
						break;
					case ParameterType_Integer: parameter = new CIntegerParameter(this->getKernelContext(), parameterType);
						break;
					case ParameterType_Enumeration: parameter = new CEnumerationParameter(this->getKernelContext(), parameterType, subTypeID);
						break;
					case ParameterType_Boolean: parameter = new CBooleanParameter(this->getKernelContext(), parameterType);
						break;
					case ParameterType_Float: parameter = new CFloatParameter(this->getKernelContext(), parameterType);
						break;
					case ParameterType_String: parameter = new CStringParameter(this->getKernelContext(), parameterType);
						break;
					case ParameterType_Identifier: parameter = new CIdentifierParameter(this->getKernelContext(), parameterType);
						break;
					case ParameterType_Matrix: parameter = new CMatrixParameter(this->getKernelContext(), parameterType);
						break;
					case ParameterType_StimulationSet: parameter = new CStimulationSetParameter(this->getKernelContext(), parameterType);
						break;
					case ParameterType_MemoryBuffer: parameter = new CMemoryBufferParameter(this->getKernelContext(), parameterType);
						break;
					case ParameterType_Object: parameter = new CObjectParameter(this->getKernelContext(), parameterType);
						break;
					case ParameterType_None:
					case ParameterType_Pointer: parameter = new CPointerParameter(this->getKernelContext(), parameterType);
						break;
				}

				if (parameter != nullptr) { m_vParameter[parameterID] = std::pair<bool, IParameter*>(true, parameter); }

				return parameter;
			}

			bool removeParameter(const CIdentifier& rParameterIdentifier) override
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
