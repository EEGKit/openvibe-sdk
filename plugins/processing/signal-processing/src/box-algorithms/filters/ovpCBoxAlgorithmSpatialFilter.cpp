#include "ovpCBoxAlgorithmSpatialFilter.h"

#include <system/ovCMemory.h>

#include <sstream>
#include <string>
#include <cstdio>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

#if defined TARGET_HAS_ThirdPartyEIGEN
#include <Eigen/Dense>
typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> MatrixXdRowMajor;
#endif

uint32_t CBoxAlgorithmSpatialFilter::loadCoefficients(const CString& rCoefficients, const char c1, const char c2, uint32_t nRows, uint32_t nCols)
{
	// Count the number of entries
	// @Note To avoid doing a ton of subsequent memory allocations (very slow on Windows debug builds), we first count the number of entries in the vector. If the file format had specified the vector dimension, we wouldn't have to do this step.
	uint32_t count = 0;
	const char* ptr  = rCoefficients.toASCIIString();
	while (*ptr != 0)
	{
		// Skip separator characters
		while (*ptr == c1 || *ptr == c2)
		{
			ptr++;
		}
		if (*ptr == 0) { break; }
		// Ok, we have reached something that is not NULL or separator, assume its a number
		count++;
		// Skip the normal characters
		while (*ptr != c1 && *ptr != c2 && *ptr != 0)
		{
			ptr++;
		}
	}

	OV_ERROR_UNLESS_KRZ(count == nRows*nCols, "Invalid computed coefficients count [" << count << "] (expected " << nRows * nCols << " coefficients)", OpenViBE::Kernel::ErrorType::BadProcessing);

	// Resize in one step for efficiency.
	m_oFilterBank.setDimensionCount(2);
	m_oFilterBank.setDimensionSize(0, nRows);
	m_oFilterBank.setDimensionSize(1, nCols);

	double* filter = m_oFilterBank.getBuffer();

	// Ok, convert to floats
	ptr          = rCoefficients.toASCIIString();
	uint32_t idx = 0;
	while (*ptr != 0)
	{
		const int BUFFSIZE = 1024;
		char buffer[BUFFSIZE];
		// Skip separator characters
		while (*ptr == c1 || *ptr == c2)
		{
			ptr++;
		}
		if (*ptr == 0) { break; }
		// Copy the normal characters, don't exceed buffer size
		int i = 0;
		while (*ptr != c1 && *ptr != c2 && *ptr != 0)
		{
			if (i < BUFFSIZE - 1)
			{
				buffer[i++] = *ptr;
			}
			else { break; }
			ptr++;
		}
		buffer[i] = 0;

		OV_ERROR_UNLESS_KRZ(idx < count, "Invalid parsed coefficient number [" << idx << "] (expected maximium " << count << " coefficients)", OpenViBE::Kernel::ErrorType::BadProcessing);

		// Finally, convert
		try
		{
			filter[idx] = std::stod(buffer);
		}
		catch (const std::exception&)
		{
			const uint32_t row = idx / nRows + 1;
			const uint32_t col = idx % nRows + 1;

			OV_ERROR_KRZ("Failed to parse coefficient number [" << idx << "] at matrix positions [" << row << "," << col << "]", OpenViBE::Kernel::ErrorType::BadProcessing);
		}

		idx++;
	}

	return idx;
}

