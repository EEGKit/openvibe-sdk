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

		OpenViBE::Kernel::TParameterHandler<uint64_t> m_experimentID;
		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> m_experimentDate;

		OpenViBE::Kernel::TParameterHandler<uint64_t> m_subjectID;
		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> m_subjectName;
		OpenViBE::Kernel::TParameterHandler<uint64_t> m_subjectAge;
		OpenViBE::Kernel::TParameterHandler<uint64_t> m_subjectGender;

		OpenViBE::Kernel::TParameterHandler<uint64_t> m_laboratoryID;
		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> m_laboratoryName;
		OpenViBE::Kernel::TParameterHandler<uint64_t> m_technicianID;
		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> m_technicianName;

		using T::m_codec;
		using T::m_boxAlgorithm;
		using T::m_oBuffer;

		bool initializeImpl()
		{
			m_codec = &m_boxAlgorithm->getAlgorithmManager().getAlgorithm(m_boxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_ExperimentInformationStreamEncoder));
			m_codec->initialize();
			m_experimentID.initialize(m_codec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentIdentifier));
			m_experimentDate.initialize(m_codec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_ExperimentDate));

			m_subjectID.initialize(m_codec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectIdentifier));
			m_subjectName.initialize(m_codec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectName));
			m_subjectAge.initialize(m_codec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectAge));
			m_subjectGender.initialize(m_codec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_SubjectGender));

			m_laboratoryID.initialize(m_codec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryIdentifier));
			m_laboratoryName.initialize(m_codec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_LaboratoryName));
			m_technicianID.initialize(m_codec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianIdentifier));
			m_technicianName.initialize(m_codec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputParameterId_TechnicianName));

			m_oBuffer.initialize(m_codec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_OutputParameterId_EncodedMemoryBuffer));

			return true;
		}

	public:
		using T::initialize;

		bool uninitialize()
		{
			if (m_boxAlgorithm == nullptr || m_codec == nullptr) { return false; }

			m_experimentID.uninitialize();
			m_experimentDate.uninitialize();
			m_subjectID.uninitialize();
			m_subjectName.uninitialize();
			m_subjectAge.uninitialize();
			m_subjectGender.uninitialize();
			m_laboratoryID.uninitialize();
			m_laboratoryName.uninitialize();
			m_technicianID.uninitialize();
			m_technicianName.uninitialize();

			m_oBuffer.uninitialize();
			m_codec->uninitialize();
			m_boxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_codec);
			m_boxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getInputExperimentIdentifier() { return m_experimentID; }
		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*>& getInputExperimentDate() { return m_experimentDate; }
		OpenViBE::Kernel::TParameterHandler<uint64_t>& getInputSubjectIdentifier() { return m_subjectID; }
		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*>& getInputSubjectName() { return m_subjectName; }
		OpenViBE::Kernel::TParameterHandler<uint64_t>& getInputSubjectAge() { return m_subjectAge; }
		OpenViBE::Kernel::TParameterHandler<uint64_t>& getInputSubjectGender() { return m_subjectGender; }
		OpenViBE::Kernel::TParameterHandler<uint64_t>& getInputLaboratoryIdentifier() { return m_laboratoryID; }
		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*>& getInputLaboratoryName() { return m_laboratoryName; }
		OpenViBE::Kernel::TParameterHandler<uint64_t>& getInputTechnicianIdentifier() { return m_technicianID; }
		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*>& getInputTechnicianName() { return m_technicianName; }

	protected:
		bool encodeHeaderImpl() { return m_codec->process(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputTriggerId_EncodeHeader); }
		bool encodeBufferImpl() { return m_codec->process(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputTriggerId_EncodeBuffer); }
		bool encodeEndImpl() { return m_codec->process(OVP_GD_Algorithm_ExperimentInformationStreamEncoder_InputTriggerId_EncodeEnd); }
	};

	template <class T>
	class TExperimentInformationEncoder : public TExperimentInformationEncoderLocal<TEncoder<T>>
	{
		using TExperimentInformationEncoderLocal<TEncoder<T>>::m_boxAlgorithm;

	public:
		using TExperimentInformationEncoderLocal<TEncoder<T>>::uninitialize;

		TExperimentInformationEncoder() { }

		TExperimentInformationEncoder(T& boxAlgorithm, size_t index)
		{
			m_boxAlgorithm = NULL;
			this->initialize(boxAlgorithm, index);
		}

		virtual ~TExperimentInformationEncoder() { this->uninitialize(); }
	};
} // namespace OpenViBEToolkit

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
