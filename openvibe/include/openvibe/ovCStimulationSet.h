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
		CStimulationSet();
		/**
		 * \brief Destructor
		 *
		 * The internal implementation is released.
		 */
		~CStimulationSet() override;

		//@}
		void clear() override;
		const uint64_t getStimulationCount() const override;
		const uint64_t getStimulationIdentifier(uint64_t ui64StimulationIndex) const override;
		const uint64_t getStimulationDate(uint64_t ui64StimulationIndex) const override;
		const uint64_t getStimulationDuration(uint64_t ui64StimulationIndex) const override;
		bool setStimulationCount(uint64_t ui64StimulationCount) override;
		bool setStimulationIdentifier(uint64_t ui64StimulationIndex, uint64_t ui64StimulationIdentifier) override;
		bool setStimulationDate(uint64_t ui64StimulationIndex, uint64_t ui64StimulationDate) override;
		bool setStimulationDuration(uint64_t ui64StimulationIndex, uint64_t ui64StimulationDuration) override;
		uint64_t appendStimulation(uint64_t ui64StimulationIdentifier, uint64_t ui64StimulationDate, uint64_t ui64StimulationDuration) override;
		uint64_t insertStimulation(uint64_t ui64StimulationIndex, uint64_t ui64StimulationIdentifier, uint64_t ui64StimulationDate, uint64_t ui64StimulationDuration) override;
		bool removeStimulation(uint64_t ui64StimulationIndex) override;

		_IsDerivedFromClass_Final_(OpenViBE::IStimulationSet, OV_ClassId_StimulationSetBridge)

	private:

		IStimulationSet* m_pStimulationSetImpl; //!< Internal implementation
	};
}  // namespace OpenViBE
