#include "ovpCStimulationEncoder.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

bool CStimulationEncoder::initialize()
{
	CEBMLBaseEncoder::initialize();
	ip_pStimulationSet.initialize(getInputParameter(OVP_Algorithm_StimulationStreamEncoder_InputParameterId_StimulationSet));
	return true;
}

bool CStimulationEncoder::uninitialize()
{
	ip_pStimulationSet.uninitialize();
	CEBMLBaseEncoder::uninitialize();
	return true;
}

// ________________________________________________________________________________________________________________
//

bool CStimulationEncoder::processBuffer()
{
	IStimulationSet* stimulationSet = ip_pStimulationSet;

	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Buffer_Stimulation);
	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Buffer_Stimulation_NumberOfStimulations);
	m_pEBMLWriterHelper->setUIntegerAsChildData(stimulationSet->getStimulationCount());
	m_pEBMLWriterHelper->closeChild();
	for (uint32_t i = 0; i < stimulationSet->getStimulationCount(); i++)
	{
		m_pEBMLWriterHelper->openChild(OVTK_NodeId_Buffer_Stimulation_Stimulation);
		m_pEBMLWriterHelper->openChild(OVTK_NodeId_Buffer_Stimulation_Stimulation_Identifier);
		m_pEBMLWriterHelper->setUIntegerAsChildData(stimulationSet->getStimulationIdentifier(i));
		m_pEBMLWriterHelper->closeChild();
		m_pEBMLWriterHelper->openChild(OVTK_NodeId_Buffer_Stimulation_Stimulation_Date);
		m_pEBMLWriterHelper->setUIntegerAsChildData(stimulationSet->getStimulationDate(i));
		m_pEBMLWriterHelper->closeChild();
		m_pEBMLWriterHelper->openChild(OVTK_NodeId_Buffer_Stimulation_Stimulation_Duration);
		m_pEBMLWriterHelper->setUIntegerAsChildData(stimulationSet->getStimulationDuration(i));
		m_pEBMLWriterHelper->closeChild();
		m_pEBMLWriterHelper->closeChild();
	}
	m_pEBMLWriterHelper->closeChild();

	return true;
}
