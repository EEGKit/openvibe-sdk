#include "ovpCBoxAlgorithmExternalProcessing.h"
#include <fs/Files.h>

#include <chrono>
#include <cstdlib>
#include <stdlib.h>
#include <random>
#include <thread>
#include <algorithm>

#ifdef TARGET_OS_Windows
#include <process.h>
#include <Windows.h>
#include <ctime>
#else
#include <spawn.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
extern char **environ;
#endif

#include <system/ovCTime.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Tools;

uint64_t CBoxAlgorithmExternalProcessing::getClockFrequency()
{
	if (m_IsGenerator)
	{
		// We slow down the generator type boxes by default, in order to limit syncing
		// In fast forward we limit the syncing even more by setting the frequency to 1Hz
		if (this->getPlayerContext().getStatus() == PlayerStatus_Forward) { return 1LL << 32; }
		return 16LL << 32;
	}
	return 128LL << 32;
}

bool CBoxAlgorithmExternalProcessing::initialize()
{
	m_Port = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);

	// Check that the port is not in the system range
	OV_ERROR_UNLESS_KRF((m_Port >= 49152 && m_Port <= 65535) || (m_Port == 0),
						"Port [" << m_Port << "] is invalid. It must be either 0 or a number in the range 49152-65535.", ErrorType::BadConfig);

	// Settings
	const IBox* staticBoxContext = this->getBoxAlgorithmContext()->getStaticBoxContext();

	for (uint32_t i = 8; i < staticBoxContext->getSettingCount(); ++i)
	{
		CString name;
		staticBoxContext->getSettingName(i, name);

		CIdentifier type;
		staticBoxContext->getSettingType(i, type);

		const CString value = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), i);

		OV_FATAL_UNLESS_K(m_Messaging.addParameter(i, type.toUInteger(), name.toASCIIString(), value.toASCIIString()),
						  "Failed to add a parameter: " << i, ErrorType::Internal);
	}

	// Inputs
	for (uint32_t i = 0; i < staticBoxContext->getInputCount(); ++i)
	{
		CIdentifier type;
		staticBoxContext->getInputType(i, type);

		if (type == OV_TypeId_Stimulations) { m_StimulationDecoders[i].initialize(*this, i); }

		CString name;
		staticBoxContext->getInputName(i, name);

		OV_FATAL_UNLESS_K(m_Messaging.addInput(i, type.toUInteger(), name.toASCIIString()),
						  "Failed to add an input: " << i, ErrorType::Internal);
	}

	// Outputs
	for (uint32_t i = 0; i < staticBoxContext->getOutputCount(); ++i)
	{
		CIdentifier type;
		staticBoxContext->getOutputType(i, type);

		CString name;
		staticBoxContext->getOutputName(i, name);

		if (!m_Messaging.addOutput(i, type.toUInteger(), name.toASCIIString()))
		{
			this->getLogManager() << LogLevel_Error << "Failed to add an output: " << i << "\n";
			return false;
		}
	}

	const bool mustGenerateConnectionID = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 4);

	if (mustGenerateConnectionID) { m_ConnectionID = generateConnectionID(32); }
	else
	{
		const CString connectionIDSetting = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 5);
		m_ConnectionID                    = connectionIDSetting.toASCIIString();
	}

	OV_ERROR_UNLESS_KRF(m_Messaging.listen(m_Port),
						"Could not listen to TCP port: " << m_Port << ". This port may be already used by another service. Please try another one.",
						ErrorType::BadNetworkConnection);

	if (m_Port == 0)
	{
		m_Messaging.getSocketPort(m_Port);
		this->getLogManager() << LogLevel_Info << "Box is now listening on TCP port [" << m_Port << "].\n";
	}

	m_ShouldLaunchProgram = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);

	if (m_ShouldLaunchProgram)
	{
		const CString programPath = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
		const CString arguments   = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);

		if (!launchThirdPartyProgram(programPath.toASCIIString(), arguments.toASCIIString()))
		{
			// Error message in the function
			return false;
		}

		this->getLogManager() << LogLevel_Info << "Third party program [" << programPath.toASCIIString() << "] started.\n";
	}

	const auto startTime = System::Time::zgetTime();

	bool clientConnected    = false;
	m_HasReceivedEndMessage = false;

	m_AcceptTimeout = TimeArithmetics::secondsToTime(double(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 6)));
	m_IsGenerator   = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 7);

	while (System::Time::zgetTime() - startTime < m_AcceptTimeout)
	{
		if (m_Messaging.accept())
		{
			clientConnected = true;
			this->getLogManager() << LogLevel_Info << "Client connected to the server.\n";
			break;
		}
		
		const Communication::MessagingServer::ELibraryError error = m_Messaging.getLastError();
		if (error == Communication::MessagingServer::ELibraryError::BadAuthenticationReceived)
		{
			OV_WARNING_K("A client sent a bad authentication.");
			break;
		}
		if (error == Communication::MessagingServer::ELibraryError::NoAuthenticationReceived) { OV_WARNING_K("The client has not sent authentication."); }
	}

	OV_ERROR_UNLESS_KRF(clientConnected, "No client connected before the timeout.", ErrorType::Internal);


	// Now synchronize once with the client so it can perform its initialize before
	// the processing starts
	m_Messaging.pushSync();


	while (m_Messaging.isConnected() && !m_Messaging.waitForSyncMessage()) { if (!m_Messaging.waitForSyncMessage()) { std::this_thread::sleep_for(std::chrono::milliseconds(1)); } }

	m_SyncTimeout  = TimeArithmetics::secondsToTime(0.0625);
	m_LastSyncTime = System::Time::zgetTime();
	return true;
}

