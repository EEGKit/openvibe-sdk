#include "ovkCKernelContext.h"
#include "ovkCKernelObjectFactory.h"
#include "ovkCTypeManager.h"

#include "algorithm/ovkCAlgorithmManager.h"
#include "configuration/ovkCConfigurationManager.h"
#include "player/ovkCPlayerManager.h"
#include "plugins/ovkCPluginManager.h"
#include "scenario/ovkCScenarioManager.h"
#include "log/ovkCLogManager.h"
#include "log/ovkCLogListenerConsole.h"
#include "log/ovkCLogListenerFile.h"

#include <cassert>
#include <string>
#include <algorithm>
#include <functional>
#include <cctype>

#include <fs/Files.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;

namespace
{
	// because std::tolower has multiple signatures,
	// it can not be easily used in std::transform
	// this workaround is taken from http://www.gcek.net/ref/books/sw/cpp/ticppv2/
	template <class charT>
	charT to_lower(charT c)
	{
		return std::tolower(c);
	}

};


CKernelContext::CKernelContext(const IKernelContext* pMasterKernelContext, const CString& rApplicationName, const CString& rConfigurationFile)
	:m_rMasterKernelContext(pMasterKernelContext?*pMasterKernelContext:*this)
	,m_pAlgorithmManager(nullptr)
	,m_pConfigurationManager(nullptr)
	,m_pKernelObjectFactory(nullptr)
	,m_pPlayerManager(nullptr)
	,m_pPluginManager(nullptr)
	,m_pScenarioManager(nullptr)
	,m_pTypeManager(nullptr)
	,m_pLogManager(nullptr)
	,m_sApplicationName(rApplicationName)
	,m_sConfigurationFile(rConfigurationFile)
	,m_pLogListenerConsole(nullptr)
	,m_pLogListenerFile(nullptr)
{
}

CKernelContext::~CKernelContext(void)
{
	this->uninitialize();
}

boolean CKernelContext::initialize(void)
{
	m_pKernelObjectFactory.reset(new CKernelObjectFactory(m_rMasterKernelContext));

	m_pLogManager.reset(new CLogManager(m_rMasterKernelContext));
	m_pLogManager->activate(true);

	m_pLogListenerConsole.reset(new CLogListenerConsole(m_rMasterKernelContext, m_sApplicationName));
	m_pLogListenerConsole->activate(false);
	m_pLogListenerConsole->activate(LogLevel_Info, LogLevel_Last, true);
	this->getLogManager().addListener(m_pLogListenerConsole.get());

	m_pConfigurationManager.reset(new CConfigurationManager(m_rMasterKernelContext));

	m_pConfigurationManager->createConfigurationToken("ApplicationName", m_sApplicationName);
	m_pConfigurationManager->createConfigurationToken("Path_UserData", OpenViBE::Directories::getUserDataDir());
	m_pConfigurationManager->createConfigurationToken("Path_Log", OpenViBE::Directories::getLogDir());
	m_pConfigurationManager->createConfigurationToken("Path_Tmp", "${Path_UserData}/tmp");
	m_pConfigurationManager->createConfigurationToken("Path_Lib", OpenViBE::Directories::getLibDir());
	m_pConfigurationManager->createConfigurationToken("Path_Bin", OpenViBE::Directories::getBinDir());
	
#if defined TARGET_OS_Windows
	m_pConfigurationManager->createConfigurationToken("OperatingSystem", "Windows");
#elif defined TARGET_OS_Linux
	m_pConfigurationManager->createConfigurationToken("OperatingSystem", "Linux");
#elif defined TARGET_OS_MacOS
	m_pConfigurationManager->createConfigurationToken("OperatingSystem", "MacOS");
#else
	m_pConfigurationManager->createConfigurationToken("OperatingSystem", "Unknown");
#endif

	m_pConfigurationManager->createConfigurationToken("Kernel_PluginsPatternMacOS",   "libopenvibe-plugins-*.dylib");
	m_pConfigurationManager->createConfigurationToken("Kernel_PluginsPatternLinux",   "libopenvibe-plugins-*.so");
	m_pConfigurationManager->createConfigurationToken("Kernel_PluginsPatternWindows", "openvibe-plugins-*.dll");
	m_pConfigurationManager->createConfigurationToken("Kernel_Plugins", "${Path_Lib}/${Kernel_PluginsPattern${OperatingSystem}}");

	m_pConfigurationManager->createConfigurationToken("Kernel_MainLogLevel", "Debug");
	m_pConfigurationManager->createConfigurationToken("Kernel_ConsoleLogLevel", "Information");
	m_pConfigurationManager->createConfigurationToken("Kernel_FileLogLevel", "Debug");
	m_pConfigurationManager->createConfigurationToken("Kernel_PlayerFrequency", "128");
	// Add this two tokens to be used to know what documentation should be loaded
	m_pConfigurationManager->createConfigurationToken("Brand_Name", BRAND_NAME);

	this->getLogManager() << LogLevel_Info << "Adding kernel configuration file [" << m_sConfigurationFile << "]\n";
	if(!m_pConfigurationManager->addConfigurationFromFile(m_sConfigurationFile)) {
		this->getLogManager() << LogLevel_Error << "Problem parsing config file [" << m_sConfigurationFile << "]. This will not work.\n";
		// Since OpenViBE components usually don't react to return value of initialize(), we just quit here
		this->getLogManager() << LogLevel_Error << "Forcing an exit.\n";
		exit(-1);
	}

	// Generate the openvibe directories that the applications may write to. These are done after addConfigurationFromFile(), in case the defaults have been modified.
	// @FIXME note that there is an issue if these paths are changed by a delayed configuration, then the directories are not created unless the caller does it.
	CString l_sPathTmp;
	l_sPathTmp = m_pConfigurationManager->expand("${Path_UserData}");
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

	ELogLevel l_eMainLogLevel   =this->earlyGetLogLevel(m_pConfigurationManager->expand("${Kernel_MainLogLevel}"));
	ELogLevel l_eConsoleLogLevel=this->earlyGetLogLevel(m_pConfigurationManager->expand("${Kernel_ConsoleLogLevel}"));
	ELogLevel l_eFileLogLevel   =this->earlyGetLogLevel(m_pConfigurationManager->expand("${Kernel_FileLogLevel}"));

	m_pLogManager->activate(false);
	m_pLogManager->activate(l_eMainLogLevel, LogLevel_Last, true);
	m_pLogListenerFile->activate(false);
	m_pLogListenerFile->activate(l_eFileLogLevel, LogLevel_Last, true);
	m_pLogListenerFile->configure(*m_pConfigurationManager);
	m_pLogListenerConsole->activate(false);
	m_pLogListenerConsole->activate(l_eConsoleLogLevel, LogLevel_Last, true);

	m_pLogListenerFile->configure(*m_pConfigurationManager);
	m_pLogListenerConsole->configure(*m_pConfigurationManager);

	m_pAlgorithmManager.reset(new CAlgorithmManager(m_rMasterKernelContext));

	m_pPlayerManager.reset(new CPlayerManager(m_rMasterKernelContext));

	m_pTypeManager.reset(new CTypeManager(m_rMasterKernelContext));

	m_pTypeManager->registerType(OV_TypeId_Boolean, "Boolean");
	m_pTypeManager->registerType(OV_TypeId_Integer, "Integer");
	m_pTypeManager->registerType(OV_TypeId_Float, "Float");
	m_pTypeManager->registerType(OV_TypeId_String, "String");
	m_pTypeManager->registerType(OV_TypeId_Filename, "Filename");
	m_pTypeManager->registerType(OV_TypeId_Script, "Script");
	m_pTypeManager->registerType(OV_TypeId_Color, "Color");
	m_pTypeManager->registerType(OV_TypeId_ColorGradient, "Color Gradient");

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

	m_pTypeManager->registerType(OV_TypeId_Message,  "Message");

	m_pScenarioManager.reset(new CScenarioManager(m_rMasterKernelContext));

	m_pPluginManager.reset(new CPluginManager(m_rMasterKernelContext));

	return true;
}

