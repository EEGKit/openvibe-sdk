#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <vector>
#include <map>

namespace OpenViBE {
namespace Plugins {
namespace Classification {

class CBoxAlgorithmVotingClassifier final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_VotingClassifier)

protected:

	size_t m_nRepetitions         = 0;
	size_t m_targetClassLabel     = 0;
	size_t m_nonTargetClassLabel  = 0;
	size_t m_rejectClassLabel     = 0;
	size_t m_resultClassLabelBase = 0;
	bool m_chooseOneIfExAequo     = false;

private:

	typedef struct
	{
		Toolkit::TDecoder<CBoxAlgorithmVotingClassifier>* decoder = nullptr;
		Kernel::TParameterHandler<CStimulationSet*> op_stimSet;
		Kernel::TParameterHandler<IMatrix*> op_matrix;
		bool twoValueInput;
		std::vector<std::pair<double, CTime>> scores;
	} input_t;

	std::map<size_t, input_t> m_results;

	Toolkit::TStimulationEncoder<CBoxAlgorithmVotingClassifier> m_classificationChoiceEncoder;
	Kernel::TParameterHandler<const CStimulationSet*> ip_classificationChoiceStimSet;

	CTime m_lastTime   = 0;
	bool m_matrixBased = false;
};


class CBoxAlgorithmVotingClassifierListener final : public Toolkit::TBoxListener<IBoxListener>
{
public:

	CBoxAlgorithmVotingClassifierListener() : m_inputTypeID(OV_TypeId_Stimulations) { }

	bool onInputTypeChanged(Kernel::IBox& box, const size_t index) override
	{
		CIdentifier id = OV_UndefinedIdentifier;
		box.getInputType(index, id);
		if (id == OV_TypeId_Stimulations || id == OV_TypeId_StreamedMatrix)
		{
			m_inputTypeID = id;
			for (size_t i = 0; i < box.getInputCount(); ++i) { box.setInputType(i, m_inputTypeID); }
		}
		else { box.setInputType(index, m_inputTypeID); }
		return true;
	}

	bool onInputAdded(Kernel::IBox& box, const size_t /*index*/) override
	{
		for (size_t i = 0; i < box.getInputCount(); ++i)
		{
			box.setInputType(i, m_inputTypeID);
			box.setInputName(i, ("Classification result " + std::to_string(i)).c_str());
		}
		return true;
	}

	_IsDerivedFromClass_Final_(Toolkit::TBoxListener<IBoxListener>, OV_UndefinedIdentifier)

protected:

	CIdentifier m_inputTypeID = OV_UndefinedIdentifier;
};

class CBoxAlgorithmVotingClassifierDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return "Voting Classifier"; }
	CString getAuthorName() const override { return "Yann Renard"; }
	CString getAuthorCompanyName() const override { return "INRIA"; }
	CString getShortDescription() const override { return "Majority voting classifier. Returns the chosen class."; }

	CString getDetailedDescription() const override
	{
		return "Each classifier used as input is assumed to have its own two-class output stream. Mainly designed for P300 scenario use.";
	}

	CString getCategory() const override { return CString("Classification"); }
	CString getVersion() const override { return CString("1.0"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_VotingClassifier; }
	IPluginObject* create() override { return new CBoxAlgorithmVotingClassifier; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
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
		prototype.addFlag(Kernel::BoxFlag_CanAddInput);
		prototype.addFlag(Kernel::BoxFlag_CanModifyInput);
		return true;
	}

	IBoxListener* createBoxListener() const override { return new CBoxAlgorithmVotingClassifierListener; }
	void releaseBoxListener(IBoxListener* listener) const override { delete listener; }

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_VotingClassifierDesc)
};

} // namespace Classification
} // namespace Plugins
} // namespace OpenViBE
