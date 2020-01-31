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
	 * This class offers a basic standalone impementation of the IStimulationSet
	 * interface. This class can be directly instanciated and used.
	 *
	 * Instances of this class use an internal implementation of the IStimulationSet
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
		~CStimulationSet() override { delete m_stimSet; }

		//@}
		void clear() override { m_stimSet->clear(); }
		size_t getStimulationCount() const override { return m_stimSet->getStimulationCount(); }
		uint64_t getStimulationIdentifier(const size_t index) const override { return m_stimSet->getStimulationIdentifier(index); }
		uint64_t getStimulationDate(const size_t index) const override { return m_stimSet->getStimulationDate(index); }
		uint64_t getStimulationDuration(const size_t index) const override { return m_stimSet->getStimulationDuration(index); }
		bool setStimulationCount(const size_t n) override { return m_stimSet->setStimulationCount(n); }
		bool setStimulationIdentifier(const size_t index, const uint64_t id) override { return m_stimSet->setStimulationIdentifier(index, id); }
		bool setStimulationDate(const size_t index, const uint64_t date) override { return m_stimSet->setStimulationDate(index, date); }
		bool setStimulationDuration(const size_t index, const uint64_t duration) override { return m_stimSet->setStimulationDuration(index, duration); }

		size_t appendStimulation(const uint64_t id, const uint64_t date, const uint64_t duration) override
		{
			return m_stimSet->appendStimulation(id, date, duration);
		}

		size_t insertStimulation(const size_t index, const uint64_t id, const uint64_t date, const uint64_t duration) override
		{
			return m_stimSet->insertStimulation(index, id, date, duration);
		}

		bool removeStimulation(const size_t index) override { return m_stimSet->removeStimulation(index); }

		_IsDerivedFromClass_Final_(IStimulationSet, OV_ClassId_StimulationSetBridge)

	private:

		IStimulationSet* m_stimSet = nullptr; //!< Internal implementation
	};
} // namespace OpenViBE
