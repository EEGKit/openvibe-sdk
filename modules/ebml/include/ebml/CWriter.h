#pragma once

#include "IWriter.h"

namespace EBML
{
	class EBML_API CWriter : public IWriter
	{
	public:

		explicit CWriter(IWriterCallback& rWriterCallback);
		~CWriter() override;
		bool openChild(const CIdentifier& rIdentifier) override;
		bool setChildData(const void* pBuffer, const uint64_t ui64BufferSize) override;
		bool closeChild() override;
		void release() override;

	protected:

		IWriter* m_pWriterImplementation = nullptr;

	private:

		CWriter();
	};
}  // namespace EBML
