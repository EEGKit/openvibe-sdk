#ifndef __FS_Files_H__
#define __FS_Files_H__

#include "defines.h"
#include <cstdio>
#include <iostream>
#include <fstream>

namespace FS
{
	class FS_API Files
	{
	public:
		static FILE* open(const char* sFile, const char* sMode);
		static FILE* popen(const char* sFile, const char* sMode);
		static void openOFStream(std::ofstream& rStream, const char* sFile, std::ios_base::openmode oMode = std::ios_base::out);
		static void openIFStream(std::ifstream& rStream, const char* sFile, std::ios_base::openmode oMode = std::ios_base::in);
		static void openFStream(std::fstream& rStream, const char* sFile, std::ios_base::openmode oMode);

		static FS::boolean equals(const char* pFile1, const char* pFile2);
		static FS::boolean fileExists(const char* pathToCheck);
		static FS::boolean directoryExists(const char* pathToCheck);
		// Creates all components of a path to the filesystem
		static FS::boolean createPath(const char* sPath);
		// Creates all components of a path to the filesystem except the last part (i.e. for paths including a filename in the end)
		static FS::boolean createParentPath(const char* sPath);
		// Returns a path omitting the last part of it (essentially boost::filesystem::parent_path). Output sParentPath needs to be pre-allocated.
		static FS::boolean getParentPath(const char *sPath, char *sParentPath);
		static FS::boolean getParentPath(const char *path, char *parentPath, size_t size);
		// Returns a filename component of a path (essentially boost::filesystem::filename) Output sParentPath needs to be pre-allocated
		static FS::boolean getFilename(const char *sPath, char *sFilename);
		static FS::boolean getFilename(const char *path, char *filename, size_t size);
		// Returns a filename component of a path without the extension (essentially boost::filesystem::filename) Output sParentPath needs to be pre-allocated
		static FS::boolean getFilenameWithoutExtension(const char *sPath, char *sFilename);
		static FS::boolean getFilenameWithoutExtension(const char *path, char *filename, size_t size);
		// Returns the extension component of a path
		static FS::boolean getFilenameExtension(const char* path, char* fileNameExtension);
		static FS::boolean getFilenameExtension(const char* path, char* fileNameExtension, size_t size);

#if defined TARGET_OS_Windows
		static::FS::boolean copyFile(const char* sSourceFile, const char* sDestination);
#endif
	private:

		Files(void);
	};
}

#endif // __FS_Files_H__