bool CBoxAlgorithmSpatialFilter::initialize()
{
	const IBox& boxContext = this->getStaticBoxContext();

	m_pStreamDecoder = nullptr;
	m_pStreamEncoder = nullptr;

	CIdentifier l_oIdentifier;
	boxContext.getInputType(0, l_oIdentifier);

	if (l_oIdentifier == OV_TypeId_StreamedMatrix)
	{
		m_pStreamDecoder = new OpenViBEToolkit::TStreamedMatrixDecoder<CBoxAlgorithmSpatialFilter>(*this, 0);
		m_pStreamEncoder = new OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmSpatialFilter>(*this, 0);
	}
	else if (l_oIdentifier == OV_TypeId_Signal)
	{
		m_pStreamDecoder = new OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmSpatialFilter>(*this, 0);
		m_pStreamEncoder = new OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmSpatialFilter>(*this, 0);

		((OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmSpatialFilter>*)m_pStreamEncoder)->getInputSamplingRate().setReferenceTarget(
			((OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmSpatialFilter>*)m_pStreamDecoder)->getOutputSamplingRate());
	}
	else if (l_oIdentifier == OV_TypeId_Spectrum)
	{
		m_pStreamDecoder = new OpenViBEToolkit::TSpectrumDecoder<CBoxAlgorithmSpatialFilter>(*this, 0);
		m_pStreamEncoder = new OpenViBEToolkit::TSpectrumEncoder<CBoxAlgorithmSpatialFilter>(*this, 0);

		((OpenViBEToolkit::TSpectrumEncoder<CBoxAlgorithmSpatialFilter>*)m_pStreamEncoder)->getInputFrequencyAbscissa().setReferenceTarget(
			((OpenViBEToolkit::TSpectrumDecoder<CBoxAlgorithmSpatialFilter>*)m_pStreamDecoder)->getOutputFrequencyAbscissa());
		((OpenViBEToolkit::TSpectrumEncoder<CBoxAlgorithmSpatialFilter>*)m_pStreamEncoder)->getInputSamplingRate().setReferenceTarget(
			((OpenViBEToolkit::TSpectrumDecoder<CBoxAlgorithmSpatialFilter>*)m_pStreamDecoder)->getOutputSamplingRate());
	}
	else
	{
		OV_ERROR_KRF("Invalid input stream type [" << l_oIdentifier.toString() << "]", OpenViBE::Kernel::ErrorType::BadInput);
	}

	// If we have a filter file, use dimensions and coefficients from that. Otherwise, use box config params.
	const CString filterFile = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);
	if (filterFile != CString(""))
	{
		OV_ERROR_UNLESS_KRF(OpenViBEToolkit::Tools::Matrix::loadFromTextFile(m_oFilterBank, filterFile),
							"Failed to load filter parameters from file at location [" << filterFile << "]",
							OpenViBE::Kernel::ErrorType::BadFileRead);

		OV_ERROR_UNLESS_KRF(m_oFilterBank.getDimensionCount() == 2,
							"Invalid filter matrix in file " << filterFile << ": found [" << m_oFilterBank.getDimensionCount() << "] dimensions (expected 2 dimension)",
							OpenViBE::Kernel::ErrorType::BadConfig);

#if defined(DEBUG)
		OpenViBEToolkit::Tools::Matrix::saveToTextFile(m_oFilterBank, this->getConfigurationManager().expand("${Path_UserData}/spatialfilter_debug.txt"));
#endif
	}
	else
	{
		const CString l_sCoefficient = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
		// The double cast is needed until FSettingValueAutoCast supports uint32_t.
		const uint32_t nOutputChannelSetting = uint32_t(uint64_t(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1)));
		const uint32_t nInputChannelSetting  = uint32_t(uint64_t(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2)));
		const uint32_t nCoefficients         = loadCoefficients(l_sCoefficient, ' ', OV_Value_EnumeratedStringSeparator, nOutputChannelSetting, nInputChannelSetting);

		OV_ERROR_UNLESS_KRF(nCoefficients == nOutputChannelSetting * nInputChannelSetting,
							"Invalid number of coefficients [" << nCoefficients << "] (expected "<< nOutputChannelSetting * nInputChannelSetting <<" coefficients)",
							OpenViBE::Kernel::ErrorType::BadConfig);

#if defined(DEBUG)
		OpenViBEToolkit::Tools::Matrix::saveToTextFile(m_oFilterBank, this->getConfigurationManager().expand("${Path_UserData}/spatialfilter_debug.txt"));
#endif
	}

	return true;
}

bool CBoxAlgorithmSpatialFilter::uninitialize()
{
	if (m_pStreamDecoder)
	{
		m_pStreamDecoder->uninitialize();
		delete m_pStreamDecoder;
		m_pStreamDecoder = nullptr;
	}

	if (m_pStreamEncoder)
	{
		m_pStreamEncoder->uninitialize();
		delete m_pStreamEncoder;
		m_pStreamEncoder = nullptr;
	}

	return true;
}

