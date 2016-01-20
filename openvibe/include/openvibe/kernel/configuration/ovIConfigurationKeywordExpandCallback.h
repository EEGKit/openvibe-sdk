#ifndef __OpenViBE_Kernel_Configuration_IConfigurationKeywordExpandCallback_H__
#define __OpenViBE_Kernel_Configuration_IConfigurationKeywordExpandCallback_H__

#include <ov_common_defines.h>
#include "../../ov_types.h"

namespace OpenViBE
{
	class CString;

	namespace Kernel
	{
		/**
		 * \class IConfigurationKeywordExpandCallback
		 * \author Jozef Legeny (Mensia Technologies)
		 * \date 2014-05-06
		 * \brief Callback used for overriding a keyword in IConfigurationManager
		 * \ingroup Group_Configuration
		 * \ingroup Group_Kernel
		 */
		class OV_API IConfigurationKeywordExpandCallback
		{
		public:

			virtual ~IConfigurationKeywordExpandCallback()
			{}

			virtual OpenViBE::boolean expand(
			        const OpenViBE::CString& rStringToExpand,
			        OpenViBE::CString& rExpandedString) const = 0;

		};
	}
}

#endif // __OpenViBE_Kernel_Configuration_IConfigurationKeywordExpandCallback_H__