bool CKernelContext::uninitialize(void)
{
	m_pPluginManager.reset();
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

	return true;
}

IAlgorithmManager& CKernelContext::getAlgorithmManager(void) const
{
	assert(m_pAlgorithmManager);
	
	return *m_pAlgorithmManager;
}

IConfigurationManager& CKernelContext::getConfigurationManager(void) const
{
	assert(m_pConfigurationManager);
	
	return *m_pConfigurationManager;
}

IKernelObjectFactory& CKernelContext::getKernelObjectFactory(void) const
{
	assert(m_pKernelObjectFactory);
	
	return *m_pKernelObjectFactory;
}

IPlayerManager& CKernelContext::getPlayerManager(void) const
{
	assert(m_pPlayerManager);
	
	return *m_pPlayerManager;
}

IPluginManager& CKernelContext::getPluginManager(void) const
{
	assert(m_pPluginManager);
	
	return *m_pPluginManager;
}

IScenarioManager& CKernelContext::getScenarioManager(void) const
{
	assert(m_pScenarioManager);
	
	return *m_pScenarioManager;
}

ITypeManager& CKernelContext::getTypeManager(void) const
{
	assert(m_pTypeManager);
	
	return *m_pTypeManager;
}

ILogManager& CKernelContext::getLogManager(void) const
{
	assert(m_pLogManager);
	
	return *m_pLogManager;
}

ELogLevel CKernelContext::earlyGetLogLevel(const CString& rLogLevelName)
{
	assert(m_pLogManager);
	
	std::string l_sValue(rLogLevelName.toASCIIString());
	std::transform(l_sValue.begin(), l_sValue.end(), l_sValue.begin(), ::to_lower<std::string::value_type>);

	if(l_sValue=="none")                     return LogLevel_None;
	if(l_sValue=="debug")                    return LogLevel_Debug;
	if(l_sValue=="benchmarking / profiling") return LogLevel_Benchmark;
	if(l_sValue=="trace")                    return LogLevel_Trace;
	if(l_sValue=="information")              return LogLevel_Info;
	if(l_sValue=="warning")                  return LogLevel_Warning;
	if(l_sValue=="important warning")        return LogLevel_ImportantWarning;
	if(l_sValue=="error")                    return LogLevel_Error;
	if(l_sValue=="fatal error")              return LogLevel_Fatal;

	(*m_pLogManager) << LogLevel_Warning << "Invalid log level " << rLogLevelName << " specified in configuration file, falling back to " << CString("Debug") << "\n";

	return LogLevel_Debug;
}
