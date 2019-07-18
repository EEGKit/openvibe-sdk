#include "ovpCChannelUnitsEncoder.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

bool CChannelUnitsEncoder::initialize(void)
{
	CStreamedMatrixEncoder::initialize();

	ip_bDynamic.initialize(getInputParameter(OVP_Algorithm_ChannelUnitsStreamEncoder_InputParameterId_Dynamic));

	return true;
}

bool CChannelUnitsEncoder::uninitialize(void)
{
	ip_bDynamic.uninitialize();

	CStreamedMatrixEncoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CChannelUnitsEncoder::processHeader(void)
{
	CStreamedMatrixEncoder::processHeader();

	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_ChannelUnits);
	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_ChannelUnits_Dynamic);
	m_pEBMLWriterHelper->setUIntegerAsChildData(ip_bDynamic ? 1 : 0);
	m_pEBMLWriterHelper->closeChild();
	m_pEBMLWriterHelper->closeChild();

	return true;
}
