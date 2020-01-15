#include "ovtkMatrix.h"

#include <system/ovCMemory.h>
#include <fs/Files.h>

#include <cstring>
#include <cmath>
#include <cstdlib>
#include <cerrno>

// for save/load
#include <fstream>
#include <iostream>
#include <vector>
#include <locale> // std::isspace
#include <sstream>

using namespace OpenViBE;
using namespace OpenViBEToolkit;
using namespace Tools;

bool Matrix::copy(IMatrix& dst, const IMatrix& src)
{
	if (&dst == &src) { return true; }

	if (!copyDescription(dst, src)) { return false; }
	if (!copyContent(dst, src)) { return false; }
	return true;
}

bool Matrix::copyDescription(IMatrix& dst, const IMatrix& src)
{
	if (&dst == &src) { return true; }

	const size_t nDim = src.getDimensionCount();
	if (!dst.setDimensionCount(nDim)) { return false; }
	for (size_t i = 0; i < nDim; ++i)
	{
		const size_t dimSize = src.getDimensionSize(i);
		if (!dst.setDimensionSize(i, dimSize)) { return false; }
		for (size_t j = 0; j < dimSize; ++j) { if (!dst.setDimensionLabel(i, j, src.getDimensionLabel(i, j))) { return false; } }
	}
	return true;
}

bool Matrix::copyContent(IMatrix& dst, const IMatrix& src)
{
	if (&dst == &src) { return true; }

	const size_t nElementIn  = src.getBufferElementCount();
	const size_t nElementOut = dst.getBufferElementCount();
	if (nElementOut != nElementIn) { return false; }
	const double* bufferIn = src.getBuffer();
	double* bufferOut      = dst.getBuffer();
	System::Memory::copy(bufferOut, bufferIn, nElementIn * sizeof(double));
	return true;
}

bool Matrix::clearContent(IMatrix& matrix)
{
	System::Memory::set(matrix.getBuffer(), matrix.getBufferElementCount() * sizeof(double), 0);
	return true;
}

bool Matrix::isDescriptionSimilar(const IMatrix& src1, const IMatrix& src2, const bool checkLabels)
{
	if (&src1 == &src2) { return true; }

	if (src1.getDimensionCount() != src2.getDimensionCount()) { return false; }

	for (size_t i = 0; i < src1.getDimensionCount(); ++i) { if (src1.getDimensionSize(i) != src2.getDimensionSize(i)) { return false; } }

	if (checkLabels)
	{
		for (size_t i = 0; i < src1.getDimensionCount(); ++i)
		{
			for (size_t j = 0; j < src1.getDimensionSize(i); ++j)
			{
				if (strcmp(src1.getDimensionLabel(i, j), src2.getDimensionLabel(i, j)) != 0) { return false; }
			}
		}
	}

	return true;
}

bool Matrix::isContentSimilar(const IMatrix& src1, const IMatrix& src2)
{
	if (&src1 == &src2) { return true; }

	if (src1.getBufferElementCount() != src2.getBufferElementCount()) { return false; }

	return memcmp(src1.getBuffer(), src2.getBuffer(), src1.getBufferElementCount() * sizeof(double)) == 0;
}

bool Matrix::isContentValid(const IMatrix& src, const bool checkNotANumber, const bool checkInfinity)
{
	const double* buffer    = src.getBuffer();
	const double* bufferEnd = src.getBuffer() + src.getBufferElementCount();
	while (buffer != bufferEnd)
	{
		if (checkNotANumber && std::isnan(*buffer)) { return false; }
		if (checkInfinity && std::isinf(*buffer)) { return false; }
		buffer++;
	}
	return true;
}

enum EStatus
{
	Status_Nothing,
	Status_ParsingHeader,
	Status_ParsingHeaderDimension,
	Status_ParsingHeaderLabel,
	Status_ParsingBuffer,
	Status_ParsingBufferValue
};

// tokens in the ascii matrix format
const char CONSTANT_LEFT_SQUARE_BRACKET  = '[';
const char CONSTANT_RIGHT_SQUARE_BRACKET = ']';
const char CONSTANT_HASHTAG              = '#';
const char CONSTANT_DOUBLE_QUOTE         = '"';
const char CONSTANT_TAB                  = '\t';
const char CONSTANT_CARRIAGE_RETURN      = '\r';
const char CONSTANT_EOL                  = '\n';
const char CONSTANT_SPACE                = ' ';

