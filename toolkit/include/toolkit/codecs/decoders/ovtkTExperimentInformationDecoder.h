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

		OpenViBE::Kernel::TParameterHandler<uint64_t> m_outputExperimentID;
		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> m_pOutputpExperimentDate;

		OpenViBE::Kernel::TParameterHandler<uint64_t> m_outputSubjectID;
		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> m_pOutputpSubjectName;
		OpenViBE::Kernel::TParameterHandler<uint64_t> m_outputSubjectAge;
		OpenViBE::Kernel::TParameterHandler<uint64_t> m_outputSubjectGender;

		OpenViBE::Kernel::TParameterHandler<uint64_t> m_outputLaboratoryID;
		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> m_pOutputpLaboratoryName;
		OpenViBE::Kernel::TParameterHandler<uint64_t> m_outputTechnicianID;
		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> m_pOutputpTechnicianName;

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pInputMemoryBuffer;

		bool initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(
				m_pBoxAlgorithm->getAlgorithmManager().
								 createAlgorithm(OVP_GD_ClassId_Algorithm_ExperimentInformationStreamDecoder));
			m_pCodec->initialize();

			m_pInputMemoryBuffer.initialize(
				m_pCodec->getInputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_InputParameterId_MemoryBufferToDecode));

			m_outputExperimentID.initialize(
				m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_ExperimentIdentifier));
			m_pOutputpExperimentDate.initialize(
				m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_ExperimentDate));

			m_outputSubjectID.initialize(
				m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectIdentifier));
			m_pOutputpSubjectName.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectName));
			m_outputSubjectAge.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectAge));
			m_outputSubjectGender.initialize(
				m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectGender));

			m_outputLaboratoryID.initialize(
				m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_LaboratoryIdentifier));
			m_pOutputpLaboratoryName.initialize(
				m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_LaboratoryName));
			m_outputTechnicianID.initialize(
				m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_TechnicianIdentifier));
			m_pOutputpTechnicianName.initialize(
				m_pCodec->getOutputParameter(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_TechnicianName));

			return true;
		}

	public:
		using T::initialize;

		bool uninitialize()
		{
			if (m_pBoxAlgorithm == nullptr || m_pCodec == nullptr) { return false; }

			m_outputExperimentID.uninitialize();
			m_pOutputpExperimentDate.uninitialize();
			m_outputSubjectID.uninitialize();
			m_pOutputpSubjectName.uninitialize();
			m_outputSubjectAge.uninitialize();
			m_outputSubjectGender.uninitialize();
			m_outputLaboratoryID.uninitialize();
			m_pOutputpLaboratoryName.uninitialize();
			m_outputTechnicianID.uninitialize();
			m_pOutputpTechnicianName.uninitialize();

			m_pInputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getOutputExperimentIdentifier() { return m_outputExperimentID; }

		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*>& getOutputExperimentDate() { return m_pOutputpExperimentDate; }

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getOutputSubjectIdentifier() { return m_outputSubjectID; }

		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*>& getOutputSubjectName() { return m_pOutputpSubjectName; }

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getOutputSubjectAge() { return m_outputSubjectAge; }

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getOutputSubjectGender() { return m_outputSubjectGender; }

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getOutputLaboratoryIdentifier() { return m_outputLaboratoryID; }

		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*>& getOutputLaboratoryName() { return m_pOutputpLaboratoryName; }

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getOutputTechnicianIdentifier() { return m_outputTechnicianID; }

		OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*>& getOutputTechnicianName() { return m_pOutputpTechnicianName; }

		virtual bool isHeaderReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputTriggerId_ReceivedHeader);
		}

		virtual bool isBufferReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputTriggerId_ReceivedBuffer);
		}

		virtual bool isEndReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_ExperimentInformationStreamDecoder_OutputTriggerId_ReceivedEnd);
		}
	};

	template <class T>
	class TExperimentInformationDecoder : public TExperimentInformationDecoderLocal<TDecoder<T>>
	{
		using TExperimentInformationDecoderLocal<TDecoder<T>>::m_pBoxAlgorithm;

	public:
		using TExperimentInformationDecoderLocal<TDecoder<T>>::uninitialize;

		TExperimentInformationDecoder() { }

		TExperimentInformationDecoder(T& rBoxAlgorithm, uint32_t ui32ConnectorIndex)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm, ui32ConnectorIndex);
		}

		virtual ~TExperimentInformationDecoder() { this->uninitialize(); }
	};
} // namespace OpenViBEToolkit

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
