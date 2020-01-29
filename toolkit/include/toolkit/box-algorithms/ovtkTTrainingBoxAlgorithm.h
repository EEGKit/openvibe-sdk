#pragma once

#include "ovtkTBoxAlgorithm.h"

#include "../training/ovtkISignalTrial.h"
#include "../training/ovtkISignalTrialSet.h"

#include <ebml/CReader.h>

#include <vector>
#include <climits>

namespace OpenViBE
{
	namespace Toolkit
	{
	template <class TBoxAlgorithmParentClass>
	class TTrainingBoxAlgorithm : public TBoxAlgorithm<TBoxAlgorithmParentClass>
	{
	public:

		TTrainingBoxAlgorithm();
		virtual ~TTrainingBoxAlgorithm();

		// Signal input reader callback
		virtual void setChannelCount(const size_t count) { m_pendingSignal->setChannelCount(count); }
		virtual void setChannelName(const size_t index, const char* name) { m_pendingSignal->setChannelName(index, name); }
		virtual void setSampleCountPerBuffer(const size_t count) { m_nSamplePerBuffer = count; }
		virtual void setSamplingRate(const size_t sampling) { m_pendingSignal->setSamplingRate(sampling); }
		virtual void setSampleBuffer(const double* buffer);

		// Stimulation input reader callback
		virtual void setStimulationCount(size_t /*count*/) {}
		virtual void setStimulation(const size_t index, uint64_t identifier, uint64_t date);

		// What should be implemented by the derived class
		virtual CIdentifier getStimulationIdentifierTrialStart() = 0;
		virtual CIdentifier getStimulationIdentifierTrialEnd() = 0;
		virtual CIdentifier getStimulationIdentifierTrialLabelRangeStart() = 0;
		virtual CIdentifier getStimulationIdentifierTrialLabelRangeEnd() = 0;
		virtual CIdentifier getStimulationIdentifierTrain() = 0;
		virtual bool train(ISignalTrialSet& trialSet) = 0;

		_IsDerivedFromClass_(TBoxAlgorithm<TBoxAlgorithmParentClass>, OVTK_ClassId_)

	private:

		ISignalTrial* m_pendingSignal = nullptr;

		uint64_t m_trialStartTime          = 0;
		uint64_t m_trialEndTime            = 0;
		size_t m_nSamplePerBuffer          = 0;
		CIdentifier m_trialLabel = OV_UndefinedIdentifier;

		std::vector<ISignalTrial*> m_signalTrials;
	};
	}  // namespace Toolkit
}  // namespace OpenViBE

// ________________________________________________________________________________________________________________
//

namespace OpenViBE
{
	namespace Toolkit
	{

	// ________________________________________________________________________________________________________________
	//

	template <class TBoxAlgorithmParentClass>
	TTrainingBoxAlgorithm<TBoxAlgorithmParentClass>::TTrainingBoxAlgorithm() : m_trialStartTime(ULLONG_MAX), m_trialEndTime(ULLONG_MAX)
	{
		m_pendingSignal = createSignalTrial();
	}

	template <class TBoxAlgorithmParentClass>
	TTrainingBoxAlgorithm<TBoxAlgorithmParentClass>::~TTrainingBoxAlgorithm()
	{
		releaseSignalTrial(m_pendingSignal);
		for (auto itSignalTrial = m_signalTrials.begin(); itSignalTrial != m_signalTrials.end(); ++itSignalTrial) { releaseSignalTrial(*itSignalTrial); }
	}

	// ________________________________________________________________________________________________________________
	//
	// Signal input reader callback
	// ________________________________________________________________________________________________________________
	//

	template <class TBoxAlgorithmParentClass>
	void TTrainingBoxAlgorithm<TBoxAlgorithmParentClass>::setSampleBuffer(const double* buffer)
	{
		insertBufferSamples(*m_pendingSignal, m_pendingSignal->getSampleCount(), m_nSamplePerBuffer, buffer, m_pendingSignal);

		this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << Kernel::LogLevel_Debug
				<< "Appended " << m_nSamplePerBuffer << " bytes resulting in " << (m_pendingSignal->getDuration() >> 32) << " seconds of signal\n";
	}

	// ________________________________________________________________________________________________________________
	//
	// Stimulation input reader callback
	// ________________________________________________________________________________________________________________
	//

	template <class TBoxAlgorithmParentClass>
	void TTrainingBoxAlgorithm<TBoxAlgorithmParentClass>::setStimulation(const size_t /*index*/, const uint64_t identifier, const uint64_t date)
	{
		if (identifier == this->getStimulationIdentifierTrain())
		{
			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager()
					<< Kernel::LogLevel_Trace
					<< "Constituting a signal trial set based on previous signal trials...\n";

			ISignalTrialSet* signalTrialSet = createSignalTrialSet();
			for (auto it = m_signalTrials.begin(); it != m_signalTrials.end(); ++it) { signalTrialSet->addSignalTrial(**it); }

			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << Kernel::LogLevel_Info << "Calling train function...\n";

			this->train(*signalTrialSet);

			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << Kernel::LogLevel_Trace
					<< "Training done... will clear signal trials and signal trial set now...\n";

			for (auto it = m_signalTrials.begin(); it != m_signalTrials.end(); ++it) { releaseSignalTrial(*it); }
			releaseSignalTrialSet(signalTrialSet);
			m_signalTrials.clear();

			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << Kernel::LogLevel_Info << "Training phase finished !\n";
		}
		else if (identifier == this->getStimulationIdentifierTrialStart())
		{
			m_trialStartTime = date;
			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << Kernel::LogLevel_Trace
					<< "Saved trial start time " << time64(m_trialStartTime) << "...\n";
		}
		else if (identifier == this->getStimulationIdentifierTrialEnd())
		{
			m_trialEndTime = date;
			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << Kernel::LogLevel_Trace
					<< "Saved trial end time " << time64(m_trialEndTime) << "...\n";
		}
		else if (this->getStimulationIdentifierTrialLabelRangeStart() <= CIdentifier(identifier) && CIdentifier(identifier) <= this->
				 getStimulationIdentifierTrialLabelRangeEnd())
		{
			m_trialLabel = identifier;
			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << Kernel::LogLevel_Trace
					<< "Labeled trial " << m_trialLabel << "...\n";
		}
		else
		{
			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << Kernel::LogLevel_Debug
					<< "Unhandled stimulation " << CIdentifier(identifier) << " at time " << time64(date) << "\n";
		}

		if (m_trialEndTime != ULLONG_MAX && m_trialStartTime != ULLONG_MAX && m_trialEndTime > m_trialStartTime)
		{
			size_t nSample = size_t(((m_trialEndTime - m_trialStartTime) * m_pendingSignal->getSamplingRate()) >> 32);

			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << Kernel::LogLevel_Trace
					<< "Created trial " << time64(m_trialStartTime) << "-" << time64(m_trialEndTime) << " with " << nSample << " samples\n";

			ISignalTrial* signalTrial = createSignalTrial();
			copyHeader(*signalTrial, m_pendingSignal);
			selectTime(*signalTrial, m_trialStartTime, m_trialEndTime, m_pendingSignal);
			signalTrial->setLabelIdentifier(m_trialLabel);

			m_signalTrials.push_back(signalTrial);

			m_trialStartTime = ULLONG_MAX;
			m_trialEndTime   = ULLONG_MAX;
			m_trialLabel     = OV_UndefinedIdentifier;
		}
	}

	// ________________________________________________________________________________________________________________
	//
	}  // namespace Toolkit
}  // namespace OpenViBE
