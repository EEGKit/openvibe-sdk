#include "ovpCAcquisitionEncoder.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

bool CAcquisitionEncoder::initialize()
{
	CEBMLBaseEncoder::initialize();

	ip_bufferDuration.initialize(getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_BufferDuration));
	ip_pExperimentInfoStream.initialize(getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_ExperimentInfoStream));
	ip_pSignalStream.initialize(getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_SignalStream));
	ip_pStimulationStream.initialize(getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_StimulationStream));
	ip_pChannelLocalisationStream.initialize(getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_ChannelLocalisationStream));
	ip_pChannelUnitsStream.initialize(getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_ChannelUnitsStream));

	return true;
}

bool CAcquisitionEncoder::uninitialize()
{
	ip_pChannelUnitsStream.uninitialize();
	ip_pChannelLocalisationStream.uninitialize();
	ip_pStimulationStream.uninitialize();
	ip_pSignalStream.uninitialize();
	ip_pExperimentInfoStream.uninitialize();
	ip_bufferDuration.uninitialize();

	CEBMLBaseEncoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CAcquisitionEncoder::processHeader()
{
	m_writerHelper->openChild(OVTK_NodeId_Acquisition_Header_BufferDuration);
	m_writerHelper->setUInt(ip_bufferDuration);
	m_writerHelper->closeChild();
	m_writerHelper->openChild(OVTK_NodeId_Acquisition_Header_ExperimentInfo);
	m_writerHelper->setBinary(ip_pExperimentInfoStream->getDirectPointer(), ip_pExperimentInfoStream->getSize());
	m_writerHelper->closeChild();
	m_writerHelper->openChild(OVTK_NodeId_Acquisition_Header_Signal);
	m_writerHelper->setBinary(ip_pSignalStream->getDirectPointer(), ip_pSignalStream->getSize());
	m_writerHelper->closeChild();
	m_writerHelper->openChild(OVTK_NodeId_Acquisition_Header_Stimulation);
	m_writerHelper->setBinary(ip_pStimulationStream->getDirectPointer(), ip_pStimulationStream->getSize());
	m_writerHelper->closeChild();
	m_writerHelper->openChild(OVTK_NodeId_Acquisition_Header_ChannelLocalisation);
	m_writerHelper->setBinary(ip_pChannelLocalisationStream->getDirectPointer(), ip_pChannelLocalisationStream->getSize());
	m_writerHelper->closeChild();
	m_writerHelper->openChild(OVTK_NodeId_Acquisition_Header_ChannelUnits);
	m_writerHelper->setBinary(ip_pChannelUnitsStream->getDirectPointer(), ip_pChannelUnitsStream->getSize());
	m_writerHelper->closeChild();

	return true;
}

bool CAcquisitionEncoder::processBuffer()
{
	m_writerHelper->openChild(OVTK_NodeId_Acquisition_Buffer_ExperimentInfo);
	m_writerHelper->setBinary(ip_pExperimentInfoStream->getDirectPointer(), ip_pExperimentInfoStream->getSize());
	m_writerHelper->closeChild();
	m_writerHelper->openChild(OVTK_NodeId_Acquisition_Buffer_Signal);
	m_writerHelper->setBinary(ip_pSignalStream->getDirectPointer(), ip_pSignalStream->getSize());
	m_writerHelper->closeChild();
	m_writerHelper->openChild(OVTK_NodeId_Acquisition_Buffer_Stimulation);
	m_writerHelper->setBinary(ip_pStimulationStream->getDirectPointer(), ip_pStimulationStream->getSize());
	m_writerHelper->closeChild();
	m_writerHelper->openChild(OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation);
	m_writerHelper->setBinary(ip_pChannelLocalisationStream->getDirectPointer(), ip_pChannelLocalisationStream->getSize());
	m_writerHelper->closeChild();
	m_writerHelper->openChild(OVTK_NodeId_Acquisition_Buffer_ChannelUnits);
	m_writerHelper->setBinary(ip_pChannelUnitsStream->getDirectPointer(), ip_pChannelUnitsStream->getSize());
	m_writerHelper->closeChild();

	return true;
}
