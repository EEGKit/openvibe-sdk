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
		class CBoxAlgorithmSpectrumAverage final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }

			// virtual uint64_t getClockFrequency();
			bool initialize() override;
			bool uninitialize() override;
			// virtual bool processEvent(OpenViBE::CMessageEvent& rMessageEvent);
			// virtual bool processSignal(OpenViBE::CMessageSignal& rMessageSignal);
			// virtual bool processClock(OpenViBE::CMessageClock& rMessageClock);
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_SpectrumAverage)

		protected:

			bool m_bZeroCare = false;

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder = nullptr;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder = nullptr;

			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> ip_pMatrix;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_pMatrix;

			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_pMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pMemoryBuffer;

			std::vector<uint32_t> m_vSelectedIndices;
		};

		class CBoxAlgorithmSpectrumAverageDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Spectrum Average"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Computes the average of all the frequency band powers for a spectrum"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Spectral Analysis"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_SpectrumAverage; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmSpectrumAverage; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Spectrum", OV_TypeId_Spectrum);
				prototype.addOutput("Spectrum average", OV_TypeId_StreamedMatrix);
				prototype.addSetting("Considers zeros", OV_TypeId_Boolean, "false");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SpectrumAverageDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins
