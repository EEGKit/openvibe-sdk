#include <iostream>
#include <string>

#include "gtest/gtest.h"

#include "ovtAssert.h"
#include "ovtTestFixtureCommon.h"

#include <ovp_global_defines.h>
#include <array>

using namespace OpenViBE;
using namespace Kernel;

// DO NOT USE a global OpenViBETest::ScopedTest<OpenViBETest::SKernelFixture> variable here
// because it causes a bug due to plugins global descriptors beeing destroyed before
// the kernel context.
IKernelContext* context = nullptr;
std::string g_dataDirectory;


bool importScenarioFromFile(const char* filename)
{
	const std::string scenarioFilePath = std::string(g_dataDirectory) + "/" + filename;

	context->getErrorManager().releaseErrors();

	CIdentifier scenarioID;
	if (context->getScenarioManager().importScenarioFromFile(scenarioID, scenarioFilePath.c_str(), OVP_GD_ClassId_Algorithm_XMLScenarioImporter))
	{
		context->getScenarioManager().releaseScenario(scenarioID);
		return true;
	}

	return false;
}

// should be called after importScenarioFromFile
bool checkForSchemaValidationError()
{
	auto& errorManager = context->getErrorManager();
	auto error         = errorManager.getLastError();

	while (error)
	{
		if (error->getErrorType() == ErrorType::BadXMLSchemaValidation) { return true; }
		error = error->getNestedError();
	}

	return false;
}


TEST(validate_scenario_test_case, test_no_false_positive)
{
	const std::array<char*, 3> files = { "test-scenario-false-positive1.mxs", "test-scenario-false-positive2.mxs", "test-scenario-false-positive3.mxs" };

	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(context != nullptr);

	for (uint32_t i = 0; i < 3; i++) { EXPECT_TRUE(importScenarioFromFile(files[i])); }
}

TEST(validate_scenario_test_case, test_root)
{
	const std::array<char*, 9> files = {
		"test-root-dup-attributes.mxs",
		"test-root-dup-boxes.mxs",
		"test-root-dup-comments.mxs",
		"test-root-dup-creator.mxs",
		"test-root-dup-inputs.mxs",
		"test-root-dup-links.mxs",
		"test-root-dup-outputs.mxs",
		"test-root-dup-settings.mxs",
		"test-root-dup-version.mxs"
	};

	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(context != nullptr);

	for (uint32_t i = 0; i < 9; i++)
	{
		EXPECT_FALSE(importScenarioFromFile(files[i]));
		EXPECT_TRUE(checkForSchemaValidationError());
	}
}

TEST(validate_scenario_test_case, test_attribute)
{
	const std::array<char*, 4> files = {
		"test-attribute-dup-id.mxs",
		"test-attribute-dup-value.mxs",
		"test-attribute-missing-id.mxs",
		"test-attribute-missing-value.mxs"
	};

	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(context != nullptr);

	for (uint32_t i = 0; i < 4; i++)
	{
		EXPECT_FALSE(importScenarioFromFile(files[i]));
		EXPECT_TRUE(checkForSchemaValidationError());
	}
}

TEST(validate_scenario_test_case, test_box)
{
	const std::array<char*, 10> files = {
		"test-box-dup-algo.mxs",
		"test-box-dup-attributes.mxs",
		"test-box-dup-id.mxs",
		"test-box-dup-inputs.mxs",
		"test-box-dup-name.mxs",
		"test-box-dup-outputs.mxs",
		"test-box-dup-settings.mxs",
		"test-box-missing-algo.mxs",
		"test-box-missing-id.mxs",
		"test-box-missing-name.mxs"
	};

	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(context != nullptr);

	for (uint32_t i = 0; i < 10; i++)
	{
		EXPECT_FALSE(importScenarioFromFile(files[i]));
		EXPECT_TRUE(checkForSchemaValidationError());
	}
}

TEST(validate_scenario_test_case, test_comment)
{
	const std::array<char*, 5> files = {
		"test-comment-dup-attributes.mxs",
		"test-comment-dup-id.mxs",
		"test-comment-dup-text.mxs",
		"test-comment-missing-id.mxs",
		"test-comment-missing-text.mxs"
	};

	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(context != nullptr);

	for (uint32_t i = 0; i < 5; i++)
	{
		EXPECT_FALSE(importScenarioFromFile(files[i]));
		EXPECT_TRUE(checkForSchemaValidationError());
	}
}

TEST(validate_scenario_test_case, test_input)
{
	const std::array<char*, 4> files = {
		"test-input-dup-id.mxs",
		"test-input-dup-name.mxs",
		"test-input-missing-id.mxs",
		"test-input-missing-name.mxs"
	};

	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(context != nullptr);

	for (uint32_t i = 0; i < 4; i++)
	{
		EXPECT_FALSE(importScenarioFromFile(files[i]));
		EXPECT_TRUE(checkForSchemaValidationError());
	}
}