bool Matrix::fromString(IMatrix& matrix, const CString& sString)
{
	std::stringstream buffer;

	buffer << sString.toASCIIString();

	const std::locale locale("C");
	//current string to parse
	std::string what;
	//current parsing status
	size_t status = Status_Nothing;
	//current element index (incremented every time a value is stored in matrix)
	size_t curElementIdx = 0;
	//current dimension index
	size_t curDimIdx = size_t(-1);
	//vector keeping track of dimension sizes
	std::vector<size_t> dimSize;
	//vector keeping track of number of values found in each dimension
	std::vector<size_t> nValue;
	// Dim labels
	std::vector<std::string> labels;
	//current quote-delimited string
	std::string curString;

	do
	{
		//read current line
		std::getline(buffer, what, CONSTANT_EOL);

		//is line empty?
		if (what.length() == 0) { continue; } //skip it

		//output line to be parsed in debug level
		// getLogManager() << LogLevel_Debug << what << "\n";

		//remove ending carriage return (if any) for windows / linux compatibility
		if (what[what.length() - 1] == CONSTANT_CARRIAGE_RETURN) { what.erase(what.length() - 1, 1); }

		//start parsing current line
		auto it = what.begin();

		//parse current line
		while (it != what.end())
		{
			switch (status)
			{
					//initial parsing status
				case Status_Nothing:

					//comments starting
					if (*it == CONSTANT_HASHTAG) { it = what.end() - 1; }								// ignore rest of line by skipping to last character
						//header starting
					else if (*it == CONSTANT_LEFT_SQUARE_BRACKET) { status = Status_ParsingHeader; }	// update status
					else if (!std::isspace(*it, locale))
					{
						return false;
					}								// getLogManager() << LogLevel_Trace << "Unexpected character found on line " << what.c_str() << ", parsing aborted\n";
					break;

					//parse header
				case Status_ParsingHeader:

					//comments starting
					if (*it == CONSTANT_HASHTAG) { it = what.end() - 1; }	//ignore rest of line by skipping to last character
						//new dimension opened
					else if (*it == CONSTANT_LEFT_SQUARE_BRACKET)
					{
						dimSize.resize(dimSize.size() + 1);			//increment dimension count
						curDimIdx++;								//update current dimension index
						status = Status_ParsingHeaderDimension;		//update status
					}
						//finished parsing header
					else if (*it == CONSTANT_RIGHT_SQUARE_BRACKET)
					{
						//ensure at least one dimension was found
						if (dimSize.empty()
						) { return false; } // getLogManager() << LogLevel_Trace << "End of header section reached, found 0 dimensions : parsing aborted\n";

						//resize matrix
						matrix.setDimensionCount(dimSize.size());
						for (size_t i = 0; i < dimSize.size(); ++i) { matrix.setDimensionSize(i, dimSize[i]); }

						nValue.resize(matrix.getDimensionCount());

						// set labels now that we know the matrix size
						size_t element = 0;
						for (size_t i = 0; i < matrix.getDimensionCount(); ++i)
						{
							for (size_t j = 0; j < matrix.getDimensionSize(i); ++j) { matrix.setDimensionLabel(i, j, labels[element++].c_str()); }
						}

						/*
						//dump dimension count and size
						char buf[1024]={'\0'};
						for (size_t i=0; i<dimSize.size(); ++i)
						{
							if(i>0)
							{
								strcat(buf, ", ");
								sprintf(buf+strlen(buf), "%d", dimSize[i]);
							}
							else
							{
								sprintf(buf+strlen(buf), "%d", dimSize[i]);
							}
						}
						getLogManager() << LogLevel_Trace << "End of header section reached, found " << dimSize.size() << " dimensions of size [" << CString(buf) << "]\n";
						*/

						//reset current dimension index
						curDimIdx = size_t(-1);

						//update status
						status = Status_ParsingBuffer;
					}
					else if (!std::isspace(*it, locale))
					{
						// getLogManager() << LogLevel_Trace << "Unexpected character found on line " << what << ", parsing aborted\n";
						return false;
					}
					break;

				case Status_ParsingHeaderDimension:

					//comments starting
					if (*it == CONSTANT_HASHTAG)
					{
						//ignore rest of line by skipping to last character
						it = what.end() - 1;
					}
						//new label found
					else if (*it == CONSTANT_DOUBLE_QUOTE)
					{
						//new element found in current dimension
						dimSize[curDimIdx]++;

						//update status
						status = Status_ParsingHeaderLabel;
					}
						//finished parsing current dimension header
					else if (*it == CONSTANT_RIGHT_SQUARE_BRACKET)
					{
						//update status
						status = Status_ParsingHeader;
					}
					else if (!std::isspace(*it, locale))
					{
						// getLogManager() << LogLevel_Trace << "Unexpected character found on line " << what << ", parsing aborted\n";
						return false;
					}
					break;

					//look for end of label (first '"' char not preceded by the '\' escape char)
				case Status_ParsingHeaderLabel:

					//found '"' char not preceded by escape char : end of label reached
					if (*it == CONSTANT_DOUBLE_QUOTE && *(it - 1) != '\\')
					{
						// We can only attach the label later after we know the size
						labels.push_back(curString);

						// std::cout << " lab " << curDimensionIdx << " " << dimSize[curDimensionIdx]-1 <<  " : " << curString << "\n";

						//clear current string
						curString.erase();

						//update status
						status = Status_ParsingHeaderDimension;
					}
						//otherwise, keep parsing current label
					else { curString.append(1, *it); }
					break;

				case Status_ParsingBuffer:

					//comments starting
					if (*it == CONSTANT_HASHTAG)
					{
						//ignore rest of line by skipping to last character
						it = what.end() - 1;
					}
						//going down one dimension
					else if (*it == CONSTANT_LEFT_SQUARE_BRACKET)
					{
						//update dimension index
						curDimIdx++;

						//ensure dimension count remains in allocated range
						if (curDimIdx == matrix.getDimensionCount())
						{
							// getLogManager() << LogLevel_Trace << "Exceeded expected number of dimensions while parsing values, parsing aborted\n";
							return false;
						}

						//ensure values count remains in allocated range
						if (nValue[curDimIdx] == matrix.getDimensionSize(curDimIdx))
						{
							// getLogManager() << LogLevel_Trace << "Exceeded expected number of values for dimension " << curDimensionIdx << ", parsing aborted\n";
							return false;
						}

						//increment values count for current dimension, if it is not the innermost
						if (curDimIdx < matrix.getDimensionCount() - 1) { nValue[curDimIdx]++; }
					}
						//going up one dimension
					else if (*it == CONSTANT_RIGHT_SQUARE_BRACKET)
					{
						//if we are not in innermost dimension
						if (curDimIdx < matrix.getDimensionCount() - 1)
						{
							//ensure the right number of values was parsed in lower dimension
							if (nValue[curDimIdx + 1] != matrix.getDimensionSize(curDimIdx + 1))
							{
								//getLogManager() << LogLevel_Trace << "Found " << valuesCount[curDimIdx+1] << " values in dimension "
								//		<< curDimensionIdx+1 << ", expected " << op_matrix->getDimensionSize(curDimIdx+1) << ", parsing aborted\n";
								return false;
							}
							//reset values count of lower dimension to 0
							nValue[curDimIdx + 1] = 0;
						}
							//ensure dimension count is correct
						else if (curDimIdx == size_t(-1))
						{
							// getLogManager() << LogLevel_Trace << "Found one too many closing bracket character, parsing aborted\n";
							return false;
						}

						//go up one dimension
						curDimIdx--;
					}
						//non whitespace character found
					else if (!std::isspace(*it, locale))
					{
						//if we are in innermost dimension, assume a value is starting here
						if (curDimIdx == matrix.getDimensionCount() - 1)
						{
							//ensure values parsed so far in current dimension doesn't exceed current dimension size
							if (nValue.back() == matrix.getDimensionSize(curDimIdx))
							{
								//getLogManager() << LogLevel_Trace
								//		<< "Found " << valuesCount.back() << " values in dimension " << curDimensionIdx
								//		<< ", expected " << RDestinationMatrix.getDimensionSize(curDimensionIdx) << ", parsing aborted\n";
								return false;
							}

							//increment values count found in innermost dimension
							nValue[curDimIdx]++;

							//append current character to current string
							curString.append(1, *it);

							//update status
							status = Status_ParsingBufferValue;
						}
						else
						{
							// getLogManager() << LogLevel_Trace << "Unexpected character found on line " << what << ", parsing aborted\n";
							return false;
						}
					}
					break;

					//look for end of value (first '"' char not preceded by the '\' escape char)
				case Status_ParsingBufferValue:

					//values end at first whitespace character or ']' character
					if (std::isspace(*it, locale) == true || *it == CONSTANT_RIGHT_SQUARE_BRACKET)
					{
						//if dimension closing bracket is found
						if (*it == CONSTANT_RIGHT_SQUARE_BRACKET)
						{
							//move back iterator by one character so that closing bracket is taken into account in Status_ParsingBuffer case
							--it;
						}

						//retrieve value
						errno = 0;
						char* end;
						const double value = strtod(curString.c_str(), &end);
#if defined TARGET_OS_Windows
						if (errno == ERANGE)
						{
							//string couldn't be converted to a double
							// getLogManager() << LogLevel_Trace << "Couldn't convert token \"" << sCurString << "\" to floating point value, parsing aborted\n";
							return false;
						}
#endif
						//store value in matrix
						(matrix.getBuffer())[curElementIdx] = value;

						//update element index
						curElementIdx++;

						//reset current string
						curString.erase();

						//update status
						status = Status_ParsingBuffer;
					}
						//otherwise, append current character to current string
					else { curString.append(1, *it); }
					break;

				default:
					break;
			} // switch(status)

			//increment iterator
			++it;
		} // while(it != what.end()) (read each character of current line)
	} while (buffer.good()); //read each line in turn

	//If the file is empty or other (like directory)
	if (nValue.empty()) { return false; }
	//ensure the right number of values were parsed in first dimension
	if (nValue[0] != matrix.getDimensionSize(0))
	{
		//	getLogManager() << LogLevel_Trace <<
		//		"Found " << valuesCount[0] << " values in dimension 0, expected " << op_matrix->getDimensionSize(0) << ", parsing aborted\n";
		return false;
	}

	return true;
}

