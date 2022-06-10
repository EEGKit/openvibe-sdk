#include "Files.h"

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
 #include <stdio.h>   // For fopen
 #include <unistd.h>  //For access().
 #include <sys/stat.h>
 #include <sys/types.h>  // For stat().
 #include <cstdlib>
#elif defined TARGET_OS_Windows
#include "m_ConverterUtf8.h"
#include <Windows.h>
#else
#endif

#include <boost/filesystem.hpp>
#include <boost/version.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <cstdio>
#include <cstring>

namespace FS {

//  * 2006-08-30 YRD - Portability note : using namespace FS confuses windows platform SDK because it defines itself a 'boolean' type. Thus the following define to force the use of FS::boolean !

//---------------------------------------------------------------------------------------------------
boost::filesystem::path Files::toBoostPath(const std::string& path)
{
#if defined TARGET_OS_Windows
	return boost::filesystem::wpath(Common::Converter::Utf8ToUtf16(path));
#else
		return boost::filesystem::path(path);
#endif
}

//---------------------------------------------------------------------------------------------------
bool Files::recursiveCopy(const boost::filesystem::path& source, const boost::filesystem::path& target)
{
	if (boost::filesystem::exists(target)) { return false; }

	if (boost::filesystem::is_directory(source)) {
		if (!create_directories(target)) { return false; }
		for (boost::filesystem::directory_entry& item : boost::filesystem::directory_iterator(source)) {
			// boost::filesystem::path overlods '/' operator !
			if (!recursiveCopy(item.path(), target / item.path().filename())) { return false; }
		}
	}
	else if (boost::filesystem::is_regular_file(source)) {
		try { boost::filesystem::copy(source, target); }
		catch (...) { return false; }
	}
	else { return false; }
	return true;
}

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
//---------------------------------------------------------------------------------------------------
FILE* Files::open(const char* file, const char* mode)
{
	FILE* handle = fopen(file, mode);
	return handle;
}

//---------------------------------------------------------------------------------------------------
FILE* Files::popen(const char* file, const char* mode)
{
	FILE* handle = ::popen(file, mode);
	return handle;
}

//---------------------------------------------------------------------------------------------------
void Files::OpenOFStream(std::ofstream& stream, const std::string& file, std::ios_base::openmode mode) { stream.open(file, mode); }
void Files::OpenIFStream(std::ifstream& stream, const std::string& file, std::ios_base::openmode mode) { stream.open(file, mode); }
void Files::OpenFStream(std::fstream& stream, const std::string& file, std::ios_base::openmode mode) { stream.open(file, mode); }

//---------------------------------------------------------------------------------------------------
void Files::openOFStream(std::ofstream& stream, const char* file, std::ios_base::openmode mode) { stream.open(file, mode); }
void Files::openIFStream(std::ifstream& stream, const char* file, std::ios_base::openmode mode) { stream.open(file, mode); }
void Files::openFStream(std::fstream& stream, const char* file, std::ios_base::openmode mode) { stream.open(file, mode); }

#elif defined TARGET_OS_Windows

//---------------------------------------------------------------------------------------------------
FILE* Files::open(const char* file, const char* mode)
{
	FILE* handle;

	try {
		const std::wstring utf16FileName = Common::Converter::Utf8ToUtf16(file);
		const std::wstring utf16Mode     = Common::Converter::Utf8ToUtf16(mode);

		handle = _wfopen(utf16FileName.c_str(), utf16Mode.c_str());
	}
	catch (const std::logic_error&) { handle = fopen(file, mode); }

	return handle;
}

//---------------------------------------------------------------------------------------------------
FILE* Files::popen(const char* file, const char* mode)
{
	FILE* handle;

	try {
		const std::wstring utf16FileName = Common::Converter::Utf8ToUtf16(file);
		const std::wstring utf16Mode     = Common::Converter::Utf8ToUtf16(mode);

		handle = _wpopen(utf16FileName.c_str(), utf16Mode.c_str());
	}
	catch (const std::logic_error&) { handle = popen(file, mode); }

	return handle;
}

//---------------------------------------------------------------------------------------------------
template <class T>
void openStream(T& stream, const std::string& file, std::ios_base::openmode mode)
{
	try {
		const std::wstring utf16FileName = Common::Converter::Utf8ToUtf16(file);
		stream.open(utf16FileName.c_str(), mode);
	}
	catch (const std::logic_error&) { stream.open(file, mode); }
}

//---------------------------------------------------------------------------------------------------
void Files::OpenOFStream(std::ofstream& stream, const std::string& file, const std::ios_base::openmode mode) { openStream<std::ofstream>(stream, file, mode); }
void Files::OpenIFStream(std::ifstream& stream, const std::string& file, const std::ios_base::openmode mode) { openStream<std::ifstream>(stream, file, mode); }
void Files::OpenFStream(std::fstream& stream, const std::string& file, const std::ios_base::openmode mode) { openStream<std::fstream>(stream, file, mode); }

//---------------------------------------------------------------------------------------------------
void Files::openOFStream(std::ofstream& stream, const char* file, const std::ios_base::openmode mode) { openStream<std::ofstream>(stream, file, mode); }
void Files::openIFStream(std::ifstream& stream, const char* file, const std::ios_base::openmode mode) { openStream<std::ifstream>(stream, file, mode); }
void Files::openFStream(std::fstream& stream, const char* file, const std::ios_base::openmode mode) { openStream<std::fstream>(stream, file, mode); }

#endif

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
//---------------------------------------------------------------------------------------------------
bool Files::equals(const char* file1, const char* file2)
{
	bool res=true;
	if(file1 && file2)
	{
		struct stat stat1;
		struct stat stat2;
		bool bStat1=!stat(file1, &stat1);
		bool bStat2=!stat(file2, &stat2);

		if(!bStat1 && !bStat2) { res=true; }
		else if(bStat1 && bStat2)
		{
			res=
				(stat1.st_dev==stat2.st_dev)&&
				(stat1.st_ino==stat2.st_ino)&&
				(stat1.st_size==stat2.st_size)&&
				(stat1.st_mtime==stat2.st_mtime);
		}
		else { res=false; }
	}
	return res;
}

#elif defined TARGET_OS_Windows
//---------------------------------------------------------------------------------------------------
bool Files::equals(const char* file1, const char* file2)
{
	bool res = true;
	if (file1 && file2) {
		const HANDLE handle1 = ::CreateFile(file1, 0, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
											FILE_FLAG_BACKUP_SEMANTICS, nullptr);
		const HANDLE handle2 = ::CreateFile(file2, 0, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING,
											FILE_FLAG_BACKUP_SEMANTICS, nullptr);
		if (handle1 && handle2) {
			BY_HANDLE_FILE_INFORMATION stat1;
			BY_HANDLE_FILE_INFORMATION stat2;
			const BOOL bStat1 = GetFileInformationByHandle(handle1, &stat1);
			const BOOL bStat2 = GetFileInformationByHandle(handle2, &stat2);
			if (!bStat1 && !bStat2) { res = true; }
			else if (bStat1 && bStat2) {
				res = (stat1.dwVolumeSerialNumber == stat2.dwVolumeSerialNumber) && (stat1.nFileIndexHigh == stat2.nFileIndexHigh)
					  && (stat1.nFileIndexLow == stat2.nFileIndexLow) && (stat1.nFileSizeHigh == stat2.nFileSizeHigh)
					  && (stat1.nFileSizeLow == stat2.nFileSizeLow) && (stat1.ftLastWriteTime.dwHighDateTime == stat2.ftLastWriteTime.dwHighDateTime)
					  && (stat1.ftLastWriteTime.dwLowDateTime == stat2.ftLastWriteTime.dwLowDateTime);
			}
			else { res = false; }
			CloseHandle(handle1);
			CloseHandle(handle2);
		}
		else {
			if (handle1) { CloseHandle(handle1); }
			if (handle2) { CloseHandle(handle2); }
		}
	}
	return res;
}

#else

#endif

//---------------------------------------------------------------------------------------------------
bool Files::fileExists(const char* pathToCheck)
{
	if (!pathToCheck) { return false; }
	FILE* fp = open(pathToCheck, "r");
	if (!fp) { return false; }
	fclose(fp);
	return true;
}

//---------------------------------------------------------------------------------------------------
bool Files::directoryExists(const char* pathToCheck)
{
	if (!pathToCheck) { return false; }
#if defined TARGET_OS_Windows
	const std::wstring pathUTF16 = Common::Converter::Utf8ToUtf16(pathToCheck);
	const DWORD ftyp             = GetFileAttributesW(pathUTF16.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES) { return false; }
	if (ftyp & FILE_ATTRIBUTE_DIRECTORY) { return true; }
#endif
#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	if ( access( pathToCheck, 0 ) == 0 ) {
		struct stat status;
		stat( pathToCheck, &status );
		if ( status.st_mode & S_IFDIR ) { return true; }
	} 
#endif
	return false;
}

