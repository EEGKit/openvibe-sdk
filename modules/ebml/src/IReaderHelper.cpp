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
		class CReaderHelper : public IReaderHelper
		{
		public:
			CReaderHelper();

			virtual uint64_t getUIntegerFromChildData(const void* pBuffer, uint64_t ui64BufferSize);
			virtual int64_t getSIntegerFromChildData(const void* pBuffer, uint64_t ui64BufferSize);
			virtual double getFloatFromChildData(const void* pBuffer, uint64_t ui64BufferSize);
			virtual const char* getASCIIStringFromChildData(const void* pBuffer, uint64_t ui64BufferSize);

			virtual void release();

			std::string m_sASCIIString;
		};
	}  // namespace
}  // namespace EBML

CReaderHelper::CReaderHelper() {}

uint64_t CReaderHelper::getUIntegerFromChildData(const void* pBuffer, const uint64_t ui64BufferSize)
{
	uint64_t l_ui64Result = 0;
	uint64_t i;
	for (i = 0; i < ui64BufferSize; i++)
	{
		l_ui64Result <<= 8;
		l_ui64Result |= ((unsigned char*)pBuffer)[i];
	}
	return l_ui64Result;
}

int64_t CReaderHelper::getSIntegerFromChildData(const void* pBuffer, const uint64_t ui64BufferSize)
{
	int64_t l_i64Result = 0;
	uint64_t i;

	if (ui64BufferSize != 0 && ((unsigned char*)pBuffer)[0] & 0x80)
	{
		l_i64Result = -1;
	}

	for (i = 0; i < ui64BufferSize; i++)
	{
		l_i64Result <<= 8;
		l_i64Result |= ((unsigned char*)pBuffer)[i];
	}
	return l_i64Result;
}

double CReaderHelper::getFloatFromChildData(const void* pBuffer, const uint64_t ui64BufferSize)
{
	float l_f32Result;
	double l_f64Result;
	int32_t l_ui32Result;
	int64_t l_ui64Result;

	switch (ui64BufferSize)
	{
		case 0:
			l_f64Result = 0;
			break;

		case 4:
			l_ui32Result = (uint32_t)getUIntegerFromChildData(pBuffer, ui64BufferSize);
			memcpy(&l_f32Result, &l_ui32Result, sizeof(l_f32Result));
			l_f64Result = l_f32Result;
			break;

		case 8:
			l_ui64Result = (uint64_t)getUIntegerFromChildData(pBuffer, ui64BufferSize);
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

const char* CReaderHelper::getASCIIStringFromChildData(const void* pBuffer, const uint64_t ui64BufferSize)
{
	if (ui64BufferSize)
	{
		m_sASCIIString.assign((char*)pBuffer, (size_t)(ui64BufferSize));
	}
	else
	{
		m_sASCIIString = "";
	}
	return m_sASCIIString.c_str();
}

void CReaderHelper::release()
{
	delete this;
}

EBML_API IReaderHelper* EBML::createReaderHelper()
{
	return new CReaderHelper();
}