bool CBoxAlgorithmExternalProcessing::uninitialize()
{
	for (auto& decoder : m_StimulationDecoders) { decoder.second.uninitialize(); }

	if (!m_HasReceivedEndMessage)
	{
		const bool result = m_Messaging.close();

#ifdef TARGET_OS_Windows
		if (m_ShouldLaunchProgram && m_ThirdPartyProgramProcessId > 0)
		{
			DWORD exitCode;

			// Wait for external process to stop by himself, terminate it if necessary
			const auto startTime = System::Time::zgetTime();
			while (System::Time::zgetTime() - startTime < m_AcceptTimeout)
			{
				GetExitCodeProcess(HANDLE(m_ThirdPartyProgramProcessId), &exitCode);

				if (exitCode != STILL_ACTIVE) { break; }
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}


			if (exitCode == STILL_ACTIVE) { OV_ERROR_UNLESS_KRF(::TerminateProcess(HANDLE(m_ThirdPartyProgramProcessId), EXIT_FAILURE), "Failed to kill third party program.", ErrorType::Unknown); }
			else if (exitCode != 0) { OV_WARNING_K("Third party program [" << m_ThirdPartyProgramProcessId << "] has terminated with exit code [" << int(exitCode) << "]"); }
		}
#else
		if (m_ShouldLaunchProgram && m_ThirdPartyProgramProcessId != 0)
		{
			int status;
			pid_t pid = waitpid(m_ThirdPartyProgramProcessId, &status, WNOHANG);
			
			// Wait for external process to stop by himself, terminate it after 10s
			auto startTime = System::Time::zgetTime();
			while (pid == 0 && System::Time::zgetTime() - startTime < m_AcceptTimeout)
			{
				// Check if the program has hung itself
				pid = waitpid(m_ThirdPartyProgramProcessId, &status, WNOHANG);
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			}

			if (pid != 0)
			{
				if (WIFEXITED(status))
				{
					OV_WARNING_K("Third party program [" << m_ThirdPartyProgramProcessId << "] has terminated with exit code [" << WEXITSTATUS(status) << "]");
				}
				else if (WIFSIGNALED(status))
				{
					OV_WARNING_K("Third party program [" << m_ThirdPartyProgramProcessId << "] killed by signal [" << WTERMSIG(status) << "]");
				}
			}
			else
			{
				kill(m_ThirdPartyProgramProcessId, SIGTERM);
				waitpid(m_ThirdPartyProgramProcessId, &status, 0);
				this->getLogManager() << LogLevel_Info << "Third party program [" << m_ThirdPartyProgramProcessId << "] exited with status [" << WEXITSTATUS(status) << "]\n";
			}
		}
#endif
		return result;
	}

	return true;
}

