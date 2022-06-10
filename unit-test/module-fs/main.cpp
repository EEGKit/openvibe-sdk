#include <gtest/gtest.h>

// ReSharper disable CppUnusedIncludeDirective
#include "Test_Files.hpp"
#include "Test_Files_UTF.hpp"
// ReSharper restore CppUnusedIncludeDirective

int main(int argc, char** argv)
{
	try {
		testing::InitGoogleTest(&argc, argv);
		return RUN_ALL_TESTS();
	}
	catch (std::exception&) { return 1; }
}
