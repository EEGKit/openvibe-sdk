#pragma once
#include "ovbt_sg_defines.h"

#include <fstream>

class CFileGeneratorBase
{
public:
	virtual ~CFileGeneratorBase() = default;
	virtual bool openFile(const char* filename) = 0;
	virtual bool appendStimulation(SStimulation& stim) = 0;
	virtual bool closeFile() = 0;
protected:
	std::ofstream m_file;
};


class CCppDefineGenerator final : public CFileGeneratorBase
{
public:
	bool openFile(const char* filename) override;
	bool appendStimulation(SStimulation& stim) override;
	bool closeFile() override;
};

class CCppCodeGenerator final : public CFileGeneratorBase
{
public:
	bool openFile(const char* filename) override;
	bool appendStimulation(SStimulation& stim) override;
	bool closeFile() override;
};

class CMatlabGenerator final : public CFileGeneratorBase
{
public:
	bool openFile(const char* filename) override;
	bool appendStimulation(SStimulation& stim) override;
	bool closeFile() override;
};
