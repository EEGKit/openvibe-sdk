#include "ovpCBoxAlgorithmMatrixValidityChecker.h"

#include <cmath>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Tools;

#if defined TARGET_OS_Windows
 #define isnan _isnan
 #define isinf !_finite
#elif defined TARGET_OS_MacOS
 #define isnan std::isnan
 #define isinf std::isinf
#endif

boolean CBoxAlgorithmMatrixValidityChecker::initialize(void)
{
	const IBox& l_rStaticBoxContext=this->getStaticBoxContext();

	uint64 l_ui64LogLevel = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_eLogLevel = static_cast<ELogLevel>(l_ui64LogLevel);
	m_ui64ValidityCheckerType = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	if(l_rStaticBoxContext.getSettingCount()==1) m_ui64ValidityCheckerType = OVP_TypeId_ValidityCheckerType_LogWarning.toUInteger(); // note that for boxes with one setting, we fallback to the old behavior

	if(l_rStaticBoxContext.getSettingCount()>1 && l_rStaticBoxContext.getInputCount()!=l_rStaticBoxContext.getOutputCount())
	{
		this->getLogManager() << LogLevel_Error << "Number of input(s) different from number of output(s).\n";
		return false;
	}

	m_vStreamDecoder.resize(l_rStaticBoxContext.getInputCount());
	m_vStreamEncoder.resize(l_rStaticBoxContext.getInputCount());
	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		m_vStreamDecoder[i].initialize(*this, i);
		m_vStreamEncoder[i].initialize(*this, i);
		m_vStreamEncoder[i].getInputMatrix().setReferenceTarget(m_vStreamDecoder[i].getOutputMatrix());
	}
	
	m_vLastValidSample.clear();
	m_vLastValidSample.resize(l_rStaticBoxContext.getInputCount());
	m_ui32TotalInterpolatedSampleCount.clear();
	m_ui32TotalInterpolatedSampleCount.resize(l_rStaticBoxContext.getInputCount());
	m_ui32TotalInterpolatedChunkCount.clear();
	m_ui32TotalInterpolatedChunkCount.resize(l_rStaticBoxContext.getInputCount());

	return true;
}

boolean CBoxAlgorithmMatrixValidityChecker::uninitialize(void)
{
	const IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	for(uint32 i=0; i<l_rStaticBoxContext.getInputCount(); i++)
	{
		m_vStreamDecoder[i].uninitialize();
		m_vStreamEncoder[i].uninitialize();
	}
	m_vStreamDecoder.clear();
	m_vStreamEncoder.clear();

	return true;
}

boolean CBoxAlgorithmMatrixValidityChecker::processInput(uint32 ui32InputIndex)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

