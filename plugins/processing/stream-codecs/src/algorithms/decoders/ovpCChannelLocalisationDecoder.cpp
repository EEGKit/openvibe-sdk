#include "ovpCChannelLocalisationDecoder.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

// ________________________________________________________________________________________________________________
//

bool CChannelLocalisationDecoder::initialize()
{
	CStreamedMatrixDecoder::initialize();

	op_bDynamic.initialize(getOutputParameter(OVP_Algorithm_ChannelLocalisationStreamDecoder_OutputParameterId_Dynamic));

	return true;
}

bool CChannelLocalisationDecoder::uninitialize()
{
	op_bDynamic.uninitialize();

	CStreamedMatrixDecoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CChannelLocalisationDecoder::isMasterChild(const EBML::CIdentifier& identifier)
{
	if (identifier == OVTK_NodeId_Header_ChannelLocalisation) { return true; }
	if (identifier == OVTK_NodeId_Header_ChannelLocalisation_Dynamic) { return false; }
	return CStreamedMatrixDecoder::isMasterChild(identifier);
}

void CChannelLocalisationDecoder::openChild(const EBML::CIdentifier& identifier)
{
	m_vNodes.push(identifier);

	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Header_ChannelLocalisation) || (l_rTop == OVTK_NodeId_Header_ChannelLocalisation_Dynamic)) { }
	else { CStreamedMatrixDecoder::openChild(identifier); }
}

void CChannelLocalisationDecoder::processChildData(const void* buffer, const uint64_t size)
{
	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Header_ChannelLocalisation)
		|| (l_rTop == OVTK_NodeId_Header_ChannelLocalisation_Dynamic))
	{
		if (l_rTop == OVTK_NodeId_Header_ChannelLocalisation_Dynamic)
		{
			op_bDynamic = (m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size) ? true : false);
		}
	}
	else { CStreamedMatrixDecoder::processChildData(buffer, size); }
}

void CChannelLocalisationDecoder::closeChild()
{
	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Header_ChannelLocalisation)
		|| (l_rTop == OVTK_NodeId_Header_ChannelLocalisation_Dynamic)) { }
	else { CStreamedMatrixDecoder::closeChild(); }

	m_vNodes.pop();
}
