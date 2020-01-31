#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>
#include <vector>
#include <map>

namespace OpenViBE
{
	namespace Plugins
	{
		namespace Stimulation
		{
			class CBoxAlgorithmStimulationMultiplexer final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
			{
			public:

				void release() override { delete this; }

				bool initialize() override;
				bool uninitialize() override;
				bool processInput(const size_t index) override;
				bool process() override;

				_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_StimulationMultiplexer)

			private:

				std::vector<Toolkit::TStimulationDecoder<CBoxAlgorithmStimulationMultiplexer>> m_decoders;
				Toolkit::TStimulationEncoder<CBoxAlgorithmStimulationMultiplexer> m_encoder;

				std::vector<uint64_t> m_decoderEndTimes;

				uint64_t m_lastStartTime = 0;
				uint64_t m_lastEndTime   = 0;
				bool m_wasHeaderSent     = false;

				std::multimap<uint64_t, std::tuple<uint64_t, uint64_t, uint64_t>> m_stimulations;
			};

			class CBoxAlgorithmStimulationMultiplexerListener final : public Toolkit::TBoxListener<IBoxListener>
			{
			public:

				bool check(Kernel::IBox& box) const
				{
					for (size_t input = 0; input < box.getInputCount(); ++input)
					{
						box.setInputName(input, ("Input stimulations " + std::to_string(input + 1)).c_str());
						box.setInputType(input, OV_TypeId_Stimulations);
					}

					return true;
				}

				bool onInputRemoved(Kernel::IBox& box, const size_t /*index*/) override { return this->check(box); }
				bool onInputAdded(Kernel::IBox& box, const size_t /*index*/) override { return this->check(box); }

				_IsDerivedFromClass_Final_(Toolkit::TBoxListener<IBoxListener>, OV_UndefinedIdentifier)
			};

			class CBoxAlgorithmStimulationMultiplexerDesc final : public IBoxAlgorithmDesc
			{
			public:

				void release() override { }

				CString getName() const override { return CString("Stimulation multiplexer"); }
				CString getAuthorName() const override { return CString("Yann Renard"); }
				CString getAuthorCompanyName() const override { return CString("INRIA/IRISA"); }
				CString getShortDescription() const override { return CString("Merges several stimulation streams into one."); }

				CString getDetailedDescription() const override
				{
					return CString(
						"The stimulations are ordered according to their start date. Thus each time all the input have chunks covering a period of time, a new output chunk is sent. This box may eventually produce output chunk reflecting a different duration depending on the inputs.");
				}

				CString getCategory() const override { return CString("Streaming"); }
				CString getVersion() const override { return CString("1.1"); }
				CString getStockItemName() const override { return CString("gtk-sort-ascending"); }
				CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
				CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
				CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }

				CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_StimulationMultiplexer; }
				IPluginObject* create() override { return new CBoxAlgorithmStimulationMultiplexer; }
				IBoxListener* createBoxListener() const override { return new CBoxAlgorithmStimulationMultiplexerListener; }
				void releaseBoxListener(IBoxListener* listener) const override { delete listener; }

				bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
				{
					prototype.addInput("Input stimulations 1", OV_TypeId_Stimulations);
					prototype.addInput("Input stimulations 2", OV_TypeId_Stimulations);
					prototype.addOutput("Multiplexed stimulations", OV_TypeId_Stimulations);
					prototype.addFlag(Kernel::BoxFlag_CanAddInput);
					prototype.addInputSupport(OV_TypeId_Stimulations);
					return true;
				}

				_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StimulationMultiplexerDesc)
			};
		} // namespace Stimulation
	}  // namespace Plugins
}  // namespace OpenViBE
