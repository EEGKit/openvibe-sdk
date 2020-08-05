#pragma once

#include "ovIObject.h"

namespace OpenViBE
{
	/**
	 * \class IStimulationSet
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2007-11-21
	 * \brief Basic OpenViBE stimulation set interface
	 *
	 * This interface offers functionalities to handle a collection of OpenViBE stimulations.
	 * This collection basicaly consists in a list of stimulation information. Each stimulation
	 * has three information : an identifier, a date and a duration.
	 *
	 * OpenViBE provides an standalone implementation of this interface in CStimulationSet
	 */

	class OV_API IStimulationSet : public IObject
	{
	public:

		/**
		 * \brief Clears this stimulation set removing every existing stimulation
		 */
		virtual void clear() = 0;
		/**
		 * \brief Gets the number of stimulations contained in this stimulation set
		 * \return the number of stimulations contained in this stimulation set.
		 */
		virtual size_t getStimulationCount() const = 0;
		/**
		 * \brief Gets the identifier of a specific stimulation in this stimulation set
		 * \param index [in] : the index of the stimulation which identifier has to be returned
		 * \return the identifier for the specified stimulation.
		 */
		virtual uint64_t getStimulationIdentifier(const size_t index) const = 0;
		/**
		 * \brief Gets the date of a specific stimulation in this stimulation set
		 * \param index [in] : the index of the stimulation which date has to be returned
		 * \return the date for the specified stimulation.
		 * \note the returned date is relative to the beginning of this stimulation set
		 * \note dates and durations are returned in seconds fixed point 32:32. Refer to \Doc_TimeManagement for more details
		 */
		virtual uint64_t getStimulationDate(const size_t index) const = 0;
		/**
		 * \brief Gets the duration of a specific stimulation in this stimulation set
		 * \param index [in] : the index of the stimulation which duration has to be returned
		 * \return the duration for the specified stimulation.
		 * \note dates and durations are returned in seconds fixed point 32:32. Refer to \Doc_TimeManagement for more details
		 */
		virtual uint64_t getStimulationDuration(const size_t index) const = 0;
		/**
		 * \brief Changes the stimulation count in this stimulation set
		 * \param n [in] : the new number of stimulations
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 */
		virtual bool setStimulationCount(const size_t n) = 0;
		/**
		 * \brief Changes the identifier of a specific stimulation in this stimulation set
		 * \param index [in] : the index of the stimulation which id should be changed
		 * \param id [in] : the new id for the specified stimulation
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 */
		virtual bool setStimulationIdentifier(const size_t index, const uint64_t id) = 0;
		/**
		 * \brief Changes the date of a specific stimulation in this stimulation set
		 * \param index [in] : the index of the stimulation which date should be changed
		 * \param date [in] : the new date for the specified stimulation
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 * \note dates and durations are returned in seconds fixed point 32:32. Refer to \Doc_TimeManagement for more details
		 */
		virtual bool setStimulationDate(const size_t index, const uint64_t date) = 0;
		/**
		 * \brief Changes the duration of a specific stimulation in this stimulation set
		 * \param index [in] : the index of the stimulation which duration should be changed
		 * \param duration [in] : the new duration for the specified stimulation
		 * \return \e true in case of success.
		 * \return \e false in case of error.
		 * \note the returned date is relative to the beginning of this stimulation set
		 * \note dates and durations are returned in seconds fixed point 32:32. Refer to \Doc_TimeManagement for more details
		 */
		virtual bool setStimulationDuration(const size_t index, const uint64_t duration) = 0;
		/**
		 * \brief Appends a stimulation to this stimulation set
		 * \param id [in] : the identifier of the stimulation to append
		 * \param date [in] : the date of the stimulation
		 * \param duration [in] : the duration of the stimulation
		 * \return the index of the appended stimulation in this stimulation set
		 */
		virtual size_t appendStimulation(const uint64_t id, const uint64_t date, const uint64_t duration) = 0;
		/**
		 * \brief Inserts a stimulation to this stimulation set
		 * \param index [in] : the index of the stimulation to insert
		 * \param id [in] : the identifier of the stimulation
		 * \param date [in] : the date of the stimulation
		 * \param duration [in] : the duration of the stimulation
		 * \return \e true in case of success
		 * \return \e false in case of error
		 * \warning stimulation indexing change after call to this function :
		 *          following stimulation(s) get one more indexed.
		 */
		virtual size_t insertStimulation(const size_t index, const uint64_t id, const uint64_t date, const uint64_t duration) = 0;
		/**
		 * \brief Removes a stimulation from this stimulation set
		 * \param index [in] : the index of the stimulation to remove
		 * \return \e true in case of success
		 * \return \e false in case of error
		 * \warning stimulation indexing change after call to this function :
		 *          following stimulation(s) get one less indexed.
		 */
		virtual bool removeStimulation(const size_t index) = 0;

		_IsDerivedFromClass_(IObject, OV_ClassId_StimulationSet)
	};
} // namespace OpenViBE