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

#include "gtest/gtest.h"

#include "csv/ovICSV.h"

#include <string>
#include <fstream>
#include <streambuf>

using namespace OpenViBE::CSV;

namespace
{
	std::string directoryPath = "";
}

TEST(CSV_Writer_Test_Case, signalWriterNormalGoodSignal)
{
	ICSVHandler* signalWriterTest = createCSVHandler();
	const std::string filename    = directoryPath + "testCSVSignalWriter01.csv";

	ASSERT_TRUE(signalWriterTest->openFile(filename, EFileAccessMode::Write));
	signalWriterTest->setFormatType(EStreamType::Signal);

	ASSERT_TRUE(signalWriterTest->setSignalInformation({ "O1", "O2", "Pz", "P1", "P2" }, 8, 8));

	for (double index = 0.0; index < 1.2; index += 0.125)
	{
		double epoch = index / 0.5;
		ASSERT_TRUE(signalWriterTest->addSample({ index, index + 0.125, { -10.10, -5.05, 0.00, 5.05, 10.10 }, static_cast<unsigned int>(epoch) }));

		if (index == 0.25 || index == 0.75)
		{
			ASSERT_TRUE(signalWriterTest->addEvent(35000, index, 0.0));
		}
	}

	ASSERT_TRUE(signalWriterTest->writeHeaderToFile());

	ASSERT_TRUE(signalWriterTest->writeDataToFile());
	ASSERT_TRUE(signalWriterTest->closeFile());
	releaseCSVHandler(signalWriterTest);
}

TEST(CSV_Writer_Test_Case, signalWriterNoStimulations)
{
	ICSVHandler* signalWriterTest = createCSVHandler();
	const std::string filename    = directoryPath + "testCSVSignalWriter02.csv";

	ASSERT_TRUE(signalWriterTest->openFile(filename, EFileAccessMode::Write));
	signalWriterTest->setFormatType(EStreamType::Signal);

	ASSERT_TRUE(signalWriterTest->setSignalInformation({ "O1", "O2", "Pz", "P1", "P2" }, 8, 8));
	ASSERT_TRUE(signalWriterTest->noEventsUntilDate(2.0));
	for (double index = 0; index < 1.2; index += 0.125)
	{
		double epoch = index / 0.5;
		ASSERT_TRUE(signalWriterTest->addSample({ index, index + 0.125, { -10.10, -5.05, 0.00, 5.05, 10.10 }, static_cast<unsigned int>(epoch) }));
	}

	ASSERT_TRUE(signalWriterTest->writeHeaderToFile());

	ASSERT_TRUE(signalWriterTest->writeDataToFile());
	ASSERT_TRUE(signalWriterTest->closeFile());
	releaseCSVHandler(signalWriterTest);
}

TEST(CSV_Writer_Test_Case, signalWriterNoFileOpen)
{
	ICSVHandler* signalWriterTest = createCSVHandler();
	signalWriterTest->setFormatType(EStreamType::Signal);
	ASSERT_TRUE(signalWriterTest->setSignalInformation({ "O1", "O2", "Pz", "P1", "P2" }, 8, 8));

	for (double index = 0; index < 1.2; index += 0.125)
	{
		double epoch = index / 0.5;
		ASSERT_TRUE(signalWriterTest->addSample({ index, index + 0.125, { -10.10, -5.05, 0.00, 5.05, 10.10 }, static_cast<unsigned int>(epoch) }));
	}

	ASSERT_FALSE(signalWriterTest->writeHeaderToFile());

	ASSERT_TRUE(signalWriterTest->closeFile());
	releaseCSVHandler(signalWriterTest);
}

TEST(CSV_Writer_Test_Case, signalWriterWrongInputType)
{
	ICSVHandler* signalWriterTest = createCSVHandler();
	signalWriterTest->setFormatType(EStreamType::Spectrum);
	ASSERT_FALSE(signalWriterTest->setSignalInformation({ "O1", "O2", "Pz", "P1", "P2" }, 8, 8));
	releaseCSVHandler(signalWriterTest);
}

