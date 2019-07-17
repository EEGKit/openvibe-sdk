#include "ebml/IWriterHelper.h"
#include "ebml/IWriter.h"

#include <cstdlib>
#include <cstring>

using namespace EBML;

// ________________________________________________________________________________________________________________
//

namespace EBML
{
	namespace
	{
		class CWriterHelper : public IWriterHelper
		{
		public:

			CWriterHelper(void);

			virtual bool connect(IWriter* pWriter);
			virtual bool disconnect(void);

			virtual bool openChild(const CIdentifier& rIdentifier);
			virtual bool closeChild(void);

			virtual bool setSIntegerAsChildData(const int64 iValue);
			virtual bool setUIntegerAsChildData(const uint64 uiValue);
			virtual bool setFloat32AsChildData(const float fValue);
			virtual bool setFloat64AsChildData(const double fValue);
			virtual bool setBinaryAsChildData(const void* pBuffer, const uint64 ui64BufferLength);
			virtual bool setASCIIStringAsChildData(const char* sValue);

			virtual void release(void);

		protected:

			IWriter* m_pWriter;
		};
	};
};

// ________________________________________________________________________________________________________________
//

CWriterHelper::CWriterHelper(void) : m_pWriter(0) {}

// ________________________________________________________________________________________________________________
//

bool CWriterHelper::connect(IWriter* pWriter)
{
	m_pWriter = pWriter;
	return m_pWriter ? true : false;
}

bool CWriterHelper::disconnect(void)
{
	if (!m_pWriter) { return false; }
	m_pWriter = 0;
	return true;
}

// ________________________________________________________________________________________________________________
//

bool CWriterHelper::openChild(const CIdentifier& rIdentifier)
{
	return m_pWriter ? m_pWriter->openChild(rIdentifier) : false;
}

bool CWriterHelper::closeChild(void)
{
	return m_pWriter ? m_pWriter->closeChild() : false;
}

// ________________________________________________________________________________________________________________
//

bool CWriterHelper::setSIntegerAsChildData(const int64 iValue)
{
	uint64 i;
	uint64 l_ui64BufferSize = 0;
	unsigned char l_pBuffer[8];

	if (iValue == 0x00000000000000LL)
		l_ui64BufferSize = 0;
	else if (iValue >= -0x00000000000080LL && iValue <= 0x0000000000007fLL)
		l_ui64BufferSize = 1;
	else if (iValue >= -0x00000000008000LL && iValue <= 0x00000000007fffLL)
		l_ui64BufferSize = 2;
	else if (iValue >= -0x00000000800000LL && iValue <= 0x000000007fffffLL)
		l_ui64BufferSize = 3;
	else if (iValue >= -0x00000080000000LL && iValue <= 0x0000007fffffffLL)
		l_ui64BufferSize = 4;
	else if (iValue >= -0x00008000000000LL && iValue <= 0x00007fffffffffLL)
		l_ui64BufferSize = 5;
	else if (iValue >= -0x00800000000000LL && iValue <= 0x007fffffffffffLL)
		l_ui64BufferSize = 6;
	else if (iValue >= -0x80000000000000LL && iValue <= 0x7fffffffffffffLL)
		l_ui64BufferSize = 7;
	else
		l_ui64BufferSize = 8;

	for (i = 0; i < l_ui64BufferSize; i++)
	{
		l_pBuffer[l_ui64BufferSize - i - 1] = (unsigned char)((iValue >> (i * 8)) & 0xff);
	}

	return m_pWriter->setChildData(l_pBuffer, l_ui64BufferSize);
}

bool CWriterHelper::setUIntegerAsChildData(const uint64 uiValue)
{
	uint64 i;
	uint64 l_ui64BufferSize = 0;
	unsigned char l_pBuffer[8];

	if (uiValue == 0x000000000000000LL)
		l_ui64BufferSize = 0;
	else if (uiValue < 0x000000000000100LL)
		l_ui64BufferSize = 1;
	else if (uiValue < 0x000000000010000LL)
		l_ui64BufferSize = 2;
	else if (uiValue < 0x000000001000000LL)
		l_ui64BufferSize = 3;
	else if (uiValue < 0x000000100000000LL)
		l_ui64BufferSize = 4;
	else if (uiValue < 0x000010000000000LL)
		l_ui64BufferSize = 5;
	else if (uiValue < 0x001000000000000LL)
		l_ui64BufferSize = 6;
	else if (uiValue < 0x100000000000000LL)
		l_ui64BufferSize = 7;
	else
		l_ui64BufferSize = 8;

	for (i = 0; i < l_ui64BufferSize; i++)
	{
		l_pBuffer[l_ui64BufferSize - i - 1] = (unsigned char)((uiValue >> (i * 8)) & 0xff);
	}

	return m_pWriter->setChildData(l_pBuffer, l_ui64BufferSize);
}

bool CWriterHelper::setFloat32AsChildData(const float fValue)
{
	uint32 l_uiValue;
	uint64 i;
	uint64 l_ui64BufferSize = 0;
	unsigned char l_pBuffer[8];
	::memcpy(&l_uiValue, &fValue, sizeof(fValue));

	l_ui64BufferSize = (fValue != 0 ? 4 : 0);
	for (i = 0; i < l_ui64BufferSize; i++)
	{
		l_pBuffer[l_ui64BufferSize - i - 1] = (unsigned char)((l_uiValue >> (i * 8)) & 0xff);
	}
	return m_pWriter->setChildData(l_pBuffer, l_ui64BufferSize);
}

bool CWriterHelper::setFloat64AsChildData(const double fValue)
{
	uint64 l_uiValue;
	uint64 i;
	uint64 l_ui64BufferSize = 0;
	unsigned char l_pBuffer[8];
	::memcpy(&l_uiValue, &fValue, sizeof(fValue));

	l_ui64BufferSize = (fValue != 0 ? 8 : 0);
	for (i = 0; i < l_ui64BufferSize; i++)
	{
		l_pBuffer[l_ui64BufferSize - i - 1] = (unsigned char)((l_uiValue >> (i * 8)) & 0xff);
	}
	return m_pWriter->setChildData(l_pBuffer, l_ui64BufferSize);
}

bool CWriterHelper::setBinaryAsChildData(const void* pBuffer, const uint64 ui64BufferLength)
{
	return m_pWriter->setChildData(pBuffer, ui64BufferLength);
}

bool CWriterHelper::setASCIIStringAsChildData(const char* sValue)
{
	return m_pWriter->setChildData(sValue, strlen(sValue));
}

// ________________________________________________________________________________________________________________
//

void CWriterHelper::release(void)
{
	delete this;
}

// ________________________________________________________________________________________________________________
//

EBML_API IWriterHelper* EBML::createWriterHelper(void)
{
	return new CWriterHelper();
}
