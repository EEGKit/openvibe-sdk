#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <string>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {
class CBoxAlgorithmChannelSelector final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_ChannelSelector)

protected:

	Toolkit::TDecoder<CBoxAlgorithmChannelSelector>* m_decoder = nullptr;
	Toolkit::TEncoder<CBoxAlgorithmChannelSelector>* m_encoder = nullptr;

	CMatrix* m_iMatrix = nullptr;
	CMatrix* m_oMatrix = nullptr;

	std::vector<size_t> m_vLookup;
};

class CBoxAlgorithmChannelSelectorListener final : public Toolkit::TBoxListener<IBoxListener>
{
public:
	bool onOutputTypeChanged(Kernel::IBox& box, const size_t /*index*/) override
	{
		CIdentifier typeID = OV_UndefinedIdentifier;
		box.getOutputType(0, typeID);
		if (typeID == OV_TypeId_Signal || typeID == OV_TypeId_Spectrum || typeID == OV_TypeId_StreamedMatrix)
		{
			box.setInputType(0, typeID);
			return true;
		}
		box.getInputType(0, typeID);
		box.setOutputType(0, typeID);
		OV_ERROR_KRF("Invalid output type [" << typeID.str() << "] (expected Signal, Spectrum or Streamed Matrix)", Kernel::ErrorType::BadOutput);
	}

	bool onInputTypeChanged(Kernel::IBox& box, const size_t /*index*/) override
	{
		CIdentifier typeID = OV_UndefinedIdentifier;
		box.getInputType(0, typeID);
		if (typeID == OV_TypeId_Signal || typeID == OV_TypeId_Spectrum || typeID == OV_TypeId_StreamedMatrix)
		{
			box.setOutputType(0, typeID);
			return true;
		}
		box.getOutputType(0, typeID);
		box.setInputType(0, typeID);

		OV_ERROR_KRF("Invalid input type [" << typeID.str() << "] (expected Signal, Spectrum or Streamed Matrix)", Kernel::ErrorType::BadInput);
	}

	bool onSettingValueChanged(Kernel::IBox& box, const size_t index) override
	{
		//we are only interested in the setting 0 and the type changes (select or reject)
		if ((index == 0 || index == 1) && (!m_hasUserSetName))
		{
			CString channels;
			box.getSettingValue(0, channels);

			CString method;
			CIdentifier enumID = OV_UndefinedIdentifier;
			box.getSettingValue(1, method);
			box.getSettingType(1, enumID);

			const ESelectionMethod methodID = ESelectionMethod(this->getTypeManager().getEnumerationEntryValueFromName(enumID, method));

			if (methodID == ESelectionMethod::Reject) { channels = CString("!") + channels; }
			box.setName(channels);
		}
		return true;
	}

	bool onNameChanged(Kernel::IBox& box) override
	//when user set box name manually
	{
		if (m_hasUserSetName)
		{
			const CString rename = box.getName();
			if (rename == CString("Channel Selector"))
			{//default name, we switch back to default behaviour
				m_hasUserSetName = false;
			}
		}
		else { m_hasUserSetName = true; }
		return true;
	}

	bool initialize() override
	{
		m_hasUserSetName = false;//need to initialize this value
		return true;
	}

private:
	bool m_hasUserSetName = false;

	_IsDerivedFromClass_Final_(Toolkit::TBoxListener<IBoxListener>, OV_UndefinedIdentifier)
};

class CBoxAlgorithmChannelSelectorDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return CString("Channel Selector"); }
	CString getAuthorName() const override { return CString("Yann Renard"); }
	CString getAuthorCompanyName() const override { return CString("INRIA"); }
	CString getShortDescription() const override { return CString("Select a subset of signal channels"); }

	CString getDetailedDescription() const override { return CString("Selection can be based on channel name (case-sensitive) or index starting from 0"); }

	CString getCategory() const override { return CString("Signal processing/Channels"); }
	CString getVersion() const override { return CString("1.0"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_ChannelSelector; }
	IPluginObject* create() override { return new CBoxAlgorithmChannelSelector; }
	IBoxListener* createBoxListener() const override { return new CBoxAlgorithmChannelSelectorListener; }
	void releaseBoxListener(IBoxListener* listener) const override { delete listener; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input signal", OV_TypeId_Signal);
		prototype.addOutput("Output signal", OV_TypeId_Signal);
		prototype.addSetting("Channel List", OV_TypeId_String, ":");
		prototype.addSetting("Action", OVP_TypeId_SelectionMethod, "Select");
		prototype.addSetting("Channel Matching Method", OVP_TypeId_MatchMethod, "Smart");

		prototype.addFlag(Kernel::BoxFlag_CanModifyInput);
		prototype.addFlag(Kernel::BoxFlag_CanModifyOutput);

		prototype.addInputSupport(OV_TypeId_Signal);
		prototype.addInputSupport(OV_TypeId_Spectrum);
		prototype.addInputSupport(OV_TypeId_StreamedMatrix);

		prototype.addOutputSupport(OV_TypeId_Signal);
		prototype.addOutputSupport(OV_TypeId_Spectrum);
		prototype.addOutputSupport(OV_TypeId_StreamedMatrix);
		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ChannelSelectorDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
