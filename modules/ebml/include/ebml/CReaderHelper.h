#ifndef __EBML_CReaderHelper_H__
#define __EBML_CReaderHelper_H__

#include "IReaderHelper.h"

namespace EBML
{
	class EBML_API CReaderHelper : public IReaderHelper
	{
	public:

		CReaderHelper(void);
		virtual ~CReaderHelper(void);

		virtual uint64_t getUIntegerFromChildData(const void* pBuffer, const uint64_t ui64BufferSize);
		virtual int64_t getSIntegerFromChildData(const void* pBuffer, const uint64_t ui64BufferSize);
		virtual double getFloatFromChildData(const void* pBuffer, const uint64_t ui64BufferSize);
		virtual const char* getASCIIStringFromChildData(const void* pBuffer, const uint64_t ui64BufferSize);
		virtual void release(void);

	protected:

		IReaderHelper* m_pReaderHelperImplementation;
	};
};

#endif // __EBML_CReaderHelper_H__
