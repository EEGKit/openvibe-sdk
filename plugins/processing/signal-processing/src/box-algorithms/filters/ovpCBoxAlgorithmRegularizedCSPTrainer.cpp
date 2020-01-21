#if defined TARGET_HAS_ThirdPartyEIGEN

#include "ovpCBoxAlgorithmRegularizedCSPTrainer.h"

#include <sstream>
#include <cstdio>

#include <Eigen/Eigenvalues>
#include <fs/Files.h>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

using namespace Eigen;

// typedef Eigen::Matrix< double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor > MatrixXdRowMajor;

CBoxAlgorithmRegularizedCSPTrainer::CBoxAlgorithmRegularizedCSPTrainer() : m_tikhonov(0.0) {}

bool CBoxAlgorithmRegularizedCSPTrainer::initialize()
{
	m_stimDecoder.initialize(*this, 0);
	m_encoder.initialize(*this, 0);

	const IBox& staticBoxContext = this->getStaticBoxContext();

	m_nClasses = staticBoxContext.getInputCount() - 1;

	m_covProxies.resize(m_nClasses);

	m_stimID          = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_configFilename  = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_filtersPerClass = size_t(uint64_t(FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2)));
	m_saveAsBoxConf   = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);

	// @FIXME CERT
	OV_ERROR_UNLESS_KRF(m_filtersPerClass > 0, // && m_filtersPerClass%2 == 0,
						"Invalid filter dimension number [" << m_filtersPerClass << "] (expected value > 0)", // even ?
						OpenViBE::Kernel::ErrorType::BadSetting);

	m_hasBeenInitialized = true;


	m_signalDecoders.resize(m_nClasses);
	for (size_t i = 0; i < m_nClasses; ++i)
	{
		m_signalDecoders[i].initialize(*this, i + 1);

		const CIdentifier covAlgId = this->getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_OnlineCovariance);
		OV_ERROR_UNLESS_KRF(covAlgId != OV_UndefinedIdentifier, "Failed to create online covariance algorithm",
							OpenViBE::Kernel::ErrorType::BadResourceCreation);


		m_covProxies[i].cov = &this->getAlgorithmManager().getAlgorithm(covAlgId);
		OV_ERROR_UNLESS_KRF(m_covProxies[i].cov->initialize(), "Failed to initialize online covariance algorithm",
							OpenViBE::Kernel::ErrorType::Internal);


		// Set the params of the cov algorithm
		TParameterHandler<uint64_t> updateMethod(m_covProxies[i].cov->getInputParameter(OVP_Algorithm_OnlineCovariance_InputParameterId_UpdateMethod));
		TParameterHandler<bool> traceNormalization(m_covProxies[i].cov->getInputParameter(OVP_Algorithm_OnlineCovariance_InputParameterId_TraceNormalization));
		TParameterHandler<double> shrinkage(m_covProxies[i].cov->getInputParameter(OVP_Algorithm_OnlineCovariance_InputParameterId_Shrinkage));

		updateMethod       = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 4);
		traceNormalization = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 5);
		shrinkage          = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 6);
	}

	m_tikhonov = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 7);

	OV_ERROR_UNLESS_KRF(m_configFilename != CString(""), "Output filename is required in box configuration",
						OpenViBE::Kernel::ErrorType::BadSetting);

	return true;
}

bool CBoxAlgorithmRegularizedCSPTrainer::uninitialize()
{
	m_stimDecoder.uninitialize();
	m_encoder.uninitialize();

	if (m_hasBeenInitialized)
	{
		for (size_t i = 0; i < m_nClasses; ++i)
		{
			m_signalDecoders[i].uninitialize();
			if (m_covProxies[i].cov)
			{
				m_covProxies[i].cov->uninitialize();
				getAlgorithmManager().releaseAlgorithm(*m_covProxies[i].cov);
			}
		}
	}
	m_covProxies.clear();

	return true;
}

