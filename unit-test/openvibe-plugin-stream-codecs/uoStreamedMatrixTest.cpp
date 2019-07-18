#include <iostream>

#include "gtest/gtest.h"

#include "ovtAssert.h"

#include "../common/ovtKernelContext.h"
#include <openvibe/ov_all.h>

#include <ovp_global_defines.h>

using namespace OpenViBE;
using namespace Kernel;

namespace
{
	const char* kernelConfig = nullptr;

	class StreamedMatrixTest : public testing::Test
	{
	protected:

		StreamedMatrixTest()
			: m_KernelContext() { }


		~StreamedMatrixTest() override { }

		void SetUp() override
		{
			m_KernelContext.initialize();
			m_KernelContext->getPluginManager().addPluginsFromFiles(m_KernelContext->getConfigurationManager().expand("${Path_Lib}/*openvibe-plugins-sdk-stream-codecs*"));

			m_DecoderId = OV_UndefinedIdentifier;
			m_DecoderId = m_KernelContext->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamDecoder);
			ASSERT_NE(OV_UndefinedIdentifier, m_DecoderId);

			m_EncoderId = OV_UndefinedIdentifier;
			m_EncoderId = m_KernelContext->getAlgorithmManager().createAlgorithm(OVP_GD_ClassId_Algorithm_StreamedMatrixStreamEncoder);
			ASSERT_NE(OV_UndefinedIdentifier, m_EncoderId);
		}

		void TearDown() override
		{
			ASSERT_TRUE(m_KernelContext->getAlgorithmManager().releaseAlgorithm(m_DecoderId));
			m_DecoderId = OV_UndefinedIdentifier;
			ASSERT_TRUE(m_KernelContext->getAlgorithmManager().releaseAlgorithm(m_EncoderId));
			m_EncoderId = OV_UndefinedIdentifier;
			m_KernelContext.uninitialize();
		}

		CIdentifier m_DecoderId;
		CIdentifier m_EncoderId;
		OpenViBETest::KernelContext m_KernelContext;
	};

	TEST_F(StreamedMatrixTest, test_init)
	{
		auto& decoder = m_KernelContext->getAlgorithmManager().getAlgorithm(m_DecoderId);
		auto& encoder = m_KernelContext->getAlgorithmManager().getAlgorithm(m_EncoderId);
		EXPECT_TRUE(decoder.initialize());
		EXPECT_TRUE(encoder.initialize());
		EXPECT_TRUE(decoder.uninitialize());
		EXPECT_TRUE(encoder.uninitialize());
	}

	TEST_F(StreamedMatrixTest, matrix_decoder_does_not_crash_when_given_an_empty_matrix)
	{
		auto& encoder = m_KernelContext->getAlgorithmManager().getAlgorithm(m_EncoderId);
		auto& decoder = m_KernelContext->getAlgorithmManager().getAlgorithm(m_DecoderId);
		EXPECT_TRUE(encoder.initialize());
		EXPECT_TRUE(decoder.initialize());
		CMatrix mat;
		ASSERT_TRUE(mat.setDimensionCount(1));
		ASSERT_TRUE(mat.setDimensionSize(0, 0));

		TParameterHandler<const IMatrix*> inputMatrix(encoder.getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix));
		TParameterHandler<const IMemoryBuffer*> outputBuffer(encoder.getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
		TParameterHandler<const IMemoryBuffer*> inputBuffer(decoder.getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_InputParameterId_MemoryBufferToDecode));
		TParameterHandler<const IMatrix*> outputMatrix(decoder.getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));

		inputMatrix = &mat;
		inputBuffer.setReferenceTarget(outputBuffer);

		encoder.process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
		decoder.process();

		EXPECT_TRUE(decoder.isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedHeader));

		EXPECT_EQ(mat.getDimensionCount(), outputMatrix->getDimensionCount());

		EXPECT_TRUE(encoder.uninitialize());
		EXPECT_TRUE(decoder.uninitialize());
	}

	TEST_F(StreamedMatrixTest, matrix_encoded_and_then_decoded_is_identical_to_original)
	{
		auto& encoder = m_KernelContext->getAlgorithmManager().getAlgorithm(m_EncoderId);
		auto& decoder = m_KernelContext->getAlgorithmManager().getAlgorithm(m_DecoderId);
		EXPECT_TRUE(encoder.initialize());
		EXPECT_TRUE(decoder.initialize());
		CMatrix mat;
		ASSERT_TRUE(mat.setDimensionCount(2));
		ASSERT_TRUE(mat.setDimensionSize(0, 2));
		ASSERT_TRUE(mat.setDimensionSize(1, 3));
		size_t elemCount = 1;
		for (uint32_t dim = 0; dim < mat.getDimensionCount(); ++dim)
		{
			auto size = mat.getDimensionSize(dim);
			for (uint32_t entry = 0; entry < size; ++entry)
			{
				std::string label = std::to_string(dim) + ":" + std::to_string(entry);
				mat.setDimensionLabel(0, 0, label.c_str());
			}
			elemCount *= size;
		}
		for (size_t i = 0; i < elemCount; ++i)
		{
			mat.getBuffer()[i] = i;
		}

		TParameterHandler<const IMatrix*> inputMatrix(encoder.getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix));
		TParameterHandler<const IMemoryBuffer*> outputBuffer(encoder.getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_OutputParameterId_EncodedMemoryBuffer));
		TParameterHandler<const IMemoryBuffer*> inputBuffer(decoder.getInputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_InputParameterId_MemoryBufferToDecode));
		TParameterHandler<const IMatrix*> outputMatrix(decoder.getOutputParameter(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix));

		inputMatrix = &mat;
		inputBuffer.setReferenceTarget(outputBuffer);

		encoder.process(OVP_GD_Algorithm_StreamedMatrixStreamEncoder_InputTriggerId_EncodeHeader);
		decoder.process();

		EXPECT_TRUE(decoder.isOutputTriggerActive(OVP_GD_Algorithm_StreamedMatrixStreamDecoder_OutputTriggerId_ReceivedHeader));

		EXPECT_EQ(mat.getDimensionCount(), outputMatrix->getDimensionCount());

		EXPECT_TRUE(encoder.uninitialize());
		EXPECT_TRUE(decoder.uninitialize());
	}
}

int uoStreamedMatrixTest(int argc, char* argv[])
{
	if (argc > 1)
	{
		kernelConfig = argv[1];
	}
	testing::InitGoogleTest(&argc, argv);
	::testing::GTEST_FLAG(filter) = "StreamedMatrixTest.*";
	return RUN_ALL_TESTS();
}
