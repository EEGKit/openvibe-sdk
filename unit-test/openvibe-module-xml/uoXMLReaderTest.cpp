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
#include <vector>
#include <map>
#include <memory>

#include "xml/IReader.h"
#include "xml/IXMLHandler.h"
#include <fs/Files.h>

#include <gtest/gtest.h>

class CReaderCallBack : public XML::IReaderCallBack
{
public:

	struct Node
	{
		std::string name;
		std::string data;
		std::map<std::string, std::string> attributes;
		std::vector<std::shared_ptr<Node>> children;
		std::shared_ptr<Node> parent{ nullptr };
	};

	std::shared_ptr<Node> currentNode{ nullptr };

protected:

	virtual void openChild(const char* name, const char** attributeName, const char** attributeValue, XML::uint64 attributeCount) override
	{
		auto node = std::make_shared<Node>();

		if (currentNode)
		{
			node->parent = currentNode;
			currentNode->children.push_back(node);
		}

		currentNode = node;
		currentNode->name = name;

		for (XML::uint64 i = 0; i<attributeCount; i++)
		{
			currentNode->attributes[attributeName[i]] = attributeValue[i];
		}
	}

	virtual void processChildData(const char* data) override
	{
		currentNode->data = data;
	}

	virtual void closeChild(void) override
	{
		if (currentNode->parent)
		{
			currentNode = currentNode->parent;
		}
	}
};

TEST(XML_Reader_Test_Case, validateReader)
{
	std::string dataFile = std::string(DATA_DIR) + "/ref_data.xml";

	CReaderCallBack readerCallback;
	XML::IReader* xmlReader = XML::createReader(readerCallback);

	FILE* inputTestDataFile = fopen(dataFile.c_str(), "r");

	ASSERT_NE(nullptr, inputTestDataFile);

	char dataBuffer[1024];
	while (!feof(inputTestDataFile))
	{
		size_t length = std::fread(dataBuffer, 1, sizeof(dataBuffer), inputTestDataFile);
		xmlReader->processData(dataBuffer, length);
	}
	std::fclose(inputTestDataFile);

	xmlReader->release();

	// Analyze results

	auto rootNode = readerCallback.currentNode;

	// Root node check
	ASSERT_EQ("Document", rootNode->name);
	ASSERT_EQ(1, rootNode->attributes.size());
	ASSERT_EQ("test_reference", rootNode->attributes["name"]);

	ASSERT_EQ(3, rootNode->children.size());

	auto complexChild = rootNode->children[0];
	auto dataChild = rootNode->children[1];
	auto emptyChild = rootNode->children[2];

	// Simple child with data check
	ASSERT_EQ("NodeWithData", dataChild->name);
	ASSERT_EQ("node data with special characters <>,;:!?./&\"'(-_)=~#{[|`\\^@]}/*-+", dataChild->data);
	ASSERT_EQ(1, dataChild->attributes.size());
	ASSERT_EQ("hasData", dataChild->attributes["status"]);

	// Simple child with no data and multiple attributes check
	ASSERT_EQ("NodeEmptyWithNumber666", emptyChild->name);
	ASSERT_EQ(2, emptyChild->attributes.size());
	ASSERT_EQ("noData", emptyChild->attributes["status"]);
	ASSERT_EQ("test", emptyChild->attributes["ref"]);

	// Complex child check
	ASSERT_EQ("NodeWithChildren", complexChild->name);
	ASSERT_TRUE(complexChild->attributes.empty());

	ASSERT_EQ(3, complexChild->children.size());

	dataChild = complexChild->children[0];
	emptyChild = complexChild->children[1];
	complexChild = complexChild->children[2];

	ASSERT_EQ("ChildNodeWithData", dataChild->name);
	ASSERT_EQ("child node data with more than 10 alphanumeric characters", dataChild->data);
	ASSERT_EQ(0, dataChild->attributes.size());

	ASSERT_EQ("ChildNodeEmpty", emptyChild->name);
	ASSERT_TRUE(emptyChild->attributes.empty());

	ASSERT_EQ("ChildNodeWithChildren", complexChild->name);
	ASSERT_TRUE(complexChild->attributes.empty());
	ASSERT_EQ(1, complexChild->children.size());
}

TEST(XML_Reader_Test_Case, validateHandlerReadJapanese)
{
	std::string dataFile = std::string(DATA_DIR) + "/日本語/ref_data_jp.xml";

	XML::IXMLHandler* xmlHandler = XML::createXMLHandler();
	XML::IXMLNode* rootNode = xmlHandler->parseFile(dataFile.c_str());

	ASSERT_NE(nullptr, rootNode);
	ASSERT_EQ(std::string("Document"), rootNode->getName());
	ASSERT_TRUE(rootNode->hasAttribute("name"));
	ASSERT_EQ(std::string("日本語"), rootNode->getAttribute("name"));
	ASSERT_EQ(3, rootNode->getChildCount());

	ASSERT_STREQ("日本語 1", rootNode->getChild(0)->getPCData());

	xmlHandler->release();
}

TEST(XML_Reader_Test_Case, validateHandlerReadFrench)
{
	std::string dataFile = std::string(DATA_DIR) + "/Français/ref_data_fr.xml";

	XML::IXMLHandler* xmlHandler = XML::createXMLHandler();
	XML::IXMLNode* rootNode = xmlHandler->parseFile(dataFile.c_str());

	ASSERT_NE(nullptr, rootNode);
	ASSERT_EQ(std::string("Document"), rootNode->getName());
	ASSERT_TRUE(rootNode->hasAttribute("name"));
	ASSERT_EQ(std::string("Français"), rootNode->getAttribute("name"));
	ASSERT_EQ(3, rootNode->getChildCount());

	ASSERT_STREQ("Français 1 (àèáéîôïç)", rootNode->getChild(0)->getPCData());

	xmlHandler->release();
}

TEST(XML_Reader_Test_Case, validateHandlerReadNBSP)
{
	std::string dataFile = std::string(DATA_DIR) + "/NB\xC2\xA0SP/ref_data_nbsp.xml";

	XML::IXMLHandler* xmlHandler = XML::createXMLHandler();
	XML::IXMLNode* rootNode = xmlHandler->parseFile(dataFile.c_str());

	ASSERT_NE(nullptr, rootNode);
	ASSERT_EQ(std::string("Document"), rootNode->getName());
	ASSERT_TRUE(rootNode->hasAttribute("name"));
	ASSERT_EQ(std::string("NB\xC2\xA0SP"), rootNode->getAttribute("name"));
	ASSERT_EQ(3, rootNode->getChildCount());

	ASSERT_STREQ("NB\xC2\xA0SP 1", rootNode->getChild(0)->getPCData());

	xmlHandler->release();
}

int uoXMLReaderTest(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);

	::testing::GTEST_FLAG(filter) = "XML_Reader_Test_Case.*";
	return RUN_ALL_TESTS();
}
