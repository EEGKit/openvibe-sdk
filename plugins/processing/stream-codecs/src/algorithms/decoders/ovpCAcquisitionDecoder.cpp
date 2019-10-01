#include "ovpCAcquisitionDecoder.h"

#include <system/ovCMemory.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

CAcquisitionDecoder::CAcquisitionDecoder() {}

// ________________________________________________________________________________________________________________
//

bool CAcquisitionDecoder::initialize()
{
	CEBMLBaseDecoder::initialize();

	op_ui64BufferDuration.initialize(getOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_BufferDuration));
	op_pExperimentInformationStream.initialize(getOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ExperimentInformationStream));
	op_pSignalStream.initialize(getOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_SignalStream));
	op_pStimulationStream.initialize(getOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_StimulationStream));
	op_pChannelLocalisationStream.initialize(getOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ChannelLocalisationStream));
	op_pChannelUnitsStream.initialize(getOutputParameter(OVP_Algorithm_AcquisitionStreamDecoder_OutputParameterId_ChannelUnitsStream));

	return true;
}

bool CAcquisitionDecoder::uninitialize()
{
	op_pChannelUnitsStream.uninitialize();
	op_pChannelLocalisationStream.uninitialize();
	op_pStimulationStream.uninitialize();
	op_pSignalStream.uninitialize();
	op_pExperimentInformationStream.uninitialize();
	op_ui64BufferDuration.uninitialize();

	CEBMLBaseDecoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CAcquisitionDecoder::isMasterChild(const EBML::CIdentifier& identifier)
{
	if (identifier == OVTK_NodeId_Acquisition_Header_BufferDuration) { return false; }
	if (identifier == OVTK_NodeId_Acquisition_Header_ExperimentInformation) { return false; }
	if (identifier == OVTK_NodeId_Acquisition_Header_Signal) { return false; }
	if (identifier == OVTK_NodeId_Acquisition_Header_Stimulation) { return false; }
	if (identifier == OVTK_NodeId_Acquisition_Header_ChannelLocalisation) { return false; }
	if (identifier == OVTK_NodeId_Acquisition_Header_ChannelUnits) { return false; }
	if (identifier == OVTK_NodeId_Acquisition_Buffer_ExperimentInformation) { return false; }
	if (identifier == OVTK_NodeId_Acquisition_Buffer_Signal) { return false; }
	if (identifier == OVTK_NodeId_Acquisition_Buffer_Stimulation) { return false; }
	if (identifier == OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation) { return false; }
	if (identifier == OVTK_NodeId_Acquisition_Buffer_ChannelUnits) { return false; }
	return CEBMLBaseDecoder::isMasterChild(identifier);
}

void CAcquisitionDecoder::openChild(const EBML::CIdentifier& identifier)
{
	m_vNodes.push(identifier);

	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Acquisition_Header_BufferDuration)
		|| (l_rTop == OVTK_NodeId_Acquisition_Header_ExperimentInformation)
		|| (l_rTop == OVTK_NodeId_Acquisition_Header_Signal)
		|| (l_rTop == OVTK_NodeId_Acquisition_Header_Stimulation)
		|| (l_rTop == OVTK_NodeId_Acquisition_Header_ChannelLocalisation)
		|| (l_rTop == OVTK_NodeId_Acquisition_Header_ChannelUnits)
		|| (l_rTop == OVTK_NodeId_Acquisition_Buffer_ExperimentInformation)
		|| (l_rTop == OVTK_NodeId_Acquisition_Buffer_Signal)
		|| (l_rTop == OVTK_NodeId_Acquisition_Buffer_Stimulation)
		|| (l_rTop == OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation)
		|| (l_rTop == OVTK_NodeId_Acquisition_Buffer_ChannelUnits)
	) { }
	else { CEBMLBaseDecoder::openChild(identifier); }
}