bool CBoxAlgorithmExternalProcessing::processClock(CMessageClock& /*messageClock*/) { return this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess(); }

bool CBoxAlgorithmExternalProcessing::processInput(const uint32_t /*index*/)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmExternalProcessing::process()
{
	if (m_Messaging.isInErrorState())
	{
		const std::string errorString = Communication::MessagingServer::getErrorString(m_Messaging.getLastError());
		OV_ERROR_KRF("Error state connection: " << errorString.c_str() << ".\n This may be due to a broken client connection.",
					 ErrorType::BadNetworkConnection);
	}

	if (m_HasReceivedEndMessage == false && m_Messaging.isEndReceived() == true)
	{
		this->getLogManager() << LogLevel_Info << "The third party program has ended the communication.\n";
		m_Messaging.close();
		m_HasReceivedEndMessage = true;
	}

	OV_ERROR_UNLESS_KRF(m_Messaging.pushTime(this->getPlayerContext().getCurrentTime())
						, "Failed to push Time.", ErrorType::BadNetworkConnection);

	const IBox* staticBoxContext = getBoxAlgorithmContext()->getStaticBoxContext();
	IBoxIO* dynamicBoxContext    = getBoxAlgorithmContext()->getDynamicBoxContext();

	// Send input EBML to the client

	bool hasSentDataToClient = false;

	for (uint32_t i = 0; i < staticBoxContext->getInputCount(); ++i)
	{
		auto maybeStimulationDecoder = m_StimulationDecoders.find(i);
		for (uint32_t j = 0; j < dynamicBoxContext->getInputChunkCount(i); j++)
		{
			if (!m_HasReceivedEndMessage)
			{
				uint64_t startTime         = 0;
				uint64_t endTime           = 0;
				uint64_t chunkSize         = 0;
				const uint8_t* chunkBuffer = nullptr;

				OV_FATAL_UNLESS_K(dynamicBoxContext->getInputChunk(i, j, startTime, endTime, chunkSize, chunkBuffer),
								  "Failed to get input chunk [" << i << "][" << j << "]", ErrorType::Internal);

				std::shared_ptr<std::vector<uint8_t>> ebml(new std::vector<uint8_t>(chunkBuffer, chunkBuffer + chunkSize));

				// We only encode stimulation stream chunks if they contain stimulations
				if (maybeStimulationDecoder != m_StimulationDecoders.end())
				{
					maybeStimulationDecoder->second.decode(j, false); // The input will be marked as deprecated later

					// Cache empty chunks, we will send them when a stimulation or a signal chunk arrives
					if (maybeStimulationDecoder->second.getOutputStimulationSet()->getStimulationCount() == 0)
					{
						m_PacketHistory.emplace(startTime, endTime, i, ebml);
						OV_FATAL_UNLESS_K(dynamicBoxContext->markInputAsDeprecated(i, j), "Failed to mark input as deprecated", ErrorType::Internal);
						break;
					}
				}

				// Empty the history before to send useful data
				while (!m_PacketHistory.empty())
				{
					OV_ERROR_UNLESS_KRF(m_Messaging.pushEBML(m_PacketHistory.front().index,
											m_PacketHistory.front().startTime,
											m_PacketHistory.front().endTime,
											m_PacketHistory.front().EBML),
										"Failed to push EBML.", ErrorType::BadNetworkConnection);

					m_PacketHistory.pop();
				}

				// Push the last EBML
				OV_ERROR_UNLESS_KRF(m_Messaging.pushEBML(i, startTime, endTime, ebml), "Failed to push EBML.", ErrorType::BadNetworkConnection);
				hasSentDataToClient = true;
			}

			OV_FATAL_UNLESS_K(dynamicBoxContext->markInputAsDeprecated(i, j), "Failed to mark input as deprecated", ErrorType::Internal);
		}
	}

	if (hasSentDataToClient || m_IsGenerator || System::Time::zgetTime() - m_LastSyncTime > m_SyncTimeout)
	{
		m_LastSyncTime = System::Time::zgetTime();
		// Here, we send a sync message to tell to the client that we have no more data to send.
		// Generators do not have input data, so the box never has to send data to the external program
		// and thus needs to perform syncing on each tick.

		OV_ERROR_UNLESS_KRF(m_Messaging.pushSync(), "Failed to push sync message.", ErrorType::BadNetworkConnection);

		if (!m_HasReceivedEndMessage)
		{
			this->log();

			uint64_t packetId;
			uint32_t index;
			uint64_t startTime;
			uint64_t endTime;
			std::shared_ptr<const std::vector<uint8_t>> ebml;

			bool receivedSync = false;
			while (!receivedSync && !m_HasReceivedEndMessage && !m_Messaging.isInErrorState() && m_Messaging.isConnected())
			{
				receivedSync = m_Messaging.waitForSyncMessage();
				while (m_Messaging.popEBML(packetId, index, startTime, endTime, ebml))
				{
					OV_ERROR_UNLESS_KRF(dynamicBoxContext->appendOutputChunkData(index, ebml->data(), ebml->size()),
										"Failed to append output chunk data.", ErrorType::Internal);

					OV_ERROR_UNLESS_KRF(dynamicBoxContext->markOutputAsReadyToSend(index, startTime, endTime),
										"Failed to mark output as ready to send.", ErrorType::Internal);
				}
				if (!receivedSync) { std::this_thread::sleep_for(std::chrono::milliseconds(1)); }
			}
		}
	}

	return true;
}

std::string CBoxAlgorithmExternalProcessing::generateConnectionID(const size_t size)
{
	std::default_random_engine generator{ std::random_device()() };
	std::uniform_int_distribution<int> character(0, 34);

	std::string connectionID;

	for (size_t i = 0; i < size; ++i)
	{
		const char c = char(character(generator));
		connectionID.push_back((c < 26) ? ('A' + c) : '1' + (c - 26));
	}

	return connectionID;
}

static std::vector<std::string> splitCommandLine(const std::string& cmdLine)
{
	std::vector<std::string> list;
	std::string arg;
	bool escape                         = false;
	enum { Idle, Arg, QuotedArg } state = Idle;

	for (const char c : cmdLine)
	{
		if (!escape && c == '\\')
		{
			escape = true;
			continue;
		}

		switch (state)
		{
			case Idle:
				if (!escape && c == '"')
				{
					state = QuotedArg;
#if defined TARGET_OS_Windows
					arg += c;
#endif
				}
				else if (escape || c != ' ')
				{
					arg += c;
					state = Arg;
				}
				break;

			case Arg:
				if (!escape && c == '"')
				{
					state = QuotedArg;
#if defined TARGET_OS_Windows
					arg += c;
#endif
				}
				else if (escape || c != ' ') { arg += c; }
				else
				{
					list.push_back(arg);
					arg.clear();
					state = Idle;
				}
				break;

			case QuotedArg:
				if (!escape && c == '"')
				{
					state = arg.empty() ? Idle : Arg;
#if defined TARGET_OS_Windows
					arg += c;
#endif
				}
				else { arg += c; }
				break;
		}

		escape = false;
	}

	if (!arg.empty()) { list.push_back(arg); }

	return list;
}

char* strToChar(const std::string& s)
{
	char* pc;
	std::copy(s.begin(), s.end(), pc);
	return pc;
}

