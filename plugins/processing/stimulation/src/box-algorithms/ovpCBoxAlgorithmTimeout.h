#pragma once

#include "../ovp_defines.h"

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_Timeout OpenViBE::CIdentifier(0x24FCD292, 0x5C8F6AA8)
#define OVP_ClassId_BoxAlgorithm_TimeoutDesc OpenViBE::CIdentifier(0xB3D90844, 0x79A4CF6D)

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		/**
		 * \class CBoxAlgorithmTimeout
		 * \author Jozef Legény (Inria)
		 * \date Thu Mar 21 14:40:23 2013
		 * \brief The class CBoxAlgorithmTimeout describes the box Timeout.
		 *
		 */
		class CBoxAlgorithmTimeout : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			virtual void release(void) { delete this; }

			virtual bool initialize(void);
			virtual bool uninitialize(void);

			virtual bool processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual bool processInput(uint32_t ui32InputIndex);

			virtual uint64_t getClockFrequency(void);

			virtual bool process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_Timeout)

		protected:
			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmTimeout> m_StimulationEncoder;

		private:
			enum ETimeoutState
			{
				ETimeout_No,
				ETimeout_Occurred,
				ETimeout_Sent
			};

			ETimeoutState m_TimeoutState;
			bool m_IsHeaderSent;

			uint64_t m_Timeout;
			uint64_t m_LastTimePolled;
			uint64_t m_PreviousTime;
			uint64_t m_StimulationToSend;
		};

		/**
		 * \class CBoxAlgorithmTimeoutDesc
		 * \author Jozef Legény (Inria)
		 * \date Thu Mar 21 14:40:23 2013
		 * \brief Descriptor of the box Timeout.
		 *
		 */
		class CBoxAlgorithmTimeoutDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const { return OpenViBE::CString("Timeout"); }
			virtual OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Jozef Legény"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("Inria"); }
			virtual OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString("Sends a stimulation after a period of time without receiving signal"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Sends a stimulation after a period of time without receiving signal. Useful for stopping scenarios after hardware disconnection."); }
			virtual OpenViBE::CString getCategory(void) const { return OpenViBE::CString("Stimulation"); }
			virtual OpenViBE::CString getVersion(void) const { return OpenViBE::CString("1.1"); }
			virtual OpenViBE::CString getSoftwareComponent(void) const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_BoxAlgorithm_Timeout; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new OpenViBEPlugins::Stimulation::CBoxAlgorithmTimeout; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Input Stream",OV_TypeId_StreamedMatrix);

				rBoxAlgorithmPrototype.addOutput("Output Stimulations",OV_TypeId_Stimulations);

				rBoxAlgorithmPrototype.addSetting("Timeout delay",OV_TypeId_Integer, "5");
				rBoxAlgorithmPrototype.addSetting("Output Stimulation",OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_TimeoutDesc);
		};
	};
};
