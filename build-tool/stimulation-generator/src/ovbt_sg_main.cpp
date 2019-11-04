#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "ovbt_sg_defines.h"
#include "ovbt_sg_file_generator_base.h"

using namespace std;

typedef enum
{
	CPP, MATLAB, PYTHON, LUA, UNKNOWN
} generation_type;


generation_type parse_argument(string option)
{
	if (option == "--cpp") { return CPP; }
	if (option == "--matlab") { return MATLAB; }
	if (option == "--python") { return PYTHON; }
	if (option == "--lua") { return LUA; }
	return UNKNOWN;
}

int generate_generator_list(vector<CFileGeneratorBase*>& rList, generation_type rType, int argc, char** argv)
{
	switch (rType)
	{
		case CPP:
		{
			if (argc < 4) { return -1; }
			CFileGeneratorBase* gen = new CCppDefineGenerator();
			if (!gen->openFile(argv[3]))
			{
				cerr << "Unable to open " << argv[3] << endl;
				return -1;
			}
			rList.push_back(gen);

			gen = new CCppCodeGenerator();
			if (!gen->openFile(argv[4]))
			{
				cerr << "Unable to open " << argv[4] << endl;
				return -1;
			}
			rList.push_back(gen);
			return 0;
		}

		case MATLAB:
		{
			CFileGeneratorBase* gen = new CMatlabGenerator();
			if (!gen->openFile(argv[3]))
			{
				cerr << "Unable to open " << argv[3] << endl;
				return -1;
			}
			rList.push_back(gen);
			return 0;
		}
		case PYTHON:
		case LUA:
		case UNKNOWN:
		default:
		{
			cerr << "Unhandle type. Fatal error" << endl;
			return -1;
		}
	}
}

int main(int argc, char** argv)
{
	if (argc < 3) { return -1; }
	generation_type type = parse_argument(argv[1]);

	vector<SStimulation> stimulations;
	vector<CFileGeneratorBase*> generators;

	ifstream stimFile(argv[2]);
	string name, id, hexaCode;
	while (stimFile >> name >> id >> hexaCode)
	{
		SStimulation temp = { name, id, hexaCode };
		stimulations.push_back(temp);
	}

	if (generate_generator_list(generators, type, argc, argv)) { return -1; }

	//Now we generate all files that needs to be done
	for (auto it = stimulations.begin(); it != stimulations.end(); ++it)
	{
		SStimulation& temp = *it;
		for (auto it_gen = generators.begin(); it_gen != generators.end(); ++it_gen) { (*it_gen)->appendStimulation(temp); }
	}

	for (auto it_gen = generators.begin(); it_gen != generators.end(); ++it_gen) { (*it_gen)->closeFile(); }

	return 0;
}
