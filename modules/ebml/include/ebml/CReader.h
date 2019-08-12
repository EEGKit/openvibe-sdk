#pragma once

#include "IReader.h"

namespace EBML
{
	class EBML_API CReader : public IReader
	{
	public:

		explicit CReader(IReaderCallback& rReaderCallback);
		~CReader() override;
		bool processData(const void* pBuffer, uint64_t ui64BufferSize) override;
		CIdentifier getCurrentNodeIdentifier() const override;
		uint64_t getCurrentNodeSize() const override;
		void release() override;

	protected:

		IReader* m_pReaderImplementation = nullptr;

	private:

		CReader();
	};
}  // namespace EBML
