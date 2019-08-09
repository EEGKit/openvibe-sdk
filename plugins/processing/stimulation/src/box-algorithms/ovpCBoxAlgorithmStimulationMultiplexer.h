#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>
#include <vector>
#include <map>
#include <cstdio>

#define OVP_ClassId_BoxAlgorithm_StimulationMultiplexer       OpenViBE::CIdentifier(0x07DB4EFA, 0x472B0938)
#define OVP_ClassId_BoxAlgorithm_StimulationMultiplexerDesc   OpenViBE::CIdentifier(0x79EF4E4D, 0x178F09E6)

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		class CBoxAlgorithmStimulationMultiplexer : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			void release() override { delete this; }

			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t ui32InputIndex) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_StimulationMultiplexer)

		private:

			std::vector<OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmStimulationMultiplexer>> m_StimulationDecoders;
			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmStimulationMultiplexer> m_StimulationEncoder;

			std::vector<uint64_t> m_StreamDecoderEndTimes;

			uint64_t m_LastStartTime = 0;
			uint64_t m_LastEndTime = 0;
			bool m_WasHeaderSent = false;

			std::multimap<uint64_t, std::tuple<uint64_t, uint64_t, uint64_t>> m_vStimulation;
		};

		class CBoxAlgorithmStimulationMultiplexerListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			bool check(OpenViBE::Kernel::IBox& box)
			{
				char inputName[1024];

				for (uint32_t input = 0; input < box.getInputCount(); ++input)
				{
					sprintf(inputName, "Input stimulations %u", input + 1);
					box.setInputName(input, inputName);
					box.setInputType(input, OV_TypeId_Stimulations);
				}

				return true;
			}

			bool onInputRemoved(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) override { return this->check(rBox); }
			bool onInputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) override { return this->check(rBox); }

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmStimulationMultiplexerDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			void release() override { }

			OpenViBE::CString getName() const override { return OpenViBE::CString("Stimulation multiplexer"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Merges several stimulation streams into one."); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("The stimulations are ordered according to their start date. Thus each time all the input have chunks covering a period of time, a new output chunk is sent. This box may eventually produce output chunk reflecting a different duration depending on the inputs."); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Streaming"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.1"); }
			OpenViBE::CString getStockItemName() const override { return OpenViBE::CString("gtk-sort-ascending"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }

			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_StimulationMultiplexer; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmStimulationMultiplexer; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmStimulationMultiplexerListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const override { delete pBoxListener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const override
			{
				rBoxAlgorithmPrototype.addInput("Input stimulations 1", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInput("Input stimulations 2", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addOutput("Multiplexed stimulations", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_Stimulations);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StimulationMultiplexerDesc)
		};
	}
}
