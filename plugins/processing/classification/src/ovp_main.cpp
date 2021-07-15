#include <vector>

#include "ovp_defines.h"
#include "toolkit/algorithms/classification/ovtkCAlgorithmPairingStrategy.h" //For comparision mecanism

#include "algorithms/ovpCAlgorithmClassifierNULL.h"
#include "algorithms/ovpCAlgorithmClassifierOneVsAll.h"
#include "algorithms/ovpCAlgorithmClassifierOneVsOne.h"

#include "algorithms/ovpCAlgorithmPairwiseDecision.h"
#include "algorithms/ovpCAlgorithmPairwiseStrategyPKPD.h"
#include "algorithms/ovpCAlgorithmPairwiseDecisionVoting.h"
#include "algorithms/ovpCAlgorithmPairwiseDecisionHT.h"

#include "box-algorithms/ovpCBoxAlgorithmVotingClassifier.h"
#include "box-algorithms/ovpCBoxAlgorithmClassifierTrainer.h"
#include "box-algorithms/ovpCBoxAlgorithmClassifierProcessor.h"
#include "box-algorithms/ovpCBoxAlgorithmAdaptativeClassifier.h"

#if defined TARGET_HAS_ThirdPartyEIGEN
#include "algorithms/ovpCAlgorithmConditionedCovariance.h"
#include "algorithms/ovpCAlgorithmClassifierLDA.h"

#endif // TARGET_HAS_ThirdPartyEIGEN

#include<cmath>

const char* const PAIRWISE_STRATEGY_ENUMERATION_NAME = "Pairwise Decision Strategy";

namespace OpenViBE {
namespace Plugins {
namespace Classification {


OVP_Declare_Begin()
	context.getTypeManager().registerEnumerationEntry(OVTK_TypeId_ClassificationStrategy, "Native", CIdentifier::undefined().id());
	context.getTypeManager().registerEnumerationEntry(OVTK_TypeId_ClassificationStrategy, "OneVsAll", OVP_ClassId_Algorithm_ClassifierOneVsAll.id());
	context.getTypeManager().registerEnumerationEntry(OVTK_TypeId_ClassificationStrategy, "OneVsOne", OVP_ClassId_Algorithm_ClassifierOneVsOne.id());

	//	context.getTypeManager().registerEnumerationEntry(OVTK_TypeId_ClassificationAlgorithm, "NULL Classifier (does nothing)",OVP_ClassId_Algorithm_ClassifierNULL.id());
	//	OVP_Declare_New(CAlgorithmClassifierNULLDesc);


	OVP_Declare_New(CBoxAlgorithmVotingClassifierDesc);
	OVP_Declare_New(CBoxAlgorithmClassifierTrainerDesc);
	OVP_Declare_New(CBoxAlgorithmClassifierProcessorDesc);

	OVP_Declare_New(CAlgorithmClassifierOneVsAllDesc);
	OVP_Declare_New(CAlgorithmClassifierOneVsOneDesc);
	OVP_Declare_New(CBoxAlgorithmAdaptativeClassifierDesc);
	// Functions related to deciding winner in OneVsOne multiclass decision strategy
	context.getTypeManager().registerEnumerationType(OVP_TypeId_ClassificationPairwiseStrategy, PAIRWISE_STRATEGY_ENUMERATION_NAME);

	OVP_Declare_New(CAlgorithmPairwiseStrategyPKPDDesc);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_ClassificationPairwiseStrategy, "PKPD", OVP_ClassId_Algorithm_PairwiseStrategy_PKPD.id());
	OVP_Declare_New(CAlgorithmPairwiseDecisionVotingDesc);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_ClassificationPairwiseStrategy, "Voting", OVP_ClassId_Algorithm_PairwiseDecision_Voting.id());
	OVP_Declare_New(CAlgorithmPairwiseDecisionHTDesc);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_ClassificationPairwiseStrategy, "HT", OVP_ClassId_Algorithm_PairwiseDecision_HT.id());

#if defined TARGET_HAS_ThirdPartyEIGEN
	OVP_Declare_New(CAlgorithmConditionedCovarianceDesc);
	context.getTypeManager().registerEnumerationEntry(OVTK_TypeId_ClassificationAlgorithm, "Linear Discrimimant Analysis (LDA)",
													  OVP_ClassId_Algorithm_ClassifierLDA.id());
	Toolkit::registerClassificationComparisonFunction(OVP_ClassId_Algorithm_ClassifierLDA, LDAClassificationCompare);
	OVP_Declare_New(CAlgorithmClassifierLDADesc);
	context.getTypeManager().registerEnumerationType(OVP_ClassId_Algorithm_ClassifierLDA_DecisionAvailable, PAIRWISE_STRATEGY_ENUMERATION_NAME);
	context.getTypeManager().registerEnumerationEntry(
		OVP_ClassId_Algorithm_ClassifierLDA_DecisionAvailable, "PKPD", OVP_ClassId_Algorithm_PairwiseStrategy_PKPD.id());
	context.getTypeManager().registerEnumerationEntry(
		OVP_ClassId_Algorithm_ClassifierLDA_DecisionAvailable, "Voting", OVP_ClassId_Algorithm_PairwiseDecision_Voting.id());
	context.getTypeManager().registerEnumerationEntry(
		OVP_ClassId_Algorithm_ClassifierLDA_DecisionAvailable, "HT", OVP_ClassId_Algorithm_PairwiseDecision_HT.id());

	context.getTypeManager().registerEnumerationType(OVP_TypeId_OneVsOne_DecisionAlgorithms, "One vs One Decision Algorithms");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_OneVsOne_DecisionAlgorithms, "Linear Discrimimant Analysis (LDA)",
													  OVP_ClassId_Algorithm_ClassifierLDA_DecisionAvailable.id());

	
#endif // TARGET_HAS_ThirdPartyEIGEN
	//
OVP_Declare_End()

}  // namespace Classification
}  // namespace Plugins
}  // namespace OpenViBE

bool OVFloatEqual(const double first, const double second)
{
	const double epsilon = 0.000001;
	return epsilon > fabs(first - second);
}
