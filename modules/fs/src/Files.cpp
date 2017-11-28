#include "Files.h"

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
 #include <stdio.h>   // For fopen
 #include <unistd.h>  //For access().
 #include <sys/stat.h>
 #include <sys/types.h>  // For stat().
#elif defined TARGET_OS_Windows
 #include "m_ConverterUtf8.h"
 #include <Windows.h>
#else
#endif

#include <boost/filesystem.hpp>
#include <boost/version.hpp>
#include <iostream>
#include <cstdio>
#include <cstring>

using namespace FS;
using namespace std;

//  * 2006-08-30 YRD - Portability note : using namespace FS confuses
//                     windows platform SDK because it defines itself
//                     a 'boolean' type. Thus the following define to
//                     force the use of FS::boolean !

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS

FILE* Files::open(const char* sFile, const char* sMode)
{
	FILE* fHandle = fopen(sFile, sMode);

	return fHandle;
}

FILE* Files::popen(const char* sFile, const char* sMode)
{
	FILE* fHandle = ::popen(sFile, sMode);

	return fHandle;
}

void Files::openOFStream(std::ofstream& rStream, const char* sFile, std::ios_base::openmode oMode)
{
	rStream.open(sFile, oMode);
}

void Files::openIFStream(std::ifstream& rStream, const char* sFile, std::ios_base::openmode oMode)
{
	rStream.open(sFile, oMode);
}

void Files::openFStream(std::fstream& rStream, const char* sFile, std::ios_base::openmode oMode)
{
	rStream.open(sFile, oMode);
}

#elif defined TARGET_OS_Windows


FILE* Files::open(const char* sFile, const char* sMode)
{
	FILE* fHandle;

	try
	{
		wstring l_sUTF16FileName = Common::Converter::utf8_to_utf16(sFile);
		wstring l_sUTF16Mode = Common::Converter::utf8_to_utf16(sMode);

		fHandle = _wfopen(l_sUTF16FileName.c_str(), l_sUTF16Mode.c_str());
	}
	catch (const std::logic_error&)
	{
		fHandle = fopen(sFile, sMode);
	}

	return fHandle;
}

FILE* Files::popen(const char* sFile, const char* sMode)
{
	FILE* fHandle;

	try
	{
		wstring l_sUTF16FileName = Common::Converter::utf8_to_utf16(sFile);
		wstring l_sUTF16Mode = Common::Converter::utf8_to_utf16(sMode);

		fHandle = _wpopen(l_sUTF16FileName.c_str(), l_sUTF16Mode.c_str());
	}
	catch (const std::logic_error&)
	{
		fHandle = popen(sFile, sMode);
	}

	return fHandle;
}

template<class T>
void openStream(T& rStream, const char* sFile, std::ios_base::openmode oMode)
{
	try
	{
		wstring l_sUTF16FileName = Common::Converter::utf8_to_utf16(sFile);

		rStream.open(l_sUTF16FileName.c_str(), oMode);
	}
	catch (const std::logic_error&)
	{
		rStream.open(sFile, oMode);
	}
}

void Files::openOFStream(std::ofstream& rStream, const char* sFile, std::ios_base::openmode oMode)
{
	openStream<std::ofstream>(rStream, sFile, oMode);
}

void Files::openIFStream(std::ifstream& rStream, const char* sFile, std::ios_base::openmode oMode)
{
	openStream<std::ifstream>(rStream, sFile, oMode);
}

void Files::openFStream(std::fstream& rStream, const char* sFile, std::ios_base::openmode oMode)
{
	openStream<std::fstream>(rStream, sFile, oMode);
}

#endif

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS

bool Files::equals(const char* pFile1, const char* pFile2)
{
	bool l_bResult=true;
	if(pFile1 && pFile2)
	{
		struct stat l_oStat1;
		struct stat l_oStat2;
		bool l_bStat1=!stat(pFile1, &l_oStat1);
		bool l_bStat2=!stat(pFile2, &l_oStat2);

		if(!l_bStat1 && !l_bStat2)
		{
			l_bResult=true;
		}
		else if(l_bStat1 && l_bStat2)
		{
			l_bResult=
				(l_oStat1.st_dev==l_oStat2.st_dev)&&
				(l_oStat1.st_ino==l_oStat2.st_ino)&&
				(l_oStat1.st_size==l_oStat2.st_size)&&
				(l_oStat1.st_mtime==l_oStat2.st_mtime);
		}
		else
		{
			l_bResult=false;
		}
	}
	return l_bResult;
}

