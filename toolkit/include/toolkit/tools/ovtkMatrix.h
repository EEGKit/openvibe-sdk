#pragma once

#include "../ovtk_base.h"

namespace OpenViBE {
namespace Toolkit {
namespace Matrix {
// Can be used : CMatrix class behaviour is a little different but this function is never used (except in test). Avoid to used it, intended to be removed. 
OVTK_API bool toString(const CMatrix& matrix, CString& str, const size_t precision = 6);
OVTK_API bool fromString(CMatrix& matrix, const CString& str);
OVTK_API bool saveToTextFile(const CMatrix& matrix, const CString& filename, const size_t precision = 6);
OVTK_API bool loadFromTextFile(CMatrix& matrix, const CString& filename);

//@todo All is useless now. Avoid to used it, intended to be removed.  
OVTK_API bool copy(CMatrix& dst, const CMatrix& src);
OVTK_API bool copyDescription(CMatrix& dst, const CMatrix& src);
OVTK_API bool copyContent(CMatrix& dst, const CMatrix& src);
OVTK_API bool clearContent(CMatrix& matrix);															//(resetBuffer)
OVTK_API bool isDescriptionSimilar(const CMatrix& src1, const CMatrix& src2, bool checkLabels = true);	//(isDescriptionEqual)
OVTK_API bool isContentSimilar(const CMatrix& src1, const CMatrix& src2);								//(isBufferEqual)
OVTK_API bool isContentValid(const CMatrix& src, const bool checkNotANumber = true, const bool checkInfinity = true);	//(isBufferValid)
}  // namespace Matrix

//@todo All is useless now. Avoid to used it, intended to be removed. 
namespace MatrixManipulation {
inline bool copy(CMatrix& dst, const CMatrix& src) { return Matrix::copy(dst, src); }
inline bool copyDescription(CMatrix& dst, const CMatrix& src) { return Matrix::copyDescription(dst, src); }
inline bool copyContent(CMatrix& dst, const CMatrix& src) { return Matrix::copyContent(dst, src); }
inline bool clearContent(CMatrix& rMatrix) { return Matrix::clearContent(rMatrix); }
}  // namespace MatrixManipulation
}  // namespace Toolkit
}  // namespace OpenViBE
