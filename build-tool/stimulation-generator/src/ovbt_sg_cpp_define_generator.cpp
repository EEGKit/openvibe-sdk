#include "ovbt_sg_defines.h"

#include <fstream>

using namespace std;

bool CCppDefineGenerator::openFile(const char* filename)
{
	m_file.open(filename, ios::out | ios::trunc);
	if (!m_file.is_open()) { return false; }
	m_file << "#pragma once" << endl << endl;

	return true;
}

bool CCppDefineGenerator::appendStimulation(SStimulation& stim)
{
	m_file << "#define " << stim.id << "  " << stim.hexa << endl;
	return true;
}

bool CCppDefineGenerator::closeFile()
{
	m_file << endl;
	m_file.close();
	return true;
}
