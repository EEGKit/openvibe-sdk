#include "ovpCExperimentInfoEncoder.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

bool CExperimentInfoEncoder::initialize()
{
	CEBMLBaseEncoder::initialize();

	ip_ui64ExperimentID.initialize(getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_ExperimentID));
	ip_pExperimentDate.initialize(getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_ExperimentDate));
	ip_ui64SubjectID.initialize(getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_SubjectID));
	ip_pSubjectName.initialize(getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_SubjectName));
	ip_ui64SubjectAge.initialize(getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_SubjectAge));
	ip_ui64SubjectGender.initialize(getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_SubjectGender));
	ip_ui64LaboratoryID.initialize(getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_LaboratoryID));
	ip_pLaboratoryName.initialize(getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_LaboratoryName));
	ip_ui64TechnicianID.initialize(getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_TechnicianID));
	ip_pTechnicianName.initialize(getInputParameter(OVP_Algorithm_ExperimentInfoEncoder_InputParameterId_TechnicianName));

	return true;
}

bool CExperimentInfoEncoder::uninitialize()
{
	ip_pTechnicianName.uninitialize();
	ip_ui64TechnicianID.uninitialize();
	ip_pLaboratoryName.uninitialize();
	ip_ui64LaboratoryID.uninitialize();
	ip_ui64SubjectGender.uninitialize();
	ip_ui64SubjectAge.uninitialize();
	ip_pSubjectName.uninitialize();
	ip_ui64SubjectID.uninitialize();
	ip_pExperimentDate.uninitialize();
	ip_ui64ExperimentID.uninitialize();

	CEBMLBaseEncoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CExperimentInfoEncoder::processHeader()
{
	m_writerHelper->openChild(OVTK_NodeId_Header_ExperimentInfo);
	m_writerHelper->openChild(OVTK_NodeId_Header_ExperimentInfo_Experiment);
	m_writerHelper->openChild(OVTK_NodeId_Header_ExperimentInfo_Experiment_ID);
	m_writerHelper->setUInt(ip_ui64ExperimentID);
	m_writerHelper->closeChild();
	m_writerHelper->openChild(OVTK_NodeId_Header_ExperimentInfo_Experiment_Date);
	m_writerHelper->setStr(ip_pExperimentDate->toASCIIString());
	m_writerHelper->closeChild();
	m_writerHelper->closeChild();
	m_writerHelper->openChild(OVTK_NodeId_Header_ExperimentInfo_Subject);
	m_writerHelper->openChild(OVTK_NodeId_Header_ExperimentInfo_Subject_ID);
	m_writerHelper->setUInt(ip_ui64SubjectID);
	m_writerHelper->closeChild();
	m_writerHelper->openChild(OVTK_NodeId_Header_ExperimentInfo_Subject_Name);
	m_writerHelper->setStr(ip_pSubjectName->toASCIIString());
	m_writerHelper->closeChild();
	m_writerHelper->openChild(OVTK_NodeId_Header_ExperimentInfo_Subject_Age);
	m_writerHelper->setUInt(ip_ui64SubjectAge);
	m_writerHelper->closeChild();
	m_writerHelper->openChild(OVTK_NodeId_Header_ExperimentInfo_Subject_Gender);
	m_writerHelper->setUInt(ip_ui64SubjectGender);
	m_writerHelper->closeChild();
	m_writerHelper->closeChild();
	m_writerHelper->openChild(OVTK_NodeId_Header_ExperimentInfo_Context);
	m_writerHelper->openChild(OVTK_NodeId_Header_ExperimentInfo_Context_LaboratoryID);
	m_writerHelper->setUInt(ip_ui64LaboratoryID);
	m_writerHelper->closeChild();
	m_writerHelper->openChild(OVTK_NodeId_Header_ExperimentInfo_Context_LaboratoryName);
	m_writerHelper->setStr(ip_pLaboratoryName->toASCIIString());
	m_writerHelper->closeChild();
	m_writerHelper->openChild(OVTK_NodeId_Header_ExperimentInfo_Context_TechnicianID);
	m_writerHelper->setUInt(ip_ui64TechnicianID);
	m_writerHelper->closeChild();
	m_writerHelper->openChild(OVTK_NodeId_Header_ExperimentInfo_Context_TechnicianName);
	m_writerHelper->setStr(ip_pTechnicianName->toASCIIString());
	m_writerHelper->closeChild();
	m_writerHelper->closeChild();
	m_writerHelper->closeChild();

	return true;
}
