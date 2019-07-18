/*********************************************************************
* Software License Agreement (AGPL-3 License)
*
* OpenViBE SDK Test Software
* Based on OpenViBE V1.1.0, Copyright (C) Inria, 2006-2015
* Copyright (C) Inria, 2015-2017,V1.0
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License version 3,
* as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <iomanip>
#include <fstream>

#include "ebml/IReader.h"
#include "ebml/IReaderHelper.h"
#include "ebml/CReader.h"
#include "ebml/CReaderHelper.h"

#include "ovtAssert.h"

std::ofstream g_OutputStream;

class CReaderCallBack : public EBML::IReaderCallBack
{
public:
	CReaderCallBack(void)
		: m_Depth(0) { }

	virtual ~CReaderCallBack(void) { }

	bool isMasterChild(const EBML::CIdentifier& rIdentifier) override
	{
		if (rIdentifier == EBML_Identifier_Header) return true;
		if (rIdentifier == EBML::CIdentifier(0xffff)) return true;

		return false;
	}

	void openChild(const EBML::CIdentifier& rIdentifier) override
	{
		m_CurrentIdentifier = rIdentifier;

		for (int i = 0; i < m_Depth; i++) g_OutputStream << "   ";
		g_OutputStream << "Opening child node [0x" << std::setw(16) << std::setfill('0') << std::hex << m_CurrentIdentifier << std::dec << "]\n";
		m_Depth++;
	}

	void processChildData(const void* pBuffer, const uint64_t ui64BufferSize) override
	{
		for (int i = 0; i < m_Depth; i++) g_OutputStream << "   ";
		if (m_CurrentIdentifier == EBML_Identifier_DocType)
			g_OutputStream << "Got doc type : [" << m_ReaderHelper.getASCIIStringFromChildData(pBuffer, ui64BufferSize) << "]\n";
		else if (m_CurrentIdentifier == EBML_Identifier_EBMLVersion)
			g_OutputStream << "Got EBML version : [0x" << std::setw(16) << std::setfill('0') << std::hex << m_ReaderHelper.getUIntegerFromChildData(pBuffer, ui64BufferSize) << std::dec << "]\n";
		else if (m_CurrentIdentifier == EBML_Identifier_EBMLIdLength)
			g_OutputStream << "Got EBML ID length : [0x" << std::setw(16) << std::setfill('0') << std::hex << m_ReaderHelper.getUIntegerFromChildData(pBuffer, ui64BufferSize) << std::dec << "]\n";
		else if (m_CurrentIdentifier == EBML_Identifier_DocTypeVersion)
			g_OutputStream << "Got doc type version : [0x" << std::setw(16) << std::setfill('0') << std::hex << m_ReaderHelper.getUIntegerFromChildData(pBuffer, ui64BufferSize) << std::dec << "]\n";
		else if (m_CurrentIdentifier == EBML_Identifier_DocTypeReadVersion)
			g_OutputStream << "Got doc type read version : [0x" << std::setw(16) << std::setfill('0') << std::hex << m_ReaderHelper.getUIntegerFromChildData(pBuffer, ui64BufferSize) << std::dec << "]\n";
		else if (m_CurrentIdentifier == EBML::CIdentifier(0x1234))
			g_OutputStream << "Got uinteger : [0x" << std::setw(16) << std::setfill('0') << std::hex << m_ReaderHelper.getUIntegerFromChildData(pBuffer, ui64BufferSize) << std::dec << "]\n";
		else if (m_CurrentIdentifier == EBML::CIdentifier(0xffffffffffffffffLL))
			g_OutputStream << "Got uinteger : [0x" << std::setw(16) << std::setfill('0') << std::hex << m_ReaderHelper.getUIntegerFromChildData(pBuffer, ui64BufferSize) << std::dec << "]\n";
		else if (m_CurrentIdentifier == EBML::CIdentifier(0x4321))
			g_OutputStream << "Got double : [" << m_ReaderHelper.getFloatFromChildData(pBuffer, ui64BufferSize) << "]\n";
		else if (m_CurrentIdentifier == EBML::CIdentifier(0x8765))
			g_OutputStream << "Got float : [" << m_ReaderHelper.getFloatFromChildData(pBuffer, ui64BufferSize) << "]\n";
		else
			g_OutputStream << "Got " << ui64BufferSize << " data bytes, node id not known\n";
	}

	void closeChild(void) override
	{
		m_Depth--;
		for (int i = 0; i < m_Depth; i++) g_OutputStream << "   ";
		g_OutputStream << "Node closed\n";
	}

private:

	int m_Depth;

	EBML::CReaderHelper m_ReaderHelper;
	EBML::CIdentifier m_CurrentIdentifier;
};

int uoEBMLReaderTest(int argc, char* argv[])
{
	OVT_ASSERT(argc == 3, "Failure to retrieve tests arguments. Expecting: data_dir output_dir");

	std::string dataFile     = std::string(argv[1]) + "ref_data.ebml";
	std::string expectedFile = std::string(argv[1]) + "ref_result.txt";
	std::string outputFile   = std::string(argv[2]) + "uoEBMLReaderTest.txt";

	// The test parses a known ebml file,
	// writes the results into a text file and compares the output
	// text file to a reference text file.

	g_OutputStream.open(outputFile);


	OVT_ASSERT(g_OutputStream.is_open(), "Failure to open output file for writing");

	// parsing
	for (unsigned long n = 17; n >= 1; n--)
	{
		CReaderCallBack callback;
		EBML::CReader reader(callback);

		g_OutputStream << "testing with n=" << n << std::endl;


		FILE* file = fopen(dataFile.c_str(), "rb");

		OVT_ASSERT(file != nullptr, "Failure to open data file for reading");

		unsigned char* c = new unsigned char[n];
		size_t i         = 0;
		while (!feof(file))
		{
			i = fread(c, 1, n * sizeof(unsigned char), file);
			reader.processData(c, i);
		}
		delete[] c;
		fclose(file);
	}

	g_OutputStream.close();

	// comparison part
	std::ifstream generatedStream(outputFile);
	std::ifstream expectedStream(expectedFile);

	OVT_ASSERT(generatedStream.is_open(), "Failure to open generated results for reading");
	OVT_ASSERT(expectedStream.is_open(), "Failure to open expected results for reading");

	std::string generatedString;
	std::string expectedString;

	while (std::getline(expectedStream, expectedString))
	{
		OVT_ASSERT(std::getline(generatedStream, generatedString), "Failure to retrieve a line to match");
		OVT_ASSERT_STREQ(expectedString, generatedString, "Failure to match expected line to generated line");
	}

	// last check to verify the expected file has no additional line
	OVT_ASSERT(!std::getline(generatedStream, generatedString), "Failure to match expected file size and generated file size");


	return EXIT_SUCCESS;
}