//---------------------------------------------------------------------------------------------------
bool Files::createPath(const char* path)
{
	if (strcmp(path, "") == 0) { return false; }
	return boost::filesystem::create_directories(toBoostPath(path));
}

//---------------------------------------------------------------------------------------------------
bool Files::createParentPath(const char* path)
{
	if (strcmp(path, "") == 0) { return false; }
	return boost::filesystem::create_directories(toBoostPath(path).parent_path());
}

//---------------------------------------------------------------------------------------------------
bool Files::getParentPath(const char* path, char* parentPath)
{
	if (!path || !parentPath) { return false; }
	strcpy(parentPath, boost::filesystem::path(path).parent_path().string().c_str());
	return true;
}

//---------------------------------------------------------------------------------------------------
bool Files::getParentPath(const char* path, char* parentPath, const size_t size)
{
	if (!path || !parentPath) { return false; }
	strncpy(parentPath, boost::filesystem::path(path).parent_path().string().c_str(), size);
	return true;
}

//---------------------------------------------------------------------------------------------------
bool Files::getFilename(const char* path, char* filename)
{
	if (!path || !filename) { return false; }
	strcpy(filename, boost::filesystem::path(path).filename().string().c_str());
	return true;
}

//---------------------------------------------------------------------------------------------------
bool Files::getFilename(const char* path, char* filename, const size_t size)
{
	if (!path || !filename) { return false; }
	strncpy(filename, boost::filesystem::path(path).filename().string().c_str(), size);
	return true;
}

//---------------------------------------------------------------------------------------------------
bool Files::getFilenameWithoutExtension(const char* path, char* filename)
{
	if (!path || !filename) { return false; }
	strcpy(filename, boost::filesystem::path(path).filename().replace_extension("").string().c_str());
	return true;
}

//---------------------------------------------------------------------------------------------------
bool Files::getFilenameWithoutExtension(const char* path, char* filename, const size_t size)
{
	if (!path || !filename) { return false; }
	strncpy(filename, boost::filesystem::path(path).filename().replace_extension("").string().c_str(), size);
	return true;
}

//---------------------------------------------------------------------------------------------------
bool Files::getFilenameExtension(const char* path, char* extension)
{
	if (!path || !extension) { return false; }
	strcpy(extension, boost::filesystem::path(path).extension().string().c_str());
	return true;
}

//---------------------------------------------------------------------------------------------------
bool Files::getFilenameExtension(const char* path, char* extension, const size_t size)
{
	if (!path || !extension) { return false; }
	strncpy(extension, boost::filesystem::path(path).extension().string().c_str(), size);
	return true;
}

//---------------------------------------------------------------------------------------------------
bool Files::copyFile(const char* srcFile, const char* dstPath)
{
	if (!srcFile || !dstPath) { return false; }
	boost::filesystem::copy_file(toBoostPath(srcFile), toBoostPath(dstPath));
	return true;
}

//---------------------------------------------------------------------------------------------------
bool Files::copyDirectory(const char* srcDir, const char* dstDir)
{
	if (!srcDir || !dstDir) { return false; }
	return recursiveCopy(toBoostPath(srcDir), toBoostPath(dstDir));
}

//---------------------------------------------------------------------------------------------------
bool Files::remove(const char* path)
{
	if (fileExists(path) || directoryExists(path)) { return boost::filesystem::remove(toBoostPath(path)); }
	return true;
}

//---------------------------------------------------------------------------------------------------
bool Files::removeAll(const char* path)
{
	if (fileExists(path) || directoryExists(path)) { return (boost::filesystem::remove_all(toBoostPath(path)) != 0); }
	return true;
}

}  // namespace FS
