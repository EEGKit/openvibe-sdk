/*********************************************************************
* Software License Agreement (AGPL-3 License)
*
* OpenViBE SDK Test Software
* Based on OpenViBE V1.1.0, Copyright (C) Inria, 2006-2015
* Copyright (C) Inria, 2015-2017,V1.0
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

#include <tuple>
#include <numeric>

#include "gtest/gtest.h"

#include "csv/ovICSV.h"

using namespace OpenViBE::CSV;

struct SSignalFile
{
	std::vector<std::string> m_ChannelNames;
	uint32_t m_SamplingRate;
	uint32_t m_nSamplePerBuffer;
	std::vector<std::pair<std::pair<double, double>, std::vector<double>>> m_data;
};

namespace
{
	std::string dataDirectory = "";

	const struct SSignalFile SIMPLE_SIGNAL_FILE = {
		{ "Time Signal" },
		32,
		8,
		{
			{
				{ 0.00000, 0.25000 },
				{ 0.00000, 0.03125, 0.06250, 0.09375, 0.125, 0.15625, 0.1875, 0.21875 }
			},
			{
				{ 0.250000, 0.500000 },
				{ 0.250000, 0.281250, 0.312500, 0.343750, 0.375000, 0.406250, 0.437500, 0.468750 }
			},
			{
				{ 0.500000, 0.750000 },
				{ 0.500000, 0.531250, 0.562500, 0.593750, 0.625000, 0.656250, 0.687500, 0.718750 }
			},
		}
	};

	void compareChunks(const std::pair<std::pair<double, double>, std::vector<double>>& expected, const SMatrixChunk& actual)
	{
		ASSERT_EQ(expected.first.first, actual.startTime);
		ASSERT_EQ(expected.first.second, actual.endTime);
		ASSERT_EQ(expected.second.size(), actual.matrix.size());
		for (size_t sample = 0; sample < expected.second.size(); ++sample) { ASSERT_EQ(expected.second[sample], actual.matrix[sample]); }
	}
} // namespace

TEST(CSV_Reader_Test_Case, signalReaderUNIXEndlines)
{
	ICSVHandler* csv           = createCSVHandler();
	const std::string filepath = dataDirectory + "/testCSVSignalUNIXEndlines.csv";

	ASSERT_TRUE(csv->openFile(filepath, EFileAccessMode::Read));
	csv->setFormatType(EStreamType::Signal);
	std::vector<std::string> channelNames;
	uint32_t samplingRate;
	uint32_t sampleCountPerBuffer;
	std::vector<SMatrixChunk> chunks;
	std::vector<SStimulationChunk> stimulations;

	ASSERT_TRUE(csv->getSignalInformation(channelNames, samplingRate, sampleCountPerBuffer));
	ASSERT_TRUE(csv->readSamplesAndEventsFromFile(1, chunks, stimulations));
	ASSERT_EQ(1, chunks.size());
	compareChunks(SIMPLE_SIGNAL_FILE.m_data[0], chunks[0]);
	ASSERT_TRUE(csv->readSamplesAndEventsFromFile(2, chunks, stimulations));
	ASSERT_EQ(2, chunks.size());
	compareChunks(SIMPLE_SIGNAL_FILE.m_data[1], chunks[0]);
	compareChunks(SIMPLE_SIGNAL_FILE.m_data[2], chunks[1]);

	ASSERT_TRUE(csv->closeFile());
	releaseCSVHandler(csv);
}

TEST(CSV_Reader_Test_Case, signalReaderWindowsEndlines)
{
	ICSVHandler* csv           = createCSVHandler();
	const std::string filepath = dataDirectory + "/testCSVSignalWindowsEndlines.csv";

	ASSERT_TRUE(csv->openFile(filepath, EFileAccessMode::Read));
	csv->setFormatType(EStreamType::Signal);
	std::vector<std::string> channelNames;
	uint32_t samplingRate;
	uint32_t sampleCountPerBuffer;
	std::vector<SMatrixChunk> chunks;
	std::vector<SStimulationChunk> stimulations;

	ASSERT_TRUE(csv->getSignalInformation(channelNames, samplingRate, sampleCountPerBuffer));
	ASSERT_TRUE(csv->readSamplesAndEventsFromFile(1, chunks, stimulations));
	ASSERT_EQ(1, chunks.size());
	compareChunks(SIMPLE_SIGNAL_FILE.m_data[0], chunks[0]);
	ASSERT_TRUE(csv->readSamplesAndEventsFromFile(2, chunks, stimulations));
	ASSERT_EQ(2, chunks.size());
	compareChunks(SIMPLE_SIGNAL_FILE.m_data[1], chunks[0]);
	compareChunks(SIMPLE_SIGNAL_FILE.m_data[2], chunks[1]);

	ASSERT_TRUE(csv->closeFile());
	releaseCSVHandler(csv);
}

TEST(CSV_Reader_Test_Case, signalReaderNormalGoodSignal)
{
	ICSVHandler* signalReaderTest = createCSVHandler();
	const std::string filepath    = dataDirectory + "testCSVSignalReader01.csv";

	ASSERT_TRUE(signalReaderTest->openFile(filepath, EFileAccessMode::Read));
	signalReaderTest->setFormatType(EStreamType::Signal);
	std::vector<SMatrixChunk> chunks;
	std::vector<SStimulationChunk> stimulations;
	std::vector<std::string> channelNames;
	const std::vector<std::string> expectedChannels = { "O1", "O2", "Pz", "P1", "P2" };
	uint32_t samplingFrequency;
	uint32_t sampleCountPerBuffer;

	ASSERT_TRUE(signalReaderTest->getSignalInformation(channelNames, samplingFrequency, sampleCountPerBuffer));
	ASSERT_TRUE(signalReaderTest->readSamplesAndEventsFromFile(3, chunks, stimulations));
	ASSERT_EQ(chunks.size(), 3);

	ASSERT_TRUE(!chunks.empty());
	ASSERT_TRUE(!stimulations.empty());
	ASSERT_EQ(channelNames, expectedChannels);
	ASSERT_EQ(samplingFrequency, 8U);
	ASSERT_EQ(sampleCountPerBuffer, 4);
	ASSERT_TRUE(signalReaderTest->closeFile());
	releaseCSVHandler(signalReaderTest);
}

TEST(CSV_Reader_Test_Case, signalReaderNotEnoughChunk)
{
	ICSVHandler* signalReaderTest = createCSVHandler();
	const std::string filepath    = dataDirectory + "testCSVSignalReader01.csv";

	ASSERT_TRUE(signalReaderTest->openFile(filepath, EFileAccessMode::Read));
	signalReaderTest->setFormatType(EStreamType::Signal);
	std::vector<SMatrixChunk> chunks;
	std::vector<SStimulationChunk> stimulations;
	std::vector<std::string> channelNames;
	const std::vector<std::string> expectedChannels = { "O1", "O2", "Pz", "P1", "P2" };
	uint32_t samplingFrequency;
	uint32_t sampleCountPerBuffer;

	ASSERT_TRUE(signalReaderTest->getSignalInformation(channelNames, samplingFrequency, sampleCountPerBuffer));

	ASSERT_TRUE(signalReaderTest->readSamplesAndEventsFromFile(3, chunks, stimulations));
	ASSERT_EQ(chunks.size(), size_t(3));
	ASSERT_EQ(stimulations.size(), size_t(3));
	ASSERT_EQ(channelNames, expectedChannels);
	ASSERT_EQ(samplingFrequency, 8U);
	ASSERT_EQ(sampleCountPerBuffer, 4);
	ASSERT_TRUE(signalReaderTest->closeFile());
	releaseCSVHandler(signalReaderTest);
}

TEST(CSV_Reader_Test_Case, SignalReaderEmptyFile)
{
	ICSVHandler* signalReaderTest = createCSVHandler();
	const std::string filepath    = dataDirectory + "testCSVSignalEmptyFile.csv";
	ASSERT_TRUE(signalReaderTest->openFile(filepath, EFileAccessMode::Read));
	releaseCSVHandler(signalReaderTest);
}

TEST(CSV_Reader_Test_Case, SignalReaderWrongHeader)
{
	ICSVHandler* signalReaderTest = createCSVHandler();
	const std::string filepath    = dataDirectory + "testCSVSignalWrongHeader.csv";
	ASSERT_TRUE(signalReaderTest->openFile(filepath, EFileAccessMode::Read));
	signalReaderTest->setFormatType(EStreamType::Signal);
	std::vector<std::string> channelNames;
	uint32_t samplingFrequency;
	uint32_t sampleCountPerBuffer;
	ASSERT_FALSE(signalReaderTest->getSignalInformation(channelNames, samplingFrequency, sampleCountPerBuffer));

	ASSERT_TRUE(signalReaderTest->closeFile());
	releaseCSVHandler(signalReaderTest);
}

TEST(CSV_Reader_Test_Case, spectrumReaderNormalGoodSignal)
{
	ICSVHandler* spectrumReaderTest = createCSVHandler();
	std::string filepath            = dataDirectory + "testCSVSpectrumReader01.csv";
	ASSERT_TRUE(spectrumReaderTest->openFile(filepath, EFileAccessMode::Read));
	spectrumReaderTest->setFormatType(EStreamType::Spectrum);
	std::vector<SMatrixChunk> chunks;
	std::vector<SStimulationChunk> stimulations;
	std::vector<std::string> channelNames;
	std::vector<std::string> expectedChannels = { "O1", "O2" };
	std::vector<double> expectedData(128);
	std::iota(expectedData.begin(), expectedData.begin() + 64, 0);
	std::iota(expectedData.begin() + 64, expectedData.end(), 0);

	std::vector<double> frequencyAbscissa;
	uint32_t originalSampleRate;

	ASSERT_TRUE(spectrumReaderTest->getSpectrumInformation(channelNames, frequencyAbscissa, originalSampleRate));

	ASSERT_TRUE(spectrumReaderTest->readSamplesAndEventsFromFile(3, chunks, stimulations));
	ASSERT_EQ(chunks.size(), 3);
	ASSERT_EQ(chunks[0].matrix, expectedData);
	ASSERT_EQ(chunks[1].matrix, expectedData);
	ASSERT_EQ(chunks[2].matrix, expectedData);
	ASSERT_EQ(chunks[0].startTime, 0);
	ASSERT_EQ(chunks[0].endTime, 1);
	ASSERT_EQ(chunks[1].startTime, 0.125);
	ASSERT_EQ(chunks[1].endTime, 1.125);
	ASSERT_EQ(chunks[2].startTime, 0.25);
	ASSERT_EQ(chunks[2].endTime, 1.25);

	ASSERT_TRUE(!chunks.empty());
	ASSERT_EQ(channelNames, expectedChannels);
	ASSERT_EQ(originalSampleRate, 128);
	ASSERT_TRUE(spectrumReaderTest->closeFile());
	releaseCSVHandler(spectrumReaderTest);
}

TEST(CSV_Reader_Test_Case, spectrumReaderNotEnoughChunk)
{
	ICSVHandler* spectrumReaderTest = createCSVHandler();
	const std::string filepath      = dataDirectory + "testCSVSpectrumReader01.csv";
	ASSERT_TRUE(spectrumReaderTest->openFile(filepath, EFileAccessMode::Read));
	spectrumReaderTest->setFormatType(EStreamType::Spectrum);
	std::vector<SMatrixChunk> chunks;
	std::vector<SStimulationChunk> stimulations;
	std::vector<std::string> channelNames;
	const std::vector<std::string> expectedChannels = { "O1", "O2" };
	std::vector<double> frequencyAbscissa;
	uint32_t originalSampleRate;

	ASSERT_TRUE(spectrumReaderTest->getSpectrumInformation(channelNames, frequencyAbscissa, originalSampleRate));

	ASSERT_TRUE(spectrumReaderTest->readSamplesAndEventsFromFile(13, chunks, stimulations));
	ASSERT_NE(chunks.size(), 13);

	ASSERT_TRUE(!chunks.empty());
	ASSERT_EQ(channelNames, expectedChannels);
	ASSERT_EQ(originalSampleRate, 128);
	ASSERT_NE(4, chunks.size());
	ASSERT_TRUE(spectrumReaderTest->closeFile());
	releaseCSVHandler(spectrumReaderTest);
}

TEST(CSV_Reader_Test_Case, spectrumReaderWrongHeader)
{
	ICSVHandler* spectrumReaderTest = createCSVHandler();
	const std::string filepath      = dataDirectory + "testCSVSpectrumWrongHeader.csv";
	ASSERT_TRUE(spectrumReaderTest->openFile(filepath, EFileAccessMode::Read));
	spectrumReaderTest->setFormatType(EStreamType::Spectrum);
	std::vector<std::string> channelNames;
	std::vector<double> frequencyAbscissa;
	uint32_t originalSampleRate;
	ASSERT_FALSE(spectrumReaderTest->getSpectrumInformation(channelNames, frequencyAbscissa, originalSampleRate));

	ASSERT_TRUE(spectrumReaderTest->closeFile());
	releaseCSVHandler(spectrumReaderTest);
}

TEST(CSV_Reader_Test_Case, matrixReaderNormalGoodSignal)
{
	ICSVHandler* matrixReaderTest = createCSVHandler();
	const std::string filepath    = dataDirectory + "testCSVMatrixReader01.csv";
	ASSERT_TRUE(matrixReaderTest->openFile(filepath, EFileAccessMode::Read));
	matrixReaderTest->setFormatType(EStreamType::StreamedMatrix);

	std::vector<SMatrixChunk> chunks;
	std::vector<SStimulationChunk> stimulations;
	std::vector<uint32_t> dimensionSizes;
	std::vector<std::string> labels;
	const std::vector<std::string> expectedLabels       = { "", "", "", "", "", "" };
	const std::vector<uint32_t> goodDimensionsSizes = { 2, 2, 2 };

	ASSERT_TRUE(matrixReaderTest->getStreamedMatrixInformation(dimensionSizes, labels));
	ASSERT_TRUE(matrixReaderTest->readSamplesAndEventsFromFile(10, chunks, stimulations));
	ASSERT_EQ(chunks.size(), 10);

	ASSERT_EQ(dimensionSizes, goodDimensionsSizes);
	ASSERT_EQ(labels, expectedLabels);

	ASSERT_TRUE(matrixReaderTest->closeFile());
	releaseCSVHandler(matrixReaderTest);
}


TEST(CSV_Reader_Test_Case, matrixReaderWrongHeader)
{
	ICSVHandler* matrixReaderTest = createCSVHandler();
	const std::string filepath    = dataDirectory + "testCSVMatrixWrongHeader.csv";
	ASSERT_TRUE(matrixReaderTest->openFile(filepath, EFileAccessMode::Read));
	matrixReaderTest->setFormatType(EStreamType::StreamedMatrix);

	std::vector<uint32_t> dimensionSizes;
	std::vector<std::string> labels;
	ASSERT_FALSE(matrixReaderTest->getStreamedMatrixInformation(dimensionSizes, labels));

	ASSERT_TRUE(matrixReaderTest->closeFile());
	releaseCSVHandler(matrixReaderTest);
}

TEST(CSV_Reader_Test_Case, matrixReaderTooManyLabels)
{
	ICSVHandler* matrixReaderTest = createCSVHandler();
	const std::string filepath    = dataDirectory + "testCSVMatrixTooManyLabels.csv";
	ASSERT_TRUE(matrixReaderTest->openFile(filepath, EFileAccessMode::Read));
	matrixReaderTest->setFormatType(EStreamType::StreamedMatrix);

	std::vector<uint32_t> dimensionSizes;
	std::vector<std::string> labels;
	ASSERT_FALSE(matrixReaderTest->getStreamedMatrixInformation(dimensionSizes, labels));

	ASSERT_TRUE(matrixReaderTest->closeFile());
	releaseCSVHandler(matrixReaderTest);
}

TEST(CSV_Reader_Test_Case, matrixReaderWrongStimulation)
{
	ICSVHandler* matrixReaderTest = createCSVHandler();
	const std::string filepath    = dataDirectory + "testCSVMatrixWrongStimulation.csv";
	ASSERT_TRUE(matrixReaderTest->openFile(filepath, EFileAccessMode::Read));
	matrixReaderTest->setFormatType(EStreamType::StreamedMatrix);

	std::vector<uint32_t> dimensionSizes;
	std::vector<std::string> labels;
	ASSERT_TRUE(matrixReaderTest->getStreamedMatrixInformation(dimensionSizes, labels));

	std::vector<SMatrixChunk> chunks;
	std::vector<SStimulationChunk> stimulations;

	ASSERT_FALSE(matrixReaderTest->readSamplesAndEventsFromFile(1, chunks, stimulations));

	ASSERT_TRUE(matrixReaderTest->closeFile());
	releaseCSVHandler(matrixReaderTest);
}


TEST(CSV_Reader_Test_Case, covarianceMatrixReaderNormalGoodSignal)
{
	ICSVHandler* matrixReaderTest = createCSVHandler();
	const std::string filepath    = dataDirectory + "testCSVCovarMatrixReader01.csv";
	ASSERT_TRUE(matrixReaderTest->openFile(filepath, EFileAccessMode::Read));
	matrixReaderTest->setFormatType(EStreamType::CovarianceMatrix);

	std::vector<SMatrixChunk> chunks;
	std::vector<SStimulationChunk> stimulations;
	std::vector<uint32_t> dimensionSizes;
	std::vector<std::string> labels;
	const std::vector<std::string> expectedLabels       = { "X", "Y", "X", "Y", "Z1", "Z2", "Z3", "Z4", "Z5" };
	const std::vector<uint32_t> goodDimensionsSizes = { 2, 2, 5 };

	ASSERT_TRUE(matrixReaderTest->getStreamedMatrixInformation(dimensionSizes, labels));
	ASSERT_TRUE(matrixReaderTest->readSamplesAndEventsFromFile(3, chunks, stimulations)) << matrixReaderTest->getLastLogError() << ".Details: " <<
 matrixReaderTest->getLastErrorString();
	ASSERT_EQ(chunks.size(), 3);

	ASSERT_EQ(dimensionSizes, goodDimensionsSizes);
	ASSERT_EQ(labels, expectedLabels);

	ASSERT_TRUE(matrixReaderTest->closeFile());
	releaseCSVHandler(matrixReaderTest);
}

TEST(CSV_Reader_Test_Case, covarianceMatrixReaderWrongHeader)
{
	ICSVHandler* matrixReaderTest = createCSVHandler();
	const std::string filepath    = dataDirectory + "testCSVCovarMatrixWrongHeader.csv";
	ASSERT_TRUE(matrixReaderTest->openFile(filepath, EFileAccessMode::Read));

	matrixReaderTest->setFormatType(EStreamType::CovarianceMatrix);

	std::vector<uint32_t> dimensionSizes;
	std::vector<std::string> labels;
	ASSERT_FALSE(matrixReaderTest->getStreamedMatrixInformation(dimensionSizes, labels));

	ASSERT_TRUE(matrixReaderTest->closeFile());
	releaseCSVHandler(matrixReaderTest);
}

TEST(CSV_Reader_Test_Case, covarianceMatrixReaderTooManyLabels)
{
	ICSVHandler* matrixReaderTest = createCSVHandler();
	const std::string filepath    = dataDirectory + "testCSVCovarMatrixTooManyLabels.csv";
	ASSERT_TRUE(matrixReaderTest->openFile(filepath, EFileAccessMode::Read));
	matrixReaderTest->setFormatType(EStreamType::CovarianceMatrix);

	std::vector<uint32_t> dimensionSizes;
	std::vector<std::string> labels;
	ASSERT_FALSE(matrixReaderTest->getStreamedMatrixInformation(dimensionSizes, labels));

	ASSERT_TRUE(matrixReaderTest->closeFile());
	releaseCSVHandler(matrixReaderTest);
}

int uoCSVReaderTest(int argc, char* argv[])
{
	if (argv[1] != nullptr) { dataDirectory = argv[1]; }
	testing::InitGoogleTest(&argc, argv);
	::testing::GTEST_FLAG(filter) = "CSV_Reader_Test_Case.*";
	return RUN_ALL_TESTS();
}
