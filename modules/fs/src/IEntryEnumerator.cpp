#include "IEntryEnumerator.h"

#include <string>
#include <cstring>
#include <stack>

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
 #include <glob.h>
 #include <sys/stat.h>
#elif defined TARGET_OS_Windows
 #include <windows.h>

 #ifndef UNICODE
 #define UNICODE
 #endif
#else
#endif

#include <iostream>

using namespace std;
using namespace FS;

//  * 2006-08-30 YRD - Portability note : using namespace FS confuses
//                     windows platform SDK because it defines itself
//                     a 'boolean' type. Thus the following define to
//                     force the use of FS::boolean !
#define boolean FS::boolean

// ________________________________________________________________________________________________________________
//

namespace FS
{
	class CEntry : public IEntryEnumerator::IEntry
	{
	public:

		CEntry(const string& sName);

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

		virtual boolean isFile(void);
		virtual boolean isDirectory(void);
		virtual boolean isSymbolicLink(void);

		virtual boolean isArchive(void);
		virtual boolean isReadOnly(void);
		virtual boolean isHidden(void);
		virtual boolean isSystem(void);
		virtual boolean isExecutable(void);

		virtual uint64 getSize(void);

	public:

		boolean m_bIsFile;
		boolean m_bIsDirectory;
		boolean m_bIsSymbolicLink;
		boolean m_bIsArchive;
		boolean m_bIsReadOnly;
		boolean m_bIsHidden;
		boolean m_bIsSystem;
		boolean m_bIsExecutable;
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

boolean CAttributes::isFile(void)
{
	return m_bIsFile;
}

boolean CAttributes::isDirectory(void)
{
	return m_bIsDirectory;
}

boolean CAttributes::isSymbolicLink(void)
{
	return m_bIsSymbolicLink;
}

boolean CAttributes::isArchive(void)
{
	return m_bIsArchive;
}

boolean CAttributes::isReadOnly(void)
{
	return m_bIsReadOnly;
}

boolean CAttributes::isHidden(void)
{
	return m_bIsHidden;
}

boolean CAttributes::isSystem(void)
{
	return m_bIsSystem;
}

boolean CAttributes::isExecutable(void)
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
		CEntryEnumerator(IEntryEnumeratorCallBack& rEntryEnumeratorCallBack);
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
		CEntryEnumeratorWindows(IEntryEnumeratorCallBack& rEntryEnumeratorCallBack);
		virtual boolean enumerate(const char* sWildCard, boolean bRecursive=false);
	};
};

#else

namespace FS
{
	class CEntryEnumeratorDummy : public CEntryEnumerator
	{
	public:
		CEntryEnumeratorDummy(IEntryEnumeratorCallBack& rEntryEnumeratorCallBack);
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

boolean CEntryEnumeratorLinux::enumerate(const char* sWildCard, boolean bRecursive)
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
	boolean l_bFinished=false;
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

boolean CEntryEnumeratorWindows::enumerate(const char* sWildCard, boolean bRecursive)
{
	if(!sWildCard || strlen(sWildCard)==0)
	{
		return false;
	}

	// $$$ TODO
	// $$$ Find better method to enumerate files
	// $$$ under windows including their initial path
	// $$$ (cFileName member of WIN32_FIND_DATA structure
	// $$$ loses the initial path !!)
	// $$$ TODO

	char l_sExtendedWildCard[1024];
	char* l_sExtendedWildCardFileName=NULL;
	int a=GetFullPathName(sWildCard, sizeof(l_sExtendedWildCard), l_sExtendedWildCard, &l_sExtendedWildCardFileName);
	std::string l_sPath(sWildCard, strlen(sWildCard)-(l_sExtendedWildCardFileName?strlen(l_sExtendedWildCardFileName):0));
	
	std::stack<std::string> l_vFoldersToEnumerate;
	l_vFoldersToEnumerate.push(std::string(l_sPath));
	
	// if we need to recurse over subfolders, let's fetch all subfolders in l_vFoldersToEnumerate
	if(bRecursive)
	{
		std::stack<std::string> l_oTemporaryFolderSearchStack;
		l_oTemporaryFolderSearchStack.push(l_sPath);
		std::string l_sCurrentSearchPath;
		while(! l_oTemporaryFolderSearchStack.empty())
		{
			l_sCurrentSearchPath = l_oTemporaryFolderSearchStack.top();
			l_oTemporaryFolderSearchStack.pop();

			WIN32_FIND_DATA l_oFindData;
			HANDLE l_pFileHandle;
			l_pFileHandle=FindFirstFile((l_sCurrentSearchPath+"*").c_str(), &l_oFindData);
			if(l_pFileHandle != INVALID_HANDLE_VALUE)
			{
				boolean l_bFinished=false;
				while(!l_bFinished)
				{
					if (std::string(l_oFindData.cFileName) != "." && std::string(l_oFindData.cFileName) != "..")
					{
						if (l_oFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) 
						{
							l_vFoldersToEnumerate.push(l_sCurrentSearchPath + l_oFindData.cFileName + "/");
							l_oTemporaryFolderSearchStack.push(l_sCurrentSearchPath + l_oFindData.cFileName + "/");
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
	
	std::string l_sCurrentPath;
	while(! l_vFoldersToEnumerate.empty())
	{
		l_sCurrentPath = l_vFoldersToEnumerate.top();
		l_vFoldersToEnumerate.pop();

		WIN32_FIND_DATA l_oFindData;
		HANDLE l_pFileHandle;
		l_pFileHandle=FindFirstFile((l_sCurrentPath+l_sExtendedWildCardFileName).c_str(), &l_oFindData);
		if(l_pFileHandle!=INVALID_HANDLE_VALUE)
		{
			boolean l_bFinished=false;
			while(!l_bFinished)
			{
				CEntry l_oEntry(std::string(l_sCurrentPath+l_oFindData.cFileName).c_str());
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
