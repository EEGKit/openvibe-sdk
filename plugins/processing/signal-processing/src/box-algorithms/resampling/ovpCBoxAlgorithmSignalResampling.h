/*********************************************************************
 * Software License Agreement (AGPL-3 License)
 *
 * OpenViBE SDK
 * Based on OpenViBE V1.1.0, Copyright (C) Inria, 2006-2015
 * Copyright (C) Inria, 2015-2017,V1.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "ovCResampler.h"

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <Eigen/Eigen>

#define OVP_ClassId_BoxAlgorithm_SignalResampling     OpenViBE::CIdentifier(0x0E923A5E, 0xDA474058)
#define OVP_ClassId_BoxAlgorithm_SignalResamplingDesc OpenViBE::CIdentifier(0xA675A433, 0xC6690920)

#define OVP_ClassId_BoxAlgorithm_SignalResampling_SettingId_NewSamplingFrequency	OpenViBE::CIdentifier(0x158A8EFD, 0xAA894F86)
#define OVP_ClassId_BoxAlgorithm_SignalResampling_SettingId_SampleCountPerBuffer	OpenViBE::CIdentifier(0x588783F3, 0x8E8DCF86)
#define OVP_ClassId_BoxAlgorithm_SignalResampling_SettingId_LowPassFilterSignalFlag	OpenViBE::CIdentifier(0xAFDD8EFD, 0x23EF94F6)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		typedef Common::Resampler::CResamplerSd CResampler;

		class CBoxAlgorithmSignalResampling : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, CResampler::ICallback
		{
		public:

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processInput(uint32_t ui32InputIndex);
			virtual bool process();

			// implementation for TResampler::ICallback
			virtual void processResampler(const double* pSample, size_t ui32ChannelCount) const;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_SignalResampling)

		protected:

			OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmSignalResampling> m_oDecoder;
			mutable OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmSignalResampling> m_oEncoder;

			uint32_t m_outSamplingRate;
			uint32_t m_outSampleCount;

			int m_iFractionalDelayFilterSampleCount;
			double m_f64TransitionBandInPercent;
			double m_f64StopBandAttenuation;

			uint32_t m_inSamplingRate;
			mutable uint64_t m_totalOutSampleCount;
			CResampler m_oResampler;
			OpenViBE::Kernel::IBoxIO* m_pDynamicBoxContext;
		};

		class CBoxAlgorithmSignalResamplingDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Signal Resampling"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Quentin Barthelemy"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("Mensia Technologies SA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Resamples and re-epochs input signal to chosen sampling frequency"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("The input signal is resampled, down-sampled or up-sampled, at a chosen sampling frequency and then re-epoched."); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Signal processing/Temporal Filtering"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("2.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_SignalResampling; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmSignalResampling; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Input signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput("Output signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("New Sampling Frequency", OV_TypeId_Integer, "128", false, OVP_ClassId_BoxAlgorithm_SignalResampling_SettingId_NewSamplingFrequency);
				rBoxAlgorithmPrototype.addSetting("Sample Count Per Buffer", OV_TypeId_Integer, "8", false, OVP_ClassId_BoxAlgorithm_SignalResampling_SettingId_SampleCountPerBuffer);
				rBoxAlgorithmPrototype.addSetting("Low Pass Filter Signal Before Downsampling", OV_TypeId_Boolean, "true", false, OVP_ClassId_BoxAlgorithm_SignalResampling_SettingId_LowPassFilterSignalFlag); // displayed for backward compatibility, but never used

				//rBoxAlgorithmPrototype.addSetting("New Sampling Frequency",OV_TypeId_Integer,"128");
				//rBoxAlgorithmPrototype.addSetting("Sample Count Per Buffer",OV_TypeId_Integer,"8");
				//rBoxAlgorithmPrototype.addSetting("Low Pass Filter Signal Before Downsampling", OV_TypeId_Boolean, "true"); // displayed for backward compatibility, but never used

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SignalResamplingDesc)
		};
	};
};
