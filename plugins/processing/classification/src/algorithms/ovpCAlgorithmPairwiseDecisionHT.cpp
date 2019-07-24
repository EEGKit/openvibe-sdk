#define HT_DEBUG 0

#define ALPHA_DELTA 0.01
#include "ovpCAlgorithmPairwiseDecisionHT.h"

#include <iostream>
#include <sstream>


#include <xml/IXMLNode.h>
#include <xml/IXMLHandler.h>

namespace
{
	const char* const c_sTypeNodeName        = "PairwiseDecision_HT";
	const char* const c_sRepartitionNodeName = "Repartition";
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
	m_ui32ClassCount = static_cast<uint32_t>(ip_pClassCount);

	OV_ERROR_UNLESS_KRF(m_ui32ClassCount >= 2, "Pairwise decision HT algorithm needs at least 2 classes [" << m_ui32ClassCount << "] found", OpenViBE::Kernel::ErrorType::BadInput);

	return true;
}


bool CAlgorithmPairwiseDecisionHT::compute(std::vector<SClassificationInfo>& pClassificationValueList, IMatrix* pProbabilityVector)
{
	OV_ERROR_UNLESS_KRF(m_ui32ClassCount >= 2, "Pairwise decision HT algorithm needs at least 2 classes [" << m_ui32ClassCount << "] found", OpenViBE::Kernel::ErrorType::BadConfig);

	TParameterHandler<IMatrix*> ip_pRepartitionSetVector = this->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_SetRepartition);
	double* l_pProbabilityMatrix                         = new double[m_ui32ClassCount * m_ui32ClassCount];

	//First we set the diagonal to 0
	for (size_t i = 0; i < m_ui32ClassCount; ++i)
	{
		l_pProbabilityMatrix[i * m_ui32ClassCount + i] = 0.;
	}

#if HT_DEBUG
	for(uint32_t i = 0 ; i< m_ui32ClassCount ; ++i){

		for(uint32_t j = 0 ; j<m_ui32ClassCount ; ++j){
			std::cout << l_pProbabilityMatrix[i*m_ui32ClassCount + j] << " ";
		}
		std::cout << std::endl;
	}
#endif

	for (size_t i = 0; i < pClassificationValueList.size(); ++i)
	{
		SClassificationInfo& l_rTemp                                                = pClassificationValueList[i];
		const uint32_t l_f64FirstIndex                                              = static_cast<uint32_t>(l_rTemp.m_f64FirstClass);
		const uint32_t l_f64SecondIndex                                             = static_cast<uint32_t>(l_rTemp.m_f64SecondClass);
		const double* l_pValues                                                     = l_rTemp.m_pClassificationValue->getBuffer();
		l_pProbabilityMatrix[l_f64FirstIndex * m_ui32ClassCount + l_f64SecondIndex] = l_pValues[0];
		l_pProbabilityMatrix[l_f64SecondIndex * m_ui32ClassCount + l_f64FirstIndex] = 1 - l_pValues[0];
	}

	double* l_pP                = new double[m_ui32ClassCount];
	double** l_pMu              = new double*[m_ui32ClassCount];
	uint32_t l_ui32AmountSample = 0;

	for (size_t i = 0; i < m_ui32ClassCount; ++i) { l_pMu[i] = new double[m_ui32ClassCount]; }

	for (size_t i = 0; i < m_ui32ClassCount; ++i)
	{
		l_ui32AmountSample += static_cast<uint32_t>(ip_pRepartitionSetVector->getBuffer()[i]);
	}

	for (size_t i = 0; i < m_ui32ClassCount; ++i)
	{
		l_pP[i] = ip_pRepartitionSetVector->getBuffer()[i] / l_ui32AmountSample;
	}

	for (size_t i = 0; i < m_ui32ClassCount; ++i)
	{
		for (size_t j = 0; j < m_ui32ClassCount; ++j)
		{
			if (i != j)
			{
				l_pMu[i][j] = l_pP[i] / (l_pP[i] + l_pP[j]);
			}
			else
			{
				l_pMu[i][i] = 0;
			}
		}
	}