#elif defined TARGET_OS_Windows

bool Files::equals(const char* pFile1, const char* pFile2)
{
	bool l_bResult=true;
	if(pFile1 && pFile2)
	{
		::HANDLE l_pHandle1=::CreateFile(pFile1, 0, FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);
		::HANDLE l_pHandle2=::CreateFile(pFile2, 0, FILE_SHARE_DELETE|FILE_SHARE_READ|FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, 0);
		if(l_pHandle1 && l_pHandle2)
		{
			::BY_HANDLE_FILE_INFORMATION l_oStat1;
			::BY_HANDLE_FILE_INFORMATION l_oStat2;
			::BOOL l_bStat1=::GetFileInformationByHandle(l_pHandle1, &l_oStat1);
			::BOOL l_bStat2=::GetFileInformationByHandle(l_pHandle2, &l_oStat2);
			if(!l_bStat1 && !l_bStat2)
			{
				l_bResult=true;
			}
			else if(l_bStat1 && l_bStat2)
			{
				l_bResult=
					(l_oStat1.dwVolumeSerialNumber==l_oStat2.dwVolumeSerialNumber)&&
					(l_oStat1.nFileIndexHigh==l_oStat2.nFileIndexHigh)&&
					(l_oStat1.nFileIndexLow==l_oStat2.nFileIndexLow)&&
					(l_oStat1.nFileSizeHigh==l_oStat2.nFileSizeHigh)&&
					(l_oStat1.nFileSizeLow==l_oStat2.nFileSizeLow)&&
					(l_oStat1.ftLastWriteTime.dwHighDateTime==l_oStat2.ftLastWriteTime.dwHighDateTime)&&
					(l_oStat1.ftLastWriteTime.dwLowDateTime==l_oStat2.ftLastWriteTime.dwLowDateTime);
			}
			else
			{
				l_bResult=false;
			}
			::CloseHandle(l_pHandle1);
			::CloseHandle(l_pHandle2);
		}
		else
		{
			if(l_pHandle1) ::CloseHandle(l_pHandle1);
			if(l_pHandle2) ::CloseHandle(l_pHandle2);
		}
	}
	return l_bResult;
}

#else

#endif

bool Files::fileExists(const char *pathToCheck) {
	if(!pathToCheck) 
	{
		return false;
	}
	FILE *fp = Files::open( pathToCheck, "r");
	if(!fp) {
		return false;
	} else {
		fclose(fp);
		return true;
	}
}

bool Files::directoryExists(const char *pathToCheck) {
	if(!pathToCheck) 
	{
		return false;
	}
#if defined TARGET_OS_Windows
	wstring pathUTF16 = Common::Converter::utf8_to_utf16(pathToCheck);
	DWORD ftyp = GetFileAttributesW(pathUTF16.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES) 
	{
		return false;
	}
	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
	{
		return true;
	}
#endif
#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	if ( access( pathToCheck, 0 ) == 0 )
	{
		struct stat status;
		stat( pathToCheck, &status );

		if ( status.st_mode & S_IFDIR )
		{
			return true;
		}
	} 
#endif
	return false;
}

bool Files::createPath(const char *path)
{
	if (strcmp(path, "") == 0)
	{
		return false;
	}
#if defined TARGET_OS_Windows
	wstring pathUTF16 = Common::Converter::utf8_to_utf16(path);
	boost::filesystem::create_directories(boost::filesystem::wpath(pathUTF16));
	return boost::filesystem::is_directory(boost::filesystem::wpath(pathUTF16));
#else
	return boost::filesystem::create_directories(boost::filesystem::path(path));
#endif
}

