#pragma once

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

#include "ovtkTDecoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TExperimentInformationDecoderLocal : public T
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
		using T::m_iBuffer;

		bool initializeImpl()
		{
			m_codec = &m_boxAlgorithm->getAlgorithmManager().getAlgorithm(m_boxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_ExperimentInformationStreamDecoder));
			m_codec->initialize();

			m_iBuffer.initialize(m_codec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_InputParameterId_MemoryBufferToDecode));

			m_experimentID.initialize(m_codec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_ExperimentIdentifier));
			m_experimentDate.initialize(m_codec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_ExperimentDate));

			m_subjectID.initialize(m_codec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectIdentifier));
			m_subjectName.initialize(m_codec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectName));
			m_subjectAge.initialize(m_codec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectAge));
			m_subjectGender.initialize(m_codec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectGender));

			m_laboratoryID.initialize(m_codec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_LaboratoryIdentifier));
			m_laboratoryName.initialize(m_codec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_LaboratoryName));
			m_technicianID.initialize(m_codec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_TechnicianIdentifier));
			m_technicianName.initialize(m_codec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_TechnicianName));

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

			m_iBuffer.uninitialize();
			m_codec->uninitialize();
			m_boxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_codec);
			m_boxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getOutputExperimentIdentifier() { return m_experimentID; }
		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*>& getOutputExperimentDate() { return m_experimentDate; }
		OpenViBE::Kernel::TParameterHandler<uint64_t>& getOutputSubjectIdentifier() { return m_subjectID; }
		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*>& getOutputSubjectName() { return m_subjectName; }
		OpenViBE::Kernel::TParameterHandler<uint64_t>& getOutputSubjectAge() { return m_subjectAge; }
		OpenViBE::Kernel::TParameterHandler<uint64_t>& getOutputSubjectGender() { return m_subjectGender; }
		OpenViBE::Kernel::TParameterHandler<uint64_t>& getOutputLaboratoryIdentifier() { return m_laboratoryID; }
		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*>& getOutputLaboratoryName() { return m_laboratoryName; }
		OpenViBE::Kernel::TParameterHandler<uint64_t>& getOutputTechnicianIdentifier() { return m_technicianID; }
		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*>& getOutputTechnicianName() { return m_technicianName; }

		virtual bool isHeaderReceived() { return m_codec->isOutputTriggerActive(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputTriggerId_ReceivedHeader); }
		virtual bool isBufferReceived() { return m_codec->isOutputTriggerActive(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputTriggerId_ReceivedBuffer); }
		virtual bool isEndReceived() { return m_codec->isOutputTriggerActive(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputTriggerId_ReceivedEnd); }
	};

	template <class T>
	class TExperimentInformationDecoder : public TExperimentInformationDecoderLocal<TDecoder<T>>
	{
		using TExperimentInformationDecoderLocal<TDecoder<T>>::m_boxAlgorithm;

	public:
		using TExperimentInformationDecoderLocal<TDecoder<T>>::uninitialize;

		TExperimentInformationDecoder() { }

		TExperimentInformationDecoder(T& boxAlgorithm, size_t index)
		{
			m_boxAlgorithm = NULL;
			this->initialize(boxAlgorithm, index);
		}

		virtual ~TExperimentInformationDecoder() { this->uninitialize(); }
	};
} // namespace OpenViBEToolkit

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
