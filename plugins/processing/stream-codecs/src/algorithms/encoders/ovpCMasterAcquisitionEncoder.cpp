#include "ovpCMasterAcquisitionEncoder.h"
#include "../../algorithms/encoders/ovpCAcquisitionEncoder.h"
#include "../../algorithms/encoders/ovpCExperimentInfoEncoder.h"
#include "../../algorithms/encoders/ovpCSignalEncoder.h"
#include "../../algorithms/encoders/ovpCStimulationEncoder.h"
#include "../../algorithms/encoders/ovpCChannelLocalisationEncoder.h"
#include "../../algorithms/encoders/ovpCChannelUnitsEncoder.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace StreamCodecs;

bool CMasterAcquisitionEncoder::initialize()
{	
	// Manages sub-algorithms

	m_acquisitionStreamEncoder = &this->getAlgorithmManager().getAlgorithm(
		this->getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_AcquisitionEncoder));
	m_experimentInfoStreamEncoder = &this->getAlgorithmManager().getAlgorithm(
		this->getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ExperimentInfoEncoder));
	m_signalStreamEncoder      = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_SignalEncoder));
	m_stimulationStreamEncoder = &this->getAlgorithmManager().getAlgorithm(
		this->getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_StimulationEncoder));
	m_channelLocalisationStreamEncoder = &this->getAlgorithmManager().getAlgorithm(
		this->getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ChannelLocalisationEncoder));
	m_channelUnitsStreamEncoder = &this->getAlgorithmManager().getAlgorithm(
		this->getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ChannelUnitsEncoder));

	m_acquisitionStreamEncoder->initialize();
	m_experimentInfoStreamEncoder->initialize();
	m_signalStreamEncoder->initialize();
	m_stimulationStreamEncoder->initialize();
	m_channelLocalisationStreamEncoder->initialize();
	m_channelUnitsStreamEncoder->initialize();

	// Declares parameter handlers for this algorithm

	TParameterHandler<uint64_t> ip_subjectID(this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_SubjectID));
	TParameterHandler<uint64_t> ip_subjectAge(this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_SubjectAge));
	TParameterHandler<uint64_t> ip_subjectGender(this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_SubjectGender));
	TParameterHandler<IMatrix*> ip_pMatrix(this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_SignalMatrix));
	TParameterHandler<uint64_t> ip_sampling(this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_SignalSampling));
	TParameterHandler<IStimulationSet*> ip_stimSet(this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_StimulationSet));
	TParameterHandler<uint64_t> ip_bufferDuration(this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_BufferDuration));
	TParameterHandler<IMemoryBuffer*> op_buffer(this->getOutputParameter(OVP_Algorithm_EBMLEncoder_OutputParameterId_EncodedMemoryBuffer));
	TParameterHandler<IMatrix*> ip_channelLocalisationMaster(
		this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_ChannelLocalisation));
	TParameterHandler<IMatrix*> ip_channelUnitsMaster(this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_ChannelUnits));

	// Declares parameter handlers for sub-algorithm acquisition

	TParameterHandler<IMemoryBuffer*> ip_pAcquisitionChannelUnitsMemoryBuffer(
		m_acquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_ChannelUnitsStream));
	TParameterHandler<IMemoryBuffer*> ip_pAcquisitionChannelLocalisationMemoryBuffer(
		m_acquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_ChannelLocalisationStream));
	TParameterHandler<IMemoryBuffer*> ip_pAcquisitionExperimentInfoMemoryBuffer(
		m_acquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_ExperimentInfoStream));
	TParameterHandler<IMemoryBuffer*> ip_pAcquisitionSignalMemoryBuffer(
		m_acquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_SignalStream));
	TParameterHandler<IMemoryBuffer*> ip_pAcquisitionStimulationMemoryBuffer(
		m_acquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_StimulationStream));
	TParameterHandler<uint64_t> ip_AcquisitionBufferDuration(
		m_acquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_BufferDuration));
	TParameterHandler<IMemoryBuffer*> op_pAcquisitionMemoryBuffer(
		m_acquisitionStreamEncoder->getOutputParameter(OVP_Algorithm_EBMLEncoder_OutputParameterId_EncodedMemoryBuffer));

	// Declares parameter handlers for sub-algorithm experiment information

	// TParameterHandler < uint64_t > ip_ExperimentInfoExperimentID(m_experimentInfoStreamEncoder->getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_ExperimentID));
	// TParameterHandler < CString* > ip_experimentInfoExperimentDate(m_experimentInfoStreamEncoder->getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_ExperimentDate));
	TParameterHandler<uint64_t> ip_ExperimentInfoSubjectID(
		m_experimentInfoStreamEncoder->getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_SubjectID));
	// TParameterHandler < CString* > ip_experimentInfoSubjectName(m_experimentInfoStreamEncoder->getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_SubjectName));
	TParameterHandler<uint64_t> ip_ExperimentInfoSubjectAge(
		m_experimentInfoStreamEncoder->getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_SubjectAge));
	TParameterHandler<uint64_t> ip_ExperimentInfoSubjectGender(
		m_experimentInfoStreamEncoder->getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_SubjectGender));
	// TParameterHandler < uint64_t > ip_ExperimentInfoLaboratoryID(m_experimentInfoStreamEncoder->getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_LaboratoryID));
	// TParameterHandler < CString* > ip_experimentInfoLaboratoryName(m_experimentInfoStreamEncoder->getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_LaboratoryName));
	// TParameterHandler < uint64_t > ip_ExperimentInfoTechnicianID(m_experimentInfoStreamEncoder->getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_TechnicianID));
	// TParameterHandler < CString* > ip_experimentInfoTehnicianName(m_experimentInfoStreamEncoder->getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_TechnicianName));
	TParameterHandler<IMemoryBuffer*> op_experimentInfoMemoryBuffer(
		m_experimentInfoStreamEncoder->getOutputParameter(OVP_Algorithm_EBMLEncoder_OutputParameterId_EncodedMemoryBuffer));

	// Declares parameter handlers for sub-algorithm signal

	TParameterHandler<IMatrix*> ip_matrix(m_signalStreamEncoder->getInputParameter(OVP_Algorithm_StreamedMatrixEncoder_InputParameterId_Matrix));
	TParameterHandler<uint64_t> ip_signalSamplingRate(m_signalStreamEncoder->getInputParameter(OVP_Algorithm_SignalEncoder_InputParameterId_Sampling));
	TParameterHandler<IMemoryBuffer*> op_signalMemoryBuffer(
		m_signalStreamEncoder->getOutputParameter(OVP_Algorithm_EBMLEncoder_OutputParameterId_EncodedMemoryBuffer));

	// Declares parameter handlers for sub-algorithm stimulation

	TParameterHandler<IStimulationSet*> ip_stimulationStimulationSet(
		m_stimulationStreamEncoder->getInputParameter(OVP_Algorithm_StimulationEncoder_InputParameterId_StimulationSet));
	TParameterHandler<IMemoryBuffer*> op_stimulationMemoryBuffer(
		m_stimulationStreamEncoder->getOutputParameter(OVP_Algorithm_EBMLEncoder_OutputParameterId_EncodedMemoryBuffer));

	TParameterHandler<IMatrix*> ip_channelLocalisation(
		m_channelLocalisationStreamEncoder->getInputParameter(OVP_Algorithm_StreamedMatrixEncoder_InputParameterId_Matrix));
	TParameterHandler<IMemoryBuffer*> op_channelLocalisationMemoryBuffer(
		m_channelLocalisationStreamEncoder->getOutputParameter(OVP_Algorithm_EBMLEncoder_OutputParameterId_EncodedMemoryBuffer));

	TParameterHandler<IMatrix*> ip_pUnits(m_channelUnitsStreamEncoder->getInputParameter(OVP_Algorithm_StreamedMatrixEncoder_InputParameterId_Matrix));
	TParameterHandler<IMemoryBuffer*> op_channelUnitsMemoryBuffer(
		m_channelUnitsStreamEncoder->getOutputParameter(OVP_Algorithm_EBMLEncoder_OutputParameterId_EncodedMemoryBuffer));



	// Manage parameter connection / referencing | this algorithm to sub algorithm

	ip_ExperimentInfoSubjectID.setReferenceTarget(ip_subjectID);
	ip_ExperimentInfoSubjectAge.setReferenceTarget(ip_subjectAge);
	ip_ExperimentInfoSubjectGender.setReferenceTarget(ip_subjectGender);
	ip_matrix.setReferenceTarget(ip_pMatrix);
	ip_signalSamplingRate.setReferenceTarget(ip_sampling);
	ip_stimulationStimulationSet.setReferenceTarget(ip_stimSet);
	ip_AcquisitionBufferDuration.setReferenceTarget(ip_bufferDuration);
	op_buffer.setReferenceTarget(op_pAcquisitionMemoryBuffer);
	ip_channelLocalisationMaster.setReferenceTarget(ip_channelLocalisation);
	ip_channelUnitsMaster.setReferenceTarget(ip_pUnits);

	// Manage parameter connection / referencing | sub-algorithm to sub algorithm

	ip_pAcquisitionExperimentInfoMemoryBuffer.setReferenceTarget(op_experimentInfoMemoryBuffer);
	ip_pAcquisitionSignalMemoryBuffer.setReferenceTarget(op_signalMemoryBuffer);
	ip_pAcquisitionStimulationMemoryBuffer.setReferenceTarget(op_stimulationMemoryBuffer);
	ip_pAcquisitionChannelLocalisationMemoryBuffer.setReferenceTarget(op_channelLocalisationMemoryBuffer);
	ip_pAcquisitionChannelUnitsMemoryBuffer.setReferenceTarget(op_channelUnitsMemoryBuffer);

	return true;
}

