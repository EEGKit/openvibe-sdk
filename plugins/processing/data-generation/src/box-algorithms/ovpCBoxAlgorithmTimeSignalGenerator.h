#pragma once

#include "../ovp_defines.h"

#include <toolkit/ovtk_all.h>


namespace OpenViBE {
namespace Plugins {
namespace DataGeneration {
class CBoxAlgorithmTimeSignalGenerator final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:

	CBoxAlgorithmTimeSignalGenerator();

	void release() override;

	uint64_t getClockFrequency() override { return 128LL << 32; }

	bool initialize() override;
	bool uninitialize() override;

	bool processClock(Kernel::IMessageClock& messageClock) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_TimeSignalGenerator)

protected:

	Toolkit::TSignalEncoder<CBoxAlgorithmTimeSignalGenerator> m_encoder;

	bool m_headerSent              = false;
	size_t m_sampling              = 0;
	size_t m_nGeneratedEpochSample = 0;
	size_t m_nSentSample           = 0;
};

class CBoxAlgorithmTimeSignalGeneratorDesc final : public IBoxAlgorithmDesc
{
public:

	void release() override { }
	CString getName() const override { return CString("Time signal"); }
	CString getAuthorName() const override { return CString("Yann Renard"); }
	CString getAuthorCompanyName() const override { return CString("INRIA/IRISA"); }
	CString getShortDescription() const override { return CString("Simple time signal generator (for use with DSP)"); }
	CString getDetailedDescription() const override { return CString(""); }
	CString getCategory() const override { return CString("Data generation"); }
	CString getVersion() const override { return CString("1.1"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("0.4.0"); }

	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_TimeSignalGenerator; }
	IPluginObject* create() override { return new CBoxAlgorithmTimeSignalGenerator(); }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addOutput("Generated signal", OV_TypeId_Signal);

		prototype.addSetting("Sampling frequency", OV_TypeId_Integer, "512");
		prototype.addSetting("Generated epoch sample count", OV_TypeId_Integer, "32");

		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_TimeSignalGeneratorDesc)
};
}  // namespace DataGeneration
}  // namespace Plugins
}  // namespace OpenViBE
