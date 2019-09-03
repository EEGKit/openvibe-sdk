#pragma once

#include "IReaderHelper.h"

namespace EBML
{
	class EBML_API CReaderHelper final : public IReaderHelper
	{
	public:

		CReaderHelper();
		~CReaderHelper() override;
		uint64_t getUIntegerFromChildData(const void* buffer, uint64_t size) override;
		int64_t getSIntegerFromChildData(const void* buffer, uint64_t size) override;
		double getFloatFromChildData(const void* buffer, uint64_t size) override;
		const char* getASCIIStringFromChildData(const void* buffer, uint64_t size) override;
		void release() override;

	protected:

		IReaderHelper* m_pReaderHelperImplementation = nullptr;
	};
} // namespace EBML
