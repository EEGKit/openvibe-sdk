#pragma once

#include "ovtkISignalTrial.h"
#include <openvibe/ovTimeArithmetics.h>

#include <map>
#include <string>

namespace OpenViBEToolkit
{
	class CSignalTrial final : public ISignalTrial
	{
	public:

		CSignalTrial();
		~CSignalTrial() override;
		bool setSamplingRate(uint32_t samplingFrequency) override;
		bool setChannelCount(uint32_t count) override;
		bool setChannelName(uint32_t index, const char* name) override;
		bool setLabelIdentifier(const OpenViBE::CIdentifier& labelID) override;
		bool setSampleCount(uint32_t count, bool preserve) override;
		uint32_t getSamplingRate() const override { return m_samplingRate; }
		uint32_t getChannelCount() const override { return m_nChannel; }
		const char* getChannelName(uint32_t index) const override;
		OpenViBE::CIdentifier getLabelIdentifier() const override { return m_labelID; }
		uint32_t getSampleCount() const override { return m_nSample; }
		uint64_t getDuration() const override { return (m_samplingRate ? OpenViBE::TimeArithmetics::sampleCountToTime(m_samplingRate, m_nSample) : 0); }
		double* getChannelSampleBuffer(uint32_t index) const override;

		_IsDerivedFromClass_Final_(OpenViBEToolkit::ISignalTrial, OVTK_ClassId_)

	protected:

		std::map<uint32_t, std::string> m_channelNames;
		std::map<uint32_t, double*> m_channelSamples;
		uint32_t m_nChannel             = 0;
		uint32_t m_nSample              = 0;
		uint32_t m_nSampleReserved      = 0;
		uint32_t m_samplingRate         = 0;
		OpenViBE::CIdentifier m_labelID = OV_UndefinedIdentifier;
	};
} // namespace OpenViBEToolkit
