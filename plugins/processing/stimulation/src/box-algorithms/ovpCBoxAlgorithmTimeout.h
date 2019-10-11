#pragma once

#include "../ovp_defines.h"

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

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
			bool processInput(const size_t index) override;
			uint64_t getClockFrequency() override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_Timeout)

		protected:
			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmTimeout> m_encoder;

		private:
			enum ETimeoutState
			{
				Timeout_No,
				Timeout_Occurred,
				Timeout_Sent
			};

			ETimeoutState m_timeoutState = Timeout_No;
			bool m_isHeaderSent = false;

			uint64_t m_timeout           = 0;
			uint64_t m_lastTimePolled    = 0;
			uint64_t m_previousTime      = 0;
			uint64_t m_stimulationToSend = 0;
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

			OpenViBE::CString getShortDescription() const override
			{
				return OpenViBE::CString("Sends a stimulation after a period of time without receiving signal");
			}

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString(
					"Sends a stimulation after a period of time without receiving signal. Useful for stopping scenarios after hardware disconnection.");
			}

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