// A recursive helper function to spool matrix contents to a txt stringstream.
bool dumpMatrixBuffer(const IMatrix& matrix, std::stringstream& buffer, const size_t index1, size_t& index2)
{
	//are we in innermost dimension?
	if (index1 == matrix.getDimensionCount() - 1)
	{
		//dimension start
		for (size_t j = 0; j < index1; ++j) { buffer << CONSTANT_TAB; }
		buffer << CONSTANT_LEFT_SQUARE_BRACKET;

		//dump current cell contents
		for (size_t j = 0; j < matrix.getDimensionSize(index1); j++, index2++) { buffer << CONSTANT_SPACE << matrix.getBuffer()[index2]; }

		//dimension end
		buffer << CONSTANT_SPACE << CONSTANT_RIGHT_SQUARE_BRACKET << CONSTANT_EOL;
	}
	else
	{
		//dump all entries in current dimension
		for (size_t i = 0; i < matrix.getDimensionSize(index1); ++i)
		{
			//dimension start
			for (size_t j = 0; j < index1; ++j) { buffer << CONSTANT_TAB; }
			buffer << CONSTANT_LEFT_SQUARE_BRACKET << CONSTANT_EOL;

			dumpMatrixBuffer(matrix, buffer, index1 + 1, index2);

			//dimension end
			for (size_t j = 0; j < index1; ++j) { buffer << CONSTANT_TAB; }
			buffer << CONSTANT_RIGHT_SQUARE_BRACKET << CONSTANT_EOL;
		}
	}

	return true;
}

