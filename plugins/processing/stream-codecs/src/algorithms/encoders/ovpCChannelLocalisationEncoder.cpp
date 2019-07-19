#include "ovpCChannelLocalisationEncoder.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

bool CChannelLocalisationEncoder::initialize()
{
	CStreamedMatrixEncoder::initialize();

	ip_bDynamic.initialize(getInputParameter(OVP_Algorithm_ChannelLocalisationStreamEncoder_InputParameterId_Dynamic));

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

	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_ChannelLocalisation);
	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_ChannelLocalisation_Dynamic);
	m_pEBMLWriterHelper->setUIntegerAsChildData(ip_bDynamic ? 1 : 0);
	m_pEBMLWriterHelper->closeChild();
	m_pEBMLWriterHelper->closeChild();

	return true;
}
