#pragma once

#include "../ovp_defines.h"
#include <toolkit/ovtk_all.h>
#include <array>

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CEncoderAlgorithmTest : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			uint64_t getClockFrequency() override { return 1LL << 32; }
			bool initialize() override;
			bool uninitialize() override;
			bool processClock(OpenViBE::Kernel::IMessageClock& messageClock) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_EncoderAlgorithmTest)

		protected:

			std::array<OpenViBE::Kernel::IAlgorithmProxy*, 7> m_encoders;
			std::array<OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*>, 7> op_pMemoryBuffer;

			bool m_hasSentHeader = false;
			uint64_t m_startTime = 0;
			uint64_t m_endTime   = 0;

			OpenViBE::CMatrix* m_matrix1         = nullptr;
			OpenViBE::CMatrix* m_matrix2         = nullptr;
			OpenViBE::CMatrix* m_matrix3         = nullptr;
			OpenViBE::CStimulationSet* m_stimSet = nullptr;
		};

		class CEncoderAlgorithmTestDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Encoder algorithm test"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Setups various streams and outputs them"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("Note: Data sent in the streams does not change over time"); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Tests/Algorithms"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_EncoderAlgorithmTest; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CEncoderAlgorithmTest(); }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const override
			{
				rPrototype.addOutput("Experiment information", OV_TypeId_ExperimentInfo);
				rPrototype.addOutput("Feature vector", OV_TypeId_FeatureVector);
				rPrototype.addOutput("Signal", OV_TypeId_Signal);
				rPrototype.addOutput("Spectrum", OV_TypeId_Spectrum);
				rPrototype.addOutput("Stimulation", OV_TypeId_Stimulations);
				rPrototype.addOutput("Streamed matrix", OV_TypeId_StreamedMatrix);
				rPrototype.addOutput("Channel localisation", OV_TypeId_ChannelLocalisation);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_EncoderAlgorithmTestDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
