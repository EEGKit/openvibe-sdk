#ifndef __FS_Files_H__
#define __FS_Files_H__

#include "defines.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <cinttypes>

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

		static bool equals(const char* pFile1, const char* pFile2);
		static bool fileExists(const char* pathToCheck);
		static bool directoryExists(const char* pathToCheck);
		// Creates all components of a path to the filesystem
		static bool createPath(const char* sPath);
		// Creates all components of a path to the filesystem except the last part (i.e. for paths including a filename in the end)
		static bool createParentPath(const char* sPath);
		// Returns a path omitting the last part of it (essentially boost::filesystem::parent_path). Output sParentPath needs to be pre-allocated.
		static bool getParentPath(const char *sPath, char *sParentPath);
		static bool getParentPath(const char *path, char *parentPath, size_t size);
		// Returns a filename component of a path (essentially boost::filesystem::filename) Output sParentPath needs to be pre-allocated
		static bool getFilename(const char *sPath, char *sFilename);
		static bool getFilename(const char *path, char *filename, size_t size);
		// Returns a filename component of a path without the extension (essentially boost::filesystem::filename) Output sParentPath needs to be pre-allocated
		static bool getFilenameWithoutExtension(const char *sPath, char *sFilename);
		static bool getFilenameWithoutExtension(const char *path, char *filename, size_t size);
		// Returns the extension component of a path
		static bool getFilenameExtension(const char* path, char* fileNameExtension);
		static bool getFilenameExtension(const char* path, char* fileNameExtension, size_t size);

		static bool copyFile(const char* sSourceFile, const char* sDestination);
		static bool copyDirectory(const char* sourceDir, const char* targetDir);

		static bool remove(const char* path);
		static bool removeAll(const char* path);
	private:

		Files(void);
	};
}

#endif // __FS_Files_H__
