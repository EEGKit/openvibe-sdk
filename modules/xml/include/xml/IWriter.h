#pragma once

#include "defines.h"

namespace XML
{
	class XML_API IWriterCallback
	{
	public:
		virtual ~IWriterCallback(void) { }
		virtual void write(const char* sString) = 0;
	};

	class XML_API IWriterCallBack : public IWriterCallback { };

	class XML_API IWriter
	{
	public:
		virtual bool openChild(const char* sName) = 0;
		virtual bool setAttribute(const char* sAttributeName, const char* sAttributeValue) = 0;
		virtual bool setChildData(const char* sData) = 0;
		virtual bool closeChild(void) = 0;
		virtual void release(void) = 0;
	protected:
		virtual ~IWriter(void) { }
	};

	extern XML_API IWriter* createWriter(IWriterCallback& rWriterCallback);
};


