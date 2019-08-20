#pragma once

#include "../../ovk_base.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Kernel
	{
		class CBuffer : public CMemoryBuffer
		{
		public:

			CBuffer() { }
			explicit CBuffer(const CBuffer& rBuffer);

			CBuffer& operator=(const CBuffer& rBuffer);
		};
	}  // namespace Kernel
}  // namespace OpenViBE
