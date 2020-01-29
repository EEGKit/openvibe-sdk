#include "ovpCSignalEncoder.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

bool CSignalEncoder::initialize()
{
	CStreamedMatrixEncoder::initialize();

	ip_sampling.initialize(getInputParameter(OVP_Algorithm_SignalEncoder_InputParameterId_Sampling));

	return true;
}

bool CSignalEncoder::uninitialize()
{
	ip_sampling.uninitialize();

	CStreamedMatrixEncoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CSignalEncoder::processHeader()
{
	m_writerHelper->openChild(OVTK_NodeId_Header_Signal);
	m_writerHelper->openChild(OVTK_NodeId_Header_Signal_Sampling);
	m_writerHelper->setUInt(ip_sampling);
	m_writerHelper->closeChild();
	m_writerHelper->closeChild();

	CStreamedMatrixEncoder::processHeader();

	return true;
}
