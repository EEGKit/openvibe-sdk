#pragma once

#include "defines.h"
#include <cstdio>
#include <string>
#include <fstream>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/replace.hpp>

namespace FS {
class FS_API Files
{
public:
	//------------------------------------------------------
	//---------------------- Openning ----------------------
	//------------------------------------------------------

	/// <summary> Opens std::ofstream. </summary>
	/// <param name="stream">The stream.</param>
	/// <param name="file"> The file. </param>
	/// <param name="mode"> The mode. </param>
	///	<remarks> Before Opening Filename is converted utf8 to utf16. Because mix of char*, std::string and other cause issue needed to pass to wstring on windows. </remarks>
	static void OpenOFStream(std::ofstream& stream, const std::string& file, std::ios_base::openmode mode = std::ios_base::out);


	/// <summary> Opens std::ifstream. </summary>
	/// <param name="stream"> The stream. </param>
	/// <param name="file"> The file. </param>
	/// <param name="mode"> The mode. </param>
	///	<remarks> Before Opening Filename is converted utf8 to utf16. Because mix of char*, std::string and other cause issue needed to pass to wstring on windows. </remarks>
	static void OpenIFStream(std::ifstream& stream, const std::string& file, std::ios_base::openmode mode = std::ios_base::in);


	/// <summary> Opens a std::fstream. </summary>
	/// <param name="stream">The stream.</param>
	/// <param name="file"> The file. </param>
	/// <param name="mode"> The mode. </param>
	static void OpenFStream(std::fstream& stream, const std::string& file, std::ios_base::openmode mode);

	/// <summary> Opens the specified file. </summary>
	/// <param name="file"> The file. </param>
	/// <param name="mode"> The mode. </param>
	/// <returns> <c>FILE</c> object openend. </returns>
	/// \deprecated Use (i|o)fstream method instead.
	OV_Deprecated("Use (i|o)fstream method instead.")
	static FILE* open(const char* file, const char* mode);

	/// <summary> popens the specified file. </summary>
	/// <param name="file"> The file. </param>
	/// <param name="mode"> The mode. </param>
	/// <returns> <c>FILE</c> object openend. </returns>
	/// \deprecated Use (i|o)fstream method instead.
	OV_Deprecated("Use (i|o)fstream method instead.")
	static FILE* popen(const char* file, const char* mode);

	/// <summary> Opens std::ofstream. </summary>
	/// <param name="stream">The stream.</param>
	/// <param name="file"> The file. </param>
	/// <param name="mode"> The mode. </param>
	///	<remarks> Before Opening Filename is converted utf8 to utf16. Because mix of char*, std::string and other cause issue needed to pass to wstring on windows. </remarks>
	/// \deprecated Use the same method with std::string parameter instead.
	OV_Deprecated("Use the same method with std::string parameter instead.")
	static void openOFStream(std::ofstream& stream, const char* file, std::ios_base::openmode mode = std::ios_base::out);


	/// <summary> Opens std::ifstream. </summary>
	/// <param name="stream"> The stream. </param>
	/// <param name="file"> The file. </param>
	/// <param name="mode"> The mode. </param>
	///	<remarks> Before Opening Filename is converted utf8 to utf16. Because mix of char*, std::string and other cause issue needed to pass to wstring on windows. </remarks>
	/// \deprecated Use the same method with std::string parameter instead.
	OV_Deprecated("Use the same method with std::string parameter instead.")
	static void openIFStream(std::ifstream& stream, const char* file, std::ios_base::openmode mode = std::ios_base::in);


	/// <summary> Opens a std::fstream. </summary>
	/// <param name="stream">The stream.</param>
	/// <param name="file"> The file. </param>
	/// <param name="mode"> The mode. </param>
	///	<remarks> Before Opening Filename is converted utf8 to utf16. Because mix of char*, std::string and other cause issue needed to pass to wstring on windows. </remarks>
	/// \deprecated Use the same method with std::string parameter instead.
	OV_Deprecated("Use the same method with std::string parameter instead.")
	static void openFStream(std::fstream& stream, const char* file, std::ios_base::openmode mode);


	//---------------------------------------------------
	//---------------------- Check ----------------------
	//---------------------------------------------------

	/// <summary> Check if files the exists. </summary>
	/// <param name="file">The path to check.</param>
	/// <returns> <c>true</c> if exist, <c>false</c> otherwise. </returns>
	static bool FileExists(const std::string& file) { return boost::filesystem::exists(toBoostPath(file)); }

