/*********************************************************************
 * Software License Agreement (AGPL-3 License)
 *
 * \file CBoxAlgorithmOVCSVFileReader.cpp
 * \brief Implementation of the box CSV File Reader
 * \author Victor Herlin (Mensia), Thomas Prampart (Inria)
 * \version 1.1.0
 * \date Fri May 7 16:40:49 2021.
 *
 * \copyright (C) 2006-2021 INRIA
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#include <sstream>
#include <map>
#include <algorithm>
#include <utility>

#include "CBoxAlgorithmOVCSVFileReader.hpp"

namespace OpenViBE {
namespace Plugins {
namespace FileIO {

bool CBoxAlgorithmOVCSVFileReader::initialize()
{
	m_sampling                = 0;
	m_isHeaderSent            = false;
	m_isStimulationHeaderSent = false;
	m_nSamplePerBuffer = 1;

	this->getStaticBoxContext().getOutputType(0, m_typeID);

	const CString filename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	OV_ERROR_UNLESS_KRF(m_readerLib->openFile(filename.toASCIIString(), CSV::EFileAccessMode::Read),
						(CSV::ICSVHandler::getLogError(m_readerLib->getLastLogError()) + (m_readerLib->getLastErrorString().empty() ? "" : ". Details: " +
							m_readerLib->getLastErrorString())).c_str(), Kernel::ErrorType::Internal);


	OV_ERROR_UNLESS_KRF(m_stimEncoder.initialize(*this, 1), "Error during stimulation encoder initialize", Kernel::ErrorType::Internal);

	OV_ERROR_UNLESS_KRF(m_readerLib->parseHeader(),
	                   (CSV::ICSVHandler::getLogError(m_readerLib->getLastLogError()) + (m_readerLib->getLastErrorString().empty() ? "" : ". Details: " +
	                                                                                                                                      m_readerLib->getLastErrorString())).c_str(), Kernel::ErrorType::Internal);

	if (m_typeID == OV_TypeId_Signal && m_readerLib->getFormatType() == CSV::EStreamType::Signal)
	{
		m_algorithmEncoder = new Toolkit::TSignalEncoder<CBoxAlgorithmOVCSVFileReader>(*this, 0);
		m_readerLib->getSignalInformation(m_channelNames, m_sampling, m_nSamplePerBuffer);
	}
	else if ((m_typeID == OV_TypeId_StreamedMatrix || m_typeID == OV_TypeId_CovarianceMatrix)
	         && (m_readerLib->getFormatType() == CSV::EStreamType::StreamedMatrix || m_readerLib->getFormatType() == CSV::EStreamType::FeatureVector))
	{
		m_algorithmEncoder = new Toolkit::TStreamedMatrixEncoder<CBoxAlgorithmOVCSVFileReader>(*this, 0);
		m_readerLib->getStreamedMatrixInformation(m_dimSizes, m_channelNames);
	}
	else if (m_typeID == OV_TypeId_FeatureVector && m_readerLib->getFormatType() == CSV::EStreamType::FeatureVector)
	{
		m_algorithmEncoder = new Toolkit::TFeatureVectorEncoder<CBoxAlgorithmOVCSVFileReader>(*this, 0);
		m_readerLib->getFeatureVectorInformation(m_channelNames);
	}
	else if (m_typeID == OV_TypeId_Spectrum && m_readerLib->getFormatType() == CSV::EStreamType::Spectrum)
	{
		m_algorithmEncoder = new Toolkit::TSpectrumEncoder<CBoxAlgorithmOVCSVFileReader>(*this, 0);
		m_readerLib->getSpectrumInformation(m_channelNames, m_frequencyAbscissa, m_sampling);
	}
	else if (m_readerLib->getFormatType() == CSV::EStreamType::Stimulations)
	{
		this->getLogManager() << Kernel::LogLevel_Info << "File contains only stimulations\n";
	}
	else
	{
		this->getLogManager() << Kernel::LogLevel_Error
							  << "File content type [" << CSV::toString(m_readerLib->getFormatType())
							  << "] not matching box output [" << this->getTypeManager().getTypeName(m_typeID) << "].\n";
		return false;
	}

	return true;
}

bool CBoxAlgorithmOVCSVFileReader::uninitialize()
{
	m_channelNames.clear();
	m_dimSizes.clear();
	m_frequencyAbscissa.clear();

	m_algorithmEncoder.uninitialize();

	OV_ERROR_UNLESS_KRF(m_stimEncoder.uninitialize(), "Failed to uninitialize stimulation encoder", Kernel::ErrorType::Internal);

	return true;
}

bool CBoxAlgorithmOVCSVFileReader::processClock(Kernel::CMessageClock& /*msg*/)
{
	OV_ERROR_UNLESS_KRF(getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess(),
						"Failed to mark clock algorithm as ready to process",
						Kernel::ErrorType::Internal);
	return true;
}

