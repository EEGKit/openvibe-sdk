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

#include "defines.hpp"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>
#include "ovCResampler.h"

#include <Eigen/Eigen>

namespace OpenViBE {
namespace Plugins {
namespace SignalProcessing {
typedef Common::Resampler::CResamplerSd CResampler;

class CBoxAlgorithmSignalResampling final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>, CResampler::ICallback
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processInput(const size_t index) override;
	bool process() override;

	// implementation for TResampler::ICallback
	void processResampler(const double* sample, const size_t nChannel) const override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_SignalResampling)

protected:

	Toolkit::TSignalDecoder<CBoxAlgorithmSignalResampling> m_decoder;
	mutable Toolkit::TSignalEncoder<CBoxAlgorithmSignalResampling> m_encoder;

	size_t m_oSampling = 0;
	size_t m_oNSample  = 0;

	int m_nFractionalDelayFilterSample = 0;
	double m_transitionBandPercent     = 0;
	double m_stopBandAttenuation       = 0;

	size_t m_iSampling              = 0;
	mutable uint64_t m_oTotalSample = 0;
	CResampler m_resampler;
	Kernel::IBoxIO* m_boxContext = nullptr;
};

class CBoxAlgorithmSignalResamplingDesc final : public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return CString("Signal Resampling"); }
	CString getAuthorName() const override { return CString("Quentin Barthelemy"); }
	CString getAuthorCompanyName() const override { return CString("Mensia Technologies SA"); }

	CString getShortDescription() const override { return CString("Resamples and re-epochs input signal to chosen sampling frequency"); }

	CString getDetailedDescription() const override
	{
		return CString("The input signal is resampled, down-sampled or up-sampled, at a chosen sampling frequency and then re-epoched.");
	}

	CString getCategory() const override { return CString("Signal processing/Temporal Filtering"); }
	CString getVersion() const override { return CString("2.0"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_SignalResampling; }
	IPluginObject* create() override { return new CBoxAlgorithmSignalResampling; }

	bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
	{
		prototype.addInput("Input signal", OV_TypeId_Signal);
		prototype.addOutput("Output signal", OV_TypeId_Signal);
		prototype.addSetting("New Sampling Frequency", OV_TypeId_Integer, "128", false,
							 OVP_ClassId_BoxAlgorithm_SignalResampling_SettingId_NewSampling);
		prototype.addSetting("Sample Count Per Buffer", OV_TypeId_Integer, "8", false,
							 OVP_ClassId_BoxAlgorithm_SignalResampling_SettingId_SampleCountPerBuffer);
		prototype.addSetting("Low Pass Filter Signal Before Downsampling", OV_TypeId_Boolean, "true", false,
							 OVP_ClassId_BoxAlgorithm_SignalResampling_SettingId_LowPassFilterSignalFlag); // displayed for backward compatibility, but never used

		//prototype.addSetting("New Sampling Frequency",OV_TypeId_Integer,"128");
		//prototype.addSetting("Sample Count Per Buffer",OV_TypeId_Integer,"8");
		//prototype.addSetting("Low Pass Filter Signal Before Downsampling", OV_TypeId_Boolean, "true"); // displayed for backward compatibility, but never used

		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SignalResamplingDesc)
};
}  // namespace SignalProcessing
}  // namespace Plugins
}  // namespace OpenViBE