	/// <summary> Directories the exists. </summary>
	/// <param name="dir"> The path to check. </param>
	/// <returns> <c>true</c> if exist, <c>false</c> otherwise. </returns>
	static bool DirectoryExists(const std::string& dir) { return boost::filesystem::is_directory(toBoostPath(dir)); }

	/// <summary> Compare the information of 2 files. </summary>
	/// <param name="file1">The first file.</param>
	/// <param name="file2">The second file.</param>
	/// <returns> <c>true</c> if equals, <c>false</c> otherwise. </returns>
	/// \deprecated Use the same method with std::string parameter instead.
	OV_Deprecated("Use the same method with std::string parameter instead.")
	static bool equals(const char* file1, const char* file2);


	/// <summary> Check if files the exists. </summary>
	/// <param name="pathToCheck">The path to check.</param>
	/// <returns> <c>true</c> if exist, <c>false</c> otherwise or if path is pre-allocated. </returns>
	/// \deprecated Use the same method with std::string parameter instead.
	OV_Deprecated("Use the same method with std::string parameter instead.")
	static bool fileExists(const char* pathToCheck);

	/// <summary> Directories the exists. </summary>
	/// <param name="pathToCheck"> The path to check. </param>
	/// <returns> <c>true</c> if exist, <c>false</c> otherwise or if path is pre-allocated. </returns>
	/// \deprecated Use the same method with std::string parameter instead.
	OV_Deprecated("Use the same method with std::string parameter instead.")
	static bool directoryExists(const char* pathToCheck);


	//----------------------------------------------------
	//---------------------- Create ----------------------
	//----------------------------------------------------

	/// <summary> Creates all components of a path to the filesystem </summary>
	/// <param name="path">The path.</param>
	/// <returns> <c>true</c> if succeed, <c>false</c> otherwise or empty path. </returns>
	static bool CreatePath(const std::string& path) { return path.empty() ? false : boost::filesystem::create_directories(toBoostPath(path)); }

	/// <summary> Creates all components of a path to the filesystem except the last part (i.e. for paths including a filename in the end). </summary>
	/// <param name="path">The path.</param>
	/// <returns> <c>true</c> if succeed, <c>false</c> otherwise or empty path. </returns>
	static bool CreateParentPath(const std::string& path)
	{
		return path.empty() ? false : boost::filesystem::create_directories(toBoostPath(path).parent_path());
	}

	/// <summary> Creates all components of a path to the filesystem </summary>
	/// <param name="path">The path.</param>
	/// <returns> <c>true</c> if succeed, <c>false</c> otherwise or empty path. </returns>
	/// \deprecated Use the same method with std::string parameter instead.
	OV_Deprecated("Use the same method with std::string parameter instead.")
	static bool createPath(const char* path);


	/// <summary> Creates all components of a path to the filesystem except the last part (i.e. for paths including a filename in the end). </summary>
	/// <param name="path">The path.</param>
	/// <returns> <c>true</c> if succeed, <c>false</c> otherwise or empty path. </returns>
	/// \deprecated Use the same method with std::string parameter instead.
	OV_Deprecated("Use the same method with std::string parameter instead.")
	static bool createParentPath(const char* path);

	//----------------------------------------------------
	//---------------------- Getter ----------------------
	//----------------------------------------------------

	/// <summary> Return filename. </summary>
	/// <param name="path"> The path. </param>
	/// <returns> The filename. </returns>
	static std::string GetParentPath(const std::string& path) { return toBoostPath(path).parent_path().string(); }

	/// <summary> Return filename. </summary>
	/// <param name="path"> The path. </param>
	/// <returns> The filename. </returns>
	static std::string GetFilename(const std::string& path) { return toBoostPath(path).filename().string(); }

	/// <summary> Return filename without the extension. </summary>
	/// <param name="path"> The path. </param>
	/// <returns> The filename without the extension. </returns>
	static std::string GetFilenameWithoutExtension(const std::string& path) { return toBoostPath(path).filename().replace_extension("").string(); }

	/// <summary> Gets the filename extension. </summary>
	/// <param name="path"> The path to test. </param>
	/// <returns> the extension at end of the path. </returns>
	static std::string GetFilenameExtension(const std::string& path) { return toBoostPath(path).extension().string(); }

	// Returns a path omitting the last part of it (essentially boost::filesystem::parent_path). Output sParentPath needs to be pre-allocated.
	/// <summary>
	/// Gets the parent path.
	/// </summary>
	/// <param name="path">The path.</param>
	/// <param name="parentPath">The parent path.</param>
	/// <returns> <c>true</c> if input arguments are pre-allocated. </returns>
	/// \deprecated Use the same method with std::string parameter instead.
	OV_Deprecated("Use the same method with std::string parameter instead.")
	static bool getParentPath(const char* path, char* parentPath);

	/// <summary>
	/// Gets the parent path.
	/// </summary>
	/// <param name="path">The path.</param>
	/// <param name="parentPath">The parent path.</param>
	/// <param name="size">The size.</param>
	/// <returns> <c>true</c> if input arguments are pre-allocated. </returns>
	/// \deprecated Use the same method with std::string parameter instead.
	OV_Deprecated("Use the same method with std::string parameter instead.")
	static bool getParentPath(const char* path, char* parentPath, size_t size);


	/// <summary> Fills filename as a component of a path. </summary>
	/// <param name="path">The path.</param>
	/// <param name="filename"> The filename, needs to be pre-allocated. </param>
	/// <returns> <c>true</c> if input arguments are pre-allocated. </returns>
	/// \deprecated Use the same method with std::string parameter instead.
	OV_Deprecated("Use the same method with std::string parameter instead.")
	static bool getFilename(const char* path, char* filename);

	/// <summary> Fills filename as a component of a path. </summary>
	/// <param name="path">The path.</param>
	/// <param name="filename"> The filename, needs to be pre-allocated. </param>
	/// <param name="size"> The size. </param>
	/// <returns> <c>true</c> if input arguments are pre-allocated. </returns>
	/// \deprecated Use the same method with std::string parameter instead.
	OV_Deprecated("Use the same method with std::string parameter instead.")
	static bool getFilename(const char* path, char* filename, size_t size);

	/// <summary> Fills filename as a component of a path without the extension. </summary>
	/// <param name="path"> The path. </param>
	/// <param name="filename"> The filename without extension, needs to be pre-allocated. </param>
	/// <returns> <c>true</c> if input arguments are pre-allocated. </returns>
	/// \deprecated Use the same method with std::string parameter instead.
	OV_Deprecated("Use the same method with std::string parameter instead.")
	static bool getFilenameWithoutExtension(const char* path, char* filename);

	/// <summary> Fills filename as a component of a path without the extension. </summary>
	/// <param name="path"> The path. </param>
	/// <param name="filename"> The filename without extension, needs to be pre-allocated. </param>
	/// <param name="size"> The size of the filename. </param>
	/// <returns> <c>true</c> if input arguments are pre-allocated. </returns>
	/// \deprecated Use the same method with std::string parameter instead.
	OV_Deprecated("Use the same method with std::string parameter instead.")
	static bool getFilenameWithoutExtension(const char* path, char* filename, size_t size);

	/// <summary> Gets the filename extension. </summary>
	/// <param name="path"> The path. </param>
	/// <param name="extension"> The filename extension, needs to be pre-allocated. </param>
	/// <returns> <c>true</c> if input arguments are pre-allocated. </returns>
	/// \deprecated Use <c>std::string GetFilenameExtension(const std::string& path)</c> method instead.
	OV_Deprecated("Use std::string GetFilenameExtension(const std::string& path) method instead.")
	static bool getFilenameExtension(const char* path, char* extension);

	/// <summary> Gets the filename extension. </summary>
	/// <param name="path"> The path. </param>
	/// <param name="extension"> The extension, needs to be pre-allocated. </param>
	/// <param name="size"> The size of extension. </param>
	/// <returns> <c>true</c> if input arguments are pre-allocated. </returns>
	/// \deprecated Use <c>std::string GetFilenameExtension(const std::string& path)</c> method instead.
	OV_Deprecated("Use std::string GetFilenameExtension(const std::string& path) method instead.")
	static bool getFilenameExtension(const char* path, char* extension, size_t size);

	//--------------------------------------------------
	//---------------------- Copy ----------------------
	//--------------------------------------------------

	/// <summary> Copies the file (Use boost::filesystem::copy_file). </summary>
	/// <param name="srcFile"> The source file. </param>
	/// <param name="dstPath"> The target path. </param>
	/// <returns> <c>true</c> if input arguments are pre-allocated. </returns>
	///	<remarks> Before Opening arguments are converted utf8 to utf16. Because mix of char*, std::string and other cause issue needed to pass to wstring on windows. </remarks>
	static void Copyfile(const std::string& srcFile, const std::string& dstPath) { boost::filesystem::copy_file(toBoostPath(srcFile), toBoostPath(dstPath)); }

