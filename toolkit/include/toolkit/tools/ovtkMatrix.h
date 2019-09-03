#pragma once

#include "../ovtk_base.h"

namespace OpenViBEToolkit
{
	namespace Tools
	{
		namespace Matrix
		{
			OVTK_API bool copy(OpenViBE::IMatrix& dst, const OpenViBE::IMatrix& src);
			OVTK_API bool copyDescription(OpenViBE::IMatrix& dst, const OpenViBE::IMatrix& src);
			OVTK_API bool copyContent(OpenViBE::IMatrix& dst, const OpenViBE::IMatrix& src);
			OVTK_API bool clearContent(OpenViBE::IMatrix& rMatrix);
			OVTK_API bool isDescriptionSimilar(const OpenViBE::IMatrix& src1, const OpenViBE::IMatrix& src2, bool bCheckLabels = true);
			OVTK_API bool isContentSimilar(const OpenViBE::IMatrix& src1, const OpenViBE::IMatrix& src2);
			OVTK_API bool isContentValid(const OpenViBE::IMatrix& src, bool bCheckNotANumber = true, bool bCheckInfinity = true);

			OVTK_API bool toString(const OpenViBE::IMatrix& rMatrix, OpenViBE::CString& sString, uint32_t ui32Precision = 6);
			OVTK_API bool fromString(OpenViBE::IMatrix& rMatrix, const OpenViBE::CString& sString);

			OVTK_API bool saveToTextFile(const OpenViBE::IMatrix& rMatrix, const OpenViBE::CString& sFilename, uint32_t ui32Precision = 6);
			OVTK_API bool loadFromTextFile(OpenViBE::IMatrix& rMatrix, const OpenViBE::CString& sFilename);
		} // namespace Matrix

		namespace MatrixManipulation
		{
			inline bool copy(OpenViBE::IMatrix& dst, const OpenViBE::IMatrix& src) { return Matrix::copy(dst, src); }
			inline bool copyDescription(OpenViBE::IMatrix& dst, const OpenViBE::IMatrix& src) { return Matrix::copyDescription(dst, src); }
			inline bool copyContent(OpenViBE::IMatrix& dst, const OpenViBE::IMatrix& src) { return Matrix::copyContent(dst, src); }
			inline bool clearContent(OpenViBE::IMatrix& rMatrix) { return Matrix::clearContent(rMatrix); }
		} // namespace MatrixManipulation
	} // namespace Tools
} // namespace OpenViBEToolkit