void CAcquisitionDecoder::processChildData(const void* buffer, const uint64_t size)
{
	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Acquisition_Header_BufferDuration)
		|| (l_rTop == OVTK_NodeId_Acquisition_Header_ExperimentInformation)
		|| (l_rTop == OVTK_NodeId_Acquisition_Header_Signal)
		|| (l_rTop == OVTK_NodeId_Acquisition_Header_Stimulation)
		|| (l_rTop == OVTK_NodeId_Acquisition_Header_ChannelLocalisation)
		|| (l_rTop == OVTK_NodeId_Acquisition_Header_ChannelUnits)
		|| (l_rTop == OVTK_NodeId_Acquisition_Buffer_ExperimentInformation)
		|| (l_rTop == OVTK_NodeId_Acquisition_Buffer_Signal)
		|| (l_rTop == OVTK_NodeId_Acquisition_Buffer_Stimulation)
		|| (l_rTop == OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation)
		|| (l_rTop == OVTK_NodeId_Acquisition_Buffer_ChannelUnits)
	)
	{
		if (l_rTop == OVTK_NodeId_Acquisition_Header_BufferDuration) { op_ui64BufferDuration = m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size); }
		if (l_rTop == OVTK_NodeId_Acquisition_Header_ExperimentInformation) { this->appendMemoryBuffer(op_pExperimentInformationStream, buffer, size); }
		if (l_rTop == OVTK_NodeId_Acquisition_Header_Signal) { this->appendMemoryBuffer(op_pSignalStream, buffer, size); }
		if (l_rTop == OVTK_NodeId_Acquisition_Header_Stimulation) { this->appendMemoryBuffer(op_pStimulationStream, buffer, size); }
		if (l_rTop == OVTK_NodeId_Acquisition_Header_ChannelLocalisation) { this->appendMemoryBuffer(op_pChannelLocalisationStream, buffer, size); }
		if (l_rTop == OVTK_NodeId_Acquisition_Header_ChannelUnits) { this->appendMemoryBuffer(op_pChannelUnitsStream, buffer, size); }
		if (l_rTop == OVTK_NodeId_Acquisition_Buffer_ExperimentInformation) { this->appendMemoryBuffer(op_pExperimentInformationStream, buffer, size); }
		if (l_rTop == OVTK_NodeId_Acquisition_Buffer_Signal) { this->appendMemoryBuffer(op_pSignalStream, buffer, size); }
		if (l_rTop == OVTK_NodeId_Acquisition_Buffer_Stimulation) { this->appendMemoryBuffer(op_pStimulationStream, buffer, size); }
		if (l_rTop == OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation) { this->appendMemoryBuffer(op_pChannelLocalisationStream, buffer, size); }
		if (l_rTop == OVTK_NodeId_Acquisition_Buffer_ChannelUnits) { this->appendMemoryBuffer(op_pChannelUnitsStream, buffer, size); }
	}
	else { CEBMLBaseDecoder::processChildData(buffer, size); }
}

void CAcquisitionDecoder::closeChild()
{
	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Acquisition_Header_BufferDuration)
		|| (l_rTop == OVTK_NodeId_Acquisition_Header_ExperimentInformation)
		|| (l_rTop == OVTK_NodeId_Acquisition_Header_Signal)
		|| (l_rTop == OVTK_NodeId_Acquisition_Header_Stimulation)
		|| (l_rTop == OVTK_NodeId_Acquisition_Header_ChannelLocalisation)
		|| (l_rTop == OVTK_NodeId_Acquisition_Header_ChannelUnits)
		|| (l_rTop == OVTK_NodeId_Acquisition_Buffer_ExperimentInformation)
		|| (l_rTop == OVTK_NodeId_Acquisition_Buffer_Signal)
		|| (l_rTop == OVTK_NodeId_Acquisition_Buffer_Stimulation)
		|| (l_rTop == OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation)
		|| (l_rTop == OVTK_NodeId_Acquisition_Buffer_ChannelUnits)
	) { }
	else { CEBMLBaseDecoder::closeChild(); }

	m_vNodes.pop();
}

void CAcquisitionDecoder::appendMemoryBuffer(IMemoryBuffer* pMemoryBuffer, const void* buffer, const uint64_t size)
{
	if (pMemoryBuffer)
	{
		const uint64_t currentBufferSize = pMemoryBuffer->getSize();
		pMemoryBuffer->setSize(currentBufferSize + size, false);
		System::Memory::copy(pMemoryBuffer->getDirectPointer() + currentBufferSize, buffer, size);
	}
}
