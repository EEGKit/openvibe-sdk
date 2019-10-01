#include "ovtkCSignalTrial.hpp"

#include <system/ovCMemory.h>

using namespace OpenViBE;
using namespace OpenViBEToolkit;
using namespace std;

// ________________________________________________________________________________________________________________
//

CSignalTrial::CSignalTrial() {}

CSignalTrial::~CSignalTrial()
{
	for (auto itChannel = m_channelSamples.begin(); itChannel != m_channelSamples.end(); ++itChannel)
	{
		delete [] itChannel->second;
	}
}

// ________________________________________________________________________________________________________________
//

bool CSignalTrial::setSamplingRate(const uint32_t samplingFrequency)
{
	m_samplingRate = samplingFrequency;
	return m_samplingRate != 0;
}

bool CSignalTrial::setChannelCount(const uint32_t count)
{
	uint32_t i;
	for (i = 0; i < count; i++)
	{
		if (m_channelSamples.find(i) == m_channelSamples.end()) { m_channelSamples[i] = new double[m_nSampleReserved]; }
	}
	for (i = count; i < m_nChannel; i++)
	{
		delete [] m_channelSamples[i];
		m_channelSamples.erase(m_channelSamples.find(i));
	}

	m_nChannel = count;
	m_nSample  = 0;
	m_channelNames.clear();
	return m_nChannel != 0;
}

bool CSignalTrial::setChannelName(const uint32_t index, const char* name)
{
	if (index < m_nChannel)
	{
		m_channelNames[index] = name;
		return true;
	}
	return false;
}

bool CSignalTrial::setLabelIdentifier(const CIdentifier& labelID)
{
	m_labelID = labelID;
	return true;
}

bool CSignalTrial::setSampleCount(const uint32_t count, const bool preserve)
{
	const uint32_t nSampleRounding = 0x00000fff;

	if (count > m_nSampleReserved)
	{
		const uint32_t nSampleReserved = (count + nSampleRounding + 1) & (~nSampleRounding);
		for (auto itChannelSample = m_channelSamples.begin(); itChannelSample != m_channelSamples.end(); ++itChannelSample)
		{
			double* sample = new double[nSampleReserved];
			if (preserve)
			{
				System::Memory::copy(sample, itChannelSample->second, (count < m_nSample ? count : m_nSample) * sizeof(double));
			}
			delete [] itChannelSample->second;
			itChannelSample->second = sample;
		}
		m_nSampleReserved = nSampleReserved;
	}
	m_nSample = count;
	return true;
}

// ________________________________________________________________________________________________________________
//

const char* CSignalTrial::getChannelName(const uint32_t index) const
{
	const auto itChannelName = m_channelNames.find(index);
	if (itChannelName != m_channelNames.end()) { return itChannelName->second.c_str(); }
	return "";
}

double* CSignalTrial::getChannelSampleBuffer(const uint32_t index) const
{
	const auto itChannelSample = m_channelSamples.find(index);
	if (itChannelSample != m_channelSamples.end()) { return itChannelSample->second; }
	return nullptr;
}

// ________________________________________________________________________________________________________________
//

ISignalTrial* OpenViBEToolkit::createSignalTrial() { return new CSignalTrial(); }

void OpenViBEToolkit::releaseSignalTrial(ISignalTrial* trial) { delete trial; }
