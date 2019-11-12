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

bool CStimulationDecoder::isMasterChild(const EBML::CIdentifier& identifier)
{
	if (identifier == OVTK_NodeId_Buffer_Stimulation) { return true; }
	if (identifier == OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations) { return false; }
	if (identifier == OVTK_NodeId_Buffer_Stimulation_Stimulation) { return true; }
	if (identifier == OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier) { return false; }
	if (identifier == OVTK_NodeId_Buffer_Stimulation_Stimulation_Date) { return false; }
	if (identifier == OVTK_NodeId_Buffer_Stimulation_Stimulation_Duration) { return false; }
	return CEBMLBaseDecoder::isMasterChild(identifier);
}

void CStimulationDecoder::openChild(const EBML::CIdentifier& identifier)
{
	m_nodes.push(identifier);

	EBML::CIdentifier& top = m_nodes.top();

	if ((top == OVTK_NodeId_Buffer_Stimulation)
		|| (top == OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations)
		|| (top == OVTK_NodeId_Buffer_Stimulation_Stimulation)
		|| (top == OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier)
		|| (top == OVTK_NodeId_Buffer_Stimulation_Stimulation_Date)
		|| (top == OVTK_NodeId_Buffer_Stimulation_Stimulation_Duration)) { }
	else { CEBMLBaseDecoder::openChild(identifier); }
}

void CStimulationDecoder::processChildData(const void* buffer, const uint64_t size)
{
	EBML::CIdentifier& top = m_nodes.top();

	if ((top == OVTK_NodeId_Buffer_Stimulation)
		|| (top == OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations)
		|| (top == OVTK_NodeId_Buffer_Stimulation_Stimulation)
		|| (top == OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier)
		|| (top == OVTK_NodeId_Buffer_Stimulation_Stimulation_Date)
		|| (top == OVTK_NodeId_Buffer_Stimulation_Stimulation_Duration))
	{
		if (top == OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations)
		{
			op_pStimulationSet->setStimulationCount(m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size));
			m_stimulationIdx = 0;
		}
		if (top == OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier)
		{
			op_pStimulationSet->setStimulationIdentifier(m_stimulationIdx, m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size));
		}
		if (top == OVTK_NodeId_Buffer_Stimulation_Stimulation_Date)
		{
			op_pStimulationSet->setStimulationDate(m_stimulationIdx, m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size));
		}
		if (top == OVTK_NodeId_Buffer_Stimulation_Stimulation_Duration)
		{
			op_pStimulationSet->setStimulationDuration(m_stimulationIdx, m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size));
		}
	}
	else { CEBMLBaseDecoder::processChildData(buffer, size); }
}

void CStimulationDecoder::closeChild()
{
	EBML::CIdentifier& top = m_nodes.top();

	if ((top == OVTK_NodeId_Buffer_Stimulation)
		|| (top == OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations)
		|| (top == OVTK_NodeId_Buffer_Stimulation_Stimulation)
		|| (top == OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier)
		|| (top == OVTK_NodeId_Buffer_Stimulation_Stimulation_Date)
		|| (top == OVTK_NodeId_Buffer_Stimulation_Stimulation_Duration))
	{
		if (top == OVTK_NodeId_Buffer_Stimulation_Stimulation) { m_stimulationIdx++; }
	}
	else { CEBMLBaseDecoder::closeChild(); }

	m_nodes.pop();
}
