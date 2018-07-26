#include <iostream>

#include "gtest/gtest.h"

#include "ovtAssert.h"

#include <openvibe/ovCMatrix.h>

using namespace OpenViBE;



TEST(CMatrix_Test_Case, test_copy_constructor)
{
	OpenViBE::CMatrix matA;
	matA.setDimensionCount(2);
	matA.setDimensionSize(0,1);
	matA.setDimensionSize(1,2);
	matA.getBuffer()[0] = 10;
	matA.getBuffer()[1] = 20;

	OpenViBE::CMatrix matB(matA);

	ASSERT_EQ(2, matB.getDimensionCount());
	ASSERT_EQ(1, matB.getDimensionSize(0));
	ASSERT_EQ(2, matB.getDimensionSize(1));
	EXPECT_EQ(10, matB.getBuffer()[0]);
	EXPECT_EQ(20, matB.getBuffer()[1]);
}

TEST(CMatrix_Test_Case, test_copy_constructor_in_array_push)
{
	OpenViBE::CMatrix tmpMatrix;
	tmpMatrix.setDimensionCount(2);
	tmpMatrix.setDimensionSize(0,10);
	tmpMatrix.setDimensionSize(1,20);
	std::vector<OpenViBE::CMatrix> list;
	list.push_back(tmpMatrix);
	list.push_back(tmpMatrix);
}

TEST(CMatrix_Test_Case, test_copy_assignment_operator)
{
	OpenViBE::CMatrix matA;
	matA.setDimensionCount(2);
	matA.setDimensionSize(0,1);
	matA.setDimensionSize(1,2);
	matA.getBuffer()[0] = 10;
	matA.getBuffer()[1] = 20;

	EXPECT_EQ(2, matA.getDimensionCount());
	EXPECT_EQ(1, matA.getDimensionSize(0));
	EXPECT_EQ(2, matA.getDimensionSize(1));
	EXPECT_EQ(10, matA.getBuffer()[0]);
	EXPECT_EQ(20, matA.getBuffer()[1]);

	auto matB = matA;

	EXPECT_EQ(2, matB.getDimensionCount());
	EXPECT_EQ(1, matB.getDimensionSize(0));
	EXPECT_EQ(2, matB.getDimensionSize(1));
	EXPECT_EQ(10, matB.getBuffer()[0]);
	EXPECT_EQ(20, matB.getBuffer()[1]);

	matA.getBuffer()[0] = 15;
	matA.getBuffer()[1] = 25;

	EXPECT_EQ(15, matA.getBuffer()[0]);
	EXPECT_EQ(25, matA.getBuffer()[1]);
	EXPECT_EQ(10, matB.getBuffer()[0]);
	EXPECT_EQ(20, matB.getBuffer()[1]);
}

int uoCMatrixTest(int argc, char* argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	::testing::GTEST_FLAG(filter) = "CMatrix_Test_Case.*";
	return RUN_ALL_TESTS();
}