boolean CBoxAlgorithmMatrixValidityChecker::process(void)
{
	const IBox& l_rStaticBoxContext=this->getStaticBoxContext();
	IBoxIO& l_rDynamicBoxContext=this->getDynamicBoxContext();

	for(uint32 i = 0; i < l_rStaticBoxContext.getInputCount(); i++)
	{
		for(uint32 j = 0; j < l_rDynamicBoxContext.getInputChunkCount(i); j++)
		{
			m_vStreamDecoder[i].decode(j);
			IMatrix* l_pMatrix = m_vStreamDecoder[i].getOutputMatrix();

			if(m_vStreamDecoder[i].isHeaderReceived())
			{
				if(l_rStaticBoxContext.getSettingCount()>1) m_vStreamEncoder[i].encodeHeader();

				if( m_ui64ValidityCheckerType == OVP_TypeId_ValidityCheckerType_Interpolate.toUInteger() )
				{
					m_ui32TotalInterpolatedSampleCount[i] = 0;
					m_ui32TotalInterpolatedChunkCount[i] = 0;
					// for each channel, save of the last valid sample
					m_vLastValidSample[i].resize(l_pMatrix->getDimensionSize(0));
				}
			}
			if(m_vStreamDecoder[i].isBufferReceived())
			{
				// log warning
				if( m_ui64ValidityCheckerType == OVP_TypeId_ValidityCheckerType_LogWarning.toUInteger() )
				{
					if(!OpenViBEToolkit::Tools::Matrix::isContentValid(*l_pMatrix))
					{
						getLogManager() << m_eLogLevel << "Matrix on input " << i << " either contains NAN or Infinity between " << time64(l_rDynamicBoxContext.getInputChunkStartTime(i, j)) << " and " << time64(l_rDynamicBoxContext.getInputChunkEndTime(i, j)) << ".\n";
					}
				}
				// stop player
				else if( m_ui64ValidityCheckerType == OVP_TypeId_ValidityCheckerType_StopPlayer.toUInteger() )
				{
					if(!OpenViBEToolkit::Tools::Matrix::isContentValid(*l_pMatrix))
					{
						this->getLogManager() << LogLevel_Error << "Matrix on input " << i << " either contains NAN or Infinity between " << time64(l_rDynamicBoxContext.getInputChunkStartTime(i, j)) << " and " << time64(l_rDynamicBoxContext.getInputChunkEndTime(i, j)) << ".\n";
						this->getPlayerContext().stop();
					}
				}
				// interpolate
				else if( m_ui64ValidityCheckerType == OVP_TypeId_ValidityCheckerType_Interpolate.toUInteger() )
				{
					uint32 l_ui32ChannelCount=l_pMatrix->getDimensionSize(0);
					uint32 l_ui32SampleCount=l_pMatrix->getDimensionSize(1);
					float64* l_pBuffer=l_pMatrix->getBuffer();
					uint32 l_ui32InterpolatedSampleCount = 0;

					for(uint32 k=0; k<l_ui32ChannelCount; k++)
					{
						for(uint32 l=0; l<l_ui32SampleCount; l++)
						{
							if( isnan( l_pBuffer[l+k*l_ui32SampleCount] ) || isinf( l_pBuffer[l+k*l_ui32SampleCount] ) )
							{
								// interpolation : order 0 (easiest for online interpolation)
								l_pBuffer[l+k*l_ui32SampleCount] = m_vLastValidSample[i][k];
								l_ui32InterpolatedSampleCount++;
							}
							else
							{
								// save of the last valid sample of channel k
								m_vLastValidSample[i][k] = l_pBuffer[l+k*l_ui32SampleCount];
							}
						}
					}
					m_ui32TotalInterpolatedSampleCount[i] += l_ui32InterpolatedSampleCount;

					// log management
					if (l_ui32InterpolatedSampleCount>0 && m_ui32TotalInterpolatedSampleCount[i]==l_ui32InterpolatedSampleCount) // beginning of interpolation
					{
						getLogManager() << m_eLogLevel << "Matrix on input " << i << " either contains NAN or Infinity from " << time64(l_rDynamicBoxContext.getInputChunkStartTime(i, j)) << ": interpolation is enable.\n";
					}
					if (l_ui32InterpolatedSampleCount>0) // update of ChunkCount during interpolation
					{
						m_ui32TotalInterpolatedChunkCount[i]++;
					}
					if (l_ui32InterpolatedSampleCount==0 && m_ui32TotalInterpolatedSampleCount[i]>0) // end of interpolation
					{
						getLogManager() << m_eLogLevel << "Matrix on input " << i << " contained " << 100.0*m_ui32TotalInterpolatedSampleCount[i]/(m_ui32TotalInterpolatedChunkCount[i]*l_ui32SampleCount*l_ui32ChannelCount) << " % of NAN or Infinity. Interpolation disable from " << time64(l_rDynamicBoxContext.getInputChunkStartTime(i, j)) << ".\n";
						m_ui32TotalInterpolatedSampleCount[i] = 0; // reset
						m_ui32TotalInterpolatedChunkCount[i] = 0;
					}
				}
				else
				{
					this->getLogManager() << LogLevel_Warning << "Unknown action type [" << m_ui64ValidityCheckerType << "].\n";
				}

				if(l_rStaticBoxContext.getSettingCount()>1) m_vStreamEncoder[i].encodeBuffer();
			}
			if(m_vStreamDecoder[i].isEndReceived())
			{
				if(l_rStaticBoxContext.getSettingCount()>1) m_vStreamEncoder[i].encodeEnd();
			}
			if(l_rStaticBoxContext.getSettingCount()>1) l_rDynamicBoxContext.markOutputAsReadyToSend(i, l_rDynamicBoxContext.getInputChunkStartTime(i, j), l_rDynamicBoxContext.getInputChunkEndTime(i, j));
		}
	}

	return true;
}
