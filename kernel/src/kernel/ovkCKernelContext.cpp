#include "ovkCKernelContext.h"
#include "ovkCKernelObjectFactory.h"
#include "ovkCTypeManager.h"

#include <openvibe/ovCIdentifier.h>

#include "algorithm/ovkCAlgorithmManager.h"
#include "configuration/ovkCConfigurationManager.h"
#include "player/ovkCPlayerManager.h"
#include "plugins/ovkCPluginManager.h"
#include "metabox/ovkCMetaboxManager.h"
#include "scenario/ovkCScenarioManager.h"
#include "log/ovkCLogManager.h"
#include "log/ovkCLogListenerConsole.h"
#include "log/ovkCLogListenerFile.h"
#include "error/ovkCErrorManager.h"

#include <cassert>
#include <string>
#include <algorithm>
#include <functional>
#include <cctype>

#include <fs/Files.h>

using namespace OpenViBE;
using namespace Kernel;

CKernelContext::CKernelContext(const IKernelContext* pMasterKernelContext, const CString& rApplicationName, const CString& rConfigurationFile)
	: m_rMasterKernelContext(pMasterKernelContext ? *pMasterKernelContext : *this)
	  , m_pAlgorithmManager(nullptr)
	  , m_pConfigurationManager(nullptr)
	  , m_pKernelObjectFactory(nullptr)
	  , m_pPlayerManager(nullptr)
	  , m_pPluginManager(nullptr)
	  , m_pMetaboxManager(nullptr)
	  , m_pScenarioManager(nullptr)
	  , m_pTypeManager(nullptr)
	  , m_pLogManager(nullptr)
	  , m_pErrorManager(nullptr)
	  , m_sApplicationName(rApplicationName)
	  , m_sConfigurationFile(rConfigurationFile)
	  , m_pLogListenerConsole(nullptr)
	  , m_pLogListenerFile(nullptr) {}

CKernelContext::~CKernelContext()
{
	this->uninitialize();
}

