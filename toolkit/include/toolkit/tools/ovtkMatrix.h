#pragma once

#include "../ovtk_base.h"

namespace OpenViBE {
namespace Toolkit {
namespace Matrix {
OVTK_API bool copy(CMatrix& dst, const CMatrix& src);
OVTK_API bool copyDescription(CMatrix& dst, const CMatrix& src);
OVTK_API bool copyContent(CMatrix& dst, const CMatrix& src);

OVTK_API bool toString(const CMatrix& matrix, CString& str, const size_t precision = 6);
OVTK_API bool fromString(CMatrix& matrix, const CString& str);

OVTK_API bool saveToTextFile(const CMatrix& matrix, const CString& filename, const size_t precision = 6);
OVTK_API bool loadFromTextFile(CMatrix& matrix, const CString& filename);
}  // namespace Matrix
}  // namespace Toolkit
}  // namespace OpenViBE
