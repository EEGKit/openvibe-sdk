#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_ClockStimulator     OpenViBE::CIdentifier(0x4F756D3F, 0x29FF0B96)
#define OVP_ClassId_BoxAlgorithm_ClockStimulatorDesc OpenViBE::CIdentifier(0x4FD067E9, 0x740D2AF0)

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		class CBoxAlgorithmClockStimulator : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			virtual void release() { delete this; }

			virtual uint64_t getClockFrequency();
			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ClockStimulator)

		protected:

			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmClockStimulator> m_oStimulationEncoder;
			uint64_t m_ui64StimulationId;
			uint64_t m_ui64LastStimulationDate;
			uint64_t m_ui64LastEndTime;

			uint64_t m_SentStimulationCount;
			double m_StimulationInterval;
		};

		class CBoxAlgorithmClockStimulatorDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Clock stimulator"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Periodic stimulation generator"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("Triggers stimulation at fixed frequency"); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Stimulation"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_ClockStimulator; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmClockStimulator; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addOutput("Generated stimulations", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addSetting("Interstimulation interval (in sec)", OV_TypeId_Float, "1.0");
				rBoxAlgorithmPrototype.addSetting("Stimulation", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ClockStimulatorDesc)
		};
	};
};