TEST(CSV_Writer_Test_Case, signalWriterWrongMatrixSize)
{
	ICSVHandler* signalWriterTest = createCSVHandler();
	const std::string filename    = directoryPath + "testCSVSignalWriter05.csv";

	ASSERT_TRUE(signalWriterTest->openFile(filename, EFileAccessMode::Write));
	signalWriterTest->setFormatType(EStreamType::Signal);
	ASSERT_TRUE(signalWriterTest->setSignalInformation({ "O1", "O2", "Pz", "P1", "P2" }, 8, 8));

	ASSERT_FALSE(signalWriterTest->addSample({ 0, 0.125, { -20.20, -10.10, -5.05, 5.05, 10.10, 20.20 }, 0 }));

	ASSERT_TRUE(signalWriterTest->closeFile());
	releaseCSVHandler(signalWriterTest);
}

// should have nothing in the file
TEST(CSV_Writer_Test_Case, signalWriterTonsOfSignalWithoutSetNoEventsUntilDate)
{
	ICSVHandler* signalWriterTest = createCSVHandler();
	const std::string filename    = directoryPath + "testCSVSignalWriter06.csv";

	ASSERT_TRUE(signalWriterTest->openFile(filename, EFileAccessMode::Write));
	signalWriterTest->setFormatType(EStreamType::Signal);
	ASSERT_TRUE(signalWriterTest->setSignalInformation({ "O1", "O2", "Pz", "P1", "P2" }, 8, 8));
	for (double time = 0.0; time < 100.0; time += 0.125)
	{
		ASSERT_TRUE(signalWriterTest->addSample({ time, time + 0.125, { -20.20, -10.10, 0.0, 10.10, 20.20 }, static_cast<unsigned int>(time / 0.125) }));
	}
	ASSERT_TRUE(signalWriterTest->writeHeaderToFile());
	ASSERT_TRUE(signalWriterTest->writeDataToFile());
	releaseCSVHandler(signalWriterTest);
}

// file should be full
TEST(CSV_Writer_Test_Case, signalWriterTonsOfSignalWithSetNoEventsUntilDate)
{
	ICSVHandler* signalWriterTest = createCSVHandler();
	const std::string filename    = directoryPath + "testCSVSignalWriter07.csv";

	ASSERT_TRUE(signalWriterTest->openFile(filename, EFileAccessMode::Write));
	signalWriterTest->setFormatType(EStreamType::Signal);
	ASSERT_TRUE(signalWriterTest->setSignalInformation({ "O1", "O2", "Pz", "P1", "P2" }, 8, 8));
	ASSERT_TRUE(signalWriterTest->noEventsUntilDate(100.001));
	for (double time = 0.0; time < 100.0; time += 0.125)
	{
		ASSERT_TRUE(signalWriterTest->addSample({ time, time + 0.125, { -20.20, -10.10, 0.0, 10.10, 20.20 }, static_cast<unsigned int>(time / 0.5) }));
	}
	ASSERT_TRUE(signalWriterTest->writeHeaderToFile());
	ASSERT_TRUE(signalWriterTest->writeDataToFile());
	ASSERT_TRUE(signalWriterTest->writeAllDataToFile());
	releaseCSVHandler(signalWriterTest);
}

