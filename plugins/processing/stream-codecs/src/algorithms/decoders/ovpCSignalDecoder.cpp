#include "ovpCSignalDecoder.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

// ________________________________________________________________________________________________________________
//

bool CSignalDecoder::initialize()
{
	CStreamedMatrixDecoder::initialize();

	op_ui64SamplingRate.initialize(getOutputParameter(OVP_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate));

	return true;
}

bool CSignalDecoder::uninitialize()
{
	op_ui64SamplingRate.uninitialize();

	CStreamedMatrixDecoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CSignalDecoder::isMasterChild(const EBML::CIdentifier& identifier)
{
	if (identifier == OVTK_NodeId_Header_Signal) { return true; }
	if (identifier == OVTK_NodeId_Header_Signal_SamplingRate) { return false; }
	return CStreamedMatrixDecoder::isMasterChild(identifier);
}

void CSignalDecoder::openChild(const EBML::CIdentifier& identifier)
{
	m_vNodes.push(identifier);

	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Header_Signal)
		|| (l_rTop == OVTK_NodeId_Header_Signal_SamplingRate)) { }
	else { CStreamedMatrixDecoder::openChild(identifier); }
}

void CSignalDecoder::processChildData(const void* buffer, const uint64_t size)
{
	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Header_Signal)
		|| (l_rTop == OVTK_NodeId_Header_Signal_SamplingRate))
	{
		if (l_rTop == OVTK_NodeId_Header_Signal_SamplingRate) { op_ui64SamplingRate = m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size); }
	}
	else { CStreamedMatrixDecoder::processChildData(buffer, size); }
}

void CSignalDecoder::closeChild()
{
	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Header_Signal)
		|| (l_rTop == OVTK_NodeId_Header_Signal_SamplingRate)) { }
	else { CStreamedMatrixDecoder::closeChild(); }

	m_vNodes.pop();
}
