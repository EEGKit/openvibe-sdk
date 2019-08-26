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
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t ui32InputIndex) override;
			bool process() override;

			// implementation for TResampler::ICallback
			void processResampler(const double* pSample, size_t ui32ChannelCount) const override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_SignalResampling)

		protected:

			OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmSignalResampling> m_oDecoder;
			mutable OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmSignalResampling> m_oEncoder;

			uint32_t m_outSamplingRate = 0;
			uint32_t m_outSampleCount  = 0;

			int m_iFractionalDelayFilterSampleCount = 0;
			double m_f64TransitionBandInPercent     = 0;
			double m_f64StopBandAttenuation         = 0;

			uint32_t m_inSamplingRate = 0;
			mutable uint64_t m_totalOutSampleCount;
			CResampler m_oResampler;
			OpenViBE::Kernel::IBoxIO* m_pDynamicBoxContext = nullptr;
		};

		class CBoxAlgorithmSignalResamplingDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Signal Resampling"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Quentin Barthelemy"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Mensia Technologies SA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Resamples and re-epochs input signal to chosen sampling frequency"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("The input signal is resampled, down-sampled or up-sampled, at a chosen sampling frequency and then re-epoched."); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Temporal Filtering"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("2.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_SignalResampling; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmSignalResampling; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input signal", OV_TypeId_Signal);
				prototype.addOutput("Output signal", OV_TypeId_Signal);
				prototype.addSetting("New Sampling Frequency", OV_TypeId_Integer, "128", false, OVP_ClassId_BoxAlgorithm_SignalResampling_SettingId_NewSamplingFrequency);
				prototype.addSetting("Sample Count Per Buffer", OV_TypeId_Integer, "8", false, OVP_ClassId_BoxAlgorithm_SignalResampling_SettingId_SampleCountPerBuffer);
				prototype.addSetting("Low Pass Filter Signal Before Downsampling", OV_TypeId_Boolean, "true", false, OVP_ClassId_BoxAlgorithm_SignalResampling_SettingId_LowPassFilterSignalFlag); // displayed for backward compatibility, but never used

				//prototype.addSetting("New Sampling Frequency",OV_TypeId_Integer,"128");
				//prototype.addSetting("Sample Count Per Buffer",OV_TypeId_Integer,"8");
				//prototype.addSetting("Low Pass Filter Signal Before Downsampling", OV_TypeId_Boolean, "true"); // displayed for backward compatibility, but never used

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SignalResamplingDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins
