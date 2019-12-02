#include "ovtkCAlgorithmClassifierTrainer.h"
#include "ovtkCFeatureVectorSet.hpp"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;
using namespace OpenViBEToolkit;

bool CAlgorithmClassifierTrainer::process()
{
	TParameterHandler<IMatrix*> ip_pFeatureVectorSet(this->getInputParameter(OVTK_Algorithm_ClassifierTrainer_InputParameterId_FeatureVectorSet));
	TParameterHandler<IMemoryBuffer*> op_pConfiguration(this->getOutputParameter(OVTK_Algorithm_ClassifierTrainer_OutputParameterId_Config));

	if (this->isInputTriggerActive(OVTK_Algorithm_ClassifierTrainer_InputTriggerId_Train))
	{
		IMatrix* featureVectorSet = ip_pFeatureVectorSet;
		if (!featureVectorSet)
		{
			this->activateOutputTrigger(OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Failed, true);
			OV_ERROR_KRF("Feature vector set is NULL", OpenViBE::Kernel::ErrorType::BadInput);
		}
		const CFeatureVectorSet featureVectorSetAdapter(*featureVectorSet);
		if (this->train(featureVectorSetAdapter)) { this->activateOutputTrigger(OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Success, true); }
		else
		{
			this->activateOutputTrigger(OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Failed, true);
			OV_ERROR_KRF("Training failed", OpenViBE::Kernel::ErrorType::Internal);
		}
	}

	if (this->isInputTriggerActive(OVTK_Algorithm_ClassifierTrainer_InputTriggerId_SaveConfig))
	{
		IMemoryBuffer* config = op_pConfiguration;
		if (!config)
		{
			this->activateOutputTrigger(OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Failed, true);
			OV_ERROR_KRF("Configuration memory buffer is NULL", OpenViBE::Kernel::ErrorType::BadOutput);
		}
		config->setSize(0, true);
		if (this->saveConfig(*config)) { this->activateOutputTrigger(OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Success, true); }
		else
		{
			this->activateOutputTrigger(OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Failed, true);
			OV_ERROR_KRF("Saving configuration failed", OpenViBE::Kernel::ErrorType::Internal);
		}
	}

	return true;
}
