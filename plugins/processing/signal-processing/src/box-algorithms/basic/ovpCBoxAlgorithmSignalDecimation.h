#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmSignalDecimation final : public OpenViBE::Toolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const size_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_SignalDecimation)

		protected:

			size_t m_decimationFactor = 0;
			size_t m_nChannel         = 0;
			size_t m_iSampleIdx       = 0;
			size_t m_iNSamplePerBlock = 0;
			size_t m_iSampling        = 0;
			size_t m_oSampleIdx       = 0;
			size_t m_oNSamplePerBlock = 0;
			size_t m_oSampling        = 0;

			size_t m_nTotalSample    = 0;
			uint64_t m_startTimeBase = 0;
			uint64_t m_lastStartTime = 0;
			uint64_t m_lastEndTime   = 0;

			OpenViBE::Kernel::IAlgorithmProxy* m_decoder = nullptr;
			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_buffer;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_pMatrix;
			OpenViBE::Kernel::TParameterHandler<uint64_t> op_sampling;

			OpenViBE::Kernel::IAlgorithmProxy* m_encoder = nullptr;
			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_sampling;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> ip_pMatrix;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_buffer;
		};

		class CBoxAlgorithmSignalDecimationDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Signal Decimation"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }

			OpenViBE::CString getShortDescription() const override
			{
				return OpenViBE::CString("Reduces the sampling frequency to a divider of the original sampling frequency");
			}

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString("No pre filtering applied - Number of samples per block have to be a multiple of the decimation factor");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Temporal Filtering"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_SignalDecimation; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmSignalDecimation; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input signal", OV_TypeId_Signal);
				prototype.addOutput("Output signal", OV_TypeId_Signal);
				prototype.addSetting("Decimation factor", OV_TypeId_Integer, "8");
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SignalDecimationDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins
