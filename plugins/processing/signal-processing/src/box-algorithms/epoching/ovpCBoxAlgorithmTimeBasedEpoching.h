#ifndef __OpenViBEPlugins_BoxAlgorithm_TimeBasedEpoching_H__
#define __OpenViBEPlugins_BoxAlgorithm_TimeBasedEpoching_H__

#include <toolkit/ovtk_all.h>
#include <vector>
#include <cstdio>

#define OVP_ClassId_BoxAlgorithm_TimeBasedEpoching                                     OpenViBE::CIdentifier(0x00777FA0, 0x5DC3F560)
#define OVP_ClassId_BoxAlgorithm_TimeBasedEpochingDesc                                 OpenViBE::CIdentifier(0x00ABDABE, 0x41381683)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmTimeBasedEpoching final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			virtual void release(void) { delete this; }

			virtual bool initialize(void);
			virtual bool uninitialize(void);
			virtual bool processInput(uint32_t ui32InputIndex);
			virtual bool process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_TimeBasedEpoching)

		private:

			OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmTimeBasedEpoching> m_SignalDecoder;
			OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmTimeBasedEpoching> m_SignalEncoder;

			double m_EpochDuration;
			double m_EpochInterval;

			uint64_t m_SamplingRate;
			uint32_t m_OutputSampleCount;
			uint32_t m_OutputSampleCountBetweenEpoch;
			uint64_t m_LastInputEndTime;
			uint32_t m_OutputSampleIndex;
			uint32_t m_OutputChunkIndex;
			uint64_t m_ReferenceTime;
		};

		class CBoxAlgorithmTimeBasedEpochingDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const { return OpenViBE::CString("Time based epoching"); }
			virtual OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Quentin Barthelemy"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("Mensia Technologies SA"); }
			virtual OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString("Generates signal 'slices' or 'blocks' having a specified duration and interval"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Interval can be used to control the overlap of epochs"); }
			virtual OpenViBE::CString getCategory(void) const { return OpenViBE::CString("Signal processing/Epoching"); }
			virtual OpenViBE::CString getVersion(void) const { return OpenViBE::CString("2.0"); }
			virtual OpenViBE::CString getSoftwareComponent(void) const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getStockItemName(void) const { return OpenViBE::CString("gtk-cut"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_BoxAlgorithm_TimeBasedEpoching; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new OpenViBEPlugins::SignalProcessing::CBoxAlgorithmTimeBasedEpoching(); }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput("Input signal", OV_TypeId_Signal);
				rPrototype.addOutput("Epoched signal", OV_TypeId_Signal);
				rPrototype.addSetting("Epoch duration (in sec)", OV_TypeId_Float, "1");
				rPrototype.addSetting("Epoch intervals (in sec)", OV_TypeId_Float, "0.5");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_TimeBasedEpochingDesc)
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_TimeBasedEpoching_H__
