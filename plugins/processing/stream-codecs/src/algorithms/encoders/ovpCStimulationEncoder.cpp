#include "ovpCStimulationEncoder.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
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
	IStimulationSet* stimulationSet = ip_stimSet;

	m_writerHelper->openChild(OVTK_NodeId_Buffer_Stimulation);
	m_writerHelper->openChild(OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations);
	m_writerHelper->setUInt(stimulationSet->getStimulationCount());
	m_writerHelper->closeChild();
	for (size_t i = 0; i < stimulationSet->getStimulationCount(); ++i)
	{
		m_writerHelper->openChild(OVTK_NodeId_Buffer_Stimulation_Stimulation);
		m_writerHelper->openChild(OVTK_NodeId_Buffer_Stimulation_Stimulation_ID);
		m_writerHelper->setUInt(stimulationSet->getStimulationIdentifier(i));
		m_writerHelper->closeChild();
		m_writerHelper->openChild(OVTK_NodeId_Buffer_Stimulation_Stimulation_Date);
		m_writerHelper->setUInt(stimulationSet->getStimulationDate(i));
		m_writerHelper->closeChild();
		m_writerHelper->openChild(OVTK_NodeId_Buffer_Stimulation_Stimulation_Duration);
		m_writerHelper->setUInt(stimulationSet->getStimulationDuration(i));
		m_writerHelper->closeChild();
		m_writerHelper->closeChild();
	}
	m_writerHelper->closeChild();

	return true;
}
