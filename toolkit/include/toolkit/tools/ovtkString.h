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
				virtual void setToken(const char* sToken) const = 0;
			};

			template <class TContainer>
			class TSplitCallback final : public ISplitCallback
			{
			public:

				TSplitCallback(TContainer& vTokenContainer) : m_pTokenContainer(&vTokenContainer) { }

				void setToken(const char* sToken) const override { m_pTokenContainer->push_back(sToken); }

			protected:

				TContainer* m_pTokenContainer = nullptr;
			};

			OVTK_API uint32_t split(const OpenViBE::CString& rString, const ISplitCallback& splitCB, uint8_t separator);
			OVTK_API uint32_t split(const OpenViBE::CString& rString, const ISplitCallback& splitCB, uint8_t* separator, uint32_t nSeparator);
			OVTK_API bool isAlmostEqual(const OpenViBE::CString& rString1, const OpenViBE::CString& rString2, bool bCaseSensitive = true,
										bool bRemoveStartSpaces                                                                   = true,
										bool bRemoveEndSpaces                                                                     = true);
		} // namespace String
	} // namespace Tools
} // namespace OpenViBEToolkit
