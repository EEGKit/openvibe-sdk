#include <iostream>

#include "gtest/gtest.h"

#include "ovtAssert.h"
#include "ovtTestFixtureCommon.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;

// DO NOT USE a global OpenViBETest::ScopedTest<OpenViBETest::SKernelFixture> variable here
// because it causes a bug due to plugins global descriptors beeing destroyed before the kernel context.
IKernelContext* context = nullptr;

TEST(error_manager_test_case, test_init)
{
	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(context != nullptr);

	auto& errorManager = context->getErrorManager();

	// check manager is correctly initialized
	EXPECT_FALSE(errorManager.hasError());
	EXPECT_TRUE(errorManager.getLastError() == nullptr);
	EXPECT_TRUE(std::string(errorManager.getLastErrorString()).empty());
	EXPECT_EQ(errorManager.getLastErrorType(), ErrorType::NoErrorFound);

	EXPECT_NO_THROW(errorManager.releaseErrors());
}

TEST(error_manager_test_case, test_push)
{
	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(context != nullptr);

	auto& errorManager = context->getErrorManager();

	// push an error
	errorManager.pushError(ErrorType::Overflow, "An integer overflow error occurred");

	EXPECT_TRUE(errorManager.hasError());
	EXPECT_STREQ(errorManager.getLastErrorString(), "An integer overflow error occurred");
	EXPECT_EQ(errorManager.getLastErrorType(), ErrorType::Overflow);

	// test match error features returned direclty by manager match error features
	auto error = errorManager.getLastError();

	ASSERT_TRUE(error != nullptr);
	EXPECT_STREQ(error->getErrorString(), "An integer overflow error occurred");
	EXPECT_EQ(error->getErrorType(), ErrorType::Overflow);
	EXPECT_STREQ(error->getErrorLocation(), "NoLocationInfo:0");
	EXPECT_TRUE(error->getNestedError() == nullptr);

	// push another error
	errorManager.pushErrorAtLocation(ErrorType::BadAlloc, "Memory allocation failed", "urErrorManagerTest.cpp", 64);

	// test top error has changed
	EXPECT_STREQ(errorManager.getLastErrorString(), "Memory allocation failed");
	EXPECT_EQ(errorManager.getLastErrorType(), ErrorType::BadAlloc);

	error = errorManager.getLastError();
	ASSERT_TRUE(error != nullptr);
	EXPECT_STREQ(error->getErrorString(), "Memory allocation failed");
	EXPECT_EQ(error->getErrorType(), ErrorType::BadAlloc);
	EXPECT_STREQ(error->getErrorLocation(), "urErrorManagerTest.cpp:64");

	auto nestedError = error->getNestedError();
	ASSERT_TRUE(nestedError != nullptr);
	EXPECT_STREQ(nestedError->getErrorString(), "An integer overflow error occurred");
	EXPECT_EQ(nestedError->getErrorType(), ErrorType::Overflow);
	EXPECT_STREQ(nestedError->getErrorLocation(), "NoLocationInfo:0");
	EXPECT_TRUE(nestedError->getNestedError() == nullptr);
}

TEST(error_manager_test_case, test_release)
{
	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(context != nullptr);

	auto& errorManager = context->getErrorManager();
	errorManager.releaseErrors();

	// check manager is correctly released
	EXPECT_FALSE(errorManager.hasError());
	EXPECT_TRUE(errorManager.getLastError() == nullptr);
	EXPECT_TRUE(std::string(errorManager.getLastErrorString()).empty());
	EXPECT_EQ(errorManager.getLastErrorType(), ErrorType::NoErrorFound);

	// add an error after release
	errorManager.pushErrorAtLocation(ErrorType::ResourceNotFound, "File not found on system", "urErrorManagerTest.cpp", 93);

	EXPECT_TRUE(errorManager.hasError());
	EXPECT_STREQ(errorManager.getLastErrorString(), "File not found on system");
	EXPECT_EQ(errorManager.getLastErrorType(), ErrorType::ResourceNotFound);

	auto error = errorManager.getLastError();

	ASSERT_TRUE(error != nullptr);
	EXPECT_STREQ(error->getErrorString(), "File not found on system");
	EXPECT_EQ(error->getErrorType(), ErrorType::ResourceNotFound);
	EXPECT_STREQ(error->getErrorLocation(), "urErrorManagerTest.cpp:93");
	EXPECT_TRUE(error->getNestedError() == nullptr);
}

TEST(error_manager_test_case, test_stress_push)
{
	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(context != nullptr);

	auto& errorManager = context->getErrorManager();

	errorManager.releaseErrors();
	const size_t expectedErrorCount = 10;
	for (size_t i = 0; i < expectedErrorCount; ++i) { errorManager.pushError(ErrorType::Unknown, "Error"); }

	size_t errorCount = 0;
	auto error        = errorManager.getLastError();
	while (error)
	{
		errorCount++;
		error = error->getNestedError();
	}

	EXPECT_EQ(errorCount, expectedErrorCount);
}

int urErrorManagerTest(int argc, char* argv[])
{
	OVT_ASSERT(argc >= 2, "Failure retrieve test parameters");

	OpenViBETest::ScopedTest<OpenViBETest::SKernelFixture> fixture;
	fixture->setConfigFile(argv[1]);

	context = fixture->context;

	testing::InitGoogleTest(&argc, argv);
	::testing::GTEST_FLAG(filter) = "error_manager_test_case.*";
	return RUN_ALL_TESTS();
}
