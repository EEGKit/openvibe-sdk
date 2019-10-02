#pragma once

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

#include "ovtkTEncoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TExperimentInformationEncoderLocal : public T
	{
	protected:

		OpenViBE::Kernel::TParameterHandler<uint64_t> m_inputExperimentIdentifier;
		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> m_pInputpExperimentDate;

		OpenViBE::Kernel::TParameterHandler<uint64_t> m_inputSubjectIdentifier;
		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> m_pInputpSubjectName;
		OpenViBE::Kernel::TParameterHandler<uint64_t> m_inputSubjectAge;
		OpenViBE::Kernel::TParameterHandler<uint64_t> m_inputSubjectGender;

		OpenViBE::Kernel::TParameterHandler<uint64_t> m_inputLaboratoryIdentifier;
		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> m_pInputpLaboratoryName;
		OpenViBE::Kernel::TParameterHandler<uint64_t> m_inputTechnicianIdentifier;
		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> m_pInputTechnicianName;

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pOutputMemoryBuffer;

		bool initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(
				m_pBoxAlgorithm->getAlgorithmManager().
								 createAlgorithm(OVP_GD_ClassId_Algorithm_ExperimentInformationStreamEncoder));
			m_pCodec->initialize();
			m_inputExperimentIdentifier.initialize(
				m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentIdentifier));
			m_pInputpExperimentDate.
					initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentDate));

			m_inputSubjectIdentifier.initialize(
				m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectIdentifier));
			m_pInputpSubjectName.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectName));
			m_inputSubjectAge.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectAge));
			m_inputSubjectGender.
					initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectGender));

			m_inputLaboratoryIdentifier.initialize(
				m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryIdentifier));
			m_pInputpLaboratoryName.
					initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryName));
			m_inputTechnicianIdentifier.initialize(
				m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianIdentifier));
			m_pInputTechnicianName.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianName));

			m_pOutputMemoryBuffer.initialize(
				m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

			return true;
		}

	public:
		using T::initialize;

		bool uninitialize()
		{
			if (m_pBoxAlgorithm == nullptr || m_pCodec == nullptr) { return false; }

			m_inputExperimentIdentifier.uninitialize();
			m_pInputpExperimentDate.uninitialize();
			m_inputSubjectIdentifier.uninitialize();
			m_pInputpSubjectName.uninitialize();
			m_inputSubjectAge.uninitialize();
			m_inputSubjectGender.uninitialize();
			m_inputLaboratoryIdentifier.uninitialize();
			m_pInputpLaboratoryName.uninitialize();
			m_inputTechnicianIdentifier.uninitialize();
			m_pInputTechnicianName.uninitialize();

			m_pOutputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getInputExperimentIdentifier() { return m_inputExperimentIdentifier; }

		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*>& getInputExperimentDate() { return m_pInputpExperimentDate; }

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getInputSubjectIdentifier() { return m_inputSubjectIdentifier; }

		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*>& getInputSubjectName() { return m_pInputpSubjectName; }

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getInputSubjectAge() { return m_inputSubjectAge; }

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getInputSubjectGender() { return m_inputSubjectGender; }

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getInputLaboratoryIdentifier() { return m_inputLaboratoryIdentifier; }

		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*>& getInputLaboratoryName() { return m_pInputpLaboratoryName; }

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getInputTechnicianIdentifier() { return m_inputTechnicianIdentifier; }

		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*>& getInputTechnicianName() { return m_pInputTechnicianName; }

	protected:
		bool encodeHeaderImpl() { return m_pCodec->process(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputTriggerId_EncodeHeader); }

		bool encodeBufferImpl() { return m_pCodec->process(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputTriggerId_EncodeBuffer); }

		bool encodeEndImpl() { return m_pCodec->process(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputTriggerId_EncodeEnd); }
	};

	template <class T>
	class TExperimentInformationEncoder : public TExperimentInformationEncoderLocal<TEncoder<T>>
	{
		using TExperimentInformationEncoderLocal<TEncoder<T>>::m_pBoxAlgorithm;

	public:
		using TExperimentInformationEncoderLocal<TEncoder<T>>::uninitialize;

		TExperimentInformationEncoder() { }

		TExperimentInformationEncoder(T& rBoxAlgorithm, uint32_t ui32ConnectorIndex)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm, ui32ConnectorIndex);
		}

		virtual ~TExperimentInformationEncoder() { this->uninitialize(); }
	};
} // namespace OpenViBEToolkit

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
