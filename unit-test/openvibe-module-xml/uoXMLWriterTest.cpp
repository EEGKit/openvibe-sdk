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
#include <cstdio>

#include "xml/IWriter.h"

#include "ovtAssert.h"


class CWriterCallBack : public XML::IWriterCallBack
{
public:

	CWriterCallBack(const char* filename)
	{
		m_File = std::fopen(filename, "wb");
	}

	virtual ~CWriterCallBack(void)
	{
		if (m_File)
		{
			std::fclose(m_File); // in case release is not called
		}
	}

	virtual void write(const char* outputData) override
	{
		if (m_File)
		{
			std::fputs(outputData, m_File);
		}
	}

	// necessary thjor the test to close the stream and re-open it for inspection
	void release()
	{
		std::fclose(m_File);
		m_File = nullptr;
	}

private:

	std::FILE* m_File{ nullptr };
};

int uoXMLWriterTest(int argc, char* argv[])
{
	OVT_ASSERT(argc == 3, "Failure to retrieve tests arguments. Expecting: data_dir output_dir");

	std::string expectedFile = std::string(argv[1]) + "ref_data.xml";
	std::string outputFile = std::string(argv[2]) + "uoXMLWriterTest.xml";

	// The test serializes a known xml sequence and compares the output
	// to a reference.

	// serializing
	CWriterCallBack writerCallback(outputFile.c_str());

	XML::IWriter* writer = XML::createWriter(writerCallback);
	
	writer->openChild("Document"); //!< Document Node
	writer->setAttribute("name", "test_reference");

	writer->openChild("NodeWithChildren"); //!< NodeWithChildren Node

	writer->openChild("ChildNodeWithData");
	writer->setChildData("child node data with more than 10 alphanumeric characters");
	writer->closeChild();

	writer->openChild("ChildNodeEmpty");
	writer->closeChild();

	writer->openChild("ChildNodeWithChildren");
	writer->openChild("ChildNodeEmpty");
	writer->closeChild();
	writer->closeChild();

	writer->closeChild(); //!< NodeWithChildren END

	writer->openChild("NodeWithData"); 
	writer->setAttribute("status", "hasData");
	writer->setChildData("node data with special characters <>,;:!?./&\"'(-_)=~#{[|`\\^@]}/*-+");
	writer->closeChild();

	writer->openChild("NodeEmptyWithNumber666");
	writer->setAttribute("status", "noData");
	writer->setAttribute("ref", "test");
	writer->closeChild();

	writer->closeChild(); //!< Document Node END

	writer->release();
	writerCallback.release();

	// comparison part
	std::ifstream generatedStream(outputFile);
	std::ifstream expectedStream(expectedFile);

	OVT_ASSERT(generatedStream.is_open(), "Failure to open generated xml stream for reading");
	OVT_ASSERT(expectedStream.is_open(), "Failure to open reference stream for reading");

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

