#ifndef __FS_IEntryEnumerator_H__
#define __FS_IEntryEnumerator_H__

#include "defines.h"
#include <cinttypes>

namespace FS
{
	class IEntryEnumeratorCallBack;

	class FS_API IEntryEnumerator
	{
	public:

		class FS_API IAttributes
		{
		public:
			virtual bool isFile(void) =0;
			virtual bool isDirectory(void) =0;
			virtual bool isSymbolicLink(void) =0;

			virtual bool isArchive(void) =0;
			virtual bool isReadOnly(void) =0;
			virtual bool isHidden(void) =0;
			virtual bool isSystem(void) =0;
			virtual bool isExecutable(void) =0;

			virtual uint64_t getSize(void) =0;
		protected:
			virtual ~IAttributes(void);
		};

		class FS_API IEntry
		{
		public:
			virtual const char* getName(void) =0;
		protected:
			virtual ~IEntry(void);
		};

		virtual bool enumerate(const char* sWildcard, bool bRecursive = false) =0;
		virtual void release(void) =0;
	protected:
		virtual ~IEntryEnumerator(void);
	};

	class FS_API IEntryEnumeratorCallBack
	{
	public:
		virtual bool callback(FS::IEntryEnumerator::IEntry& rEntry, FS::IEntryEnumerator::IAttributes& rAttributes) =0;
		virtual ~IEntryEnumeratorCallBack(void) { }
	};

	extern FS_API FS::IEntryEnumerator* createEntryEnumerator(FS::IEntryEnumeratorCallBack& rCallBack);
};

#endif // __FS_IEntryEnumerator_H__
