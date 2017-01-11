#include "ovpCBoxAlgorithmXDAWNTrainer.h"

#include "fs/Files.h"

#include <cstdio>
#include <iostream>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::SignalProcessing;

namespace
{
	Eigen::MatrixXd operator ^ (const Eigen::MatrixXd& A, const Eigen::MatrixXd& B)
	{
		Eigen::MatrixXd C(A.rows(), A.cols() + B.cols());
		C.block(0, 0, A.rows(), A.cols()) = A;
		C.block(0, A.cols(), B.rows(), B.cols()) = B;
		return C;
	}
};

boolean CBoxAlgorithmXDAWNTrainer::initialize(void)
{
	m_ui64TrainStimulationId = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_sFilterFilename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	OV_ERROR_UNLESS_KRF(
		FS::Files::fileExists(m_sFilterFilename),
		"The filter file does not exist.\n",
		OpenViBE::Kernel::ErrorType::BadFileRead
		);

	int32 l_i32FilterDimension = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);

	OV_ERROR_UNLESS_KRF(
		m_ui32FilterDimension > 0,
		"The dimension of the filter must be strictly positive.\n",
		OpenViBE::Kernel::ErrorType::OutOfBound
		);

	m_ui32FilterDimension = static_cast<uint32>(l_i32FilterDimension);
	
	m_oStimDecoder.initialize(*this, 0);
	m_oSignalDecoder[0].initialize(*this, 1);
	m_oSignalDecoder[1].initialize(*this, 2);
	m_oStimEncoder.initialize(*this, 0);

	return true;
}

boolean CBoxAlgorithmXDAWNTrainer::uninitialize(void)
{
	m_oStimDecoder.uninitialize();
	m_oSignalDecoder[0].uninitialize();
	m_oSignalDecoder[1].uninitialize();
	m_oStimEncoder.uninitialize();

	return true;
}

boolean CBoxAlgorithmXDAWNTrainer::processInput(uint32 ui32InputIndex)
{
	if (ui32InputIndex == 0)
	{
		this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	}

	return true;
}

