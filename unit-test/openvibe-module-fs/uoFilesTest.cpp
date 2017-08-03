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

#include <string>

#include <fs/Files.h>

#include "ovtAssert.h"

int uoFilesTest(int argc, char* argv[])
{
	std::string outputDirectory = argv[1];

	OVT_ASSERT(FS::Files::directoryExists(outputDirectory.c_str()), "Failure to find test data directory");

	std::string testDir = outputDirectory + "uoFilesTest";
	std::string testFile = testDir + "/uoFilesTest.txt";

	// test path creation

	FS::Files::createParentPath(testFile.c_str());
	OVT_ASSERT(FS::Files::directoryExists(testDir.c_str()), "Failure to create directory");
	OVT_ASSERT(!FS::Files::fileExists(testFile.c_str()), "Failure to create parent path only");

	// test path retrieval methods

	char parentPathFromFile[1024];
	FS::Files::getParentPath(testFile.c_str(), parentPathFromFile);
	OVT_ASSERT_STREQ(std::string(parentPathFromFile), testDir, "Failure to retrieve parent path from full path");

	char parentPathFromDir[1024];
	FS::Files::getParentPath(testDir.c_str(), parentPathFromDir);
	std::cout << parentPathFromDir << " " << outputDirectory << std::endl;
	OVT_ASSERT_STREQ((std::string(parentPathFromDir) + "/"), outputDirectory, "Failure to retrieve parent path from path with no slash");

	testDir += "/"; // adding a slash should now give testDir as parent
	FS::Files::getParentPath(testDir.c_str(), parentPathFromDir);
	OVT_ASSERT_STREQ((std::string(parentPathFromDir) + "/"), testDir, "Failure to retrieve parent path from path with slash");

	char filename[256];
	FS::Files::getFilename(testFile.c_str(), filename);
	OVT_ASSERT_STREQ(std::string(filename), std::string("uoFilesTest.txt"), "Failure to retrieve filename from full path");

	FS::Files::getFilenameWithoutExtension(testFile.c_str(), filename);
	OVT_ASSERT_STREQ(std::string(filename), std::string("uoFilesTest"), "Failure to retrieve filename with no extension from full path");

	// test file creation and opening

	std::ofstream ostream;
	FS::Files::openOFStream(ostream, testFile.c_str());
	OVT_ASSERT(FS::Files::fileExists(testFile.c_str()), "Failure to create file");
	OVT_ASSERT(ostream.is_open(), "Failure to open file");
	ostream.close();

	std::string testFileInMissingnDir = testDir + "/newDir/uoFilesTest.txt";
	FS::Files::openOFStream(ostream, testFileInMissingnDir.c_str());
	OVT_ASSERT(!FS::Files::fileExists(testFileInMissingnDir.c_str()), "Failure to check for non-existing file");

	std::ifstream istream;
	FS::Files::openIFStream(istream, testFile.c_str());
	OVT_ASSERT(istream.is_open(), "Failure to open file in an input stream");
	istream.close();

	std::fstream fstream;
	FS::Files::openFStream(fstream, testFile.c_str(), std::ios_base::out);
	OVT_ASSERT(fstream.is_open(), "Failure to open file in a generic stream");
	fstream.close();

	auto file = FS::Files::open(testFile.c_str(), "r");
	OVT_ASSERT(file != nullptr, "Failure to open file in a FILE object");

	file = FS::Files::open(testFileInMissingnDir.c_str(), "r");
	OVT_ASSERT(file == nullptr, "Failure to return NULL FILE object for non-existing file");

	testDir = outputDirectory + "uoFilesTest2/long spaced/path";
	FS::Files::createPath(testDir.c_str());
	OVT_ASSERT(FS::Files::directoryExists(testDir.c_str()), "Failure to create directory with path containing spaces");

	// test equality

	OVT_ASSERT(!FS::Files::equals(testFile.c_str(), testFileInMissingnDir.c_str()), "Failure to compare different files");
	OVT_ASSERT(FS::Files::equals(testFile.c_str(), testFile.c_str()), "Failure to compare same files");


	return EXIT_SUCCESS;
}

