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

#include <xml/IWriter.h>
#include <xml/IXMLHandler.h>

#include <fs/Files.h>

#include <gtest/gtest.h>
#include <boost/filesystem.hpp>

class CWriterCallBack : public XML::IWriterCallBack
{
public:

	CWriterCallBack(const char* filename) { m_File = FS::Files::open(filename, "wb"); }

	~CWriterCallBack() override
	{
		if (m_File)
		{
			std::fclose(m_File); // in case release is not called
		}
	}

	void write(const char* outputData) override
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

TEST(XML_Writer_Test_Case, validateWriter)
{
	std::string expectedFile = DATA_DIR "/ref_data.xml";
	std::string outputFile   = TEMP_DIR "/uoXMLWriterTest.xml";

	FS::Files::createPath(TEMP_DIR);
	ASSERT_TRUE(FS::Files::directoryExists(TEMP_DIR));

	FS::Files::removeAll(outputFile.c_str());
	ASSERT_FALSE(FS::Files::fileExists(outputFile.c_str()));

	// The test serializes a known xml sequence and compares the output
	// to a reference.

	// serializing
	CWriterCallBack writerCallback(outputFile.c_str());

	XML::IWriter* writer = createWriter(writerCallback);

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
	std::ifstream generatedStream;
	std::ifstream expectedStream;

	FS::Files::openIFStream(generatedStream, outputFile.c_str());
	FS::Files::openIFStream(expectedStream, expectedFile.c_str());

	ASSERT_TRUE(generatedStream.is_open());
	ASSERT_TRUE(expectedStream.is_open());

	std::string generatedString;
	std::string expectedString;

	while (std::getline(expectedStream, expectedString))
	{
		std::getline(generatedStream, generatedString);
		ASSERT_EQ(expectedString, generatedString);
	}

	// last check to verify the expected file has no additional line
	std::getline(generatedStream, generatedString);
}

TEST(XML_Writer_Test_Case, validateHandlerWriteToJapanesePath)
{
	std::string expectedFile = DATA_DIR "/日本語/ref_data_jp.xml";
	std::string outputFile   = TEMP_DIR "/オッペﾝヴィベ/日本語.xml";

	FS::Files::createPath(TEMP_DIR);
	ASSERT_TRUE(FS::Files::directoryExists(TEMP_DIR));

	FS::Files::removeAll(outputFile.c_str());
	ASSERT_FALSE(FS::Files::fileExists(outputFile.c_str()));

	XML::IXMLHandler* xmlHandler = XML::createXMLHandler();
	std::string testData         = "<Document name=\"日本語\"><Node>日本語 1</Node><Node>日本語 2</Node><Node>日本語 3</Node></Document>";
	XML::IXMLNode* rootNode      = xmlHandler->parseString(testData.c_str(), static_cast<uint32_t>(testData.size()));
	xmlHandler->writeXMLInFile(*rootNode, outputFile.c_str());

	// comparison part
	std::ifstream generatedStream;
	std::ifstream expectedStream;

	FS::Files::openIFStream(generatedStream, outputFile.c_str());
	FS::Files::openIFStream(expectedStream, expectedFile.c_str());

	ASSERT_TRUE(generatedStream.is_open());
	ASSERT_TRUE(expectedStream.is_open());

	std::string generatedString;
	std::string expectedString;

	while (std::getline(expectedStream, expectedString))
	{
		std::getline(generatedStream, generatedString);
		ASSERT_EQ(expectedString, generatedString);
	}

	// last check to verify the expected file has no additional line
	std::getline(generatedStream, generatedString);
}

int uoXMLWriterTest(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);

	::testing::GTEST_FLAG(filter) = "XML_Writer_Test_Case.*";
	return RUN_ALL_TESTS();
}
