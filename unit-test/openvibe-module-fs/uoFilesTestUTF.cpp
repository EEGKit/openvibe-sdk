#include <fs/Files.h>
#include <boost/filesystem.hpp>
#include <boost/version.hpp>

#include <gtest/gtest.h>

#include <m_ConverterUtf8.h>

#define TEMP_OUTPUT_DIR TMP_DIR "/オッペﾝヴィベ"
#define TEMP_OUTPUT_DIR_COPY TMP_DIR "/オッペﾝヴィベ_Copy"
#define TEMP_OUTPUT_ASCII_FILE_PATH TEMP_OUTPUT_DIR "/file.txt"
#define TEMP_OUTPUT_UTF_FILE_PATH TEMP_OUTPUT_DIR "/日本語.txt"
#define TEMP_OUTPUT_UTF_FILE_PATH_COPY TEMP_OUTPUT_DIR_COPY "/日本語.txt"
#define TEMP_OUTPUT_ASCII_FILE_PATH_COPY TEMP_OUTPUT_DIR_COPY "/file.txt"


#define TEST_ASCII_FILE_PATH DATA_DIR "/オッペﾝヴィベ/file.txt"
#define TEST_UTF_FILE_PATH DATA_DIR "/オッペﾝヴィベ/日本語.txt"
#define TEST_ASCII_DIR DATA_DIR "/オッペﾝヴィベ"


TEST(FS_Files_Test_Directories_UTF, validateFileExists)
{
	ASSERT_TRUE(FS::Files::FileExists(TEST_ASCII_FILE_PATH));
	ASSERT_TRUE(FS::Files::FileExists(TEST_UTF_FILE_PATH));
	ASSERT_FALSE(FS::Files::FileExists(DATA_DIR "/オッペﾝヴィベ/file"));
	ASSERT_FALSE(FS::Files::FileExists(DATA_DIR "/オッペﾝヴィベ/日本語"));
}

TEST(FS_Files_Test_Directories_UTF, validateDirectoryExists)
{
	ASSERT_FALSE(FS::Files::DirectoryExists(DATA_DIR "/inexistent"));
	ASSERT_TRUE(FS::Files::DirectoryExists(DATA_DIR "/オッペﾝヴィベ"));
}

TEST(FS_Files_Test_Directories_UTF, validateGetParentPath)
{
	ASSERT_STREQ(TEST_ASCII_DIR, FS::Files::GetParentPath(TEST_ASCII_FILE_PATH).c_str());
}

TEST(FS_Files_Test_Directories_UTF, validateGetFileName)
{
	ASSERT_STREQ("file.txt", FS::Files::GetFilename(TEST_ASCII_FILE_PATH).c_str());
	ASSERT_STREQ("日本語.txt", FS::Files::GetFilename(TEST_UTF_FILE_PATH).c_str());
}

TEST(FS_Files_Test_Directories_UTF, validateGetFileNameWithoutExtension)
{
	ASSERT_STREQ("file", FS::Files::GetFilenameWithoutExtension(TEST_ASCII_FILE_PATH).c_str());
	ASSERT_STREQ("日本語", FS::Files::GetFilenameWithoutExtension(TEST_UTF_FILE_PATH).c_str());
}

TEST(FS_Files_Test_Directories_UTF, validateGetFileNameExtension)
{
	ASSERT_STREQ(".txt", FS::Files::GetFilenameExtension(TEST_ASCII_FILE_PATH).c_str());
	ASSERT_STREQ(".txt", FS::Files::GetFilenameExtension(TEST_UTF_FILE_PATH).c_str());
}

TEST(FS_Files_Test_Directories_UTF, validateCreatePath)
{
	FS::Files::RemoveAll(TEMP_OUTPUT_DIR);
	ASSERT_FALSE(FS::Files::DirectoryExists(TEMP_OUTPUT_DIR));
	ASSERT_TRUE(FS::Files::CreatePath(TEMP_OUTPUT_DIR));
	ASSERT_TRUE(FS::Files::DirectoryExists(TEMP_OUTPUT_DIR));
}

TEST(FS_Files_Test_Directories_UTF, validateCreateParentPath)
{
	FS::Files::RemoveAll(TEMP_OUTPUT_DIR);
	ASSERT_FALSE(FS::Files::DirectoryExists(TEMP_OUTPUT_DIR));
	ASSERT_TRUE(FS::Files::CreateParentPath(TEMP_OUTPUT_DIR "/file.txt"));
	ASSERT_TRUE(FS::Files::DirectoryExists(TEMP_OUTPUT_DIR));
}

TEST(FS_Files_Test_Directories_UTF, validateCopyFile)
{
	FS::Files::RemoveAll(TEMP_OUTPUT_ASCII_FILE_PATH);
	FS::Files::CreateParentPath(TEMP_OUTPUT_ASCII_FILE_PATH);
	FS::Files::Copyfile(TEST_ASCII_FILE_PATH, TEMP_OUTPUT_ASCII_FILE_PATH);
	ASSERT_TRUE(FS::Files::FileExists(TEMP_OUTPUT_ASCII_FILE_PATH));
}

TEST(FS_Files_Test_Directories_UTF, validateCopyDirectory)
{
	ASSERT_TRUE(FS::Files::CopyDirectory(TEST_ASCII_DIR, TEMP_OUTPUT_DIR_COPY));
	ASSERT_TRUE(FS::Files::FileExists(TEMP_OUTPUT_UTF_FILE_PATH_COPY));
	ASSERT_TRUE(FS::Files::FileExists(TEMP_OUTPUT_ASCII_FILE_PATH_COPY));
}

int uoFSFilesTestUTF(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);

	::testing::GTEST_FLAG(filter) = "FS_Files_Test_Directories_UTF.*";
	return RUN_ALL_TESTS();
}
