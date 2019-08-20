#pragma once

#include "ovtkISignalTrial.h"

#include <map>
#include <string>

namespace OpenViBEToolkit
{
	class CSignalTrial : public ISignalTrial
	{
	public:

		CSignalTrial();
		~CSignalTrial() override;
		bool setSamplingRate(uint32_t ui32SamplingFrequency) override;
		bool setChannelCount(uint32_t ui32ChannelCount) override;
		bool setChannelName(uint32_t ui32ChannelIndex, const char* sChannelName) override;
		bool setLabelIdentifier(const OpenViBE::CIdentifier& rLabelIdentifier) override;
		bool setSampleCount(uint32_t ui32SampleCount, bool bPreserve) override;
		uint32_t getSamplingRate() const override;
		uint32_t getChannelCount() const override;
		const char* getChannelName(uint32_t ui32ChannelIndex) const override;
		OpenViBE::CIdentifier getLabelIdentifier() const override;
		uint32_t getSampleCount() const override;
		uint64_t getDuration() const override;
		double* getChannelSampleBuffer(uint32_t ui32ChannelIndex) const override;

		_IsDerivedFromClass_Final_(OpenViBEToolkit::ISignalTrial, OVTK_ClassId_)

	protected:

		std::map<uint32_t, std::string> m_vChannelName;
		std::map<uint32_t, double*> m_vChannelSample;
		uint32_t m_ui32ChannelCount              = 0;
		uint32_t m_ui32SampleCount               = 0;
		uint32_t m_ui32SampleCountReserved       = 0;
		uint32_t m_ui32SamplingRate              = 0;
		OpenViBE::CIdentifier m_oLabelIdentifier = OV_UndefinedIdentifier;
	};
}  // namespace OpenViBEToolkit
