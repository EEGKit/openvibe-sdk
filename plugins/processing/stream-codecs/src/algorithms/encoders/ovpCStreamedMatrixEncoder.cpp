#include "ovpCStreamedMatrixEncoder.h"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

bool CStreamedMatrixEncoder::initialize()
{
	CEBMLBaseEncoder::initialize();
	ip_pMatrix.initialize(getInputParameter(OVP_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix));
	m_matrixBufferSize = 0;
	return true;
}

bool CStreamedMatrixEncoder::uninitialize()
{
	ip_pMatrix.uninitialize();
	CEBMLBaseEncoder::uninitialize();
	return true;
}

// ________________________________________________________________________________________________________________
//

bool CStreamedMatrixEncoder::processHeader()
{
	IMatrix* matrix = ip_pMatrix;
	uint32_t j;

	m_matrixBufferSize = (matrix->getDimensionCount() == 0 ? 0 : 1);

	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_StreamedMatrix);
	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_StreamedMatrix_DimensionCount);
	m_pEBMLWriterHelper->setUIntegerAsChildData(matrix->getDimensionCount());
	m_pEBMLWriterHelper->closeChild();
	for (uint32_t i = 0; i < matrix->getDimensionCount(); i++)
	{
		m_matrixBufferSize *= matrix->getDimensionSize(i);
		m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_StreamedMatrix_Dimension);
		m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_StreamedMatrix_Dimension_Size);
		m_pEBMLWriterHelper->setUIntegerAsChildData(matrix->getDimensionSize(i));
		m_pEBMLWriterHelper->closeChild();
		bool shouldSendLabels = false;
		for (j = 0; j < matrix->getDimensionSize(i) && !shouldSendLabels; j++)
		{
			if (matrix->getDimensionLabel(i, j) != nullptr && matrix->getDimensionLabel(i, j)[0] != '\0') { shouldSendLabels = true; }
		}
		if (shouldSendLabels)
		{
			for (j = 0; j < matrix->getDimensionSize(i); j++)
			{
				m_pEBMLWriterHelper->openChild(OVTK_NodeId_Header_StreamedMatrix_Dimension_Label);
				m_pEBMLWriterHelper->setASCIIStringAsChildData(matrix->getDimensionLabel(i, j));
				m_pEBMLWriterHelper->closeChild();
			}
		}
		m_pEBMLWriterHelper->closeChild();
	}
	m_pEBMLWriterHelper->closeChild();

	return true;
}

bool CStreamedMatrixEncoder::processBuffer()
{
	IMatrix* matrix = ip_pMatrix;

	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Buffer_StreamedMatrix);
	m_pEBMLWriterHelper->openChild(OVTK_NodeId_Buffer_StreamedMatrix_RawBuffer);
	m_pEBMLWriterHelper->setBinaryAsChildData(matrix->getBuffer(), m_matrixBufferSize * sizeof(double));
	m_pEBMLWriterHelper->closeChild();
	m_pEBMLWriterHelper->closeChild();

	return true;
}
