#define HT_DEBUG 0

#define ALPHA_DELTA 0.01
#include "ovpCAlgorithmPairwiseDecisionHT.h"

#include <iostream>
#include <sstream>


#include <xml/IXMLNode.h>
#include <xml/IXMLHandler.h>

namespace
{
	const char* const TYPE_NODE_NAME        = "PairwiseDecision_HT";
	const char* const REPARTITION_NODE_NAME = "Repartition";
}


using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Classification;

using namespace OpenViBEToolkit;

bool CAlgorithmPairwiseDecisionHT::initialize() { return true; }

bool CAlgorithmPairwiseDecisionHT::uninitialize() { return true; }

bool CAlgorithmPairwiseDecisionHT::parameterize()
{
	TParameterHandler<uint64_t> ip_pClassCount(this->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassCount));
	m_nClass = uint32_t(ip_pClassCount);

	OV_ERROR_UNLESS_KRF(m_nClass >= 2, "Pairwise decision HT algorithm needs at least 2 classes [" << m_nClass << "] found",
						OpenViBE::Kernel::ErrorType::BadInput);

	return true;
}


bool CAlgorithmPairwiseDecisionHT::compute(std::vector<SClassificationInfo>& pClassificationValueList, IMatrix* probabilityVector)
{
	OV_ERROR_UNLESS_KRF(m_nClass >= 2, "Pairwise decision HT algorithm needs at least 2 classes [" << m_nClass << "] found",
						OpenViBE::Kernel::ErrorType::BadConfig);

	TParameterHandler<IMatrix*> ip_pRepartitionSetVector = this->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_SetRepartition);
	double* l_pProbabilityMatrix                         = new double[m_nClass * m_nClass];

	//First we set the diagonal to 0
	for (size_t i = 0; i < m_nClass; ++i) { l_pProbabilityMatrix[i * m_nClass + i] = 0.; }

#if HT_DEBUG
	for(uint32_t i = 0 ; i< m_nClass ; ++i){

		for(uint32_t j = 0 ; j<m_nClass ; ++j){
			std::cout << l_pProbabilityMatrix[i*m_nClass + j] << " ";
		}
		std::cout << std::endl;
	}
#endif

	for (size_t i = 0; i < pClassificationValueList.size(); ++i)
	{
		SClassificationInfo& l_rTemp                                                = pClassificationValueList[i];
		const uint32_t l_f64FirstIndex                                              = uint32_t(l_rTemp.m_f64FirstClass);
		const uint32_t l_f64SecondIndex                                             = uint32_t(l_rTemp.m_f64SecondClass);
		const double* l_pValues                                                     = l_rTemp.m_pClassificationValue->getBuffer();
		l_pProbabilityMatrix[l_f64FirstIndex * m_nClass + l_f64SecondIndex] = l_pValues[0];
		l_pProbabilityMatrix[l_f64SecondIndex * m_nClass + l_f64FirstIndex] = 1 - l_pValues[0];
	}

	double* l_pP                = new double[m_nClass];
	double** l_pMu              = new double*[m_nClass];
	uint32_t l_ui32AmountSample = 0;

	for (size_t i = 0; i < m_nClass; ++i) { l_pMu[i] = new double[m_nClass]; }

	for (size_t i = 0; i < m_nClass; ++i) { l_ui32AmountSample += uint32_t(ip_pRepartitionSetVector->getBuffer()[i]); }

	for (size_t i = 0; i < m_nClass; ++i) { l_pP[i] = ip_pRepartitionSetVector->getBuffer()[i] / l_ui32AmountSample; }

	for (size_t i = 0; i < m_nClass; ++i)
	{
		for (size_t j = 0; j < m_nClass; ++j)
		{
			if (i != j) { l_pMu[i][j] = l_pP[i] / (l_pP[i] + l_pP[j]); }
			else { l_pMu[i][i] = 0; }
		}
	}

