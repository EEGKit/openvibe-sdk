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
	class OV_API CStimulationSet : public IStimulationSet
	{
	public:

		/** \name Constructors */
		//@{

		/**
		 * \brief Default constructor
		 *
		 * This constructor builds the internal implementation of this stimulation set.
		 */
		CStimulationSet(void);
		/**
		 * \brief Destructor
		 *
		 * The internal implementation is released.
		 */
		virtual ~CStimulationSet(void);

		//@}

		virtual void clear(void);
		virtual const uint64_t getStimulationCount(void) const;
		virtual const uint64_t getStimulationIdentifier(
			uint64_t ui64StimulationIndex) const;
		virtual const uint64_t getStimulationDate(
			uint64_t ui64StimulationIndex) const;
		virtual const uint64_t getStimulationDuration(
			uint64_t ui64StimulationIndex) const;

		virtual bool setStimulationCount(
			uint64_t ui64StimulationCount);
		virtual bool setStimulationIdentifier(
			uint64_t ui64StimulationIndex,
			uint64_t ui64StimulationIdentifier);
		virtual bool setStimulationDate(
			uint64_t ui64StimulationIndex,
			uint64_t ui64StimulationDate);
		virtual bool setStimulationDuration(
			uint64_t ui64StimulationIndex,
			uint64_t ui64StimulationDuration);

		virtual uint64_t appendStimulation(
			uint64_t ui64StimulationIdentifier,
			uint64_t ui64StimulationDate,
			uint64_t ui64StimulationDuration);
		virtual uint64_t insertStimulation(
			uint64_t ui64StimulationIndex,
			uint64_t ui64StimulationIdentifier,
			uint64_t ui64StimulationDate,
			uint64_t ui64StimulationDuration);
		virtual bool removeStimulation(
			uint64_t ui64StimulationIndex);

		_IsDerivedFromClass_Final_(OpenViBE::IStimulationSet, OV_ClassId_StimulationSetBridge);

	private:

		IStimulationSet* m_pStimulationSetImpl; //!< Internal implementation
	};
};


