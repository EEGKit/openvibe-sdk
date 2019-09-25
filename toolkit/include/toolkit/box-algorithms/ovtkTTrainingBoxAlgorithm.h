#pragma once

#include "ovtkTBoxAlgorithm.h"

#include "../training/ovtkISignalTrial.h"
#include "../training/ovtkISignalTrialSet.h"

#include <ebml/CReader.h>

#include <vector>

namespace OpenViBEToolkit
{
	template <class CBoxAlgorithmParentClass>
	class TTrainingBoxAlgorithm : public TBoxAlgorithm<CBoxAlgorithmParentClass>
	{
	public:

		TTrainingBoxAlgorithm();
		virtual ~TTrainingBoxAlgorithm();

		// Signal input reader callback
		virtual void setChannelCount(uint32_t count) { m_pPendingSignal->setChannelCount(count); }
		virtual void setChannelName(uint32_t index, const char* name) { m_pPendingSignal->setChannelName(index, name); }
		virtual void setSampleCountPerBuffer(uint32_t count) { m_ui32SampleCountPerBuffer = count; }
		virtual void setSamplingRate(uint32_t samplingRate) { m_pPendingSignal->setSamplingRate(samplingRate); }
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

		_IsDerivedFromClass_(OpenViBEToolkit::TBoxAlgorithm<CBoxAlgorithmParentClass>, OVTK_ClassId_)

	private:

		ISignalTrial* m_pPendingSignal = nullptr;

		uint64_t m_ui64TrialStartTime       = 0;
		uint64_t m_ui64TrialEndTime         = 0;
		uint32_t m_ui32SampleCountPerBuffer = 0;
		OpenViBE::CIdentifier m_oTrialLabel = OV_UndefinedIdentifier;

		std::vector<ISignalTrial*> m_vSignalTrial;
	};
} // namespace OpenViBEToolkit

// ________________________________________________________________________________________________________________
//

namespace OpenViBEToolkit
{

	// ________________________________________________________________________________________________________________
	//

#define _no_time_ 0xffffffffffffffffLL

	template <class CBoxAlgorithmParentClass>
	TTrainingBoxAlgorithm<CBoxAlgorithmParentClass>::TTrainingBoxAlgorithm() : m_ui64TrialStartTime(_no_time_), m_ui64TrialEndTime(_no_time_)
	{
		m_pPendingSignal = createSignalTrial();
	}

	template <class CBoxAlgorithmParentClass>
	TTrainingBoxAlgorithm<CBoxAlgorithmParentClass>::~TTrainingBoxAlgorithm()
	{
		releaseSignalTrial(m_pPendingSignal);
		for (auto itSignalTrial = m_vSignalTrial.begin(); itSignalTrial != m_vSignalTrial.end(); ++itSignalTrial)
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
		insertBufferSamples(*m_pPendingSignal, m_pPendingSignal->getSampleCount(), m_ui32SampleCountPerBuffer, buffer, m_pPendingSignal);

		this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << OpenViBE::Kernel::LogLevel_Debug
				<< "Appended " << m_ui32SampleCountPerBuffer << " bytes resulting in " << (m_pPendingSignal->getDuration() >> 32) << " seconds of signal\n";
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
			for (auto it = m_vSignalTrial.begin(); it != m_vSignalTrial.end(); ++it) { signalTrialSet->addSignalTrial(**it); }

			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << OpenViBE::Kernel::LogLevel_Info << "Calling train function...\n";

			this->train(*signalTrialSet);

			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << OpenViBE::Kernel::LogLevel_Trace
					<< "Training done... will clear signal trials and signal trial set now...\n";

			for (auto it = m_vSignalTrial.begin(); it != m_vSignalTrial.end(); ++it) { releaseSignalTrial(*it); }
			releaseSignalTrialSet(signalTrialSet);
			m_vSignalTrial.clear();

			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << OpenViBE::Kernel::LogLevel_Info << "Training phase finished !\n";
		}
		else if (identifier == this->getStimulationIdentifierTrialStart())
		{
			m_ui64TrialStartTime = date;
			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << OpenViBE::Kernel::LogLevel_Trace
					<< "Saved trial start time " << OpenViBE::time64(m_ui64TrialStartTime) << "...\n";
		}
		else if (identifier == this->getStimulationIdentifierTrialEnd())
		{
			m_ui64TrialEndTime = date;
			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << OpenViBE::Kernel::LogLevel_Trace
					<< "Saved trial end time " << OpenViBE::time64(m_ui64TrialEndTime) << "...\n";
		}
		else if (this->getStimulationIdentifierTrialLabelRangeStart() <= OpenViBE::CIdentifier(identifier) && OpenViBE::CIdentifier(identifier) <= this->
				 getStimulationIdentifierTrialLabelRangeEnd())
		{
			m_oTrialLabel = identifier;
			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << OpenViBE::Kernel::LogLevel_Trace
					<< "Labeled trial " << m_oTrialLabel << "...\n";
		}
		else
		{
			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << OpenViBE::Kernel::LogLevel_Debug
					<< "Unhandled stimulation " << OpenViBE::CIdentifier(identifier) << " at time " << OpenViBE::time64(date) << "\n";
		}

		if (m_ui64TrialEndTime != _no_time_ && m_ui64TrialStartTime != _no_time_ && m_ui64TrialEndTime > m_ui64TrialStartTime)
		{
			uint32_t nSample = uint32_t(((m_ui64TrialEndTime - m_ui64TrialStartTime) * m_pPendingSignal->getSamplingRate()) >> 32);

			this->getBoxAlgorithmContext()->getPlayerContext()->getLogManager() << OpenViBE::Kernel::LogLevel_Trace
					<< "Created trial " << OpenViBE::time64(m_ui64TrialStartTime) << "-" << OpenViBE::time64(m_ui64TrialEndTime) << " with " << nSample << " samples\n";

			ISignalTrial* signalTrial = createSignalTrial();
			copyHeader(*signalTrial, m_pPendingSignal);
			selectTime(*signalTrial, m_ui64TrialStartTime, m_ui64TrialEndTime, m_pPendingSignal);
			signalTrial->setLabelIdentifier(m_oTrialLabel);

			m_vSignalTrial.push_back(signalTrial);

			m_ui64TrialStartTime = _no_time_;
			m_ui64TrialEndTime   = _no_time_;
			m_oTrialLabel        = OV_UndefinedIdentifier;
		}
	}

	// ________________________________________________________________________________________________________________
	//
} // namespace OpenViBEToolkit
