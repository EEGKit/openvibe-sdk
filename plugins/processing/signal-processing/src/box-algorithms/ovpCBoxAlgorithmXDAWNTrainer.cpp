#include "ovpCBoxAlgorithmXDAWNTrainer.h"

#include "fs/Files.h"

#include <cstdio>
#include <iostream>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace SignalProcessing;

CBoxAlgorithmXDAWNTrainer::CBoxAlgorithmXDAWNTrainer() {}

bool CBoxAlgorithmXDAWNTrainer::initialize()
{
	m_trainStimulationID = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_filterFilename     = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	OV_ERROR_UNLESS_KRF(m_filterFilename.length() != 0, "The filter filename is empty.\n", Kernel::ErrorType::BadSetting);

	if (FS::Files::fileExists(m_filterFilename))
	{
		FILE* file = FS::Files::open(m_filterFilename, "wt");

		OV_ERROR_UNLESS_KRF(file != nullptr, "The filter file exists but cannot be used.\n", Kernel::ErrorType::BadFileRead);

		fclose(file);
	}

	const int filterDimension = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);

	OV_ERROR_UNLESS_KRF(filterDimension > 0, "The dimension of the filter must be strictly positive.\n", Kernel::ErrorType::OutOfBound);

	m_filterDim = size_t(filterDimension);

	m_saveAsBoxConfig = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);


	m_stimDecoder.initialize(*this, 0);
	m_signalDecoder[0].initialize(*this, 1);
	m_signalDecoder[1].initialize(*this, 2);
	m_stimEncoder.initialize(*this, 0);

	return true;
}

bool CBoxAlgorithmXDAWNTrainer::uninitialize()
{
	m_stimDecoder.uninitialize();
	m_signalDecoder[0].uninitialize();
	m_signalDecoder[1].uninitialize();
	m_stimEncoder.uninitialize();

	return true;
}

bool CBoxAlgorithmXDAWNTrainer::processInput(const size_t index)
{
	if (index == 0) { this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess(); }

	return true;
}

