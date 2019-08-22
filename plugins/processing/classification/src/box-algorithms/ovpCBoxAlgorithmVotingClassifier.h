#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <vector>
#include <map>
#include <cstdio>

#define OVP_ClassId_BoxAlgorithm_VotingClassifier     OpenViBE::CIdentifier(0xFAF62C2B, 0x0B75D1B3)
#define OVP_ClassId_BoxAlgorithm_VotingClassifierDesc OpenViBE::CIdentifier(0x97E3CCC5, 0xAC353ED2)

namespace OpenViBEPlugins
{
	namespace Classification
	{
		class CBoxAlgorithmVotingClassifier : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_VotingClassifier)

		protected:

			uint64_t m_ui64NumberOfRepetitions  = 0;
			uint64_t m_ui64TargetClassLabel     = 0;
			uint64_t m_ui64NonTargetClassLabel  = 0;
			uint64_t m_ui64RejectClassLabel     = 0;
			uint64_t m_ui64ResultClassLabelBase = 0;
			bool m_bChooseOneIfExAequo;

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

			uint64_t m_ui64LastTime = 0;
			bool m_bMatrixBased;
		};


		class CBoxAlgorithmVotingClassifierListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			CBoxAlgorithmVotingClassifierListener()
				: m_oInputTypeIdentifier(OV_TypeId_Stimulations) { }

			bool onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t index) override
			{
				OpenViBE::CIdentifier l_oInputTypeIdentifier = OV_UndefinedIdentifier;
				rBox.getInputType(index, l_oInputTypeIdentifier);
				if (l_oInputTypeIdentifier == OV_TypeId_Stimulations || l_oInputTypeIdentifier == OV_TypeId_StreamedMatrix)
				{
					m_oInputTypeIdentifier = l_oInputTypeIdentifier;
					for (uint32_t i = 0; i < rBox.getInputCount(); i++)
					{
						rBox.setInputType(i, m_oInputTypeIdentifier);
					}
				}
				else
				{
					rBox.setInputType(index, m_oInputTypeIdentifier);
				}
				return true;
			}

			bool onInputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t index) override
			{
				for (uint32_t i = 0; i < rBox.getInputCount(); i++)
				{
					char l_sBuffer[1024];
					sprintf(l_sBuffer, "Classification result %i", i);
					rBox.setInputType(i, m_oInputTypeIdentifier);
					rBox.setInputName(i, l_sBuffer);
				}
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)

		protected:

			OpenViBE::CIdentifier m_oInputTypeIdentifier = OV_UndefinedIdentifier;
		};

		class CBoxAlgorithmVotingClassifierDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Voting Classifier"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Majority voting classifier. Returns the chosen class."); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("Each classifier used as input is assumed to have its own two-class output stream. Mainly designed for P300 scenario use."); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Classification"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_VotingClassifier; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmVotingClassifier; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const override
			{
				rBoxAlgorithmPrototype.addInput("Classification result 1", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInput("Classification result 2", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addOutput("Classification choice", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addSetting("Number of repetitions", OV_TypeId_Integer, "12");
				rBoxAlgorithmPrototype.addSetting("Target class label", OV_TypeId_Stimulation, "OVTK_StimulationId_Target");
				rBoxAlgorithmPrototype.addSetting("Non target class label", OV_TypeId_Stimulation, "OVTK_StimulationId_NonTarget");
				rBoxAlgorithmPrototype.addSetting("Reject class label", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");
				rBoxAlgorithmPrototype.addSetting("Result class label base", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_01");
				rBoxAlgorithmPrototype.addSetting("Choose one if ex-aequo", OV_TypeId_Boolean, "false");
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				return true;
			}

			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmVotingClassifierListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const override { delete pBoxListener; }

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_VotingClassifierDesc)
		};
	} // namespace Classification
} // namespace OpenViBEPlugins
