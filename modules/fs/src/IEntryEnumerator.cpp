#include "IEntryEnumerator.h"

#include <string>
#include <cstring>
#include <stack>
#include <codecvt>
#include <locale>

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
 #include <glob.h>
 #include <sys/stat.h>
#elif defined TARGET_OS_Windows
 #ifndef UNICODE
 #define UNICODE
 #endif
 #include <Windows.h>
#else
#endif

#include <iostream>

using namespace std;
using namespace FS;

// ________________________________________________________________________________________________________________
//

namespace FS
{
	class CEntry : public IEntryEnumerator::IEntry
	{
	public:

		explicit CEntry(const string& sName);
		explicit CEntry(const wstring& sName);

		virtual const char* getName(void);

	public:

		string m_sName;
	};
};

// ________________________________________________________________________________________________________________
//

IEntryEnumerator::IEntry::~IEntry(void)
{
}

CEntry::CEntry(const string& sName)
	:m_sName(sName)
{
}

CEntry::CEntry(const wstring& sName)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

	m_sName = converter.to_bytes(sName);
}

const char* CEntry::getName(void)
{
	return m_sName.c_str();
}

// ________________________________________________________________________________________________________________
//

namespace FS
{
	class CAttributes : public IEntryEnumerator::IAttributes
	{
	public:

		CAttributes(void);
		virtual ~CAttributes(void);

		virtual bool isFile(void);
		virtual bool isDirectory(void);
		virtual bool isSymbolicLink(void);

		virtual bool isArchive(void);
		virtual bool isReadOnly(void);
		virtual bool isHidden(void);
		virtual bool isSystem(void);
		virtual bool isExecutable(void);

		virtual uint64 getSize(void);

	public:

		bool m_bIsFile;
		bool m_bIsDirectory;
		bool m_bIsSymbolicLink;
		bool m_bIsArchive;
		bool m_bIsReadOnly;
		bool m_bIsHidden;
		bool m_bIsSystem;
		bool m_bIsExecutable;
		uint64 m_ui64Size;
	};
};

// ________________________________________________________________________________________________________________
//

IEntryEnumerator::IAttributes::~IAttributes(void)
{
}

CAttributes::CAttributes(void)
	:m_bIsFile(false)
	,m_bIsDirectory(false)
	,m_bIsSymbolicLink(false)
	,m_bIsArchive(false)
	,m_bIsReadOnly(false)
	,m_bIsHidden(false)
	,m_bIsSystem(false)
	,m_bIsExecutable(false)
	,m_ui64Size(0)
{
}

CAttributes::~CAttributes(void)
{
}

// ________________________________________________________________________________________________________________
//

bool CAttributes::isFile(void)
{
	return m_bIsFile;
}

bool CAttributes::isDirectory(void)
{
	return m_bIsDirectory;
}

bool CAttributes::isSymbolicLink(void)
{
	return m_bIsSymbolicLink;
}

bool CAttributes::isArchive(void)
{
	return m_bIsArchive;
}

bool CAttributes::isReadOnly(void)
{
	return m_bIsReadOnly;
}

bool CAttributes::isHidden(void)
{
	return m_bIsHidden;
}

bool CAttributes::isSystem(void)
{
	return m_bIsSystem;
}

bool CAttributes::isExecutable(void)
{
	return m_bIsExecutable;
}

// ________________________________________________________________________________________________________________
//

uint64 CAttributes::getSize(void)
{
	return m_ui64Size;
}

// ________________________________________________________________________________________________________________
//

IEntryEnumerator::~IEntryEnumerator(void)
{
}

// ________________________________________________________________________________________________________________
//

namespace FS
{
	class CEntryEnumerator : public IEntryEnumerator
	{
	public:
		explicit CEntryEnumerator(IEntryEnumeratorCallBack& rEntryEnumeratorCallBack);
		virtual void release(void);
	protected:
		IEntryEnumeratorCallBack& m_rEntryEnumeratorCallBack;
	};
};

// ________________________________________________________________________________________________________________
//

