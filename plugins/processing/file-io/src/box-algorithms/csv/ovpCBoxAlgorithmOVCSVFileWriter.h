#pragma once

#include "../../ovp_defines.h"
#include <cstdio>
#include <memory>

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include "csv/ovICSV.h"

namespace OpenViBE {
namespace Plugins {
namespace FileIO {
class CBoxAlgorithmOVCSVFileWriter final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:

	CBoxAlgorithmOVCSVFileWriter();
	void release() override { delete this; }

	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_OVCSVFileWriter)

private:
	bool processStreamedMatrix();
	bool processStimulation();

	std::unique_ptr<CSV::ICSVHandler, decltype(&CSV::releaseCSVHandler)> m_writerLib;

	CIdentifier m_typeID = OV_UndefinedIdentifier;

	Toolkit::TGenericDecoder<CBoxAlgorithmOVCSVFileWriter> m_streamDecoder;
	Toolkit::TStimulationDecoder<CBoxAlgorithmOVCSVFileWriter> m_stimDecoder;

	uint64_t m_epoch = 0;

	bool m_isHeaderReceived = false;
	bool m_isFileOpen       = false;
	bool m_appendData       = false;
	bool m_lastMatrixOnly   = false;
	bool m_writeHeader      = true;
};

class CBoxAlgorithmOVCSVFileWriterListener final : public Toolkit::TBoxListener<IBoxListener>
{
public:
	bool onInputTypeChanged(Kernel::IBox& box, const size_t index) override
	{
		if (index == 1)
		{
			CIdentifier typeID = OV_UndefinedIdentifier;
			box.getInputType(1, typeID);
			if (typeID != OV_TypeId_Stimulations)
			{
				box.setInputType(1, OV_TypeId_Stimulations);
				return true;
			}
		}

		return true;
	}

	_IsDerivedFromClass_Final_(Toolkit::TBoxListener<IBoxListener>, OV_UndefinedIdentifier)
};

class CBoxAlgorithmOVCSVFileWriterDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return "CSV File Writer"; }
	CString getAuthorName() const override { return "Victor Herlin"; }
	CString getAuthorCompanyName() const override { return "Mensia Technologies SA"; }
	CString getShortDescription() const override { return "Writes signal in a CSV (text based) file"; }
	CString getDetailedDescription() const override { return ""; }
	CString getCategory() const override { return "File reading and writing/CSV"; }
	CString getVersion() const override { return "1.0"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.1.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.1.0"; }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_OVCSVFileWriter; }
	IPluginObject* create() override { return new CBoxAlgorithmOVCSVFileWriter; }
	IBoxListener* createBoxListener() const override { return new CBoxAlgorithmOVCSVFileWriterListener; }
	void releaseBoxListener(IBoxListener* boxListener) const override { delete boxListener; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input stream", OV_TypeId_Signal);
		prototype.addInput("Stimulations stream", OV_TypeId_Stimulations);
		prototype.addSetting("Filename", OV_TypeId_Filename, "record-[$core{date}-$core{time}].csv");
		prototype.addSetting("Precision", OV_TypeId_Integer, "10");
		prototype.addSetting("Append data", OV_TypeId_Boolean, "false");
		prototype.addSetting("Only last matrix", OV_TypeId_Boolean, "false");
		prototype.addFlag(Kernel::BoxFlag_CanModifyInput);

		prototype.addInputSupport(OV_TypeId_Signal);
		prototype.addInputSupport(OV_TypeId_StreamedMatrix);
		prototype.addInputSupport(OV_TypeId_Spectrum);
		prototype.addInputSupport(OV_TypeId_FeatureVector);
		prototype.addInputSupport(OV_TypeId_CovarianceMatrix);

		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_OVCSVFileWriterDesc)
};
}  // namespace FileIO
}  // namespace Plugins
}  // namespace OpenViBE
