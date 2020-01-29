#include "ovpCChannelLocalisationEncoder.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

bool CChannelLocalisationEncoder::initialize()
{
	CStreamedMatrixEncoder::initialize();

	ip_bDynamic.initialize(getInputParameter(OVP_Algorithm_ChannelLocalisationEncoder_InputParameterId_Dynamic));

	return true;
}

bool CChannelLocalisationEncoder::uninitialize()
{
	ip_bDynamic.uninitialize();

	CStreamedMatrixEncoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CChannelLocalisationEncoder::processHeader()
{
	CStreamedMatrixEncoder::processHeader();

	m_writerHelper->openChild(OVTK_NodeId_Header_ChannelLocalisation);
	m_writerHelper->openChild(OVTK_NodeId_Header_ChannelLocalisation_Dynamic);
	m_writerHelper->setUInt(ip_bDynamic ? 1 : 0);
	m_writerHelper->closeChild();
	m_writerHelper->closeChild();

	return true;
}
