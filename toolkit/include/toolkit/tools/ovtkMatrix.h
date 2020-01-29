#pragma once

#include "../ovtk_base.h"

namespace OpenViBE
{
	namespace Toolkit
	{
	namespace Tools
	{
		namespace Matrix
		{
			OVTK_API bool copy(IMatrix& dst, const IMatrix& src);
			OVTK_API bool copyDescription(IMatrix& dst, const IMatrix& src);
			OVTK_API bool copyContent(IMatrix& dst, const IMatrix& src);
			OVTK_API bool clearContent(IMatrix& matrix);
			OVTK_API bool isDescriptionSimilar(const IMatrix& src1, const IMatrix& src2, bool checkLabels = true);
			OVTK_API bool isContentSimilar(const IMatrix& src1, const IMatrix& src2);
			OVTK_API bool isContentValid(const IMatrix& src, const bool checkNotANumber = true, const bool checkInfinity = true);

			OVTK_API bool toString(const IMatrix& matrix, CString& sString, const size_t precision = 6);
			OVTK_API bool fromString(IMatrix& matrix, const CString& sString);

			OVTK_API bool saveToTextFile(const IMatrix& matrix, const CString& filename, const size_t precision = 6);
			OVTK_API bool loadFromTextFile(IMatrix& matrix, const CString& filename);
		} // namespace Matrix

		namespace MatrixManipulation
		{
			inline bool copy(IMatrix& dst, const IMatrix& src) { return Matrix::copy(dst, src); }
			inline bool copyDescription(IMatrix& dst, const IMatrix& src) { return Matrix::copyDescription(dst, src); }
			inline bool copyContent(IMatrix& dst, const IMatrix& src) { return Matrix::copyContent(dst, src); }
			inline bool clearContent(IMatrix& rMatrix) { return Matrix::clearContent(rMatrix); }
		} // namespace MatrixManipulation
	} // namespace Tools
	}  // namespace Toolkit
}  // namespace OpenViBE
