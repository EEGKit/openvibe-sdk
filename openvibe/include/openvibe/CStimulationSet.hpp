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

//typedef std::vector<CStimulation> CStimulationSet; // Why not ?
namespace OpenViBE {
/// <summary> Basic standalone OpenViBE stimulation set implementation.
///
/// This class offers a basic standalone impementation of the CStimulationSet interface. This class can be directly instanciatedand used.
/// Instances of this class use an internal implementation of the CStimulationSet interfaceand redirect their calls to this implementation.
/// </summary>
class OV_API CStimulationSet
{
public:

	//--------------------------------------------------
	//------------ Constructor / Destructor ------------
	//--------------------------------------------------
	CStimulationSet()  = default;	///< Default constructor
	~CStimulationSet() = default;	///< Default destructor

	//--------------------------------------------------
	//------------- Basic Vector Functions -------------
	//--------------------------------------------------
	void clear() { m_stimulations.clear(); }
	size_t size() const { return m_stimulations.size(); }
	void resize(const size_t n) { m_stimulations.resize(n); }

	void append(const CStimulation& stim) { m_stimulations.push_back(stim); }
	void pop() { m_stimulations.pop_back(); }

	void insert(const CStimulation& stim, const size_t index) { m_stimulations.insert(m_stimulations.begin() + index, stim); }
	void remove(const size_t index) { m_stimulations.erase(m_stimulations.begin() + index); }
	
	/// <summary> Shifts by the time shift. </summary>
	/// <param name="shift"> The time shift. </param>
	void shift(const CTime& shift) { for (auto& s : m_stimulations) { s.m_Date += shift; } }

	/// <summary> First object Iterator (usefull for modern for loop). </summary>
	/// <returns> Iterator. </returns>
	std::vector<CStimulation>::iterator begin() { return m_stimulations.begin(); }
	/// <summary> End of vector Iterator (usefull for modern for loop). </summary>
	/// <returns> Iterator. </returns>
	std::vector<CStimulation>::iterator end() { return m_stimulations.end(); }
	/// <summary> First object Iterator (usefull for modern for loop). </summary>
	/// <returns> Iterator. </returns>
	std::vector<CStimulation>::const_iterator begin() const { return m_stimulations.cbegin(); }
	/// <summary> End of vector Iterator (usefull for modern for loop). </summary>
	/// <returns> Iterator. </returns>
	std::vector<CStimulation>::const_iterator end() const { return m_stimulations.cend(); }

	//--------------------------------------------------
	//------------------- Operators --------------------
	//--------------------------------------------------

	CStimulation& operator[](const size_t index) { return m_stimulations[index]; }
	const CStimulation& operator[](const size_t index) const { return m_stimulations[index]; }

	//--------------------------------------------------
	//---------------- Static Functions ----------------
	//--------------------------------------------------
	static void copy(CStimulationSet& dst, const CStimulationSet& src, const CTime& shift = 0);
	static void append(CStimulationSet& dst, const CStimulationSet& src, const CTime& shift = 0);
	static void appendRange(CStimulationSet& dst, const CStimulationSet& src, const CTime& startTime, const CTime& endTime, const CTime& shift = 0);
	static void removeRange(CStimulationSet& set, const CTime& startTime, const CTime& endTime);

protected:

	std::vector<CStimulation> m_stimulations; ///< Internal implementation
};
} // namespace OpenViBE
