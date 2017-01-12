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
		if (A.rows() == 0 && A.cols() == 0)
		{
			return B;
		}
		else
		{
			Eigen::MatrixXd C(A.rows(), A.cols() + B.cols());
			C.block(0, 0, A.rows(), A.cols()) = A;
			C.block(0, A.cols(), B.rows(), B.cols()) = B;
			return C;
		}
	}
};

boolean CBoxAlgorithmXDAWNTrainer::initialize(void)
{
	m_TrainStimulationId = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_FilterFilename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	OV_ERROR_UNLESS_KRF(
		m_FilterFilename.length() != 0,
		"The filter filename is empty.\n",
		OpenViBE::Kernel::ErrorType::BadSetting
		);

	if (FS::Files::fileExists(m_FilterFilename))
	{
		FILE* file = FS::Files::open(m_FilterFilename, "wt");

		OV_ERROR_UNLESS_KRF(
			file != NULL,
			"The filter file exists but cannot be used.\n",
			OpenViBE::Kernel::ErrorType::BadFileRead
			);

		::fclose(file);
	}

	int filterDimension = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);

	OV_ERROR_UNLESS_KRF(
		filterDimension > 0,
		"The dimension of the filter must be strictly positive.\n",
		OpenViBE::Kernel::ErrorType::OutOfBound
		);

	m_FilterDimension = static_cast<unsigned int>(filterDimension);

	m_StimDecoder.initialize(*this, 0);
	m_SignalDecoder[0].initialize(*this, 1);
	m_SignalDecoder[1].initialize(*this, 2);
	m_StimEncoder.initialize(*this, 0);

	return true;
}

