#pragma once

#include "IWriter.h"

namespace EBML
{
	class EBML_API CWriter : public IWriter
	{
	public:

		explicit CWriter(IWriterCallback& rWriterCallback);
		virtual ~CWriter();

		virtual bool openChild(const CIdentifier& rIdentifier);
		virtual bool setChildData(const void* pBuffer, uint64_t ui64BufferSize);
		virtual bool closeChild();
		virtual void release();

	protected:

		IWriter* m_pWriterImplementation;

	private:

		CWriter();
	};
};


