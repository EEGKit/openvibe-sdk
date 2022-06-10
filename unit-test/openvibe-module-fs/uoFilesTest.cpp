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
#include <iostream>

int uoFilesTest(int /*argc*/, char* argv[])
{
	std::string outputDirectory = argv[1];

	OVT_ASSERT(FS::Files::DirectoryExists(outputDirectory), "Failure to find test data directory");

	std::string testDir  = outputDirectory + "uoFilesTest";
	std::string testFile = testDir + "/uoFilesTest.txt";

	// test path creation

	FS::Files::CreateParentPath(testFile);
	OVT_ASSERT(FS::Files::DirectoryExists(testDir), "Failure to create directory");
	OVT_ASSERT(!FS::Files::FileExists(testFile), "Failure to create parent path only");

	// test path retrieval methods

	std::string parentPathFromFile = FS::Files::GetParentPath(testFile);
	OVT_ASSERT_STREQ(parentPathFromFile, testDir, "Failure to retrieve parent path from full path");

	std::string parentPathFromDir = FS::Files::GetParentPath(testDir);
	std::cout << parentPathFromDir << " " << outputDirectory << std::endl;
	OVT_ASSERT_STREQ((parentPathFromDir + "/"), outputDirectory, "Failure to retrieve parent path from path with no slash");

	testDir += "/"; // adding a slash should now give testDir as parent
	parentPathFromDir = FS::Files::GetParentPath(testDir);
	OVT_ASSERT_STREQ((parentPathFromDir + "/"), testDir, "Failure to retrieve parent path from path with slash");

	std::string filename = FS::Files::GetFilename(testFile);
	OVT_ASSERT_STREQ(filename, std::string("uoFilesTest.txt"), "Failure to retrieve filename from full path");

	filename = FS::Files::GetFilenameWithoutExtension(testFile);
	OVT_ASSERT_STREQ(std::string(filename), std::string("uoFilesTest"), "Failure to retrieve filename with no extension from full path");

	// test file creation and opening

	std::ofstream ostream;
	FS::Files::OpenOFStream(ostream, testFile);
	OVT_ASSERT(FS::Files::FileExists(testFile), "Failure to create file");
	OVT_ASSERT(ostream.is_open(), "Failure to open file");
	ostream.close();

	std::string testFileInMissingnDir = testDir + "/newDir/uoFilesTest.txt";
	FS::Files::OpenOFStream(ostream, testFileInMissingnDir);
	OVT_ASSERT(!FS::Files::FileExists(testFileInMissingnDir), "Failure to check for non-existing file");

	std::ifstream istream;
	FS::Files::OpenIFStream(istream, testFile);
	OVT_ASSERT(istream.is_open(), "Failure to open file in an input stream");
	istream.close();

	std::fstream fstream;
	FS::Files::OpenFStream(fstream, testFile, std::ios_base::out);
	OVT_ASSERT(fstream.is_open(), "Failure to open file in a generic stream");
	fstream.close();

	auto file = FS::Files::open(testFile.c_str(), "r");
	OVT_ASSERT(file != nullptr, "Failure to open file in a FILE object");

	file = FS::Files::open(testFileInMissingnDir.c_str(), "r");
	OVT_ASSERT(file == nullptr, "Failure to return NULL FILE object for non-existing file");

	testDir = outputDirectory + "uoFilesTest2/long spaced/path";
	FS::Files::CreatePath(testDir);
	OVT_ASSERT(FS::Files::DirectoryExists(testDir), "Failure to create directory with path containing spaces");

	// test equality

	OVT_ASSERT(!FS::Files::equals(testFile.c_str(), testFileInMissingnDir.c_str()), "Failure to compare different files");
	OVT_ASSERT(FS::Files::equals(testFile.c_str(), testFile.c_str()), "Failure to compare same files");


	// test folder copy
	std::string testFile2       = outputDirectory + "uoFilesTest" + "/uoFilesTestChild/uoFilesTest.txt";
	std::string testTargetDir   = outputDirectory + "uoFilesTestCopy";
	std::string testTargetFile1 = testTargetDir + "/uoFilesTest.txt";
	std::string testTargetFile2 = testTargetDir + "/uoFilesTestChild/uoFilesTest.txt";

	// create a subfolder with file
	FS::Files::CreateParentPath(testFile2);
	std::ofstream ostream2;
	FS::Files::OpenOFStream(ostream2, testFile2);
	OVT_ASSERT(FS::Files::FileExists(testFile2), "Failure to create file in subfolder");
	OVT_ASSERT(ostream2.is_open(), "Failure to open file");
	ostream2.close();

	// copy folder
	testDir = outputDirectory + "uoFilesTest";
	FS::Files::CopyDirectory(testDir, testTargetDir);
	OVT_ASSERT(FS::Files::DirectoryExists(testTargetDir), "Failure in copying folder");
	OVT_ASSERT(FS::Files::FileExists(testTargetFile1), "Failure in copying child files of the folder");
	OVT_ASSERT(FS::Files::FileExists(testTargetFile2), "Failure in copying subfolder");
	// test folder copy on existing folder
	OVT_ASSERT(!FS::Files::CopyDirectory(testDir, testTargetDir), "Failure: Copy should not have been done if folder exits");

	return EXIT_SUCCESS;
}
