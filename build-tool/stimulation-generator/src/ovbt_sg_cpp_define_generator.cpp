#include "ovbt_sg_defines.h"
#include "ovbt_sg_file_generator_base.h"

#include <fstream>

using namespace std;

bool CCppDefineGenerator::openFile(const char* sFilename)
{
	m_oFile.open(sFilename, ios::out | ios::trunc);
	if (!m_oFile.is_open())
		return false;
	m_oFile << "#pragma once" << endl << endl;

	return true;
}


bool CCppDefineGenerator::appendStimulation(SStimulation& rStim)
{
	m_oFile << "#define " << rStim.m_sId << "  " << rStim.m_sHexaCode << endl;
	return true;
}


bool CCppDefineGenerator::closeFile(void)
{
	m_oFile << endl;
	m_oFile.close();
	return true;
}
