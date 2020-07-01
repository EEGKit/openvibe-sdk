#pragma once

#include "../ovkTKernelObject.h"
#include "ovkCScheduler.h"

#include "../ovkCKernelContext.h"

#include <system/ovCChrono.h>

#include <string>

namespace OpenViBE {
namespace Kernel {
class CScenarioSettingKeywordParserCallback;

class CPlayer final : public TKernelObject<IPlayer>
{
public:

	explicit CPlayer(const IKernelContext& ctx);
	~CPlayer() override;
	bool setScenario(const CIdentifier& scenarioID, const CNameValuePairList* localConfigurationTokens) override;
	IConfigurationManager& getRuntimeConfigurationManager() const override { return *m_runtimeConfigManager; }
	IScenarioManager& getRuntimeScenarioManager() const override { return *m_runtimeScenarioManager; }
	CIdentifier getRuntimeScenarioIdentifier() const override { return m_runtimeScenarioID; }


	bool isHoldingResources() const { return m_scheduler.isHoldingResources(); }
	EPlayerReturnCodes initialize() override;
	bool uninitialize() override;
	bool stop() override;
	bool pause() override;
	bool step() override;
	bool play() override;
	bool forward() override;
	EPlayerStatus getStatus() const override { return m_status; }
	bool setFastForwardMaximumFactor(double fastForwardFactor) override;
	double getFastForwardMaximumFactor() const override { return m_fastForwardMaximumFactor; }
	double getCPUUsage() const override { return m_scheduler.getCPUUsage(); }
	bool loop(CTime elapsedTime, CTime maximumTimeToReach) override;
	CTime getCurrentSimulatedTime() const override { return m_scheduler.getCurrentTime(); }
	CTime getCurrentSimulatedLateness() const { return m_innerLateness; }

	_IsDerivedFromClass_Final_(TKernelObject<IPlayer>, OVK_ClassId_Kernel_Player_Player)

protected:

	CKernelContextBridge m_kernelCtxBridge;
	IConfigurationManager* m_runtimeConfigManager                           = nullptr;
	IScenarioManager* m_runtimeScenarioManager                              = nullptr;
	CScenarioSettingKeywordParserCallback* m_scenarioSettingKeywordParserCB = nullptr;

	CScheduler m_scheduler;

	CTime m_currentTimeToReach        = 0;
	CTime m_lateness                  = 0;
	CTime m_innerLateness             = 0;
	EPlayerStatus m_status            = EPlayerStatus::Stop;
	bool m_isInitializing             = false;
	double m_fastForwardMaximumFactor = 0;

	std::string m_scenarioConfigFile;
	std::string m_workspaceConfigFile;

	// Stores the identifier of the scenario that is being played
	CIdentifier m_scenarioID = OV_UndefinedIdentifier;

private:
	CIdentifier m_runtimeScenarioID = OV_UndefinedIdentifier;

	System::CChrono m_benchmarkChrono;
};
}  // namespace Kernel
}  // namespace OpenViBE
