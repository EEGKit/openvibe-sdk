#include "ebml/IReaderHelper.h"

#include <string>
#include <cstring>
#include <cstdlib>

using namespace EBML;
using namespace std;

namespace EBML
{
	namespace
	{
		class CReaderHelper final : public IReaderHelper
		{
		public:
			CReaderHelper();
			uint64_t getUIntegerFromChildData(const void* buffer, uint64_t size) override;
			int64_t getSIntegerFromChildData(const void* buffer, uint64_t size) override;
			double getFloatFromChildData(const void* buffer, uint64_t size) override;
			const char* getASCIIStringFromChildData(const void* buffer, uint64_t size) override;
			void release() override;

			std::string m_sASCIIString;
		};
	} // namespace
} // namespace EBML

CReaderHelper::CReaderHelper() {}

uint64_t CReaderHelper::getUIntegerFromChildData(const void* buffer, const uint64_t size)
{
	uint64_t result = 0;
	for (uint64_t i = 0; i < size; i++)
	{
		result <<= 8;
		result |= ((unsigned char*)buffer)[i];
	}
	return result;
}

int64_t CReaderHelper::getSIntegerFromChildData(const void* buffer, const uint64_t size)
{
	int64_t result = 0;
	if (size != 0 && ((unsigned char*)buffer)[0] & 0x80) { result = -1; }

	for (uint64_t i = 0; i < size; i++)
	{
		result <<= 8;
		result |= ((unsigned char*)buffer)[i];
	}
	return result;
}

double CReaderHelper::getFloatFromChildData(const void* buffer, const uint64_t size)
{
	float l_f32Result;
	double l_f64Result;
	int l_ui32Result;
	int64_t l_ui64Result;

	switch (size)
	{
		case 0:
			l_f64Result = 0;
			break;

		case 4:
			l_ui32Result = uint32_t(getUIntegerFromChildData(buffer, size));
			memcpy(&l_f32Result, &l_ui32Result, sizeof(l_f32Result));
			l_f64Result = l_f32Result;
			break;

		case 8:
			l_ui64Result = uint64_t(getUIntegerFromChildData(buffer, size));
			memcpy(&l_f64Result, &l_ui64Result, sizeof(l_f64Result));
			break;

		case 10:
			l_f64Result = 0;
			break;

		default:
			l_f64Result = 0;
			break;
	}

	return l_f64Result;
}

const char* CReaderHelper::getASCIIStringFromChildData(const void* buffer, const uint64_t size)
{
	if (size)
	{
		m_sASCIIString.assign((char*)buffer, size_t(size));
	}
	else { m_sASCIIString = ""; }
	return m_sASCIIString.c_str();
}

void CReaderHelper::release() { delete this; }

EBML_API IReaderHelper* EBML::createReaderHelper() { return new CReaderHelper(); }
