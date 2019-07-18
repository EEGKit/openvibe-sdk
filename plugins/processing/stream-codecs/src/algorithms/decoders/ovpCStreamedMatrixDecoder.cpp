#include "ovpCStreamedMatrixDecoder.h"

#include <system/ovCMemory.h>

#include <cstring>
#include <cstdlib>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::StreamCodecs;

namespace
{
	// removes pre and post spaces, tabs and carriage returns
	void trim(char* dst, const char* src1, const char* src2)
	{
		if (!src1 || *src1 == '\0')
		{
			dst[0] = '\0';
		}
		if (!src2)
		{
			src2 = src1 + strlen(src1) - 1;
		}
		while (src1 < src2 && (*src1 == ' ' || *src1 == '\t' || *src1 == '\r' || *src1 == '\n'))
		{
			src1++;
		}
		while (src1 < src2 && (*src2 == ' ' || *src2 == '\t' || *src2 == '\r' || *src2 == '\n'))
		{
			src2--;
		}
		src2++;
		::strncpy(dst, src1, src2 - src1);
		dst[src2 - src1] = '\0';
	}
}

CStreamedMatrixDecoder::CStreamedMatrixDecoder(void)
	: m_ui32Status(Status_ParsingNothing)
	  , m_ui64MatrixBufferSize(0) {}

// ________________________________________________________________________________________________________________
//

bool CStreamedMatrixDecoder::initialize(void)
{
	CEBMLBaseDecoder::initialize();

	op_pMatrix.initialize(getOutputParameter(OVP_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));

	return true;
}

bool CStreamedMatrixDecoder::uninitialize(void)
{
	op_pMatrix.uninitialize();

	CEBMLBaseDecoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CStreamedMatrixDecoder::isMasterChild(const EBML::CIdentifier& rIdentifier)
{
	if (rIdentifier == OVTK_NodeId_Header_StreamedMatrix) return true;
	else if (rIdentifier == OVTK_NodeId_Header_StreamedMatrix_Dimension) return true;
	else if (rIdentifier == OVTK_NodeId_Header_StreamedMatrix_DimensionCount) return false;
	else if (rIdentifier == OVTK_NodeId_Header_StreamedMatrix_Dimension_Size) return false;
	else if (rIdentifier == OVTK_NodeId_Header_StreamedMatrix_Dimension_Label) return false;
	else if (rIdentifier == OVTK_NodeId_Buffer_StreamedMatrix) return true;
	else if (rIdentifier == OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer) return false;
	return CEBMLBaseDecoder::isMasterChild(rIdentifier);
}

void CStreamedMatrixDecoder::openChild(const EBML::CIdentifier& rIdentifier)
{
	m_vNodes.push(rIdentifier);

	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Header_StreamedMatrix)
		|| (l_rTop == OVTK_NodeId_Header_StreamedMatrix_Dimension)
		|| (l_rTop == OVTK_NodeId_Header_StreamedMatrix_DimensionCount)
		|| (l_rTop == OVTK_NodeId_Header_StreamedMatrix_Dimension_Size)
		|| (l_rTop == OVTK_NodeId_Header_StreamedMatrix_Dimension_Label)
		|| (l_rTop == OVTK_NodeId_Buffer_StreamedMatrix)
		|| (l_rTop == OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer))
	{
		if (l_rTop == OVTK_NodeId_Header_StreamedMatrix && m_ui32Status == Status_ParsingNothing)
		{
			m_ui32Status         = Status_ParsingHeader;
			m_ui32DimensionIndex = 0;
		}
		else if (l_rTop == OVTK_NodeId_Header_StreamedMatrix_Dimension && m_ui32Status == Status_ParsingHeader)
		{
			m_ui32Status              = Status_ParsingDimension;
			m_ui32DimensionEntryIndex = 0;
		}
		else if (l_rTop == OVTK_NodeId_Buffer_StreamedMatrix && m_ui32Status == Status_ParsingNothing)
		{
			m_ui32Status = Status_ParsingBuffer;
		}
	}
	else
	{
		CEBMLBaseDecoder::openChild(rIdentifier);
	}
}

