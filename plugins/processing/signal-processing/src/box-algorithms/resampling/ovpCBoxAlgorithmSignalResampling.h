/*
 * MENSIA TECHNOLOGIES CONFIDENTIAL
 * ________________________________
 *
 *  [2012] - [2013] Mensia Technologies SA
 *  Copyright, All Rights Reserved.
 *
 * NOTICE: All information contained herein is, and remains
 * the property of Mensia Technologies SA.
 * The intellectual and technical concepts contained
 * herein are proprietary to Mensia Technologies SA
 * and are covered copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Mensia Technologies SA.
 */

#ifndef __OpenViBEPlugins_BoxAlgorithm_SignalResampling_H__
#define __OpenViBEPlugins_BoxAlgorithm_SignalResampling_H__

#include "ovCResampler.h"

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <Eigen/Eigen>

#define OVP_ClassId_BoxAlgorithm_SignalResampling     OpenViBE::CIdentifier(0x0E923A5E, 0xDA474058)
#define OVP_ClassId_BoxAlgorithm_SignalResamplingDesc OpenViBE::CIdentifier(0xA675A433, 0xC6690920)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		typedef ::Common::Resampler::CResamplerSd CResampler;

		class CBoxAlgorithmSignalResampling : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, CResampler::ICallback
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			// implementation for TResampler::ICallback
			virtual void process(const OpenViBE::float64* pSample, size_t ui32ChannelCount) const;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_SignalResampling);

		protected:

			OpenViBEToolkit::TSignalDecoder < CBoxAlgorithmSignalResampling > m_oDecoder;
			mutable OpenViBEToolkit::TSignalEncoder < CBoxAlgorithmSignalResampling > m_oEncoder;

			OpenViBE::uint32 m_ui32OutputSamplingRate;
			OpenViBE::uint32 m_ui32OutputSampleCount;

			int m_iFractionalDelayFilterSampleCount;
			double m_f64TransitionBandInPercent;
			double m_f64StopBandAttenuation;

			OpenViBE::uint32 m_ui32InputSamplingRate;
			mutable OpenViBE::uint64 m_ui64TotalOutputSampleCount;
			CResampler m_oResampler;
			OpenViBE::Kernel::IBoxIO* m_pDynamicBoxContext;
		};

		class CBoxAlgorithmSignalResamplingDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                    { return OpenViBE::CString("Signal Resampling"); }
			virtual OpenViBE::CString getAuthorName(void) const              { return OpenViBE::CString("Quentin Barthelemy"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const       { return OpenViBE::CString("Mensia Technologies SA"); }
			virtual OpenViBE::CString getShortDescription(void) const        { return OpenViBE::CString("Resamples and re-epochs input signal to chosen sampling frequency"); }
			virtual OpenViBE::CString getDetailedDescription(void) const     { return OpenViBE::CString("The input signal is resampled, down-sampled or up-sampled, at a chosen sampling frequency and then re-epoched."); }
			virtual OpenViBE::CString getCategory(void) const                { return OpenViBE::CString("Signal processing/Temporal Filtering"); }
			virtual OpenViBE::CString getVersion(void) const                 { return OpenViBE::CString("2.0"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const    { return OpenViBE::CString("1.0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const  { return OpenViBE::CString("2.3.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const        { return OVP_ClassId_BoxAlgorithm_SignalResampling; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)           { return new OpenViBEPlugins::SignalProcessing::CBoxAlgorithmSignalResampling; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Input signal",                               OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput ("Output signal",                              OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("New Sampling Frequency",                     OV_TypeId_Integer, "128");
				rBoxAlgorithmPrototype.addSetting("Sample Count Per Buffer",                    OV_TypeId_Integer, "8");
				rBoxAlgorithmPrototype.addSetting("Low Pass Filter Signal Before Downsampling", OV_TypeId_Boolean, "true"); // displayed for backward compatibility, but never used

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SignalResamplingDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_SignalResampling_H__
