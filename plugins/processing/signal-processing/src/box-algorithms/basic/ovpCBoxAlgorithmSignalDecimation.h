#pragma once

#include "defines.hpp"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {
class CBoxAlgorithmSignalDecimation final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_SignalDecimation)

protected:
	size_t m_decimationFactor = 0;
	size_t m_nChannel         = 0;
	size_t m_iSampleIdx       = 0;
	size_t m_iNSamplePerBlock = 0;
	size_t m_iSampling        = 0;
	size_t m_oSampleIdx       = 0;
	size_t m_oNSamplePerBlock = 0;
	size_t m_oSampling        = 0;

	size_t m_nTotalSample    = 0;
	uint64_t m_startTimeBase = 0;
	uint64_t m_lastStartTime = 0;
	uint64_t m_lastEndTime   = 0;

	Kernel::IAlgorithmProxy* m_decoder = nullptr;
	Kernel::TParameterHandler<const IMemoryBuffer*> ip_buffer;
	Kernel::TParameterHandler<CMatrix*> op_pMatrix;
	Kernel::TParameterHandler<uint64_t> op_sampling;

	Kernel::IAlgorithmProxy* m_encoder = nullptr;
	Kernel::TParameterHandler<uint64_t> ip_sampling;
	Kernel::TParameterHandler<CMatrix*> ip_pMatrix;
	Kernel::TParameterHandler<IMemoryBuffer*> op_buffer;
};

class CBoxAlgorithmSignalDecimationDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }

	CString getName() const override { return "Signal Decimation"; }
	CString getAuthorName() const override { return "Yann Renard"; }
	CString getAuthorCompanyName() const override { return "INRIA"; }
	CString getShortDescription() const override { return "Reduces the sampling frequency to a divider of the original sampling frequency"; }

	CString getDetailedDescription() const override
	{
		return "No pre filtering applied - Number of samples per block have to be a multiple of the decimation factor";
	}

	CString getCategory() const override { return "Signal processing/Temporal Filtering"; }
	CString getVersion() const override { return "1.0"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }

	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_SignalDecimation; }
	IPluginObject* create() override { return new CBoxAlgorithmSignalDecimation; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input signal", OV_TypeId_Signal);
		prototype.addOutput("Output signal", OV_TypeId_Signal);
		prototype.addSetting("Decimation factor", OV_TypeId_Integer, "8");
		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SignalDecimationDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
