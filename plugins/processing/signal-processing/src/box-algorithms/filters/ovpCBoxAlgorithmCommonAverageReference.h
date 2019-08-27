#pragma once

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_CommonAverageReference     OpenViBE::CIdentifier(0x009C0CE3, 0x6BDF71C3)
#define OVP_ClassId_BoxAlgorithm_CommonAverageReferenceDesc OpenViBE::CIdentifier(0x0033EAF8, 0x09C65E4E)


namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmCommonAverageReference : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_CommonAverageReference)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder = nullptr;
			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_pMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_pMatrix;
			OpenViBE::Kernel::TParameterHandler<uint64_t> op_ui64SamplingRate;

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder = nullptr;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> ip_pMatrix;
			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64SamplingRate;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pMemoryBuffer;

			OpenViBE::CMatrix m_oMatrix;
		};

		class CBoxAlgorithmCommonAverageReferenceDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Common Average Reference"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Re-reference the signal to common average reference"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("Re-referencing the signal to common average reference consists in subtracting from each sample the average value of the samples of all electrodes at this time"); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Spatial Filtering"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_CommonAverageReference; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmCommonAverageReference; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input signal", OV_TypeId_Signal);
				prototype.addOutput("Output signal", OV_TypeId_Signal);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_CommonAverageReferenceDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins
