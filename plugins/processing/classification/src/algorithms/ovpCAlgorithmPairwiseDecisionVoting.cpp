#define VOTING_DEBUG 0
#include "ovpCAlgorithmPairwiseDecisionVoting.h"

#include <iostream>

#include <xml/IXMLNode.h>

namespace
{
	const char* const TYPE_NODE_NAME = "PairwiseDecision_Voting";
}

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Classification;

using namespace OpenViBEToolkit;

bool CAlgorithmPairwiseDecisionVoting::parameterize()
{
	TParameterHandler<uint64_t> ip_pClassCount(this->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassCount));
	m_nClass = uint32_t(ip_pClassCount);

	OV_ERROR_UNLESS_KRF(m_nClass >= 2, "Pairwise decision Voting algorithm needs at least 2 classes [" << m_nClass << "] found",
						OpenViBE::Kernel::ErrorType::BadInput);

	return true;
}

bool CAlgorithmPairwiseDecisionVoting::compute(std::vector<SClassificationInfo>& pClassificationValueList, IMatrix* pProbabilityVector)
{
	OV_ERROR_UNLESS_KRF(m_nClass >= 2, "Pairwise decision Voting algorithm needs at least 2 classes [" << m_nClass << "] found",
						OpenViBE::Kernel::ErrorType::BadInput);

#if VOTING_DEBUG
	std::cout << pClassificationValueList.size() << std::endl;

	for (uint32_t i = 0 ; i< pClassificationValueList.size() ; ++i){
		std::cout << pClassificationValueList[i].m_f64FirstClass << " " << pClassificationValueList[i].m_f64SecondClass << std::endl;
		std::cout << pClassificationValueList[i].m_f64ClassLabel;
		std::cout << std::endl;
	}
#endif

	uint32_t* l_pWinCount = new uint32_t[m_nClass];
	for (size_t i = 0; i < m_nClass; ++i) { l_pWinCount[i] = 0; }

	for (uint32_t i = 0; i < pClassificationValueList.size(); ++i)
	{
		SClassificationInfo& l_rTemp = pClassificationValueList[i];
		if (l_rTemp.m_f64ClassLabel == 0) { ++(l_pWinCount[uint32_t(l_rTemp.m_f64FirstClass)]); }
		else { ++(l_pWinCount[uint32_t(l_rTemp.m_f64SecondClass)]); }
	}

#if VOTING_DEBUG
	for (size_t i = 0; i < m_nClass ;  ++i)
	{
		std::cout << (double(l_pWinCount[i])/pClassificationValueList.size() <<  " ";
	}
	std::cout << std::endl;
#endif

	pProbabilityVector->setDimensionCount(1);
	pProbabilityVector->setDimensionSize(0, m_nClass);

	for (uint32_t i = 0; i < m_nClass; ++i) { pProbabilityVector->getBuffer()[i] = double(l_pWinCount[i]) / pClassificationValueList.size(); }

	delete[] l_pWinCount;
	return true;
}

XML::IXMLNode* CAlgorithmPairwiseDecisionVoting::saveConfiguration()
{
	XML::IXMLNode* l_pRootNode = XML::createNode(TYPE_NODE_NAME);
	return l_pRootNode;
}