#if HT_DEBUG
	std::cout << "Initial probability and Mu" << std::endl;
	for(uint32_t i = 0 ; i< m_ui32ClassCount ; ++i){
			std::cout << l_pP[i] << " ";
	}
	std::cout << std::endl << std::endl;

	for(uint32_t i = 0 ; i< m_ui32ClassCount ; ++i){

		for(uint32_t j = 0 ; j<m_ui32ClassCount ; ++j){
			std::cout << l_pMu[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
#endif


	uint32_t l_ui32ConsecutiveAlpha = 0;
	uint32_t l_ui32Index            = 0;
	while (l_ui32ConsecutiveAlpha != m_ui32ClassCount)
	{
		double l_f64FirstSum  = 0.0;
		double l_f64SecondSum = 0.0;
		double l_f64Alpha     = 0.0;

		for (size_t j = 0; j < m_ui32ClassCount; ++j)
		{
			if (j != l_ui32Index)
			{
				const uint32_t l_ui32Temp = static_cast<uint32_t>(l_pProbabilityMatrix[l_ui32Index] + ip_pRepartitionSetVector->getBuffer()[j]);

				l_f64FirstSum += l_ui32Temp * l_pProbabilityMatrix[l_ui32Index * m_ui32ClassCount + j];
				l_f64SecondSum += l_ui32Temp * l_pMu[l_ui32Index][j];
			}
		}
		if (l_f64SecondSum != 0)
		{
			l_f64Alpha = l_f64FirstSum / l_f64SecondSum;
		}
		else
		{
			l_f64Alpha = 1;
		}

		for (size_t j = 0; j < m_ui32ClassCount; ++j)
		{
			if (j != l_ui32Index)
			{
				l_pMu[l_ui32Index][j] = (l_f64Alpha * l_pMu[l_ui32Index][j]) /
										(l_f64Alpha * l_pMu[l_ui32Index][j] + l_pMu[j][l_ui32Index]);
				l_pMu[j][l_ui32Index] = 1 - l_pMu[l_ui32Index][j];
			}
		}

		l_pP[l_ui32Index] *= l_f64Alpha;
		if (l_f64Alpha > 1 - ALPHA_DELTA && l_f64Alpha < 1 + ALPHA_DELTA)
		{
			++l_ui32ConsecutiveAlpha;
		}
		else
		{
			l_ui32ConsecutiveAlpha = 0;
		}
		l_ui32Index = (l_ui32Index + 1) % m_ui32ClassCount;

#if HT_DEBUG
	std::cout << "Intermediate probability, MU and alpha" << std::endl;
	std::cout << l_f64Alpha << std::endl;
	for(uint32_t i = 0 ; i< m_ui32ClassCount ; ++i){
			std::cout << l_pP[i] << " ";
	}
	std::cout << std::endl << std::endl;

	for(uint32_t i = 0 ; i< m_ui32ClassCount ; ++i){

		for(uint32_t j = 0 ; j<m_ui32ClassCount ; ++j){
			std::cout << l_pMu[i][j] << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
#endif
	}


#if HT_DEBUG
	std::cout << "Result " << std::endl;
	for(uint32_t i = 0; i<m_ui32ClassCount ; ++i) { std::cout << l_pP[i] << " "; }
	std::cout << std::endl << std::endl;
#endif

	pProbabilityVector->setDimensionCount(1);
	pProbabilityVector->setDimensionSize(0, m_ui32ClassCount);
	for (uint32_t i = 0; i < m_ui32ClassCount; ++i)
	{
		pProbabilityVector->getBuffer()[i] = l_pP[i];
	}

	delete[] l_pP;
	for (size_t i = 0; i < m_ui32ClassCount; ++i) { delete[] l_pMu[i]; }
	delete[] l_pProbabilityMatrix;
	delete[] l_pMu;
	return true;
}

XML::IXMLNode* CAlgorithmPairwiseDecisionHT::saveConfiguration()
{
	XML::IXMLNode* l_pRootNode = XML::createNode(c_sTypeNodeName);

	TParameterHandler<IMatrix*> ip_pRepartitionSetVector = this->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_SetRepartition);
	const uint32_t l_ui32ClassCount                      = ip_pRepartitionSetVector->getDimensionSize(0);

	std::stringstream l_sRepartition;
	for (size_t i = 0; i < l_ui32ClassCount; i++)
	{
		l_sRepartition << ip_pRepartitionSetVector->getBuffer()[i] << " ";
	}
	XML::IXMLNode* l_pRepartition = XML::createNode(c_sRepartitionNodeName);
	l_pRepartition->setPCData(l_sRepartition.str().c_str());
	l_pRootNode->addChild(l_pRepartition);

	return l_pRootNode;
}

bool CAlgorithmPairwiseDecisionHT::loadConfiguration(XML::IXMLNode& rNode)
{
	std::stringstream l_sData(rNode.getChildByName(c_sRepartitionNodeName)->getPCData());
	TParameterHandler<IMatrix*> ip_pRepartitionSetVector = this->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_SetRepartition);


	std::vector<double> l_vRepartition;
	while (!l_sData.eof())
	{
		uint32_t l_ui32Value;
		l_sData >> l_ui32Value;
		l_vRepartition.push_back(l_ui32Value);
	}

	ip_pRepartitionSetVector->setDimensionCount(1);
	ip_pRepartitionSetVector->setDimensionSize(0, l_vRepartition.size());
	for (size_t i = 0; i < l_vRepartition.size(); i++)
	{
		ip_pRepartitionSetVector->getBuffer()[i] = l_vRepartition[i];
	}
	return true;
}
