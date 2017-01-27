#include "ovpCSpectrumEncoder.h"

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::StreamCodecs;

boolean CSpectrumEncoder::initialize(void)
{
	CStreamedMatrixEncoder::initialize();
	ip_pCenterFrenquencyBands.initialize(getInputParameter(OVP_Algorithm_SpectrumStreamEncoder_InputParameterId_CenterFrequencyBands));
	ip_pSamplingRate.initialize(getInputParameter(OVP_Algorithm_SpectrumStreamEncoder_InputParameterId_SamplingRate));
	return true;
}

boolean CSpectrumEncoder::uninitialize(void)
{
	ip_pCenterFrenquencyBands.uninitialize();
	ip_pSamplingRate.uninitialize();

	CStreamedMatrixEncoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

boolean CSpectrumEncoder::processHeader(void)
{
	// ip_pCenterFrenquencyBands dimension count should be 1
	// ip_pMinMaxFrenquencyBands dimension size 0 should be the same as streamed matrix dimension size 1

	IMatrix* l_pFrequencyBandCenter = ip_pCenterFrenquencyBands;
	uint64 l_pSamplingRate = ip_pSamplingRate;
	CStreamedMatrixEncoder::processHeader();
	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_Spectrum);
		m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_Spectrum_SamplingRate);
			m_pEBMLWriterHelper->setUIntegerAsChildData(l_pSamplingRate);
		m_pEBMLWriterHelper->closeChild();
	for(uint32 i=0; i<l_pFrequencyBandCenter->getDimensionSize(0); i++)
	{
		m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_Spectrum_CenterFrequencyBand);;
			m_pEBMLWriterHelper->setFloat64AsChildData(l_pFrequencyBandCenter->getBuffer()[i]);
		m_pEBMLWriterHelper->closeChild();
	}
	m_pEBMLWriterHelper->closeChild();

	return true;
}
