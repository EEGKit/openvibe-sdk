#ifndef __EBML_CWriterHelper_H__
#define __EBML_CWriterHelper_H__

#include "IWriterHelper.h"

namespace EBML
{
	class EBML_API CWriterHelper : public EBML::IWriterHelper
	{
	public:

		CWriterHelper(void);
		virtual ~CWriterHelper(void);

		virtual bool connect(EBML::IWriter* pWriter);
		virtual bool disconnect(void);

		virtual bool openChild(const EBML::CIdentifier& rIdentifier);
		virtual bool closeChild(void);

		virtual bool setSIntegerAsChildData(const EBML::int64 iValue);
		virtual bool setUIntegerAsChildData(const EBML::uint64 uiValue);
		virtual bool setFloat32AsChildData(const float fValue);
		virtual bool setFloat64AsChildData(const double fValue);
		virtual bool setBinaryAsChildData(const void* pBuffer, const EBML::uint64 ui64BufferLength);
		virtual bool setASCIIStringAsChildData(const char* sValue);

		virtual void release(void);

	protected:

		EBML::IWriterHelper* m_pWriterHelperImplementation;
	};
};

#endif // __EBML_CWriterHelper_H__