bool CBoxAlgorithmOVCSVFileReader::process()
{
	if (m_readerLib->getFormatType() == CSV::EStreamType::Stimulations)
	{
		// If the file contains only stimulations
		const double currentTime = CTime(this->getPlayerContext().getCurrentTime()).toSeconds();
		std::vector<CSV::SStimulationChunk> stimulationChunk;

		if (!m_readerLib->hasDataToRead() && m_savedStimulations.empty()) { return true; }
		if (m_savedStimulations.empty())
		{
			OV_ERROR_UNLESS_KRF(m_readerLib->readEventsFromFile(5, stimulationChunk),
			                    (CSV::ICSVHandler::getLogError(m_readerLib->getLastLogError()) +
			                     (m_readerLib->getLastErrorString().empty() ? "" : ". Details: "
			                                                                       + m_readerLib->getLastErrorString())).c_str(),
			                    Kernel::ErrorType::Internal);
			m_savedStimulations.insert(m_savedStimulations.end(), stimulationChunk.begin(), stimulationChunk.end());
		}

		return processStimulation(CTime(m_lastStimulationDate).toSeconds(), currentTime);
	}

	if (m_readerLib->getFormatType() != CSV::EStreamType::Undefined)
	{
		// If the file contains data chunks and potentially stimulations.
		return processChunksAndStimulations();
	}

	// Undefined Stream Type.
	this->getLogManager() << Kernel::LogLevel_Error << "Cannot process file with undefined format\n";
	return false;
}