CEntryEnumerator::CEntryEnumerator(IEntryEnumeratorCallBack& rEntryEnumeratorCallBack)
	:m_rEntryEnumeratorCallBack(rEntryEnumeratorCallBack)
{
}

void CEntryEnumerator::release(void)
{
	delete this;
}

// ________________________________________________________________________________________________________________
//

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS

namespace FS
{
	class CEntryEnumeratorLinux : public CEntryEnumerator
	{
	public:
		CEntryEnumeratorLinux(IEntryEnumeratorCallBack& rEntryEnumeratorCallBack);
		virtual boolean enumerate(const char* sWildCard, boolean bRecursive=false);
	};
};

#elif defined TARGET_OS_Windows

namespace FS
{
	class CEntryEnumeratorWindows : public CEntryEnumerator
	{
	public:
		explicit CEntryEnumeratorWindows(IEntryEnumeratorCallBack& rEntryEnumeratorCallBack);
		virtual boolean enumerate(const char* sWildCard, boolean bRecursive=false);
	};
};

#else

namespace FS
{
	class CEntryEnumeratorDummy : public CEntryEnumerator
	{
	public:
		explicit CEntryEnumeratorDummy(IEntryEnumeratorCallBack& rEntryEnumeratorCallBack);
		virtual boolean enumerate(const char* sWildCard, boolean bRecursive=false);
	};
};

#endif

// ________________________________________________________________________________________________________________
//

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS

CEntryEnumeratorLinux::CEntryEnumeratorLinux(IEntryEnumeratorCallBack& rEntryEnumeratorCallBack)
	:CEntryEnumerator(rEntryEnumeratorCallBack)
{
}

