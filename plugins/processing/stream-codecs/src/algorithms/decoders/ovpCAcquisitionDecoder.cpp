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

bool CAcquisitionDecoder::isMasterChild(const EBML::CIdentifier& rIdentifier)
{
	if (rIdentifier == OVTK_NodeId_Acquisition_Header_BufferDuration) { return false; }
	if (rIdentifier == OVTK_NodeId_Acquisition_Header_ExperimentInformation) { return false; }
	if (rIdentifier == OVTK_NodeId_Acquisition_Header_Signal) { return false; }
	if (rIdentifier == OVTK_NodeId_Acquisition_Header_Stimulation) { return false; }
	if (rIdentifier == OVTK_NodeId_Acquisition_Header_ChannelLocalisation) { return false; }
	if (rIdentifier == OVTK_NodeId_Acquisition_Header_ChannelUnits) { return false; }
	if (rIdentifier == OVTK_NodeId_Acquisition_Buffer_ExperimentInformation) { return false; }
	if (rIdentifier == OVTK_NodeId_Acquisition_Buffer_Signal) { return false; }
	if (rIdentifier == OVTK_NodeId_Acquisition_Buffer_Stimulation) { return false; }
	if (rIdentifier == OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation) { return false; }
	if (rIdentifier == OVTK_NodeId_Acquisition_Buffer_ChannelUnits) { return false; }
	return CEBMLBaseDecoder::isMasterChild(rIdentifier);
}

void CAcquisitionDecoder::openChild(const EBML::CIdentifier& rIdentifier)
{
	m_vNodes.push(rIdentifier);

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
	else
	{
		CEBMLBaseDecoder::openChild(rIdentifier);
	}
}

void CAcquisitionDecoder::processChildData(const void* pBuffer, const uint64_t ui64BufferSize)
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
		if (l_rTop == OVTK_NodeId_Acquisition_Header_BufferDuration) { op_ui64BufferDuration = m_pEBMLReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize); }
		if (l_rTop == OVTK_NodeId_Acquisition_Header_ExperimentInformation) { this->appendMemoryBuffer(op_pExperimentInformationStream, pBuffer, ui64BufferSize); }
		if (l_rTop == OVTK_NodeId_Acquisition_Header_Signal) { this->appendMemoryBuffer(op_pSignalStream, pBuffer, ui64BufferSize); }
		if (l_rTop == OVTK_NodeId_Acquisition_Header_Stimulation) { this->appendMemoryBuffer(op_pStimulationStream, pBuffer, ui64BufferSize); }
		if (l_rTop == OVTK_NodeId_Acquisition_Header_ChannelLocalisation) { this->appendMemoryBuffer(op_pChannelLocalisationStream, pBuffer, ui64BufferSize); }
		if (l_rTop == OVTK_NodeId_Acquisition_Header_ChannelUnits) { this->appendMemoryBuffer(op_pChannelUnitsStream, pBuffer, ui64BufferSize); }
		if (l_rTop == OVTK_NodeId_Acquisition_Buffer_ExperimentInformation) { this->appendMemoryBuffer(op_pExperimentInformationStream, pBuffer, ui64BufferSize); }
		if (l_rTop == OVTK_NodeId_Acquisition_Buffer_Signal) { this->appendMemoryBuffer(op_pSignalStream, pBuffer, ui64BufferSize); }
		if (l_rTop == OVTK_NodeId_Acquisition_Buffer_Stimulation) { this->appendMemoryBuffer(op_pStimulationStream, pBuffer, ui64BufferSize); }
		if (l_rTop == OVTK_NodeId_Acquisition_Buffer_ChannelLocalisation) { this->appendMemoryBuffer(op_pChannelLocalisationStream, pBuffer, ui64BufferSize); }
		if (l_rTop == OVTK_NodeId_Acquisition_Buffer_ChannelUnits) { this->appendMemoryBuffer(op_pChannelUnitsStream, pBuffer, ui64BufferSize); }
	}
	else
	{
		CEBMLBaseDecoder::processChildData(pBuffer, ui64BufferSize);
	}
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

void CAcquisitionDecoder::appendMemoryBuffer(IMemoryBuffer* pMemoryBuffer, const void* pBuffer, const uint64_t ui64BufferSize)
{
	if (pMemoryBuffer)
	{
		uint64_t l_ui64CurrentBufferSize = pMemoryBuffer->getSize();
		pMemoryBuffer->setSize(l_ui64CurrentBufferSize + ui64BufferSize, false);
		System::Memory::copy(pMemoryBuffer->getDirectPointer() + l_ui64CurrentBufferSize, pBuffer, ui64BufferSize);
	}
}
