#include "ovpCBoxAlgorithmReferenceChannel.h"

#include <limits>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace SignalProcessing;

namespace
{
	size_t FindChannel(const IMatrix& matrix, const CString& channel, const CIdentifier& matchMethodID, const size_t start = 0)
	{
		size_t res = std::numeric_limits<size_t>::max();

		if (matchMethodID == Name)
		{
			for (size_t i = start; i < matrix.getDimensionSize(0); ++i)
			{
				if (Toolkit::String::isAlmostEqual(matrix.getDimensionLabel(0, i), channel, false)) { res = i; }
			}
		}
		else if (matchMethodID == Index)
		{
			try
			{
				size_t value = std::stoul(channel.toASCIIString());
				value--; // => makes it 0-indexed !

				if (start <= size_t(value) && size_t(value) < matrix.getDimensionSize(0)) { res = size_t(value); }
			}
			catch (const std::exception&)
			{
				// catch block intentionnaly left blank
			}
		}
		else if (matchMethodID == Smart)
		{
			if (res == std::numeric_limits<size_t>::max()) { res = FindChannel(matrix, channel, Name, start); }
			if (res == std::numeric_limits<size_t>::max()) { res = FindChannel(matrix, channel, Index, start); }
		}

		return res;
	}
} // namespace

bool CBoxAlgorithmReferenceChannel::initialize()
{
	m_decoder.initialize(*this, 0);
	m_encoder.initialize(*this, 0);
	m_encoder.getInputSamplingRate().setReferenceTarget(m_decoder.getOutputSamplingRate());
	return true;
}

bool CBoxAlgorithmReferenceChannel::uninitialize()
{
	m_decoder.uninitialize();
	m_encoder.uninitialize();
	return true;
}

bool CBoxAlgorithmReferenceChannel::processInput(const size_t /*index*/)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmReferenceChannel::process()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();

	for (size_t i = 0; i < boxContext.getInputChunkCount(0); ++i)
	{
		m_decoder.decode(i);
		if (m_decoder.isHeaderReceived())
		{
			IMatrix& iMatrix = *m_decoder.getOutputMatrix();
			IMatrix& oMatrix = *m_encoder.getInputMatrix();

			OV_ERROR_UNLESS_KRF(iMatrix.getDimensionSize(0) >= 2,
								"Invalid input matrix with [" << iMatrix.getDimensionSize(0) << "] channels (expected channels >= 2)",
								ErrorType::BadInput);

			CString channel            = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
			const uint64_t matchMethod = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

			m_referenceChannelIdx = FindChannel(iMatrix, channel, matchMethod, 0);

			OV_ERROR_UNLESS_KRF(m_referenceChannelIdx != std::numeric_limits<size_t>::max(), "Invalid channel [" << channel << "]: channel not found",
								ErrorType::BadSetting);

			if (FindChannel(*m_decoder.getOutputMatrix(), channel, matchMethod, m_referenceChannelIdx + 1) != std::numeric_limits<size_t>::max())
			{
				OV_WARNING_K("Multiple channels match for setting [" << channel << "]. Selecting [" << m_referenceChannelIdx << "]");
			}

			oMatrix.setDimensionCount(2);
			oMatrix.setDimensionSize(0, iMatrix.getDimensionSize(0) - 1);
			oMatrix.setDimensionSize(1, iMatrix.getDimensionSize(1));
			for (size_t j = 0, k = 0; j < iMatrix.getDimensionSize(0); ++j)
			{
				if (j != m_referenceChannelIdx) { oMatrix.setDimensionLabel(0, k++, iMatrix.getDimensionLabel(0, j)); }
			}

			m_encoder.encodeHeader();
		}
		if (m_decoder.isBufferReceived())
		{
			IMatrix& iMatrix      = *m_decoder.getOutputMatrix();
			IMatrix& oMatrix      = *m_encoder.getInputMatrix();
			double* iBuffer       = iMatrix.getBuffer();
			double* oBuffer       = oMatrix.getBuffer();
			double* refBuffer     = iMatrix.getBuffer() + m_referenceChannelIdx * iMatrix.getDimensionSize(1);
			const size_t nChannel = iMatrix.getDimensionSize(0);
			const size_t nSample  = iMatrix.getDimensionSize(1);
			for (size_t j = 0; j < nChannel; ++j)
			{
				if (j != m_referenceChannelIdx)
				{
					for (size_t k = 0; k < nSample; ++k) { oBuffer[k] = iBuffer[k] - refBuffer[k]; }
					oBuffer += nSample;
				}
				iBuffer += nSample;
			}

			m_encoder.encodeBuffer();
		}
		if (m_decoder.isEndReceived()) { m_encoder.encodeEnd(); }
		boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
	}

	return true;
}
