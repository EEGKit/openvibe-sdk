#include "ovtkCAlgorithmClassifier.h"
#include "ovtkCAlgorithmPairingStrategy.h"

#include <map>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEToolkit;

static std::map<uint64_t, fClassifierComparison> mComparisionFunctionMap;

void OpenViBEToolkit::registerClassificationComparisonFunction(const CIdentifier& rClassIdentifier, fClassifierComparison pComparision)
{
	mComparisionFunctionMap[rClassIdentifier.toUInteger()] = pComparision;
}

fClassifierComparison OpenViBEToolkit::getClassificationComparisonFunction(const CIdentifier& rClassIdentifier)
{
	if (mComparisionFunctionMap.count(rClassIdentifier.toUInteger()) == 0)
		return nullptr;
	return mComparisionFunctionMap[rClassIdentifier.toUInteger()];
}

bool CAlgorithmPairingStrategy::process()
{
	if (this->isInputTriggerActive(OVTK_Algorithm_PairingStrategy_InputTriggerId_DesignArchitecture))
	{
		TParameterHandler<CIdentifier*> ip_pClassifierIdentifier(this->getInputParameter(OVTK_Algorithm_PairingStrategy_InputParameterId_SubClassifierAlgorithm));
		TParameterHandler<uint64_t> ip_pClassCount(this->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_NumberOfClasses));

		const uint64_t l_ui64ClassCount           = (uint64_t)ip_pClassCount;
		const CIdentifier l_oClassifierIdentifier = *((CIdentifier*)ip_pClassifierIdentifier);
		if (this->designArchitecture(l_oClassifierIdentifier, static_cast<uint32_t>(l_ui64ClassCount)))
		{
			this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Success, true);
		}
		else
		{
			this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Failed, true);
			OV_ERROR_KRF("Designing architecture failed", OpenViBE::Kernel::ErrorType::Internal);
		}
	}
	else { return CAlgorithmClassifier::process(); }
	return true;
}
