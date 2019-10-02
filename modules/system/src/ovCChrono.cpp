#include "system/ovCChrono.h"
#include "system/ovCTime.h"

using namespace System;

CChrono::CChrono() {}

CChrono::~CChrono()
{
	delete [] m_pStepInTime;
	delete [] m_pStepOutTime;
}

bool CChrono::reset(uint32_t ui32StepCount)
{
	if (!ui32StepCount) { return false; }

	uint64_t* stepInTime  = new uint64_t[ui32StepCount + 1];
	uint64_t* stepOutTime = new uint64_t[ui32StepCount + 1];
	if (!stepInTime || !stepOutTime)
	{
		delete [] stepInTime;
		delete [] stepOutTime;
		return false;
	}

	for (uint32_t i = 0; i <= ui32StepCount; i++)
	{
		stepInTime[i]  = 0;
		stepOutTime[i] = 0;
	}

	delete [] m_pStepInTime;
	delete [] m_pStepOutTime;
	m_pStepInTime  = stepInTime;
	m_pStepOutTime = stepOutTime;

	m_nStep     = ui32StepCount;
	m_ui32StepIndex     = 0;
	m_bIsInStep         = false;
	m_bHasNewEstimation = false;

	m_totalStepInTime  = 0;
	m_totalStepOutTime = 0;

	return true;
}

bool CChrono::stepIn()
{
	if (m_bIsInStep || !m_nStep) { return false; }

	m_bIsInStep = !m_bIsInStep;

	m_pStepInTime[m_ui32StepIndex] = Time::zgetTime();
	if (m_ui32StepIndex == m_nStep)
	{
		m_totalStepInTime  = 0;
		m_totalStepOutTime = 0;
		for (uint32_t i = 0; i < m_nStep; i++)
		{
			m_totalStepInTime += m_pStepOutTime[i] - m_pStepInTime[i];
			m_totalStepOutTime += m_pStepInTime[i + 1] - m_pStepOutTime[i];
		}
		m_pStepInTime[0]    = m_pStepInTime[m_nStep];
		m_ui32StepIndex     = 0;
		m_bHasNewEstimation = true;
	}
	else { m_bHasNewEstimation = false; }

	return true;
}

bool CChrono::stepOut()
{
	if (!m_bIsInStep || !m_nStep) { return false; }

	m_bIsInStep = !m_bIsInStep;

	m_pStepOutTime[m_ui32StepIndex] = Time::zgetTime();
	m_ui32StepIndex++;

	return true;
}

uint64_t CChrono::getTotalStepInDuration() const { return m_totalStepInTime; }

uint64_t CChrono::getTotalStepOutDuration() const { return m_totalStepOutTime; }

uint64_t CChrono::getAverageStepInDuration() const { return m_nStep ? this->getTotalStepInDuration() / m_nStep : 0; }

uint64_t CChrono::getAverageStepOutDuration() const { return m_nStep ? this->getTotalStepOutDuration() / m_nStep : 0; }

double CChrono::getStepInPercentage() const
{
	const uint64_t totalStepDuration = (this->getTotalStepInDuration() + this->getTotalStepOutDuration());
	return totalStepDuration ? (this->getTotalStepInDuration() * 100.0) / totalStepDuration : 0;
}

double CChrono::getStepOutPercentage() const
{
	const uint64_t totalStepDuration = (this->getTotalStepOutDuration() + this->getTotalStepInDuration());
	return totalStepDuration ? (this->getTotalStepOutDuration() * 100.0) / totalStepDuration : 0;
}

bool CChrono::hasNewEstimation() { return m_bHasNewEstimation; }
