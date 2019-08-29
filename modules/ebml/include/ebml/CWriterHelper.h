#pragma once

#include "IWriterHelper.h"

namespace EBML
{
	class EBML_API CWriterHelper final : public IWriterHelper
	{
	public:

		CWriterHelper();
		~CWriterHelper() override;
		bool connect(IWriter* pWriter) override;
		bool disconnect() override;
		bool openChild(const CIdentifier& rIdentifier) override;
		bool closeChild() override;
		bool setSIntegerAsChildData(int64_t iValue) override;
		bool setUIntegerAsChildData(uint64_t uiValue) override;
		bool setFloat32AsChildData(float fValue) override;
		bool setFloat64AsChildData(double fValue) override;
		bool setBinaryAsChildData(const void* buffer, uint64_t ui64BufferLength) override;
		bool setASCIIStringAsChildData(const char* sValue) override;
		void release() override;

	protected:

		IWriterHelper* m_pWriterHelperImplementation = nullptr;
	};
} // namespace EBML
