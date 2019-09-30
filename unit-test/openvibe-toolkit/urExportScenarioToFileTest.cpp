#include <iostream>
#include <vector>
#include <tuple>
#include <ovp_global_defines.h>

#include "ovtAssert.h"
#include "ovtTestFixtureCommon.h"

using namespace OpenViBE;
using namespace Kernel;

int urImportScenarioFromFileTest(int argc, char* argv[]);

#define OVP_ClassId_BoxAlgorithm_ClockStimulator OpenViBE::CIdentifier(0x4F756D3F, 0x29FF0B96)
#define OVP_ClassId_BoxAlgorithm_ClockStimulatorDesc OpenViBE::CIdentifier(0x4FD067E9, 0x740D2AF0)
#define OVP_ClassId_BoxAlgorithm_StimulationListener OpenViBE::CIdentifier(0x65731E1D, 0x47DE5276)


#include "urSimpleTestScenarioDefinition.h"

int urExportScenarioToFileTest(int argc, char* argv[])
{
	const char* configurationFile  = argv[1];
	const char* temporaryDirectory = argv[2];

	{
		OpenViBETest::ScopedTest<OpenViBETest::SKernelFixture> fixture(configurationFile);

		auto& context = fixture->context;

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

		CIdentifier emptyScenarioIdentifier;
		context->getScenarioManager().createScenario(emptyScenarioIdentifier);

		CMemoryBuffer memoryBuffer;
		OVT_ASSERT(context->getScenarioManager().exportScenario(memoryBuffer, emptyScenarioIdentifier, OVP_GD_ClassId_Algorithm_XMLScenarioExporter),
				   "Failed to export an empty scenario");
		OVT_ASSERT(
			context->getScenarioManager().exportScenarioToFile((std::string(temporaryDirectory) + "/test-scenario-empty.mxs").c_str(), emptyScenarioIdentifier,
				OVP_GD_ClassId_Algorithm_XMLScenarioExporter), "Failed to export an empty scenario to a file");

		CIdentifier importedEmptyScenarioIdentifier;
		OVT_ASSERT(context->getScenarioManager().importScenario(importedEmptyScenarioIdentifier, memoryBuffer, OVP_GD_ClassId_Algorithm_XMLScenarioImporter),
				   "Failed to import an empty exported buffer");

		// Create a scenario to be imported by the importer test
		CIdentifier simpleScenarioIdentifier;
		context->getScenarioManager().createScenario(simpleScenarioIdentifier);

		IScenario& scenario = context->getScenarioManager().getScenario(simpleScenarioIdentifier);

		// Test scenario attributes
		for (auto& attribute : simpleScenarioAttributes) { scenario.addAttribute(std::get<0>(attribute), std::get<1>(attribute).c_str()); }

		uint32_t settingIndex = 0;
		for (auto& setting : simpleScenarioSettings)
		{
			scenario.addSetting(std::get<1>(setting).c_str(), std::get<0>(setting), std::get<2>(setting).c_str());
			scenario.setSettingValue(settingIndex, std::get<3>(setting).c_str());
			settingIndex += 1;
		}

		CIdentifier actualClockStimulatorBoxId;
		scenario.addBox(actualClockStimulatorBoxId, OVP_ClassId_BoxAlgorithm_ClockStimulator, s_ClockStimulatorBoxId);

		CIdentifier actualStimulationListenerBoxId;
		scenario.addBox(actualStimulationListenerBoxId, OVP_ClassId_BoxAlgorithm_StimulationListener, s_StimulationListenerBoxId);

		IBox* stimulatorListenerBox = scenario.getBoxDetails(s_StimulationListenerBoxId);
		stimulatorListenerBox->addInput("Stimulation stream 2", OV_TypeId_Stimulations);

		int scenarioInputIdx = 0;
		for (auto& scenarioInput : simpleScenarioInputs)
		{
			scenario.addInput(std::get<1>(scenarioInput).c_str(), std::get<0>(scenarioInput));
			if (std::get<2>(scenarioInput) != OV_UndefinedIdentifier)
			{
				scenario.setScenarioInputLink(scenarioInputIdx, std::get<2>(scenarioInput), std::get<3>(scenarioInput));
			}
			scenarioInputIdx += 1;
		}

		int scenarioOutputIdx = 0;
		for (auto& scenarioOutput : simpleScenarioOutputs)
		{
			scenario.addOutput(std::get<1>(scenarioOutput).c_str(), std::get<0>(scenarioOutput));
			if (std::get<2>(scenarioOutput) != OV_UndefinedIdentifier)
			{
				scenario.setScenarioOutputLink(scenarioOutputIdx, std::get<2>(scenarioOutput), std::get<3>(scenarioOutput));
			}
			scenarioOutputIdx += 1;
		}

		CIdentifier actualClockStimulatorToStimulationListenerLinkId;
		scenario.connect(actualClockStimulatorToStimulationListenerLinkId, s_ClockStimulatorBoxId, 0, s_StimulationListenerBoxId, 1,
						 s_ClockStimulatorToStimulationListenerLinkId);

		CIdentifier actualSimpleCommentIdentifier;
		scenario.addComment(actualSimpleCommentIdentifier, s_SimpleCommentId);

		IComment* simpleComment = scenario.getCommentDetails(s_SimpleCommentId);
		simpleComment->setText("Content of a comment");

		CIdentifier actualUnicodeCommentIdentifier;
		scenario.addComment(actualUnicodeCommentIdentifier, s_UnicodeCommentId);

		IComment* unicodeComment = scenario.getCommentDetails(s_UnicodeCommentId);
		unicodeComment->setText("This comment contains a newline\nand unicode characters 日本語");


		OVT_ASSERT(
			context->getScenarioManager().exportScenarioToFile((std::string(temporaryDirectory) + "/" + s_SimpleScenarioFileName).c_str(),
				simpleScenarioIdentifier, OVP_GD_ClassId_Algorithm_XMLScenarioExporter), "Failed to export a simple scenario");
	}

	return urImportScenarioFromFileTest(argc, argv);
}
