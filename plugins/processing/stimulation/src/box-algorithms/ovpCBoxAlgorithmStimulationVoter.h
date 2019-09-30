#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <map>
#include <deque>

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		class CBoxAlgorithmStimulationVoter final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_StimulationVoter)

		protected:

			uint64_t m_minimumVotes                   = 0;
			double m_timeWindow                       = 0;
			uint64_t m_rejectClassLabel               = 0;
			OpenViBE::CIdentifier m_clearVotes        = OV_UndefinedIdentifier;
			OpenViBE::CIdentifier m_outputDateMode    = OV_UndefinedIdentifier;
			OpenViBE::CIdentifier m_rejectClassCanWin = OV_UndefinedIdentifier;

		private:

			OpenViBE::Kernel::IAlgorithmProxy* m_encoder = nullptr;
			OpenViBE::Kernel::IAlgorithmProxy* m_decoder = nullptr;
			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_pMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IStimulationSet*> op_pStimulationSet;

			std::deque<std::pair<uint64_t, uint64_t>> m_oStimulusDeque; // <label,time>

			uint64_t m_latestStimulusDate = 0;
			uint64_t m_lastTime           = 0;
		};


		class CBoxAlgorithmStimulationVoterListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			CBoxAlgorithmStimulationVoterListener() : m_inputTypeID(OV_TypeId_Stimulations) { }

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)

		protected:

			OpenViBE::CIdentifier m_inputTypeID = OV_UndefinedIdentifier;
		};

		class CBoxAlgorithmStimulationVoterDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Stimulation Voter"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Jussi T. Lindgren"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Inria"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Performs majority vote on the input stimuli"); }

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString(
					"Votes the most frequent stimulus ID in a given time window. Outputs the winning stimulus type. Several options are possible. To process multiple inputs, use Stimulation Multiplexer first.");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Streaming"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_StimulationVoter; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmStimulationVoter; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Stimulus input", OV_TypeId_Stimulations);
				prototype.addOutput("Selected stimulus", OV_TypeId_Stimulations);
				prototype.addSetting("Number of stimuli required for vote", OV_TypeId_Integer, "4");
				prototype.addSetting("Time window (secs)", OV_TypeId_Float, "2");
				prototype.addSetting("Clear votes", OVP_TypeId_Voting_ClearVotes, "After output");
				prototype.addSetting("Output timestamp", OVP_TypeId_Voting_OutputTime, "Time of last voting stimulus");
				prototype.addSetting("Reject class label", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
				prototype.addSetting("Reject class can win", OVP_TypeId_Voting_RejectClass_CanWin, "No");

				return true;
			}

			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmStimulationVoterListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StimulationVoterDesc)
		};
	} // namespace Stimulation
} // namespace OpenViBEPlugins
