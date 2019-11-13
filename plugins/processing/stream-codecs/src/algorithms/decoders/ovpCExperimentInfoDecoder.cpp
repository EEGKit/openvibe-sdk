#include "ovpCExperimentInfoDecoder.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

CExperimentInfoDecoder::CExperimentInfoDecoder() {}

// ________________________________________________________________________________________________________________
//

bool CExperimentInfoDecoder::initialize()
{
	CEBMLBaseDecoder::initialize();

	op_ui64ExperimentID.initialize(getOutputParameter(OVP_Algorithm_ExperimentInfoDecoder_OutputParameterId_ExperimentID));
	op_pExperimentDate.initialize(getOutputParameter(OVP_Algorithm_ExperimentInfoDecoder_OutputParameterId_ExperimentDate));
	op_ui64SubjectID.initialize(getOutputParameter(OVP_Algorithm_ExperimentInfoDecoder_OutputParameterId_SubjectID));
	op_pSubjectName.initialize(getOutputParameter(OVP_Algorithm_ExperimentInfoDecoder_OutputParameterId_SubjectName));
	op_ui64SubjectAge.initialize(getOutputParameter(OVP_Algorithm_ExperimentInfoDecoder_OutputParameterId_SubjectAge));
	op_ui64SubjectGender.initialize(getOutputParameter(OVP_Algorithm_ExperimentInfoDecoder_OutputParameterId_SubjectGender));
	op_ui64LaboratoryID.initialize(getOutputParameter(OVP_Algorithm_ExperimentInfoDecoder_OutputParameterId_LaboratoryID));
	op_pLaboratoryName.initialize(getOutputParameter(OVP_Algorithm_ExperimentInfoDecoder_OutputParameterId_LaboratoryName));
	op_ui64TechnicianID.initialize(getOutputParameter(OVP_Algorithm_ExperimentInfoDecoder_OutputParameterId_TechnicianID));
	op_pTechnicianName.initialize(getOutputParameter(OVP_Algorithm_ExperimentInfoDecoder_OutputParameterId_TechnicianName));

	return true;
}

bool CExperimentInfoDecoder::uninitialize()
{
	op_pTechnicianName.uninitialize();
	op_ui64TechnicianID.uninitialize();
	op_pLaboratoryName.uninitialize();
	op_ui64LaboratoryID.uninitialize();
	op_ui64SubjectGender.uninitialize();
	op_ui64SubjectAge.uninitialize();
	op_pSubjectName.uninitialize();
	op_ui64SubjectID.uninitialize();
	op_pExperimentDate.uninitialize();
	op_ui64ExperimentID.uninitialize();

	CEBMLBaseDecoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CExperimentInfoDecoder::isMasterChild(const EBML::CIdentifier& identifier)
{
	if (identifier == OVTK_NodeId_Header_ExperimentInfo) { return true; }
	if (identifier == OVTK_NodeId_Header_ExperimentInfo_Experiment) { return true; }
	if (identifier == OVTK_NodeId_Header_ExperimentInfo_Experiment_ID) { return false; }
	if (identifier == OVTK_NodeId_Header_ExperimentInfo_Experiment_Date) { return false; }
	if (identifier == OVTK_NodeId_Header_ExperimentInfo_Subject) { return true; }
	if (identifier == OVTK_NodeId_Header_ExperimentInfo_Subject_ID) { return false; }
	if (identifier == OVTK_NodeId_Header_ExperimentInfo_Subject_Name) { return false; }
	if (identifier == OVTK_NodeId_Header_ExperimentInfo_Subject_Age) { return false; }
	if (identifier == OVTK_NodeId_Header_ExperimentInfo_Subject_Gender) { return false; }
	if (identifier == OVTK_NodeId_Header_ExperimentInfo_Context) { return true; }
	if (identifier == OVTK_NodeId_Header_ExperimentInfo_Context_LaboratoryID) { return false; }
	if (identifier == OVTK_NodeId_Header_ExperimentInfo_Context_LaboratoryName) { return false; }
	if (identifier == OVTK_NodeId_Header_ExperimentInfo_Context_TechnicianID) { return false; }
	if (identifier == OVTK_NodeId_Header_ExperimentInfo_Context_TechnicianName) { return false; }
	return CEBMLBaseDecoder::isMasterChild(identifier);
}

void CExperimentInfoDecoder::openChild(const EBML::CIdentifier& identifier)
{
	m_nodes.push(identifier);

	EBML::CIdentifier& top = m_nodes.top();

	if ((top == OVTK_NodeId_Header_ExperimentInfo)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Experiment)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Experiment_ID)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Experiment_Date)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Subject)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Subject_ID)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Subject_Name)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Subject_Age)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Subject_Gender)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Context)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Context_LaboratoryID)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Context_LaboratoryName)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Context_TechnicianID)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Context_TechnicianName)) { }
	else { CEBMLBaseDecoder::openChild(identifier); }
}

