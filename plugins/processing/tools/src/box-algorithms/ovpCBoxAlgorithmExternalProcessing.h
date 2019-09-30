#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <communication/ovCMessagingServer.h>

#include <map>
#include <queue>
#include <vector>
#include <cstdint>

namespace OpenViBEPlugins
{
	namespace Tools
	{
		class CBoxAlgorithmExternalProcessing final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			CBoxAlgorithmExternalProcessing();
			void release() override { delete this; }
			uint64_t getClockFrequency() override;
			bool initialize() override;
			bool uninitialize() override;
			bool processClock(OpenViBE::CMessageClock& messageClock) override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ExternalProcessing)

		private:

			struct SPacket
			{
				uint64_t startTime;
				uint64_t endTime;
				uint32_t index;
				std::shared_ptr<std::vector<uint8_t>> EBML;

				SPacket(const uint64_t startTime, const uint64_t endTime, const uint32_t index, const std::shared_ptr<std::vector<uint8_t>> ebml)
					: startTime(startTime), endTime(endTime), index(index), EBML(ebml) { }
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
			uint32_t m_Port = 0;
			std::string m_ConnectionID;
			std::string m_ProgramPath;
			bool m_IsGenerator = false;

			int m_ThirdPartyProgramProcessId = 0;

			uint64_t m_AcceptTimeout     = 0;
			bool m_ShouldLaunchProgram   = false;
			bool m_HasReceivedEndMessage = false;
			// Synchronization timeout, and save time of last synchronization
			uint64_t m_SyncTimeout  = 0;
			uint64_t m_LastSyncTime = 0;

			std::map<uint64_t, OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmExternalProcessing>> m_StimulationDecoders;
			std::queue<SPacket> m_PacketHistory;
		};

		class CBoxAlgorithmExternalProcessingListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmExternalProcessingDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override {}
			OpenViBE::CString getName() const override { return OpenViBE::CString("External Processing"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Alexis Placet"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Mensia Technologies SA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("This box can communicate via TCP with an other program."); }

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString(
					"Launches an external program which can then processes data. This box and the program communicate using TCP connection and a defined protocol.");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Scripting"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("1.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("1.0.0"); }
			OpenViBE::CString getStockItemName() const override { return OpenViBE::CString("gtk-edit"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_ExternalProcessing; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmExternalProcessing; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmExternalProcessingListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* boxListener) const override { delete boxListener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& boxAlgorithmPrototype) const override
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
	} // namespace Tools
} // namespace OpenViBEPlugins
