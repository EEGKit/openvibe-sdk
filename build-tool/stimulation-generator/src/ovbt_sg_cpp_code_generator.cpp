#include "ovbt_sg_defines.h"
#include "ovbt_sg_file_generator_base.h"

#include <fstream>

using namespace std;

bool CCppCodeGenerator::openFile(const char* sFilename)
{
	m_oFile.open(sFilename, ios::out | ios::trunc);
	if (!m_oFile.is_open()) { return false; }
	m_oFile << "#include \"toolkit/ovtk_all.h\"" << endl << endl;


	m_oFile << "using namespace OpenViBE;" << endl;
	m_oFile << "using namespace OpenViBE::Kernel;" << endl;
	m_oFile << "using namespace OpenViBEToolkit;" << endl << endl << endl;

	m_oFile << "bool OpenViBEToolkit::initializeStimulationList(const IKernelContext& ctx)" << endl;
	m_oFile << "{" << endl;
	m_oFile << "\tITypeManager& typeManager=ctx.getTypeManager();" << endl << endl;
	return true;
}


bool CCppCodeGenerator::appendStimulation(SStimulation& rStim)
{
	m_oFile << "\ttypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, \""
			<< rStim.m_Name
			<< "\", "
			<< rStim.m_Id
			<< ");"
			<< endl;
	return true;
}


bool CCppCodeGenerator::closeFile()
{
	m_oFile << endl << "\treturn true;" << endl;
	m_oFile << "}" << endl;
	m_oFile.close();
	return true;
}
