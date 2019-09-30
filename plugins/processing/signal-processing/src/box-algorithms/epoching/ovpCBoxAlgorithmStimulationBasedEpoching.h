#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <deque>
#include <memory>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmStimulationBasedEpoching final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_StimulationBasedEpoching)

		private:
			OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmStimulationBasedEpoching> m_signalDecoder;
			OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmStimulationBasedEpoching> m_stimulationDecoder;
			OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmStimulationBasedEpoching> m_signalEncoder;

			uint64_t m_stimulationID        = 0;
			double m_epochDurationInSeconds = 0;
			uint64_t m_epochDuration        = 0;
			int64_t m_epochOffset           = 0;

			// Input matrix parameters
			uint64_t m_samplingRate              = 0;
			uint32_t m_nSamplePerInputBuffer = 0;

			// Output matrix dimensions
			uint32_t m_nChannel              = 0;
			uint32_t m_nSampleCountOutputEpoch = 0;

			uint64_t m_lastSignalChunkEndTime        = 0;
			uint64_t m_lastStimulationChunkStartTime = 0;
			uint64_t m_lastReceivedStimulationDate   = 0;

			std::deque<uint64_t> m_receivedStimulations;

			struct CachedChunk
			{
				CachedChunk(const uint64_t startTime, const uint64_t endTime, OpenViBE::IMatrix* matrix)
					: startTime(startTime), endTime(endTime), matrix(matrix) {}

				CachedChunk& operator=(CachedChunk&& other)
				{
					this->startTime = other.startTime;
					this->endTime   = other.endTime;
					this->matrix    = std::move(other.matrix);
					return *this;
				}

				uint64_t startTime;
				uint64_t endTime;
				std::unique_ptr<OpenViBE::IMatrix> matrix;
			};

			std::deque<CachedChunk> m_cachedChunks;
		};

		class CBoxAlgorithmStimulationBasedEpochingDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Stimulation based epoching"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Jozef Legeny"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Mensia Technologies"); }

			OpenViBE::CString getShortDescription() const override
			{
				return OpenViBE::CString("Slices signal into chunks of a desired length following a stimulation event.");
			}

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString("Slices signal into chunks of a desired length following a stimulation event.");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Epoching"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("2.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.1.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_StimulationBasedEpoching; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmStimulationBasedEpoching; }
			OpenViBE::CString getStockItemName() const override { return "gtk-cut"; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input signal", OV_TypeId_Signal);
				prototype.addInput("Input stimulations", OV_TypeId_Stimulations);

				prototype.addOutput("Epoched signal", OV_TypeId_Signal);

				prototype.addSetting("Epoch duration (in sec)", OV_TypeId_Float, "1");
				prototype.addSetting("Epoch offset (in sec)", OV_TypeId_Float, "0.5");
				prototype.addSetting("Stimulation to epoch from", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StimulationBasedEpochingDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins
