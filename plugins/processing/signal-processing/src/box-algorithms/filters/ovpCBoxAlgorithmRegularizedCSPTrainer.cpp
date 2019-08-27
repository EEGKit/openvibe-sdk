#if defined TARGET_HAS_ThirdPartyEIGEN

#include "ovpCBoxAlgorithmRegularizedCSPTrainer.h"

#include <sstream>
#include <cstdio>

#include <Eigen/Eigenvalues>
#include <fs/Files.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace SignalProcessing;

using namespace Eigen;

// typedef Eigen::Matrix< double , Eigen::Dynamic , Eigen::Dynamic, Eigen::RowMajor > MatrixXdRowMajor;

CBoxAlgorithmRegularizedCSPTrainer::CBoxAlgorithmRegularizedCSPTrainer() : m_Tikhonov(0.0) {}

bool CBoxAlgorithmRegularizedCSPTrainer::initialize()
{
	m_StimulationDecoder.initialize(*this, 0);
	m_StimulationEncoder.initialize(*this, 0);

	const IBox& staticBoxContext = this->getStaticBoxContext();

	m_NumClasses = staticBoxContext.getInputCount() - 1;

	m_IncCovarianceProxies.resize(m_NumClasses);

	m_StimulationIdentifier              = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_SpatialFilterConfigurationFilename = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_FiltersPerClass                    = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	m_SaveAsBoxConf                      = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);

	// @FIXME CERT
	OV_ERROR_UNLESS_KRF(m_FiltersPerClass > 0, // && m_FiltersPerClass%2 == 0,
						"Invalid filter dimension number [" << m_FiltersPerClass << "] (expected value > 0)", // even ?
						OpenViBE::Kernel::ErrorType::BadSetting);

	m_HasBeenInitialized = true;


	m_SignalDecoders.resize(m_NumClasses);
	for (uint32_t i = 0; i < m_NumClasses; i++)
	{
		m_SignalDecoders[i].initialize(*this, i + 1);

		const CIdentifier covAlgId = this->getAlgorithmManager().createAlgorithm(OVP_ClassId_Algorithm_OnlineCovariance);
		OV_ERROR_UNLESS_KRF(covAlgId != OV_UndefinedIdentifier, "Failed to create online covariance algorithm", OpenViBE::Kernel::ErrorType::BadResourceCreation);


		m_IncCovarianceProxies[i].incrementalCov = &this->getAlgorithmManager().getAlgorithm(covAlgId);
		OV_ERROR_UNLESS_KRF(m_IncCovarianceProxies[i].incrementalCov->initialize(), "Failed to initialize online covariance algorithm", OpenViBE::Kernel::ErrorType::Internal);


		// Set the params of the cov algorithm
		TParameterHandler<uint64_t> updateMethod(m_IncCovarianceProxies[i].incrementalCov->getInputParameter(OVP_Algorithm_OnlineCovariance_InputParameterId_UpdateMethod));
		TParameterHandler<bool> traceNormalization(m_IncCovarianceProxies[i].incrementalCov->getInputParameter(OVP_Algorithm_OnlineCovariance_InputParameterId_TraceNormalization));
		TParameterHandler<double> shrinkage(m_IncCovarianceProxies[i].incrementalCov->getInputParameter(OVP_Algorithm_OnlineCovariance_InputParameterId_Shrinkage));

		updateMethod       = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 4);
		traceNormalization = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 5);
		shrinkage          = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 6);
	}

	m_Tikhonov = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 7);

	OV_ERROR_UNLESS_KRF(m_SpatialFilterConfigurationFilename != CString(""), "Output filename is required in box configuration", OpenViBE::Kernel::ErrorType::BadSetting);

	return true;
}

bool CBoxAlgorithmRegularizedCSPTrainer::uninitialize()
{
	m_StimulationDecoder.uninitialize();
	m_StimulationEncoder.uninitialize();

	if (m_HasBeenInitialized)
	{
		for (uint32_t i = 0; i < m_NumClasses; i++)
		{
			m_SignalDecoders[i].uninitialize();
			if (m_IncCovarianceProxies[i].incrementalCov)
			{
				m_IncCovarianceProxies[i].incrementalCov->uninitialize();
				getAlgorithmManager().releaseAlgorithm(*m_IncCovarianceProxies[i].incrementalCov);
			}
		}
	}
	m_IncCovarianceProxies.clear();

	return true;
}