boolean CBoxAlgorithmXDAWNTrainer::process(void)
{
	IBoxIO& l_rDynamicBoxContext = this->getDynamicBoxContext();

	boolean l_bTrain = false;

	for (uint32 i = 0; i < l_rDynamicBoxContext.getInputChunkCount(0); i++)
	{
		m_oStimEncoder.getInputStimulationSet()->clear();

		m_oStimDecoder.decode(i);

		if (m_oStimDecoder.isHeaderReceived())
		{
			m_oStimEncoder.encodeHeader();
		}

		if (m_oStimDecoder.isBufferReceived())
		{
			for (uint32 j = 0; j < m_oStimDecoder.getOutputStimulationSet()->getStimulationCount(); j++)
			{
				uint64 l_ui64StimulationId = m_oStimDecoder.getOutputStimulationSet()->getStimulationIdentifier(j);
				
				if (l_ui64StimulationId == m_ui64TrainStimulationId)
				{
					l_bTrain = true;

					m_oStimEncoder.getInputStimulationSet()->appendStimulation(
						OVTK_StimulationId_TrainCompleted,
						m_oStimDecoder.getOutputStimulationSet()->getStimulationDate(j),
						0);
				}
			}

			m_oStimEncoder.encodeBuffer();
		}

		if (m_oStimDecoder.isEndReceived())
		{
			m_oStimEncoder.encodeEnd();
		}

		l_rDynamicBoxContext.markOutputAsReadyToSend(0, l_rDynamicBoxContext.getInputChunkStartTime(0, i), l_rDynamicBoxContext.getInputChunkEndTime(0, i));
	}

	if (l_bTrain)
	{
		std::vector<uint32>l_vERPSampleIndex;
		Eigen::MatrixXd A;
		Eigen::MatrixXd D, DI;
		Eigen::MatrixXd X[2]; // X[0] is session matrix, X[1] is averaged ERP
		Eigen::MatrixXd C[2]; // Covariance matrices
		uint32 n[2];
		uint32 l_ui32ChannelCount = 0;
		uint32 l_ui32SampleCount = 0;
		uint32 l_ui32SamplingRate = 0;

		this->getLogManager() << LogLevel_Info << "Received train stimulation...\n";

		// Decodes input signals

		for (size_t j = 0; j < 2; j++)
		{
			n[j] = 0;

			for (unsigned int i = 0; i < l_rDynamicBoxContext.getInputChunkCount(j + 1); i++)
			{
				OpenViBEToolkit::TSignalDecoder < CBoxAlgorithmXDAWNTrainer >& m_rSignalDecoder = m_oSignalDecoder[j];
				m_rSignalDecoder.decode(i);

				IMatrix* l_pMatrix = m_rSignalDecoder.getOutputMatrix();
				l_ui32ChannelCount = l_pMatrix->getDimensionSize(0);
				l_ui32SampleCount = l_pMatrix->getDimensionSize(1);
				l_ui32SamplingRate = static_cast<uint32>(m_rSignalDecoder.getOutputSamplingRate());

				if (m_rSignalDecoder.isHeaderReceived())
				{
					OV_ERROR_UNLESS_KRF(
						l_ui32SamplingRate > 0,
						"Input sampling frequency is equal to 0. Plugin can not process.\n",
						OpenViBE::Kernel::ErrorType::OutOfBound
						);

					OV_ERROR_UNLESS_KRF(
						l_ui32ChannelCount > 0,
						"For condition " << j + 1 << " got no channel in signal stream.\n",
						OpenViBE::Kernel::ErrorType::OutOfBound
						);

					OV_ERROR_UNLESS_KRF(
						l_ui32SampleCount > 0,
						"For condition " << j + 1 << " got no samples in signal stream.\n",
						OpenViBE::Kernel::ErrorType::OutOfBound
						);

					OV_ERROR_UNLESS_KRF(
						m_ui32FilterDimension <= l_ui32ChannelCount,
						"The filter dimension must not be superior than the channel count.\n",
						OpenViBE::Kernel::ErrorType::OutOfBound
						);

					X[j] = Eigen::MatrixXd::Zero(l_ui32ChannelCount, l_ui32SampleCount);
				}

				if (m_rSignalDecoder.isBufferReceived())
				{
					A = Eigen::Map < Eigen::Matrix < double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor > >(l_pMatrix->getBuffer(), l_ui32ChannelCount, l_ui32SampleCount);
					
					switch (j)
					{
						case 0: // Session
							X[j] = X[j] ^ A; // Builds up complete recording session
							break;
						
						case 1: // ERP
							X[j] = X[j] + A; // Computes sumed ERP

							// $$$ Assumes continuous session signal starting at date 0
							{
								uint32 l_ui32ERPSampleIndex = static_cast<uint32>(((l_rDynamicBoxContext.getInputChunkStartTime(j + 1, i) >> 16)*l_ui32SamplingRate) >> 16);
								l_vERPSampleIndex.push_back(l_ui32ERPSampleIndex);
							}
							break;
						
						default:
							break;
					}

					n[j]++;
				}

#if 0
				if (m_rSignalDecoder.isEndReceived())
				{
				}
#endif
			}

			OV_ERROR_UNLESS_KRF(
				n[j] != 0,
				"Did not have input signal for condition " << j + 1 << "\n",
				OpenViBE::Kernel::ErrorType::BadValue
				);

			switch (j)
			{
				case 0: // Session
					break;

				case 1: // ERP
					X[j] = X[j] / n[j]; // Averages ERP
					break;

				default:
					break;
			}
		}

		// We need equal number of channels
		OV_ERROR_UNLESS_KRF(
			X[0].rows() != X[1].rows(),
			"Dimension mismatch, first input had " << uint32(X[0].rows()) << " channels while second input had " << uint32(X[1].rows()) << " channels\n",
			OpenViBE::Kernel::ErrorType::BadValue
			);

		// Grabs usefull values

		uint32 l_ui32SampleCountSession = X[0].cols();
		uint32 l_ui32SampleCountERP = X[1].cols();

		// Now we compute matrix D

		DI = Eigen::MatrixXd::Identity(l_ui32SampleCountERP, l_ui32SampleCountERP);
		D = Eigen::MatrixXd::Zero(l_ui32SampleCountERP, l_ui32SampleCountSession);
		
		for (size_t j = 0; j < l_vERPSampleIndex.size(); j++)
		{
			D.block(0, l_vERPSampleIndex[j], l_ui32SampleCountERP, l_ui32SampleCountERP) += DI;
		}

		// Computes covariance matrices

		C[0] = X[0] * X[0].transpose();
		C[1] = /*Y * Y.transpose();*/ X[1] * /* D.transpose() * */ (D * D.transpose()).fullPivLu().inverse() /* * D */ * X[1].transpose();

		// Solves generalized eigen decomposition

		Eigen::GeneralizedSelfAdjointEigenSolver < Eigen::MatrixXd > l_oEigenSolver(C[0].selfadjointView<Eigen::Lower>(), C[1].selfadjointView<Eigen::Lower>());
		
		if (l_oEigenSolver.info() != Eigen::Success)
		{
			enum Eigen::ComputationInfo l_iError = l_oEigenSolver.info();
			const char* l_sError = "unknown";
			
			switch (l_iError)
			{
				case Eigen::NumericalIssue: l_sError = "Numerical issue"; break;
				case Eigen::NoConvergence: l_sError = "No convergence"; break;
					//				case Eigen::InvalidInput: l_sError="Invalid input"; break; // FIXME
				default: break;
			}

			OV_ERROR_KRF(
				"Could not solve generalized eigen decomposition, got error[" << CString(l_sError) << "]\n",
				OpenViBE::Kernel::ErrorType::BadProcessing
				);
		}

		// Gets filters

		Eigen::MatrixXd W = l_oEigenSolver.eigenvectors();
		Eigen::VectorXd V = l_oEigenSolver.eigenvalues();
		Eigen::MatrixXd W_forward = W;

		// Saves filters

		FILE* l_pFile = FS::Files::open(m_sFilterFilename.toASCIIString(), "wt");

		OV_ERROR_UNLESS_KRF(
			l_pFile != NULL,
			"Could not open file [" << m_sFilterFilename << "] for writing.\n",
			OpenViBE::Kernel::ErrorType::BadFileWrite
			);

		::fprintf(l_pFile, "<OpenViBE-SettingsOverride>\n");
		::fprintf(l_pFile, "\t<SettingValue>");

		for (uint32 c = 0; c < m_ui32FilterDimension; c++)
		{
			for (uint32 r = 0; r < l_ui32ChannelCount; r++)
			{
				const char* sep = (r == 0 && c == 0 ? "" : "; ");
				::fprintf(l_pFile, "%s%g", sep, W_forward(r, c));
			}
		}

		::fprintf(l_pFile, "</SettingValue>\n");
		::fprintf(l_pFile, "\t<SettingValue>%i</SettingValue>\n", m_ui32FilterDimension);
		::fprintf(l_pFile, "\t<SettingValue>%i</SettingValue>\n", l_ui32ChannelCount);
		::fprintf(l_pFile, "</OpenViBE-SettingsOverride>");

		OV_WARNING_UNLESS_K(
			::fclose(l_pFile) == 0,
			"Could not close file[" << m_sFilterFilename << "].\n"
			);

		this->getLogManager() << LogLevel_Info << "Training finished and saved to [" << m_sFilterFilename << "]!\n";
	}

	return true;
}
