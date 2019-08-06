#pragma once

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_SignalDecimation     OpenViBE::CIdentifier(0x012F4BEA, 0x3BE37C66)
#define OVP_ClassId_BoxAlgorithm_SignalDecimationDesc OpenViBE::CIdentifier(0x1C5F1356, 0x1E685777)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmSignalDecimation : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processInput(const uint32_t ui32InputIndex);
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_SignalDecimation)

		protected:

			int64_t m_i64DecimationFactor;
			uint32_t m_ui32ChannelCount;
			uint32_t m_ui32InputSampleIndex;
			uint32_t m_ui32InputSampleCountPerSentBlock;
			uint64_t m_ui64InputSamplingFrequency;
			uint32_t m_ui32OutputSampleIndex;
			uint32_t m_ui32OutputSampleCountPerSentBlock;
			uint64_t m_ui64OutputSamplingFrequency;

			uint64_t m_ui64TotalSampleCount;
			uint64_t m_ui64StartTimeBase;
			uint64_t m_ui64LastStartTime;
			uint64_t m_ui64LastEndTime;

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder;
			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_pMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_pMatrix;
			OpenViBE::Kernel::TParameterHandler<uint64_t> op_ui64SamplingRate;

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;
			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64SamplingRate;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> ip_pMatrix;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pMemoryBuffer;
		};

		class CBoxAlgorithmSignalDecimationDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Signal Decimation"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Reduces the sampling frequency to a divider of the original sampling frequency"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("No pre filtering applied - Number of samples per block have to be a multiple of the decimation factor"); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Signal processing/Temporal Filtering"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_SignalDecimation; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmSignalDecimation; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Input signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput("Output signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("Decimation factor", OV_TypeId_Integer, "8");
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SignalDecimationDesc)
		};
	}  // namespace SignalProcessing
}  // namespace OpenViBEPlugins
