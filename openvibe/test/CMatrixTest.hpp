///-------------------------------------------------------------------------------------------------
/// 
/// \file CMatrixTest.hpp
/// \brief Test Definitions for OpenViBE Matrix Class.
/// \author Thibaut Monseigne (Inria).
/// \version 1.0.
/// \date 11/05/2020.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/">GNU Affero General Public License v3.0</a>.
/// 
///-------------------------------------------------------------------------------------------------

#pragma once

#include <gtest/gtest.h>
#include <openvibe/CMatrix.hpp>

//---------------------------------------------------------------------------------------------------
class CMatrix_Tests : public testing::Test
{
protected:
	OpenViBE::CMatrix m_mat;

	void SetUp() override
	{
		m_mat.setDimensionCount(2);
		m_mat.setDimensionSize(0, 1);
		m_mat.setDimensionSize(1, 2);
		m_mat.getBuffer()[0] = 10;
		m_mat.getBuffer()[1] = 20;
		m_mat.setDimensionLabel(0, 0, "dim0e0");
		m_mat.setDimensionLabel(1, 0, "dim1e0");
		m_mat.setDimensionLabel(1, 1, "dim1e1");
	}
};
//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------
TEST_F(CMatrix_Tests, constructor_copy)
{
	OpenViBE::CMatrix res(m_mat);

	ASSERT_EQ(2, res.getDimensionCount());
	ASSERT_EQ(1, res.getDimensionSize(0));
	ASSERT_EQ(2, res.getDimensionSize(1));
	EXPECT_EQ(10, res.getBuffer()[0]);
	EXPECT_EQ(20, res.getBuffer()[1]);
	EXPECT_STREQ(m_mat.getDimensionLabel(0, 0).c_str(), res.getDimensionLabel(0, 0).c_str());
	EXPECT_STREQ(m_mat.getDimensionLabel(0, 0).c_str(), res.getDimensionLabel(1, 0).c_str());
	EXPECT_STREQ(m_mat.getDimensionLabel(0, 0).c_str(), res.getDimensionLabel(1, 1).c_str());

	res.getBuffer()[0] = 15;
	res.getBuffer()[1] = 25;
	res.setDimensionLabel(1, 0, "changed");

	EXPECT_EQ(10, m_mat.getBuffer()[0]);
	EXPECT_EQ(20, m_mat.getBuffer()[1]);
	EXPECT_STREQ("dim1e0", m_mat.getDimensionLabel(1, 0).c_str());
	EXPECT_EQ(15, res.getBuffer()[0]);
	EXPECT_EQ(25, res.getBuffer()[1]);
	EXPECT_STREQ("changed", res.getDimensionLabel(1, 0).c_str());
}
//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------
TEST_F(CMatrix_Tests, constructor_copy_in_array_push)
{
	OpenViBE::CMatrix tmpMatrix;
	tmpMatrix.setDimensionCount(2);
	tmpMatrix.setDimensionSize(0, 10);
	tmpMatrix.setDimensionSize(1, 20);
	std::vector<OpenViBE::CMatrix> list;
	list.push_back(tmpMatrix);
	list.push_back(tmpMatrix);
}
//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------
TEST_F(CMatrix_Tests, operator_copy_assignment)
{
	OpenViBE::CMatrix matB = m_mat;

	EXPECT_EQ(2, matB.getDimensionCount());
	EXPECT_EQ(1, matB.getDimensionSize(0));
	EXPECT_EQ(2, matB.getDimensionSize(1));
	EXPECT_EQ(10, matB.getBuffer()[0]);
	EXPECT_EQ(20, matB.getBuffer()[1]);

	m_mat.getBuffer()[0] = 15;
	m_mat.getBuffer()[1] = 25;
	m_mat.setDimensionLabel(1, 0, "changed");

	EXPECT_EQ(15, m_mat.getBuffer()[0]);
	EXPECT_EQ(25, m_mat.getBuffer()[1]);
	EXPECT_STREQ("changed", m_mat.getDimensionLabel(1, 0).c_str());
	EXPECT_EQ(10, matB.getBuffer()[0]);
	EXPECT_EQ(20, matB.getBuffer()[1]);
	EXPECT_STREQ("dim1e0", matB.getDimensionLabel(1, 0).c_str());
}
//---------------------------------------------------------------------------------------------------
