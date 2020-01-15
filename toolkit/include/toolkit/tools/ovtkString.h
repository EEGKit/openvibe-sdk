#pragma once

#include "../ovtk_base.h"

namespace OpenViBEToolkit
{
	namespace Tools
	{
		namespace String
		{
			class OVTK_API ISplitCallback
			{
			public:
				virtual ~ISplitCallback() { }
				virtual void beginSplit() const { }
				virtual void endSplit() const { }
				virtual void setToken(const char* token) const = 0;
			};

			template <class TContainer>
			class TSplitCallback final : public ISplitCallback
			{
			public:

				explicit TSplitCallback(TContainer& tokenContainers) : m_tokenContainer(&tokenContainers) { }

				void setToken(const char* token) const override { m_tokenContainer->push_back(token); }

			protected:

				TContainer* m_tokenContainer = nullptr;
			};

			OVTK_API size_t split(const OpenViBE::CString& rString, const ISplitCallback& splitCB, uint8_t separator);
			OVTK_API size_t split(const OpenViBE::CString& rString, const ISplitCallback& splitCB, uint8_t* separator, const size_t nSeparator);
			OVTK_API bool isAlmostEqual(const OpenViBE::CString& rString1, const OpenViBE::CString& rString2, bool bCaseSensitive = true,
										bool bRemoveStartSpaces                                                                   = true,
										bool bRemoveEndSpaces                                                                     = true);
		} // namespace String
	} // namespace Tools
} // namespace OpenViBEToolkit
