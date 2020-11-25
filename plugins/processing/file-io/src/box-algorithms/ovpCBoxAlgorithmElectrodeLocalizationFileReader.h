#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

namespace OpenViBE {
namespace Plugins {
namespace FileIO {
class CBoxAlgorithmElectrodeLocalisationFileReader final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	uint64_t getClockFrequency() override;
	bool initialize() override;
	bool uninitialize() override;
	bool processClock(Kernel::CMessageClock& msg) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_ElectrodeLocalisationFileReader)

protected:

	Kernel::IAlgorithmProxy* m_pOVMatrixFileReader                                                = nullptr;
	Toolkit::TChannelLocalisationEncoder<CBoxAlgorithmElectrodeLocalisationFileReader>* m_encoder = nullptr;

	CString m_filename;
	bool m_headerSent = false;
	bool m_bufferSent = false;
};

class CBoxAlgorithmElectrodeLocalisationFileReaderDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return CString("Electrode localisation file reader"); }
	CString getAuthorName() const override { return CString("Vincent Delannoy"); }
	CString getAuthorCompanyName() const override { return CString("INRIA/IRISA"); }

	CString getShortDescription() const override { return CString("Loads files containing the normalized coordinates of an electrode set"); }

	CString getDetailedDescription() const override { return CString(""); }
	CString getCategory() const override { return CString("File reading and writing/OpenViBE"); }
	CString getVersion() const override { return CString("1.0"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_ElectrodeLocalisationFileReader; }
	IPluginObject* create() override { return new CBoxAlgorithmElectrodeLocalisationFileReader; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		// Adds box outputs
		prototype.addOutput("Channel localisation", OV_TypeId_ChannelLocalisation);

		// Adds settings
		prototype.addSetting("Filename", OV_TypeId_Filename, "");

		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ElectrodeLocalisationFileReaderDesc)
};
}  // namespace FileIO
}  // namespace Plugins
}  // namespace OpenViBE
