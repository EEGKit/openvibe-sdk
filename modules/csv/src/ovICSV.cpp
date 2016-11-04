#include <map>

#include <ovICSV.h>

using namespace OpenViBE::CSV;
namespace {
	const std::map<ELogErrorCodes, std::string> s_ErrorMap =
	{
		{ LogErrorCodes_ErrorWhileClosing, "Error while closing the file" },
		{ LogErrorCodes_NoFileDefined, "No file defined yet" },
		{ LogErrorCodes_CantOpenFile, "Could not open the file" },
		{ LogErrorCodes_DurationError, "The duration can't be negative" },
		{ LogErrorCodes_DateError, "The date can't be negative" },
		{ LogErrorCodes_NoSample, "Their is no sample to add" },
		{ LogErrorCodes_NoMatrixLabels, "No matrix labels" },
		{ LogErrorCodes_WrongMatrixSize, "Matrix size is different from the initial size" },
		{ LogErrorCodes_MatrixEmpty, "A matrix of the buffer is empty" },
		{ LogErrorCodes_DimensionSizeZero, "Dimensions can not have size equal to 0" },
		{ LogErrorCodes_DimensionSizeEmpty, "Dimension sizes vector is empty" },
		{ LogErrorCodes_NoChannelsName, "No channel names in the vector" },
		{ LogErrorCodes_SetInfoOnce, "The setTypeInformation function should be called only once" },
		{ LogErrorCodes_WrongInputType, "Wrong input type" },
		{ LogErrorCodes_WrongHeader, "Error in the header of the file, refer to getLastErrorString function for more information" },
		{ LogErrorCodes_WrongLineSize, "There is a difference between the current line size and the size that it should have" },
		{ LogErrorCodes_SampleNotEmpty, "Matrix must be empty to store in results of reading" },
		{ LogErrorCodes_NotEnoughLines, "Can't reach the line in parameter, not enough lines in the file" },
		{ LogErrorCodes_NegativeStimulation, "Stimulations cannot have negatives values" },
		{ LogErrorCodes_WrongSampleDate, "Sample dates must be correct (greater than or equal to zero, and start date less than end date)" },
		{ LogErrorCodes_InvalidStimulationArgument, "Wrong stimulation entry" },
		{ LogErrorCodes_StimulationSize, "Some stimulation(s) has/have incomplete parameter(s)" },
		{ LogErrorCodes_EmptyColumn, "A column of the file is empty" },
		{ LogErrorCodes_WrongDimensionSize, "Size of dimension sizes vector must be equal to the number of dimensions" },
		{ LogErrorCodes_DimensionCountZero, "Number of dimension must be at least 1" },
		{ LogErrorCodes_OutOfRangeException, "Convertion from string to number is impossible : string is out of range" },
		{ LogErrorCodes_InvalidArgumentException, "An invalid argument exeption have been thrown, refer to getLastErrorString function for more information" },
		{ LogErrorCodes_CantWriteHeader, "Can not write header, data already began to be written or header already written" },
		{ LogErrorCodes_ErrorWhileWriting, "Error occured while trying to write" },
		{ LogErrorCodes_WrongParameters, "Error with (one of )the parameter(s), refer to GetLastErrorString function for more information" }
	};
}

std::string ICSVLib::getLogError(ELogErrorCodes code)
{
	return s_ErrorMap.at(code);
}
