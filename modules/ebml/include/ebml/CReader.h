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

		virtual bool processData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
		virtual EBML::CIdentifier getCurrentNodeIdentifier(void) const;
		virtual EBML::uint64 getCurrentNodeSize(void) const;
		virtual void release(void);

	protected:

		EBML::IReader* m_pReaderImplementation;

	private:

		CReader(void);
	};
};

#endif // __EBML_IReader_H__
