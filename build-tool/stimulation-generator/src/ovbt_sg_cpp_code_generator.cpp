#include "ovbt_sg_defines.h"
#include "ovbt_sg_file_generator_base.h"

#include <fstream>

using namespace std;

bool CCppCodeGenerator::openFile(const char* filename)
{
	m_file.open(filename, ios::out | ios::trunc);
	if (!m_file.is_open()) { return false; }
	m_file << "#include \"toolkit/ovtk_all.h\"" << endl << endl;

	m_file << "using namespace OpenViBE;" << endl;
	m_file << "using namespace OpenViBE::Kernel;" << endl;
	m_file << "using namespace OpenViBEToolkit;" << endl << endl << endl;

	m_file << "bool OpenViBEToolkit::initializeStimulationList(const IKernelContext& ctx)" << endl;
	m_file << "{" << endl;
	m_file << "\tITypeManager& typeManager=ctx.getTypeManager();" << endl << endl;
	return true;
}


bool CCppCodeGenerator::appendStimulation(SStimulation& stim)
{
	m_file << "\ttypeManager.registerEnumerationEntry(OV_TypeId_Stimulation, \"" << stim.name << "\", " << stim.id << ");" << endl;
	return true;
}


bool CCppCodeGenerator::closeFile()
{
	m_file << endl << "\treturn true;" << endl << "}" << endl;
	m_file.close();
	return true;
}
