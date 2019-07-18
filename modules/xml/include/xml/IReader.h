#ifndef __XML_IReader_H__
#define __XML_IReader_H__

#include "defines.h"

namespace XML
{
	class XML_API IReaderCallback
	{
	public:
		virtual ~IReaderCallback(void) { }
		virtual void openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64_t ui64AttributeCount) = 0;
		virtual void processChildData(const char* sData) = 0;
		virtual void closeChild(void) = 0;
	};

	class XML_API IReaderCallBack : public IReaderCallback { };

	class XML_API IReader
	{
	public:
		virtual bool processData(const void* pBuffer, const uint64_t ui64BufferSize) = 0;
		virtual void release(void) = 0;
	protected:
		virtual ~IReader(void) { }
	};

	extern XML_API IReader* createReader(IReaderCallback& rReaderCallback);
};

#endif // __XML_IReader_H__