bool CBoxAlgorithmRegularizedCSPTrainer::processInput(const uint32_t index)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmRegularizedCSPTrainer::updateCov(uint32_t index)
{
	IBoxIO& dynamicBoxContext = this->getDynamicBoxContext();
	IncrementalCovarianceProxy& curCovProxy(m_IncCovarianceProxies[index]);
	for (uint32_t i = 0; i < dynamicBoxContext.getInputChunkCount(index + 1); i++)
	{
		OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmRegularizedCSPTrainer>* decoder = &m_SignalDecoders[index];
		const IMatrix* inputSignal                                                   = decoder->getOutputMatrix();

		decoder->decode(i);
		if (decoder->isHeaderReceived())
		{
			TParameterHandler<IMatrix*> featureVectorSet(curCovProxy.incrementalCov->getInputParameter(OVP_Algorithm_OnlineCovariance_InputParameterId_InputVectors));

			featureVectorSet->setDimensionCount(2);
			featureVectorSet->setDimensionSize(0, inputSignal->getDimensionSize(1));
			featureVectorSet->setDimensionSize(1, inputSignal->getDimensionSize(0));

			OV_ERROR_UNLESS_KRF(m_FiltersPerClass <= inputSignal->getDimensionSize(0),
								"Invalid CSP filter dimension of [" << m_FiltersPerClass << "] for stream " << i+1 << " (expected value must be less than input channel count ["<< inputSignal->getDimensionSize(1) <<"])",
								OpenViBE::Kernel::ErrorType::BadSetting);

			curCovProxy.incrementalCov->activateInputTrigger(OVP_Algorithm_OnlineCovariance_Process_Reset, true);

			OV_ERROR_UNLESS_KRF(curCovProxy.incrementalCov->process(), "Failed to parametrize covariance algorithm", OpenViBE::Kernel::ErrorType::Internal);
		}
		if (decoder->isBufferReceived())
		{
			TParameterHandler<IMatrix*> ip_pFeatureVectorSet(curCovProxy.incrementalCov->getInputParameter(OVP_Algorithm_OnlineCovariance_InputParameterId_InputVectors));

			// transpose data
			const uint32_t nChannels = inputSignal->getDimensionSize(0);
			const uint32_t nSamples  = inputSignal->getDimensionSize(1);

			const Map<MatrixXdRowMajor> l_oInputMapper(const_cast<double*>(inputSignal->getBuffer()), nChannels, nSamples);
			Map<MatrixXdRowMajor> outputMapper(ip_pFeatureVectorSet->getBuffer(), nSamples, nChannels);
			outputMapper = l_oInputMapper.transpose();

			curCovProxy.incrementalCov->activateInputTrigger(OVP_Algorithm_OnlineCovariance_Process_Update, true);
			curCovProxy.incrementalCov->process();

			curCovProxy.numBuffers++;
			curCovProxy.numSamples += nSamples;
		}
		if (decoder->isEndReceived())
		{
			// nop
		}
	}

	return true;
}

//
// Returns a sample-weighted average of given covariance matrices that does not include the cov of ui32SkipIndex
//
// @todo error handling is a bit scarce
//
// @note This will recompute the weights on every call, but given how small amount of
// computations we're speaking of, there's not much point in optimizing.
//
bool CBoxAlgorithmRegularizedCSPTrainer::outclassCovAverage(uint32_t skipIndex, const std::vector<MatrixXd>& cov, MatrixXd& covAvg)
{
	if (cov.size() == 0 || skipIndex >= cov.size()) { return false; }

	std::vector<double> classWeights;
	uint64_t totalOutclassSamples = 0;

	// Compute the total number of samples
	for (uint32_t i = 0; i < m_NumClasses; i++)
	{
		if (i != skipIndex)
		{
			totalOutclassSamples += m_IncCovarianceProxies[i].numSamples;
		}
	}

	// Compute weigths for averaging
	classWeights.resize(m_NumClasses);
	for (uint32_t i = 0; i < m_NumClasses; i++)
	{
		classWeights[i] = i == skipIndex ? 0 : m_IncCovarianceProxies[i].numSamples / double(totalOutclassSamples);
		this->getLogManager() << LogLevel_Debug << "Condition " << i + 1 << " averaging weight = " << classWeights[i] << "\n";
	}

	// Average the covs
	covAvg.resizeLike(cov[0]);
	covAvg.setZero();
	for (uint32_t i = 0; i < m_NumClasses; i++) { covAvg += (classWeights[i] * cov[i]); }

	return true;
}

