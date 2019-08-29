#include "ovpCExperimentInformationDecoder.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

CExperimentInformationDecoder::CExperimentInformationDecoder() {}

// ________________________________________________________________________________________________________________
//

bool CExperimentInformationDecoder::initialize()
{
	CEBMLBaseDecoder::initialize();

	op_ui64ExperimentIdentifier.initialize(getOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_ExperimentIdentifier));
	op_pExperimentDate.initialize(getOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_ExperimentDate));
	op_ui64SubjectIdentifier.initialize(getOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectIdentifier));
	op_pSubjectName.initialize(getOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectName));
	op_ui64SubjectAge.initialize(getOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectAge));
	op_ui64SubjectGender.initialize(getOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_SubjectGender));
	op_ui64LaboratoryIdentifier.initialize(getOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_LaboratoryIdentifier));
	op_pLaboratoryName.initialize(getOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_LaboratoryName));
	op_ui64TechnicianIdentifier.initialize(getOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_TechnicianIdentifier));
	op_pTechnicianName.initialize(getOutputParameter(OVP_Algorithm_ExperimentInformationStreamDecoder_OutputParameterId_TechnicianName));

	return true;
}

bool CExperimentInformationDecoder::uninitialize()
{
	op_pTechnicianName.uninitialize();
	op_ui64TechnicianIdentifier.uninitialize();
	op_pLaboratoryName.uninitialize();
	op_ui64LaboratoryIdentifier.uninitialize();
	op_ui64SubjectGender.uninitialize();
	op_ui64SubjectAge.uninitialize();
	op_pSubjectName.uninitialize();
	op_ui64SubjectIdentifier.uninitialize();
	op_pExperimentDate.uninitialize();
	op_ui64ExperimentIdentifier.uninitialize();

	CEBMLBaseDecoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CExperimentInformationDecoder::isMasterChild(const EBML::CIdentifier& rIdentifier)
{
	if (rIdentifier == OVTK_NodeId_Header_ExperimentInformation) { return true; }
	if (rIdentifier == OVTK_NodeId_Header_ExperimentInformation_Experiment) { return true; }
	if (rIdentifier == OVTK_NodeId_Header_ExperimentInformation_Experiment_Identifier) { return false; }
	if (rIdentifier == OVTK_NodeId_Header_ExperimentInformation_Experiment_Date) { return false; }
	if (rIdentifier == OVTK_NodeId_Header_ExperimentInformation_Subject) { return true; }
	if (rIdentifier == OVTK_NodeId_Header_ExperimentInformation_Subject_Identifier) { return false; }
	if (rIdentifier == OVTK_NodeId_Header_ExperimentInformation_Subject_Name) { return false; }
	if (rIdentifier == OVTK_NodeId_Header_ExperimentInformation_Subject_Age) { return false; }
	if (rIdentifier == OVTK_NodeId_Header_ExperimentInformation_Subject_Gender) { return false; }
	if (rIdentifier == OVTK_NodeId_Header_ExperimentInformation_Context) { return true; }
	if (rIdentifier == OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryIdentifier) { return false; }
	if (rIdentifier == OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryName) { return false; }
	if (rIdentifier == OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianIdentifier) { return false; }
	if (rIdentifier == OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianName) { return false; }
	return CEBMLBaseDecoder::isMasterChild(rIdentifier);
}

void CExperimentInformationDecoder::openChild(const EBML::CIdentifier& rIdentifier)
{
	m_vNodes.push(rIdentifier);

	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Header_ExperimentInformation)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Experiment)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Experiment_Identifier)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Experiment_Date)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Subject)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Subject_Identifier)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Subject_Name)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Subject_Age)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Subject_Gender)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Context)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryIdentifier)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryName)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianIdentifier)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianName)) { }
	else { CEBMLBaseDecoder::openChild(rIdentifier); }
}

void CExperimentInformationDecoder::processChildData(const void* buffer, const uint64_t size)
{
	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Header_ExperimentInformation)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Experiment)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Experiment_Identifier)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Experiment_Date)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Subject)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Subject_Identifier)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Subject_Name)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Subject_Age)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Subject_Gender)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Context)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryIdentifier)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryName)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianIdentifier)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianName))
	{
		if (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Experiment_Identifier) { op_ui64ExperimentIdentifier = m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size); }
		if (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Experiment_Date) { op_pExperimentDate->set(m_pEBMLReaderHelper->getASCIIStringFromChildData(buffer, size)); }
		if (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Subject_Identifier) { op_ui64SubjectIdentifier = m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size); }
		if (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Subject_Name) { op_pSubjectName->set(m_pEBMLReaderHelper->getASCIIStringFromChildData(buffer, size)); }
		if (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Subject_Age) { op_ui64SubjectAge = m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size); }
		if (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Subject_Gender) { op_ui64SubjectGender = m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size); }
		if (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryIdentifier) { op_ui64LaboratoryIdentifier = m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size); }
		if (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryName) { op_pLaboratoryName->set(m_pEBMLReaderHelper->getASCIIStringFromChildData(buffer, size)); }
		if (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianIdentifier) { op_ui64TechnicianIdentifier = m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size); }
		if (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianName) { op_pTechnicianName->set(m_pEBMLReaderHelper->getASCIIStringFromChildData(buffer, size)); }
	}
	else
	{
		CEBMLBaseDecoder::processChildData(buffer, size);
	}
}

void CExperimentInformationDecoder::closeChild()
{
	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Header_ExperimentInformation)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Experiment)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Experiment_Identifier)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Experiment_Date)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Subject)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Subject_Identifier)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Subject_Name)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Subject_Age)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Subject_Gender)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Context)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryIdentifier)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Context_LaboratoryName)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianIdentifier)
		|| (l_rTop == OVTK_NodeId_Header_ExperimentInformation_Context_TechnicianName)) { }
	else { CEBMLBaseDecoder::closeChild(); }

	m_vNodes.pop();
}