bool CBoxAlgorithmExternalProcessing::launchThirdPartyProgram(const std::string& programPath, const std::string& arguments)
{
	m_ThirdPartyProgramProcessId = 0;

	const std::vector<std::string> argumentsVector = splitCommandLine(arguments);

	std::vector<std::string> programArguments = { programPath, "--connection-id", m_ConnectionID, "--port", std::to_string(m_Port) };
	programArguments.insert(programArguments.begin() + 1, argumentsVector.cbegin(), argumentsVector.cend()); // Add the arguments after the program path.
	std::vector<char*> argv;

	std::transform(programArguments.begin(), programArguments.end(), std::back_inserter(argv), strToChar);

	argv.push_back(nullptr);

#ifdef TARGET_OS_Windows
	// _spawnp on Windows has a special case for empty program
	int status;
	if (programPath.empty())
	{
		status = -1;
		_set_errno(ENOENT);
	}
	else { status = int(_spawnvp(_P_NOWAIT, programPath.c_str(), argv.data())); }
	m_ThirdPartyProgramProcessId = status;
	//	_P_DETACH,
#else
	posix_spawn_file_actions_t fileAction;
	int res = posix_spawn_file_actions_init(&fileAction);
	OV_ERROR_UNLESS_KRF(res==0, "File action could not be initialized. Got error [" << res << "]", ErrorType::BadCall);
	res = posix_spawn_file_actions_addclose(&fileAction, STDOUT_FILENO);
	OV_ERROR_UNLESS_KRF(res==0, "File action 'close' could not be added. Got error [" << res << "]", ErrorType::BadCall);

	this->getLogManager() << LogLevel_Info << "Run third party program [" << programPath.c_str()
		<< "] with arguments [" << arguments.c_str() << "].\n";
	int status = posix_spawnp(&m_ThirdPartyProgramProcessId, programPath.c_str(), &fileAction, nullptr, argv.data(), environ);

#ifdef TARGET_OS_Linux
	// On linux the glibc is bugged and posix_spawnp does not actually work as specified
	// we have to check if the program did not exit immediately with a 127 error code
	// for this we need to wait a bit because it could be that the process has not yet launched

	// wait until the process exists or if it has failed

	bool processExists = false;
	bool processHasFailed = false;
	while (!processExists && !processHasFailed)
	{
		System::Time::sleep(10);
		processExists = (kill(m_ThirdPartyProgramProcessId, 0) == 0);

		int childStatus;
		pid_t pid = waitpid(m_ThirdPartyProgramProcessId, &childStatus, WNOHANG);

		if (pid != 0)
		{
			// The process is dead
			if (WIFEXITED(childStatus))
			{
				// If the exit status is 0 this means that maybe we will actually succeed in launching the program
				if (WEXITSTATUS(childStatus) != 0)
				{
					m_ThirdPartyProgramProcessId = 0;
					status = WEXITSTATUS(childStatus);
				}
			}
			else
			{
				OV_WARNING_K("The third party process died");
				m_ThirdPartyProgramProcessId = 0;
				status = 1;
			}
			processHasFailed = true;
		}
	}

#endif

#endif

	for (size_t i = 0; i < argv.size() - 1; ++i) { delete[] argv[i]; }

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	if (status != 0)
	{
		const std::string errorMessage = "Failed to launch the program [" + programPath + "]: ";
		m_ThirdPartyProgramProcessId = 0;
		OV_ERROR_KRF( errorMessage.c_str() << "[" << status << "]", ErrorType::BadResourceCreation);
	}
#else
	if (status == -1)
	{
		std::string errorMessage = "Failed to launch the program [" + programPath + "]:";

		switch (errno)
		{
			case E2BIG:
				errorMessage += "Argument list exceeds 1024 bytes.\n";
				break;

			case EINVAL:
				errorMessage += "Mode argument is invalid.\n";
				break;

			case ENOENT:
				errorMessage += "File or path is not found.\n";
				break;

			case ENOEXEC:
				errorMessage += "Specified file is not executable or has invalid executable-file format.\n";
				break;

			case ENOMEM:
				errorMessage += "Not enough memory is available to execute the new process.\n";
				break;

			default:
				errorMessage += "Unknown error.\n";
				break;
		}

		OV_ERROR_KRF(errorMessage.c_str(), ErrorType::BadResourceCreation);
	}
#endif

	return true;
}

void CBoxAlgorithmExternalProcessing::log()
{
	Communication::ELogLevel logLevel;
	std::string logMessage;
	uint64_t packetId;

	while (m_Messaging.popLog(packetId, logLevel, logMessage))
	{
		ELogLevel loglevel = LogLevel_Info;

		switch (logLevel)
		{
			case Communication::LogLevel_Info:
				loglevel = LogLevel_Info;
				break;

			case Communication::LogLevel_Warning:
				loglevel = LogLevel_Warning;
				break;

			case Communication::LogLevel_Error:
				loglevel = LogLevel_Error;
				break;

			case Communication::LogLevel_Fatal:
				loglevel = LogLevel_Fatal;

				break;

			case Communication::LogLevel_Unknown:
			case Communication::LogLevel_Max:
				break;
		}

		this->getLogManager() << loglevel << "From third party program: " << logMessage.c_str() << "\n";
	}
}
