#pragma once

#include "IReader.h"

namespace EBML
{
	class EBML_API CReader final : public IReader
	{
	public:

		explicit CReader(IReaderCallback& rReaderCallback);
		~CReader() override;
		bool processData(const void* buffer, uint64_t size) override;
		CIdentifier getCurrentNodeIdentifier() const override;
		uint64_t getCurrentNodeSize() const override;
		void release() override;

	protected:

		IReader* m_pReaderImplementation = nullptr;

	private:

		CReader();
	};
} // namespace EBML
