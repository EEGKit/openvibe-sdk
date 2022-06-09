#pragma once

#include "defines.hpp"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {
class CBoxAlgorithmCommonAverageReference final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_CommonAverageReference)

protected:
	Kernel::IAlgorithmProxy* m_decoder = nullptr;
	Kernel::TParameterHandler<const IMemoryBuffer*> ip_buffer;
	Kernel::TParameterHandler<CMatrix*> op_matrix;
	Kernel::TParameterHandler<uint64_t> op_sampling;

	Kernel::IAlgorithmProxy* m_encoder = nullptr;
	Kernel::TParameterHandler<CMatrix*> ip_matrix;
	Kernel::TParameterHandler<uint64_t> ip_sampling;
	Kernel::TParameterHandler<IMemoryBuffer*> op_buffer;

	CMatrix m_oMatrix;
};

class CBoxAlgorithmCommonAverageReferenceDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }

	CString getName() const override { return "Common Average Reference"; }
	CString getAuthorName() const override { return "Yann Renard"; }
	CString getAuthorCompanyName() const override { return "INRIA"; }
	CString getShortDescription() const override { return "Re-reference the signal to common average reference"; }

	CString getDetailedDescription() const override
	{
		return
				"Re-referencing the signal to common average reference consists in subtracting from each sample the average value of the samples of all electrodes at this time";
	}

	CString getCategory() const override { return "Signal processing/Spatial Filtering"; }
	CString getVersion() const override { return "1.0"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.0.0"; }

	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_CommonAverageReference; }
	IPluginObject* create() override { return new CBoxAlgorithmCommonAverageReference; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input signal", OV_TypeId_Signal);
		prototype.addOutput("Output signal", OV_TypeId_Signal);
		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_CommonAverageReferenceDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
