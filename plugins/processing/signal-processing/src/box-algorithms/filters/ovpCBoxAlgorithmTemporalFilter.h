#ifndef __OpenViBEPlugins_BoxAlgorithm_TemporalFilter_H__
#define __OpenViBEPlugins_BoxAlgorithm_TemporalFilter_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <dsp-filters/Dsp.h>

#define OVP_ClassId_BoxAlgorithm_TemporalFilter     OpenViBE::CIdentifier(0xB4F9D042, 0x9D79F2E5)
#define OVP_ClassId_BoxAlgorithm_TemporalFilterDesc OpenViBE::CIdentifier(0x7BF6BA62, 0xAF829A37)

// Filter method identifiers from OpenViBE 0.14.0
#define OVP_TypeId_FilterMethod                     OpenViBE::CIdentifier(0x2F2C606C, 0x8512ED68)
#define OVP_TypeId_FilterMethod_Butterworth         OpenViBE::CIdentifier(0x00E4EADE, 0x9586A50E)
#define OVP_TypeId_FilterMethod_Chebyshev           OpenViBE::CIdentifier(0x0C3F5301, 0x7397A7FB)
#define OVP_TypeId_FilterMethod_YuleWalker          OpenViBE::CIdentifier(0x7BF59C86, 0xB6D459BA)

// Filter type identifiers from OpenViBE 0.14.0
#define OVP_TypeId_FilterType                       OpenViBE::CIdentifier(0xFA20178E, 0x4CBA62E9)
#define OVP_TypeId_FilterType_LowPass               OpenViBE::CIdentifier(0x0C33A568, 0xE2BD46D2)
#define OVP_TypeId_FilterType_BandPass              OpenViBE::CIdentifier(0x19E89DDB, 0xD1FF7FF5)
#define OVP_TypeId_FilterType_HighPass              OpenViBE::CIdentifier(0x2F677195, 0x9F9F15B9)
#define OVP_TypeId_FilterType_BandStop              OpenViBE::CIdentifier(0x41C34DD7, 0xF70E4043)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmTemporalFilter : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);
#if 0
			void filtfilt2 (Dsp::Filter* pFilter1, Dsp::Filter* pFilter2, OpenViBE::uint32 SampleCount, OpenViBE::float64* pBuffer);
			void filtfilt2mirror (Dsp::Filter* pFilter1, Dsp::Filter* pFilter2, OpenViBE::uint32 SampleCount, OpenViBE::float64* pBuffer);
#endif

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_TemporalFilter);

		protected:

			OpenViBEToolkit::TSignalDecoder < CBoxAlgorithmTemporalFilter > m_oDecoder;
			OpenViBEToolkit::TSignalEncoder < CBoxAlgorithmTemporalFilter > m_oEncoder;

			OpenViBE::uint64 m_ui64FilterMethod;
			OpenViBE::uint64 m_ui64FilterType;
			OpenViBE::uint64 m_ui64FilterOrder;

			OpenViBE::float64 m_f64LowCutFrequency;
			OpenViBE::float64 m_f64HighCutFrequency;
			OpenViBE::float64 m_f64BandPassRipple; // for Chebyshev

			std::vector < Dsp::Filter* > m_vFilter;
			//std::vector < Dsp::Filter* > m_vFilter2;

			std::vector < double > m_vFirstSample;
		};

		class CBoxAlgorithmTemporalFilterDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Temporal Filter"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard & Laurent Bonnet"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("Mensia Technologies SA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Temporal filtering based on various one-way IIR filter designs"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Applies a temporal filter, based on various one-way IIR filter designs, to the input stream."); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Temporal Filtering"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.1"); }
			virtual OpenViBE::CString getStockItemName(void) const       { return OpenViBE::CString("gtk-execute"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_TemporalFilter; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CBoxAlgorithmTemporalFilter; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Input signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput ("Output signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("Filter Method", OVP_TypeId_FilterMethod, OVP_TypeId_FilterMethod_Butterworth.toString());
				rBoxAlgorithmPrototype.addSetting("Filter Type",   OVP_TypeId_FilterType, OVP_TypeId_FilterType_BandPass.toString());
				rBoxAlgorithmPrototype.addSetting("Filter Order",  OV_TypeId_Integer, "4");
				rBoxAlgorithmPrototype.addSetting("Low Cut-off Frequency (Hz)",  OV_TypeId_Float, "1");
				rBoxAlgorithmPrototype.addSetting("High Cut-off Frequency (Hz)", OV_TypeId_Float, "40");
				rBoxAlgorithmPrototype.addSetting("Band Pass Ripple (dB)", OV_TypeId_Float, "0.5");
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_IsMensia);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_TemporalFilterDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_TemporalFilter_H__
