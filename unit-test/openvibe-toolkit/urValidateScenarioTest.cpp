#include <iostream>
#include <string>

#include "gtest/gtest.h"

#include "ovtAssert.h"
#include "ovtTestFixtureCommon.h"

#include <ovp_global_defines.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;

// DO NOT USE a global OpenViBETest::ScopedTest<OpenViBETest::KernelFixture> variable here
// because it causes a bug due to plugins global descriptors beeing destroyed before
// the kernel context.
OpenViBE::Kernel::IKernelContext* g_context = nullptr;
std::string g_dataDirectory;


bool importScenarioFromFile(const char* filename)
{
	std::string scenarioFilePath = std::string(g_dataDirectory) + "/" + filename;

	g_context->getErrorManager().releaseErrors();

	CIdentifier scenarioIdentifier;
	if(g_context->getScenarioManager().importScenarioFromFile(scenarioIdentifier, scenarioFilePath.c_str(), OVP_GD_ClassId_Algorithm_XMLScenarioImporter))
	{
		g_context->getScenarioManager().releaseScenario(scenarioIdentifier);
		return true;
	}

	return false;
}

// should be called after importScenarioFromFile
bool checkForSchemaValidationError()
{
	auto& errorManager = g_context->getErrorManager();

	auto error = errorManager.getLastError();

	while(error)
	{
		if(error->getErrorType() == OpenViBE::Kernel::ErrorType::BadXMLSchemaValidation)
		{
			return true;
		}

		error = error->getNestedError();
	}

	return false;
}


TEST(validate_scenario_test_case, test_no_false_positive)
{
	const char* files[3] = {
		"test-scenario-false-positive1.mxs",
		"test-scenario-false-positive2.mxs",
		"test-scenario-false-positive3.mxs"
	};

	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(g_context != nullptr);

	for(unsigned int i = 0; i < 3; i++)
	{
		EXPECT_TRUE(importScenarioFromFile(files[i]));
	}
}

