#include "ovpCMasterAcquisitionEncoder.h"
#include "../../algorithms/encoders/ovpCAcquisitionEncoder.h"
#include "../../algorithms/encoders/ovpCExperimentInfoEncoder.h"
#include "../../algorithms/encoders/ovpCSignalEncoder.h"
#include "../../algorithms/encoders/ovpCStimulationEncoder.h"
#include "../../algorithms/encoders/ovpCChannelLocalisationEncoder.h"
#include "../../algorithms/encoders/ovpCChannelUnitsEncoder.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

bool CMasterAcquisitionEncoder::initialize()
{	
	// Manages sub-algorithms

	m_pAcquisitionStreamEncoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_AcquisitionEncoder));
	m_pExperimentInfoStreamEncoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ExperimentInfoEncoder));
	m_pSignalStreamEncoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_SignalEncoder));
	m_pStimulationStreamEncoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_StimulationEncoder));
	m_pChannelLocalisationStreamEncoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ChannelLocalisationEncoder));
	m_pChannelUnitsStreamEncoder = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_ChannelUnitsEncoder));

	m_pAcquisitionStreamEncoder->initialize();
	m_pExperimentInfoStreamEncoder->initialize();
	m_pSignalStreamEncoder->initialize();
	m_pStimulationStreamEncoder->initialize();
	m_pChannelLocalisationStreamEncoder->initialize();
	m_pChannelUnitsStreamEncoder->initialize();

	// Declares parameter handlers for this algorithm

	TParameterHandler<uint64_t> ip_ui64SubjectID(this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_SubjectID));
	TParameterHandler<uint64_t> ip_ui64SubjectAge(this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_SubjectAge));
	TParameterHandler<uint64_t> ip_ui64SubjectGender(this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_SubjectGender));
	TParameterHandler<IMatrix*> ip_pMatrix(this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_SignalMatrix));
	TParameterHandler<uint64_t> ip_ui64SamplingRate(this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_SignalSampling));
	TParameterHandler<IStimulationSet*> ip_pStimulationSet(this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_StimulationSet));
	TParameterHandler<uint64_t> ip_ui64BufferDuration(this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_BufferDuration));
	TParameterHandler<IMemoryBuffer*> op_pEncodedMemoryBuffer(this->getOutputParameter(OVP_Algorithm_EBMLEncoder_OutputParameterId_EncodedMemoryBuffer));
	TParameterHandler<IMatrix*> ip_pChannelLocalisationMaster(this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_ChannelLocalisation));
	TParameterHandler<IMatrix*> ip_pChannelUnitsMaster(this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_ChannelUnits));

	// Declares parameter handlers for sub-algorithm acquisition

	TParameterHandler<IMemoryBuffer*> ip_pAcquisitionChannelUnitsMemoryBuffer(m_pAcquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_ChannelUnitsStream));
	TParameterHandler<IMemoryBuffer*> ip_pAcquisitionChannelLocalisationMemoryBuffer(m_pAcquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_ChannelLocalisationStream));
	TParameterHandler<IMemoryBuffer*> ip_pAcquisitionExperimentInfoMemoryBuffer(m_pAcquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_ExperimentInfoStream));
	TParameterHandler<IMemoryBuffer*> ip_pAcquisitionSignalMemoryBuffer(m_pAcquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_SignalStream));
	TParameterHandler<IMemoryBuffer*> ip_pAcquisitionStimulationMemoryBuffer(m_pAcquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_StimulationStream));
	TParameterHandler<uint64_t> ip_ui64AcquisitionBufferDuration(m_pAcquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_BufferDuration));
	TParameterHandler<IMemoryBuffer*> op_pAcquisitionMemoryBuffer(m_pAcquisitionStreamEncoder->getOutputParameter(OVP_Algorithm_EBMLEncoder_OutputParameterId_EncodedMemoryBuffer));

	// Declares parameter handlers for sub-algorithm experiment information

	// TParameterHandler < uint64_t > ip_ui64ExperimentInfoExperimentID(m_pExperimentInfoStreamEncoder->getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_ExperimentID));
	// TParameterHandler < CString* > ip_pExperimentInfoExperimentDate(m_pExperimentInfoStreamEncoder->getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_ExperimentDate));
	TParameterHandler<uint64_t> ip_ui64ExperimentInfoSubjectID(m_pExperimentInfoStreamEncoder->getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_SubjectID));
	// TParameterHandler < CString* > ip_pExperimentInfoSubjectName(m_pExperimentInfoStreamEncoder->getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_SubjectName));
	TParameterHandler<uint64_t> ip_ui64ExperimentInfoSubjectAge(m_pExperimentInfoStreamEncoder->getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_SubjectAge));
	TParameterHandler<uint64_t> ip_ui64ExperimentInfoSubjectGender(m_pExperimentInfoStreamEncoder->getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_SubjectGender));
	// TParameterHandler < uint64_t > ip_ui64ExperimentInfoLaboratoryID(m_pExperimentInfoStreamEncoder->getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_LaboratoryID));
	// TParameterHandler < CString* > ip_pExperimentInfoLaboratoryName(m_pExperimentInfoStreamEncoder->getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_LaboratoryName));
	// TParameterHandler < uint64_t > ip_ui64ExperimentInfoTechnicianID(m_pExperimentInfoStreamEncoder->getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_TechnicianID));
	// TParameterHandler < CString* > ip_pExperimentInfoTehnicianName(m_pExperimentInfoStreamEncoder->getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_TechnicianName));
	TParameterHandler<IMemoryBuffer*> op_pExperimentInfoMemoryBuffer(m_pExperimentInfoStreamEncoder->getOutputParameter(OVP_Algorithm_EBMLEncoder_OutputParameterId_EncodedMemoryBuffer));

	// Declares parameter handlers for sub-algorithm signal

	TParameterHandler<IMatrix*> ip_pSignalMatrix(m_pSignalStreamEncoder->getInputParameter(OVP_Algorithm_StreamedMatrixEncoder_InputParameterId_Matrix));
	TParameterHandler<uint64_t> ip_ui64SignalSamplingRate(m_pSignalStreamEncoder->getInputParameter(OVP_Algorithm_SignalEncoder_InputParameterId_Sampling));
	TParameterHandler<IMemoryBuffer*> op_pSignalMemoryBuffer(m_pSignalStreamEncoder->getOutputParameter(OVP_Algorithm_EBMLEncoder_OutputParameterId_EncodedMemoryBuffer));

	// Declares parameter handlers for sub-algorithm stimulation

	TParameterHandler<IStimulationSet*> ip_pStimulationStimulationSet(m_pStimulationStreamEncoder->getInputParameter(OVP_Algorithm_StimulationEncoder_InputParameterId_StimulationSet));
	TParameterHandler<IMemoryBuffer*> op_pStimulationMemoryBuffer(m_pStimulationStreamEncoder->getOutputParameter(OVP_Algorithm_EBMLEncoder_OutputParameterId_EncodedMemoryBuffer));

	TParameterHandler<IMatrix*> ip_pChannelLocalisation(m_pChannelLocalisationStreamEncoder->getInputParameter(OVP_Algorithm_StreamedMatrixEncoder_InputParameterId_Matrix));
	TParameterHandler<IMemoryBuffer*> op_pChannelLocalisationMemoryBuffer(m_pChannelLocalisationStreamEncoder->getOutputParameter(OVP_Algorithm_EBMLEncoder_OutputParameterId_EncodedMemoryBuffer));

	TParameterHandler<IMatrix*> ip_pUnits(m_pChannelUnitsStreamEncoder->getInputParameter(OVP_Algorithm_StreamedMatrixEncoder_InputParameterId_Matrix));
	TParameterHandler<IMemoryBuffer*> op_pChannelUnitsMemoryBuffer(m_pChannelUnitsStreamEncoder->getOutputParameter(OVP_Algorithm_EBMLEncoder_OutputParameterId_EncodedMemoryBuffer));



	// Manage parameter connection / referencing | this algorithm to sub algorithm

	ip_ui64ExperimentInfoSubjectID.setReferenceTarget(ip_ui64SubjectID);
	ip_ui64ExperimentInfoSubjectAge.setReferenceTarget(ip_ui64SubjectAge);
	ip_ui64ExperimentInfoSubjectGender.setReferenceTarget(ip_ui64SubjectGender);
	ip_pSignalMatrix.setReferenceTarget(ip_pMatrix);
	ip_ui64SignalSamplingRate.setReferenceTarget(ip_ui64SamplingRate);
	ip_pStimulationStimulationSet.setReferenceTarget(ip_pStimulationSet);
	ip_ui64AcquisitionBufferDuration.setReferenceTarget(ip_ui64BufferDuration);
	op_pEncodedMemoryBuffer.setReferenceTarget(op_pAcquisitionMemoryBuffer);
	ip_pChannelLocalisationMaster.setReferenceTarget(ip_pChannelLocalisation);
	ip_pChannelUnitsMaster.setReferenceTarget(ip_pUnits);

	// Manage parameter connection / referencing | sub-algorithm to sub algorithm

	ip_pAcquisitionExperimentInfoMemoryBuffer.setReferenceTarget(op_pExperimentInfoMemoryBuffer);
	ip_pAcquisitionSignalMemoryBuffer.setReferenceTarget(op_pSignalMemoryBuffer);
	ip_pAcquisitionStimulationMemoryBuffer.setReferenceTarget(op_pStimulationMemoryBuffer);
	ip_pAcquisitionChannelLocalisationMemoryBuffer.setReferenceTarget(op_pChannelLocalisationMemoryBuffer);
	ip_pAcquisitionChannelUnitsMemoryBuffer.setReferenceTarget(op_pChannelUnitsMemoryBuffer);

	return true;
}

