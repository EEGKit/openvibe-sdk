#include "ovpCSpectrumDecoder.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

// ________________________________________________________________________________________________________________
//

bool CSpectrumDecoder::initialize()
{
	CStreamedMatrixDecoder::initialize();

	op_pFrequencyAbscissa.initialize(getOutputParameter(OVP_Algorithm_SpectrumStreamDecoder_OutputParameterId_FrequencyAbscissa));
	op_pSamplingRate.initialize(getOutputParameter(OVP_Algorithm_SpectrumStreamDecoder_OutputParameterId_SamplingRate));

	return true;
}

bool CSpectrumDecoder::uninitialize()
{
	op_pFrequencyAbscissa.uninitialize();
	op_pSamplingRate.uninitialize();

	CStreamedMatrixDecoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CSpectrumDecoder::isMasterChild(const EBML::CIdentifier& rIdentifier)
{
	if (rIdentifier == OVTK_NodeId_Header_Spectrum) { return true; }
	if (rIdentifier == OVTK_NodeId_Header_Spectrum_FrequencyBand_Deprecated) { return true; }
	if (rIdentifier == OVTK_NodeId_Header_Spectrum_FrequencyBand_Start_Deprecated) { return false; }
	if (rIdentifier == OVTK_NodeId_Header_Spectrum_FrequencyBand_Stop_Deprecated) { return false; }
	if (rIdentifier == OVTK_NodeId_Header_Spectrum_FrequencyAbscissa) { return false; }
	if (rIdentifier == OVTK_NodeId_Header_Spectrum_SamplingRate) { return false; }
	return CStreamedMatrixDecoder::isMasterChild(rIdentifier);
}

void CSpectrumDecoder::openChild(const EBML::CIdentifier& rIdentifier)
{
	m_vNodes.push(rIdentifier);

	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if (l_rTop == OVTK_NodeId_Header_Spectrum)
	{
		op_pFrequencyAbscissa->setDimensionCount(1);
		op_pFrequencyAbscissa->setDimensionSize(0, op_pMatrix->getDimensionSize(1));
		m_ui32FrequencyBandIndex = 0;
	}
	else if (l_rTop == OVTK_NodeId_Header_Spectrum_FrequencyAbscissa) { }
	else
	{
		CStreamedMatrixDecoder::openChild(rIdentifier);
	}
}

void CSpectrumDecoder::processChildData(const void* pBuffer, const uint64_t ui64BufferSize)
{
	EBML::CIdentifier& l_rTop = m_vNodes.top();
	if ((l_rTop == OVTK_NodeId_Header_Spectrum)
		|| (l_rTop == OVTK_NodeId_Header_Spectrum_FrequencyBand_Deprecated)) { }
	else if (l_rTop == OVTK_NodeId_Header_Spectrum_FrequencyBand_Start_Deprecated)
	{
		m_lowerFreq = m_pEBMLReaderHelper->getFloatFromChildData(pBuffer, ui64BufferSize);
	}
	else if (l_rTop == OVTK_NodeId_Header_Spectrum_FrequencyBand_Stop_Deprecated)
	{
		double upperFreq            = m_pEBMLReaderHelper->getFloatFromChildData(pBuffer, ui64BufferSize);
		double curFrequencyAbscissa = 0;
		if (op_pFrequencyAbscissa->getDimensionSize(0) > 1)
		{
			// In the old format, frequencies were separated into bins with lower and upper bounds.
			// These were calculated as lowerFreq = frequencyIndex/frequencyCount, upperFreq = (frequencyIndex + 1)/frequencyCount, with 0 based indexes.
			// This formula reverses the calculation and puts the 'middle' frequency into the right place
			curFrequencyAbscissa = m_lowerFreq + static_cast<double>(m_ui32FrequencyBandIndex) / (op_pFrequencyAbscissa->getDimensionSize(0) - 1) * (upperFreq - m_lowerFreq);
		}
		op_pFrequencyAbscissa->getBuffer()[m_ui32FrequencyBandIndex] = curFrequencyAbscissa;
		std::ostringstream s;
		s << std::setprecision(10);
		s << curFrequencyAbscissa;
		op_pMatrix->setDimensionLabel(1, m_ui32FrequencyBandIndex, s.str().c_str());

		op_pSamplingRate = static_cast<uint64_t>((m_ui32FrequencyBandIndex + 1) * (upperFreq - op_pFrequencyAbscissa->getBuffer()[0]));
	}
	else if (l_rTop == OVTK_NodeId_Header_Spectrum_FrequencyAbscissa)
	{
		op_pFrequencyAbscissa->getBuffer()[m_ui32FrequencyBandIndex] = m_pEBMLReaderHelper->getFloatFromChildData(pBuffer, ui64BufferSize);
	}
	else if (l_rTop == OVTK_NodeId_Header_Spectrum_SamplingRate)
	{
		op_pSamplingRate = m_pEBMLReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize);
	}
	else
	{
		CStreamedMatrixDecoder::processChildData(pBuffer, ui64BufferSize);
	}
}

void CSpectrumDecoder::closeChild()
{
	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Header_Spectrum)
		|| (l_rTop == OVTK_NodeId_Header_Spectrum_FrequencyBand_Start_Deprecated)
		|| (l_rTop == OVTK_NodeId_Header_Spectrum_FrequencyBand_Stop_Deprecated)) { }
	else if ((l_rTop == OVTK_NodeId_Header_Spectrum_FrequencyBand_Deprecated)
			 || (l_rTop == OVTK_NodeId_Header_Spectrum_FrequencyAbscissa))
	{
		m_ui32FrequencyBandIndex++;
	}
	else { CStreamedMatrixDecoder::closeChild(); }

	m_vNodes.pop();
}
