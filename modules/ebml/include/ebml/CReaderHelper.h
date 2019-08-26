#pragma once

#include "IReaderHelper.h"

namespace EBML
{
	class EBML_API CReaderHelper : public IReaderHelper
	{
	public:

		CReaderHelper();
		~CReaderHelper() override;
		uint64_t getUIntegerFromChildData(const void* pBuffer, uint64_t ui64BufferSize) override;
		int64_t getSIntegerFromChildData(const void* pBuffer, uint64_t ui64BufferSize) override;
		double getFloatFromChildData(const void* pBuffer, uint64_t ui64BufferSize) override;
		const char* getASCIIStringFromChildData(const void* pBuffer, uint64_t ui64BufferSize) override;
		void release() override;

	protected:

		IReaderHelper* m_pReaderHelperImplementation = nullptr;
	};
} // namespace EBML