bool CBoxAlgorithmOVCSVFileReader::processChunksAndStimulations()
{
	Kernel::IBoxIO& boxContext = this->getDynamicBoxContext();
	CMatrix* matrix            = m_algorithmEncoder.getInputMatrix();

	// encode Header if not already encoded
	if (!m_isHeaderSent)
	{
		if (m_typeID == OV_TypeId_Signal)
		{
			matrix->resize(m_channelNames.size(), m_nSamplePerBuffer);
			size_t index = 0;

			for (const std::string& channelName : m_channelNames)
			{
				OV_ERROR_UNLESS_K(matrix->setDimensionLabel(0, index++, channelName.c_str()), "Failed to set dimension label", Kernel::ErrorType::Internal, false);
			}

			m_algorithmEncoder.getInputSamplingRate() = m_sampling;
		}
		else if (m_typeID == OV_TypeId_StreamedMatrix || m_typeID == OV_TypeId_CovarianceMatrix)
		{
			OV_ERROR_UNLESS_K(matrix->setDimensionCount(m_dimSizes.size()), "Failed to set dimension count", Kernel::ErrorType::Internal, false);
			size_t prevDimSize = 0;

			for (size_t d1 = 0; d1 < m_dimSizes.size(); ++d1)
			{
				OV_ERROR_UNLESS_K(matrix->setDimensionSize(d1, m_dimSizes[d1]), "Failed to set dimension size " << d1 + 1, Kernel::ErrorType::Internal, false);

				for (size_t d2 = 0; d2 < m_dimSizes[d1]; ++d2)
				{
					OV_ERROR_UNLESS_K(matrix->setDimensionLabel(d1, d2, m_channelNames[prevDimSize + d2].c_str()),
									  "Failed to set dimension label",
					                  Kernel::ErrorType::Internal,
					                  false);
				}

				prevDimSize += m_dimSizes[d1];
			}
		}
		else if (m_typeID == OV_TypeId_FeatureVector)
		{
			matrix->resize(m_channelNames.size());

			size_t index = 0;
			for (const std::string& channelName : m_channelNames)
			{
				OV_ERROR_UNLESS_K(matrix->setDimensionLabel(0, index++, channelName.c_str()), "Failed to set dimension label", Kernel::ErrorType::Internal, false);
			}
		}
		else if (m_typeID == OV_TypeId_Spectrum)
		{
			CMatrix* frequencyAbscissaMatrix = m_algorithmEncoder.getInputFrequencyAbcissa();

			matrix->resize(m_channelNames.size(), m_frequencyAbscissa.size());
			frequencyAbscissaMatrix->resize(m_frequencyAbscissa.size());

			size_t index = 0;
			for (const std::string& channelName : m_channelNames)
			{
				OV_ERROR_UNLESS_K(matrix->setDimensionLabel(0, index++, channelName.c_str()), "Failed to set dimension label", Kernel::ErrorType::Internal, false);
			}

			index = 0;
			for (const double& frequencyAbscissaValue : m_frequencyAbscissa)
			{
				frequencyAbscissaMatrix->getBuffer()[index] = frequencyAbscissaValue;
				OV_ERROR_UNLESS_K(matrix->setDimensionLabel(1, index++, std::to_string(frequencyAbscissaValue).c_str()),
					              "Failed to set dimension label",
				                  Kernel::ErrorType::Internal,
				                  false);
			}

			m_algorithmEncoder.getInputSamplingRate() = m_sampling;
		}

		OV_ERROR_UNLESS_KRF(m_algorithmEncoder.encodeHeader(), "Failed to encode signal header", Kernel::ErrorType::Internal);

		m_isHeaderSent = true;
		OV_ERROR_UNLESS_KRF(boxContext.markOutputAsReadyToSend(0, 0, 0), "Failed to mark signal header as ready to send", Kernel::ErrorType::Internal);
	}
	const double currentTime = CTime(this->getPlayerContext().getCurrentTime()).toSeconds();

	if (!m_readerLib->hasDataToRead() && m_savedChunks.empty()) { return true; }

	// Fill the chunk buffer if there is no enough data.
	if ((m_savedChunks.empty() || m_savedChunks.back().startTime < currentTime) && m_readerLib->hasDataToRead())
	{
		do
		{
			std::vector<CSV::SMatrixChunk> matrixChunk;
			std::vector<CSV::SStimulationChunk> stimulationChunk;

			OV_ERROR_UNLESS_KRF(m_readerLib->readSamplesAndEventsFromFile(1, matrixChunk, stimulationChunk),
			                    (CSV::ICSVHandler::getLogError(m_readerLib->getLastLogError()) + (m_readerLib->getLastErrorString().empty() ? "" : ". Details: "
			                                                                                                                                       + m_readerLib->getLastErrorString())).c_str(),
			                    Kernel::ErrorType::Internal);

			m_savedChunks.insert(m_savedChunks.end(), matrixChunk.begin(), matrixChunk.end());
			m_savedStimulations.insert(m_savedStimulations.end(), stimulationChunk.begin(), stimulationChunk.end());
		} while (!m_savedChunks.empty() && m_savedChunks.back().startTime < currentTime && m_readerLib->hasDataToRead());
	}

	if (!m_savedChunks.empty())
	{
		const double chunkStartTime = m_savedChunks.cbegin()->startTime;
		const double chunkEndTime   = m_savedChunks.back().endTime;

		// send stimulations chunk even if there is no stimulations, chunks have to be continued
		OV_ERROR_UNLESS_KRF(this->processStimulation(chunkStartTime, chunkEndTime), "Error during stimulation process", Kernel::ErrorType::Internal);

		size_t chunksToRemove = 0;

		for (const CSV::SMatrixChunk& chunk : m_savedChunks)
		{
			if (currentTime > chunk.startTime)
			{
				// move read matrix into buffer to encode
				std::move(chunk.matrix.begin(), chunk.matrix.end(), matrix->getBuffer());

				OV_ERROR_UNLESS_KRF(m_algorithmEncoder.encodeBuffer(), "Failed to encode signal buffer", Kernel::ErrorType::Internal);

				OV_ERROR_UNLESS_KRF(
						boxContext.markOutputAsReadyToSend(0, CTime(chunk.startTime).time(), CTime(chunk.endTime).time()),
						"Failed to mark signal output as ready to send",
						Kernel::ErrorType::Internal);

				chunksToRemove++;
			}
			else
			{
				break;
			}
		}

		// If there is no more data to send, we push the end.
		if (m_savedChunks.size() == chunksToRemove && !m_readerLib->hasDataToRead())
		{
			OV_ERROR_UNLESS_KRF(m_algorithmEncoder.encodeEnd(), "Failed to encode end.", Kernel::ErrorType::Internal);

			OV_ERROR_UNLESS_KRF(
					boxContext.markOutputAsReadyToSend(0, CTime(m_savedChunks.back().startTime).time(), CTime(m_savedChunks.back().endTime).time()),
					"Failed to mark signal output as ready to send", Kernel::ErrorType::Internal);
		}

		if (chunksToRemove != 0)
		{
			m_savedChunks.erase(m_savedChunks.begin(), m_savedChunks.begin() + chunksToRemove);
		}
	}

	return true;
}

