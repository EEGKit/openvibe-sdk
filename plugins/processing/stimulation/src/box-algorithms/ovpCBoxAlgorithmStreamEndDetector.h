#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

namespace OpenViBE {
namespace Plugins {
namespace Stimulation {
class CBoxAlgorithmStreamEndDetector final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	static CIdentifier inputEBMLId() { return CIdentifier(0x0, 0x1); }
	static CIdentifier outputStimulationsID() { return CIdentifier(0x1, 0x1); }
	static CIdentifier settingStimulationNameID() { return CIdentifier(0x2, 0x1); }
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_StreamEndDetector)

protected:
	Toolkit::TStreamStructureDecoder<CBoxAlgorithmStreamEndDetector> m_decoder;
	Toolkit::TStimulationEncoder<CBoxAlgorithmStreamEndDetector> m_encoder;

	uint64_t m_stimulationID = 0;
	uint64_t m_actionID      = 0;

private:
	enum class EEndState
	{
		WaitingForEnd,
		EndReceived,
		StimulationSent,
		Finished
	};

	uint64_t m_endDate             = 0;
	uint64_t m_currentChunkEndDate = 0;
	uint64_t m_previousTime        = 0;
	size_t m_inputEBMLIdx          = 0;
	size_t m_outputStimulationsIdx = 0;
	bool m_isHeaderSent            = false;
	EEndState m_endState           = EEndState::WaitingForEnd;
};

class CBoxAlgorithmStreamEndDetectorDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }

	CString getName() const override { return "Stream End Detector"; }
	CString getAuthorName() const override { return "Jozef Legeny"; }
	CString getAuthorCompanyName() const override { return "Mensia Technologies"; }
	CString getShortDescription() const override { return "Sends a stimulation upon receiving an End chunk"; }
	CString getDetailedDescription() const override { return "Sends a stimulation upon receiving an End chunk"; }
	CString getCategory() const override { return "Stimulation"; }
	CString getVersion() const override { return "1.0"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "2.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }

	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_StreamEndDetector; }
	IPluginObject* create() override { return new CBoxAlgorithmStreamEndDetector; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("EBML Stream", OV_TypeId_EBMLStream, CBoxAlgorithmStreamEndDetector::inputEBMLId());
		prototype.addOutput("Output Stimulations", OV_TypeId_Stimulations, CBoxAlgorithmStreamEndDetector::outputStimulationsID());
		prototype.addSetting("Stimulation name", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00", false,
							 CBoxAlgorithmStreamEndDetector::settingStimulationNameID());

		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StreamEndDetectorDesc)
};
}  // namespace Stimulation
}  // namespace Plugins
}  // namespace OpenViBE
