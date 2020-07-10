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
#include "utils.hpp"

//---------------------------------------------------------------------------------------------------
class CMatrix_Tests : public testing::Test
{
protected:
	OpenViBE::CMatrix m_mat;

	void SetUp() override
	{
		m_mat.resize(1, 2);							// one row two column buffer not init
		m_mat.getBuffer()[0] = 10;					// Buffer init with getBuffer and First element set
		m_mat.getBuffer()[1] = 20;					// Second Element set (buffer already init so refresh function not run)
		m_mat.setDimensionLabel(0, 0, "dim0e0");	// Row label set
		m_mat.setDimensionLabel(1, 0, "dim1e0");	// Column 1 Label set
		m_mat.setDimensionLabel(1, 1, "dim1e1");	// Column 2 Label set
	}
};
//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------
TEST_F(CMatrix_Tests, Constructor)
{
	OpenViBE::CMatrix res;
	ASSERT_EQ(0, res.getSize()) << "Default constructor haven't a size of 0.";
	EXPECT_STREQ("", res.getDimensionLabel(0, 0).c_str()) << "Default constructor dimension so haven't label.";
	
	ASSERT_EQ(2, m_mat.getSize());
	ASSERT_EQ(1, m_mat.getDimensionSize(0));
	ASSERT_EQ(2, m_mat.getDimensionSize(1));
	EXPECT_TRUE(AlmostEqual(10, m_mat.getBuffer()[0]));
	EXPECT_TRUE(AlmostEqual(20, m_mat.getBuffer()[1]));
	EXPECT_STREQ("dim0e0", m_mat.getDimensionLabel(0, 0).c_str());
	EXPECT_STREQ("dim1e0", m_mat.getDimensionLabel(1, 0).c_str());
	EXPECT_STREQ("dim1e1", m_mat.getDimensionLabel(1, 1).c_str());
}
//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------
TEST_F(CMatrix_Tests, Constructor_Copy)
{
	OpenViBE::CMatrix res(m_mat);

	EXPECT_TRUE(m_mat == res) << ErrorMsg("Copy Constructor", m_mat, res);

	res.getBuffer()[0] = 15;
	res.getBuffer()[1] = 25;
	res.setDimensionLabel(1, 0, "changed");

	EXPECT_TRUE(AlmostEqual(10, m_mat.getBuffer()[0]));
	EXPECT_TRUE(AlmostEqual(20, m_mat.getBuffer()[1]));
	EXPECT_STREQ("dim1e0", m_mat.getDimensionLabel(1, 0).c_str());
	EXPECT_TRUE(AlmostEqual(15, res.getBuffer()[0]));
	EXPECT_TRUE(AlmostEqual(25, res.getBuffer()[1]));
	EXPECT_STREQ("changed", res.getDimensionLabel(1, 0).c_str());
}
//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------
TEST_F(CMatrix_Tests, Constructor_copy_in_array_push)
{
	std::vector<OpenViBE::CMatrix> res;
	res.push_back(m_mat);
	res.push_back(m_mat);

	res[0].getBuffer()[0] = 15;
	res[0].getBuffer()[1] = 25;
	res[0].setDimensionLabel(1, 0, "changed");

	EXPECT_TRUE(AlmostEqual(10, res[1].getBuffer()[0]));
	EXPECT_TRUE(AlmostEqual(20, res[1].getBuffer()[1]));
	EXPECT_STREQ("dim1e0", m_mat.getDimensionLabel(1, 0).c_str());
	EXPECT_TRUE(AlmostEqual(15, res[0].getBuffer()[0]));
	EXPECT_TRUE(AlmostEqual(25, res[0].getBuffer()[1]));
	EXPECT_STREQ("changed", res[0].getDimensionLabel(1, 0).c_str());
}
//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------
TEST_F(CMatrix_Tests, Operators)
{
	OpenViBE::CMatrix res = m_mat;

	EXPECT_TRUE(m_mat.isDescriptionEqual(m_mat)) << ErrorMsg("equality description", m_mat, m_mat);
	EXPECT_TRUE(m_mat.isBufferEqual(m_mat)) << ErrorMsg("equality buffer", m_mat, m_mat);
	EXPECT_TRUE(m_mat.isBufferAlmostEqual(m_mat)) << ErrorMsg("Almost equality buffer", m_mat, m_mat);
	EXPECT_TRUE(m_mat == m_mat) << ErrorMsg("equality operator", m_mat, m_mat);
	EXPECT_TRUE(m_mat == res) << ErrorMsg("Copy assignement", m_mat, res);

	m_mat.getBuffer()[0] = 15;
	m_mat.getBuffer()[1] = 25;
	m_mat.setDimensionLabel(1, 0, "changed");

	EXPECT_TRUE(AlmostEqual(15, m_mat.getBuffer()[0]));
	EXPECT_TRUE(AlmostEqual(25, m_mat.getBuffer()[1]));
	EXPECT_STREQ("changed", m_mat.getDimensionLabel(1, 0).c_str());
	EXPECT_TRUE(AlmostEqual(10, res.getBuffer()[0]));
	EXPECT_TRUE(AlmostEqual(20, res.getBuffer()[1]));
	EXPECT_STREQ("dim1e0", res.getDimensionLabel(1, 0).c_str());
}
//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------
TEST_F(CMatrix_Tests, Resize)
{
	OpenViBE::CMatrix res(1, 2);
	res.reset();
	res.setDimensionLabel(0, 0, "label");
	ASSERT_EQ(2, res.getDimensionCount());
	ASSERT_EQ(1, res.getDimensionSize(0));
	ASSERT_EQ(2, res.getDimensionSize(1));
	EXPECT_TRUE(AlmostEqual(0, res.getBuffer()[0]));
	EXPECT_TRUE(AlmostEqual(0, res.getBuffer()[1]));
	EXPECT_STREQ("label", res.getDimensionLabel(0, 0).c_str());
	EXPECT_STREQ("", res.getDimensionLabel(1, 0).c_str());
	EXPECT_STREQ("", res.getDimensionLabel(1, 1).c_str());
	
	res.resize(2, 2);
	res.setDimensionLabel(1, 1, "label");
	ASSERT_EQ(2, res.getDimensionCount());
	ASSERT_EQ(2, res.getDimensionSize(0));
	ASSERT_EQ(2, res.getDimensionSize(1));
	EXPECT_STREQ("", res.getDimensionLabel(0, 0).c_str());		// The resize remove all previous label and size
	EXPECT_STREQ("label", res.getDimensionLabel(1, 1).c_str());
		
	res.resetLabels();
	EXPECT_STREQ("", res.getDimensionLabel(1, 1).c_str());
		
	res.resize(2);
	ASSERT_EQ(1, res.getDimensionCount());
	ASSERT_EQ(2, res.getDimensionSize(0));
}
//---------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------
TEST_F(CMatrix_Tests, Save_Load)
{
	OpenViBE::CMatrix res;
	EXPECT_TRUE(m_mat.toTextFile("Save_2DMatrix-output.txt")) << "Error during Saving 2D Matrix : " << std::endl << m_mat << std::endl;
	EXPECT_TRUE(res.fromTextFile("Save_2DMatrix-output.txt")) << "Error during Loading 2D Matrix : " << std::endl << res << std::endl;
	EXPECT_TRUE(m_mat == res) << ErrorMsg("Save", m_mat, res);

	OpenViBE::CMatrix row(2);
	row.getBuffer()[0] = -1;
	row.getBuffer()[1] = -4.549746549678;
	EXPECT_TRUE(row.toTextFile("Save_2DMatrix-output.txt")) << "Error during Saving 1D Matrix : " << std::endl << row << std::endl;
	EXPECT_TRUE(res.fromTextFile("Save_2DMatrix-output.txt")) << "Error during Loading 1D Matrix : " << std::endl << res << std::endl;
	EXPECT_TRUE(row == res) << ErrorMsg("Save", row, res);
	
}
//---------------------------------------------------------------------------------------------------
