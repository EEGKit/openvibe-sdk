#include "ovpCAlgorithmOVMatrixFileReader.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace FileIO;

bool CAlgorithmOVMatrixFileReader::initialize()
{
	ip_sFilename.initialize(getInputParameter(OVP_Algorithm_OVMatrixFileReader_InputParameterId_Filename));
	op_pMatrix.initialize(getOutputParameter(OVP_Algorithm_OVMatrixFileReader_OutputParameterId_Matrix));
	return true;
}

bool CAlgorithmOVMatrixFileReader::uninitialize()
{
	op_pMatrix.uninitialize();
	ip_sFilename.uninitialize();
	return true;
}

bool CAlgorithmOVMatrixFileReader::process()
{
	OV_ERROR_UNLESS_KRF(Toolkit::Matrix::loadFromTextFile(*op_pMatrix, ip_sFilename->toASCIIString()),
						"Reading matrix file " << *ip_sFilename << " failed", ErrorType::BadFileRead);

	return true;
}
