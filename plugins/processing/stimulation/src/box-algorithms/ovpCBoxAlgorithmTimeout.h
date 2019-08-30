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
		class CBoxAlgorithmTimeout final : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processClock(OpenViBE::CMessageClock& messageClock) override;
			bool processInput(const uint32_t index) override;
			uint64_t getClockFrequency() override;
			bool process() override;

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
			bool m_IsHeaderSent = false;

			uint64_t m_Timeout           = 0;
			uint64_t m_LastTimePolled    = 0;
			uint64_t m_PreviousTime      = 0;
			uint64_t m_StimulationToSend = 0;
		};

		/**
		 * \class CBoxAlgorithmTimeoutDesc
		 * \author Jozef Legény (Inria)
		 * \date Thu Mar 21 14:40:23 2013
		 * \brief Descriptor of the box Timeout.
		 *
		 */
		class CBoxAlgorithmTimeoutDesc final : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Timeout"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Jozef Legény"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Inria"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Sends a stimulation after a period of time without receiving signal"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("Sends a stimulation after a period of time without receiving signal. Useful for stopping scenarios after hardware disconnection."); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stimulation"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.1"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_Timeout; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmTimeout; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input Stream",OV_TypeId_StreamedMatrix);

				prototype.addOutput("Output Stimulations",OV_TypeId_Stimulations);

				prototype.addSetting("Timeout delay",OV_TypeId_Integer, "5");
				prototype.addSetting("Output Stimulation",OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_TimeoutDesc)
		};
	} // namespace Stimulation
} // namespace OpenViBEPlugins
