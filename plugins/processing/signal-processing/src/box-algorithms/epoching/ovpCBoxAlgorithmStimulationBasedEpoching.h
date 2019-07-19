#pragma once

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <deque>
#include <memory>

#define OVP_ClassId_BoxAlgorithm_StimulationBasedEpoching                              OpenViBE::CIdentifier(0x426163D1, 0x324237B0)
#define OVP_ClassId_BoxAlgorithm_StimulationBasedEpochingDesc                          OpenViBE::CIdentifier(0x4F60616D, 0x468E0A8C)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmStimulationBasedEpoching final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processInput(uint32_t index);
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_StimulationBasedEpoching);

		private:
			OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmStimulationBasedEpoching> m_SignalDecoder;
			OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmStimulationBasedEpoching> m_StimulationDecoder;
			OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmStimulationBasedEpoching> m_SignalEncoder;

			uint64_t m_StimulationId;
			double m_EpochDurationInSeconds;
			uint64_t m_EpochDuration;
			int64_t m_EpochOffset;

			// Input matrix parameters
			uint64_t m_SamplingRate;
			uint32_t m_SampleCountPerInputBuffer;

			// Output matrix dimensions
			uint32_t m_ChannelCount;
			uint32_t m_SampleCountPerOutputEpoch;

			uint64_t m_LastSignalChunkEndTime;
			uint64_t m_LastStimulationChunkStartTime;
			uint64_t m_LastReceivedStimulationDate;

			std::deque<uint64_t> m_ReceivedStimulations;

			struct CachedChunk
			{
				CachedChunk(uint64_t startTime, uint64_t endTime, OpenViBE::IMatrix* matrix)
					: startTime(startTime)
					  , endTime(endTime)
					  , matrix(matrix) {}

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

			std::deque<CachedChunk> m_CachedChunks;
		};

		class CBoxAlgorithmStimulationBasedEpochingDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Stimulation based epoching"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Jozef Legeny"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("Mensia Technologies"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Slices signal into chunks of a desired length following a stimulation event."); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("Slices signal into chunks of a desired length following a stimulation event."); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Signal processing/Epoching"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("2.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_StimulationBasedEpoching; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmStimulationBasedEpoching; }
			virtual OpenViBE::CString getStockItemName() const { return "gtk-cut"; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Input signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addInput("Input stimulations", OV_TypeId_Stimulations);

				rBoxAlgorithmPrototype.addOutput("Epoched signal", OV_TypeId_Signal);

				rBoxAlgorithmPrototype.addSetting("Epoch duration (in sec)", OV_TypeId_Float, "1");
				rBoxAlgorithmPrototype.addSetting("Epoch offset (in sec)", OV_TypeId_Float, "0.5");
				rBoxAlgorithmPrototype.addSetting("Stimulation to epoch from", OV_TypeId_Stimulation, "OVTK_StimulationId_Label_00");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StimulationBasedEpochingDesc);
		};
	};
};


