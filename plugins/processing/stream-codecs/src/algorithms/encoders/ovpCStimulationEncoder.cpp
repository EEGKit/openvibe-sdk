#include "ovpCStimulationEncoder.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Plugins;
using namespace StreamCodecs;

bool CStimulationEncoder::initialize()
{
	CEBMLBaseEncoder::initialize();
	ip_stimSet.initialize(getInputParameter(OVP_Algorithm_StimulationEncoder_InputParameterId_StimulationSet));
	return true;
}

bool CStimulationEncoder::uninitialize()
{
	ip_stimSet.uninitialize();
	CEBMLBaseEncoder::uninitialize();
	return true;
}

// ________________________________________________________________________________________________________________
//

bool CStimulationEncoder::processBuffer()
{
	CStimulationSet& set = *ip_stimSet;

	m_writerHelper->openChild(OVTK_NodeId_Buffer_Stimulation);
	m_writerHelper->openChild(OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations);
	m_writerHelper->setUInt(set.size());
	m_writerHelper->closeChild();
	for(const auto& s : set)
	{
		m_writerHelper->openChild(OVTK_NodeId_Buffer_Stimulation_Stimulation);
		m_writerHelper->openChild(OVTK_NodeId_Buffer_Stimulation_Stimulation_ID);
		m_writerHelper->setUInt(s.m_ID);
		m_writerHelper->closeChild();
		m_writerHelper->openChild(OVTK_NodeId_Buffer_Stimulation_Stimulation_Date);
		m_writerHelper->setUInt(s.m_Date.time());
		m_writerHelper->closeChild();
		m_writerHelper->openChild(OVTK_NodeId_Buffer_Stimulation_Stimulation_Duration);
		m_writerHelper->setUInt(s.m_Duration.time());
		m_writerHelper->closeChild();
		m_writerHelper->closeChild();
	}
	m_writerHelper->closeChild();

	return true;
}