TEST(validate_scenario_test_case, test_root)
{
	const char* files[9] = {
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
	ASSERT_TRUE(g_context != nullptr);

	for(unsigned int i = 0; i < 9; i++)
	{
		EXPECT_FALSE(importScenarioFromFile(files[i]));
		EXPECT_TRUE(checkForSchemaValidationError());
	}
}

TEST(validate_scenario_test_case, test_attribute)
{
	const char* files[4] = {
		"test-attribute-dup-id.mxs",
		"test-attribute-dup-value.mxs",
		"test-attribute-missing-id.mxs",
		"test-attribute-missing-value.mxs"
	};

	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(g_context != nullptr);

	for(unsigned int i = 0; i < 4; i++)
	{
		EXPECT_FALSE(importScenarioFromFile(files[i]));
		EXPECT_TRUE(checkForSchemaValidationError());
	}
}

TEST(validate_scenario_test_case, test_box)
{
	const char* files[10] = {
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
	ASSERT_TRUE(g_context != nullptr);

	for(unsigned int i = 0; i < 10; i++)
	{
		EXPECT_FALSE(importScenarioFromFile(files[i]));
		EXPECT_TRUE(checkForSchemaValidationError());
	}
}

TEST(validate_scenario_test_case, test_comment)
{
	const char* files[5] = {
		"test-comment-dup-attributes.mxs",
		"test-comment-dup-id.mxs",
		"test-comment-dup-text.mxs",
		"test-comment-missing-id.mxs",
		"test-comment-missing-text.mxs"
	};

	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(g_context != nullptr);

	for(unsigned int i = 0; i < 5; i++)
	{
		EXPECT_FALSE(importScenarioFromFile(files[i]));
		EXPECT_TRUE(checkForSchemaValidationError());
	}
}

TEST(validate_scenario_test_case, test_input)
{
	const char* files[4] = {
		"test-input-dup-id.mxs",
		"test-input-dup-name.mxs",
		"test-input-missing-id.mxs",
		"test-input-missing-name.mxs"
	};

	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(g_context != nullptr);

	for(unsigned int i = 0; i < 4; i++)
	{
		EXPECT_FALSE(importScenarioFromFile(files[i]));
		EXPECT_TRUE(checkForSchemaValidationError());
	}
}

TEST(validate_scenario_test_case, test_link)
{
	const char* files[7] = {
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
	ASSERT_TRUE(g_context != nullptr);

	for(unsigned int i = 0; i < 7; i++)
	{
		EXPECT_FALSE(importScenarioFromFile(files[i]));
		EXPECT_TRUE(checkForSchemaValidationError());
	}
}

TEST(validate_scenario_test_case, test_output)
{
	const char* files[4] = {
		"test-output-dup-id.mxs",
		"test-output-dup-name.mxs",
		"test-output-missing-id.mxs",
		"test-output-missing-name.mxs"
	};

	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(g_context != nullptr);

	for(unsigned int i = 0; i < 4; i++)
	{
		EXPECT_FALSE(importScenarioFromFile(files[i]));
		EXPECT_TRUE(checkForSchemaValidationError());
	}
}

TEST(validate_scenario_test_case, test_setting)
{
	const char* files[9] = {
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
	ASSERT_TRUE(g_context != nullptr);

	for(unsigned int i = 0; i < 9; i++)
	{
		EXPECT_FALSE(importScenarioFromFile(files[i]));
		EXPECT_TRUE(checkForSchemaValidationError());
	}
}

TEST(validate_scenario_test_case, test_source)
{
	const char* files[5] = {
		"test-source-bad-index.mxs",
		"test-source-dup-id.mxs",
		"test-source-dup-index.mxs",
		"test-source-missing-id.mxs",
		"test-source-missing-index.mxs"
	};

	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(g_context != nullptr);

	for(unsigned int i = 0; i < 5; i++)
	{
		EXPECT_FALSE(importScenarioFromFile(files[i]));
		EXPECT_TRUE(checkForSchemaValidationError());
	}
}

TEST(validate_scenario_test_case, test_target)
{
	const char* files[5] = {
		"test-target-bad-index.mxs",
		"test-target-dup-id.mxs",
		"test-target-dup-index.mxs",
		"test-target-missing-id.mxs",
		"test-target-missing-index.mxs"

	};

	// here we use assert because we want to fail directly
	// in order to avoid a segfault
	ASSERT_TRUE(g_context != nullptr);

	for(unsigned int i = 0; i < 5; i++)
	{
		EXPECT_FALSE(importScenarioFromFile(files[i]));
		EXPECT_TRUE(checkForSchemaValidationError());
	}
}

int urValidateScenarioTest(int argc, char* argv[])
{
	OVT_ASSERT(argc >= 3, "Failure retrieve test parameters");

	OpenViBETest::ScopedTest<OpenViBETest::KernelFixture> fixture;
	fixture->setConfigurationFile(argv[1]);

	g_dataDirectory = argv[2];
	g_context = fixture->context;

	#if defined TARGET_OS_Windows
	g_context->getPluginManager().addPluginsFromFiles(OpenViBE::Directories::getLibDir() + "/openvibe-plugins-sdk-file-io*dll");
	g_context->getPluginManager().addPluginsFromFiles(OpenViBE::Directories::getLibDir() + "/openvibe-plugins-sdk-stimulation*dll");
	g_context->getPluginManager().addPluginsFromFiles(OpenViBE::Directories::getLibDir() + "/openvibe-plugins-sdk-tools*dll");
	#elif defined TARGET_OS_Linux
	g_context->getPluginManager().addPluginsFromFiles(OpenViBE::Directories::getLibDir() + "/libopenvibe-plugins-sdk-file-io*so");
	g_context->getPluginManager().addPluginsFromFiles(OpenViBE::Directories::getLibDir() + "/libopenvibe-plugins-sdk-stimulation*so");
	g_context->getPluginManager().addPluginsFromFiles(OpenViBE::Directories::getLibDir() + "/libopenvibe-plugins-sdk-tools*so");
	#elif defined TARGET_OS_MacOS
	g_context->getPluginManager().addPluginsFromFiles(OpenViBE::Directories::getLibDir() + "/libopenvibe-plugins-sdk-file-io*dylib");
	g_context->getPluginManager().addPluginsFromFiles(OpenViBE::Directories::getLibDir() + "/libopenvibe-plugins-sdk-stimulation*dylib");
	g_context->getPluginManager().addPluginsFromFiles(OpenViBE::Directories::getLibDir() + "/libopenvibe-plugins-sdk-tools*dylib");
	#endif

	::testing::InitGoogleTest(&argc, argv);
	::testing::GTEST_FLAG(filter) = "validate_scenario_test_case.*";
	return RUN_ALL_TESTS();
}


