#include "ovtkCAlgorithmClassifier.h"
#include "ovtkCAlgorithmPairingStrategy.h"

#include <map>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEToolkit;

static std::map<uint64_t, fClassifierComparison> comparisionFunctions;

void OpenViBEToolkit::registerClassificationComparisonFunction(const CIdentifier& classID, const fClassifierComparison comparision)
{
	comparisionFunctions[classID.toUInteger()] = comparision;
}

fClassifierComparison OpenViBEToolkit::getClassificationComparisonFunction(const CIdentifier& classID)
{
	if (comparisionFunctions.count(classID.toUInteger()) == 0) { return nullptr; }
	return comparisionFunctions[classID.toUInteger()];
}

bool CAlgorithmPairingStrategy::process()
{
	if (this->isInputTriggerActive(OVTK_Algorithm_PairingStrategy_InputTriggerId_DesignArchitecture))
	{
		TParameterHandler<CIdentifier*> ip_classifierID(this->getInputParameter(OVTK_Algorithm_PairingStrategy_InputParameterId_SubClassifierAlgorithm));
		TParameterHandler<uint64_t> ip_pClassCount(this->getInputParameter(OVTK_Algorithm_Classifier_InputParameterId_NClasses));

		const uint64_t nClass          = uint64_t(ip_pClassCount);
		const CIdentifier classifierID = *static_cast<CIdentifier*>(ip_classifierID);
		if (this->designArchitecture(classifierID, size_t(nClass))) { this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Success, true); }
		else
		{
			this->activateOutputTrigger(OVTK_Algorithm_Classifier_OutputTriggerId_Failed, true);
			OV_ERROR_KRF("Designing architecture failed", OpenViBE::Kernel::ErrorType::Internal);
		}
	}
	else { return CAlgorithmClassifier::process(); }
	return true;
}