boolean CBoxAlgorithmXDAWNTrainer::uninitialize(void)
{
	m_StimDecoder.uninitialize();
	m_SignalDecoder[0].uninitialize();
	m_SignalDecoder[1].uninitialize();
	m_StimEncoder.uninitialize();

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
	IBoxIO& dynamicBoxContext = this->getDynamicBoxContext();

	boolean train = false;

	for (unsigned int i = 0; i < dynamicBoxContext.getInputChunkCount(0); i++)
	{
		m_StimEncoder.getInputStimulationSet()->clear();

		m_StimDecoder.decode(i);

		if (m_StimDecoder.isHeaderReceived())
		{
			m_StimEncoder.encodeHeader();
		}

		if (m_StimDecoder.isBufferReceived())
		{
			for (unsigned int j = 0; j < m_StimDecoder.getOutputStimulationSet()->getStimulationCount(); j++)
			{
				unsigned long long stimulationId = m_StimDecoder.getOutputStimulationSet()->getStimulationIdentifier(j);

				if (stimulationId == m_TrainStimulationId)
				{
					train = true;

					m_StimEncoder.getInputStimulationSet()->appendStimulation(
						OVTK_StimulationId_TrainCompleted,
						m_StimDecoder.getOutputStimulationSet()->getStimulationDate(j),
						0);
				}
			}

			m_StimEncoder.encodeBuffer();
		}

		if (m_StimDecoder.isEndReceived())
		{
			m_StimEncoder.encodeEnd();
		}

		dynamicBoxContext.markOutputAsReadyToSend(0, dynamicBoxContext.getInputChunkStartTime(0, i), dynamicBoxContext.getInputChunkEndTime(0, i));
	}

	if (train)
	{
		std::vector<unsigned int > ERPSampleIndexes;
		Eigen::MatrixXd A;
		Eigen::MatrixXd D, DI;
		Eigen::MatrixXd X[2]; // X[0] is session matrix, X[1] is averaged ERP
		Eigen::MatrixXd C[2]; // Covariance matrices
		unsigned int n[2];
		unsigned int channelCount = 0;
		unsigned int sampleCount = 0;
		unsigned int samplingRate = 0;

		this->getLogManager() << LogLevel_Info << "Received train stimulation...\n";

		// Decodes input signals

		for (unsigned int j = 0; j < 2; j++)
		{
			n[j] = 0;

			for (unsigned int i = 0; i < dynamicBoxContext.getInputChunkCount(j + 1); i++)
			{
				OpenViBEToolkit::TSignalDecoder < CBoxAlgorithmXDAWNTrainer >& m_rSignalDecoder = m_SignalDecoder[j];
				m_rSignalDecoder.decode(i);

				IMatrix* matrix = m_rSignalDecoder.getOutputMatrix();
				channelCount = matrix->getDimensionSize(0);
				sampleCount = matrix->getDimensionSize(1);
				samplingRate = static_cast<uint32>(m_rSignalDecoder.getOutputSamplingRate());

				if (m_rSignalDecoder.isHeaderReceived())
				{
					OV_ERROR_UNLESS_KRF(
						samplingRate > 0,
						"Input sampling frequency is equal to 0. Plugin can not process.\n",
						OpenViBE::Kernel::ErrorType::OutOfBound
						);

					OV_ERROR_UNLESS_KRF(
						channelCount > 0,
						"For condition " << j + 1 << " got no channel in signal stream.\n",
						OpenViBE::Kernel::ErrorType::OutOfBound
						);

					OV_ERROR_UNLESS_KRF(
						sampleCount > 0,
						"For condition " << j + 1 << " got no samples in signal stream.\n",
						OpenViBE::Kernel::ErrorType::OutOfBound
						);

					OV_ERROR_UNLESS_KRF(
						m_FilterDimension <= channelCount,
						"The filter dimension must not be superior than the channel count.\n",
						OpenViBE::Kernel::ErrorType::OutOfBound
						);

					X[j] = Eigen::MatrixXd::Zero(channelCount, sampleCount);
				}

				if (m_rSignalDecoder.isBufferReceived())
				{
					A = Eigen::Map < Eigen::Matrix < double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor > >(matrix->getBuffer(), channelCount, sampleCount);

					switch (j)
					{
						case 0: // Session
							X[j] = X[j] ^ A; // Builds up complete recording session
							break;

						case 1: // ERP
							X[j] = X[j] + A; // Computes sumed ERP

							// $$$ Assumes continuous session signal starting at date 0
							{
								uint32 ERPSampleIndex = static_cast<uint32>(((dynamicBoxContext.getInputChunkStartTime(j + 1, i) >> 16)*samplingRate) >> 16);
								ERPSampleIndexes.push_back(ERPSampleIndex);
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

		unsigned int sampleCountSession = X[0].cols();
		unsigned int sampleCountERP = X[1].cols();

		// Now we compute matrix D

		DI = Eigen::MatrixXd::Identity(sampleCountERP, sampleCountERP);
		D = Eigen::MatrixXd::Zero(sampleCountERP, sampleCountSession);

		for (size_t j = 0; j < ERPSampleIndexes.size(); j++)
		{
			D.block(0, ERPSampleIndexes[j], sampleCountERP, sampleCountERP) += DI;
		}

		// Computes covariance matrices

		C[0] = X[0] * X[0].transpose();
		C[1] = /*Y * Y.transpose();*/ X[1] * /* D.transpose() * */ (D * D.transpose()).fullPivLu().inverse() /* * D */ * X[1].transpose();

		// Solves generalized eigen decomposition

		Eigen::GeneralizedSelfAdjointEigenSolver < Eigen::MatrixXd > eigenSolver(C[0].selfadjointView<Eigen::Lower>(), C[1].selfadjointView<Eigen::Lower>());

		if (eigenSolver.info() != Eigen::Success)
		{
			enum Eigen::ComputationInfo error = eigenSolver.info();
			const char* errorMessage = "unknown";

			switch (error)
			{
				case Eigen::NumericalIssue: errorMessage = "Numerical issue"; break;
				case Eigen::NoConvergence: errorMessage = "No convergence"; break;
					//				case Eigen::InvalidInput: errorMessage="Invalid input"; break; // FIXME
				default: break;
			}

			OV_ERROR_KRF(
				"Could not solve generalized eigen decomposition, got error[" << CString(errorMessage) << "]\n",
				OpenViBE::Kernel::ErrorType::BadProcessing
				);
		}

		// Gets filters

		Eigen::MatrixXd W = eigenSolver.eigenvectors();
		Eigen::VectorXd V = eigenSolver.eigenvalues();
		Eigen::MatrixXd W_forward = W;

		// Saves filters

		FILE* file = FS::Files::open(m_FilterFilename.toASCIIString(), "wt");

		OV_ERROR_UNLESS_KRF(
			file != NULL,
			"Could not open file [" << m_FilterFilename << "] for writing.\n",
			OpenViBE::Kernel::ErrorType::BadFileWrite
			);

		::fprintf(file, "<OpenViBE-SettingsOverride>\n");
		::fprintf(file, "\t<SettingValue>");

		for (unsigned int c = 0; c < m_FilterDimension; c++)
		{
			for (unsigned int r = 0; r < channelCount; r++)
			{
				const char* sep = (r == 0 && c == 0 ? "" : "; ");
				::fprintf(file, "%s%g", sep, W_forward(r, c));
			}
		}

		::fprintf(file, "</SettingValue>\n");
		::fprintf(file, "\t<SettingValue>%u</SettingValue>\n", m_FilterDimension);
		::fprintf(file, "\t<SettingValue>%u</SettingValue>\n", channelCount);
		::fprintf(file, "</OpenViBE-SettingsOverride>");

		OV_WARNING_UNLESS_K(
			::fclose(file) == 0,
			"Could not close file[" << m_FilterFilename << "].\n"
			);

		this->getLogManager() << LogLevel_Info << "Training finished and saved to [" << m_FilterFilename << "]!\n";
	}

	return true;
}
