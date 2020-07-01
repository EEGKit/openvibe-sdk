///-------------------------------------------------------------------------------------------------
/// 
/// \file CStimulationSet.hpp
/// \brief StimulationSet Class for OpenViBE.
/// \author Thibaut Monseigne (Inria).
/// \version 1.0.
/// \date 25/05/2020.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/">GNU Affero General Public License v3.0</a>.
/// 
///-------------------------------------------------------------------------------------------------
#pragma once

#include "CStimulation.hpp"
#include <vector>
#include <iostream>

//typedef std::vector<CStimulation> CStimulationSet; // Why not ?
namespace OpenViBE {
/// <summary> Basic standalone OpenViBE stimulation set implementation. </summary>
class OV_API CStimulationSet
{
public:

	//--------------------------------------------------
	//------------ Constructor / Destructor ------------
	//--------------------------------------------------
	CStimulationSet() { m_stimulations = new std::vector<CStimulation>; }	///< Default constructor
	~CStimulationSet() { delete m_stimulations; }							///< Default destructor

	//--------------------------------------------------
	//------------- Basic Vector Functions -------------
	//--------------------------------------------------
	/// <summary> Clears the vector. </summary>
	void clear() const { m_stimulations->clear(); }
	/// <summary> Size of the vector. </summary>
	/// <returns> Size of the vector. </returns>
	size_t size() const { return m_stimulations->size(); }
	/// <summary> Resizes the vector. </summary>
	/// <param name="n"> The size. </param>
	void resize(const size_t n) const { m_stimulations->resize(n); }
	/// <summary> Getter for the object on position n. </summary>
	/// <param name="n"> The index of the object. </param>
	/// <returns> Reference of the object. </returns>
	CStimulation& at(const size_t n) const { return m_stimulations->at(n); }

	/// <summary> First object Iterator (usefull for modern forloop). </summary>
	/// <returns> Iterator. </returns>
	std::vector<CStimulation>::iterator begin() { return m_stimulations->begin(); }
	/// <summary> End of vector Iterator (usefull for modern for loop). </summary>
	/// <returns> Iterator. </returns>
	std::vector<CStimulation>::iterator end() { return m_stimulations->end(); }
	/// <summary> First object Iterator (usefull for modern for loop). </summary>
	/// <returns> Iterator. </returns>
	std::vector<CStimulation>::const_iterator begin() const { return m_stimulations->cbegin(); }
	/// <summary> End of vector Iterator (usefull for modern for loop). </summary>
	/// <returns> Iterator. </returns>
	std::vector<CStimulation>::const_iterator end() const { return m_stimulations->cend(); }

	/// --------------------------------------------------
	/// ---------------- Special Functions ---------------
	/// --------------------------------------------------
	
	/// <summary> Appends the specified stimulation. </summary>
	/// <param name="stim"> The stimulation. </param>
	void append(const CStimulation& stim) const { m_stimulations->push_back(stim); }
	
	/// <summary> Appends the specified stimulation. </summary>
	/// <param name="id"> The identifier. </param>
	/// <param name="date"> The date. </param>
	/// <param name="duration"> The duration. </param>
	void append(const uint64_t id, const CTime& date, const CTime& duration) const { append(CStimulation(id, date, duration)); }

	/// <summary> Add source on destination and add a time shift to each (new) stimulation. </summary>
	/// <param name="set"> StimulationSet to append. </param>
	/// <param name="shift"> The time shift. </param>
	void append(const CStimulationSet& set, const CTime& shift = 0) const { for (const auto& s : set) { append(s.m_ID, s.m_Date + shift, s.m_Duration); } }

	/// <summary> Add source on destination limited by start and end times and add a time shift to each (new) stimulation. </summary>
	/// <param name="set"> StimulationSet to append. </param>
	/// <param name="startTime"> The start time. </param>
	/// <param name="endTime"> The end time. </param>
	/// <param name="shift"> The time shift. </param>
	void appendRange(const CStimulationSet& set, const CTime& startTime, const CTime& endTime, const CTime& shift = 0) const
	{
		for (const auto& s : set)
		{
			const CTime date = s.m_Date;
			if (startTime <= date && date < endTime) { append(s.m_ID, s.m_Date + shift, s.m_Duration); }
		}
	}

	/// <summary> Copy source on destination and add a time shift to each stimulation. </summary>
	/// <param name="set"> StimulationSet to copy. </param>
	/// <param name="shift"> The time shift. </param>
	void copy(const CStimulationSet& set, const CTime& shift = 0) const
	{
		clear();
		append(set, shift);
	}

	/// <summary> Remove last object of the vector. </summary>
	void pop() const { m_stimulations->pop_back(); }

	/// <summary> Inserts the specified stimulation at specified index. </summary>
	/// <param name="stim"> The stimulation. </param>
	/// <param name="index"> The index where to insert stimulation. </param>
	void insert(const CStimulation& stim, const size_t index) const { m_stimulations->insert(m_stimulations->begin() + index, stim); }
	/// <summary> Removes the object at specified index. </summary>
	/// <param name="index">The index.</param>
	void remove(const size_t index) const { m_stimulations->erase(m_stimulations->begin() + index); }

	/// <summary> Remove stimulations between start and end times. </summary>
	/// <param name="startTime"> The start time. </param>
	/// <param name="endTime"> The end time. </param>
	void removeRange(const CTime& startTime, const CTime& endTime) const
	{
		for (size_t i = 0; i < size(); ++i)
		{
			const CTime date = at(i).m_Date;
			if (startTime <= date && date < endTime) { remove(i--); }
		}
	}
	
	/// <summary> Shifts all the stimulation by the time shift. </summary>
	/// <param name="shift"> The time shift. </param>
	void shift(const CTime& shift) const { for (auto& s : *m_stimulations) { s.m_Date += shift; } }
	
	//--------------------------------------------------
	//------------------- Operators --------------------
	//--------------------------------------------------

	/// <summary> Overload of operator []. </summary>
	/// <param name="index"> The index. </param>
	/// <returns> Reference of the object. </returns>
	CStimulation& operator[](const size_t index) { return m_stimulations->operator[](index); }
	
	/// <summary> Overload of const operator []. </summary>
	/// <param name="index"> The index. </param>
	/// <returns> Const Reference of the object. </returns>
	const CStimulation& operator[](const size_t index) const { return m_stimulations->operator[](index); }

	/// <summary> Override the ostream operator. </summary>
	/// <param name="os"> The ostream. </param>
	/// <param name="obj"> The object. </param>
	/// <returns> Return the modified ostream. </returns>
	friend std::ostream& operator<<(std::ostream& os, const CStimulationSet& obj)
	{
		for (const auto& i : obj) { os << i << std::endl; }
		return os;
	}

protected:

	std::vector<CStimulation>* m_stimulations = nullptr;	///< Internal implementation
};
}  // namespace OpenViBE
