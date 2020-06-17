#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.hpp>
#include <toolkit/ovtk_all.h>
#include <vector>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {
class CBoxAlgorithmSpectrumAverage final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }

	// virtual uint64_t getClockFrequency();
	bool initialize() override;
	bool uninitialize() override;
	// virtual bool processEvent(CMessage& msg);
	// virtual bool processSignal(CMessage& msg);
	// virtual bool processClock(CMessage& msg);
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_SpectrumAverage)

protected:

	bool m_bZeroCare = false;

	Kernel::IAlgorithmProxy* m_decoder = nullptr;
	Kernel::IAlgorithmProxy* m_encoder = nullptr;

	Kernel::TParameterHandler<IMatrix*> ip_matrix;
	Kernel::TParameterHandler<IMatrix*> op_matrix;

	Kernel::TParameterHandler<const IMemoryBuffer*> ip_buffer;
	Kernel::TParameterHandler<IMemoryBuffer*> op_buffer;

	std::vector<size_t> m_selectedIndices;
};

class CBoxAlgorithmSpectrumAverageDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return "Spectrum Average"; }
	CString getAuthorName() const override { return "Yann Renard"; }
	CString getAuthorCompanyName() const override { return "INRIA"; }

	CString getShortDescription() const override { return "Computes the average of all the frequency band powers for a spectrum"; }

	CString getDetailedDescription() const override { return ""; }
	CString getCategory() const override { return "Signal processing/Spectral Analysis"; }
	CString getVersion() const override { return "1.0"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_SpectrumAverage; }
	IPluginObject* create() override { return new CBoxAlgorithmSpectrumAverage; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Spectrum", OV_TypeId_Spectrum);
		prototype.addOutput("Spectrum average", OV_TypeId_StreamedMatrix);
		prototype.addSetting("Considers zeros", OV_TypeId_Boolean, "false");

		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SpectrumAverageDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
