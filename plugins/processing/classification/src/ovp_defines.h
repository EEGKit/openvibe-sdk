#pragma once

#define OVP_Classification_BoxTrainerFormatVersion 4
#define OVP_Classification_BoxTrainerFormatVersionRequired 4

// Global defines
//---------------------------------------------------------------------------------------------------
#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
#include "ovp_global_defines.h"
#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines

#define OVP_TypeId_ClassificationPairwiseStrategy								OpenViBE::CIdentifier(0x0DD51C74, 0x3C4E74C9)
#define OVP_TypeId_OneVsOne_DecisionAlgorithms									OpenViBE::CIdentifier(0xDEC1510, 0xDEC1510)

#define OVP_ClassId_Algorithm_ClassifierLDA										OpenViBE::CIdentifier(0x2BA17A3C, 0x1BD46D84)
#define OVP_ClassId_Algorithm_ClassifierLDA_DecisionAvailable					OpenViBE::CIdentifier(0x79146976, 0xD7F01A25)
#define OVP_ClassId_Algorithm_ClassifierLDADesc									OpenViBE::CIdentifier(0x78FE2929, 0x644945B4)
#define OVP_ClassId_Algorithm_ClassifierNULL									OpenViBE::CIdentifier(0x043D09AB, 0xCB5E4859)
#define OVP_ClassId_Algorithm_ClassifierNULLDesc								OpenViBE::CIdentifier(0x3B365233, 0x812C47DD)
#define OVP_ClassId_Algorithm_ClassifierOneVsOne								OpenViBE::CIdentifier(0x638C2F90, 0xEAE10226)
#define OVP_ClassId_Algorithm_ClassifierOneVsOneDesc							OpenViBE::CIdentifier(0xE78E7CDB, 0x369AA9EF)
#define OVP_ClassId_Algorithm_ClassifierOneVsAll								OpenViBE::CIdentifier(0xD7183FC6, 0xBD74F297)
#define OVP_ClassId_Algorithm_ClassifierOneVsAllDesc							OpenViBE::CIdentifier(0xD42D5449, 0x7A28DDB0)
#define OVP_ClassId_Algorithm_ConditionedCovariance								OpenViBE::CIdentifier(0x0F3B77A6, 0x0301518A)
#define OVP_ClassId_Algorithm_ConditionedCovarianceDesc							OpenViBE::CIdentifier(0x18D15C41, 0x70545A66)
#define OVP_ClassId_Algorithm_PairwiseDecision									OpenViBE::CIdentifier(0x26EF6DDA, 0xF137053C)
#define OVP_ClassId_Algorithm_PairwiseDecisionDesc								OpenViBE::CIdentifier(0x191EB02A, 0x6866214A)
#define OVP_ClassId_Algorithm_PairwiseDecision_HT								OpenViBE::CIdentifier(0xD24F7F19, 0xA744FAD2)
#define OVP_ClassId_Algorithm_PairwiseDecision_HTDesc							OpenViBE::CIdentifier(0xE837F5C0, 0xF65C1341)
#define OVP_ClassId_Algorithm_PairwiseDecision_Voting							OpenViBE::CIdentifier(0xA111B830, 0x4679BAFD)
#define OVP_ClassId_Algorithm_PairwiseDecision_VotingDesc						OpenViBE::CIdentifier(0xAC5A39E8, 0x3A57822A)
#define OVP_ClassId_Algorithm_PairwiseStrategy_PKPD								OpenViBE::CIdentifier(0x26EF6DDA, 0xF137053C)
#define OVP_ClassId_Algorithm_PairwiseStrategy_PKPDDesc							OpenViBE::CIdentifier(0x191EB02A, 0x6866214A)
#define OVP_ClassId_BoxAlgorithm_ClassifierProcessor							OpenViBE::CIdentifier(0x5FE23D17, 0x95B0452C)
#define OVP_ClassId_BoxAlgorithm_ClassifierProcessorDesc						OpenViBE::CIdentifier(0x29B66B00, 0xB4683D49)
#define OVP_ClassId_BoxAlgorithm_ClassifierTrainer								OpenViBE::CIdentifier(0xF3DAE8A8, 0x3B444154)
#define OVP_ClassId_BoxAlgorithm_ClassifierTrainerDesc							OpenViBE::CIdentifier(0xFE277C91, 0x1593B824)
#define OVP_ClassId_BoxAlgorithm_VotingClassifier								OpenViBE::CIdentifier(0xFAF62C2B, 0x0B75D1B3)
#define OVP_ClassId_BoxAlgorithm_VotingClassifierDesc							OpenViBE::CIdentifier(0x97E3CCC5, 0xAC353ED2)