bool CMasterAcquisitionEncoder::uninitialize()
{
	m_pChannelUnitsStreamEncoder->uninitialize();
	m_pChannelLocalisationStreamEncoder->uninitialize();
	m_pStimulationStreamEncoder->uninitialize();
	m_pSignalStreamEncoder->uninitialize();
	m_pExperimentInfoStreamEncoder->uninitialize();
	m_pAcquisitionStreamEncoder->uninitialize();

	this->getAlgorithmManager().releaseAlgorithm(*m_pChannelUnitsStreamEncoder);
	this->getAlgorithmManager().releaseAlgorithm(*m_pChannelLocalisationStreamEncoder);
	this->getAlgorithmManager().releaseAlgorithm(*m_pStimulationStreamEncoder);
	this->getAlgorithmManager().releaseAlgorithm(*m_pSignalStreamEncoder);
	this->getAlgorithmManager().releaseAlgorithm(*m_pExperimentInfoStreamEncoder);
	this->getAlgorithmManager().releaseAlgorithm(*m_pAcquisitionStreamEncoder);

	return true;
}

bool CMasterAcquisitionEncoder::process()
{
	TParameterHandler<IMemoryBuffer*> ip_pAcquisitionChannelUnitsMemoryBuffer(m_pAcquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_ChannelUnitsStream));
	TParameterHandler<IMemoryBuffer*> ip_pAcquisitionChannelLocalisationMemoryBuffer(m_pAcquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_ChannelLocalisationStream));
	TParameterHandler<IMemoryBuffer*> ip_pAcquisitionExperimentInfoMemoryBuffer(m_pAcquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_ExperimentInfoStream));
	TParameterHandler<IMemoryBuffer*> ip_pAcquisitionSignalMemoryBuffer(m_pAcquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_SignalStream));
	TParameterHandler<IMemoryBuffer*> ip_pAcquisitionStimulationMemoryBuffer(m_pAcquisitionStreamEncoder->getInputParameter(OVP_Algorithm_AcquisitionEncoder_InputParameterId_StimulationStream));
	// TParameterHandler < IMemoryBuffer* > op_pAcquisitionMemoryBuffer(m_pAcquisitionStreamEncoder->getOutputParameter(OVP_Algorithm_EBMLEncoder_OutputParameterId_EncodedMemoryBuffer));

	if (this->isInputTriggerActive(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeHeader))
	{
		ip_pAcquisitionChannelUnitsMemoryBuffer->setSize(0, true);
		ip_pAcquisitionChannelLocalisationMemoryBuffer->setSize(0, true);
		ip_pAcquisitionExperimentInfoMemoryBuffer->setSize(0, true);
		ip_pAcquisitionSignalMemoryBuffer->setSize(0, true);
		ip_pAcquisitionStimulationMemoryBuffer->setSize(0, true);
		// op_pAcquisitionMemoryBuffer->setSize(0, true);

		m_pChannelLocalisationStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeHeader);
		m_pChannelUnitsStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeHeader);
		m_pStimulationStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeHeader);
		m_pSignalStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeHeader);
		m_pExperimentInfoStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeHeader);
		m_pAcquisitionStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeHeader);
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
		const TParameterHandler<bool> ip_bEncodeUnitData(
			this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_EncodeChannelUnitData));
		if (ip_bEncodeUnitData)
		{
			// this->getLogManager() << LogLevel_Info << "Encoding units " << ip_pUnits->getBufferElementCount() << "\n";
			m_pChannelUnitsStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeBuffer);
		}

		const TParameterHandler<bool> ip_bEncodeChannelLocalisationData(
			this->getInputParameter(OVP_Algorithm_MasterAcquisitionEncoder_InputParameterId_EncodeChannelLocalisationData));
		if (ip_bEncodeChannelLocalisationData) { m_pChannelLocalisationStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeBuffer); }

		m_pStimulationStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeBuffer);
		m_pSignalStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeBuffer);
		m_pExperimentInfoStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeBuffer);
		m_pAcquisitionStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeBuffer);
	}
	if (this->isInputTriggerActive(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeEnd))
	{
		ip_pAcquisitionChannelUnitsMemoryBuffer->setSize(0, true);
		ip_pAcquisitionChannelLocalisationMemoryBuffer->setSize(0, true);
		ip_pAcquisitionExperimentInfoMemoryBuffer->setSize(0, true);
		ip_pAcquisitionSignalMemoryBuffer->setSize(0, true);
		ip_pAcquisitionStimulationMemoryBuffer->setSize(0, true);
		// op_pAcquisitionMemoryBuffer->setSize(0, true);

		m_pChannelUnitsStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeEnd);
		m_pChannelLocalisationStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeEnd);
		m_pStimulationStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeEnd);
		m_pSignalStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeEnd);
		m_pExperimentInfoStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeEnd);
		m_pAcquisitionStreamEncoder->process(OVP_Algorithm_EBMLEncoder_InputTriggerId_EncodeEnd);
	}

	return true;
}
