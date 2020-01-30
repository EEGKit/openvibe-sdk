#include <iostream>

#include "gtest/gtest.h"

#include "ovtAssert.h"

#include "../common/ovtKernelContext.h"
#include <openvibe/ov_all.h>

#include <ovp_global_defines.h>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;

namespace
{
	const char* kernelConfig = nullptr;

	class StreamedMatrixTest : public testing::Test
	{
	protected:

		StreamedMatrixTest() { }
		~StreamedMatrixTest() override { }

		void SetUp() override
		{
			m_kernelCtx.initialize();
			m_kernelCtx->getPluginManager().addPluginsFromFiles(
				m_kernelCtx->getConfigurationManager().expand("${Path_Lib}/*openvibe-plugins-sdk-stream-codecs*"));

			m_decoderId = OV_UndefinedIdentifier;
			m_decoderId = m_kernelCtx->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixDecoder);
			ASSERT_NE(OV_UndefinedIdentifier, m_decoderId);

			m_encoderId = OV_UndefinedIdentifier;
			m_encoderId = m_kernelCtx->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixEncoder);
			ASSERT_NE(OV_UndefinedIdentifier, m_encoderId);
		}

		void TearDown() override
		{
			ASSERT_TRUE(m_kernelCtx->getAlgorithmManager().releaseAlgorithm(m_decoderId));
			m_decoderId = OV_UndefinedIdentifier;
			ASSERT_TRUE(m_kernelCtx->getAlgorithmManager().releaseAlgorithm(m_encoderId));
			m_encoderId = OV_UndefinedIdentifier;
			m_kernelCtx.uninitialize();
		}

		CIdentifier m_decoderId;
		CIdentifier m_encoderId;
		OpenViBE::Test::ctx m_kernelCtx;
	};

	TEST_F(StreamedMatrixTest, test_init)
	{
		auto& decoder = m_kernelCtx->getAlgorithmManager().getAlgorithm(m_decoderId);
		auto& encoder = m_kernelCtx->getAlgorithmManager().getAlgorithm(m_encoderId);
		EXPECT_TRUE(decoder.initialize());
		EXPECT_TRUE(encoder.initialize());
		EXPECT_TRUE(decoder.uninitialize());
		EXPECT_TRUE(encoder.uninitialize());
	}

	TEST_F(StreamedMatrixTest, matrix_decoder_does_not_crash_when_given_an_empty_matrix)
	{
		auto& encoder = m_kernelCtx->getAlgorithmManager().getAlgorithm(m_encoderId);
		auto& decoder = m_kernelCtx->getAlgorithmManager().getAlgorithm(m_decoderId);
		EXPECT_TRUE(encoder.initialize());
		EXPECT_TRUE(decoder.initialize());
		CMatrix mat;
		ASSERT_TRUE(mat.setDimensionCount(1));
		ASSERT_TRUE(mat.setDimensionSize(0, 0));

		TParameterHandler<const IMatrix*> iMatrix(encoder.getInputParameter(OVP_GD_Algorithm_StreamedMatrixEncoder_InputParameterId_Matrix));
		TParameterHandler<const IMemoryBuffer*> oBuffer(encoder.getOutputParameter(OVP_GD_Algorithm_StreamedMatrixEncoder_OutputParameterId_EncodedMemoryBuffer));
		TParameterHandler<const IMemoryBuffer*> iBuffer(decoder.getInputParameter(OVP_GD_Algorithm_StreamedMatrixDecoder_InputParameterId_MemoryBufferToDecode));
		TParameterHandler<const IMatrix*> oMatrix(decoder.getOutputParameter(OVP_GD_Algorithm_StreamedMatrixDecoder_OutputParameterId_Matrix));

		iMatrix = &mat;
		iBuffer.setReferenceTarget(oBuffer);

		encoder.process(OVP_GD_Algorithm_StreamedMatrixEncoder_InputTriggerId_EncodeHeader);
		decoder.process();

		EXPECT_TRUE(decoder.isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixDecoder_OutputTriggerId_ReceivedHeader));

		EXPECT_EQ(mat.getDimensionCount(), oMatrix->getDimensionCount());

		EXPECT_TRUE(encoder.uninitialize());
		EXPECT_TRUE(decoder.uninitialize());
	}

	TEST_F(StreamedMatrixTest, matrix_encoded_and_then_decoded_is_identical_to_original)
	{
		auto& encoder = m_kernelCtx->getAlgorithmManager().getAlgorithm(m_encoderId);
		auto& decoder = m_kernelCtx->getAlgorithmManager().getAlgorithm(m_decoderId);
		EXPECT_TRUE(encoder.initialize());
		EXPECT_TRUE(decoder.initialize());
		CMatrix mat;
		ASSERT_TRUE(mat.setDimensionCount(2));
		ASSERT_TRUE(mat.setDimensionSize(0, 2));
		ASSERT_TRUE(mat.setDimensionSize(1, 3));
		size_t elemCount = 1;
		for (size_t dim = 0; dim < mat.getDimensionCount(); ++dim)
		{
			const auto size = mat.getDimensionSize(dim);
			for (size_t entry = 0; entry < size; ++entry)
			{
				std::string label = std::to_string(dim) + ":" + std::to_string(entry);
				mat.setDimensionLabel(0, 0, label.c_str());
			}
			elemCount *= size;
		}
		for (size_t i = 0; i < elemCount; ++i) { mat.getBuffer()[i] = double(i); }

		TParameterHandler<const IMatrix*> iMatrix(encoder.getInputParameter(OVP_GD_Algorithm_StreamedMatrixEncoder_InputParameterId_Matrix));
		TParameterHandler<const IMemoryBuffer*> oBuffer(
			encoder.getOutputParameter(OVP_GD_Algorithm_StreamedMatrixEncoder_OutputParameterId_EncodedMemoryBuffer));
		TParameterHandler<const IMemoryBuffer*> iBuffer(
			decoder.getInputParameter(OVP_GD_Algorithm_StreamedMatrixDecoder_InputParameterId_MemoryBufferToDecode));
		TParameterHandler<const IMatrix*> oMatrix(decoder.getOutputParameter(OVP_GD_Algorithm_StreamedMatrixDecoder_OutputParameterId_Matrix));

		iMatrix = &mat;
		iBuffer.setReferenceTarget(oBuffer);

		encoder.process(OVP_GD_Algorithm_StreamedMatrixEncoder_InputTriggerId_EncodeHeader);
		decoder.process();

		EXPECT_TRUE(decoder.isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixDecoder_OutputTriggerId_ReceivedHeader));

		EXPECT_EQ(mat.getDimensionCount(), oMatrix->getDimensionCount());

		EXPECT_TRUE(encoder.uninitialize());
		EXPECT_TRUE(decoder.uninitialize());
	}
}	// namespace

int uoStreamedMatrixTest(int argc, char* argv[])
{
	if (argc > 1) { kernelConfig = argv[1]; }
	testing::InitGoogleTest(&argc, argv);
	::testing::GTEST_FLAG(filter) = "StreamedMatrixTest.*";
	return RUN_ALL_TESTS();
}
