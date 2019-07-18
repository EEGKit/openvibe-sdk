#include "ovtkCSignalTrial.hpp"

#include <system/ovCMemory.h>

#include <openvibe/ovITimeArithmetics.h>

using namespace OpenViBE;
using namespace OpenViBEToolkit;
using namespace std;

// ________________________________________________________________________________________________________________
//

CSignalTrial::CSignalTrial(void)
	: m_ui32ChannelCount(0)
	  , m_ui32SampleCount(0)
	  , m_ui32SampleCountReserved(0)
	  , m_ui32SamplingRate(0) {}

CSignalTrial::~CSignalTrial(void)
{
	map<uint32_t, double*>::iterator itChannel;
	for (itChannel = m_vChannelSample.begin(); itChannel != m_vChannelSample.end(); ++itChannel)
	{
		delete [] itChannel->second;
	}
}

// ________________________________________________________________________________________________________________
//

bool CSignalTrial::setSamplingRate(const uint32_t ui32SamplingFrequency)
{
	m_ui32SamplingRate = ui32SamplingFrequency;
	return m_ui32SamplingRate != 0;
}

bool CSignalTrial::setChannelCount(const uint32_t ui32ChannelCount)
{
	uint32_t i;
	for (i = 0; i < ui32ChannelCount; i++)
	{
		if (m_vChannelSample.find(i) == m_vChannelSample.end())
		{
			m_vChannelSample[i] = new double[m_ui32SampleCountReserved];
		}
	}
	for (i = ui32ChannelCount; i < m_ui32ChannelCount; i++)
	{
		delete [] m_vChannelSample[i];
		m_vChannelSample.erase(m_vChannelSample.find(i));
	}

	m_ui32ChannelCount = ui32ChannelCount;
	m_ui32SampleCount  = 0;
	m_vChannelName.clear();
	return m_ui32ChannelCount != 0;
}

bool CSignalTrial::setChannelName(const uint32_t ui32ChannelIndex, const char* sChannelName)
{
	if (ui32ChannelIndex < m_ui32ChannelCount)
	{
		m_vChannelName[ui32ChannelIndex] = sChannelName;
		return true;
	}
	return false;
}

bool CSignalTrial::setLabelIdentifier(const CIdentifier& rLabelIdentifier)
{
	m_oLabelIdentifier = rLabelIdentifier;
	return true;
}

bool CSignalTrial::setSampleCount(const uint32_t ui32SampleCount, const bool bPreserve)
{
	const uint32_t l_ui32SampleCountRounding = 0x00000fff;

	if (ui32SampleCount > m_ui32SampleCountReserved)
	{
		uint32_t l_ui32SampleCountReserved = (ui32SampleCount + l_ui32SampleCountRounding + 1) & (~l_ui32SampleCountRounding);
		map<uint32_t, double*>::iterator itChannelSample;
		for (itChannelSample = m_vChannelSample.begin(); itChannelSample != m_vChannelSample.end(); ++itChannelSample)
		{
			double* l_pSample = new double[l_ui32SampleCountReserved];
			if (bPreserve)
			{
				System::Memory::copy(l_pSample, itChannelSample->second, (ui32SampleCount < m_ui32SampleCount ? ui32SampleCount : m_ui32SampleCount) * sizeof(double));
			}
			delete [] itChannelSample->second;
			itChannelSample->second = l_pSample;
		}
		m_ui32SampleCountReserved = l_ui32SampleCountReserved;
	}
	m_ui32SampleCount = ui32SampleCount;
	return true;
}

// ________________________________________________________________________________________________________________
//

uint32_t CSignalTrial::getSamplingRate(void) const
{
	return m_ui32SamplingRate;
}

uint32_t CSignalTrial::getChannelCount(void) const
{
	return m_ui32ChannelCount;
}

const char* CSignalTrial::getChannelName(const uint32_t ui32ChannelIndex) const
{
	map<uint32_t, string>::const_iterator itChannelName;
	itChannelName = m_vChannelName.find(ui32ChannelIndex);
	if (itChannelName != m_vChannelName.end())
	{
		return itChannelName->second.c_str();
	}
	return "";
}

CIdentifier CSignalTrial::getLabelIdentifier(void) const
{
	return m_oLabelIdentifier;
}

uint32_t CSignalTrial::getSampleCount(void) const
{
	return m_ui32SampleCount;
}

uint64_t CSignalTrial::getDuration(void) const
{
	return (m_ui32SamplingRate ? ITimeArithmetics::sampleCountToTime(m_ui32SamplingRate, m_ui32SampleCount) : 0);
}

double* CSignalTrial::getChannelSampleBuffer(const uint32_t ui32ChannelIndex) const
{
	map<uint32_t, double*>::const_iterator itChannelSample;
	itChannelSample = m_vChannelSample.find(ui32ChannelIndex);
	if (itChannelSample != m_vChannelSample.end())
	{
		return itChannelSample->second;
	}
	return NULL;
}

// ________________________________________________________________________________________________________________
//

ISignalTrial* OpenViBEToolkit::createSignalTrial(void)
{
	return new CSignalTrial();
}

void OpenViBEToolkit::releaseSignalTrial(ISignalTrial* trial)
{
	delete trial;
}
