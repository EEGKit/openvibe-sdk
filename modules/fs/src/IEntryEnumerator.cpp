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

		explicit CEntry(const string& sName) : m_sName(sName) {}
		const char* getName() override { return m_sName.c_str(); }

		string m_sName;
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
		bool isFile() override { return m_bIsFile; }
		bool isDirectory() override { return m_bIsDirectory; }
		bool isSymbolicLink() override { return m_bIsSymbolicLink; }
		bool isArchive() override { return m_bIsArchive; }
		bool isReadOnly() override { return m_bIsReadOnly; }
		bool isHidden() override { return m_bIsHidden; }
		bool isSystem() override { return m_bIsSystem; }
		bool isExecutable() override { return m_bIsExecutable; }
		uint64_t getSize() override { return m_ui64Size; }

		bool m_bIsFile         = false;
		bool m_bIsDirectory    = false;
		bool m_bIsSymbolicLink = false;
		bool m_bIsArchive      = false;
		bool m_bIsReadOnly     = false;
		bool m_bIsHidden       = false;
		bool m_bIsSystem       = false;
		bool m_bIsExecutable   = false;
		uint64_t m_ui64Size    = 0;
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
	bool l_bFinished=false;
	while(!l_bFinished)
	{
		if(i<l_oGlobStruc.gl_pathc)
		{
			char* l_sName=l_oGlobStruc.gl_pathv[i];
			CEntry l_oEntry(l_sName);
			CAttributes l_oAttributes;

			struct stat l_oLStat;
			struct stat l_oStat;
			if(!lstat(l_sName, &l_oLStat) && !stat(l_sName, &l_oStat))
			{
				l_oAttributes.m_bIsDirectory=S_ISDIR(l_oStat.st_mode)?true:false;
				l_oAttributes.m_bIsFile=S_ISREG(l_oStat.st_mode)?true:false;
				l_oAttributes.m_bIsSymbolicLink=S_ISLNK(l_oLStat.st_mode)?true:false;

				l_oAttributes.m_bIsArchive=false;
				l_oAttributes.m_bIsReadOnly=l_oStat.st_mode&S_IWUSR?false:true;
				l_oAttributes.m_bIsHidden=false;
				l_oAttributes.m_bIsSystem=S_ISBLK(l_oStat.st_mode)|S_ISFIFO(l_oStat.st_mode)|S_ISSOCK(l_oStat.st_mode)|S_ISCHR(l_oStat.st_mode)?true:false;
				l_oAttributes.m_bIsExecutable=l_oStat.st_mode&S_IXUSR?true:false;

				l_oAttributes.m_ui64Size=l_oStat.st_size;

				// Sends to callback
				if(!m_rEntryEnumeratorCallBack.callback(l_oEntry, l_oAttributes))
				{
					l_bFinished=true;
				}
			}
			i++;
		}
		else
		{
			l_bFinished=true;
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
	wchar_t l_sExtendedWildCard[1024];
	wchar_t* l_sExtendedWildCardFileName = nullptr;
	GetFullPathName(wildCardUtf16, 1024, l_sExtendedWildCard, &l_sExtendedWildCardFileName);
	std::wstring l_sPath(wildCardUtf16, wcslen(wildCardUtf16) - (l_sExtendedWildCardFileName ? wcslen(l_sExtendedWildCardFileName) : 0));

	std::stack<std::wstring> l_vFoldersToEnumerate;
	l_vFoldersToEnumerate.push(l_sPath);

	// if we need to recurse over subfolders, let's fetch all subfolders in l_vFoldersToEnumerate
	if (bRecursive)
	{
		std::stack<std::wstring> l_oTemporaryFolderSearchStack;
		l_oTemporaryFolderSearchStack.push(l_sPath);
		std::wstring l_sCurrentSearchPath;
		while (! l_oTemporaryFolderSearchStack.empty())
		{
			l_sCurrentSearchPath = l_oTemporaryFolderSearchStack.top();
			l_oTemporaryFolderSearchStack.pop();

			WIN32_FIND_DATA l_oFindData;
			HANDLE l_pFileHandle;
			l_pFileHandle = FindFirstFile((l_sCurrentSearchPath + L"*").c_str(), &l_oFindData);
			if (l_pFileHandle != INVALID_HANDLE_VALUE)
			{
				bool l_bFinished = false;
				while (!l_bFinished)
				{
					if (std::wstring(l_oFindData.cFileName) != L"." && std::wstring(l_oFindData.cFileName) != L"..")
					{
						if (l_oFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						{
							l_vFoldersToEnumerate.push(l_sCurrentSearchPath + l_oFindData.cFileName + L"/");
							l_oTemporaryFolderSearchStack.push(l_sCurrentSearchPath + l_oFindData.cFileName + L"/");
						}
					}

					if (!FindNextFile(l_pFileHandle, &l_oFindData)) { l_bFinished = true; }
				}
				FindClose(l_pFileHandle);
			}
		}
	}
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

	std::wstring l_sCurrentPath;
	while (! l_vFoldersToEnumerate.empty())
	{
		l_sCurrentPath = l_vFoldersToEnumerate.top();
		l_vFoldersToEnumerate.pop();

		WIN32_FIND_DATA l_oFindData;
		HANDLE l_pFileHandle;
		l_pFileHandle = FindFirstFile((l_sCurrentPath + l_sExtendedWildCardFileName).c_str(), &l_oFindData);

		if (l_pFileHandle != INVALID_HANDLE_VALUE)
		{
			bool l_bFinished = false;
			while (!l_bFinished)
			{
				std::string entryName = converter.to_bytes(l_sCurrentPath + l_oFindData.cFileName);
				CEntry l_oEntry(entryName.c_str());
				CAttributes l_oAttributes;

				l_oAttributes.m_bIsDirectory    = (l_oFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? true : false;
				l_oAttributes.m_bIsFile         = (l_oFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? false : true;
				l_oAttributes.m_bIsSymbolicLink = false;

				l_oAttributes.m_bIsArchive    = (l_oFindData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) ? true : false;
				l_oAttributes.m_bIsReadOnly   = (l_oFindData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? true : false;
				l_oAttributes.m_bIsHidden     = (l_oFindData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) ? true : false;
				l_oAttributes.m_bIsSystem     = (l_oFindData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) ? true : false;
				l_oAttributes.m_bIsExecutable = false; // TODO

				l_oAttributes.m_ui64Size = (l_oFindData.nFileSizeHigh << 16) + l_oFindData.nFileSizeLow;

				// Sends to callback
				if (!m_rEntryEnumeratorCallBack.callback(l_oEntry, l_oAttributes)) { l_bFinished = true; }

				if (!FindNextFile(l_pFileHandle, &l_oFindData)) { l_bFinished = true; }
			}
			FindClose(l_pFileHandle);
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
