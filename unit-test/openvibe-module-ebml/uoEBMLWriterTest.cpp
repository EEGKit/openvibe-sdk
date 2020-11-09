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

#include <iomanip>
#include <fstream>
#include <cstdio>

#include "ebml/defines.h"
#include "ebml/IWriter.h"
#include "ebml/CWriterHelper.h"

#include "ovtAssert.h"
#include <iostream>

#define OVP_NodeId_OpenViBEStream_Header              EBML::CIdentifier(0xF59505AB, 0x3684C8D8)
#define OVP_NodeId_OpenViBEStream_Header_Compression  EBML::CIdentifier(0x40358769, 0x166380D1)
#define OVP_NodeId_OpenViBEStream_Header_StreamType   EBML::CIdentifier(0x732EC1D1, 0xFE904087)
#define OVP_NodeId_OpenViBEStream_Buffer              EBML::CIdentifier(0x2E60AD18, 0x87A29BDF)
#define OVP_NodeId_OpenViBEStream_Buffer_StreamIndex  EBML::CIdentifier(0x30A56D8A, 0xB9C12238)
#define OVP_NodeId_OpenViBEStream_Buffer_StartTime    EBML::CIdentifier(0x093E6A0A, 0xC5A9467B)
#define OVP_NodeId_OpenViBEStream_Buffer_EndTime      EBML::CIdentifier(0x8B5CCCD9, 0xC5024F29)
#define OVP_NodeId_OpenViBEStream_Buffer_Content      EBML::CIdentifier(0x8D4B0BE8, 0x7051265C)

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

class CWriterCallBack : public EBML::IWriterCallBack
{
public:
	CWriterCallBack(const char* filename) { m_file = std::fopen(filename, "wb"); }

	~CWriterCallBack() override { if (m_file) { std::fclose(m_file); } }	// in case release is not called

	void write(const void* buffer, const size_t size) override { if (m_file) { std::fwrite(buffer, size_t(size), 1, m_file); } }

	// necessary thjor the test to close the stream and re-open it for inspection
	void release()
	{
		std::fclose(m_file);
		m_file = nullptr;
	}

private:

	std::FILE* m_file{ nullptr };
};

int uoEBMLWriterTest(int argc, char* argv[])
{
	OVT_ASSERT(argc == 3, "Failure to retrieve tests arguments. Expecting: data_dir output_dir");

	const std::string expectedFile = std::string(argv[1]) + "ref_data.ebml";
	const std::string outputFile   = std::string(argv[2]) + "uoEBMLWriterTest.ebml";

	// The test serializes a known ebml sequence and compares the output
	// to a reference.

	// serializing
	CWriterCallBack callback(outputFile.c_str());

	EBML::IWriter* writer = createWriter(callback);
	EBML::CWriterHelper helper;

	helper.connect(writer);

	helper.openChild(EBML_Identifier_Header);

	helper.openChild(EBML_Identifier_DocType);
	helper.setStr("matroska");
	helper.closeChild();

	helper.openChild(EBML_Identifier_DocTypeVersion);
	helper.setUInt(1);
	helper.closeChild();

	helper.openChild(EBML_Identifier_DocTypeReadVersion);
	helper.setInt(655356);
	helper.closeChild();

	helper.closeChild();

	helper.openChild(0x1234);
	helper.setUInt(0);
	helper.closeChild();

	helper.openChild(0xffffffffffffffffLL);
	helper.setUInt(0xff000000ff000000LL);
	helper.closeChild();

	helper.openChild(0x4321);
	helper.setDouble(M_PI);
	helper.closeChild();

	helper.openChild(0x8765);
	helper.setFloat(float(M_PI));
	helper.closeChild();
	writer->release();
	callback.release();


	// comparison part
	std::ifstream generatedStream(outputFile, std::ios::binary);
	std::ifstream expectedStream(expectedFile, std::ios::binary);

	OVT_ASSERT(generatedStream.is_open(), "Failure to open generated ebml stream for reading");
	OVT_ASSERT(expectedStream.is_open(), "Failure to open reference stream for reading");

	char generatedChar;
	char expectedChar;

	while (expectedStream.get(expectedChar))
	{
		OVT_ASSERT(generatedStream.get(generatedChar), "Failure to retrieve a char to match");
		OVT_ASSERT(generatedChar == expectedChar, "Failure to match expected char to generated char");
	}

	// last check to verify the expected file has no additional line
	OVT_ASSERT(!generatedStream.get(generatedChar), "Failure to match expected file size and generated file size");

	return EXIT_SUCCESS;
}