bool CMasterAcquisitionEncoder::uninitialize()
{
	m_channelUnitsStreamEncoder->uninitialize();
	m_channelLocalisationStreamEncoder->uninitialize();
	m_stimulationStreamEncoder->uninitialize();
	m_signalStreamEncoder->uninitialize();
	m_experimentInfoStreamEncoder->uninitialize();
	m_acquisitionStreamEncoder->uninitialize();

	this->getAlgorithmManager().releaseAlgorithm(*m_channelUnitsStreamEncoder);
	this->getAlgorithmManager().releaseAlgorithm(*m_channelLocalisationStreamEncoder);
	this->getAlgorithmManager().releaseAlgorithm(*m_stimulationStreamEncoder);
	this->getAlgorithmManager().releaseAlgorithm(*m_signalStreamEncoder);
	this->getAlgorithmManager().releaseAlgorithm(*m_experimentInfoStreamEncoder);
	this->getAlgorithmManager().releaseAlgorithm(*m_acquisitionStreamEncoder);

	return true;
}

bool CMasterAcquisitionEncoder::process()
{
	TParameterHandler<IMemoryBuffer*> ip_pAcquisitionChannelUnitsMemoryBuffer(
		m_acquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_ChannelUnitsStream));
	TParameterHandler<IMemoryBuffer*> ip_pAcquisitionChannelLocalisationMemoryBuffer(
		m_acquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_ChannelLocalisationStream));
	TParameterHandler<IMemoryBuffer*> ip_pAcquisitionExperimentInfoMemoryBuffer(
		m_acquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_ExperimentInfoStream));
	TParameterHandler<IMemoryBuffer*> ip_pAcquisitionSignalMemoryBuffer(
		m_acquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_SignalStream));
	TParameterHandler<IMemoryBuffer*> ip_pAcquisitionStimulationMemoryBuffer(
		m_acquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_StimulationStream));
	// TParameterHandler < IMemoryBuffer* > op_pAcquisitionMemoryBuffer(m_acquisitionStreamEncoder->getOutputParameter(OVP_Algorithm_EBMLEncoder_OutputParameterId_EncodedMemoryBuffer));

	if (this->isInputTriggerActive(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeHeader))
	{
		ip_pAcquisitionChannelUnitsMemoryBuffer->setSize(0, true);
		ip_pAcquisitionChannelLocalisationMemoryBuffer->setSize(0, true);
		ip_pAcquisitionExperimentInfoMemoryBuffer->setSize(0, true);
		ip_pAcquisitionSignalMemoryBuffer->setSize(0, true);
		ip_pAcquisitionStimulationMemoryBuffer->setSize(0, true);
		// op_pAcquisitionMemoryBuffer->setSize(0, true);

		m_channelLocalisationStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeHeader);
		m_channelUnitsStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeHeader);
		m_stimulationStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeHeader);
		m_signalStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeHeader);
		m_experimentInfoStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeHeader);
		m_acquisitionStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeHeader);
	}
	if (this->isInputTriggerActive(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeBuffer))
	{
		ip_pAcquisitionChannelUnitsMemoryBuffer->setSize(0, true);
		ip_pAcquisitionChannelLocalisationMemoryBuffer->setSize(0, true);
		ip_pAcquisitionExperimentInfoMemoryBuffer->setSize(0, true);
		ip_pAcquisitionSignalMemoryBuffer->setSize(0, true);
		ip_pAcquisitionStimulationMemoryBuffer->setSize(0, true);
		// op_pAcquisitionMemoryBuffer->setSize(0, true);

		// For these streams, we only send the buffer if there is something to send
		const TParameterHandler<bool>
				ip_bEncodeUnitData(this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_EncodeChannelUnitData));
		if (ip_bEncodeUnitData)
		{
			// this->getLogManager() << LogLevel_Info << "Encoding units " << ip_pUnits->getBufferElementCount() << "\n";
			m_channelUnitsStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeBuffer);
		}

		const TParameterHandler<bool> ip_encodeChannelLocalisationData(
			this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_EncodeChannelLocalisationData));
		if (ip_encodeChannelLocalisationData) { m_channelLocalisationStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeBuffer); }

		m_stimulationStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeBuffer);
		m_signalStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeBuffer);
		m_experimentInfoStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeBuffer);
		m_acquisitionStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeBuffer);
	}
	if (this->isInputTriggerActive(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeEnd))
	{
		ip_pAcquisitionChannelUnitsMemoryBuffer->setSize(0, true);
		ip_pAcquisitionChannelLocalisationMemoryBuffer->setSize(0, true);
		ip_pAcquisitionExperimentInfoMemoryBuffer->setSize(0, true);
		ip_pAcquisitionSignalMemoryBuffer->setSize(0, true);
		ip_pAcquisitionStimulationMemoryBuffer->setSize(0, true);
		// op_pAcquisitionMemoryBuffer->setSize(0, true);

		m_channelUnitsStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeEnd);
		m_channelLocalisationStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeEnd);
		m_stimulationStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeEnd);
		m_signalStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeEnd);
		m_experimentInfoStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeEnd);
		m_acquisitionStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeEnd);
	}

	return true;
}
