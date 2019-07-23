#pragma once

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>
#include <vector>

#define OVP_ClassId_BoxAlgorithm_SpectrumAverage     OpenViBE::CIdentifier(0x0C092665, 0x61B82641)
#define OVP_ClassId_BoxAlgorithm_SpectrumAverageDesc OpenViBE::CIdentifier(0x24663D96, 0x71EA7295)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmSpectrumAverage : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			virtual void release() { delete this; }

			// virtual uint64_t getClockFrequency();
			virtual bool initialize();
			virtual bool uninitialize();
			// virtual bool processEvent(OpenViBE::CMessageEvent& rMessageEvent);
			// virtual bool processSignal(OpenViBE::CMessageSignal& rMessageSignal);
			// virtual bool processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual bool processInput(uint32_t ui32InputIndex);
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_SpectrumAverage)

		protected:

			bool m_bZeroCare;

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;

			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> ip_pMatrix;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_pMatrix;

			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_pMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pMemoryBuffer;

			std::vector<uint32_t> m_vSelectedIndices;
		};

		class CBoxAlgorithmSpectrumAverageDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Spectrum Average"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Computes the average of all the frequency band powers for a spectrum"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Signal processing/Spectral Analysis"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_SpectrumAverage; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmSpectrumAverage; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Spectrum", OV_TypeId_Spectrum);
				rBoxAlgorithmPrototype.addOutput("Spectrum average", OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addSetting("Considers zeros", OV_TypeId_Boolean, "false");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SpectrumAverageDesc)
		};
	}  // namespace SignalProcessing
}  // namespace OpenViBEPlugins
