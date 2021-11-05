#pragma once

#include "defines.hpp"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <deque>
#include <memory>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {

class CBoxAlgorithmStimulationBasedEpoching final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_StimulationBasedEpoching)

private:
	Toolkit::TSignalDecoder<CBoxAlgorithmStimulationBasedEpoching> m_signalDecoder;
	Toolkit::TStimulationDecoder<CBoxAlgorithmStimulationBasedEpoching> m_stimDecoder;
	Toolkit::TSignalEncoder<CBoxAlgorithmStimulationBasedEpoching> m_encoder;

	uint64_t m_stimulationID        = 0;
	double m_epochDurationInSeconds = 0;
	uint64_t m_epochDuration        = 0;
	int64_t m_epochOffset           = 0;

	// Input matrix parameters
	size_t m_sampling              = 0;
	size_t m_nSamplePerInputBuffer = 0;

	// Output matrix dimensions
	size_t m_nChannel                = 0;
	size_t m_nSampleCountOutputEpoch = 0;

	uint64_t m_lastSignalChunkEndTime        = 0;
	uint64_t m_lastStimulationChunkStartTime = 0;
	uint64_t m_lastReceivedStimulationDate   = 0;

	std::deque<uint64_t> m_receivedStimulations;

	struct SCachedChunk
	{
		SCachedChunk(const uint64_t startTime, const uint64_t endTime, CMatrix* matrix)
			: startTime(startTime), endTime(endTime), matrix(matrix) {}

		SCachedChunk& operator=(SCachedChunk&& other)
		{
			this->startTime = other.startTime;
			this->endTime   = other.endTime;
			this->matrix    = std::move(other.matrix);
			return *this;
		}

		uint64_t startTime;
		uint64_t endTime;
		std::unique_ptr<CMatrix> matrix;
	};

	std::deque<SCachedChunk> m_cachedChunks;
};

class CBoxAlgorithmStimulationBasedEpochingDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return "Stimulation based epoching"; }
	CString getAuthorName() const override { return "Jozef Legeny"; }
	CString getAuthorCompanyName() const override { return "Mensia Technologies"; }

	CString getShortDescription() const override { return "Slices signal into chunks of a desired length following a stimulation event."; }

	CString getDetailedDescription() const override { return "Slices signal into chunks of a desired length following a stimulation event."; }

	CString getCategory() const override { return "Signal processing/Epoching"; }
	CString getVersion() const override { return "2.0"; }
	CString getSoftwareComponent() const override { return "openvibe-sdk"; }
	CString getAddedSoftwareVersion() const override { return "0.0.0"; }
	CString getUpdatedSoftwareVersion() const override { return "0.1.0"; }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_StimulationBasedEpoching; }
	IPluginObject* create() override { return new CBoxAlgorithmStimulationBasedEpoching; }
	CString getStockItemName() const override { return "gtk-cut"; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input signal", OV_TypeId_Signal);
		prototype.addInput("Input stimulations", OV_TypeId_Stimulations);

		prototype.addOutput("Epoched signal", OV_TypeId_Signal);

		prototype.addSetting("Epoch duration (in sec)", OV_TypeId_Float, "1");
		prototype.addSetting("Epoch offset (in sec)", OV_TypeId_Float, "0.5");
		prototype.addSetting("Stimulation to epoch from", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");

		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StimulationBasedEpochingDesc)
};

}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