bool CKernelContext::initialize(const char* const* tokenList, size_t tokenCount)
{
	std::map<std::string, std::string> initializationTokens;
	auto token = tokenList;
	while (token && tokenCount > 0)
	{
		auto key   = tokenList++;
		auto value = tokenList++;
		tokenCount--;
		initializationTokens[*key] = *value;
	}

	m_pErrorManager.reset(new CErrorManager(m_rMasterKernelContext));

	m_pKernelObjectFactory.reset(new CKernelObjectFactory(m_rMasterKernelContext));

	m_pLogManager.reset(new CLogManager(m_rMasterKernelContext));
	m_pLogManager->activate(true);

	m_pConfigurationManager.reset(new CConfigurationManager(m_rMasterKernelContext));
	// We create the configuration manager very soon to be able to deactivate the console log listener
	if (initializationTokens.count("Kernel_SilentConsole"))
	{
		m_pConfigurationManager->createConfigurationToken("Kernel_SilentConsole", initializationTokens.at("Kernel_SilentConsole").c_str());
	}

	if (!m_pConfigurationManager->expandAsBoolean("${Kernel_SilentConsole}", false))
	{
		m_pLogListenerConsole.reset(new CLogListenerConsole(m_rMasterKernelContext, m_sApplicationName));
		m_pLogListenerConsole->activate(false);
		m_pLogListenerConsole->activate(LogLevel_Info, LogLevel_Last, true);
		this->getLogManager().addListener(m_pLogListenerConsole.get());
	}


	m_pConfigurationManager->createConfigurationToken("ApplicationName", m_sApplicationName);
	m_pConfigurationManager->createConfigurationToken("Path_UserData", Directories::getUserDataDir());
	m_pConfigurationManager->createConfigurationToken("Path_Log", Directories::getLogDir());
	m_pConfigurationManager->createConfigurationToken("Path_Tmp", "${Path_UserData}/tmp");
	m_pConfigurationManager->createConfigurationToken("Path_Lib", Directories::getLibDir());
	m_pConfigurationManager->createConfigurationToken("Path_Bin", Directories::getBinDir());
	m_pConfigurationManager->createConfigurationToken("Path_Data", Directories::getDataDir());

#if defined TARGET_OS_Windows
	m_pConfigurationManager->createConfigurationToken("OperatingSystem", "Windows");
#elif defined TARGET_OS_Linux
	m_pConfigurationManager->createConfigurationToken("OperatingSystem", "Linux");
#elif defined TARGET_OS_MacOS
	m_pConfigurationManager->createConfigurationToken("OperatingSystem", "MacOS");
#else
	m_pConfigurationManager->createConfigurationToken("OperatingSystem", "Unknown");
#endif

	m_pConfigurationManager->createConfigurationToken("Kernel_PluginsPatternMacOS", "libopenvibe-plugins-*.dylib");
	m_pConfigurationManager->createConfigurationToken("Kernel_PluginsPatternLinux", "libopenvibe-plugins-*.so");
	m_pConfigurationManager->createConfigurationToken("Kernel_PluginsPatternWindows", "openvibe-plugins-*.dll");
	m_pConfigurationManager->createConfigurationToken("Kernel_Plugins", "${Path_Lib}/${Kernel_PluginsPattern${OperatingSystem}}");

	m_pConfigurationManager->createConfigurationToken("Kernel_Metabox", "${Path_Data}/metaboxes/;${Path_UserData}/metaboxes/");

	m_pConfigurationManager->createConfigurationToken("Kernel_MainLogLevel", "Debug");
	m_pConfigurationManager->createConfigurationToken("Kernel_ConsoleLogLevel", "Information");
	m_pConfigurationManager->createConfigurationToken("Kernel_FileLogLevel", "Debug");
	m_pConfigurationManager->createConfigurationToken("Kernel_PlayerFrequency", "128");
	// Add this two tokens to be used to know what documentation should be loaded
	m_pConfigurationManager->createConfigurationToken("Brand_Name", BRAND_NAME);
	m_pConfigurationManager->createConfigurationToken("Application_Name", OV_PROJECT_NAME);
	m_pConfigurationManager->createConfigurationToken("Application_Version", OV_VERSION_MAJOR "." OV_VERSION_MINOR "." OV_VERSION_PATCH);

	for (auto& token : initializationTokens)
	{
		m_pConfigurationManager->addOrReplaceConfigurationToken(token.first.c_str(), token.second.c_str());
	}

	this->getLogManager() << LogLevel_Info << "Adding kernel configuration file [" << m_sConfigurationFile << "]\n";

	OV_ERROR_UNLESS_KRF(m_pConfigurationManager->addConfigurationFromFile(m_sConfigurationFile),
						"Problem parsing config file [" << m_sConfigurationFile << "]", ErrorType::Internal);

	CString l_sPathTmp = m_pConfigurationManager->expand("${Path_UserData}");
	FS::Files::createPath(l_sPathTmp.toASCIIString());
	l_sPathTmp = m_pConfigurationManager->expand("${Path_Tmp}");
	FS::Files::createPath(l_sPathTmp.toASCIIString());
	l_sPathTmp = m_pConfigurationManager->expand("${Path_Log}");
	FS::Files::createPath(l_sPathTmp);
	CString l_sLogFile = l_sPathTmp + "/openvibe-" + m_sApplicationName + ".log";

	// We do this here to allow user to set the Path_Log in the .conf. The downside is that earlier log messages will not appear in the file log.
	m_pLogListenerFile.reset(new CLogListenerFile(m_rMasterKernelContext, m_sApplicationName, l_sLogFile));
	m_pLogListenerFile->activate(true);
	this->getLogManager().addListener(m_pLogListenerFile.get());

	ELogLevel l_eMainLogLevel    = this->earlyGetLogLevel(m_pConfigurationManager->expand("${Kernel_MainLogLevel}"));
	ELogLevel l_eConsoleLogLevel = this->earlyGetLogLevel(m_pConfigurationManager->expand("${Kernel_ConsoleLogLevel}"));
	ELogLevel l_eFileLogLevel    = this->earlyGetLogLevel(m_pConfigurationManager->expand("${Kernel_FileLogLevel}"));

	m_pLogManager->activate(false);
	m_pLogManager->activate(l_eMainLogLevel, LogLevel_Last, true);
	m_pLogListenerFile->activate(false);
	m_pLogListenerFile->activate(l_eFileLogLevel, LogLevel_Last, true);
	m_pLogListenerFile->configure(*m_pConfigurationManager);
	m_pLogListenerFile->configure(*m_pConfigurationManager);

	if (m_pLogListenerConsole.get())
	{
		m_pLogListenerConsole->activate(false);
		m_pLogListenerConsole->activate(l_eConsoleLogLevel, LogLevel_Last, true);
		m_pLogListenerConsole->configure(*m_pConfigurationManager);
	}

	m_pAlgorithmManager.reset(new CAlgorithmManager(m_rMasterKernelContext));

	m_pPlayerManager.reset(new CPlayerManager(m_rMasterKernelContext));

	m_pTypeManager.reset(new CTypeManager(m_rMasterKernelContext));

	m_pTypeManager->registerType(OV_TypeId_Boolean, "Boolean");
	m_pTypeManager->registerType(OV_TypeId_Integer, "Integer");
	m_pTypeManager->registerType(OV_TypeId_Float, "Float");
	m_pTypeManager->registerType(OV_TypeId_String, "String");
	m_pTypeManager->registerType(OV_TypeId_Filename, "Filename");
	m_pTypeManager->registerType(OV_TypeId_Script, "Script");

	m_pTypeManager->registerEnumerationType(OV_TypeId_Stimulation, "Stimulation");

	m_pTypeManager->registerEnumerationType(OV_TypeId_MeasurementUnit, "Measurement unit");
	m_pTypeManager->registerEnumerationType(OV_TypeId_Factor, "Factor");

	m_pTypeManager->registerEnumerationType(OV_TypeId_LogLevel, "Log level");
	m_pTypeManager->registerEnumerationEntry(OV_TypeId_LogLevel, "None", LogLevel_None);
	m_pTypeManager->registerEnumerationEntry(OV_TypeId_LogLevel, "Debug", LogLevel_Debug);
	m_pTypeManager->registerEnumerationEntry(OV_TypeId_LogLevel, "Benchmarking / Profiling", LogLevel_Benchmark);
	m_pTypeManager->registerEnumerationEntry(OV_TypeId_LogLevel, "Trace", LogLevel_Trace);
	m_pTypeManager->registerEnumerationEntry(OV_TypeId_LogLevel, "Information", LogLevel_Info);
	m_pTypeManager->registerEnumerationEntry(OV_TypeId_LogLevel, "Warning", LogLevel_Warning);
	m_pTypeManager->registerEnumerationEntry(OV_TypeId_LogLevel, "Important warning", LogLevel_ImportantWarning);
	m_pTypeManager->registerEnumerationEntry(OV_TypeId_LogLevel, "Error", LogLevel_Error);
	m_pTypeManager->registerEnumerationEntry(OV_TypeId_LogLevel, "Fatal error", LogLevel_Fatal);

	m_pTypeManager->registerStreamType(OV_TypeId_EBMLStream, "EBML stream", OV_UndefinedIdentifier);
	m_pTypeManager->registerStreamType(OV_TypeId_ExperimentInformation, "Experiment information", OV_TypeId_EBMLStream);
	m_pTypeManager->registerStreamType(OV_TypeId_Stimulations, "Stimulations", OV_TypeId_EBMLStream);
	m_pTypeManager->registerStreamType(OV_TypeId_StreamedMatrix, "Streamed matrix", OV_TypeId_EBMLStream);
	m_pTypeManager->registerStreamType(OV_TypeId_ChannelLocalisation, "Channel localisation", OV_TypeId_StreamedMatrix);
	m_pTypeManager->registerStreamType(OV_TypeId_ChannelUnits, "Channel units", OV_TypeId_StreamedMatrix);
	m_pTypeManager->registerStreamType(OV_TypeId_FeatureVector, "Feature vector", OV_TypeId_StreamedMatrix);
	m_pTypeManager->registerStreamType(OV_TypeId_Signal, "Signal", OV_TypeId_StreamedMatrix);
	m_pTypeManager->registerStreamType(OV_TypeId_Spectrum, "Spectrum", OV_TypeId_StreamedMatrix);
	m_pTypeManager->registerStreamType(OV_TypeId_TimeFrequency, "Time-frequency", OV_TypeId_StreamedMatrix);
	m_pTypeManager->registerStreamType(OV_TypeId_CovarianceMatrix, "Covariance Matrix", OV_TypeId_StreamedMatrix);


	m_pTypeManager->registerType(OV_TypeId_Message, "Message");

	m_pScenarioManager.reset(new CScenarioManager(m_rMasterKernelContext));

	m_pPluginManager.reset(new CPluginManager(m_rMasterKernelContext));

	m_pMetaboxManager.reset(new CMetaboxManager(m_rMasterKernelContext));

	return true;
}