bool Matrix::toString(const IMatrix& matrix, CString& sString, const size_t precision /* = 6 */)
{
	std::stringstream buffer;

	buffer << std::scientific;
	buffer.precision(std::streamsize(precision));

	// Dump header

	//header start
	buffer << CONSTANT_LEFT_SQUARE_BRACKET << CONSTANT_EOL;

	//dump labels for each dimension
	for (size_t i = 0; i < matrix.getDimensionCount(); ++i)
	{
		buffer << CONSTANT_TAB << CONSTANT_LEFT_SQUARE_BRACKET;

		for (size_t j = 0; j < matrix.getDimensionSize(i); ++j)
		{
			buffer << CONSTANT_SPACE << CONSTANT_DOUBLE_QUOTE << matrix.getDimensionLabel(i, j) << CONSTANT_DOUBLE_QUOTE;
		}

		buffer << CONSTANT_SPACE << CONSTANT_RIGHT_SQUARE_BRACKET << CONSTANT_EOL;
	}

	//header end
	buffer << CONSTANT_RIGHT_SQUARE_BRACKET << CONSTANT_EOL;

	// Dump buffer using a recursive algorithm
	size_t elementIdx = 0;
	dumpMatrixBuffer(matrix, buffer, 0, elementIdx);

	sString = buffer.str().c_str();

	return true;
}

bool Matrix::loadFromTextFile(IMatrix& matrix, const CString& filename)
{
	std::ifstream dataFile;
	FS::Files::openIFStream(dataFile, filename.toASCIIString(), std::ios_base::in);
	if (!dataFile.is_open()) { return false; }

	std::stringstream buffer;

	buffer << dataFile.rdbuf();

	const bool res = fromString(matrix, CString(buffer.str().c_str()));

	dataFile.close();

	return res;
}

bool Matrix::saveToTextFile(const IMatrix& matrix, const CString& filename, const size_t precision /* = 6 */)
{
	std::ofstream dataFile;
	FS::Files::openOFStream(dataFile, filename.toASCIIString(), std::ios_base::out | std::ios_base::trunc);
	if (!dataFile.is_open()) { return false; }

	CString str;

	if (!toString(matrix, str, precision)) { return false; }

	dataFile << str.toASCIIString();

	dataFile.close();

	return true;
}
