#ifndef __OpenViBEToolkit_Tools_Matrix_H__
#define __OpenViBEToolkit_Tools_Matrix_H__

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
			OVTK_API bool isDescriptionSimilar(const OpenViBE::IMatrix& rSourceMatrix1, const OpenViBE::IMatrix& rSourceMatrix2, const bool bCheckLabels = true);
			OVTK_API bool isContentSimilar(const OpenViBE::IMatrix& rSourceMatrix1, const OpenViBE::IMatrix& rSourceMatrix2);
			OVTK_API bool isContentValid(const OpenViBE::IMatrix& rSourceMatrix, const bool bCheckNotANumber = true, const bool bCheckInfinity = true);

			OVTK_API bool toString(const OpenViBE::IMatrix& rMatrix, OpenViBE::CString& sString, OpenViBE::uint32 ui32Precision = 6);
			OVTK_API bool fromString(OpenViBE::IMatrix& rMatrix, const OpenViBE::CString& sString);

			OVTK_API bool saveToTextFile(const OpenViBE::IMatrix& rMatrix, const OpenViBE::CString& sFilename, OpenViBE::uint32 ui32Precision = 6);
			OVTK_API bool loadFromTextFile(OpenViBE::IMatrix& rMatrix, const OpenViBE::CString& sFilename);
		};

		namespace MatrixManipulation
		{
			inline bool copy(OpenViBE::IMatrix& rDestinationMatrix, const OpenViBE::IMatrix& rSourceMatrix)
			{
				return OpenViBEToolkit::Tools::Matrix::copy(rDestinationMatrix, rSourceMatrix);
			}

			inline bool copyDescription(OpenViBE::IMatrix& rDestinationMatrix, const OpenViBE::IMatrix& rSourceMatrix)
			{
				return OpenViBEToolkit::Tools::Matrix::copyDescription(rDestinationMatrix, rSourceMatrix);
			}

			inline bool copyContent(OpenViBE::IMatrix& rDestinationMatrix, const OpenViBE::IMatrix& rSourceMatrix)
			{
				return OpenViBEToolkit::Tools::Matrix::copyContent(rDestinationMatrix, rSourceMatrix);
			}

			inline bool clearContent(OpenViBE::IMatrix& rMatrix)
			{
				return OpenViBEToolkit::Tools::Matrix::clearContent(rMatrix);
			}
		};
	};
};

#endif // __OpenViBEToolkit_Tools_Matrix_H__
