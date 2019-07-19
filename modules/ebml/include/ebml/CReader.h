#pragma once

#include "IReader.h"

namespace EBML
{
	class EBML_API CReader : public IReader
	{
	public:

		explicit CReader(IReaderCallback& rReaderCallback);
		virtual ~CReader(void);

		virtual bool processData(const void* pBuffer, uint64_t ui64BufferSize);
		virtual CIdentifier getCurrentNodeIdentifier(void) const;
		virtual uint64_t getCurrentNodeSize(void) const;
		virtual void release(void);

	protected:

		IReader* m_pReaderImplementation;

	private:

		CReader(void);
	};
};


