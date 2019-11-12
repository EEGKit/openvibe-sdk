#include "ovpCStreamedMatrixDecoder.h"

#include <system/ovCMemory.h>

#include <cstring>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

namespace
{
	// removes pre and post spaces, tabs and carriage returns
	void trim(char* dst, const char* src1, const char* src2)
	{
		if (!src1 || *src1 == '\0') { dst[0] = '\0'; }
		if (!src2) { src2 = src1 + strlen(src1) - 1; }
		while (src1 < src2 && (*src1 == ' ' || *src1 == '\t' || *src1 == '\r' || *src1 == '\n')) { src1++; }
		while (src1 < src2 && (*src2 == ' ' || *src2 == '\t' || *src2 == '\r' || *src2 == '\n')) { src2--; }
		src2++;
		strncpy(dst, src1, src2 - src1);
		dst[src2 - src1] = '\0';
	}
} // namespace
CStreamedMatrixDecoder::CStreamedMatrixDecoder() {}

// ________________________________________________________________________________________________________________
//

bool CStreamedMatrixDecoder::initialize()
{
	CEBMLBaseDecoder::initialize();
	op_pMatrix.initialize(getOutputParameter(OVP_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));
	return true;
}

bool CStreamedMatrixDecoder::uninitialize()
{
	op_pMatrix.uninitialize();
	CEBMLBaseDecoder::uninitialize();
	return true;
}

// ________________________________________________________________________________________________________________
//

bool CStreamedMatrixDecoder::isMasterChild(const EBML::CIdentifier& identifier)
{
	if (identifier == OVTK_NodeId_Header_StreamedMatrix) { return true; }
	if (identifier == OVTK_NodeId_Header_StreamedMatrix_Dimension) { return true; }
	if (identifier == OVTK_NodeId_Header_StreamedMatrix_DimensionCount) { return false; }
	if (identifier == OVTK_NodeId_Header_StreamedMatrix_Dimension_Size) { return false; }
	if (identifier == OVTK_NodeId_Header_StreamedMatrix_Dimension_Label) { return false; }
	if (identifier == OVTK_NodeId_Buffer_StreamedMatrix) { return true; }
	if (identifier == OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer) { return false; }
	return CEBMLBaseDecoder::isMasterChild(identifier);
}

void CStreamedMatrixDecoder::openChild(const EBML::CIdentifier& identifier)
{
	m_nodes.push(identifier);

	EBML::CIdentifier& top = m_nodes.top();

	if ((top == OVTK_NodeId_Header_StreamedMatrix)
		|| (top == OVTK_NodeId_Header_StreamedMatrix_Dimension)
		|| (top == OVTK_NodeId_Header_StreamedMatrix_DimensionCount)
		|| (top == OVTK_NodeId_Header_StreamedMatrix_Dimension_Size)
		|| (top == OVTK_NodeId_Header_StreamedMatrix_Dimension_Label)
		|| (top == OVTK_NodeId_Buffer_StreamedMatrix)
		|| (top == OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer))
	{
		if (top == OVTK_NodeId_Header_StreamedMatrix && m_status == Status_ParsingNothing)
		{
			m_status   = Status_ParsingHeader;
			m_dimensionIdx = 0;
		}
		else if (top == OVTK_NodeId_Header_StreamedMatrix_Dimension && m_status == Status_ParsingHeader)
		{
			m_status        = Status_ParsingDimension;
			m_dimensionEntryIdx = 0;
		}
		else if (top == OVTK_NodeId_Buffer_StreamedMatrix && m_status == Status_ParsingNothing) { m_status = Status_ParsingBuffer; }
	}
	else { CEBMLBaseDecoder::openChild(identifier); }
}

void CStreamedMatrixDecoder::processChildData(const void* buffer, const uint64_t size)
{
	EBML::CIdentifier& top = m_nodes.top();

	if ((top == OVTK_NodeId_Header_StreamedMatrix)
		|| (top == OVTK_NodeId_Header_StreamedMatrix_Dimension)
		|| (top == OVTK_NodeId_Header_StreamedMatrix_DimensionCount)
		|| (top == OVTK_NodeId_Header_StreamedMatrix_Dimension_Size)
		|| (top == OVTK_NodeId_Header_StreamedMatrix_Dimension_Label)
		|| (top == OVTK_NodeId_Buffer_StreamedMatrix)
		|| (top == OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer))
	{
		switch (m_status)
		{
			case Status_ParsingHeader:
				if (top == OVTK_NodeId_Header_StreamedMatrix_DimensionCount)
				{
					op_pMatrix->setDimensionCount(uint32_t(m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size)));
				}
				break;

			case Status_ParsingDimension:
				if (top == OVTK_NodeId_Header_StreamedMatrix_Dimension_Size)
				{
					op_pMatrix->setDimensionSize(m_dimensionIdx, uint32_t(m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size)));
				}
				if (top == OVTK_NodeId_Header_StreamedMatrix_Dimension_Label)
				{
					char label[1024];
					trim(label, m_pEBMLReaderHelper->getASCIIStringFromChildData(buffer, size), nullptr);
					op_pMatrix->setDimensionLabel(m_dimensionIdx, m_dimensionEntryIdx++, label);
				}
				break;

			case Status_ParsingBuffer:
				if (top == OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer)
				{
					System::Memory::copy(op_pMatrix->getBuffer(), buffer, m_matrixBufferSize * sizeof(double));
				}
				break;
			default: break;
		}
	}
	else { CEBMLBaseDecoder::processChildData(buffer, size); }
}

void CStreamedMatrixDecoder::closeChild()
{
	EBML::CIdentifier& top = m_nodes.top();

	if ((top == OVTK_NodeId_Header_StreamedMatrix)
		|| (top == OVTK_NodeId_Header_StreamedMatrix_Dimension)
		|| (top == OVTK_NodeId_Header_StreamedMatrix_DimensionCount)
		|| (top == OVTK_NodeId_Header_StreamedMatrix_Dimension_Size)
		|| (top == OVTK_NodeId_Header_StreamedMatrix_Dimension_Label)
		|| (top == OVTK_NodeId_Buffer_StreamedMatrix)
		|| (top == OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer))
	{
		if (top == OVTK_NodeId_Buffer_StreamedMatrix && m_status == Status_ParsingBuffer) { m_status = Status_ParsingNothing; }
		else if (top == OVTK_NodeId_Header_StreamedMatrix_Dimension && m_status == Status_ParsingDimension)
		{
			m_status = Status_ParsingHeader;
			m_dimensionIdx++;
		}
		else if (top == OVTK_NodeId_Header_StreamedMatrix && m_status == Status_ParsingHeader)
		{
			m_status = Status_ParsingNothing;

			if (op_pMatrix->getDimensionCount() == 0) { m_matrixBufferSize = 0; }
			else
			{
				m_matrixBufferSize = 1;
				for (uint32_t i = 0; i < op_pMatrix->getDimensionCount(); ++i) { m_matrixBufferSize *= op_pMatrix->getDimensionSize(i); }
			}
		}
	}
	else { CEBMLBaseDecoder::closeChild(); }

	m_nodes.pop();
}
