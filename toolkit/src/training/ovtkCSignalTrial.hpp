#ifndef __OpenViBEToolkit_Offline_CSignalTrial_HPP__
#define __OpenViBEToolkit_Offline_CSignalTrial_HPP__

#include "ovtkISignalTrial.h"

#include <map>
#include <string>

namespace OpenViBEToolkit
{
	class CSignalTrial : public OpenViBEToolkit::ISignalTrial
	{
	public:

		CSignalTrial(void);
		virtual ~CSignalTrial(void);

		virtual bool setSamplingRate(const OpenViBE::uint32 ui32SamplingFrequency);
		virtual bool setChannelCount(const OpenViBE::uint32 ui32ChannelCount);
		virtual bool setChannelName(const OpenViBE::uint32 ui32ChannelIndex, const char* sChannelName);
		virtual bool setLabelIdentifier(const OpenViBE::CIdentifier& rLabelIdentifier);
		virtual bool setSampleCount(const OpenViBE::uint32 ui32SampleCount, const bool bPreserve);

		virtual OpenViBE::uint32 getSamplingRate(void) const;
		virtual OpenViBE::uint32 getChannelCount(void) const;
		virtual const char* getChannelName(const OpenViBE::uint32 ui32ChannelIndex) const;
		virtual OpenViBE::CIdentifier getLabelIdentifier(void) const;
		virtual OpenViBE::uint32 getSampleCount(void) const;
		virtual OpenViBE::uint64 getDuration(void) const;
		virtual double* getChannelSampleBuffer(const OpenViBE::uint32 ui32ChannelIndex) const;

		_IsDerivedFromClass_Final_(OpenViBEToolkit::ISignalTrial, OVTK_ClassId_);

	protected:

		std::map<OpenViBE::uint32, std::string> m_vChannelName;
		std::map<OpenViBE::uint32, double*> m_vChannelSample;
		OpenViBE::uint32 m_ui32ChannelCount;
		OpenViBE::uint32 m_ui32SampleCount;
		OpenViBE::uint32 m_ui32SampleCountReserved;
		OpenViBE::uint32 m_ui32SamplingRate;
		OpenViBE::CIdentifier m_oLabelIdentifier;
	};
};

#endif // __OpenViBEToolkit_Offline_CSignalTrial_HPP__
