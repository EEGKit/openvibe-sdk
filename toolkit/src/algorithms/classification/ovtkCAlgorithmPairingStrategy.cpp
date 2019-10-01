#include "ovtkCAlgorithmClassifier.h"
#include "ovtkCAlgorithmPairingStrategy.h"

#include <map>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEToolkit;

static std::map<uint64_t, fClassifierComparison> comparisionFunctionMap;

void OpenViBEToolkit::registerClassificationComparisonFunction(const CIdentifier& classID, fClassifierComparison comparision) { comparisionFunctionMap[classID.toUInteger()] = comparision; }

fClassifierComparison OpenViBEToolkit::getClassificationComparisonFunction(const CIdentifier& classID)
{
	if (comparisionFunctionMap.count(classID.toUInteger()) == 0) { return nullptr; }
	return comparisionFunctionMap[classID.toUInteger()];
}

bool CAlgorithmPairingStrategy::process()
{
	if (this->isInputTriggerActive(OVTK_Algorithm_PairingStrategy_InputTriggerId_DesignArchitecture))
	{
		TParameterHandler<CIdentifier*> ip_pClassifierID(this->getInputParameter(OVTK_Algorithm_PairingStrategy_InputParameterId_SubClassifierAlgorithm));
		TParameterHandler<uint64_t> ip_pClassCount(this->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_NumberOfClasses));

		const uint64_t nClass          = uint64_t(ip_pClassCount);
		const CIdentifier classifierID = *static_cast<CIdentifier*>(ip_pClassifierID);
		if (this->designArchitecture(classifierID, uint32_t(nClass))) { this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Success, true); }
		else
		{
			this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Failed, true);
			OV_ERROR_KRF("Designing architecture failed", OpenViBE::Kernel::ErrorType::Internal);
		}
	}
	else { return CAlgorithmClassifier::process(); }
	return true;
}
