///-------------------------------------------------------------------------------------------------
/// 
/// \file CIdentifierTest.hpp
/// \brief Test Definitions for OpenViBE Identifier Class.
/// \author Thibaut Monseigne (Inria).
/// \version 1.0.
/// \date 23/06/2020.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/">GNU Affero General Public License v3.0</a>.
/// 
///-------------------------------------------------------------------------------------------------

#pragma once

#include <gtest/gtest.h>
#include <openvibe/CIdentifier.hpp>

#include "utils.hpp"

//---------------------------------------------------------------------------------------------------
TEST(CIdentifier_Tests, constructors)
{
	OpenViBE::CIdentifier id1, id2(10), id3(-1), id4(10, 10), id5("false string"), id6("(0x00000000, 0x0000000A)");
	const uint64_t undef = std::numeric_limits<uint64_t>::max();
	
	EXPECT_EQ(id1.id(), undef) << ErrorMsg("Default Constructor", id1.id(), undef);
	EXPECT_EQ(id2.id(), 10) << ErrorMsg("Integer Constructor", id2.id(), 10);
	EXPECT_EQ(id3.id(), undef) << ErrorMsg("Negative value Constructor", id3.id(), undef);
	EXPECT_EQ(id4.id(), 42949672970) << ErrorMsg("Double ID Constructor", id4.id(), 42949672970); // 10 * 2^32 + 10
	EXPECT_EQ(id5.id(), undef) << ErrorMsg("False string Constructor", id5.id(), undef);
	EXPECT_EQ(id6.id(), 10) << ErrorMsg("String Constructor", id6.id(), 10);
}
//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------
TEST(CIdentifier_Tests, operators)
{
	OpenViBE::CIdentifier id(10);
	const OpenViBE::CIdentifier id2(10);
	const uint64_t undef = std::numeric_limits<uint64_t>::max();
	
	id = 0;
	EXPECT_EQ(id.id(), 0) << ErrorMsg("Assignement operator with unsigned", id.id(), 0);
	id = OpenViBE::CIdentifier::undefined();
	EXPECT_EQ(id.id(), undef) << ErrorMsg("Assignement operator with CIdentifier", id.id(), undef);
	
	++id;		// No change if undef
	EXPECT_EQ(id.id(), undef) << ErrorMsg("++ operator with undefined Identifier", id.id(), undef);
	--id;		// No change if undef
	EXPECT_EQ(id.id(), undef) << ErrorMsg("-- operator with undefined Identifier", id.id(), undef);
	
	id = 0;
	--id;		// 0 became max - 1
	EXPECT_EQ(id.id(), undef - 1) << ErrorMsg("-- operator with Identifier 0", id.id(), undef - 1);
	++id;		// max - 1 became 0
	EXPECT_EQ(id.id(), 0) << ErrorMsg("++ operator with Identifier 0", id.id(), undef - 1);

	id = 0;
	EXPECT_TRUE(id<=id && id>=id && id < id2 && id2 > id) << ErrorMsg("Comparison Operator", id.id(), id2.id());

	std::stringstream ss;
	ss << id;
	EXPECT_EQ(ss.str(), "(0x00000000, 0x00000000)") << ErrorMsg("ostream operator", ss.str(), "(0x00000000, 0x00000000)");

}
//---------------------------------------------------------------------------------------------------