TEST(validate_scenario_test_case, test_link)
{
	const std::array<char*, 7> files = {
		"test-link-dup-attributes.mxs",
		"test-link-dup-id.mxs",
		"test-link-dup-source.mxs",
		"test-link-dup-target.mxs",
		"test-link-missing-id.mxs",
		"test-link-missing-source.mxs",
		"test-link-missing-target.mxs"
	};

	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(context != nullptr);

	for (uint32_t i = 0; i < 7; i++)
	{
		EXPECT_FALSE(importScenarioFromFile(files[i]));
		EXPECT_TRUE(checkForSchemaValidationError());
	}
}

TEST(validate_scenario_test_case, test_output)
{
	const std::array<char*, 4> files = {
		"test-output-dup-id.mxs",
		"test-output-dup-name.mxs",
		"test-output-missing-id.mxs",
		"test-output-missing-name.mxs"
	};

	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(context != nullptr);

	for (uint32_t i = 0; i < 4; i++)
	{
		EXPECT_FALSE(importScenarioFromFile(files[i]));
		EXPECT_TRUE(checkForSchemaValidationError());
	}
}

TEST(validate_scenario_test_case, test_setting)
{
	const std::array<char*, 9> files = {
		"test-setting-bad-modif.mxs",
		"test-setting-dup-default.mxs",
		"test-setting-dup-id.mxs",
		"test-setting-dup-modif.mxs",
		"test-setting-dup-name.mxs",
		"test-setting-dup-value.mxs",
		"test-setting-missing-default.mxs",
		"test-setting-missing-id.mxs",
		"test-setting-missing-name.mxs"
	};

	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(context != nullptr);

	for (uint32_t i = 0; i < 9; i++)
	{
		EXPECT_FALSE(importScenarioFromFile(files[i]));
		EXPECT_TRUE(checkForSchemaValidationError());
	}
}

TEST(validate_scenario_test_case, test_source)
{
	const std::array<char*, 5> files = {
		"test-source-bad-index.mxs",
		"test-source-dup-id.mxs",
		"test-source-dup-index.mxs",
		"test-source-missing-id.mxs",
		"test-source-missing-index.mxs"
	};

	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(context != nullptr);

	for (uint32_t i = 0; i < 5; i++)
	{
		EXPECT_FALSE(importScenarioFromFile(files[i]));
		EXPECT_TRUE(checkForSchemaValidationError());
	}
}

TEST(validate_scenario_test_case, test_target)
{
	const std::array<char*, 5> files = {
		"test-target-bad-index.mxs",
		"test-target-dup-id.mxs",
		"test-target-dup-index.mxs",
		"test-target-missing-id.mxs",
		"test-target-missing-index.mxs"

	};

	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(context != nullptr);

	for (uint32_t i = 0; i < 5; i++)
	{
		EXPECT_FALSE(importScenarioFromFile(files[i]));
		EXPECT_TRUE(checkForSchemaValidationError());
	}
}

int urValidateScenarioTest(int argc, char* argv[])
{
	OVT_ASSERT(argc >= 3, "Failure retrieve test parameters");

	OpenViBETest::ScopedTest<OpenViBETest::SKernelFixture> fixture;
	fixture->setConfigurationFile(argv[1]);

	g_dataDirectory = argv[2];
	context       = fixture->context;

#if defined TARGET_OS_Windows
	context->getPluginManager().addPluginsFromFiles(Directories::getLibDir() + "/openvibe-plugins-sdk-file-io*dll");
	context->getPluginManager().addPluginsFromFiles(Directories::getLibDir() + "/openvibe-plugins-sdk-stimulation*dll");
	context->getPluginManager().addPluginsFromFiles(Directories::getLibDir() + "/openvibe-plugins-sdk-tools*dll");
#elif defined TARGET_OS_Linux
	context->getPluginManager().addPluginsFromFiles(OpenViBE::Directories::getLibDir() + "/libopenvibe-plugins-sdk-file-io*so");
	context->getPluginManager().addPluginsFromFiles(OpenViBE::Directories::getLibDir() + "/libopenvibe-plugins-sdk-stimulation*so");
	context->getPluginManager().addPluginsFromFiles(OpenViBE::Directories::getLibDir() + "/libopenvibe-plugins-sdk-tools*so");
#elif defined TARGET_OS_MacOS
	context->getPluginManager().addPluginsFromFiles(OpenViBE::Directories::getLibDir() + "/libopenvibe-plugins-sdk-file-io*dylib");
	context->getPluginManager().addPluginsFromFiles(OpenViBE::Directories::getLibDir() + "/libopenvibe-plugins-sdk-stimulation*dylib");
	context->getPluginManager().addPluginsFromFiles(OpenViBE::Directories::getLibDir() + "/libopenvibe-plugins-sdk-tools*dylib");
#endif

	testing::InitGoogleTest(&argc, argv);
	::testing::GTEST_FLAG(filter) = "validate_scenario_test_case.*";
	return RUN_ALL_TESTS();
}
