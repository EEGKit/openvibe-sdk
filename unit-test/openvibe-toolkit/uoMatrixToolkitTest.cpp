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

#include <iostream>

#include <toolkit/ovtk_all.h>
#include <system/ovCMath.h>

#include "ovtAssert.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Toolkit;

void fillMatrix(CMatrix& matrix)
{
	for (size_t i = 0; i < matrix.getDimensionCount(); ++i)
	{
		for (size_t j = 0; j < matrix.getDimensionSize(i); ++j)
		{
			std::stringstream dimensionLabel;

			dimensionLabel << "Label " << j + 1 << " of Dimension " << i + 1;

			matrix.setDimensionLabel(i, j, dimensionLabel.str().c_str());
		}
	}

	for (size_t i = 0; i < matrix.getSize(); ++i)
	{
		matrix.getBuffer()[i] = System::Math::random0To1() * double(int8_t(System::Math::randomI()));
	}
}

bool testMatrix(CMatrix& expectedMatrix, const std::string& textFile, const size_t precision = 6)
{
	const double threshold = 1.0 / std::pow(10.0, double(precision - 2));

	fillMatrix(expectedMatrix);

	if (!Matrix::saveToTextFile(expectedMatrix, textFile.c_str(), precision))
	{
		std::cerr << "Error: saving matrix to file " << textFile << "\n";
		return false;
	}

	CMatrix resultMatrix;

	if (!Matrix::loadFromTextFile(resultMatrix, textFile.c_str()))
	{
		std::cerr << "Error: loading matrix from file " << textFile << "\n";
		return false;
	}

	if (!Matrix::isDescriptionSimilar(expectedMatrix, resultMatrix))
	{
		std::cerr << "Error: Descriptions differ between expected matrix and result matrix after save/load\n";
		return false;
	}

	for (size_t i = 0; i < expectedMatrix.getSize(); ++i)
	{
		const double error = std::fabs(expectedMatrix.getBuffer()[i] - resultMatrix.getBuffer()[i]);

		if (error > threshold)
		{
			std::cerr << "Error: Data differs at index " << i << ", error " << error << " (thresold = " << threshold << ")\n";
			return false;
		}
	}

	return true;
}

int uoMatrixToolkitTest(int argc, char* argv[])
{
	OVT_ASSERT(argc == 2, "Failure to retrieve tests arguments. Expecting: output_dir");

	const std::string oMatrixFile = std::string(argv[1]) + "uoMatrixToolkitTest.txt";

	System::Math::initializeRandomMachine(777);
	CMatrix source;

	source.setDimensionCount(1);
	source.setDimensionSize(0, 1);

	OVT_ASSERT(testMatrix(source, oMatrixFile), "Failed to test matrix with parameters [dimension_count; dimension_size] = [1; {0,1}]");

	source.setDimensionCount(1);
	source.setDimensionSize(0, 5);

	OVT_ASSERT(testMatrix(source, oMatrixFile), "Failed to test matrix with parameters [dimension_count; dimension_size] = [1; {0,5}]");

	source.setDimensionCount(2);
	source.setDimensionSize(0, 1);
	source.setDimensionSize(1, 1);

	OVT_ASSERT(testMatrix(source, oMatrixFile), "Failed to test matrix with parameters [dimension_count; dimension_size] = [2; {0,1},{1,1}]");

	source.setDimensionCount(2);
	source.setDimensionSize(0, 1);
	source.setDimensionSize(1, 7);

	OVT_ASSERT(testMatrix(source, oMatrixFile), "Failed to test matrix with parameters [dimension_count; dimension_size] = [2; {0,1},{1,7}]");

	source.setDimensionCount(2);
	source.setDimensionSize(0, 9);
	source.setDimensionSize(1, 1);

	OVT_ASSERT(testMatrix(source, oMatrixFile), "Failed to test matrix with parameters [dimension_count; dimension_size] = [2; {0,9},{1,1}]");

	source.setDimensionCount(2);
	source.setDimensionSize(0, 2);
	source.setDimensionSize(1, 4);

	OVT_ASSERT(testMatrix(source, oMatrixFile), "Failed to test matrix with parameters [dimension_count; dimension_size] = [2; {0,2},{1,4}]");

	source.setDimensionCount(2);
	source.setDimensionSize(0, 3);
	source.setDimensionSize(1, 15);

	OVT_ASSERT(testMatrix(source, oMatrixFile), "Failed to test matrix with parameters [dimension_count; dimension_size] = [2; {0,3},{1,15}]");

	source.setDimensionCount(3);
	source.setDimensionSize(0, 1);
	source.setDimensionSize(1, 1);
	source.setDimensionSize(2, 1);

	OVT_ASSERT(testMatrix(source, oMatrixFile), "Failed to test matrix with parameters [dimension_count; dimension_size] = [3; {0,1},{1,1},{2,1}]");

	source.setDimensionCount(3);
	source.setDimensionSize(0, 1);
	source.setDimensionSize(1, 1);
	source.setDimensionSize(2, 5);

	OVT_ASSERT(testMatrix(source, oMatrixFile), "Failed to test matrix with parameters [dimension_count; dimension_size] = [3; {0,1},{1,1},{2,5}]");

	source.setDimensionCount(3);
	source.setDimensionSize(0, 2);
	source.setDimensionSize(1, 3);
	source.setDimensionSize(2, 6);

	OVT_ASSERT(testMatrix(source, oMatrixFile), "Failed to test matrix with parameters [dimension_count; dimension_size] = [3; {0,2},{1,3},{2,6}]");

	source.setDimensionCount(4);
	source.setDimensionSize(0, 9);
	source.setDimensionSize(1, 5);
	source.setDimensionSize(2, 2);
	source.setDimensionSize(3, 3);

	OVT_ASSERT(testMatrix(source, oMatrixFile), "Failed to test matrix with parameters [dimension_count; dimension_size] = [4; {0,9},{1,5},{2,2},{3,3}]");

	// special cases at boundaries
	source.setDimensionCount(2);
	source.setDimensionSize(0, 0);
	source.setDimensionSize(1, 0);

	OVT_ASSERT(testMatrix(source, oMatrixFile), "Failed to test matrix with parameters [dimension_count; dimension_size] = [2; {0,0},{1,0}]");

	CMatrix emptySource;
	OVT_ASSERT(!testMatrix(emptySource, oMatrixFile), "Failed to test matrix with no parameter");

	return EXIT_SUCCESS;
}

//==========================End OF File==============================
