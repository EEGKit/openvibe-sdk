#ifndef __EBML_CReader_H__
#define __EBML_CReader_H__

#include "IReader.h"

namespace EBML
{
	class EBML_API CReader : public EBML::IReader
	{
	public:

		explicit CReader(EBML::IReaderCallback& rReaderCallback);
		virtual ~CReader(void);

		virtual bool processData(const void* pBuffer, const uint64_t ui64BufferSize);
		virtual EBML::CIdentifier getCurrentNodeIdentifier(void) const;
		virtual uint64_t getCurrentNodeSize(void) const;
		virtual void release(void);

	protected:

		EBML::IReader* m_pReaderImplementation;

	private:

		CReader(void);
	};
};

#endif // __EBML_IReader_H__
