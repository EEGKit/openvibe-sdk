#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <vector>
#include <map>

namespace OpenViBEPlugins
{
	namespace Classification
	{
		class CBoxAlgorithmVotingClassifier final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const size_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_VotingClassifier)

		protected:

			size_t m_nRepetitions  = 0;
			size_t m_targetClassLabel     = 0;
			size_t m_nonTargetClassLabel  = 0;
			size_t m_rejectClassLabel     = 0;
			size_t m_resultClassLabelBase = 0;
			bool m_bChooseOneIfExAequo = false;

		private:

			typedef struct
			{
				OpenViBEToolkit::TDecoder<CBoxAlgorithmVotingClassifier>* m_pDecoder = nullptr;
				OpenViBE::Kernel::TParameterHandler<OpenViBE::IStimulationSet*> op_pStimulationSet;
				OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_pMatrix;
				bool m_bTwoValueInput;
				std::vector<std::pair<double, uint64_t>> m_vScore;
			} SInput;

			std::map<uint32_t, SInput> m_vClassificationResults;

			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmVotingClassifier> m_oClassificationChoiceEncoder;
			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IStimulationSet*> ip_pClassificationChoiceStimulationSet;

			uint64_t m_lastTime = 0;
			bool m_bMatrixBased = false;
		};


		class CBoxAlgorithmVotingClassifierListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			CBoxAlgorithmVotingClassifierListener()
				: m_oInputTypeID(OV_TypeId_Stimulations) { }

			bool onInputTypeChanged(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				OpenViBE::CIdentifier inputTypeID = OV_UndefinedIdentifier;
				box.getInputType(index, inputTypeID);
				if (inputTypeID == OV_TypeId_Stimulations || inputTypeID == OV_TypeId_StreamedMatrix)
				{
					m_oInputTypeID = inputTypeID;
					for (uint32_t i = 0; i < box.getInputCount(); ++i) { box.setInputType(i, m_oInputTypeID); }
				}
				else { box.setInputType(index, m_oInputTypeID); }
				return true;
			}

			bool onInputAdded(OpenViBE::Kernel::IBox& box, const uint32_t /*index*/) override
			{
				for (uint32_t i = 0; i < box.getInputCount(); ++i)
				{
					box.setInputType(i, m_oInputTypeID);
					box.setInputName(i, ("Classification result " + std::to_string(i)).c_str());
				}
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)

		protected:

			OpenViBE::CIdentifier m_oInputTypeID = OV_UndefinedIdentifier;
		};

		class CBoxAlgorithmVotingClassifierDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Voting Classifier"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Majority voting classifier. Returns the chosen class."); }

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString(
					"Each classifier used as input is assumed to have its own two-class output stream. Mainly designed for P300 scenario use.");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Classification"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_VotingClassifier; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmVotingClassifier; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Classification result 1", OV_TypeId_Stimulations);
				prototype.addInput("Classification result 2", OV_TypeId_Stimulations);
				prototype.addOutput("Classification choice", OV_TypeId_Stimulations);
				prototype.addSetting("Number of repetitions", OV_TypeId_Integer, "12");
				prototype.addSetting("Target class label", OV_TypeId_Stimulation, "OVTK_StimulationId_Target");
				prototype.addSetting("Non target class label", OV_TypeId_Stimulation, "OVTK_StimulationId_NonTarget");
				prototype.addSetting("Reject class label", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
				prototype.addSetting("Result class label base", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_01");
				prototype.addSetting("Choose one if ex-aequo", OV_TypeId_Boolean, "false");
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				return true;
			}

			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmVotingClassifierListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_VotingClassifierDesc)
		};
	} // namespace Classification
} // namespace OpenViBEPlugins