#define OVP_Algorithm_ClassifierLDA_InputParameterId_UseShrinkage				OpenViBE::CIdentifier(0x01357534, 0x028312A0)
#define OVP_Algorithm_ClassifierLDA_InputParameterId_Shrinkage					OpenViBE::CIdentifier(0x01357534, 0x028312A1)
#define OVP_Algorithm_ClassifierLDA_InputParameterId_DiagonalCov				OpenViBE::CIdentifier(0x067E45C5, 0x15285CC7)
#define OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter1				OpenViBE::CIdentifier(0x6DA99952, 0x7E72C143)
#define OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter2				OpenViBE::CIdentifier(0xEAC5694A, 0x56CFEF02)
#define OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter3				OpenViBE::CIdentifier(0x72F6222D, 0x375BAE2C)
#define OVP_Algorithm_OneVsOneStrategy_InputParameterId_DecisionType			OpenViBE::CIdentifier(0x0C347BBA, 0x180577F9)
#define OVP_Algorithm_ConditionedCovariance_InputParameterId_Shrinkage			OpenViBE::CIdentifier(0x54B90EA7, 0x600A4ACC)
#define OVP_Algorithm_ConditionedCovariance_InputParameterId_FeatureVectorSet	OpenViBE::CIdentifier(0x2CF30E42, 0x051F3996)
#define OVP_Algorithm_ConditionedCovariance_OutputParameterId_Mean				OpenViBE::CIdentifier(0x0C671FB7, 0x550B01B3)
#define OVP_Algorithm_ConditionedCovariance_OutputParameterId_CovarianceMatrix	OpenViBE::CIdentifier(0x19F07FB4, 0x084E273B)
#define OVP_Algorithm_Classifier_InputParameter_ProbabilityMatrix				OpenViBE::CIdentifier(0xF48D35AD, 0xB8EFF834)
#define OVP_Algorithm_Classifier_Pairwise_InputParameterId_Config				OpenViBE::CIdentifier(0x10EBAC09, 0x80926A63)
#define OVP_Algorithm_Classifier_Pairwise_InputParameterId_AlgorithmIdentifier	OpenViBE::CIdentifier(0xBE71BE18, 0x82A0E017)
#define OVP_Algorithm_Classifier_Pairwise_InputParameterId_SetRepartition		OpenViBE::CIdentifier(0xBE71BE18, 0x82A0E018)
#define OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassificationOutputs	OpenViBE::CIdentifier(0xBE71BE18, 0x82A0E019)
#define OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassCount				OpenViBE::CIdentifier(0xBE71BE18, 0x82A0E01A)
#define OVP_Algorithm_Classifier_OutputParameter_ProbabilityVector				OpenViBE::CIdentifier(0x883599FE, 0x2FDB32FF)
#define OVP_Algorithm_Classifier_Pairwise_OutputParameterId_Config				OpenViBE::CIdentifier(0x69F05A61, 0x25C94515)
#define OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Train					OpenViBE::CIdentifier(0x32219D21, 0xD3BE6105)
#define OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Parameterize			OpenViBE::CIdentifier(0x32219D21, 0xD3BE6106)
#define OVP_Algorithm_Classifier_Pairwise_InputTriggerId_Compute				OpenViBE::CIdentifier(0x3637344B, 0x05D03D7E)
#define OVP_Algorithm_Classifier_Pairwise_InputTriggerId_SaveConfig				OpenViBE::CIdentifier(0xF19574AD, 0x024045A7)
#define OVP_Algorithm_Classifier_Pairwise_InputTriggerId_LoadConfig				OpenViBE::CIdentifier(0x97AF6C6C, 0x670A12E6)

extern const char* const FORMAT_VERSION_ATTRIBUTE_NAME;
extern const char* const IDENTIFIER_ATTRIBUTE_NAME;

extern const char* const STRATEGY_NODE_NAME;
extern const char* const ALGORITHM_NODE_NAME;
extern const char* const STIMULATIONS_NODE_NAME;
extern const char* const REJECTED_CLASS_NODE_NAME;
extern const char* const CLASS_STIMULATION_NODE_NAME;

extern const char* const CLASSIFICATION_BOX_ROOT;
extern const char* const CLASSIFIER_ROOT;

extern const char* const PAIRWISE_STRATEGY_ENUMERATION_NAME;

extern const char* const MLP_EVALUATION_FUNCTION_NAME;
extern const char* const MLP_TRANSFERT_FUNCTION_NAME;

bool OVFloatEqual(double first, double second);
