#include "IEntryEnumerator.h"

#include <string>
#include <cstring>
#include <stack>

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
 #include <glob.h>
 #include <sys/stat.h>
#elif defined TARGET_OS_Windows
#ifndef UNICODE
#define UNICODE
#endif
#include <Windows.h>
#include <codecvt>
#include <locale>
#else
#endif

#include <iostream>

using namespace std;
using namespace FS;

// ________________________________________________________________________________________________________________
//

namespace FS
{
	class CEntry final : public IEntryEnumerator::IEntry
	{
	public:

		explicit CEntry(const string& name) : m_Name(name) {}
		const char* getName() override { return m_Name.c_str(); }

		string m_Name;
	};
} // namespace FS

// ________________________________________________________________________________________________________________
//

namespace FS
{
	class CAttributes final : public IEntryEnumerator::IAttributes
	{
	public:

		CAttributes() {}
		~CAttributes() override {}
		bool isFile() override { return m_IsFile; }
		bool isDirectory() override { return m_IsDirectory; }
		bool isSymbolicLink() override { return m_IsSymbolicLink; }
		bool isArchive() override { return m_IsArchive; }
		bool isReadOnly() override { return m_IsReadOnly; }
		bool isHidden() override { return m_IsHidden; }
		bool isSystem() override { return m_IsSystem; }
		bool isExecutable() override { return m_IsExecutable; }
		size_t getSize() override { return m_Size; }

		bool m_IsFile         = false;
		bool m_IsDirectory    = false;
		bool m_IsSymbolicLink = false;
		bool m_IsArchive      = false;
		bool m_IsReadOnly     = false;
		bool m_IsHidden       = false;
		bool m_IsSystem       = false;
		bool m_IsExecutable   = false;
		size_t m_Size         = 0;
	};
}  // namespace FS

// ________________________________________________________________________________________________________________
//

namespace FS
{
	class CEntryEnumerator : public IEntryEnumerator
	{
	public:
		explicit CEntryEnumerator(IEntryEnumeratorCallBack& rEntryEnumeratorCallBack) : m_rEntryEnumeratorCallBack(rEntryEnumeratorCallBack) {}
		void release() override { delete this; }
	protected:
		IEntryEnumeratorCallBack& m_rEntryEnumeratorCallBack;
	};
} // namespace FS

// ________________________________________________________________________________________________________________
//

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS

namespace FS
{
	class CEntryEnumeratorLinux final : public CEntryEnumerator
	{
	public:
		CEntryEnumeratorLinux(IEntryEnumeratorCallBack& rEntryEnumeratorCallBack) : CEntryEnumerator(rEntryEnumeratorCallBack) { }
		virtual bool enumerate(const char* sWildCard, bool bRecursive=false);
	};
};

#elif defined TARGET_OS_Windows

namespace FS
{
	class CEntryEnumeratorWindows final : public CEntryEnumerator
	{
	public:
		explicit CEntryEnumeratorWindows(IEntryEnumeratorCallBack& rEntryEnumeratorCallBack) : CEntryEnumerator(rEntryEnumeratorCallBack) {}
		bool enumerate(const char* sWildCard, bool bRecursive = false) override;
	};
} // namespace FS

#else

namespace FS
{
	class CEntryEnumeratorDummy : public CEntryEnumerator
	{
	public:
		explicit CEntryEnumeratorDummy(IEntryEnumeratorCallBack& rEntryEnumeratorCallBack) : CEntryEnumerator(rEntryEnumeratorCallBack) { }
		virtual bool enumerate(const char* sWildCard, bool bRecursive=false) { return !sWildCard ? false : true; }
	};
} // namespace FS

#endif

// ________________________________________________________________________________________________________________
//

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS

bool CEntryEnumeratorLinux::enumerate(const char* sWildCard, bool bRecursive)
{
	if(!sWildCard) { return false; }

	glob_t l_oGlobStruc;
	memset(&l_oGlobStruc, GLOB_NOSORT, sizeof(l_oGlobStruc));

	// Glob can retrn
	switch (glob(sWildCard, 0, nullptr, &l_oGlobStruc))
	{
	case GLOB_NOSPACE:
	case GLOB_ABORTED:
		return false;
		break;
	case GLOB_NOMATCH:
		return true;
		break;
	default:
		break;
	}

	if(l_oGlobStruc.gl_pathc<=0)
	{
		// Nothing found
		return true;
	}

	size_t i=0;
	bool finished=false;
	while(!finished)
	{
		if(i<l_oGlobStruc.gl_pathc)
		{
			char* name=l_oGlobStruc.gl_pathv[i];
			CEntry l_oEntry(name);
			CAttributes l_oAttributes;

			struct stat l_oLStat;
			struct stat l_oStat;
			if(!lstat(name, &l_oLStat) && !stat(name, &l_oStat))
			{
				l_oAttributes.m_IsDirectory=S_ISDIR(l_oStat.st_mode)?true:false;
				l_oAttributes.m_IsFile=S_ISREG(l_oStat.st_mode)?true:false;
				l_oAttributes.m_IsSymbolicLink=S_ISLNK(l_oLStat.st_mode)?true:false;

				l_oAttributes.m_IsArchive=false;
				l_oAttributes.m_IsReadOnly=l_oStat.st_mode&S_IWUSR?false:true;
				l_oAttributes.m_IsHidden=false;
				l_oAttributes.m_IsSystem=S_ISBLK(l_oStat.st_mode)|S_ISFIFO(l_oStat.st_mode)|S_ISSOCK(l_oStat.st_mode)|S_ISCHR(l_oStat.st_mode)?true:false;
				l_oAttributes.m_IsExecutable=l_oStat.st_mode&S_IXUSR?true:false;

				l_oAttributes.m_Size=l_oStat.st_size;

				// Sends to callback
				if(!m_rEntryEnumeratorCallBack.callback(l_oEntry, l_oAttributes))
				{
					finished=true;
				}
			}
			i++;
		}
		else
		{
			finished=true;
		}
	}

	return true;
}

