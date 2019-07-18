#ifndef __EBML_CWriter_H__
#define __EBML_CWriter_H__

#include "IWriter.h"

namespace EBML
{
	class EBML_API CWriter : public IWriter
	{
	public:

		explicit CWriter(IWriterCallback& rWriterCallback);
		virtual ~CWriter(void);

		virtual bool openChild(const CIdentifier& rIdentifier);
		virtual bool setChildData(const void* pBuffer, const uint64_t ui64BufferSize);
		virtual bool closeChild(void);
		virtual void release(void);

	protected:

		IWriter* m_pWriterImplementation;

	private:

		CWriter(void);
	};
};

#endif // __EBML_CWriter_H__