bool CBoxAlgorithmXDAWNTrainer::process()
{
	IBoxIO& boxCtx = this->getDynamicBoxContext();

	bool train = false;

	for (size_t i = 0; i < boxCtx.getInputChunkCount(0); ++i)
	{
		m_stimEncoder.getInputStimulationSet()->clear();
		m_stimDecoder.decode(i);

		if (m_stimDecoder.isHeaderReceived()) { m_stimEncoder.encodeHeader(); }
		if (m_stimDecoder.isBufferReceived())
		{
			CStimulationSet& set = *m_stimDecoder.getOutputStimulationSet();
			for (size_t j = 0; j < set.size(); ++j)
			{
				const uint64_t stimulationId = set[j].m_ID;

				if (stimulationId == m_trainStimulationID)
				{
					train = true;

					m_stimEncoder.getInputStimulationSet()->append(OVTK_StimulationId_TrainCompleted, set[j].m_Date, 0);
				}
			}

			m_stimEncoder.encodeBuffer();
		}
		if (m_stimDecoder.isEndReceived()) { m_stimEncoder.encodeEnd(); }

		boxCtx.markOutputAsReadyToSend(0, boxCtx.getInputChunkStartTime(0, i), boxCtx.getInputChunkEndTime(0, i));
	}

	if (train)
	{
		std::vector<size_t> erpSampleIndexes;
		std::array<Eigen::MatrixXd, 2> X; // X[0] is session matrix, X[1] is averaged ERP
		std::array<Eigen::MatrixXd, 2> C; // Covariance matrices
		std::array<size_t, 2> n;
		size_t nChannel = 0;

		this->getLogManager() << LogLevel_Info << "Received train stimulation...\n";

		// Decodes input signals

		for (size_t j = 0; j < 2; ++j)
		{
			n[j] = 0;

			for (size_t i = 0; i < boxCtx.getInputChunkCount(j + 1); ++i)
			{
				Toolkit::TSignalDecoder<CBoxAlgorithmXDAWNTrainer>& decoder = m_signalDecoder[j];
				decoder.decode(i);

				IMatrix* matrix       = decoder.getOutputMatrix();
				nChannel              = matrix->getDimensionSize(0);
				const size_t nSample  = matrix->getDimensionSize(1);
				const size_t sampling = size_t(decoder.getOutputSamplingRate());

				if (decoder.isHeaderReceived())
				{
					OV_ERROR_UNLESS_KRF(sampling > 0, "Input sampling frequency is equal to 0. Plugin can not process.\n", Kernel::ErrorType::OutOfBound);
					OV_ERROR_UNLESS_KRF(nChannel > 0, "For condition " << j + 1 << " got no channel in signal stream.\n", Kernel::ErrorType::OutOfBound);
					OV_ERROR_UNLESS_KRF(nSample > 0, "For condition " << j + 1 << " got no samples in signal stream.\n", Kernel::ErrorType::OutOfBound);
					OV_ERROR_UNLESS_KRF(m_filterDim <= nChannel, "The filter dimension must not be superior than the channel count.\n",
										Kernel::ErrorType::OutOfBound);

					if (!n[0]) // Initialize signal buffer (X[0]) only when receiving input signal header.
					{
						X[j].resize(nChannel, (boxCtx.getInputChunkCount(j + 1) - 1) * nSample);
					}
					else // otherwise, only ERP averaging buffer (X[1]) is reset
					{
						X[j] = Eigen::MatrixXd::Zero(nChannel, nSample);
					}
				}

				if (decoder.isBufferReceived())
				{
					Eigen::MatrixXd A = Eigen::Map<Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>(
						matrix->getBuffer(), nChannel, nSample);

					switch (j)
					{
						case 0: // Session							
							X[j].block(0, n[j] * A.cols(), A.rows(), A.cols()) = A;
							break;

						case 1: // ERP
							X[j] = X[j] + A; // Computes sumed ERP

							// $$$ Assumes continuous session signal starting at date 0
							{
								size_t erpSampleIdx = size_t(((boxCtx.getInputChunkStartTime(j + 1, i).time() >> 16) * sampling) >> 16);
								erpSampleIndexes.push_back(erpSampleIdx);
							}
							break;

						default:
							break;
					}

					n[j]++;
				}

#if 0
				if (decoder.isEndReceived())
				{
				}
#endif
			}

			OV_ERROR_UNLESS_KRF(n[j] != 0, "Did not have input signal for condition " << j + 1 << "\n", Kernel::ErrorType::BadValue);

			switch (j)
			{
				case 0: // Session
					break;

				case 1: // ERP
					X[j] = X[j] / double(n[j]); // Averages ERP
					break;

				default:
					break;
			}
		}

		// We need equal number of channels
		OV_ERROR_UNLESS_KRF(X[0].rows() == X[1].rows(),
							"Dimension mismatch, first input had " << size_t(X[0].rows()) << " channels while second input had " << size_t(X[1].rows()) <<
							" channels\n",
							Kernel::ErrorType::BadValue);

		// Grabs usefull values

		const size_t sampleCountSession = X[0].cols();
		const size_t sampleCountERP     = X[1].cols();

		// Now we compute matrix D

		const Eigen::MatrixXd DI = Eigen::MatrixXd::Identity(sampleCountERP, sampleCountERP);
		Eigen::MatrixXd D        = Eigen::MatrixXd::Zero(sampleCountERP, sampleCountSession);

		for (size_t sampleIndex : erpSampleIndexes) { D.block(0, sampleIndex, sampleCountERP, sampleCountERP) += DI; }

		// Computes covariance matrices

		C[0] = X[0] * X[0].transpose();
		C[1] = /*Y * Y.transpose();*/ X[1] * /* D.transpose() * */ (D * D.transpose()).fullPivLu().inverse() /* * D */ * X[1].transpose();

		// Solves generalized eigen decomposition

		const Eigen::GeneralizedSelfAdjointEigenSolver<Eigen::MatrixXd> eigenSolver(C[0].selfadjointView<Eigen::Lower>(), C[1].selfadjointView<Eigen::Lower>());

		if (eigenSolver.info() != Eigen::Success)
		{
			const enum Eigen::ComputationInfo error = eigenSolver.info();
			const char* errorMessage                = "unknown";

			switch (error)
			{
				case Eigen::NumericalIssue: errorMessage = "Numerical issue";
					break;
				case Eigen::NoConvergence: errorMessage = "No convergence";
					break;
					// case Eigen::InvalidInput: errorMessage="Invalid input"; break; // FIXME
				default: break;
			}

			OV_ERROR_KRF("Could not solve generalized eigen decomposition, got error[" << CString(errorMessage) << "]\n",
						 Kernel::ErrorType::BadProcessing);
		}

		// Create a CMatrix mapper that can spool the filters to a file

		CMatrix eigenVectors;
		eigenVectors.setDimensionCount(2);
		eigenVectors.setDimensionSize(0, m_filterDim);
		eigenVectors.setDimensionSize(1, nChannel);

		Eigen::Map<MatrixXdRowMajor> vectorsMapper(eigenVectors.getBuffer(), m_filterDim, nChannel);

		vectorsMapper.block(0, 0, m_filterDim, nChannel) = eigenSolver.eigenvectors().block(0, 0, nChannel, m_filterDim).transpose();

		// Saves filters

		FILE* file = FS::Files::open(m_filterFilename.toASCIIString(), "wt");

		OV_ERROR_UNLESS_KRF(file != nullptr, "Could not open file [" << m_filterFilename << "] for writing.\n", Kernel::ErrorType::BadFileWrite);

		if (m_saveAsBoxConfig)
		{
			fprintf(file, "<OpenViBE-SettingsOverride>\n");
			fprintf(file, "\t<SettingValue>");

			for (size_t i = 0; i < eigenVectors.getBufferElementCount(); ++i) { fprintf(file, "%e ", eigenVectors.getBuffer()[i]); }

			fprintf(file, "</SettingValue>\n");
			fprintf(file, "\t<SettingValue>%zu</SettingValue>\n", m_filterDim);
			fprintf(file, "\t<SettingValue>%zu</SettingValue>\n", nChannel);
			fprintf(file, "\t<SettingValue></SettingValue>\n");
			fprintf(file, "</OpenViBE-SettingsOverride>");
		}
		else
		{
			OV_ERROR_UNLESS_KRF(Toolkit::Matrix::saveToTextFile(eigenVectors, m_filterFilename),
								"Unable to save to [" << m_filterFilename << "]\n", Kernel::ErrorType::BadFileWrite);
		}

		OV_WARNING_UNLESS_K(::fclose(file) == 0, "Could not close file[" << m_filterFilename << "].\n");

		this->getLogManager() << LogLevel_Info << "Training finished and saved to [" << m_filterFilename << "]!\n";
	}

	return true;
}
