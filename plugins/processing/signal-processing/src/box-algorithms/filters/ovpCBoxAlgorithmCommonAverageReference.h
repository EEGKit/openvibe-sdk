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

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processInput(uint32_t ui32InputIndex);
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_CommonAverageReference)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder;
			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_pMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_pMatrix;
			OpenViBE::Kernel::TParameterHandler<uint64_t> op_ui64SamplingRate;

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> ip_pMatrix;
			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64SamplingRate;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pMemoryBuffer;

			OpenViBE::CMatrix m_oMatrix;
		};

		class CBoxAlgorithmCommonAverageReferenceDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Common Average Reference"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Re-reference the signal to common average reference"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("Re-referencing the signal to common average reference consists in subtracting from each sample the average value of the samples of all electrodes at this time"); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Signal processing/Spatial Filtering"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_CommonAverageReference; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmCommonAverageReference; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Input signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput("Output signal", OV_TypeId_Signal);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_CommonAverageReferenceDesc)
		};
	}  // namespace SignalProcessing
}  // namespace OpenViBEPlugins