TEST(CSV_Writer_Test_Case, signalWriterOnlyLastMatrix)
{
	ICSVHandler* signalWriterTest = createCSVHandler();
	signalWriterTest->setLastMatrixOnlyMode(true);
	const std::string filename = directoryPath + "testCSVSignalWriter08.csv";
	const std::string expectedFileContent("Time:8Hz,Epoch,O1,O2,Pz,P1,P2,Event Id,Event Date,Event Duration\n1.0000000000,2,-10.1000000000,-5.0500000000,0.0000000000,5.0500000000,10.1000000000,35000,1.0000000000,0.0000000000\n1.1250000000,2,-10.1000000000,-5.0500000000,0.0000000000,5.0500000000,10.1000000000,,,\n");
	ASSERT_TRUE(signalWriterTest->openFile(filename, EFileAccessMode::Write));
	signalWriterTest->setFormatType(EStreamType::Signal);

	ASSERT_TRUE(signalWriterTest->setSignalInformation({ "O1", "O2", "Pz", "P1", "P2" }, 8, 8));

	for (double index = 0.0; index < 1.2; index += 0.125)
	{
		double epoch = index / 0.5;
		ASSERT_TRUE(signalWriterTest->addSample({ index, index + 0.125, { -10.10, -5.05, 0.00, 5.05, 10.10 }, static_cast<unsigned int>(epoch) }));

		if (index == 0.25 || index == 0.75 || index == 1.0)
		{
			ASSERT_TRUE(signalWriterTest->addEvent(35000, index, 0.0));
		}
	}

	ASSERT_TRUE(signalWriterTest->writeHeaderToFile());

	ASSERT_TRUE(signalWriterTest->writeAllDataToFile());
	ASSERT_TRUE(signalWriterTest->closeFile());
	releaseCSVHandler(signalWriterTest);

	std::ifstream t(filename);
	std::string csv_out((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	ASSERT_STREQ(csv_out.c_str(), expectedFileContent.c_str());
}

TEST(CSV_Writer_Test_Case, spectrumWriterNormalGoodSignal)
{
	ICSVHandler* spectrumWriterTest = createCSVHandler();
	const std::string filename      = directoryPath + "testCSVSpectrumWriter01.csv";

	ASSERT_TRUE(spectrumWriterTest->openFile(filename, EFileAccessMode::Write));
	spectrumWriterTest->setFormatType(EStreamType::Spectrum);

	std::vector<double> frequencyAbscissa;
	for (double frequency = 0.0; frequency < 64.0; frequency++) { frequencyAbscissa.push_back(frequency); }
	ASSERT_TRUE(spectrumWriterTest->setSpectrumInformation({ "O1", "O2" }, frequencyAbscissa, 256));
	double time = 0;
	for (unsigned int i = 0; i < 10; i++)
	{
		unsigned int epoch = i / 4;
		std::vector<double> sample;
		for (double index = -64.0; index < 64.0; sample.push_back(index++)) { }

		ASSERT_TRUE(spectrumWriterTest->addSample({ time, time + 1.0, sample, epoch }));

		time += 0.125;
		if (i == 5 || i == 3 || i == 7)
		{
			ASSERT_TRUE(spectrumWriterTest->addEvent(35001, time, 1.0));
		}
	}

	ASSERT_TRUE(spectrumWriterTest->writeHeaderToFile());
	ASSERT_TRUE(spectrumWriterTest->writeDataToFile());
	ASSERT_TRUE(spectrumWriterTest->closeFile());
	releaseCSVHandler(spectrumWriterTest);
}

TEST(CSV_Writer_Test_Case, spectrumWriterWrongInputType)
{
	ICSVHandler* spectrumWriterTest = createCSVHandler();
	const std::string filename      = directoryPath + "testCSVSpectrumWriter02.csv";

	ASSERT_TRUE(spectrumWriterTest->openFile(filename, EFileAccessMode::Write));
	spectrumWriterTest->setFormatType(EStreamType::Signal);
	std::vector<double> frequencyAbscissa;
	for (double frequency = 0.0; frequency < 64.0; frequency++) { frequencyAbscissa.push_back(frequency); }

	ASSERT_FALSE(spectrumWriterTest->setSpectrumInformation({ "O1", "O2" }, frequencyAbscissa, 256));

	ASSERT_TRUE(spectrumWriterTest->closeFile());
	releaseCSVHandler(spectrumWriterTest);
}

TEST(CSV_Writer_Test_Case, spectrumWriterWrongMatrixSize)
{
	ICSVHandler* spectrumWriterTest = createCSVHandler();
	const std::string filename      = directoryPath + "testCSVSpectrumWriter03.csv";

	ASSERT_TRUE(spectrumWriterTest->openFile(filename, EFileAccessMode::Write));
	spectrumWriterTest->setFormatType(EStreamType::Spectrum);
	std::vector<double> frequencyAbscissa;
	for (double frequency = 0.0; frequency < 64.0; frequency++) { frequencyAbscissa.push_back(frequency); }
	ASSERT_TRUE(spectrumWriterTest->setSpectrumInformation({ "O1", "O2" }, frequencyAbscissa, 256));

	ASSERT_FALSE(spectrumWriterTest->addSample({ 0, 1, { -20.20, -10.10, 0.0, 10.10, 20.20 }, 0 }));

	ASSERT_TRUE(spectrumWriterTest->closeFile());
	releaseCSVHandler(spectrumWriterTest);
}

TEST(CSV_Writer_Test_Case, matrixWriterNormalGoodSignal)
{
	ICSVHandler* matrixWriterTest = createCSVHandler();
	const std::string filename    = directoryPath + "testCSVMatrixWriter01.csv";

	ASSERT_TRUE(matrixWriterTest->openFile(filename, EFileAccessMode::Write));
	matrixWriterTest->setFormatType(EStreamType::StreamedMatrix);

	ASSERT_TRUE(matrixWriterTest->setStreamedMatrixInformation({ 2, 2, 2 }, { "LA", "LB", "1", "2", "X", "Y" }));

	for (unsigned int i = 0; i < 50; i++)
	{
		unsigned int epoch = i / 10;
		ASSERT_TRUE(matrixWriterTest->addSample({ double(i), double(i)+1.0, { -20.20, -15.15, -10.10, -5.05, 5.05, 10.10, 15.15, 20.20 }, epoch }));

		if (i == 5 || i == 3 || i == 7)
		{
			ASSERT_TRUE(matrixWriterTest->addEvent(35001, double(i)+3.5, 1.0));
		}
	}

	ASSERT_TRUE(matrixWriterTest->writeHeaderToFile());
	ASSERT_TRUE(matrixWriterTest->writeDataToFile());
	ASSERT_TRUE(matrixWriterTest->closeFile());
	releaseCSVHandler(matrixWriterTest);
}

TEST(CSV_Writer_Test_Case, matrixWriterEmptyLabels)
{
	ICSVHandler* matrixWriterTest = createCSVHandler();
	const std::string filename    = directoryPath + "testCSVMatrixWriter02.csv";

	ASSERT_TRUE(matrixWriterTest->openFile(filename, EFileAccessMode::Write));
	matrixWriterTest->setFormatType(EStreamType::StreamedMatrix);

	ASSERT_TRUE(matrixWriterTest->setStreamedMatrixInformation({ 2, 2, 2 }, { "", "", "", "", "", "" }));

	for (unsigned int i = 0; i < 50; i++)
	{
		unsigned int epoch = i / 10;
		ASSERT_TRUE(matrixWriterTest->addSample({ double(i), double(i)+1.0, { -20.20, -15.15, -10.10, -5.05, 5.05, 10.10, 15.15, 20.20 }, epoch }));

		if (i == 5 || i == 3 || i == 7)
		{
			ASSERT_TRUE(matrixWriterTest->addEvent(35001, double(i)+3.5, 1.0));
		}
	}

	ASSERT_TRUE(matrixWriterTest->writeHeaderToFile());
	ASSERT_TRUE(matrixWriterTest->writeDataToFile());
	ASSERT_TRUE(matrixWriterTest->closeFile());
	releaseCSVHandler(matrixWriterTest);
}

TEST(CSV_Writer_Test_Case, matrixWithDifferentsDimensionSizes)
{
	ICSVHandler* matrixWriterTest = createCSVHandler();
	const std::string filename    = directoryPath + "testCSVMatrixWriter03.csv";

	ASSERT_TRUE(matrixWriterTest->openFile(filename, EFileAccessMode::Write));
	matrixWriterTest->setFormatType(EStreamType::StreamedMatrix);

	ASSERT_TRUE(matrixWriterTest->setStreamedMatrixInformation({ 1, 4 }, { "L1", "A", "B", "C", "D" }));

	for (unsigned int i = 0; i < 50; i++)
	{
		unsigned int epoch = i / 10;
		ASSERT_TRUE(matrixWriterTest->addSample({ double(i), double(i)+1.0, { -20.20, -10.10, 10.10, 20.20 }, epoch }));

		if (i == 5 || i == 3 || i == 7)
		{
			ASSERT_TRUE(matrixWriterTest->addEvent(35001, double(i)+3.5, 1.0));
		}
	}

	ASSERT_TRUE(matrixWriterTest->writeHeaderToFile());
	ASSERT_TRUE(matrixWriterTest->writeDataToFile());
	ASSERT_TRUE(matrixWriterTest->closeFile());
	releaseCSVHandler(matrixWriterTest);
}

TEST(CSV_Writer_Test_Case, matrixWriterWrongMatrixSize)
{
	ICSVHandler* matrixWriterTest = createCSVHandler();
	const std::string filename    = directoryPath + "testCSVMatrixWriter04.csv";

	ASSERT_TRUE(matrixWriterTest->openFile(filename, EFileAccessMode::Write));
	matrixWriterTest->setFormatType(EStreamType::StreamedMatrix);
	ASSERT_TRUE(matrixWriterTest->setStreamedMatrixInformation({ 2, 2, 2 }, { "", "", "", "", "", "" }));

	ASSERT_FALSE(matrixWriterTest->addSample({ 0, 1.0, { -25.25, -20.20, -15.15, -10.10, -5.05, 5.05, 10.10, 15.15, 20.20, 25.25 }, 0 }));

	ASSERT_TRUE(matrixWriterTest->closeFile());
	releaseCSVHandler(matrixWriterTest);
}

TEST(CSV_Writer_Test_Case, matrixWithDifferentsDimensionSizes2)
{
	ICSVHandler* matrixWriterTest = createCSVHandler();
	const std::string filename    = directoryPath + "testCSVMatrixWriter05.csv";

	ASSERT_TRUE(matrixWriterTest->openFile(filename, EFileAccessMode::Write));
	matrixWriterTest->setFormatType(EStreamType::StreamedMatrix);

	ASSERT_TRUE(matrixWriterTest->setStreamedMatrixInformation({ 6, 8, 2 }, { "L1", "L2", "L3", "L4", "L5", "L6", "A1", "B2", "C3", "D4", "E5", "F6", "G7", "H8", "X", "Y" }));

	std::vector<double> values;
	for (unsigned int index = 0; index < 96; index++) { values.push_back(double(index)); }

	for (unsigned int i = 0; i < 50; i++)
	{
		unsigned int epoch = i / 10;
		ASSERT_TRUE(matrixWriterTest->addSample({ double(i), double(i)+1.0, values, epoch }));

		if (i == 5 || i == 3 || i == 7)
		{
			ASSERT_TRUE(matrixWriterTest->addEvent(35001, double(i)+3.5, 1.0));
		}
	}

	ASSERT_TRUE(matrixWriterTest->writeHeaderToFile());
	ASSERT_TRUE(matrixWriterTest->writeDataToFile());
	ASSERT_TRUE(matrixWriterTest->closeFile());
	releaseCSVHandler(matrixWriterTest);
}

TEST(CSV_Writer_Test_Case, matrixWithDifferentsDimensionSizes3)
{
	ICSVHandler* matrixWriterTest = createCSVHandler();
	const std::string filename    = directoryPath + "testCSVMatrixWriter06.csv";

	ASSERT_TRUE(matrixWriterTest->openFile(filename, EFileAccessMode::Write));
	matrixWriterTest->setFormatType(EStreamType::StreamedMatrix);

	ASSERT_TRUE(matrixWriterTest->setStreamedMatrixInformation({ 4, 1, 4 }, { "L1", "L2", "L3", "L4", "X", "R1", "R2", "R3", "R4" }));

	std::vector<double> values;
	for (unsigned int index = 0; index < 16; index++) { values.push_back(double(index)); }

	for (unsigned int i = 0; i < 50; i++)
	{
		unsigned int epoch = i / 10;
		ASSERT_TRUE(matrixWriterTest->addSample({ double(i), double(i)+1.0, values, epoch }));

		if (i == 5 || i == 3 || i == 7)
		{
			ASSERT_TRUE(matrixWriterTest->addEvent(35001, double(i)+3.5, 1.0));
		}
	}

	ASSERT_TRUE(matrixWriterTest->writeHeaderToFile());
	ASSERT_TRUE(matrixWriterTest->writeDataToFile());
	ASSERT_TRUE(matrixWriterTest->closeFile());
	releaseCSVHandler(matrixWriterTest);
}

// As of 10/01/2017 (commit a11210cf1c3fd81bb52095c7c9c6006c760218a2), this is valid for
TEST(CSV_Writer_Test_Case, matrixWriterWithInvalidTime)
{
	ICSVHandler* matrixWriterTest = createCSVHandler();
	const std::string filename    = directoryPath + "testCSVMatrixWriter07.csv";

	ASSERT_TRUE(matrixWriterTest->openFile(filename, EFileAccessMode::Write));
	matrixWriterTest->setFormatType(EStreamType::StreamedMatrix);

	ASSERT_TRUE(matrixWriterTest->setStreamedMatrixInformation({ 1, 1, 1 }, { "X", "Y", "Z" }));

	ASSERT_FALSE(matrixWriterTest->addSample({ 1.0, 0, { -20.20, -15.15, -10.10 }, 0 }));
	ASSERT_FALSE(matrixWriterTest->addSample({ -1.0, 0, { -20.20, -15.15, -10.10 }, 1 }));
	ASSERT_FALSE(matrixWriterTest->addSample({ -1.0, -0.5, { -20.20, -15.15, -10.10 }, 2 }));
	ASSERT_FALSE(matrixWriterTest->addSample({ 1.0, -1.0, { -20.20, -15.15, -10.10 }, 3 }));

	ASSERT_TRUE(matrixWriterTest->closeFile());

	releaseCSVHandler(matrixWriterTest);
}

TEST(CSV_Writer_Test_Case, matrixWriterOnlyLastMatrix)
{
	ICSVHandler* matrixWriterTest = createCSVHandler();
	matrixWriterTest->setLastMatrixOnlyMode(true);
	const std::string filename = directoryPath + "testCSVMatrixWriter08.csv";
	const std::string expectedFileContent("Time:2x2x2,End Time,LA:1:X,LA:1:Y,LA:2:X,LA:2:Y,LB:1:X,LB:1:Y,LB:2:X,LB:2:Y,Event Id,Event Date,Event Duration\n49.0000000000,50.0000000000,49.0000000000,1.0000000000,2.0000000000,3.0000000000,4.0000000000,5.0000000000,6.0000000000,7.0000000000,,,\n");
	ASSERT_TRUE(matrixWriterTest->openFile(filename, EFileAccessMode::Write));
	matrixWriterTest->setFormatType(EStreamType::StreamedMatrix);

	ASSERT_TRUE(matrixWriterTest->setStreamedMatrixInformation({ 2, 2, 2 }, { "LA", "LB", "1", "2", "X", "Y" }));
	ASSERT_TRUE(matrixWriterTest->writeHeaderToFile());

	for (unsigned int i = 0; i < 50; i++)
	{
		unsigned int epoch = i / 10;
		ASSERT_TRUE(matrixWriterTest->addSample({ double(i), double(i) + 1.0, { double(i), 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0 }, epoch }));

		if (i == 3 || i == 5 || i == 7)
		{
			ASSERT_TRUE(matrixWriterTest->addEvent(35001, double(i) + 3.5, 1.0));
		}
	}

	ASSERT_TRUE(matrixWriterTest->writeAllDataToFile());
	ASSERT_TRUE(matrixWriterTest->closeFile());
	releaseCSVHandler(matrixWriterTest);

	std::ifstream t(filename);
	std::string csv_out((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

	ASSERT_STREQ(csv_out.c_str(), expectedFileContent.c_str());
}


TEST(CSV_Writer_Test_Case, featureVectorNormalGoodSignal)
{
	ICSVHandler* featureVectorWriterTest = createCSVHandler();
	const std::string filename           = directoryPath + "testCSVFeatureVectorWriter01.csv";

	ASSERT_TRUE(featureVectorWriterTest->openFile(filename, EFileAccessMode::Write));
	featureVectorWriterTest->setFormatType(EStreamType::FeatureVector);

	ASSERT_TRUE(featureVectorWriterTest->setFeatureVectorInformation({ "F1", "F2", "F3" }));

	for (unsigned int i = 0; i < 50; i++)
	{
		unsigned int epoch = i / 10;
		ASSERT_TRUE(featureVectorWriterTest->addSample({ double(i), double(i)+1.0, { -20.20, -15.15, -10.10 }, epoch }));

		if (i == 5 || i == 3 || i == 7)
		{
			ASSERT_TRUE(featureVectorWriterTest->addEvent(35001, double(i)+3.5, 1.0));
		}
	}

	ASSERT_TRUE(featureVectorWriterTest->writeHeaderToFile());
	ASSERT_TRUE(featureVectorWriterTest->writeDataToFile());
	ASSERT_TRUE(featureVectorWriterTest->closeFile());

	releaseCSVHandler(featureVectorWriterTest);
}

TEST(CSV_Writer_Test_Case, featureVectorEmptyLabels)
{
	ICSVHandler* featureVectorWriterTest = createCSVHandler();
	const std::string filename           = directoryPath + "testCSVFeatureVectorWriter02.csv";

	ASSERT_TRUE(featureVectorWriterTest->openFile(filename, EFileAccessMode::Write));
	featureVectorWriterTest->setFormatType(EStreamType::FeatureVector);

	ASSERT_TRUE(featureVectorWriterTest->setFeatureVectorInformation({ "", "", "" }));

	for (unsigned int i = 0; i < 50; i++)
	{
		unsigned int epoch = i / 10;
		ASSERT_TRUE(featureVectorWriterTest->addSample({ double(i), double(i)+1.0, { -20.20, -15.15, -10.10 }, epoch }));
	}

	ASSERT_TRUE(featureVectorWriterTest->writeHeaderToFile());
	ASSERT_TRUE(featureVectorWriterTest->writeDataToFile());
	ASSERT_TRUE(featureVectorWriterTest->closeFile());

	releaseCSVHandler(featureVectorWriterTest);
}

TEST(CSV_Writer_Test_Case, featureVectorWrongVectorSize)
{
	ICSVHandler* featureVectorWriterTest = createCSVHandler();
	const std::string filename           = directoryPath + "testCSVFeatureVectorWriter03.csv";

	ASSERT_TRUE(featureVectorWriterTest->openFile(filename, EFileAccessMode::Write));
	featureVectorWriterTest->setFormatType(EStreamType::FeatureVector);

	ASSERT_TRUE(featureVectorWriterTest->setFeatureVectorInformation({ "F1", "F2", "F3" }));

	ASSERT_FALSE(featureVectorWriterTest->addSample({ 0, 1.0, { -20.20, -15.15, -10.10, 12 }, 0 }));
	ASSERT_FALSE(featureVectorWriterTest->addSample({ 1.0, 2.0, { -20.20, -15.15 }, 1 }));

	ASSERT_TRUE(featureVectorWriterTest->closeFile());

	releaseCSVHandler(featureVectorWriterTest);
}


TEST(CSV_Writer_Test_Case, covarianceMatrixWriterNormalGoodSignal)
{
	ICSVHandler* matrixWriterTest = createCSVHandler();
	const std::string filename    = directoryPath + "testCSVCovarMatrixWriter01.csv";

	ASSERT_TRUE(matrixWriterTest->openFile(filename, EFileAccessMode::Write));
	matrixWriterTest->setFormatType(EStreamType::CovarianceMatrix);

	ASSERT_TRUE(matrixWriterTest->setStreamedMatrixInformation({ 2, 2, 2 }, { "C1", "C2", "C1", "C2", "Matrix 1", "Matrix 2" }));

	for (unsigned int i = 0; i < 50; i++)
	{
		unsigned int epoch = i / 10;
		ASSERT_TRUE(matrixWriterTest->addSample({ double(i), double(i)+1.0, { -20.20, -15.15, -10.10, -5.05, 5.05, 10.10, 15.15, 20.20 }, epoch }));
	}
	matrixWriterTest->noEventsUntilDate(20.0);

	ASSERT_TRUE(matrixWriterTest->writeHeaderToFile());
	ASSERT_TRUE(matrixWriterTest->writeAllDataToFile());
	ASSERT_TRUE(matrixWriterTest->closeFile());
	releaseCSVHandler(matrixWriterTest);
}

TEST(CSV_Writer_Test_Case, covarianceMatrixWriterEmptyLabels)
{
	ICSVHandler* matrixWriterTest = createCSVHandler();
	const std::string filename    = directoryPath + "testCSVCovarMatrixWriter02.csv";

	ASSERT_TRUE(matrixWriterTest->openFile(filename, EFileAccessMode::Write));
	matrixWriterTest->setFormatType(EStreamType::CovarianceMatrix);

	ASSERT_TRUE(matrixWriterTest->setStreamedMatrixInformation({ 2, 2, 2 }, { "", "", "", "", "", "" }));

	for (unsigned int i = 0; i < 50; i++)
	{
		unsigned int epoch = i / 10;
		ASSERT_TRUE(matrixWriterTest->addSample({ double(i), double(i)+1.0, { -20.20, -15.15, -10.10, -5.05, 5.05, 10.10, 15.15, 20.20 }, epoch }));
	}

	ASSERT_TRUE(matrixWriterTest->writeHeaderToFile());
	ASSERT_TRUE(matrixWriterTest->writeAllDataToFile());
	ASSERT_TRUE(matrixWriterTest->closeFile());
	releaseCSVHandler(matrixWriterTest);
}


TEST(CSV_Writer_Test_Case, covarianceMatrixWriterWrongMatrixSize)
{
	ICSVHandler* matrixWriterTest = createCSVHandler();
	const std::string filename    = directoryPath + "testCSVCovarMatrixWriter04.csv";

	ASSERT_TRUE(matrixWriterTest->openFile(filename, EFileAccessMode::Write));
	matrixWriterTest->setFormatType(EStreamType::CovarianceMatrix);
	ASSERT_TRUE(matrixWriterTest->setStreamedMatrixInformation({ 2, 2, 2 }, { "", "", "", "", "", "" }));

	ASSERT_FALSE(matrixWriterTest->addSample({ 0, 1.0, { -25.25, -20.20, -15.15, -10.10, -5.05, 5.05, 10.10, 15.15, 20.20, 25.25 }, 0 }));

	ASSERT_TRUE(matrixWriterTest->closeFile());
	releaseCSVHandler(matrixWriterTest);
}


int uoCSVWriterTest(int argc, char* argv[])
{
	if (argv[1] != nullptr) { directoryPath = argv[1]; }
	testing::InitGoogleTest(&argc, argv);
	::testing::GTEST_FLAG(filter) = "CSV_Writer_Test_Case.*";
	return RUN_ALL_TESTS();
}
