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
namespace Dataset {
static const OpenViBE::CTime T02(2.0), T03(3.0), T04(4.0), T05(5.0);
static const OpenViBE::CTime T06(6.0), T10(10.0), T20(20.0), T90(90.0);
static const OpenViBE::CStimulation S0(0, 0, 0), S1(1, T02, T03);
static const OpenViBE::CStimulation S2(4, T05, T06), S4(4, T04, T04);

}	// namespace Dataset
//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------
TEST(CStimulation_Tests, append)
{
	OpenViBE::CStimulationSet set;

	set.append(Dataset::S1);
	set.append(Dataset::S2.m_ID, Dataset::S2.m_Date, Dataset::S2.m_Duration);
	EXPECT_TRUE(set[0] == Dataset::S1) << ErrorMsg("First Append : ", Dataset::S1, set[0]);
	EXPECT_TRUE(set[1] == Dataset::S2) << ErrorMsg("Second Append : ", Dataset::S2, set[1]);
}
//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------
TEST(CStimulation_Tests, insert_remove)
{
	OpenViBE::CStimulationSet set;
	for (size_t i = 0; i < 5; ++i) { set.append(i, i << 32, i << 32); }

	set.insert(Dataset::S0, 3);		//insert at index 3 (so stim id are 0,1,2,0,3,4)
	EXPECT_TRUE(set[3] == Dataset::S0) << ErrorMsg("Insert : ", Dataset::S0, set[3]);

	set.remove(4);					//insert at index 4 (so stim id are 0,1,2,0,4)
	EXPECT_TRUE(set[4] == Dataset::S4) << ErrorMsg("Remove : ", Dataset::S4, set[4]);
}
//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------
TEST(CStimulation_Tests, range)
{
	OpenViBE::CStimulationSet set;
	for (size_t i = 0; i < 10; ++i) { set.append(i, i << 32, i << 32); }

	set.appendRange(set, Dataset::T02, Dataset::T10);	// append 2 to 9 (so new size 18)
	set.appendRange(set, Dataset::T20, Dataset::T90);	// append nothing
	EXPECT_TRUE(set.size() == 18) << ErrorMsg("Size of Stimuation Set", 18, set.size());

	set.removeRange(Dataset::T04, Dataset::T06);	// remove all between 4 and 6 second (so stim with date 4 and 5 2 time (with the previous append)
	set.removeRange(Dataset::T10, Dataset::T20);	// remove nothing
	EXPECT_TRUE(set.size() == 14) << ErrorMsg("Size of Stimuation Set", 14, set.size());
}
//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------
TEST(CStimulation_Tests, shift)
{
	OpenViBE::CStimulationSet set;
	for (size_t i = 0; i < 10; ++i) { set.append(i, i << 32, i << 32); }

	set.shift(Dataset::T10);
	for (size_t i = 0; i < 10; ++i)
	{
		EXPECT_TRUE(set[i].m_Date == OpenViBE::CTime(i + 10.0))
		<< ErrorMsg("Date Shifted for Stim " + std::to_string(i), OpenViBE::CTime(i + 10.0), set[i].m_Date);
	}
}
//---------------------------------------------------------------------------------------------------