bool CBoxAlgorithmRegularizedCSPTrainer::processInput(const size_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmRegularizedCSPTrainer::updateCov(const size_t index)
{
	IBoxIO& dynamicBoxContext = this->getDynamicBoxContext();
	SIncrementalCovarianceProxy& curCovProxy(m_covProxies[index]);
	for (size_t i = 0; i < dynamicBoxContext.getInputChunkCount(index + 1); ++i)
	{
		OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmRegularizedCSPTrainer>* decoder = &m_signalDecoders[index];
		const IMatrix* inputSignal                                                   = decoder->getOutputMatrix();

		decoder->decode(i);
		if (decoder->isHeaderReceived())
		{
			TParameterHandler<IMatrix*> featureVectorSet(
				curCovProxy.cov->getInputParameter(OVP_Algorithm_OnlineCovariance_InputParameterId_InputVectors));

			featureVectorSet->setDimensionCount(2);
			featureVectorSet->setDimensionSize(0, inputSignal->getDimensionSize(1));
			featureVectorSet->setDimensionSize(1, inputSignal->getDimensionSize(0));

			OV_ERROR_UNLESS_KRF(m_filtersPerClass <= inputSignal->getDimensionSize(0),
								"Invalid CSP filter dimension of [" << m_filtersPerClass << "] for stream " << i+1 <<
								" (expected value must be less than input channel count ["<< inputSignal->getDimensionSize(1) <<"])",
								OpenViBE::Kernel::ErrorType::BadSetting);

			curCovProxy.cov->activateInputTrigger(OVP_Algorithm_OnlineCovariance_Process_Reset, true);

			OV_ERROR_UNLESS_KRF(curCovProxy.cov->process(), "Failed to parametrize covariance algorithm", OpenViBE::Kernel::ErrorType::Internal);
		}
		if (decoder->isBufferReceived())
		{
			TParameterHandler<IMatrix*> ip_pFeatureVectorSet(curCovProxy.cov->getInputParameter(OVP_Algorithm_OnlineCovariance_InputParameterId_InputVectors));

			// transpose data
			const size_t nChannels = inputSignal->getDimensionSize(0);
			const size_t nSamples  = inputSignal->getDimensionSize(1);

			const Map<MatrixXdRowMajor> inputMapper(const_cast<double*>(inputSignal->getBuffer()), nChannels, nSamples);
			Map<MatrixXdRowMajor> outputMapper(ip_pFeatureVectorSet->getBuffer(), nSamples, nChannels);
			outputMapper = inputMapper.transpose();

			curCovProxy.cov->activateInputTrigger(OVP_Algorithm_OnlineCovariance_Process_Update, true);
			curCovProxy.cov->process();

			curCovProxy.nBuffers++;
			curCovProxy.nSamples += nSamples;
		}
		if (decoder->isEndReceived())
		{
			// nop
		}
	}

	return true;
}

//
// Returns a sample-weighted average of given covariance matrices that does not include the cov of SkipIndex
//
// @todo error handling is a bit scarce
//
// @note This will recompute the weights on every call, but given how small amount of
// computations we're speaking of, there's not much point in optimizing.
//
bool CBoxAlgorithmRegularizedCSPTrainer::outclassCovAverage(const size_t skipIndex, const std::vector<MatrixXd>& cov, MatrixXd& covAvg)
{
	if (cov.empty() || skipIndex >= cov.size()) { return false; }

	std::vector<double> classWeights;
	uint64_t totalOutclassSamples = 0;

	// Compute the total number of samples
	for (size_t i = 0; i < m_nClasses; ++i) { if (i != skipIndex) { totalOutclassSamples += m_covProxies[i].nSamples; } }

	// Compute weigths for averaging
	classWeights.resize(m_nClasses);
	for (size_t i = 0; i < m_nClasses; ++i)
	{
		classWeights[i] = i == skipIndex ? 0 : m_covProxies[i].nSamples / double(totalOutclassSamples);
		this->getLogManager() << LogLevel_Debug << "Condition " << i + 1 << " averaging weight = " << classWeights[i] << "\n";
	}

	// Average the covs
	covAvg.resizeLike(cov[0]);
	covAvg.setZero();
	for (size_t i = 0; i < m_nClasses; ++i) { covAvg += (classWeights[i] * cov[i]); }

	return true;
}

bool CBoxAlgorithmRegularizedCSPTrainer::computeCSP(const std::vector<MatrixXd>& cov, std::vector<MatrixXd>& sortedEigenVectors,
													std::vector<VectorXd>& sortedEigenValues)
{
	// We wouldn't need to store all this -- they are kept for debugging purposes
	std::vector<VectorXd> eigenValues(m_nClasses);
	std::vector<MatrixXd> eigenVectors(m_nClasses);
	std::vector<MatrixXd> covInv(m_nClasses);
	std::vector<MatrixXd> covProd(m_nClasses);
	MatrixXd tikhonov;
	MatrixXd outclassCov;
	tikhonov.resizeLike(cov[0]);
	tikhonov.setIdentity();
	tikhonov *= m_tikhonov;

	sortedEigenVectors.resize(m_nClasses);
	sortedEigenValues.resize(m_nClasses);

	// To get the CSP filters, we compute two sets of eigenvectors,
	// eig(inv(sigma2+tikhonov)*sigma1) and eig(inv(sigma1+tikhonov)*sigma2
	// and pick the ones corresponding to the largest eigenvalues as
	// spatial filters [following Lotte & Guan 2011]. Assumes the shrink
	// of the sigmas (if its used) has been performed inside the cov
	// computation algorithm.

	EigenSolver<MatrixXd> eigenSolverGeneral;

	for (size_t classIndex = 0; classIndex < m_nClasses; ++classIndex)
	{
		try { covInv[classIndex] = (cov[classIndex] + tikhonov).inverse(); }
		catch (...) { OV_ERROR_KRF("Inversion failed for condition [" << classIndex+1 << "]", OpenViBE::Kernel::ErrorType::BadProcessing); }

		// Compute covariance in all the classes except 'classIndex'.
		OV_ERROR_UNLESS_KRF(outclassCovAverage(classIndex, cov, outclassCov), "Outclass cov computation failed for condition [" << classIndex + 1 << "]",
							OpenViBE::Kernel::ErrorType::BadProcessing);

		covProd[classIndex] = covInv[classIndex] * outclassCov;

		try { eigenSolverGeneral.compute(covProd[classIndex]); }
		catch (...) { OV_ERROR_KRF("EigenSolver failed for condition [" << classIndex + 1 << "]", OpenViBE::Kernel::ErrorType::BadProcessing); }

		eigenValues[classIndex]  = eigenSolverGeneral.eigenvalues().real();
		eigenVectors[classIndex] = eigenSolverGeneral.eigenvectors().real();

		// Sort the vectors -_-
		std::vector<std::pair<double, int>> indexes;
		indexes.reserve(eigenValues[classIndex].size());
		for (int i = 0; i < eigenValues[classIndex].size(); ++i) { indexes.emplace_back(std::make_pair((eigenValues[classIndex])[i], i)); }
		std::sort(indexes.begin(), indexes.end(), std::greater<std::pair<double, int>>());

		sortedEigenValues[classIndex].resizeLike(eigenValues[classIndex]);
		sortedEigenVectors[classIndex].resizeLike(eigenVectors[classIndex]);
		for (int i = 0; i < eigenValues[classIndex].size(); ++i)
		{
			sortedEigenValues[classIndex][i]      = eigenValues[classIndex][indexes[i].second];
			sortedEigenVectors[classIndex].col(i) = eigenVectors[classIndex].col(indexes[i].second);
			// this->getLogManager() << LogLevel_Info << "E " << i << " " << (l_oSortedEigenValues[classIndex])[i] << "\n";
		}
	}
	return true;
}

bool CBoxAlgorithmRegularizedCSPTrainer::process()
{
	IBoxIO& boxContext = this->getDynamicBoxContext();

	bool shouldTrain = false;
	uint64_t date    = 0, startTime = 0, endTime = 0;

	// Handle input stimulations
	for (size_t i = 0; i < boxContext.getInputChunkCount(0); ++i)
	{
		m_stimDecoder.decode(i);
		if (m_stimDecoder.isHeaderReceived())
		{
			m_encoder.encodeHeader();
			boxContext.markOutputAsReadyToSend(0, boxContext.getInputChunkStartTime(0, i), boxContext.getInputChunkEndTime(0, i));
		}
		if (m_stimDecoder.isBufferReceived())
		{
			const TParameterHandler<IStimulationSet*> stimSet(m_stimDecoder.getOutputStimulationSet());
			for (size_t j = 0; j < stimSet->getStimulationCount(); ++j)
			{
				if (stimSet->getStimulationIdentifier(j) == m_stimID)
				{
					date        = stimSet->getStimulationDate(stimSet->getStimulationCount() - 1);
					startTime   = boxContext.getInputChunkStartTime(0, i);
					endTime     = boxContext.getInputChunkEndTime(0, i);
					shouldTrain = true;
					break;
				}
			}
		}
		if (m_stimDecoder.isEndReceived()) { m_encoder.encodeEnd(); }
	}

	// Update all covs with the current data chunks (if any)
	for (size_t i = 0; i < m_nClasses; ++i) { if (!updateCov(i)) { return false; } }

	if (shouldTrain)
	{
		this->getLogManager() << LogLevel_Info << "Received train stimulation - be patient\n";

		const IMatrix* input   = m_signalDecoders[0].getOutputMatrix();
		const size_t nChannels = input->getDimensionSize(0);

		this->getLogManager() << LogLevel_Debug << "Computing eigen vector decomposition...\n";

		// Get out the covariances
		std::vector<MatrixXd> cov(m_nClasses);

		for (size_t i = 0; i < m_nClasses; ++i)
		{
			OV_ERROR_UNLESS_KRF(m_covProxies[i].nSamples >= 2,
								"Invalid sample count of [" <<m_covProxies[i].nSamples << "] for condition number " << i << " (expected value > 2)",
								OpenViBE::Kernel::ErrorType::BadProcessing);

			TParameterHandler<IMatrix*> op_covarianceMatrix(
				m_covProxies[i].cov->getOutputParameter(OVP_Algorithm_OnlineCovariance_OutputParameterId_CovarianceMatrix));

			// Get regularized cov
			m_covProxies[i].cov->activateInputTrigger(OVP_Algorithm_OnlineCovariance_Process_GetCov, true);
			OV_ERROR_UNLESS_KRF(m_covProxies[i].cov->process(), "Failed to retrieve regularized covariance", OpenViBE::Kernel::ErrorType::Internal);

			const Map<MatrixXdRowMajor> covMapper(op_covarianceMatrix->getBuffer(), nChannels, nChannels);
			cov[i] = covMapper;

			// Get vanilla cov
			m_covProxies[i].cov->activateInputTrigger(OVP_Algorithm_OnlineCovariance_Process_GetCovRaw, true);
			OV_ERROR_UNLESS_KRF(m_covProxies[i].cov->process(), "Failed to retrieve vanilla covariance", OpenViBE::Kernel::ErrorType::Internal);
		}

		// Sanity check
		for (size_t i = 1; i < m_nClasses; ++i)
		{
			OV_ERROR_UNLESS_KRF(cov[i-1].rows() == cov[i].rows() && cov[i-1].cols() == cov[i].cols(),
								"Mismatch between the number of channel in both input streams", OpenViBE::Kernel::ErrorType::BadValue);
		}

		this->getLogManager() << LogLevel_Info << "Data covariance dims are [" << cov[0].rows() << "x" << cov[0].cols() <<
				"]. Number of samples per condition : \n";
		for (size_t i = 0; i < m_nClasses; ++i)
		{
			this->getLogManager() << LogLevel_Info << "  cond " << i + 1 << " = " << m_covProxies[i].nBuffers
					<< " chunks, sized " << input->getDimensionSize(1) << " -> " << m_covProxies[i].nSamples << " samples\n";
			// this->getLogManager() << LogLevel_Info << "Using shrinkage coeff " << m_Shrinkage << " ...\n";
		}

		// Compute the actual CSP using the obtained covariance matrices
		std::vector<MatrixXd> sortedEigenVectors;
		std::vector<VectorXd> sortedEigenValues;
		OV_ERROR_UNLESS_KRF(computeCSP(cov, sortedEigenVectors, sortedEigenValues), "Failure when computing CSP", OpenViBE::Kernel::ErrorType::BadProcessing);

		// Create a CMatrix mapper that can spool the filters to a file
		CMatrix selectedVectors;
		selectedVectors.setDimensionCount(2);
		selectedVectors.setDimensionSize(0, m_filtersPerClass * m_nClasses);
		selectedVectors.setDimensionSize(1, nChannels);

		Map<MatrixXdRowMajor> selectedVectorsMapper(selectedVectors.getBuffer(), m_filtersPerClass * m_nClasses, nChannels);

		for (size_t c = 0; c < m_nClasses; ++c)
		{
			selectedVectorsMapper.block(c * m_filtersPerClass, 0, m_filtersPerClass, nChannels) = sortedEigenVectors[c]
																								  .block(0, 0, nChannels, m_filtersPerClass).transpose();

			this->getLogManager() << LogLevel_Info << "The " << m_filtersPerClass << " filter(s) for cond " << c + 1 << " cover " << 100.0 *
					sortedEigenValues[c].head(m_filtersPerClass).sum() / sortedEigenValues[c].sum() << "% of corresp. eigenvalues\n";
		}

		if (m_saveAsBoxConf)
		{
			FILE* file = FS::Files::open(m_configFilename.toASCIIString(), "wb");
			OV_ERROR_UNLESS_KRF(file, "Failed to open file located at [" << m_configFilename << "]", OpenViBE::Kernel::ErrorType::BadFileRead);

			fprintf(file, "<OpenViBE-SettingsOverride>\n");
			fprintf(file, "\t<SettingValue>");

			const size_t numCoefficients = m_filtersPerClass * m_nClasses * nChannels;
			for (size_t i = 0; i < numCoefficients; ++i) { fprintf(file, "%e ", selectedVectors.getBuffer()[i]); }

			fprintf(file, "</SettingValue>\n");
			fprintf(file, "\t<SettingValue>%zu</SettingValue>\n", m_filtersPerClass * m_nClasses);
			fprintf(file, "\t<SettingValue>%zu</SettingValue>\n", nChannels);
			fprintf(file, "\t<SettingValue></SettingValue>\n");
			fprintf(file, "</OpenViBE-SettingsOverride>\n");

			fclose(file);
		}
		else
		{
			for (size_t i = 0; i < selectedVectors.getDimensionSize(0); ++i)
			{
				std::stringstream label;
				label << "Cond " << i / m_filtersPerClass + 1 << " filter " << i % m_filtersPerClass + 1;
				selectedVectors.setDimensionLabel(0, i, label.str().c_str());
			}

			OV_ERROR_UNLESS_KRF(OpenViBEToolkit::Tools::Matrix::saveToTextFile(selectedVectors, m_configFilename, 10),
								"Failed to save file to location [" << m_configFilename << "]",
								OpenViBE::Kernel::ErrorType::BadFileWrite);
		}

		this->getLogManager() << LogLevel_Info << "Regularized CSP Spatial filter trained successfully.\n";

		// Clean data, so if there's a new train stimulation, we'll start again.
		// @note possibly this should be a parameter in the future to allow incremental training
		for (auto& elem : m_covProxies)
		{
			elem.cov->activateInputTrigger(OVP_Algorithm_OnlineCovariance_Process_Reset, true);
			elem.nSamples = 0;
			elem.nBuffers = 0;
		}

		m_encoder.getInputStimulationSet()->clear();
		m_encoder.getInputStimulationSet()->appendStimulation(OVTK_StimulationId_TrainCompleted, date, 0);
		m_encoder.encodeBuffer();

		boxContext.markOutputAsReadyToSend(0, startTime, endTime);
	}

	return true;
}

#endif // TARGET_HAS_ThirdPartyEIGEN
