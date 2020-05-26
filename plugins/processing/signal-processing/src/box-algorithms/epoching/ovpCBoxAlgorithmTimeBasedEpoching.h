#pragma once

#include "../../ovp_defines.h"
#include <toolkit/ovtk_all.h>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {

class CBoxAlgorithmTimeBasedEpoching final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_TimeBasedEpoching)

private:

	Toolkit::TSignalDecoder<CBoxAlgorithmTimeBasedEpoching> m_decoder;
	Toolkit::TSignalEncoder<CBoxAlgorithmTimeBasedEpoching> m_encoder;

	double m_duration = 0;
	double m_interval = 0;

	size_t m_sampling             = 0;
	size_t m_oNSample             = 0;
	size_t m_oNSampleBetweenEpoch = 0;
	size_t m_oSampleIdx           = 0;
	size_t m_oChunkIdx            = 0;
	CTime m_lastInputEndTime      = 0;
	CTime m_referenceTime         = 0;
};

class CBoxAlgorithmTimeBasedEpochingDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return CString("Time based epoching"); }
	CString getAuthorName() const override { return CString("Quentin Barthelemy"); }
	CString getAuthorCompanyName() const override { return CString("Mensia Technologies SA"); }

	CString getShortDescription() const override { return CString("Generates signal 'slices' or 'blocks' having a specified duration and interval"); }

	CString getDetailedDescription() const override { return CString("Interval can be used to control the overlap of epochs"); }
	CString getCategory() const override { return CString("Signal processing/Epoching"); }
	CString getVersion() const override { return CString("2.0"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getStockItemName() const override { return CString("gtk-cut"); }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_TimeBasedEpoching; }
	IPluginObject* create() override { return new CBoxAlgorithmTimeBasedEpoching(); }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input signal", OV_TypeId_Signal);
		prototype.addOutput("Epoched signal", OV_TypeId_Signal);
		prototype.addSetting("Epoch duration (in sec)", OV_TypeId_Float, "1");
		prototype.addSetting("Epoch intervals (in sec)", OV_TypeId_Float, "0.5");

		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_TimeBasedEpochingDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
