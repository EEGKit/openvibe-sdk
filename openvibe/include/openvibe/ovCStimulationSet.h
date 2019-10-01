#pragma once

#include "ovIStimulationSet.h"

namespace OpenViBE
{
	/**
	 * \class CStimulationSet
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2007-11-21
	 * \brief Basic standalone OpenViBE stimulation set implementation
	 * \ingroup Group_Base
	 *
	 * This class offers a basic standalone impementation of the OpenViBE::IStimulationSet
	 * interface. This class can be directly instanciated and used.
	 *
	 * Instances of this class use an internal implementation of the OpenViBE::IStimulationSet
	 * interface and redirect their calls to this implementation.
	 */
	class OV_API CStimulationSet final : public IStimulationSet
	{
	public:

		/** \name Constructors */
		//@{

		/**
		 * \brief Default constructor
		 *
		 * This constructor builds the internal implementation of this stimulation set.
		 */
		CStimulationSet();
		/**
		 * \brief Destructor
		 *
		 * The internal implementation is released.
		 */
		~CStimulationSet() override { delete m_stimulationSet; }

		//@}
		void clear() override { m_stimulationSet->clear(); }
		size_t getStimulationCount() const override { return m_stimulationSet->getStimulationCount(); }
		uint64_t getStimulationIdentifier(const uint64_t index) const override { return m_stimulationSet->getStimulationIdentifier(index); }
		uint64_t getStimulationDate(const uint64_t index) const override { return m_stimulationSet->getStimulationDate(index); }
		uint64_t getStimulationDuration(const uint64_t index) const override { return m_stimulationSet->getStimulationDuration(index); }
		bool setStimulationCount(const uint64_t n) override { return m_stimulationSet->setStimulationCount(n); }

		bool setStimulationIdentifier(const uint64_t index, const uint64_t identifier) override
		{
			return m_stimulationSet->setStimulationIdentifier(index, identifier);
		}

		bool setStimulationDate(const uint64_t index, const uint64_t date) override { return m_stimulationSet->setStimulationDate(index, date); }

		bool setStimulationDuration(const uint64_t index, const uint64_t duration) override
		{
			return m_stimulationSet->setStimulationDuration(index, duration);
		}

		uint64_t appendStimulation(const uint64_t identifier, const uint64_t date, const uint64_t duration) override
		{
			return m_stimulationSet->appendStimulation(identifier, date, duration);
		}

		uint64_t insertStimulation(const uint64_t index, const uint64_t identifier, const uint64_t date, const uint64_t duration) override
		{
			return m_stimulationSet->insertStimulation(index, identifier, date, duration);
		}

		bool removeStimulation(const uint64_t index) override { return m_stimulationSet->removeStimulation(index); }

		_IsDerivedFromClass_Final_(OpenViBE::IStimulationSet, OV_ClassId_StimulationSetBridge)

	private:

		IStimulationSet* m_stimulationSet = nullptr; //!< Internal implementation
	};
} // namespace OpenViBE
