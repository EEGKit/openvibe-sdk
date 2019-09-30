#pragma once

#include "ovtkTBoxAlgorithm.h"

#include "../training/ovtkISignalTrial.h"
#include "../training/ovtkISignalTrialSet.h"

#include <ebml/CReader.h>

#include <vector>
#include <climits>

namespace OpenViBEToolkit
{
	template <class TBoxAlgorithmParentClass>
	class TTrainingBoxAlgorithm : public TBoxAlgorithm<TBoxAlgorithmParentClass>
	{
	public:

		TTrainingBoxAlgorithm();
		virtual ~TTrainingBoxAlgorithm();

		// Signal input reader callback
		virtual void setChannelCount(const uint32_t count) { m_pendingSignal->setChannelCount(count); }
		virtual void setChannelName(const uint32_t index, const char* name) { m_pendingSignal->setChannelName(index, name); }
		virtual void setSampleCountPerBuffer(const uint32_t count) { m_nSamplePerBuffer = count; }
		virtual void setSamplingRate(const uint32_t samplingRate) { m_pendingSignal->setSamplingRate(samplingRate); }
		virtual void setSampleBuffer(const double* buffer);

		// Stimulation input reader callback
		virtual void setStimulationCount(uint32_t /*count*/) {}
		virtual void setStimulation(uint32_t index, uint64_t identifier, uint64_t date);

		// What should be implemented by the derived class
		virtual OpenViBE::CIdentifier getStimulationIdentifierTrialStart() = 0;
		virtual OpenViBE::CIdentifier getStimulationIdentifierTrialEnd() = 0;
		virtual OpenViBE::CIdentifier getStimulationIdentifierTrialLabelRangeStart() = 0;
		virtual OpenViBE::CIdentifier getStimulationIdentifierTrialLabelRangeEnd() = 0;
		virtual OpenViBE::CIdentifier getStimulationIdentifierTrain() = 0;
		virtual bool train(ISignalTrialSet& trialSet) = 0;

		_IsDerivedFromClass_(OpenViBEToolkit::TBoxAlgorithm<TBoxAlgorithmParentClass>, OVTK_ClassId_)

	private:

		ISignalTrial* m_pendingSignal = nullptr;

		uint64_t m_trialStartTime       = 0;
		uint64_t m_trialEndTime         = 0;
		uint32_t m_nSamplePerBuffer = 0;
		OpenViBE::CIdentifier m_trialLabel = OV_UndefinedIdentifier;

		std::vector<ISignalTrial*> m_signalTrials;
	};
} // namespace OpenViBEToolkit

// ________________________________________________________________________________________________________________
//

namespace OpenViBEToolkit
{

	// ________________________________________________________________________________________________________________
	//

	template <class TBoxAlgorithmParentClass>
	TTrainingBoxAlgorithm<TBoxAlgorithmParentClass>::TTrainingBoxAlgorithm() : m_trialStartTime(ULLONG_MAX), m_trialEndTime(ULLONG_MAX)
	{
		m_pendingSignal = createSignalTrial();
	}

	template <class CBoxAlgorithmParentClass>
	TTrainingBoxAlgorithm<CBoxAlgorithmParentClass>::~TTrainingBoxAlgorithm()
	{
		releaseSignalTrial(m_pendingSignal);
		for (auto itSignalTrial = m_signalTrials.begin(); itSignalTrial != m_signalTrials.end(); ++itSignalTrial)
		{
			releaseSignalTrial(*itSignalTrial);
		}
	}

	// ________________________________________________________________________________________________________________
	//
	// Signal input reader callback
	// ________________________________________________________________________________________________________________
	//

	template <class CBoxAlgorithmParentClass>
	void TTrainingBoxAlgorithm<CBoxAlgorithmParentClass>::setSampleBuffer(const double* buffer)
	{
		insertBufferSamples(*m_pendingSignal, m_pendingSignal->getSampleCount(), m_nSamplePerBuffer, buffer, m_pendingSignal);

		this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << OpenViBE::Kernel::LogLevel_Debug
				<< "Appended " << m_nSamplePerBuffer << " bytes resulting in " << (m_pendingSignal->getDuration() >> 32) << " seconds of signal\n";
	}

	// ________________________________________________________________________________________________________________
	//
	// Stimulation input reader callback
	// ________________________________________________________________________________________________________________
	//

	template <class CBoxAlgorithmParentClass>
	void TTrainingBoxAlgorithm<CBoxAlgorithmParentClass>::setStimulation(const uint32_t /*index*/, const uint64_t identifier, const uint64_t date)
	{
		if (identifier == this->getStimulationIdentifierTrain())
		{
			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager()
					<< OpenViBE::Kernel::LogLevel_Trace
					<< "Constituting a signal trial set based on previous signal trials...\n";

			ISignalTrialSet* signalTrialSet = createSignalTrialSet();
			for (auto it = m_signalTrials.begin(); it != m_signalTrials.end(); ++it) { signalTrialSet->addSignalTrial(**it); }

			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << OpenViBE::Kernel::LogLevel_Info << "Calling train function...\n";

			this->train(*signalTrialSet);

			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << OpenViBE::Kernel::LogLevel_Trace
					<< "Training done... will clear signal trials and signal trial set now...\n";

			for (auto it = m_signalTrials.begin(); it != m_signalTrials.end(); ++it) { releaseSignalTrial(*it); }
			releaseSignalTrialSet(signalTrialSet);
			m_signalTrials.clear();

			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << OpenViBE::Kernel::LogLevel_Info << "Training phase finished !\n";
		}
		else if (identifier == this->getStimulationIdentifierTrialStart())
		{
			m_trialStartTime = date;
			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << OpenViBE::Kernel::LogLevel_Trace
					<< "Saved trial start time " << OpenViBE::time64(m_trialStartTime) << "...\n";
		}
		else if (identifier == this->getStimulationIdentifierTrialEnd())
		{
			m_trialEndTime = date;
			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << OpenViBE::Kernel::LogLevel_Trace
					<< "Saved trial end time " << OpenViBE::time64(m_trialEndTime) << "...\n";
		}
		else if (this->getStimulationIdentifierTrialLabelRangeStart() <= OpenViBE::CIdentifier(identifier) && OpenViBE::CIdentifier(identifier) <= this->
				 getStimulationIdentifierTrialLabelRangeEnd())
		{
			m_trialLabel = identifier;
			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << OpenViBE::Kernel::LogLevel_Trace
					<< "Labeled trial " << m_trialLabel << "...\n";
		}
		else
		{
			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << OpenViBE::Kernel::LogLevel_Debug
					<< "Unhandled stimulation " << OpenViBE::CIdentifier(identifier) << " at time " << OpenViBE::time64(date) << "\n";
		}

		if (m_trialEndTime != ULLONG_MAX && m_trialStartTime != ULLONG_MAX && m_trialEndTime > m_trialStartTime)
		{
			uint32_t nSample = uint32_t(((m_trialEndTime - m_trialStartTime) * m_pendingSignal->getSamplingRate()) >> 32);

			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << OpenViBE::Kernel::LogLevel_Trace
					<< "Created trial " << OpenViBE::time64(m_trialStartTime) << "-" << OpenViBE::time64(m_trialEndTime) << " with " << nSample << " samples\n";

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
} // namespace OpenViBEToolkit