bool Files::createParentPath(const char *path)
{
	if (strcmp(path, "") == 0)
	{
		return false;
	}
#if defined TARGET_OS_Windows
	wstring pathUTF16 = Common::Converter::utf8_to_utf16(path);
	return boost::filesystem::create_directories(boost::filesystem::wpath(pathUTF16).parent_path());
#else
	return boost::filesystem::create_directories(boost::filesystem::path(path).parent_path());
#endif
}

bool Files::getParentPath(const char *sPath, char *sParentPath) 
{
	if(!sPath || !sParentPath) {
		return false;
	}

	strcpy(sParentPath, boost::filesystem::path(sPath).parent_path().string().c_str());

	return true;
}

bool Files::getParentPath(const char *path, char *parentPath, size_t size)
{
	if(!path || !parentPath) {
		return false;
	}

	strncpy(parentPath, boost::filesystem::path(path).parent_path().string().c_str(), size);

	return true;
}

bool Files::getFilename(const char *sPath, char *sFilename)
{
	if(!sPath || !sFilename) {
		return false;
	}

	strcpy(sFilename, boost::filesystem::path(sPath).filename().string().c_str());

	return true;
}

bool Files::getFilename(const char *path, char *filename, size_t size)
{
	if(!path || !filename) {
		return false;
	}

	strncpy(filename, boost::filesystem::path(path).filename().string().c_str(), size);

	return true;
}

bool Files::getFilenameWithoutExtension(const char *sPath, char *sFilename)
{
	if(!sPath || !sFilename) {
		return false;
	}

	strcpy(sFilename, boost::filesystem::path(sPath).filename().replace_extension("").string().c_str());

	return true;
}

bool Files::getFilenameWithoutExtension(const char *path, char *filename, size_t size)
{
	if(!path || !filename) {
		return false;
	}

	strncpy(filename, boost::filesystem::path(path).filename().replace_extension("").string().c_str(), size);

	return true;
}

bool Files::getFilenameExtension(const char* path, char* fileNameExtension)
{
	if (!path || !fileNameExtension)
	{
		return false;
	}

	strcpy(fileNameExtension, boost::filesystem::path(path).extension().string().c_str());

	return true;
}

bool Files::getFilenameExtension(const char* path, char* fileNameExtension, size_t size)
{
	if (!path || !fileNameExtension)
	{
		return false;
	}

	strncpy(fileNameExtension, boost::filesystem::path(path).extension().string().c_str(), size);

	return true;
}

#if BOOST_VERSION / 100 % 1000 != 54
bool Files::copyFile(const char* sSourceFile, const char* sDestinationPath)
{
	if(!sSourceFile || !sDestinationPath)
	{
		return false;
	}
#if defined TARGET_OS_Windows
	wstring pathSourceUTF16 = Common::Converter::utf8_to_utf16(sSourceFile);
	wstring pathDestinationUTF16 = Common::Converter::utf8_to_utf16(sDestinationPath);
	boost::filesystem::copy_file(pathSourceUTF16, pathDestinationUTF16);
#else
	boost::filesystem::copy_file(sSourceFile, sDestinationPath);
#endif
	return true;
}
#endif

bool Files::remove(const char* path)
{
	if (FS::Files::fileExists(path) || FS::Files::directoryExists(path))
	{
#if defined TARGET_OS_Windows
		std::wstring pathUTF16 = Common::Converter::utf8_to_utf16(path);
		return boost::filesystem::remove(boost::filesystem::wpath(pathUTF16.c_str()));
#else
		return boost::filesystem::remove(boost::filesystem::path(path));
#endif
	}
	return true;
}

bool Files::removeAll(const char* path)
{
	if (FS::Files::fileExists(path) || FS::Files::directoryExists(path))
	{
#if defined TARGET_OS_Windows
		std::wstring pathUTF16 = Common::Converter::utf8_to_utf16(path);
		return (boost::filesystem::remove_all(boost::filesystem::wpath(pathUTF16.c_str())) != 0);
#else
		return (boost::filesystem::remove_all(boost::filesystem::path(path)) != 0);
#endif
	}
	return true;
}


