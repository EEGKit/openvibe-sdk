#include "ovbt_sg_defines.h"

#include <fstream>

class CFileGeneratorBase
{
public:
	virtual ~CFileGeneratorBase() = default;
	virtual bool openFile(const char* sFilename) = 0;
	virtual bool appendStimulation(SStimulation& rStim) = 0;
	virtual bool closeFile() = 0;
protected:
	std::ofstream m_oFile;
};


class CCppDefineGenerator final : public CFileGeneratorBase
{
public:
	bool openFile(const char* sFilename) override;
	bool appendStimulation(SStimulation& rStim) override;
	bool closeFile() override;
};

class CCppCodeGenerator final : public CFileGeneratorBase
{
public:
	bool openFile(const char* sFilename) override;
	bool appendStimulation(SStimulation& rStim) override;
	bool closeFile() override;
};

class CMatlabGenerator final : public CFileGeneratorBase
{
public:
	bool openFile(const char* sFilename) override;
	bool appendStimulation(SStimulation& rStim) override;
	bool closeFile() override;
};