void CExperimentInfoDecoder::processChildData(const void* buffer, const size_t size)
{
	EBML::CIdentifier& top = m_nodes.top();

	if ((top == OVTK_NodeId_Header_ExperimentInfo)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Experiment)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Experiment_ID)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Experiment_Date)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Subject)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Subject_ID)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Subject_Name)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Subject_Age)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Subject_Gender)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Context)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Context_LaboratoryID)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Context_LaboratoryName)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Context_TechnicianID)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Context_TechnicianName))
	{
		if (top == OVTK_NodeId_Header_ExperimentInfo_Experiment_ID) { op_ui64ExperimentID = m_readerHelper->getUInt(buffer, size); }
		if (top == OVTK_NodeId_Header_ExperimentInfo_Experiment_Date) { op_pExperimentDate->set(m_readerHelper->getStr(buffer, size)); }
		if (top == OVTK_NodeId_Header_ExperimentInfo_Subject_ID) { op_ui64SubjectID = m_readerHelper->getUInt(buffer, size); }
		if (top == OVTK_NodeId_Header_ExperimentInfo_Subject_Name) { op_pSubjectName->set(m_readerHelper->getStr(buffer, size)); }
		if (top == OVTK_NodeId_Header_ExperimentInfo_Subject_Age) { op_ui64SubjectAge = m_readerHelper->getUInt(buffer, size); }
		if (top == OVTK_NodeId_Header_ExperimentInfo_Subject_Gender) { op_ui64SubjectGender = m_readerHelper->getUInt(buffer, size); }
		if (top == OVTK_NodeId_Header_ExperimentInfo_Context_LaboratoryID)
		{
			op_ui64LaboratoryID = m_readerHelper->getUInt(buffer, size);
		}
		if (top == OVTK_NodeId_Header_ExperimentInfo_Context_LaboratoryName) { op_pLaboratoryName->set(m_readerHelper->getStr(buffer, size)); }
		if (top == OVTK_NodeId_Header_ExperimentInfo_Context_TechnicianID)
		{
			op_ui64TechnicianID = m_readerHelper->getUInt(buffer, size);
		}
		if (top == OVTK_NodeId_Header_ExperimentInfo_Context_TechnicianName) { op_pTechnicianName->set(m_readerHelper->getStr(buffer, size)); }
	}
	else { CEBMLBaseDecoder::processChildData(buffer, size); }
}

void CExperimentInfoDecoder::closeChild()
{
	EBML::CIdentifier& top = m_nodes.top();

	if ((top == OVTK_NodeId_Header_ExperimentInfo)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Experiment)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Experiment_ID)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Experiment_Date)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Subject)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Subject_ID)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Subject_Name)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Subject_Age)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Subject_Gender)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Context)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Context_LaboratoryID)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Context_LaboratoryName)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Context_TechnicianID)
		|| (top == OVTK_NodeId_Header_ExperimentInfo_Context_TechnicianName)) { }
	else { CEBMLBaseDecoder::closeChild(); }

	m_nodes.pop();
}
