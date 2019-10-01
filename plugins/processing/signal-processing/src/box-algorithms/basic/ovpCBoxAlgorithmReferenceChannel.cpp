#include "ovpCBoxAlgorithmReferenceChannel.h"

#include <cstdio>
#include <limits>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

namespace
{
	uint32_t FindChannel(const IMatrix& matrix, const CString& channel, const CIdentifier& matchMethodID, const uint32_t start = 0)
	{
		uint32_t res = std::numeric_limits<uint32_t>::max();

		if (matchMethodID == OVP_TypeId_MatchMethod_Name)
		{
			for (uint32_t i = start; i < matrix.getDimensionSize(0); i++)
			{
				if (OpenViBEToolkit::Tools::String::isAlmostEqual(matrix.getDimensionLabel(0, i), channel, false)) { res = i; }
			}
		}
		else if (matchMethodID == OVP_TypeId_MatchMethod_Index)
		{
			try
			{
				uint32_t value = std::stoul(channel.toASCIIString());
				value--; // => makes it 0-indexed !

				if (start <= uint32_t(value) && uint32_t(value) < matrix.getDimensionSize(0)) { res = uint32_t(value); }
			}
			catch (const std::exception&)
			{
				// catch block intentionnaly left blank
			}
		}
		else if (matchMethodID == OVP_TypeId_MatchMethod_Smart)
		{
			if (res == std::numeric_limits<uint32_t>::max())
			{
				res = FindChannel(matrix, channel, OVP_TypeId_MatchMethod_Name, start);
			}
			if (res == std::numeric_limits<uint32_t>::max())
			{
				res = FindChannel(matrix, channel, OVP_TypeId_MatchMethod_Index, start);
			}
		}

		return res;
	}
} // namespace

bool CBoxAlgorithmReferenceChannel::initialize()
{
	m_oDecoder.initialize(*this, 0);
	m_oEncoder.initialize(*this, 0);
	m_oEncoder.getInputSamplingRate().setReferenceTarget(m_oDecoder.getOutputSamplingRate());
	return true;
}

bool CBoxAlgorithmReferenceChannel::uninitialize()
{
	m_oDecoder.uninitialize();
	m_oEncoder.uninitialize();
	return true;
}

bool CBoxAlgorithmReferenceChannel::processInput(const uint32_t /*index*/)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

#include <cstdio>

bool CBoxAlgorithmReferenceChannel::process()
{
	// IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& boxContext = this->getDynamicBoxContext();

	for (uint32_t i = 0; i < boxContext.getInputChunkCount(0); i++)
	{
		m_oDecoder.decode(i);
		if (m_oDecoder.isHeaderReceived())
		{
			IMatrix& iMatrix  = *m_oDecoder.getOutputMatrix();
			IMatrix& oMatrix = *m_oEncoder.getInputMatrix();

			OV_ERROR_UNLESS_KRF(iMatrix.getDimensionSize(0) >= 2,
								"Invalid input matrix with [" << iMatrix.getDimensionSize(0) << "] channels (expected channels >= 2)",
								OpenViBE::Kernel::ErrorType::BadInput);

			CString channel         = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
			const uint64_t matchMethod = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

			m_referenceChannelIdx = FindChannel(iMatrix, channel, matchMethod, 0);

			OV_ERROR_UNLESS_KRF(m_referenceChannelIdx != std::numeric_limits<uint32_t>::max(),
								"Invalid channel [" << channel << "]: channel not found", OpenViBE::Kernel::ErrorType::BadSetting);

			if (FindChannel(*m_oDecoder.getOutputMatrix(), channel, matchMethod, m_referenceChannelIdx + 1) != std::numeric_limits<uint32_t>::
				max()) { OV_WARNING_K("Multiple channels match for setting [" << channel << "]. Selecting [" << m_referenceChannelIdx << "]"); }

			oMatrix.setDimensionCount(2);
			oMatrix.setDimensionSize(0, iMatrix.getDimensionSize(0) - 1);
			oMatrix.setDimensionSize(1, iMatrix.getDimensionSize(1));
			for (uint32_t j = 0, k = 0; j < iMatrix.getDimensionSize(0); j++)
			{
				if (j != m_referenceChannelIdx) { oMatrix.setDimensionLabel(0, k++, iMatrix.getDimensionLabel(0, j)); }
			}

			m_oEncoder.encodeHeader();
		}
		if (m_oDecoder.isBufferReceived())
		{
			IMatrix& iMatrix     = *m_oDecoder.getOutputMatrix();
			IMatrix& oMatrix    = *m_oEncoder.getInputMatrix();
			double* iBuffer      = iMatrix.getBuffer();
			double* oBuffer     = oMatrix.getBuffer();
			double* refBuffer  = iMatrix.getBuffer() + m_referenceChannelIdx * iMatrix.getDimensionSize(1);
			const uint32_t nChannel = iMatrix.getDimensionSize(0);
			const uint32_t nSample  = iMatrix.getDimensionSize(1);
			for (uint32_t j = 0; j < nChannel; j++)
			{
				if (j != m_referenceChannelIdx)
				{
					for (uint32_t k = 0; k < nSample; k++) { oBuffer[k] = iBuffer[k] - refBuffer[k]; }
					oBuffer += nSample;
				}
				iBuffer += nSample;
			}

			m_oEncoder.encodeBuffer();
		}
		if (m_oDecoder.isEndReceived()) { m_oEncoder.encodeEnd(); }
		boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
	}

	return true;
}