void CStreamedMatrixDecoder::processChildData(const void* pBuffer, const uint64_t ui64BufferSize)
{
	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Header_StreamedMatrix)
		|| (l_rTop == OVTK_NodeId_Header_StreamedMatrix_Dimension)
		|| (l_rTop == OVTK_NodeId_Header_StreamedMatrix_DimensionCount)
		|| (l_rTop == OVTK_NodeId_Header_StreamedMatrix_Dimension_Size)
		|| (l_rTop == OVTK_NodeId_Header_StreamedMatrix_Dimension_Label)
		|| (l_rTop == OVTK_NodeId_Buffer_StreamedMatrix)
		|| (l_rTop == OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer))
	{
		switch (m_ui32Status)
		{
			case Status_ParsingHeader:
				if (l_rTop == OVTK_NodeId_Header_StreamedMatrix_DimensionCount) { op_pMatrix->setDimensionCount((uint32_t)m_pEBMLReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize)); }
				break;

			case Status_ParsingDimension:
				if (l_rTop == OVTK_NodeId_Header_StreamedMatrix_Dimension_Size) { op_pMatrix->setDimensionSize(m_ui32DimensionIndex, (uint32_t)m_pEBMLReaderHelper->getUIntegerFromChildData(pBuffer, ui64BufferSize)); }
				if (l_rTop == OVTK_NodeId_Header_StreamedMatrix_Dimension_Label)
				{
					char l_sDimensionLabel[1024];
					::trim(l_sDimensionLabel, m_pEBMLReaderHelper->getASCIIStringFromChildData(pBuffer, ui64BufferSize), NULL);
					op_pMatrix->setDimensionLabel(m_ui32DimensionIndex, m_ui32DimensionEntryIndex++, l_sDimensionLabel);
				}
				break;

			case Status_ParsingBuffer:
				if (l_rTop == OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer) { System::Memory::copy(op_pMatrix->getBuffer(), pBuffer, m_ui64MatrixBufferSize * sizeof(double)); }
				break;
		}
	}
	else
	{
		CEBMLBaseDecoder::processChildData(pBuffer, ui64BufferSize);
	}
}

void CStreamedMatrixDecoder::closeChild(void)
{
	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Header_StreamedMatrix)
		|| (l_rTop == OVTK_NodeId_Header_StreamedMatrix_Dimension)
		|| (l_rTop == OVTK_NodeId_Header_StreamedMatrix_DimensionCount)
		|| (l_rTop == OVTK_NodeId_Header_StreamedMatrix_Dimension_Size)
		|| (l_rTop == OVTK_NodeId_Header_StreamedMatrix_Dimension_Label)
		|| (l_rTop == OVTK_NodeId_Buffer_StreamedMatrix)
		|| (l_rTop == OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer))
	{
		if (l_rTop == OVTK_NodeId_Buffer_StreamedMatrix && m_ui32Status == Status_ParsingBuffer)
		{
			m_ui32Status = Status_ParsingNothing;
		}
		else if (l_rTop == OVTK_NodeId_Header_StreamedMatrix_Dimension && m_ui32Status == Status_ParsingDimension)
		{
			m_ui32Status = Status_ParsingHeader;
			m_ui32DimensionIndex++;
		}
		else if (l_rTop == OVTK_NodeId_Header_StreamedMatrix && m_ui32Status == Status_ParsingHeader)
		{
			m_ui32Status = Status_ParsingNothing;

			if (op_pMatrix->getDimensionCount() == 0)
			{
				m_ui64MatrixBufferSize = 0;
			}
			else
			{
				m_ui64MatrixBufferSize = 1;
				for (uint32_t i = 0; i < op_pMatrix->getDimensionCount(); i++)
				{
					m_ui64MatrixBufferSize *= op_pMatrix->getDimensionSize(i);
				}
			}
		}
	}
	else
	{
		CEBMLBaseDecoder::closeChild();
	}

	m_vNodes.pop();
}
