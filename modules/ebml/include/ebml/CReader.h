#pragma once

#include "IReader.h"

namespace EBML
{
	class EBML_API CReader : public IReader
	{
	public:

		explicit CReader(IReaderCallback& rReaderCallback);
		virtual ~CReader();

		virtual bool processData(const void* pBuffer, uint64_t ui64BufferSize);
		virtual CIdentifier getCurrentNodeIdentifier() const;
		virtual uint64_t getCurrentNodeSize() const;
		virtual void release();

	protected:

		IReader* m_pReaderImplementation;

	private:

		CReader();
	};
};