bool CBoxAlgorithmOVCSVFileReader::processStimulation(const double startTime, const double endTime)
{
	Kernel::IBoxIO& boxContext = this->getDynamicBoxContext();
	if (!m_isStimulationHeaderSent)
	{
		OV_ERROR_UNLESS_KRF(m_stimEncoder.encodeHeader(), "Failed to encode stimulation header", Kernel::ErrorType::Internal);
		m_isStimulationHeaderSent = true;

		OV_ERROR_UNLESS_KRF(boxContext.markOutputAsReadyToSend(1, 0, 0), "Failed to mark stimulation header as ready to send", Kernel::ErrorType::Internal);
	}

	CStimulationSet* stimulationSet = m_stimEncoder.getInputStimulationSet();
	stimulationSet->clear();

	const uint64_t stimulationChunkStartTime = m_lastStimulationDate;
	const uint64_t currentTime               = getPlayerContext().getCurrentTime();

	if (m_savedStimulations.empty())
	{
		if (currentTime > m_lastStimulationDate)
		{
			m_lastStimulationDate = currentTime;

			OV_ERROR_UNLESS_KRF(m_stimEncoder.encodeBuffer(), "Failed to encode stimulation buffer", Kernel::ErrorType::Internal);

			OV_ERROR_UNLESS_KRF(boxContext.markOutputAsReadyToSend(1, stimulationChunkStartTime, currentTime),
								"Failed to mark stimulation output as ready to send", Kernel::ErrorType::Internal);
		}
	}
	else
	{
		auto it = m_savedStimulations.begin();
		for (; it != m_savedStimulations.end(); ++it)
		{
			const double stimulationDate = it->date;

			if (startTime <= stimulationDate && stimulationDate <= endTime)
			{
				stimulationSet->push_back(it->id, CTime(it->date).time(), CTime(it->duration).time());
				m_lastStimulationDate = CTime(it->date).time();
			}
			else
			{
				if (startTime < stimulationDate)
				{
					// stimulation is in the future of the current time frame. Stop looping
					break;
				}
				// Stimulation is in the past of the current time frame, we can discard it
				const std::string message = "The stimulation is not synced with the stream and will be ignored: [Value: "
				                            + std::to_string(it->id) + " | Date: " + std::to_string(it->date) +
				                            " | Duration: " + std::to_string(it->duration) + "]";

				OV_WARNING_K(message.c_str());
			}
		}

		if (it != m_savedStimulations.begin())
		{
			m_savedStimulations.erase(m_savedStimulations.begin(), it);
		}

		OV_ERROR_UNLESS_KRF(m_stimEncoder.encodeBuffer(), "Failed to encode stimulation buffer", Kernel::ErrorType::Internal);
		OV_ERROR_UNLESS_KRF(boxContext.markOutputAsReadyToSend(1, stimulationChunkStartTime, m_lastStimulationDate),
		                    "Failed to mark stimulation output as ready to send", Kernel::ErrorType::Internal);

	}

	if (m_savedStimulations.empty() && !m_readerLib->hasDataToRead())
	{
		OV_ERROR_UNLESS_KRF(m_stimEncoder.encodeEnd(), "Failed to encode end.", Kernel::ErrorType::Internal);
		OV_ERROR_UNLESS_KRF(boxContext.markOutputAsReadyToSend(1, m_lastStimulationDate, currentTime),
		                    "Failed to mark signal output as ready to send", Kernel::ErrorType::Internal);

	}

	return true;
}

}  // namespace FileIO
}  // namespace Plugins
}  // namespace OpenViBE
