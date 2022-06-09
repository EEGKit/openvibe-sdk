#pragma once

#include "defines.hpp"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <string>
#include <vector>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {
class CBoxAlgorithmChannelRename final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_ChannelRename)

protected:
	Toolkit::TGenericDecoder<CBoxAlgorithmChannelRename> m_decoder;
	Toolkit::TGenericEncoder<CBoxAlgorithmChannelRename> m_encoder;

	CIdentifier m_typeID = CIdentifier::undefined();

	Kernel::TParameterHandler<CMatrix*> ip_Matrix;
	Kernel::TParameterHandler<CMatrix*> op_Matrix;

	std::vector<std::string> m_names;
};

class CBoxAlgorithmChannelRenameListener final : public Toolkit::TBoxListener<IBoxListener>
{
public:
	bool onInputTypeChanged(Kernel::IBox& box, const size_t index) override
	{
		CIdentifier typeID = CIdentifier::undefined();
		box.getInputType(index, typeID);
		box.setOutputType(index, typeID);
		return true;
	}

	bool onOutputTypeChanged(Kernel::IBox& box, const size_t index) override
	{
		CIdentifier typeID = CIdentifier::undefined();
		box.getOutputType(index, typeID);
		box.setInputType(index, typeID);
		return true;
	}


	_IsDerivedFromClass_Final_(Toolkit::TBoxListener<IBoxListener>, CIdentifier::undefined())
};

class CBoxAlgorithmChannelRenameDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }

	CString getName() const override { return "Channel Rename"; }
	CString getAuthorName() const override { return "Yann Renard"; }
	CString getAuthorCompanyName() const override { return "INRIA"; }
	CString getShortDescription() const override { return "Renames channels of different types of streamed matrices"; }
	CString getDetailedDescription() const override { return ""; }
	CString getCategory() const override { return "Signal processing/Channels"; }
	CString getVersion() const override { return "1.1"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "1.1.0"; }

	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_ChannelRename; }
	IPluginObject* create() override { return new CBoxAlgorithmChannelRename; }
	IBoxListener* createBoxListener() const override { return new CBoxAlgorithmChannelRenameListener; }
	void releaseBoxListener(IBoxListener* boxListener) const override { delete boxListener; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input matrix", OV_TypeId_Signal);
		prototype.addOutput("Output matrix", OV_TypeId_Signal);
		prototype.addSetting("New channel names", OV_TypeId_String, "Channel 1;Channel 2");
		prototype.addFlag(Kernel::BoxFlag_CanModifyInput);
		prototype.addFlag(Kernel::BoxFlag_CanModifyOutput);

		prototype.addInputSupport(OV_TypeId_Signal);
		prototype.addInputSupport(OV_TypeId_Spectrum);
		prototype.addInputSupport(OV_TypeId_StreamedMatrix);
		prototype.addInputSupport(OV_TypeId_TimeFrequency);
		prototype.addInputSupport(OV_TypeId_CovarianceMatrix);

		prototype.addOutputSupport(OV_TypeId_Signal);
		prototype.addOutputSupport(OV_TypeId_Spectrum);
		prototype.addOutputSupport(OV_TypeId_StreamedMatrix);
		prototype.addOutputSupport(OV_TypeId_TimeFrequency);
		prototype.addOutputSupport(OV_TypeId_CovarianceMatrix);

		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ChannelRenameDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
