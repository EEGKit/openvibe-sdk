#pragma once

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#include "../../ovtk_base.h"

#include "ovtkTStreamedMatrixDecoder.h"

namespace OpenViBEToolkit
{
	template <class T>
	class TAcquisitionDecoderLocal : public T
	{
	protected:

		OpenViBE::Kernel::TParameterHandler<uint64_t> op_ui64BufferDuration;
		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pExperimentInformationStream;
		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pSignalStream;
		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pStimulationStream;
		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pChannelLocalisationStream;
		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pChannelUnitsStream;

		using T::m_pCodec;
		using T::m_pBoxAlgorithm;
		using T::m_pInputMemoryBuffer;

		bool initializeImpl()
		{
			m_pCodec = &m_pBoxAlgorithm->getAlgorithmManager().getAlgorithm(m_pBoxAlgorithm->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_AcquisitionStreamDecoder));
			m_pCodec->initialize();

			m_pInputMemoryBuffer.initialize(m_pCodec->getInputParameter(OVP_GD_Algorithm_AcquisitionStreamDecoder_InputParameterId_MemoryBufferToDecode));

			op_ui64BufferDuration.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_BufferDuration));
			op_pExperimentInformationStream.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ExperimentInformationStream));
			op_pSignalStream.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_SignalStream));
			op_pStimulationStream.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_StimulationStream));
			op_pChannelLocalisationStream.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ChannelLocalisationStream));
			op_pChannelUnitsStream.initialize(m_pCodec->getOutputParameter(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ChannelUnitsStream));

			return true;
		}

	public:

		using T::initialize;

		bool uninitialize()
		{
			if (m_pBoxAlgorithm == nullptr || m_pCodec == nullptr) { return false; }

			op_pChannelUnitsStream.uninitialize();
			op_pChannelLocalisationStream.uninitialize();
			op_pStimulationStream.uninitialize();
			op_pSignalStream.uninitialize();
			op_pExperimentInformationStream.uninitialize();
			op_ui64BufferDuration.uninitialize();

			m_pInputMemoryBuffer.uninitialize();
			m_pCodec->uninitialize();
			m_pBoxAlgorithm->getAlgorithmManager().releaseAlgorithm(*m_pCodec);
			m_pBoxAlgorithm = NULL;

			return true;
		}

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getBufferDuration() { return op_ui64BufferDuration; }
		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*>& getExperimentInformationStream() { return op_pExperimentInformationStream; }
		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*>& getSignalStream() { return op_pSignalStream; }
		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*>& getStimulationStream() { return op_pStimulationStream; }
		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*>& getChannelLocalisationStream() { return op_pChannelLocalisationStream; }
		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*>& getChannelUnitsStream() { return op_pChannelUnitsStream; }

		virtual bool isHeaderReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputTriggerId_ReceivedHeader);
		}

		virtual bool isBufferReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputTriggerId_ReceivedBuffer);
		}

		virtual bool isEndReceived()
		{
			return m_pCodec->isOutputTriggerActive(OVP_GD_Algorithm_AcquisitionStreamDecoder_OutputTriggerId_ReceivedEnd);
		}
	};

	template <class T>
	class TAcquisitionDecoder : public TAcquisitionDecoderLocal<TStreamedMatrixDecoderLocal<TDecoder<T>>>
	{
	private:

		using TAcquisitionDecoderLocal<TStreamedMatrixDecoderLocal<TDecoder<T>>>::m_pBoxAlgorithm;

	public:

		using TAcquisitionDecoderLocal<TStreamedMatrixDecoderLocal<TDecoder<T>>>::uninitialize;

		TAcquisitionDecoder() { }

		TAcquisitionDecoder(T& rBoxAlgorithm)
		{
			m_pBoxAlgorithm = NULL;
			this->initialize(rBoxAlgorithm);
		}

		virtual ~TAcquisitionDecoder()
		{
			this->uninitialize();
		}
	};
}  // namespace OpenViBEToolkit

#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
