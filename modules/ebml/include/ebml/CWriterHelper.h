#pragma once

#include "IWriterHelper.h"

namespace EBML
{
	class EBML_API CWriterHelper : public IWriterHelper
	{
	public:

		CWriterHelper();
		virtual ~CWriterHelper();

		virtual bool connect(IWriter* pWriter);
		virtual bool disconnect();

		virtual bool openChild(const CIdentifier& rIdentifier);
		virtual bool closeChild();

		virtual bool setSIntegerAsChildData(int64_t iValue);
		virtual bool setUIntegerAsChildData(uint64_t uiValue);
		virtual bool setFloat32AsChildData(float fValue);
		virtual bool setFloat64AsChildData(double fValue);
		virtual bool setBinaryAsChildData(const void* pBuffer, uint64_t ui64BufferLength);
		virtual bool setASCIIStringAsChildData(const char* sValue);

		virtual void release();

	protected:

		IWriterHelper* m_pWriterHelperImplementation;
	};
}  // namespace EBML