bool CKernelContext::uninitialize()
{
	// As releaseScenario() can call into Plugin Manager we have to clear the scenario manager
	// before destroying the Plugin Manager. We can not destroy the Scenario Manager first
	// before Plugin Manager destructor needs it.
	CIdentifier scenarioIdentifier;
	while ((scenarioIdentifier = m_pScenarioManager->getNextScenarioIdentifier(OV_UndefinedIdentifier)) != OV_UndefinedIdentifier)
	{
		m_pScenarioManager->releaseScenario(scenarioIdentifier);
	}

	CIdentifier algorithmIdentifier;
	while ((algorithmIdentifier = m_pAlgorithmManager->getNextAlgorithmIdentifier(OV_UndefinedIdentifier)) != OV_UndefinedIdentifier)
	{
		m_pAlgorithmManager->releaseAlgorithm(algorithmIdentifier);
	}

	m_pPluginManager.reset();
	m_pMetaboxManager.reset();
	m_pScenarioManager.reset();
	m_pTypeManager.reset();
	m_pPlayerManager.reset();
	m_pAlgorithmManager.reset();
	m_pConfigurationManager.reset();

	this->getLogManager().removeListener(m_pLogListenerConsole.get());
	this->getLogManager().removeListener(m_pLogListenerFile.get());

	m_pLogManager.reset();
	m_pLogListenerConsole.reset();
	m_pLogListenerFile.reset();

	m_pKernelObjectFactory.reset();

	m_pErrorManager.reset();

	return true;
}

