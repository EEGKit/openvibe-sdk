#include <gtest/gtest.h>

// ReSharper disable CppUnusedIncludeDirective
#include "CMatrixTest.hpp"
#include "CStimulationTest.hpp"
#include "CTimeTest.hpp"
#include "directoriesTest.hpp"

int main(int argc, char* argv[])
{
	try
	{
		testing::InitGoogleTest(&argc, argv);
		return RUN_ALL_TESTS();
	}
	catch (std::exception&) { return 1; }
}
