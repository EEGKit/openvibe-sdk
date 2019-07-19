#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <vector>
#include <map>
#include <deque>
#include <cstdio>

#define OVP_ClassId_BoxAlgorithm_StimulationVoter                      OpenViBE::CIdentifier(0x2BBD61FC, 0x041A4EDB)
#define OVP_ClassId_BoxAlgorithm_StimulationVoterDesc                  OpenViBE::CIdentifier(0x1C36287C, 0x6F143FBF)

namespace OpenViBEPlugins
{
	namespace Stimulation
	{
		class CBoxAlgorithmStimulationVoter : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processInput(uint32_t ui32Index);
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_StimulationVoter);

		protected:

			uint64_t m_ui64MinimumVotes;
			double m_f64TimeWindow;
			uint64_t m_ui64RejectClassLabel;
			OpenViBE::CIdentifier m_oClearVotes;
			OpenViBE::CIdentifier m_oOutputDateMode;
			OpenViBE::CIdentifier m_oRejectClass_CanWin;

		private:

			OpenViBE::Kernel::IAlgorithmProxy* m_pEncoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pDecoder;
			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_pMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IStimulationSet*> op_pStimulationSet;

			std::deque<std::pair<uint64_t, uint64_t>> m_oStimulusDeque; // <label,time>

			uint64_t m_ui64LatestStimulusDate;
			uint64_t m_ui64LastTime;
		};


		class CBoxAlgorithmStimulationVoterListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			CBoxAlgorithmStimulationVoterListener()
				: m_oInputTypeIdentifier(OV_TypeId_Stimulations) { }

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);

		protected:

			OpenViBE::CIdentifier m_oInputTypeIdentifier;
		};

		class CBoxAlgorithmStimulationVoterDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Stimulation Voter"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Jussi T. Lindgren"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("Inria"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Performs majority vote on the input stimuli"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("Votes the most frequent stimulus ID in a given time window. Outputs the winning stimulus type. Several options are possible. To process multiple inputs, use Stimulation Multiplexer first."); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Streaming"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_StimulationVoter; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmStimulationVoter; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Stimulus input", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addOutput("Selected stimulus", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addSetting("Number of stimuli required for vote", OV_TypeId_Integer, "4");
				rBoxAlgorithmPrototype.addSetting("Time window (secs)", OV_TypeId_Float, "2");
				rBoxAlgorithmPrototype.addSetting("Clear votes", OVP_TypeId_Voting_ClearVotes, "After output");
				rBoxAlgorithmPrototype.addSetting("Output timestamp", OVP_TypeId_Voting_OutputTime, "Time of last voting stimulus");
				rBoxAlgorithmPrototype.addSetting("Reject class label", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
				rBoxAlgorithmPrototype.addSetting("Reject class can win", OVP_TypeId_Voting_RejectClass_CanWin, "No");

				return true;
			}

			virtual OpenViBE::Plugins::IBoxListener* createBoxListener() const { return new CBoxAlgorithmStimulationVoterListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StimulationVoterDesc);
		};
	};
};


