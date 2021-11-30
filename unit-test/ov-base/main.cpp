#include <gtest/gtest.h>

// ReSharper disable CppUnusedIncludeDirective
#include "CMatrixTests.hpp"
#include "CTimeTests.hpp"
#include "CIdentifierTests.hpp"
#include "CStimulationSetTests.hpp"
#include "CErrorManagerTests.hpp"

int main(int argc, char* argv[])
{
	try {
		testing::InitGoogleTest(&argc, argv);
		return RUN_ALL_TESTS();
	}
	catch (std::exception&) { return 1; }
}