bool CBoxAlgorithmSpatialFilter::processInput(const uint32_t ui32InputIndex)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmSpatialFilter::process()
{
	// IBox& boxContext=this->getStaticBoxContext();
	IBoxIO& boxContext = this->getDynamicBoxContext();

	for (uint32_t i = 0; i < boxContext.getInputChunkCount(0); i++)
	{
		m_pStreamDecoder->decode(i);
		if (m_pStreamDecoder->isHeaderReceived())
		{
			// we can treat them all as matrix decoders as they all inherit from it
			const IMatrix* iMatrix = (static_cast<OpenViBEToolkit::TStreamedMatrixDecoder<CBoxAlgorithmSpatialFilter>*>(m_pStreamDecoder))->getOutputMatrix();

			const uint32_t nChannelIn = iMatrix->getDimensionSize(0);
			const uint32_t nSampleIn = iMatrix->getDimensionSize(1);

			OV_ERROR_UNLESS_KRF(nChannelIn != 0 && nSampleIn != 0,
								"Invalid matrix size with zero dimension on input [" << nChannelIn << " x " << nSampleIn << "]",
								OpenViBE::Kernel::ErrorType::BadConfig);

			const uint32_t nChannelFilterIn  = m_oFilterBank.getDimensionSize(1);
			const uint32_t nChannelFilterOut = m_oFilterBank.getDimensionSize(0);

			OV_ERROR_UNLESS_KRF(nChannelIn == nChannelFilterIn,
								"Invalid input channel count  [" << nChannelIn << "] (expected " << nChannelFilterIn << " channel count)",
								OpenViBE::Kernel::ErrorType::BadConfig);

			IMatrix* oMatrix = ((OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmSpatialFilter>*)m_pStreamEncoder)->getInputMatrix();
			oMatrix->setDimensionCount(2);
			oMatrix->setDimensionSize(0, nChannelFilterOut);
			oMatrix->setDimensionSize(1, nSampleIn);

			// Name channels
			for (uint32_t j = 0; j < oMatrix->getDimensionSize(0); ++j)
			{
				char buffer[64];
				sprintf(buffer, "sFiltered %d", j);
				oMatrix->setDimensionLabel(0, j, buffer);
			}

			m_pStreamEncoder->encodeHeader();
		}
		if (m_pStreamDecoder->isBufferReceived())
		{
			const IMatrix* iMatrix = ((OpenViBEToolkit::TStreamedMatrixDecoder<CBoxAlgorithmSpatialFilter>*)m_pStreamDecoder)->getOutputMatrix();
			IMatrix* oMatrix      = ((OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmSpatialFilter>*)m_pStreamEncoder)->getInputMatrix();

			const double* in           = iMatrix->getBuffer();
			double* out                = oMatrix->getBuffer();
			const uint32_t nChannelIn  = iMatrix->getDimensionSize(0);
			const uint32_t nChannelOut = oMatrix->getDimensionSize(0);
			const uint32_t nSample     = iMatrix->getDimensionSize(1);

#if defined TARGET_HAS_ThirdPartyEIGEN
			//@TODO check this part we only create matrix ?
			const Eigen::Map<MatrixXdRowMajor> inMapper(const_cast<double*>(in), nChannelIn, nSample);
			const Eigen::Map<MatrixXdRowMajor> filterMapper(const_cast<double*>(m_oFilterBank.getBuffer()), m_oFilterBank.getDimensionSize(0), m_oFilterBank.getDimensionSize(1));
			Eigen::Map<MatrixXdRowMajor> outMapper(out, nChannelOut, nSample);
			outMapper = filterMapper * inMapper;
#else
			const double* filter = m_oFilterBank.getBuffer();

			System::Memory::set(out, nSample*nChannelOut*sizeof(double), 0);

			for(uint32_t j = 0; j < nChannelOut; j++)
			{
				for(uint32_t k = 0; k < nChannelIn; k++)
				{
					for(uint32_t l = 0; l < nSample; l++) { out[j*nSample+l] += filter[j * nChannelIn + k] * in[k * nSample + l]; }
				}
			}
#endif
			m_pStreamEncoder->encodeBuffer();
		}
		if (m_pStreamDecoder->isEndReceived()) { m_pStreamEncoder->encodeEnd(); }

		boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
	}

	return true;
}
