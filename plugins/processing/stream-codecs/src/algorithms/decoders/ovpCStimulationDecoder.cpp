#include "ovpCStimulationDecoder.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

// ________________________________________________________________________________________________________________
//

bool CStimulationDecoder::initialize()
{
	CEBMLBaseDecoder::initialize();

	op_pStimulationSet.initialize(getOutputParameter(OVP_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet));

	return true;
}

bool CStimulationDecoder::uninitialize()
{
	op_pStimulationSet.uninitialize();

	CEBMLBaseDecoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CStimulationDecoder::isMasterChild(const EBML::CIdentifier& rIdentifier)
{
	if (rIdentifier == OVTK_NodeId_Buffer_Stimulation) { return true; }
	if (rIdentifier == OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations) { return false; }
	if (rIdentifier == OVTK_NodeId_Buffer_Stimulation_Stimulation) { return true; }
	if (rIdentifier == OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier) { return false; }
	if (rIdentifier == OVTK_NodeId_Buffer_Stimulation_Stimulation_Date) { return false; }
	if (rIdentifier == OVTK_NodeId_Buffer_Stimulation_Stimulation_Duration) { return false; }
	return CEBMLBaseDecoder::isMasterChild(rIdentifier);
}

void CStimulationDecoder::openChild(const EBML::CIdentifier& rIdentifier)
{
	m_vNodes.push(rIdentifier);

	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Buffer_Stimulation)
		|| (l_rTop == OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations)
		|| (l_rTop == OVTK_NodeId_Buffer_Stimulation_Stimulation)
		|| (l_rTop == OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier)
		|| (l_rTop == OVTK_NodeId_Buffer_Stimulation_Stimulation_Date)
		|| (l_rTop == OVTK_NodeId_Buffer_Stimulation_Stimulation_Duration)) { }
	else { CEBMLBaseDecoder::openChild(rIdentifier); }
}

void CStimulationDecoder::processChildData(const void* buffer, const uint64_t size)
{
	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Buffer_Stimulation)
		|| (l_rTop == OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations)
		|| (l_rTop == OVTK_NodeId_Buffer_Stimulation_Stimulation)
		|| (l_rTop == OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier)
		|| (l_rTop == OVTK_NodeId_Buffer_Stimulation_Stimulation_Date)
		|| (l_rTop == OVTK_NodeId_Buffer_Stimulation_Stimulation_Duration))
	{
		if (l_rTop == OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations)
		{
			op_pStimulationSet->setStimulationCount(m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size));
			m_ui64StimulationIndex = 0;
		}
		if (l_rTop == OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier) { op_pStimulationSet->setStimulationIdentifier(m_ui64StimulationIndex, m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size)); }
		if (l_rTop == OVTK_NodeId_Buffer_Stimulation_Stimulation_Date) { op_pStimulationSet->setStimulationDate(m_ui64StimulationIndex, m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size)); }
		if (l_rTop == OVTK_NodeId_Buffer_Stimulation_Stimulation_Duration) { op_pStimulationSet->setStimulationDuration(m_ui64StimulationIndex, m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size)); }
	}
	else
	{
		CEBMLBaseDecoder::processChildData(buffer, size);
	}
}

void CStimulationDecoder::closeChild()
{
	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Buffer_Stimulation)
		|| (l_rTop == OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations)
		|| (l_rTop == OVTK_NodeId_Buffer_Stimulation_Stimulation)
		|| (l_rTop == OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier)
		|| (l_rTop == OVTK_NodeId_Buffer_Stimulation_Stimulation_Date)
		|| (l_rTop == OVTK_NodeId_Buffer_Stimulation_Stimulation_Duration))
	{
		if (l_rTop == OVTK_NodeId_Buffer_Stimulation_Stimulation) { m_ui64StimulationIndex++; }
	}
	else { CEBMLBaseDecoder::closeChild(); }

	m_vNodes.pop();
}