bool CEntryEnumeratorLinux::enumerate(const char* sWildCard, bool bRecursive)
{
	if(!sWildCard)
	{
		return false;
	}

	glob_t l_oGlobStruc;
	memset(&l_oGlobStruc, GLOB_NOSORT, sizeof(l_oGlobStruc));

	// Glob can retrn
	switch (glob(sWildCard, 0, NULL, &l_oGlobStruc))
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

CEntryEnumeratorWindows::CEntryEnumeratorWindows(IEntryEnumeratorCallBack& rEntryEnumeratorCallBack)
	:CEntryEnumerator(rEntryEnumeratorCallBack)
{
}

bool CEntryEnumeratorWindows::enumerate(const char* sWildCard, bool bRecursive)
{

	if(!sWildCard || strlen(sWildCard)==0)
	{
		return false;
	}

	wchar_t wildCardUtf16[1024];
	MultiByteToWideChar(CP_UTF8, 0, sWildCard, -1, wildCardUtf16, 1024);

	// $$$ TODO
	// $$$ Find better method to enumerate files
	// $$$ under windows including their initial path
	// $$$ (cFileName member of WIN32_FIND_DATA structure
	// $$$ loses the initial path !!)
	// $$$ TODO
	wchar_t l_sExtendedWildCard[1024];
	wchar_t* l_sExtendedWildCardFileName = NULL;
	GetFullPathName(wildCardUtf16, 1024, l_sExtendedWildCard, &l_sExtendedWildCardFileName);
	std::wstring l_sPath(wildCardUtf16, wcslen(wildCardUtf16) - (l_sExtendedWildCardFileName ? wcslen(l_sExtendedWildCardFileName) : 0));

	std::stack<std::wstring> l_vFoldersToEnumerate;
	l_vFoldersToEnumerate.push(l_sPath);

	// if we need to recurse over subfolders, let's fetch all subfolders in l_vFoldersToEnumerate
	if(bRecursive)
	{
		std::stack<std::wstring> l_oTemporaryFolderSearchStack;
		l_oTemporaryFolderSearchStack.push(l_sPath);
		std::wstring l_sCurrentSearchPath;
		while(! l_oTemporaryFolderSearchStack.empty())
		{
			l_sCurrentSearchPath = l_oTemporaryFolderSearchStack.top();
			l_oTemporaryFolderSearchStack.pop();

			WIN32_FIND_DATA l_oFindData;
			HANDLE l_pFileHandle;
			l_pFileHandle=FindFirstFile((l_sCurrentSearchPath+L"*").c_str(), &l_oFindData);
			if(l_pFileHandle != INVALID_HANDLE_VALUE)
			{
				boolean l_bFinished=false;
				while(!l_bFinished)
				{
					if (std::wstring(l_oFindData.cFileName) != L"." && std::wstring(l_oFindData.cFileName) != L"..")
					{
						if (l_oFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						{
							l_vFoldersToEnumerate.push(l_sCurrentSearchPath + l_oFindData.cFileName + L"/");
							l_oTemporaryFolderSearchStack.push(l_sCurrentSearchPath + l_oFindData.cFileName + L"/");
						}
					}

					if(!FindNextFile(l_pFileHandle, &l_oFindData))
					{
						l_bFinished=true;
					}
				}
				FindClose(l_pFileHandle);
			}
		}
	}

	std::wstring l_sCurrentPath;
	while(! l_vFoldersToEnumerate.empty())
	{
		l_sCurrentPath = l_vFoldersToEnumerate.top();
		l_vFoldersToEnumerate.pop();

		WIN32_FIND_DATA l_oFindData;
		HANDLE l_pFileHandle;
		l_pFileHandle = FindFirstFile((l_sCurrentPath + l_sExtendedWildCardFileName).c_str(), &l_oFindData);

		if(l_pFileHandle!=INVALID_HANDLE_VALUE)
		{
			boolean l_bFinished=false;
			while(!l_bFinished)
			{
				CEntry l_oEntry(std::wstring(l_sCurrentPath+l_oFindData.cFileName).c_str());
				CAttributes l_oAttributes;

				l_oAttributes.m_bIsDirectory=(l_oFindData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)?true:false;
				l_oAttributes.m_bIsFile=(l_oFindData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)?false:true;
				l_oAttributes.m_bIsSymbolicLink=false;

				l_oAttributes.m_bIsArchive=(l_oFindData.dwFileAttributes&FILE_ATTRIBUTE_ARCHIVE)?true:false;
				l_oAttributes.m_bIsReadOnly=(l_oFindData.dwFileAttributes&FILE_ATTRIBUTE_READONLY)?true:false;
				l_oAttributes.m_bIsHidden=(l_oFindData.dwFileAttributes&FILE_ATTRIBUTE_HIDDEN)?true:false;
				l_oAttributes.m_bIsSystem=(l_oFindData.dwFileAttributes&FILE_ATTRIBUTE_SYSTEM)?true:false;
				l_oAttributes.m_bIsExecutable=false; // TODO

				l_oAttributes.m_ui64Size=(l_oFindData.nFileSizeHigh<<16)+l_oFindData.nFileSizeLow;

				// Sends to callback
				if(!m_rEntryEnumeratorCallBack.callback(l_oEntry, l_oAttributes))
				{
					l_bFinished=true;
				}

				if(!FindNextFile(l_pFileHandle, &l_oFindData))
				{
					l_bFinished=true;
				}
			}
			FindClose(l_pFileHandle);
		}
	}

	return true;
}

#else

CEntryEnumeratorDummy::CEntryEnumeratorDummy(IEntryEnumeratorCallBack& rEntryEnumeratorCallBack)
	:CEntryEnumerator(rEntryEnumeratorCallBack)
{
}

boolean CEntryEnumeratorDummy::enumerate(const char* sWildCard, boolean bRecursive)
{
	if(!sWildCard)
	{
		return false;
	}

	return true;
}

#endif

FS_API IEntryEnumerator* FS::createEntryEnumerator(IEntryEnumeratorCallBack& rEntryEnumeratorCallBack)
{
	IEntryEnumerator* l_pResult=NULL;
#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	l_pResult=new CEntryEnumeratorLinux(rEntryEnumeratorCallBack);
#elif defined TARGET_OS_Windows
	l_pResult=new CEntryEnumeratorWindows(rEntryEnumeratorCallBack);
#else
	l_pResult=new CEntryEnumeratorDummy(rEntryEnumeratorCallBack);
#endif
	return l_pResult;
}
