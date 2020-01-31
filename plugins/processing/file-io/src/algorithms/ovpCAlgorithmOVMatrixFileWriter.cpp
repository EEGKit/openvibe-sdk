#include "ovpCAlgorithmOVMatrixFileWriter.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace FileIO;

bool CAlgorithmOVMatrixFileWriter::initialize()
{
	ip_sFilename.initialize(getInputParameter(OVP_Algorithm_OVMatrixFileWriter_InputParameterId_Filename));
	ip_pMatrix.initialize(getInputParameter(OVP_Algorithm_OVMatrixFileWriter_InputParameterId_Matrix));

	return true;
}

bool CAlgorithmOVMatrixFileWriter::uninitialize()
{
	ip_sFilename.uninitialize();
	ip_pMatrix.uninitialize();

	return true;
}

bool CAlgorithmOVMatrixFileWriter::process()
{
	OV_ERROR_UNLESS_KRF(Toolkit::Matrix::saveToTextFile(*ip_pMatrix, ip_sFilename->toASCIIString()),
						"Writing matrix file " << *ip_sFilename << " failed", ErrorType::BadFileWrite);

	return true;
}
