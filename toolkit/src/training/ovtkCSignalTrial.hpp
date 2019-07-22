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
		virtual ~CSignalTrial();

		virtual bool setSamplingRate(uint32_t ui32SamplingFrequency);
		virtual bool setChannelCount(uint32_t ui32ChannelCount);
		virtual bool setChannelName(uint32_t ui32ChannelIndex, const char* sChannelName);
		virtual bool setLabelIdentifier(const OpenViBE::CIdentifier& rLabelIdentifier);
		virtual bool setSampleCount(uint32_t ui32SampleCount, bool bPreserve);

		virtual uint32_t getSamplingRate() const;
		virtual uint32_t getChannelCount() const;
		virtual const char* getChannelName(uint32_t ui32ChannelIndex) const;
		virtual OpenViBE::CIdentifier getLabelIdentifier() const;
		virtual uint32_t getSampleCount() const;
		virtual uint64_t getDuration() const;
		virtual double* getChannelSampleBuffer(uint32_t ui32ChannelIndex) const;

		_IsDerivedFromClass_Final_(OpenViBEToolkit::ISignalTrial, OVTK_ClassId_)

	protected:

		std::map<uint32_t, std::string> m_vChannelName;
		std::map<uint32_t, double*> m_vChannelSample;
		uint32_t m_ui32ChannelCount;
		uint32_t m_ui32SampleCount;
		uint32_t m_ui32SampleCountReserved;
		uint32_t m_ui32SamplingRate;
		OpenViBE::CIdentifier m_oLabelIdentifier;
	};
};
