#include "ovbt_sg_defines.h"

#include <fstream>

using namespace std;


string getBrutHexaCode(string formatedHexaCode)
{
	string res = formatedHexaCode;
	res.erase(res.begin(), res.begin() + 2);
	return res;
}

bool CMatlabGenerator::openFile(const char* filename)
{
	m_file.open(filename, ios::out | ios::trunc);
	if (!m_file.is_open()) { return false; }
	m_file << "function OV_stimulations()" << endl << endl;

	m_file << "global OVTK_StimulationId_LabelStart;" << endl;
	m_file << "OVTK_StimulationId_LabelStart = uint64_t(hex2dec('00008100'));" << endl << endl;
	m_file << "global OVTK_StimulationId_LabelEnd;" << endl;
	m_file << "OVTK_StimulationId_LabelEnd = uint64_t(hex2dec('000081ff'));" << endl << endl;

	return true;
}

bool CMatlabGenerator::appendStimulation(SStimulation& stim)
{
	m_file << "\tglobal " << stim.id << ";" << endl;
	m_file << "\t" << stim.id << " = uint64_t(hex2dec('" << getBrutHexaCode(stim.hexa) << "'));" << endl << endl;
	return true;
}


bool CMatlabGenerator::closeFile()
{
	m_file << "end" << endl;
	m_file.close();
	return true;
}
