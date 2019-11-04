#include "ovpCSignalEncoder.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

bool CSignalEncoder::initialize()
{
	CStreamedMatrixEncoder::initialize();

	ip_ui64SamplingRate.initialize(getInputParameter(OVP_Algorithm_SignalStreamEncoder_InputParameterId_Sampling));

	return true;
}

bool CSignalEncoder::uninitialize()
{
	ip_ui64SamplingRate.uninitialize();

	CStreamedMatrixEncoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CSignalEncoder::processHeader()
{
	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_Signal);
	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_Signal_SamplingRate);
	m_pEBMLWriterHelper->setUIntegerAsChildData(ip_ui64SamplingRate);
	m_pEBMLWriterHelper->closeChild();
	m_pEBMLWriterHelper->closeChild();

	CStreamedMatrixEncoder::processHeader();

	return true;
}
