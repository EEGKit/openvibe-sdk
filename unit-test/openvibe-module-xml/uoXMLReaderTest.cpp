/*********************************************************************
* Software License Agreement (AGPL-3 License)
*
* CertiViBE Test Software
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

#include "ovtAssert.h"

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

int uoXMLReaderTest(int argc, char* argv[])
{  
	OVT_ASSERT(argc == 2, "Failure to retrieve tests arguments. Expecting: data_dir");
	
	std::string dataFile = std::string(argv[1]) + "ref_data.xml";

	// Parse reference data that are well-known

	CReaderCallBack readerCallback;
	XML::IReader* xmlReader = XML::createReader(readerCallback);

	FILE* inputTestDataFile = fopen(dataFile.c_str(), "r");

	OVT_ASSERT(inputTestDataFile, "Failure to open data file");

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
	OVT_ASSERT_STREQ(rootNode->name, std::string("Document"), "Failure to retrieve root node name");
	OVT_ASSERT(rootNode->attributes.size() == 1, "Failure to retrieve root node attribute");
	OVT_ASSERT_STREQ(rootNode->attributes["name"], std::string("test_reference"), "Failure to retrieve root node attribute");

	OVT_ASSERT(rootNode->children.size() == 3, "Failure to retrieve root node children");

	auto complexChild = rootNode->children[0];
	auto dataChild = rootNode->children[1];
	auto emptyChild = rootNode->children[2];

	// Simple child with data check
	OVT_ASSERT_STREQ(dataChild->name, std::string("NodeWithData"), "Failure to retrieve data dataChild node name");
	OVT_ASSERT_STREQ(dataChild->data, std::string("node data with special characters <>,;:!?./&\"'(-_)=~#{[|`\\^@]}/*-+"), "Failure to retrieve dataChild node data");
	OVT_ASSERT(dataChild->attributes.size() == 1, "Failure to retrieve dataChild node attribute");
	OVT_ASSERT_STREQ(dataChild->attributes["status"], std::string("hasData"), "Failure to retrieve dataChild node attribute");

	// Simple child with no data and multiple attributes check
	OVT_ASSERT_STREQ(emptyChild->name, std::string("NodeEmptyWithNumber666"), "Failure to retrieve emptyChild node name");
	OVT_ASSERT(emptyChild->attributes.size() == 2, "Failure to retrieve emptyChild node attributes");
	OVT_ASSERT_STREQ(emptyChild->attributes["status"], std::string("noData"), "Failure to retrieve emptyChild node status attribute");
	OVT_ASSERT_STREQ(emptyChild->attributes["ref"], std::string("test"), "Failure to retrieve emptyChild node ref attribute");

	// Complex child check
	OVT_ASSERT_STREQ(complexChild->name, std::string("NodeWithChildren"), "Failure to retrieve complexChild node name");
	OVT_ASSERT(complexChild->attributes.empty(), "Failure to retrieve complexChild node attribute");

	OVT_ASSERT(complexChild->children.size() == 3, "Failure to retrieve complexChild node children");

	dataChild = complexChild->children[0];
	emptyChild = complexChild->children[1];
	complexChild = complexChild->children[2];

	OVT_ASSERT_STREQ(dataChild->name, std::string("ChildNodeWithData"), "Failure to retrieve data dataChild node name");
	OVT_ASSERT_STREQ(dataChild->data, std::string("child node data with more than 10 alphanumeric characters"), "Failure to retrieve dataChild node data");
	OVT_ASSERT(dataChild->attributes.size() == 0, "Failure to retrieve dataChild node attribute");

	OVT_ASSERT_STREQ(emptyChild->name, std::string("ChildNodeEmpty"), "Failure to retrieve emptyChild node name");
	OVT_ASSERT(emptyChild->attributes.empty(), "Failure to retrieve emptyChild node attribute");

	OVT_ASSERT_STREQ(complexChild->name, std::string("ChildNodeWithChildren"), "Failure to retrieve complexChild node name");
	OVT_ASSERT(complexChild->attributes.empty(), "Failure to retrieve complexChild node attribute");
	OVT_ASSERT(complexChild->children.size() == 1, "Failure to retrieve complexChild node children");
	
	return EXIT_SUCCESS;
}

