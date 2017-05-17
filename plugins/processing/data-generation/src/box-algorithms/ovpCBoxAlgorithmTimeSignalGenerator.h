#ifndef __OpenViBEPlugins_BoxAlgorithm_TimeSignalGenerator_H__
#define __OpenViBEPlugins_BoxAlgorithm_TimeSignalGenerator_H__

#include "../ovp_defines.h"

#include <toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_TimeSignalGeneratorDesc                 OpenViBE::CIdentifier(0x57AD8655, 0x1966B4DC)

#define OVP_ClassId_BoxAlgorithm_TimeSignalGenerator                     OpenViBE::CIdentifier(0x28A5E7FF, 0x530095DE)

namespace OpenViBEPlugins
{
	namespace DataGeneration
	{
		class CBoxAlgorithmTimeSignalGenerator : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			CBoxAlgorithmTimeSignalGenerator(void);

			virtual void release(void);

			virtual OpenViBE::uint64 getClockFrequency(void);

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean processClock(OpenViBE::Kernel::IMessageClock& rMessageClock);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_TimeSignalGenerator)

		protected:

			OpenViBEToolkit::TSignalEncoder < CBoxAlgorithmTimeSignalGenerator > m_oSignalEncoder;

			OpenViBE::boolean m_bHeaderSent;
			OpenViBE::uint32 m_ui32SamplingFrequency;
			OpenViBE::uint32 m_ui32GeneratedEpochSampleCount;
			OpenViBE::uint32 m_ui32SentSampleCount;
		};

		class CBoxAlgorithmTimeSignalGeneratorDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Time signal"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Simple time signal generator (for use with DSP)"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Data generation"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent(void) const   { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const   { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_TimeSignalGenerator; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::DataGeneration::CBoxAlgorithmTimeSignalGenerator(); }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addOutput("Generated signal",              OV_TypeId_Signal);

				rPrototype.addSetting("Sampling frequency",           OV_TypeId_Integer, "512");
				rPrototype.addSetting("Generated epoch sample count", OV_TypeId_Integer, "32");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_TimeSignalGeneratorDesc)
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_TimeSignalGenerator_H__
