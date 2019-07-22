#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <communication/ovCMessagingServer.h>

#include <map>
#include <queue>
#include <vector>
#include <cstdint>


#define OVP_ClassId_BoxAlgorithm_ExternalProcessing     OpenViBE::CIdentifier(0x15422959, 0x16304449)
#define OVP_ClassId_BoxAlgorithm_ExternalProcessingDesc OpenViBE::CIdentifier(0x63386942, 0x61D42502)

namespace OpenViBEPlugins
{
	namespace Tools
	{
		class CBoxAlgorithmExternalProcessing : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			CBoxAlgorithmExternalProcessing();

			virtual void release() { delete this; }

			virtual uint64_t getClockFrequency();
			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual bool processInput(uint32_t inputIndex);
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ExternalProcessing)

		private:

			struct Packet
			{
				uint64_t startTime;
				uint64_t endTime;
				uint32_t inputIndex;
				std::shared_ptr<std::vector<uint8_t>> EBML;

				Packet(uint64_t startTime, uint64_t endTime, uint32_t inputIndex, std::shared_ptr<std::vector<uint8_t>> EBML)
					: startTime(startTime)
					  , endTime(endTime)
					  , inputIndex(inputIndex)
					  , EBML(EBML) { }
			};

			/**
			 * \brief generate a connection identifier.
			 *
			 * \param size Size of the connection identifier.
			 *
			 * \return A string composed of size characters in the A-Z,0-9 range
			 */
			static std::string generateConnectionID(size_t size);

			/**
			 * \brief Launch a third party program
			 *
			 * \param programPath Executable path
			 * \param arguments Arguments to pass to the program
			 *
			 * \retval true
			 * \retval false
			 */
			bool launchThirdPartyProgram(const std::string& programPath, const std::string& arguments);

			/**
			 * \brief Log in NeuroRT log from third party program
			 */
			void log();

			Communication::MessagingServer m_Messaging;
			uint32_t m_Port;
			std::string m_ConnectionID;
			std::string m_ProgramPath;
			bool m_IsGenerator;

			int m_ThirdPartyProgramProcessId;

			uint64_t m_AcceptTimeout;
			bool m_ShouldLaunchProgram;
			bool m_HasReceivedEndMessage;
			// Synchronization timeout, and save time of last synchronization
			uint64_t m_SyncTimeout;
			uint64_t m_LastSyncTime;

			std::map<uint64_t, OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmExternalProcessing>> m_StimulationDecoders;
			std::queue<Packet> m_PacketHistory;
		};

		class CBoxAlgorithmExternalProcessingListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmExternalProcessingDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() {}

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("External Processing"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Alexis Placet"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("Mensia Technologies SA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("This box can communicate via TCP with an other program."); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("Launches an external program which can then processes data. This box and the program communicate using TCP connection and a defined protocol."); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Scripting"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("1.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("1.0.0"); }
			virtual OpenViBE::CString getStockItemName() const { return OpenViBE::CString("gtk-edit"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_ExternalProcessing; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmExternalProcessing; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener() const { return new CBoxAlgorithmExternalProcessingListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* boxListener) const { delete boxListener; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& boxAlgorithmPrototype) const
			{
				boxAlgorithmPrototype.addSetting("Launch third party program", OV_TypeId_Boolean, "true");     // 0
				boxAlgorithmPrototype.addSetting("Executable path", OV_TypeId_Filename, "");                   // 1
				boxAlgorithmPrototype.addSetting("Arguments", OV_TypeId_String, "");                           // 2
				boxAlgorithmPrototype.addSetting("Port", OV_TypeId_Integer, "0");                          // 3
				boxAlgorithmPrototype.addSetting("Automatic connection identifier", OV_TypeId_Boolean, "true");// 4
				boxAlgorithmPrototype.addSetting("Custom connection identifier", OV_TypeId_String, "");        // 5
				boxAlgorithmPrototype.addSetting("Incoming connection timeout", OV_TypeId_Integer, "10");      // 6
				boxAlgorithmPrototype.addSetting("Generator", OV_TypeId_Boolean, "false");                     // 7

				boxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				boxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				boxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddOutput);
				boxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);
				boxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddSetting);
				boxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifySetting);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ExternalProcessingDesc)
		};
	};
};
