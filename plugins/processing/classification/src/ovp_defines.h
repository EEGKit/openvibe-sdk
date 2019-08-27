#pragma once

#define OVP_Classification_BoxTrainerFormatVersion                  4
#define OVP_Classification_BoxTrainerFormatVersionRequired          4

//___________________________________________________________________//
//                                                                   //
// Global defines                                                   //
//___________________________________________________________________//
//                                                                   //

#ifdef TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines
#include "ovp_global_defines.h"
#endif // TARGET_HAS_ThirdPartyOpenViBEPluginsGlobalDefines


#define OVP_TypeId_ClassificationPairwiseStrategy OpenViBE::CIdentifier(0x0DD51C74, 0x3C4E74C9)
#define OVP_TypeId_OneVsOne_DecisionAlgorithms OpenViBE::CIdentifier(0xDEC1510, 0xDEC1510)


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

bool ov_float_equal(double first, double second);
