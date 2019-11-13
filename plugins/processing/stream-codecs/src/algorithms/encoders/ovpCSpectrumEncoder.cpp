#include "ovpCSpectrumEncoder.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

bool CSpectrumEncoder::initialize()
{
	CStreamedMatrixEncoder::initialize();
	ip_pFrequencyAbscissa.initialize(getInputParameter(OVP_Algorithm_SpectrumEncoder_InputParameterId_FrequencyAbscissa));
	ip_pSamplingRate.initialize(getInputParameter(OVP_Algorithm_SpectrumEncoder_InputParameterId_Sampling));
	return true;
}

bool CSpectrumEncoder::uninitialize()
{
	ip_pFrequencyAbscissa.uninitialize();
	ip_pSamplingRate.uninitialize();

	CStreamedMatrixEncoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CSpectrumEncoder::processHeader()
{
	// ip_pFrequencyAbscissa dimension count should be 1
	// ip_pFrequencyAbscissa dimension size 0 should be the same as streamed matrix dimension size 1

	IMatrix* frequencyAbscissa = ip_pFrequencyAbscissa;
	uint64_t sampling         = ip_pSamplingRate;
	CStreamedMatrixEncoder::processHeader();
	m_writerHelper->openChild(OVTK_NodeId_Header_Spectrum);
	m_writerHelper->openChild(OVTK_NodeId_Header_Spectrum_Sampling);
	m_writerHelper->setUInt(sampling);
	m_writerHelper->closeChild();
	for (size_t i = 0; i < frequencyAbscissa->getDimensionSize(0); ++i)
	{
		m_writerHelper->openChild(OVTK_NodeId_Header_Spectrum_FrequencyAbscissa);
		m_writerHelper->setDouble(frequencyAbscissa->getBuffer()[i]);
		m_writerHelper->closeChild();
	}
	m_writerHelper->closeChild();

	return true;
}
