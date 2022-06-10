///-------------------------------------------------------------------------------------------------
/// 
/// \brief Tests for Files with UTF Format.
/// \author Thibaut Monseigne (Inria).
/// \version 1.0.
/// \date 10/06/2022.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/">GNU Affero General Public License v3.0</a>.
/// 
///-------------------------------------------------------------------------------------------------

#pragma once

#include "gtest/gtest.h"
#include <fs/Files.h>

//---------------------------------------------------------------------------------------------------
class Test_Files_UTF : public testing::Test
{
protected:
	const std::string m_outputDir               = TMP_DIR R"(/オッペﾝヴィベ)";
	const std::string m_outputDirCopy           = TMP_DIR R"(/オッペﾝヴィベ_Copy)";
	const std::string m_outputASCIIFilePath     = m_outputDir + R"(/file.txt)";
	const std::string m_outputUtfFilePath       = m_outputDir + R"(/日本語.txt)";
	const std::string m_outputUtfFilePathCopy   = m_outputDirCopy + R"(/日本語.txt)";
	const std::string m_outputASCIIFilePathCopy = m_outputDirCopy + R"(/file.txt)";
	const std::string m_asciiFilePath           = DATA_DIR R"(/オッペﾝヴィベ/file.txt)";
	const std::string m_utfFilePath             = DATA_DIR R"(/オッペﾝヴィベ/日本語.txt)";
	const std::string m_asciiDir                = DATA_DIR R"(/オッペﾝヴィベ)";

	void SetUp() override { }
};

//---------------------------------------------------------------------------------------------------
TEST_F(Test_Files_UTF, FileExists)
{
	EXPECT_TRUE(FS::Files::FileExists(m_asciiFilePath)) << "ASCII File not found";
	EXPECT_TRUE(FS::Files::FileExists(m_utfFilePath)) << "UTF File not found";
	EXPECT_FALSE(FS::Files::FileExists(DATA_DIR R"(/オッペﾝヴィベ/file)")) << "False ASCII File found";
	EXPECT_FALSE(FS::Files::FileExists(DATA_DIR R"(/オッペﾝヴィベ/日本語)")) << "False UTF File found";
}

//---------------------------------------------------------------------------------------------------
TEST_F(Test_Files_UTF, DirectoryExists)
{
	EXPECT_FALSE(FS::Files::DirectoryExists(DATA_DIR R"(/inexistent)")) << "False Directory found";
	EXPECT_TRUE(FS::Files::DirectoryExists(DATA_DIR R"(/オッペﾝヴィベ)")) << "Directory not found";
}

//---------------------------------------------------------------------------------------------------
TEST_F(Test_Files_UTF, GetParentPath)
{
	EXPECT_TRUE(m_asciiDir == FS::Files::GetParentPath(m_asciiFilePath)) << "Good Parent Path found";
}
