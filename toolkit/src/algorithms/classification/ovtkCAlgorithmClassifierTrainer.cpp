#include "ovtkCAlgorithmClassifierTrainer.h"
#include "ovtkCFeatureVectorSet.hpp"

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;
using namespace OpenViBEToolkit;

bool CAlgorithmClassifierTrainer::process(void)
{
	TParameterHandler<IMatrix*> ip_pFeatureVectorSet(this->getInputParameter(OVTK_Algorithm_ClassifierTrainer_InputParameterId_FeatureVectorSet));
	TParameterHandler<IMemoryBuffer*> op_pConfiguration(this->getOutputParameter(OVTK_Algorithm_ClassifierTrainer_OutputParameterId_Configuration));

	if (this->isInputTriggerActive(OVTK_Algorithm_ClassifierTrainer_InputTriggerId_Train))
	{
		IMatrix* l_pFeatureVectorSet = ip_pFeatureVectorSet;
		if (!l_pFeatureVectorSet)
		{
			this->activateOutputTrigger(OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Failed, true);
			OV_ERROR_KRF("Feature vector set is NULL", OpenViBE::Kernel::ErrorType::BadInput);
		}
		else
		{
			CFeatureVectorSet l_oFeatureVectorSetAdapter(*l_pFeatureVectorSet);
			if (this->train(l_oFeatureVectorSetAdapter))
			{
				this->activateOutputTrigger(OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Success, true);
			}
			else
			{
				this->activateOutputTrigger(OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Failed, true);
				OV_ERROR_KRF("Training failed", OpenViBE::Kernel::ErrorType::Internal);
			}
		}
	}

	if (this->isInputTriggerActive(OVTK_Algorithm_ClassifierTrainer_InputTriggerId_SaveConfiguration))
	{
		IMemoryBuffer* l_pConfiguration = op_pConfiguration;
		if (!l_pConfiguration)
		{
			this->activateOutputTrigger(OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Failed, true);
			OV_ERROR_KRF("Configuration memory buffer is NULL", OpenViBE::Kernel::ErrorType::BadOutput);
		}
		else
		{
			l_pConfiguration->setSize(0, true);
			if (this->saveConfiguration(*l_pConfiguration))
			{
				this->activateOutputTrigger(OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Success, true);
			}
			else
			{
				this->activateOutputTrigger(OVTK_Algorithm_ClassifierTrainer_OutputTriggerId_Failed, true);
				OV_ERROR_KRF("Saving configuration failed", OpenViBE::Kernel::ErrorType::Internal);
			}
		}
	}

	return true;
}
