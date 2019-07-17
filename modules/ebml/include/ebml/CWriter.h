#ifndef __EBML_CWriter_H__
#define __EBML_CWriter_H__

#include "IWriter.h"

namespace EBML
{
	class EBML_API CWriter : public EBML::IWriter
	{
	public:

		explicit CWriter(EBML::IWriterCallback& rWriterCallback);
		virtual ~CWriter(void);

		virtual bool openChild(const EBML::CIdentifier& rIdentifier);
		virtual bool setChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
		virtual bool closeChild(void);
		virtual void release(void);

	protected:

		EBML::IWriter* m_pWriterImplementation;

	private:

		CWriter(void);
	};
};

#endif // __EBML_CWriter_H__