bool CBoxAlgorithmRegularizedCSPTrainer::computeCSP(const std::vector<MatrixXd>& cov, std::vector<MatrixXd>& sortedEigenVectors,
													std::vector<VectorXd>& sortedEigenValues)
{
	// We wouldn't need to store all this -- they are kept for debugging purposes
	std::vector<VectorXd> eigenValues(m_NumClasses);
	std::vector<MatrixXd> eigenVectors(m_NumClasses);
	std::vector<MatrixXd> covInv(m_NumClasses);
	std::vector<MatrixXd> covProd(m_NumClasses);
	MatrixXd tikhonov;
	MatrixXd outclassCov;
	tikhonov.resizeLike(cov[0]);
	tikhonov.setIdentity();
	tikhonov *= m_Tikhonov;

	sortedEigenVectors.resize(m_NumClasses);
	sortedEigenValues.resize(m_NumClasses);

	// To get the CSP filters, we compute two sets of eigenvectors,
	// eig(inv(sigma2+tikhonov)*sigma1) and eig(inv(sigma1+tikhonov)*sigma2
	// and pick the ones corresponding to the largest eigenvalues as
	// spatial filters [following Lotte & Guan 2011]. Assumes the shrink
	// of the sigmas (if its used) has been performed inside the cov
	// computation algorithm.

	EigenSolver<MatrixXd> eigenSolverGeneral;

	for (uint32_t classIndex = 0; classIndex < m_NumClasses; classIndex++)
	{
		try
		{
			covInv[classIndex] = (cov[classIndex] + tikhonov).inverse();
		}
		catch (...)
		{
			OV_ERROR_KRF("Inversion failed for condition [" << classIndex+1 << "]", OpenViBE::Kernel::ErrorType::BadProcessing);
		}

		// Compute covariance in all the classes except 'classIndex'.
		OV_ERROR_UNLESS_KRF(outclassCovAverage(classIndex, cov, outclassCov), "Outclass cov computation failed for condition [" << classIndex + 1 << "]", OpenViBE::Kernel::ErrorType::BadProcessing);

		covProd[classIndex] = covInv[classIndex] * outclassCov;

		try { eigenSolverGeneral.compute(covProd[classIndex]); }
		catch (...)
		{
			OV_ERROR_KRF("EigenSolver failed for condition [" << classIndex + 1 << "]", OpenViBE::Kernel::ErrorType::BadProcessing);
		}

		eigenValues[classIndex]  = eigenSolverGeneral.eigenvalues().real();
		eigenVectors[classIndex] = eigenSolverGeneral.eigenvectors().real();

		// Sort the vectors -_-
		std::vector<std::pair<double, int>> indexes;
		for (int i = 0; i < eigenValues[classIndex].size(); i++)
		{
			indexes.emplace_back(std::make_pair((eigenValues[classIndex])[i], i));
		}
		std::sort(indexes.begin(), indexes.end(), std::greater<std::pair<double, int>>());

		sortedEigenValues[classIndex].resizeLike(eigenValues[classIndex]);
		sortedEigenVectors[classIndex].resizeLike(eigenVectors[classIndex]);
		for (int i = 0; i < eigenValues[classIndex].size(); i++)
		{
			sortedEigenValues[classIndex][i]      = eigenValues[classIndex][indexes[size_t(i)].second];
			sortedEigenVectors[classIndex].col(i) = eigenVectors[classIndex].col(indexes[i].second);
			// this->getLogManager() << LogLevel_Info << "E " << i << " " << (l_oSortedEigenValues[classIndex])[i] << "\n";
		}
	}
	return true;
}

