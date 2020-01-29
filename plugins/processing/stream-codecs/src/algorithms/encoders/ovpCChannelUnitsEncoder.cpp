#include "ovpCChannelUnitsEncoder.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

bool CChannelUnitsEncoder::initialize()
{
	CStreamedMatrixEncoder::initialize();

	ip_bDynamic.initialize(getInputParameter(OVP_Algorithm_ChannelUnitsEncoder_InputParameterId_Dynamic));

	return true;
}

bool CChannelUnitsEncoder::uninitialize()
{
	ip_bDynamic.uninitialize();

	CStreamedMatrixEncoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CChannelUnitsEncoder::processHeader()
{
	CStreamedMatrixEncoder::processHeader();

	m_writerHelper->openChild(OVTK_NodeId_Header_ChannelUnits);
	m_writerHelper->openChild(OVTK_NodeId_Header_ChannelUnits_Dynamic);
	m_writerHelper->setUInt(ip_bDynamic ? 1 : 0);
	m_writerHelper->closeChild();
	m_writerHelper->closeChild();

	return true;
}
