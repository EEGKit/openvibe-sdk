#pragma once

#include "defines.h"

namespace XML
{
	class XML_API IReaderCallback
	{
	public:
		virtual ~IReaderCallback() { }
		virtual void openChild(const char* name, const char** sAttributeName, const char** sAttributeValue, uint64_t nAttribute) = 0;
		virtual void processChildData(const char* sData) = 0;
		virtual void closeChild() = 0;
	};

	class XML_API IReaderCallBack : public IReaderCallback { };

	class XML_API IReader
	{
	public:
		virtual bool processData(const void* buffer, uint64_t size) = 0;
		virtual void release() = 0;
	protected:
		virtual ~IReader() { }
	};

	extern XML_API IReader* createReader(IReaderCallback& rReaderCallback);
}
