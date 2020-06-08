///-------------------------------------------------------------------------------------------------
/// 
/// \file CStimulationTest.hpp
/// \brief Test Definitions for OpenViBE CStimulationSet.
/// \author Thibaut Monseigne (Inria).
/// \version 1.0.
/// \date 11/05/2020.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/">GNU Affero General Public License v3.0</a>.
/// 
///-------------------------------------------------------------------------------------------------

#pragma once

#include <gtest/gtest.h>
#include <openvibe/CStimulationSet.hpp>
#include "utils.hpp"

//---------------------------------------------------------------------------------------------------
namespace Dataset {}	// namespace Dataset
//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------
TEST(CStimulation_Tests, append)
{
	OpenViBE::CStimulationSet set;
	size_t i = 0;
	set.append(OpenViBE::CStimulation(i++, i++, i++));
	set.append(i++, i++, i);
	EXPECT_TRUE(set[0].m_ID == 0 && set[0].m_Date == 1 && set[0].m_Duration == 2) << "First Append is False.";
	EXPECT_TRUE(set[0].m_ID == 3 && set[0].m_Date == 4 && set[0].m_Duration == 5) << "Second Append is False.";
}
//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------
TEST(CStimulation_Tests, insert_remove)
{
	OpenViBE::CStimulationSet set;
	size_t i = 0;
	for (size_t n = 0; n < 5; ++n) { set.append(i, i, i++); }

	set.insert(OpenViBE::CStimulation(0, 0, 0), 3);	//insert at index 3 (so stim id are 0,1,2,0,3,4)
	EXPECT_TRUE(set[3].m_ID == 0 && set[3].m_Date == 0 && set[3].m_Duration == 0) << "Insert fail.";

	set.remove(4);									//insert at index 4 (so stim id are 0,1,2,0,4)
	EXPECT_TRUE(set[4].m_ID == 4 && set[4].m_Date == 4 && set[4].m_Duration == 4) << "Remove fail.";
}
//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------
TEST(CStimulation_Tests, range)
{
	OpenViBE::CStimulationSet set;
	size_t i = 0;
	for (size_t n = 0; n < 10; ++n) { set.append(i, i, i++); }

	set.appendRange(set, 2, 10);	// append 2 to 9 (so new size 18)
	set.appendRange(set, 20, 90);	// append nothing
	EXPECT_TRUE(set.size() == 18) << ErrorMsg("Size of Stimuation Set", 18, set.size());

	set.removeRange(4, 6);		// remove all between 4 and 6 second (so stim with date 4 and 5 2 time (with the previous append)
	set.removeRange(10, 20);	// remove nothing
	EXPECT_TRUE(set.size() == 14) << ErrorMsg("Size of Stimuation Set", 14, set.size());
}
//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------
TEST(CStimulation_Tests, shift)
{
	OpenViBE::CStimulationSet set;
	size_t i = 0;
	for (size_t n = 0; n < 10; ++n) { set.append(i, i, i++); }

	set.shift(10);
	for (size_t n = 0; n < 10; ++n) { EXPECT_TRUE(set[i].m_Date == n + 10) << ErrorMsg("Date Shifted", n + 10, set[i].m_Date); }
}
//---------------------------------------------------------------------------------------------------