bool CBoxAlgorithmRegularizedCSPTrainer::process()
{
	IBoxIO& dynamicBoxContext = this->getDynamicBoxContext();

	bool shouldTrain   = false;
	uint64_t trainDate = 0, trainChunkStartTime = 0, trainChunkEndTime = 0;

	// Handle input stimulations
	for (uint32_t i = 0; i < dynamicBoxContext.getInputChunkCount(0); i++)
	{
		m_StimulationDecoder.decode(i);
		if (m_StimulationDecoder.isHeaderReceived())
		{
			m_StimulationEncoder.encodeHeader();
			dynamicBoxContext.markOutputAsReadyToSend(0, dynamicBoxContext.getInputChunkStartTime(0, i), dynamicBoxContext.getInputChunkEndTime(0, i));
		}
		if (m_StimulationDecoder.isBufferReceived())
		{
			const TParameterHandler<IStimulationSet*> stimulationSet(m_StimulationDecoder.getOutputStimulationSet());
			for (uint32_t j = 0; j < stimulationSet->getStimulationCount(); j++)
			{
				if (stimulationSet->getStimulationIdentifier(j) == m_StimulationIdentifier)
				{
					trainDate           = stimulationSet->getStimulationDate(stimulationSet->getStimulationCount() - 1);
					trainChunkStartTime = dynamicBoxContext.getInputChunkStartTime(0, i);
					trainChunkEndTime   = dynamicBoxContext.getInputChunkEndTime(0, i);
					shouldTrain         = true;
					break;
				}
			}
		}
		if (m_StimulationDecoder.isEndReceived()) { m_StimulationEncoder.encodeEnd(); }
	}

	// Update all covs with the current data chunks (if any)
	for (uint32_t i = 0; i < m_NumClasses; i++)
	{
		if (!updateCov(i)) { return false; }
	}

	if (shouldTrain)
	{
		this->getLogManager() << LogLevel_Info << "Received train stimulation - be patient\n";

		const IMatrix* input     = m_SignalDecoders[0].getOutputMatrix();
		const uint32_t nChannels = input->getDimensionSize(0);

		this->getLogManager() << LogLevel_Debug << "Computing eigen vector decomposition...\n";

		// Get out the covariances
		std::vector<MatrixXd> cov(m_NumClasses);

		for (uint32_t i = 0; i < m_NumClasses; i++)
		{
			OV_ERROR_UNLESS_KRF(m_IncCovarianceProxies[i].numSamples >= 2,
								"Invalid sample count of [" <<m_IncCovarianceProxies[i].numSamples << "] for condition number " << i << " (expected value > 2)",
								OpenViBE::Kernel::ErrorType::BadProcessing);

			TParameterHandler<IMatrix*> op_pCovarianceMatrix(m_IncCovarianceProxies[i].incrementalCov->getOutputParameter(OVP_Algorithm_OnlineCovariance_OutputParameterId_CovarianceMatrix));

			// Get regularized cov
			m_IncCovarianceProxies[i].incrementalCov->activateInputTrigger(OVP_Algorithm_OnlineCovariance_Process_GetCov, true);
			OV_ERROR_UNLESS_KRF(m_IncCovarianceProxies[i].incrementalCov->process(), "Failed to retrieve regularized covariance", OpenViBE::Kernel::ErrorType::Internal);

			Map<MatrixXdRowMajor> l_oCovMapper(op_pCovarianceMatrix->getBuffer(), nChannels, nChannels);
			cov[i] = l_oCovMapper;

			// Get vanilla cov
			m_IncCovarianceProxies[i].incrementalCov->activateInputTrigger(OVP_Algorithm_OnlineCovariance_Process_GetCovRaw, true);
			OV_ERROR_UNLESS_KRF(m_IncCovarianceProxies[i].incrementalCov->process(), "Failed to retrieve vanilla covariance", OpenViBE::Kernel::ErrorType::Internal);
		}

		// Sanity check
		for (uint32_t i = 1; i < m_NumClasses; i++)
		{
			OV_ERROR_UNLESS_KRF(cov[i-1].rows() == cov[i].rows() && cov[i-1].cols() == cov[i].cols(), "Mismatch between the number of channel in both input streams", OpenViBE::Kernel::ErrorType::BadValue);
		}

		this->getLogManager() << LogLevel_Info << "Data covariance dims are [" << uint32_t(cov[0].rows()) << "x" << uint32_t(cov[0].cols())
				<< "]. Number of samples per condition : \n";
		for (uint32_t i = 0; i < m_NumClasses; i++)
		{
			this->getLogManager() << LogLevel_Info << "  cond " << i + 1 << " = "
					<< m_IncCovarianceProxies[i].numBuffers << " chunks, sized " << input->getDimensionSize(1) << " -> " << m_IncCovarianceProxies[i].numSamples << " samples\n";
			// this->getLogManager() << LogLevel_Info << "Using shrinkage coeff " << m_f64Shrinkage << " ...\n";
		}

		// Compute the actual CSP using the obtained covariance matrices
		std::vector<MatrixXd> l_vSortedEigenVectors;
		std::vector<VectorXd> l_vSortedEigenValues;
		OV_ERROR_UNLESS_KRF(computeCSP(cov, l_vSortedEigenVectors, l_vSortedEigenValues), "Failure when computing CSP", OpenViBE::Kernel::ErrorType::BadProcessing);

		// Create a CMatrix mapper that can spool the filters to a file
		CMatrix selectedVectors;
		selectedVectors.setDimensionCount(2);
		selectedVectors.setDimensionSize(0, m_FiltersPerClass * m_NumClasses);
		selectedVectors.setDimensionSize(1, nChannels);

		Map<MatrixXdRowMajor> selectedVectorsMapper(selectedVectors.getBuffer(), m_FiltersPerClass * m_NumClasses, nChannels);

		for (uint32_t c = 0; c < m_NumClasses; c++)
		{
			selectedVectorsMapper.block(c * m_FiltersPerClass, 0, m_FiltersPerClass, nChannels) =
					l_vSortedEigenVectors[c].block(0, 0, nChannels, m_FiltersPerClass).transpose();

			this->getLogManager() << LogLevel_Info << "The " << m_FiltersPerClass << " filter(s) for cond " << c + 1 << " cover " << 100.0 * l_vSortedEigenValues[c].head(m_FiltersPerClass).sum() / l_vSortedEigenValues[c].sum() << "% of corresp. eigenvalues\n";
		}

		if (m_SaveAsBoxConf)
		{
			FILE* file = FS::Files::open(m_SpatialFilterConfigurationFilename.toASCIIString(), "wb");
			OV_ERROR_UNLESS_KRF(file, "Failed to open file located at [" << m_SpatialFilterConfigurationFilename.toASCIIString() << "]", OpenViBE::Kernel::ErrorType::BadFileRead);

			fprintf(file, "<OpenViBE-SettingsOverride>\n");
			fprintf(file, "\t<SettingValue>");

			const uint32_t numCoefficients = m_FiltersPerClass * m_NumClasses * nChannels;
			for (uint32_t i = 0; i < numCoefficients; i++)
			{
				fprintf(file, "%e ", selectedVectors.getBuffer()[i]);
			}

			fprintf(file, "</SettingValue>\n");
			fprintf(file, "\t<SettingValue>%d</SettingValue>\n", m_FiltersPerClass * m_NumClasses);
			fprintf(file, "\t<SettingValue>%d</SettingValue>\n", nChannels);
			fprintf(file, "\t<SettingValue></SettingValue>\n");
			fprintf(file, "</OpenViBE-SettingsOverride>\n");

			fclose(file);
		}
		else
		{
			for (uint32_t i = 0; i < selectedVectors.getDimensionSize(0); i++)
			{
				std::stringstream label;
				label << "Cond " << i / m_FiltersPerClass + 1
						<< " filter " << i % m_FiltersPerClass + 1;

				selectedVectors.setDimensionLabel(0, i, label.str().c_str());
			}

			OV_ERROR_UNLESS_KRF(OpenViBEToolkit::Tools::Matrix::saveToTextFile(selectedVectors, m_SpatialFilterConfigurationFilename, 10),
								"Failed to save file to location [" << m_SpatialFilterConfigurationFilename << "]",
								OpenViBE::Kernel::ErrorType::BadFileWrite);
		}

		this->getLogManager() << LogLevel_Info << "Regularized CSP Spatial filter trained successfully.\n";

		// Clean data, so if there's a new train stimulation, we'll start again.
		// @note possibly this should be a parameter in the future to allow incremental training
		for (auto& elem : m_IncCovarianceProxies)
		{
			elem.incrementalCov->activateInputTrigger(OVP_Algorithm_OnlineCovariance_Process_Reset, true);
			elem.numSamples = 0;
			elem.numBuffers = 0;
		}

		m_StimulationEncoder.getInputStimulationSet()->clear();
		m_StimulationEncoder.getInputStimulationSet()->appendStimulation(OVTK_StimulationId_TrainCompleted, trainDate, 0);
		m_StimulationEncoder.encodeBuffer();

		dynamicBoxContext.markOutputAsReadyToSend(0, trainChunkStartTime, trainChunkEndTime);
	}

	return true;
}

#endif // TARGET_HAS_ThirdPartyEIGEN