IAlgorithmManager& CKernelContext::getAlgorithmManager() const
{
	assert(m_pAlgorithmManager);

	return *m_pAlgorithmManager;
}

IConfigurationManager& CKernelContext::getConfigurationManager() const
{
	assert(m_pConfigurationManager);

	return *m_pConfigurationManager;
}

IKernelObjectFactory& CKernelContext::getKernelObjectFactory() const
{
	assert(m_pKernelObjectFactory);

	return *m_pKernelObjectFactory;
}

IPlayerManager& CKernelContext::getPlayerManager() const
{
	assert(m_pPlayerManager);

	return *m_pPlayerManager;
}

IPluginManager& CKernelContext::getPluginManager() const
{
	assert(m_pPluginManager);

	return *m_pPluginManager;
}

IMetaboxManager& CKernelContext::getMetaboxManager() const
{
	assert(m_pMetaboxManager);

	return *m_pMetaboxManager;
}


IScenarioManager& CKernelContext::getScenarioManager() const
{
	assert(m_pScenarioManager);

	return *m_pScenarioManager;
}

ITypeManager& CKernelContext::getTypeManager() const
{
	assert(m_pTypeManager);

	return *m_pTypeManager;
}

ILogManager& CKernelContext::getLogManager() const
{
	assert(m_pLogManager);

	return *m_pLogManager;
}

IErrorManager& CKernelContext::getErrorManager() const
{
	assert(m_pErrorManager);

	return *m_pErrorManager;
}

ELogLevel CKernelContext::earlyGetLogLevel(const CString& rLogLevelName)
{
	assert(m_pLogManager);

	std::string l_sValue(rLogLevelName.toASCIIString());
	std::transform(l_sValue.begin(), l_sValue.end(), l_sValue.begin(), [](char c)
	{
		return static_cast<char>(std::tolower(c));
	});

	if (l_sValue == "none") { return LogLevel_None; }
	if (l_sValue == "debug") { return LogLevel_Debug; }
	if (l_sValue == "benchmarking / profiling") { return LogLevel_Benchmark; }
	if (l_sValue == "trace") { return LogLevel_Trace; }
	if (l_sValue == "information") { return LogLevel_Info; }
	if (l_sValue == "warning") { return LogLevel_Warning; }
	if (l_sValue == "important warning") { return LogLevel_ImportantWarning; }
	if (l_sValue == "error") { return LogLevel_Error; }
	if (l_sValue == "fatal error") { return LogLevel_Fatal; }

	OV_WARNING("Invalid log level " << rLogLevelName << " specified in configuration file, falling back to " << CString("Debug"), (*m_pLogManager));

	return LogLevel_Debug;
}
