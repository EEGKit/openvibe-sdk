#pragma once

#include "../ovtk_base.h"

namespace OpenViBEToolkit
{
	namespace Tools
	{
		namespace Matrix
		{
			OVTK_API bool copy(OpenViBE::IMatrix& rDestinationMatrix, const OpenViBE::IMatrix& rSourceMatrix);
			OVTK_API bool copyDescription(OpenViBE::IMatrix& rDestinationMatrix, const OpenViBE::IMatrix& rSourceMatrix);
			OVTK_API bool copyContent(OpenViBE::IMatrix& rDestinationMatrix, const OpenViBE::IMatrix& rSourceMatrix);
			OVTK_API bool clearContent(OpenViBE::IMatrix& rMatrix);
			OVTK_API bool isDescriptionSimilar(const OpenViBE::IMatrix& rSourceMatrix1, const OpenViBE::IMatrix& rSourceMatrix2, bool bCheckLabels = true);
			OVTK_API bool isContentSimilar(const OpenViBE::IMatrix& rSourceMatrix1, const OpenViBE::IMatrix& rSourceMatrix2);
			OVTK_API bool isContentValid(const OpenViBE::IMatrix& rSourceMatrix, bool bCheckNotANumber = true, bool bCheckInfinity = true);

			OVTK_API bool toString(const OpenViBE::IMatrix& rMatrix, OpenViBE::CString& sString, uint32_t ui32Precision = 6);
			OVTK_API bool fromString(OpenViBE::IMatrix& rMatrix, const OpenViBE::CString& sString);

			OVTK_API bool saveToTextFile(const OpenViBE::IMatrix& rMatrix, const OpenViBE::CString& sFilename, uint32_t ui32Precision = 6);
			OVTK_API bool loadFromTextFile(OpenViBE::IMatrix& rMatrix, const OpenViBE::CString& sFilename);
		};

		namespace MatrixManipulation
		{
			inline bool copy(OpenViBE::IMatrix& rDestinationMatrix, const OpenViBE::IMatrix& rSourceMatrix)
			{
				return Matrix::copy(rDestinationMatrix, rSourceMatrix);
			}

			inline bool copyDescription(OpenViBE::IMatrix& rDestinationMatrix, const OpenViBE::IMatrix& rSourceMatrix)
			{
				return Matrix::copyDescription(rDestinationMatrix, rSourceMatrix);
			}

			inline bool copyContent(OpenViBE::IMatrix& rDestinationMatrix, const OpenViBE::IMatrix& rSourceMatrix)
			{
				return Matrix::copyContent(rDestinationMatrix, rSourceMatrix);
			}

			inline bool clearContent(OpenViBE::IMatrix& rMatrix)
			{
				return Matrix::clearContent(rMatrix);
			}
		};
	};
};
