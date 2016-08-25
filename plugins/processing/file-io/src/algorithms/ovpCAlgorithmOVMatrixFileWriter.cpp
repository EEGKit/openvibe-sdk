#include "ovpCAlgorithmOVMatrixFileWriter.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::FileIO;

boolean CAlgorithmOVMatrixFileWriter::initialize(void)
{
	ip_sFilename.initialize(getInputParameter(OVP_Algorithm_OVMatrixFileWriter_InputParameterId_Filename));
	ip_pMatrix.initialize(getInputParameter(OVP_Algorithm_OVMatrixFileWriter_InputParameterId_Matrix));

	return true;
}

boolean CAlgorithmOVMatrixFileWriter::uninitialize(void)
{
	ip_sFilename.uninitialize();
	ip_pMatrix.uninitialize();

	return true;
}

boolean CAlgorithmOVMatrixFileWriter::process(void)
{
	OV_ERROR_UNLESS_KRF(
		OpenViBEToolkit::Tools::Matrix::saveToTextFile(*ip_pMatrix, ip_sFilename->toASCIIString()),
		"Writing matrix file " << *ip_sFilename << " failed",
		OpenViBE::Kernel::ErrorType::BadFileWrite
	);

	return true;
}