#if HT_DEBUG
	std::cout << "Initial probability and Mu" << std::endl;
	for(uint32_t i = 0 ; i< m_nClass ; ++i){
			std::cout << l_pP[i] << " ";
	}
	std::cout << std::endl << std::endl;

	for(uint32_t i = 0 ; i< m_nClass ; ++i){

		for(uint32_t j = 0 ; j<m_nClass ; ++j){
			std::cout << l_pMu[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
#endif


	uint32_t consecutiveAlpha = 0;
	uint32_t index            = 0;
	while (consecutiveAlpha != m_nClass)
	{
		double firstSum  = 0.0;
		double secondSum = 0.0;

		for (size_t j = 0; j < m_nClass; ++j)
		{
			if (j != index)
			{
				const uint32_t l_ui32Temp = uint32_t(l_pProbabilityMatrix[index] + ip_pRepartitionSetVector->getBuffer()[j]);

				firstSum += l_ui32Temp * l_pProbabilityMatrix[index * m_nClass + j];
				secondSum += l_ui32Temp * l_pMu[index][j];
			}
		}
		
		const double alpha = (secondSum != 0) ?  firstSum / secondSum : 1;

		for (size_t j = 0; j < m_nClass; ++j)
		{
			if (j != index)
			{
				l_pMu[index][j] = (alpha * l_pMu[index][j]) / (alpha * l_pMu[index][j] + l_pMu[j][index]);
				l_pMu[j][index] = 1 - l_pMu[index][j];
			}
		}

		l_pP[index] *= alpha;
		if (alpha > 1 - ALPHA_DELTA && alpha < 1 + ALPHA_DELTA) { ++consecutiveAlpha; }
		else { consecutiveAlpha = 0; }
		index = (index + 1) % m_nClass;

#if HT_DEBUG
	std::cout << "Intermediate probability, MU and alpha" << std::endl;
	std::cout << alpha << std::endl;
	for(uint32_t i = 0 ; i< m_nClass ; ++i){
			std::cout << l_pP[i] << " ";
	}
	std::cout << std::endl << std::endl;

	for(uint32_t i = 0 ; i< m_nClass ; ++i){

		for(uint32_t j = 0 ; j<m_nClass ; ++j){
			std::cout << l_pMu[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
#endif
	}


#if HT_DEBUG
	std::cout << "Result " << std::endl;
	for(uint32_t i = 0; i<m_nClass ; ++i) { std::cout << l_pP[i] << " "; }
	std::cout << std::endl << std::endl;
#endif

	probabilityVector->setDimensionCount(1);
	probabilityVector->setDimensionSize(0, m_nClass);
	for (uint32_t i = 0; i < m_nClass; ++i) { probabilityVector->getBuffer()[i] = l_pP[i]; }

	delete[] l_pP;
	for (size_t i = 0; i < m_nClass; ++i) { delete[] l_pMu[i]; }
	delete[] l_pProbabilityMatrix;
	delete[] l_pMu;
	return true;
}

XML::IXMLNode* CAlgorithmPairwiseDecisionHT::saveConfiguration()
{
	XML::IXMLNode* rootNode = XML::createNode(TYPE_NODE_NAME);

	TParameterHandler<IMatrix*> ip_pRepartitionSetVector = this->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_SetRepartition);
	const uint32_t nClass                                = ip_pRepartitionSetVector->getDimensionSize(0);

	std::stringstream ss;
	for (size_t i = 0; i < nClass; i++) { ss << ip_pRepartitionSetVector->getBuffer()[i] << " "; }
	XML::IXMLNode* repartition = XML::createNode(REPARTITION_NODE_NAME);
	repartition->setPCData(ss.str().c_str());
	rootNode->addChild(repartition);

	return rootNode;
}

bool CAlgorithmPairwiseDecisionHT::loadConfiguration(XML::IXMLNode& node)
{
	std::stringstream ss(node.getChildByName(REPARTITION_NODE_NAME)->getPCData());
	TParameterHandler<IMatrix*> ip_pRepartitionSetVector = this->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_SetRepartition);


	std::vector<double> repartition;
	while (!ss.eof())
	{
		uint32_t value;
		ss >> value;
		repartition.push_back(value);
	}

	ip_pRepartitionSetVector->setDimensionCount(1);
	ip_pRepartitionSetVector->setDimensionSize(0, repartition.size());
	for (size_t i = 0; i < repartition.size(); i++) { ip_pRepartitionSetVector->getBuffer()[i] = repartition[i]; }
	return true;
}
