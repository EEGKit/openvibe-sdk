#pragma once

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
		static bool createPath(const char* path);
		// Creates all components of a path to the filesystem except the last part (i.e. for paths including a filename in the end)
		static bool createParentPath(const char* path);
		// Returns a path omitting the last part of it (essentially boost::filesystem::parent_path). Output sParentPath needs to be pre-allocated.
		static bool getParentPath(const char* path, char* sParentPath);
		static bool getParentPath(const char* path, char* parentPath, size_t size);

		/**
		* \brief Fills filename as a component of a path
		* \param path [in]: input path
		* \param filename [out]: file name, needs to be pre-allocated
		*
		* (essentially boost::filesystem::filename)
		* \return Success status
		*/
		static bool getFilename(const char* path, char* filename);
		static bool getFilename(const char* path, char* filename, size_t size);

		/**
		* \brief Fills filename as a component of a path without the extension
		* \param path [in]: input path
		* \param filename [out]: file name without extension, needs to be pre-allocated
		*
		* (essentially boost::filesystem::filename)
		* \return Success status
		*/
		static bool getFilenameWithoutExtension(const char* path, char* filename);
		static bool getFilenameWithoutExtension(const char* path, char* filename, size_t size);
		// Returns the extension component of a path
		static bool getFilenameExtension(const char* path, char* fileNameExtension);
		static bool getFilenameExtension(const char* path, char* fileNameExtension, size_t size);

		static bool copyFile(const char* srcFile, const char* dstPath);
		static bool copyDirectory(const char* srcDir, const char* dstDir);

		static bool remove(const char* path);
		static bool removeAll(const char* path);
	private:

		Files() = delete;
	};
} // namespace FS