#elif defined TARGET_OS_Windows

bool CEntryEnumeratorWindows::enumerate(const char* sWildCard, bool bRecursive)
{
	if (!sWildCard || strlen(sWildCard) == 0) { return false; }

	wchar_t wildCardUtf16[1024];
	MultiByteToWideChar(CP_UTF8, 0, sWildCard, -1, wildCardUtf16, 1024);

	// $$$ TODO
	// $$$ Find better method to enumerate files
	// $$$ under windows including their initial path
	// $$$ (cFileName member of WIN32_FIND_DATA structure
	// $$$ loses the initial path !!)
	// $$$ TODO
	wchar_t extendedWildCard[1024];
	wchar_t* extendedWildCardFilename = nullptr;
	GetFullPathName(wildCardUtf16, 1024, extendedWildCard, &extendedWildCardFilename);
	std::wstring path(wildCardUtf16, wcslen(wildCardUtf16) - (extendedWildCardFilename ? wcslen(extendedWildCardFilename) : 0));

	std::stack<std::wstring> foldersToEnumerate;
	foldersToEnumerate.push(path);

	// if we need to recurse over subfolders, let's fetch all subfolders in l_vFoldersToEnumerate
	if (bRecursive)
	{
		std::stack<std::wstring> temporaryFolderSearchStack;
		temporaryFolderSearchStack.push(path);
		std::wstring currentSearchPath;
		while (! temporaryFolderSearchStack.empty())
		{
			currentSearchPath = temporaryFolderSearchStack.top();
			temporaryFolderSearchStack.pop();

			WIN32_FIND_DATA findData;
			HANDLE fileHandle;
			fileHandle = FindFirstFile((currentSearchPath + L"*").c_str(), &findData);
			if (fileHandle != INVALID_HANDLE_VALUE)
			{
				bool finished = false;
				while (!finished)
				{
					if (std::wstring(findData.cFileName) != L"." && std::wstring(findData.cFileName) != L"..")
					{
						if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						{
							foldersToEnumerate.push(currentSearchPath + findData.cFileName + L"/");
							temporaryFolderSearchStack.push(currentSearchPath + findData.cFileName + L"/");
						}
					}

					if (!FindNextFile(fileHandle, &findData)) { finished = true; }
				}
				FindClose(fileHandle);
			}
		}
	}
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

	std::wstring currentPath;
	while (! foldersToEnumerate.empty())
	{
		currentPath = foldersToEnumerate.top();
		foldersToEnumerate.pop();

		WIN32_FIND_DATA findData;
		HANDLE fileHandle;
		fileHandle = FindFirstFile((currentPath + extendedWildCardFilename).c_str(), &findData);

		if (fileHandle != INVALID_HANDLE_VALUE)
		{
			bool finished = false;
			while (!finished)
			{
				std::string entryName = converter.to_bytes(currentPath + findData.cFileName);
				CEntry entry(entryName);
				CAttributes attributes;

				attributes.m_IsDirectory    = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? true : false;
				attributes.m_IsFile         = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? false : true;
				attributes.m_IsSymbolicLink = false;

				attributes.m_IsArchive    = (findData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) ? true : false;
				attributes.m_IsReadOnly   = (findData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? true : false;
				attributes.m_IsHidden     = (findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ? true : false;
				attributes.m_IsSystem     = (findData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ? true : false;
				attributes.m_IsExecutable = false; // TODO

				attributes.m_Size = (findData.nFileSizeHigh << 16) + findData.nFileSizeLow;

				// Sends to callback
				if (!m_rEntryEnumeratorCallBack.callback(entry, attributes)) { finished = true; }

				if (!FindNextFile(fileHandle, &findData)) { finished = true; }
			}
			FindClose(fileHandle);
		}
	}

	return true;
}

#endif

FS_API IEntryEnumerator* FS::createEntryEnumerator(IEntryEnumeratorCallBack& rCallBack)
{
#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	IEntryEnumerator* res = new CEntryEnumeratorLinux(rCallBack);
#elif defined TARGET_OS_Windows
	IEntryEnumerator* res = new CEntryEnumeratorWindows(rCallBack);
#else
	IEntryEnumerator* res = new CEntryEnumeratorDummy(rCallBack);
#endif
	return res;
}
