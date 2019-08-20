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
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t ui32InputIndex) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_SignalDecimation)

		protected:

			int64_t m_i64DecimationFactor                = 0;
			uint32_t m_ui32ChannelCount                  = 0;
			uint32_t m_ui32InputSampleIndex              = 0;
			uint32_t m_ui32InputSampleCountPerSentBlock  = 0;
			uint64_t m_ui64InputSamplingFrequency        = 0;
			uint32_t m_ui32OutputSampleIndex             = 0;
			uint32_t m_ui32OutputSampleCountPerSentBlock = 0;
			uint64_t m_ui64OutputSamplingFrequency       = 0;

			uint64_t m_ui64TotalSampleCount = 0;
			uint64_t m_ui64StartTimeBase    = 0;
			uint64_t m_ui64LastStartTime    = 0;
			uint64_t m_ui64LastEndTime      = 0;

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder = nullptr;
			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_pMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_pMatrix;
			OpenViBE::Kernel::TParameterHandler<uint64_t> op_ui64SamplingRate;

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder = nullptr;
			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64SamplingRate;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> ip_pMatrix;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pMemoryBuffer;
		};

		class CBoxAlgorithmSignalDecimationDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Signal Decimation"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Reduces the sampling frequency to a divider of the original sampling frequency"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("No pre filtering applied - Number of samples per block have to be a multiple of the decimation factor"); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Temporal Filtering"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_SignalDecimation; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmSignalDecimation; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const override
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
