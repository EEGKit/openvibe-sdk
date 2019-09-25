#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		class CBoxAlgorithmClockStimulator final : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			uint64_t getClockFrequency() override;
			bool initialize() override;
			bool uninitialize() override;
			bool processClock(OpenViBE::CMessageClock& messageClock) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ClockStimulator)

		protected:

			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmClockStimulator> m_oStimulationEncoder;
			uint64_t m_ui64StimulationId       = 0;
			uint64_t m_ui64LastStimulationDate = 0;
			uint64_t m_ui64LastEndTime         = 0;

			uint64_t m_SentStimulationCount = 0;
			double m_StimulationInterval    = 0;
		};

		class CBoxAlgorithmClockStimulatorDesc final : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Clock stimulator"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Periodic stimulation generator"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("Triggers stimulation at fixed frequency"); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stimulation"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_ClockStimulator; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmClockStimulator; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addOutput("Generated stimulations", OV_TypeId_Stimulations);
				prototype.addSetting("Interstimulation interval (in sec)", OV_TypeId_Float, "1.0");
				prototype.addSetting("Stimulation", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ClockStimulatorDesc)
		};
	} // namespace Stimulation
} // namespace OpenViBEPlugins
