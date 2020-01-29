#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>
#include <vector>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmSpectrumAverage final : public OpenViBE::Toolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }

			// virtual uint64_t getClockFrequency();
			bool initialize() override;
			bool uninitialize() override;
			// virtual bool processEvent(OpenViBE::CMessageEvent& messageEvent);
			// virtual bool processSignal(OpenViBE::CMessageSignal& messageSignal);
			// virtual bool processClock(OpenViBE::CMessageClock& messageClock);
			bool processInput(const size_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_SpectrumAverage)

		protected:

			bool m_bZeroCare = false;

			OpenViBE::Kernel::IAlgorithmProxy* m_decoder = nullptr;
			OpenViBE::Kernel::IAlgorithmProxy* m_encoder = nullptr;

			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> ip_matrix;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_matrix;

			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_buffer;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_buffer;

			std::vector<size_t> m_selectedIndices;
		};

		class CBoxAlgorithmSpectrumAverageDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Spectrum Average"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }

			OpenViBE::CString getShortDescription() const override
			{
				return OpenViBE::CString("Computes the average of all the frequency band powers for a spectrum");
			}

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
