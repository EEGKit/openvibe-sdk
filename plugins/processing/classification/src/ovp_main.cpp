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

#if defined TARGET_HAS_ThirdPartyEIGEN
#include "algorithms/ovpCAlgorithmConditionedCovariance.h"
#include "algorithms/ovpCAlgorithmClassifierLDA.h"
#endif // TARGET_HAS_ThirdPartyEIGEN

const char* const PAIRWISE_STRATEGY_ENUMERATION_NAME = "Pairwise Decision Strategy";

OVP_Declare_Begin()
	context.getTypeManager().registerEnumerationEntry(OVTK_TypeId_ClassificationStrategy, "Native", OV_UndefinedIdentifier.toUInteger());
	context.getTypeManager().registerEnumerationEntry(OVTK_TypeId_ClassificationStrategy, "OneVsAll", OVP_ClassId_Algorithm_ClassifierOneVsAll.toUInteger());
	context.getTypeManager().registerEnumerationEntry(OVTK_TypeId_ClassificationStrategy, "OneVsOne", OVP_ClassId_Algorithm_ClassifierOneVsOne.toUInteger());

	//	context.getTypeManager().registerEnumerationEntry(OVTK_TypeId_ClassificationAlgorithm, "NULL Classifier (does nothing)",OVP_ClassId_Algorithm_ClassifierNULL.toUInteger());
	//	OVP_Declare_New(OpenViBEPlugins::Classification::CAlgorithmClassifierNULLDesc);


	OVP_Declare_New(OpenViBEPlugins::Classification::CBoxAlgorithmVotingClassifierDesc);
	OVP_Declare_New(OpenViBEPlugins::Classification::CBoxAlgorithmClassifierTrainerDesc);
	OVP_Declare_New(OpenViBEPlugins::Classification::CBoxAlgorithmClassifierProcessorDesc);

	OVP_Declare_New(OpenViBEPlugins::Classification::CAlgorithmClassifierOneVsAllDesc);
	OVP_Declare_New(OpenViBEPlugins::Classification::CAlgorithmClassifierOneVsOneDesc);

	// Functions related to deciding winner in OneVsOne multiclass decision strategy
	context.getTypeManager().registerEnumerationType(OVP_TypeId_ClassificationPairwiseStrategy, PAIRWISE_STRATEGY_ENUMERATION_NAME);

	OVP_Declare_New(OpenViBEPlugins::Classification::CAlgorithmPairwiseStrategyPKPDDesc);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_ClassificationPairwiseStrategy, "PKPD", OVP_ClassId_Algorithm_PairwiseStrategy_PKPD.toUInteger());
	OVP_Declare_New(OpenViBEPlugins::Classification::CAlgorithmPairwiseDecisionVotingDesc);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_ClassificationPairwiseStrategy, "Voting", OVP_ClassId_Algorithm_PairwiseDecision_Voting.toUInteger());
	OVP_Declare_New(OpenViBEPlugins::Classification::CAlgorithmPairwiseDecisionHTDesc);
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_ClassificationPairwiseStrategy, "HT", OVP_ClassId_Algorithm_PairwiseDecision_HT.toUInteger());

#if defined TARGET_HAS_ThirdPartyEIGEN
	OVP_Declare_New(OpenViBEPlugins::Classification::CAlgorithmConditionedCovarianceDesc);

	context.getTypeManager().registerEnumerationEntry(OVTK_TypeId_ClassificationAlgorithm, "Linear Discrimimant Analysis (LDA)", OVP_ClassId_Algorithm_ClassifierLDA.toUInteger());
	OpenViBEToolkit::registerClassificationComparisonFunction(OVP_ClassId_Algorithm_ClassifierLDA, OpenViBEPlugins::Classification::LDAClassificationCompare);
	OVP_Declare_New(OpenViBEPlugins::Classification::CAlgorithmClassifierLDADesc);
	context.getTypeManager().registerEnumerationType(OVP_ClassId_Algorithm_ClassifierLDA_DecisionAvailable, PAIRWISE_STRATEGY_ENUMERATION_NAME);
	context.getTypeManager().registerEnumerationEntry(OVP_ClassId_Algorithm_ClassifierLDA_DecisionAvailable, "PKPD", OVP_ClassId_Algorithm_PairwiseStrategy_PKPD.toUInteger());
	context.getTypeManager().registerEnumerationEntry(OVP_ClassId_Algorithm_ClassifierLDA_DecisionAvailable, "Voting", OVP_ClassId_Algorithm_PairwiseDecision_Voting.toUInteger());
	context.getTypeManager().registerEnumerationEntry(OVP_ClassId_Algorithm_ClassifierLDA_DecisionAvailable, "HT", OVP_ClassId_Algorithm_PairwiseDecision_HT.toUInteger());

	context.getTypeManager().registerEnumerationType(OVP_TypeId_OneVsOne_DecisionAlgorithms, "One vs One Decision Algorithms");
	context.getTypeManager().registerEnumerationEntry(OVP_TypeId_OneVsOne_DecisionAlgorithms, "Linear Discrimimant Analysis (LDA)", OVP_ClassId_Algorithm_ClassifierLDA_DecisionAvailable.toUInteger());

#endif // TARGET_HAS_ThirdPartyEIGEN

OVP_Declare_End()

#include<cmath>

bool OVFloatEqual(const double first, const double second)
{
	const double epsilon = 0.000001;
	return epsilon > fabs(first - second);
}