	/// <summary> Copies the directory and his contents. </summary>
	/// <param name="srcDir"> The source dir. </param>
	/// <param name="dstDir"> The target dir. </param>
	/// <returns> <c>true</c> if input arguments are pre-allocated. </returns>
	///	<remarks> Before Opening arguments are converted utf8 to utf16. Because mix of char*, std::string and other cause issue needed to pass to wstring on windows. </remarks>
	static bool CopyDirectory(const std::string& srcDir, const std::string& dstDir) { return recursiveCopy(toBoostPath(srcDir), toBoostPath(dstDir)); }

	/// <summary> Copies the file (Use boost::filesystem::copy_file). </summary>
	/// <param name="srcFile"> The source file. </param>
	/// <param name="dstPath"> The target path. </param>
	/// <returns> <c>true</c> if input arguments are pre-allocated. </returns>
	///	<remarks> Before Opening arguments are converted utf8 to utf16. Because mix of char*, std::string and other cause issue needed to pass to wstring on windows. </remarks>
	/// \deprecated Use the same method with std::string parameter instead.
	OV_Deprecated("Use the same method with std::string parameter instead.")
	static bool copyFile(const char* srcFile, const char* dstPath);

	/// <summary> Copies the directory and his contents. </summary>
	/// <param name="srcDir"> The source dir. </param>
	/// <param name="dstDir"> The target dir. </param>
	/// <returns> <c>true</c> if input arguments are pre-allocated. </returns>
	///	<remarks> Before Opening arguments are converted utf8 to utf16. Because mix of char*, std::string and other cause issue needed to pass to wstring on windows. </remarks>
	/// \deprecated Use the same method with std::string parameter instead.
	OV_Deprecated("Use the same method with std::string parameter instead.")
	static bool copyDirectory(const char* srcDir, const char* dstDir);

	//----------------------------------------------------
	//---------------------- Remove ----------------------
	//----------------------------------------------------

	/// <summary> Use boost::filesystem::remove function. </summary>
	/// <param name="path"> The path. </param>
	/// <returns> boost::filesystem::remove status. </returns>
	///	<remarks> Before Opening path is converted utf8 to utf16. Because mix of char*, std::string and other cause issue needed to pass to wstring on windows. </remarks>
	static bool Remove(const std::string& path) { return boost::filesystem::remove(toBoostPath(path)); }

	/// <summary> Use boost::filesystem::remove_all function. </summary>
	/// <param name="path"> The path. </param>
	/// <returns> <c>true</c> if succeed, <c>false</c> otherwise. </returns>
	///	<remarks> Before Opening Filename is converted utf8 to utf16. Because mix of char*, std::string and other cause issue needed to pass to wstring on windows. </remarks>
	static bool RemoveAll(const std::string& path) { return boost::filesystem::remove_all(toBoostPath(path)) != 0; }

	/// <summary> Use boost::filesystem::remove function. </summary>
	/// <param name="path"> The path. </param>
	/// <returns> boost::filesystem::remove status. </returns>
	///	<remarks> Before Opening path is converted utf8 to utf16. Because mix of char*, std::string and other cause issue needed to pass to wstring on windows. </remarks>
	/// \deprecated Use the same method with std::string parameter instead.
	OV_Deprecated("Use the same method with std::string parameter instead.")
	static bool remove(const char* path);

	/// <summary> Use boost::filesystem::remove_all function. </summary>
	/// <param name="path"> The path. </param>
	/// <returns> <c>true</c> if succeed, <c>false</c> otherwise. </returns>
	///	<remarks> Before Opening Filename is converted utf8 to utf16. Because mix of char*, std::string and other cause issue needed to pass to wstring on windows. </remarks>
	/// \deprecated Use the same method with std::string parameter instead.
	OV_Deprecated("Use the same method with std::string parameter instead.")
	static bool removeAll(const char* path);

private:
	/// <summary> Convert string to boost path but convert in utf16 before. </summary>
	/// <param name="path">The path.</param>
	/// <returns> <c>boost::filesystem::path</c>. </returns>
	static boost::filesystem::path toBoostPath(const std::string& path);

	/// <summary> Makes a recursive copy of source folder to target folder.
	/// Operation can fail in several cases: target path exists and bad permission rights. </summary>
	/// <param name="source"> the source folder path. </param>
	/// <param name="target"> the destination folder path. </param>
	/// <returns> <c>true</c> if succeed, <c>false</c> otherwise. </returns>
	static bool recursiveCopy(const boost::filesystem::path& source, const boost::filesystem::path& target);

	Files() = delete;	///< Prevents a default instance of the <see cref="Files"/> class from being created.
};
}  // namespace FS
