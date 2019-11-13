#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>
#include <vector>
#include <map>

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		class CBoxAlgorithmStimulationMultiplexer final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			void release() override { delete this; }

			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const size_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_StimulationMultiplexer)

		private:

			std::vector<OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmStimulationMultiplexer>> m_decoders;
			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmStimulationMultiplexer> m_encoder;

			std::vector<uint64_t> m_decoderEndTimes;

			uint64_t m_lastStartTime = 0;
			uint64_t m_lastEndTime   = 0;
			bool m_wasHeaderSent     = false;

			std::multimap<uint64_t, std::tuple<uint64_t, uint64_t, uint64_t>> m_vStimulation;
		};

		class CBoxAlgorithmStimulationMultiplexerListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			bool check(OpenViBE::Kernel::IBox& box) const
			{
				for (size_t input = 0; input < box.getInputCount(); ++input)
				{
					box.setInputName(input, ("Input stimulations " + std::to_string(input + 1)).c_str());
					box.setInputType(input, OV_TypeId_Stimulations);
				}

				return true;
			}

			bool onInputRemoved(OpenViBE::Kernel::IBox& box, const size_t /*index*/) override { return this->check(box); }
			bool onInputAdded(OpenViBE::Kernel::IBox& box, const size_t /*index*/) override { return this->check(box); }

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmStimulationMultiplexerDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			void release() override { }

			OpenViBE::CString getName() const override { return OpenViBE::CString("Stimulation multiplexer"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Merges several stimulation streams into one."); }

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString("The stimulations are ordered according to their start date. Thus each time all the input have chunks covering a period of time, a new output chunk is sent. This box may eventually produce output chunk reflecting a different duration depending on the inputs.");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Streaming"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.1"); }
			OpenViBE::CString getStockItemName() const override { return OpenViBE::CString("gtk-sort-ascending"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }

			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_StimulationMultiplexer; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmStimulationMultiplexer; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmStimulationMultiplexerListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input stimulations 1", OV_TypeId_Stimulations);
				prototype.addInput("Input stimulations 2", OV_TypeId_Stimulations);
				prototype.addOutput("Multiplexed stimulations", OV_TypeId_Stimulations);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				prototype.addInputSupport(OV_TypeId_Stimulations);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StimulationMultiplexerDesc)
		};
	} // namespace Stimulation
} // namespace OpenViBEPlugins
