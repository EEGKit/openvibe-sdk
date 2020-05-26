#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>


namespace OpenViBE {
namespace Plugins {
namespace FileIO {

class CAlgorithmOVMatrixFileReader final : public Toolkit::TAlgorithm<IAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TAlgorithm<IAlgorithm>, OVP_ClassId_Algorithm_OVMatrixFileReader)

protected:

	Kernel::TParameterHandler<CString*> ip_sFilename;
	Kernel::TParameterHandler<IMatrix*> op_pMatrix;
};

class CAlgorithmOVMatrixFileReaderDesc final : public IAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return CString("OpenViBE Matrix file reader"); }
	CString getAuthorName() const override { return CString("Vincent Delannoy"); }
	CString getAuthorCompanyName() const override { return CString("INRIA/IRISA"); }
	CString getShortDescription() const override { return CString(""); }
	CString getDetailedDescription() const override { return CString(""); }
	CString getCategory() const override { return CString("File reading and writing"); }
	CString getVersion() const override { return CString("1.1"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_OVMatrixFileReader; }
	IPluginObject* create() override { return new CAlgorithmOVMatrixFileReader; }

	bool getAlgorithmPrototype(Kernel::IAlgorithmProto& prototype) const override
	{
		prototype.addInputParameter(
			OVP_Algorithm_OVMatrixFileReader_InputParameterId_Filename, "Filename", Kernel::ParameterType_String);
		prototype.addOutputParameter(
			OVP_Algorithm_OVMatrixFileReader_OutputParameterId_Matrix, "Matrix", Kernel::ParameterType_Matrix);
		return true;
	}

	_IsDerivedFromClass_Final_(IAlgorithmDesc, OVP_ClassId_Algorithm_OVMatrixFileReaderDesc)
};

}  // namespace FileIO
}  // namespace Plugins
}  // namespace OpenViBE
