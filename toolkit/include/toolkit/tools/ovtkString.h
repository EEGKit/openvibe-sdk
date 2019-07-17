#ifndef __OpenViBEToolkit_Tools_String_H__
#define __OpenViBEToolkit_Tools_String_H__

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

				virtual ~ISplitCallback(void) { }

				virtual void beginSplit(void) const { }

				virtual void endSplit(void) const { }

				virtual void setToken(const char* sToken) const =0;
			};

			template <class TContainer>
			class TSplitCallback : public ISplitCallback
			{
			private:

				TSplitCallback(void);

			public:

				TSplitCallback(TContainer& vTokenContainer)
					: m_pTokenContainer(&vTokenContainer) { }

				void setToken(const char* sToken) const
				{
					m_pTokenContainer->push_back(sToken);
				}

			protected:

				TContainer* m_pTokenContainer;
			};

			OVTK_API OpenViBE::uint32 split(const OpenViBE::CString& rString, const ISplitCallback& rSplitCallback, OpenViBE::uint8 ui8Separator);
			OVTK_API OpenViBE::uint32 split(const OpenViBE::CString& rString, const ISplitCallback& rSplitCallback, OpenViBE::uint8* pSeparator, OpenViBE::uint32 ui32SeparatorCount);
			OVTK_API bool isAlmostEqual(const OpenViBE::CString& rString1, const OpenViBE::CString& rString2, const bool bCaseSensitive = true, const bool bRemoveStartSpaces = true, const bool bRemoveEndSpaces = true);
		};
	};
};

#endif // __OpenViBEToolkit_Tools_String_H__
