#include "ovpCSpectrumDecoder.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::StreamCodecs;

// ________________________________________________________________________________________________________________
//

boolean CSpectrumDecoder::initialize(void)
{
	CStreamedMatrixDecoder::initialize();

	op_pFrequencyAbscissa.initialize(getOutputParameter(OVP_Algorithm_SpectrumStreamDecoder_OutputParameterId_FrequencyAbscissa));
	op_pSamplingRate.initialize(getOutputParameter(OVP_Algorithm_SpectrumStreamDecoder_OutputParameterId_SamplingRate));

	return true;
}

boolean CSpectrumDecoder::uninitialize(void)
{
	op_pFrequencyAbscissa.uninitialize();
	op_pSamplingRate.uninitialize();

	CStreamedMatrixDecoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

EBML::boolean CSpectrumDecoder::isMasterChild(const EBML::CIdentifier& rIdentifier)
{
	if(rIdentifier==OVTK_NodeId_Header_Spectrum)                                     { return true; }
	else if(rIdentifier==OVTK_NodeId_Header_Spectrum_FrequencyBand_Deprecated)       { return true; }
	else if(rIdentifier==OVTK_NodeId_Header_Spectrum_FrequencyBand_Start_Deprecated) { return false; }
	else if(rIdentifier==OVTK_NodeId_Header_Spectrum_FrequencyBand_Stop_Deprecated)  { return false; }
	else if(rIdentifier==OVTK_NodeId_Header_Spectrum_FrequencyAbscissa)            { return false; }
	else if(rIdentifier==OVTK_NodeId_Header_Spectrum_SamplingRate)                   { return false; }
	return CStreamedMatrixDecoder::isMasterChild(rIdentifier);
}

void CSpectrumDecoder::openChild(const EBML::CIdentifier& rIdentifier)
{
	m_vNodes.push(rIdentifier);

	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if(l_rTop==OVTK_NodeId_Header_Spectrum)
	{
		op_pFrequencyAbscissa->setDimensionCount(1);
		op_pFrequencyAbscissa->setDimensionSize(0, op_pMatrix->getDimensionSize(1));
		m_ui32FrequencyBandIndex=0;
	}
	else if(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyAbscissa)
	{
	}
	else
	{
		CStreamedMatrixDecoder::openChild(rIdentifier);
	}
}

void CSpectrumDecoder::processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();
	static double leftFreq;
	if((l_rTop==OVTK_NodeId_Header_Spectrum)
		||(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand_Deprecated))
	{
	}
	else if(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand_Start_Deprecated)
	{
		leftFreq = m_pEBMLReaderHelper->getFloatFromChildData(pBuffer, ui64BufferSize);
	}
	else if(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand_Stop_Deprecated)
	{
		double rightFreq = m_pEBMLReaderHelper->getFloatFromChildData(pBuffer, ui64BufferSize);
		double band = leftFreq + m_ui32FrequencyBandIndex / op_pFrequencyAbscissa->getDimensionSize(0) * (rightFreq - leftFreq);
		op_pFrequencyAbscissa->getBuffer()[m_ui32FrequencyBandIndex] = band;
		auto g = op_pMatrix->getDimensionLabel(1, m_ui32FrequencyBandIndex);
		std::ostringstream s;
		s << band;
		op_pMatrix->setDimensionLabel(1, m_ui32FrequencyBandIndex, s.str().c_str());
		// shorter but produce unwanted 0
		// op_pMatrix->setDimensionLabel(1, m_ui32FrequencyBandIndex, std::to_string(band).c_str());

		// Do we agree on this ?
		op_pSamplingRate = (m_ui32FrequencyBandIndex + 1) * (rightFreq - op_pFrequencyAbscissa->getBuffer()[0]);
	}
	else if(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyAbscissa)
	{
		op_pFrequencyAbscissa->getBuffer()[m_ui32FrequencyBandIndex]=m_pEBMLReaderHelper->getFloatFromChildData(pBuffer, ui64BufferSize);
	}
	else if(l_rTop==OVTK_NodeId_Header_Spectrum_SamplingRate)
	{
		op_pSamplingRate = m_pEBMLReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize);
	}
	else
	{
		CStreamedMatrixDecoder::processChildData(pBuffer, ui64BufferSize);
	}
}

void CSpectrumDecoder::closeChild(void)
{
	EBML::CIdentifier& l_rTop=m_vNodes.top();

	if((l_rTop==OVTK_NodeId_Header_Spectrum)
		||(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand_Start_Deprecated)
		||(l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand_Stop_Deprecated))
	{
	}
	else if((l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyBand_Deprecated)
		|| (l_rTop==OVTK_NodeId_Header_Spectrum_FrequencyAbscissa))
	{
		m_ui32FrequencyBandIndex++;
	}
	else
	{
		CStreamedMatrixDecoder::closeChild();
	}

	m_vNodes.pop();
}
