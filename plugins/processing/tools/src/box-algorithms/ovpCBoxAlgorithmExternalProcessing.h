#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.hpp>
#include <toolkit/ovtk_all.h>

#include <communication/ovCMessagingServer.h>

#include <map>
#include <queue>
#include <vector>
#include <cstdint>

namespace OpenViBE {
namespace Plugins {
namespace Tools {

class CBoxAlgorithmExternalProcessing final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	CBoxAlgorithmExternalProcessing() : m_acceptTimeout(10ULL << 32) {}
	void release() override { delete this; }
	uint64_t getClockFrequency() override;
	bool initialize() override;
	bool uninitialize() override;
	bool processClock(CMessage& msg) override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_ExternalProcessing)

private:

	struct SPacket
	{
		CTime startTime;
		CTime endTime;
		size_t index;
		std::shared_ptr<std::vector<uint8_t>> EBML;

		SPacket(const CTime startTime, const CTime endTime, const size_t index, const std::shared_ptr<std::vector<uint8_t>>& ebml)
			: startTime(startTime), endTime(endTime), index(index), EBML(ebml) { }
	};

	/**
	 * \brief generate a connection identifier.
	 *
	 * \param size Size of the connection identifier.
	 *
	 * \return A string composed of size characters in the A-Z,0-9 range
	 */
	static std::string generateConnectionID(const size_t size);

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

	Communication::MessagingServer m_messaging;
	size_t m_port = 0;
	std::string m_connectionID;
	std::string m_programPath;
	bool m_isGenerator = false;

	int m_extProcessId = 0;

	CTime m_acceptTimeout        = 0;
	bool m_shouldLaunchProgram   = false;
	bool m_hasReceivedEndMessage = false;
	// Synchronization timeout, and save time of last synchronization
	CTime m_syncTimeout  = 0;
	CTime m_lastSyncTime = 0;

	std::map<uint64_t, Toolkit::TStimulationDecoder<CBoxAlgorithmExternalProcessing>> m_decoders;
	std::queue<SPacket> m_packetHistory;
};

class CBoxAlgorithmExternalProcessingListener final : public Toolkit::TBoxListener<IBoxListener>
{
public:
	_IsDerivedFromClass_Final_(Toolkit::TBoxListener<IBoxListener>, OV_UndefinedIdentifier)
};

class CBoxAlgorithmExternalProcessingDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override {}
	CString getName() const override { return "External Processing"; }
	CString getAuthorName() const override { return "Alexis Placet"; }
	CString getAuthorCompanyName() const override { return "Mensia Technologies SA"; }
	CString getShortDescription() const override { return "This box can communicate via TCP with an other program."; }

	CString getDetailedDescription() const override
	{
		return "Launches an external program which can then processes data. This box and the program communicate using TCP connection and a defined protocol.";
	}

	CString getCategory() const override { return "Scripting"; }
	CString getVersion() const override { return "1.0"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "1.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "1.0.0"; }
	CString getStockItemName() const override { return "gtk-edit"; }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_ExternalProcessing; }
	IPluginObject* create() override { return new CBoxAlgorithmExternalProcessing; }
	IBoxListener* createBoxListener() const override { return new CBoxAlgorithmExternalProcessingListener; }
	void releaseBoxListener(IBoxListener* boxListener) const override { delete boxListener; }

	bool getBoxPrototype(Kernel::IBoxProto& boxAlgorithmPrototype) const override
	{
		boxAlgorithmPrototype.addSetting("Launch third party program", OV_TypeId_Boolean, "true");		// 0
		boxAlgorithmPrototype.addSetting("Executable path", OV_TypeId_Filename, "");					// 1
		boxAlgorithmPrototype.addSetting("Arguments", OV_TypeId_String, "");							// 2
		boxAlgorithmPrototype.addSetting("Port", OV_TypeId_Integer, "0");								// 3
		boxAlgorithmPrototype.addSetting("Automatic connection identifier", OV_TypeId_Boolean, "true");	// 4
		boxAlgorithmPrototype.addSetting("Custom connection identifier", OV_TypeId_String, "");			// 5
		boxAlgorithmPrototype.addSetting("Incoming connection timeout", OV_TypeId_Integer, "10");		// 6
		boxAlgorithmPrototype.addSetting("Generator", OV_TypeId_Boolean, "false");						// 7

		boxAlgorithmPrototype.addFlag(Kernel::BoxFlag_CanAddInput);
		boxAlgorithmPrototype.addFlag(Kernel::BoxFlag_CanModifyInput);
		boxAlgorithmPrototype.addFlag(Kernel::BoxFlag_CanAddOutput);
		boxAlgorithmPrototype.addFlag(Kernel::BoxFlag_CanModifyOutput);
		boxAlgorithmPrototype.addFlag(Kernel::BoxFlag_CanAddSetting);
		boxAlgorithmPrototype.addFlag(Kernel::BoxFlag_CanModifySetting);
		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ExternalProcessingDesc)
};

}  // namespace Tools
}  // namespace Plugins
}  // namespace OpenViBE
