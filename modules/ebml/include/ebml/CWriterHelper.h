#ifndef __EBML_CWriterHelper_H__
#define __EBML_CWriterHelper_H__

#include "IWriterHelper.h"

namespace EBML
{
	class EBML_API CWriterHelper : public IWriterHelper
	{
	public:

		CWriterHelper(void);
		virtual ~CWriterHelper(void);

		virtual bool connect(IWriter* pWriter);
		virtual bool disconnect(void);

		virtual bool openChild(const CIdentifier& rIdentifier);
		virtual bool closeChild(void);

		virtual bool setSIntegerAsChildData(int64_t iValue);
		virtual bool setUIntegerAsChildData(uint64_t uiValue);
		virtual bool setFloat32AsChildData(float fValue);
		virtual bool setFloat64AsChildData(double fValue);
		virtual bool setBinaryAsChildData(const void* pBuffer, uint64_t ui64BufferLength);
		virtual bool setASCIIStringAsChildData(const char* sValue);

		virtual void release(void);

	protected:

		IWriterHelper* m_pWriterHelperImplementation;
	};
};

#endif // __EBML_CWriterHelper_H__
