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

	op_ui64BufferDuration.initialize(getOutputParameter(OVP_Algorithm_AcquisitionDecoder_OutputParameterId_BufferDuration));
	op_pExperimentInfoStream.initialize(getOutputParameter(OVP_Algorithm_AcquisitionDecoder_OutputParameterId_ExperimentInfoStream));
	op_pSignalStream.initialize(getOutputParameter(OVP_Algorithm_AcquisitionDecoder_OutputParameterId_SignalStream));
	op_pStimulationStream.initialize(getOutputParameter(OVP_Algorithm_AcquisitionDecoder_OutputParameterId_StimulationStream));
	op_pChannelLocalisationStream.initialize(getOutputParameter(OVP_Algorithm_AcquisitionDecoder_OutputParameterId_ChannelLocalisationStream));
	op_pChannelUnitsStream.initialize(getOutputParameter(OVP_Algorithm_AcquisitionDecoder_OutputParameterId_ChannelUnitsStream));

	return true;
}

bool CAcquisitionDecoder::uninitialize()
{
	op_pChannelUnitsStream.uninitialize();
	op_pChannelLocalisationStream.uninitialize();
	op_pStimulationStream.uninitialize();
	op_pSignalStream.uninitialize();
	op_pExperimentInfoStream.uninitialize();
	op_ui64BufferDuration.uninitialize();

	CEBMLBaseDecoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CAcquisitionDecoder::isMasterChild(const EBML::CIdentifier& identifier)
{
	if (identifier == OVTK_NodeId_Acquisition_Header_BufferDuration) { return false; }
	if (identifier == OVTK_NodeId_Acquisition_Header_ExperimentInfo) { return false; }
	if (identifier == OVTK_NodeId_Acquisition_Header_Signal) { return false; }
	if (identifier == OVTK_NodeId_Acquisition_Header_Stimulation) { return false; }
	if (identifier == OVTK_NodeId_Acquisition_Header_ChannelLocalisation) { return false; }
	if (identifier == OVTK_NodeId_Acquisition_Header_ChannelUnits) { return false; }
	if (identifier == OVTK_NodeId_Acquisition_Buffer_ExperimentInfo) { return false; }
	if (identifier == OVTK_NodeId_Acquisition_Buffer_Signal) { return false; }
	if (identifier == OVTK_NodeId_Acquisition_Buffer_Stimulation) { return false; }
	if (identifier == OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation) { return false; }
	if (identifier == OVTK_NodeId_Acquisition_Buffer_ChannelUnits) { return false; }
	return CEBMLBaseDecoder::isMasterChild(identifier);
}

void CAcquisitionDecoder::openChild(const EBML::CIdentifier& identifier)
{
	m_nodes.push(identifier);

	EBML::CIdentifier& top = m_nodes.top();

	if ((top == OVTK_NodeId_Acquisition_Header_BufferDuration)
		|| (top == OVTK_NodeId_Acquisition_Header_ExperimentInfo)
		|| (top == OVTK_NodeId_Acquisition_Header_Signal)
		|| (top == OVTK_NodeId_Acquisition_Header_Stimulation)
		|| (top == OVTK_NodeId_Acquisition_Header_ChannelLocalisation)
		|| (top == OVTK_NodeId_Acquisition_Header_ChannelUnits)
		|| (top == OVTK_NodeId_Acquisition_Buffer_ExperimentInfo)
		|| (top == OVTK_NodeId_Acquisition_Buffer_Signal)
		|| (top == OVTK_NodeId_Acquisition_Buffer_Stimulation)
		|| (top == OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation)
		|| (top == OVTK_NodeId_Acquisition_Buffer_ChannelUnits)
	) { }
	else { CEBMLBaseDecoder::openChild(identifier); }
}

void CAcquisitionDecoder::processChildData(const void* buffer, const size_t size)
{
	EBML::CIdentifier& top = m_nodes.top();

	if ((top == OVTK_NodeId_Acquisition_Header_BufferDuration)
		|| (top == OVTK_NodeId_Acquisition_Header_ExperimentInfo)
		|| (top == OVTK_NodeId_Acquisition_Header_Signal)
		|| (top == OVTK_NodeId_Acquisition_Header_Stimulation)
		|| (top == OVTK_NodeId_Acquisition_Header_ChannelLocalisation)
		|| (top == OVTK_NodeId_Acquisition_Header_ChannelUnits)
		|| (top == OVTK_NodeId_Acquisition_Buffer_ExperimentInfo)
		|| (top == OVTK_NodeId_Acquisition_Buffer_Signal)
		|| (top == OVTK_NodeId_Acquisition_Buffer_Stimulation)
		|| (top == OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation)
		|| (top == OVTK_NodeId_Acquisition_Buffer_ChannelUnits)
	)
	{
		if (top == OVTK_NodeId_Acquisition_Header_BufferDuration) { op_ui64BufferDuration = m_readerHelper->getUInt(buffer, size); }
		if (top == OVTK_NodeId_Acquisition_Header_ExperimentInfo) { appendMemoryBuffer(op_pExperimentInfoStream, buffer, size); }
		if (top == OVTK_NodeId_Acquisition_Header_Signal) { appendMemoryBuffer(op_pSignalStream, buffer, size); }
		if (top == OVTK_NodeId_Acquisition_Header_Stimulation) { appendMemoryBuffer(op_pStimulationStream, buffer, size); }
		if (top == OVTK_NodeId_Acquisition_Header_ChannelLocalisation) { appendMemoryBuffer(op_pChannelLocalisationStream, buffer, size); }
		if (top == OVTK_NodeId_Acquisition_Header_ChannelUnits) { appendMemoryBuffer(op_pChannelUnitsStream, buffer, size); }
		if (top == OVTK_NodeId_Acquisition_Buffer_ExperimentInfo) { appendMemoryBuffer(op_pExperimentInfoStream, buffer, size); }
		if (top == OVTK_NodeId_Acquisition_Buffer_Signal) { appendMemoryBuffer(op_pSignalStream, buffer, size); }
		if (top == OVTK_NodeId_Acquisition_Buffer_Stimulation) { appendMemoryBuffer(op_pStimulationStream, buffer, size); }
		if (top == OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation) { appendMemoryBuffer(op_pChannelLocalisationStream, buffer, size); }
		if (top == OVTK_NodeId_Acquisition_Buffer_ChannelUnits) { appendMemoryBuffer(op_pChannelUnitsStream, buffer, size); }
	}
	else { CEBMLBaseDecoder::processChildData(buffer, size); }
}

void CAcquisitionDecoder::closeChild()
{
	EBML::CIdentifier& top = m_nodes.top();

	if ((top == OVTK_NodeId_Acquisition_Header_BufferDuration)
		|| (top == OVTK_NodeId_Acquisition_Header_ExperimentInfo)
		|| (top == OVTK_NodeId_Acquisition_Header_Signal)
		|| (top == OVTK_NodeId_Acquisition_Header_Stimulation)
		|| (top == OVTK_NodeId_Acquisition_Header_ChannelLocalisation)
		|| (top == OVTK_NodeId_Acquisition_Header_ChannelUnits)
		|| (top == OVTK_NodeId_Acquisition_Buffer_ExperimentInfo)
		|| (top == OVTK_NodeId_Acquisition_Buffer_Signal)
		|| (top == OVTK_NodeId_Acquisition_Buffer_Stimulation)
		|| (top == OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation)
		|| (top == OVTK_NodeId_Acquisition_Buffer_ChannelUnits)
	) { }
	else { CEBMLBaseDecoder::closeChild(); }

	m_nodes.pop();
}

void CAcquisitionDecoder::appendMemoryBuffer(IMemoryBuffer* memoryBuffer, const void* buffer, const size_t size)
{
	if (memoryBuffer)
	{
		const size_t currentBufferSize = memoryBuffer->getSize();
		memoryBuffer->setSize(currentBufferSize + size, false);
		System::Memory::copy(memoryBuffer->getDirectPointer() + currentBufferSize, buffer, size);
	}
}
