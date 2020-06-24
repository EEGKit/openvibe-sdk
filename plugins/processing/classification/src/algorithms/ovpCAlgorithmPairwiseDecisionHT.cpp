#define HT_DEBUG 0

#define ALPHA_DELTA 0.01
#include "ovpCAlgorithmPairwiseDecisionHT.h"

#include <iostream>
#include <sstream>

#include <xml/IXMLNode.h>
#include <xml/IXMLHandler.h>

namespace {
const char* const TYPE_NODE_NAME        = "PairwiseDecision_HT";
const char* const REPARTITION_NODE_NAME = "Repartition";
}


using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace Classification;

using namespace /*OpenViBE::*/Toolkit;

bool CAlgorithmPairwiseDecisionHT::parameterize()
{
	TParameterHandler<uint64_t> ip_nClass(this->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameter_ClassCount));
	m_nClass = size_t(ip_nClass);

	OV_ERROR_UNLESS_KRF(m_nClass >= 2, "Pairwise decision HT algorithm needs at least 2 classes [" << m_nClass << "] found",
						Kernel::ErrorType::BadInput);

	return true;
}


bool CAlgorithmPairwiseDecisionHT::compute(std::vector<classification_info_t>& classifications, CMatrix* probabilities)
{
	OV_ERROR_UNLESS_KRF(m_nClass >= 2, "Pairwise decision HT algorithm needs at least 2 classes [" << m_nClass << "] found",
						Kernel::ErrorType::BadConfig);

	TParameterHandler<CMatrix*> ip_Repartition = this->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_SetRepartition);
	std::vector<double> probability(m_nClass * m_nClass);

	//First we set the diagonal to 0
	for (size_t i = 0; i < m_nClass; ++i) { probability[i * m_nClass + i] = 0.; }

#if HT_DEBUG
	for (size_t i = 0 ; i< m_nClass ; ++i){

		for (size_t j = 0 ; j<m_nClass ; ++j){
			std::cout << probability[i*m_nClass + j] << " ";
		}
		std::cout << std::endl;
	}
#endif

	for (size_t i = 0; i < classifications.size(); ++i)
	{
		classification_info_t& temp                  = classifications[i];
		const size_t firstIdx                        = size_t(temp.firstClass);
		const size_t secondIdx                       = size_t(temp.secondClass);
		const double* values                         = temp.classificationValue->getBuffer();
		probability[firstIdx * m_nClass + secondIdx] = values[0];
		probability[secondIdx * m_nClass + firstIdx] = 1 - values[0];
	}

	std::vector<double> p(m_nClass);
	std::vector<std::vector<double>> mu(m_nClass);
	size_t amountSample = 0;

	for (size_t i = 0; i < m_nClass; ++i) { mu[i].resize(m_nClass); }
	for (size_t i = 0; i < m_nClass; ++i) { amountSample += size_t(ip_Repartition->getBuffer()[i]); }
	for (size_t i = 0; i < m_nClass; ++i) { p[i] = ip_Repartition->getBuffer()[i] / amountSample; }

	for (size_t i = 0; i < m_nClass; ++i)
	{
		for (size_t j = 0; j < m_nClass; ++j)
		{
			if (i != j) { mu[i][j] = p[i] / (p[i] + p[j]); }
			else { mu[i][i] = 0; }
		}
	}

#if HT_DEBUG
	std::cout << "Initial probability and Mu" << std::endl;
	for (size_t i = 0 ; i < m_nClass ; ++i) { std::cout << p[i] << " "; }
	std::cout << std::endl << std::endl;

	for (size_t i = 0 ; i< m_nClass ; ++i)
	{
		for (size_t j = 0 ; j<m_nClass ; ++j) { std::cout << mu[i][j] << " "; }
		std::cout << std::endl;
	}
	std::cout << std::endl;
#endif


	size_t consecutiveAlpha = 0;
	size_t index            = 0;
	while (consecutiveAlpha != m_nClass)
	{
		double firstSum  = 0.0;
		double secondSum = 0.0;

		for (size_t j = 0; j < m_nClass; ++j)
		{
			if (j != index)
			{
				const size_t temp = size_t(probability[index] + ip_Repartition->getBuffer()[j]);

				firstSum += temp * probability[index * m_nClass + j];
				secondSum += temp * mu[index][j];
			}
		}

		const double alpha = (secondSum != 0) ? firstSum / secondSum : 1;

		for (size_t j = 0; j < m_nClass; ++j)
		{
			if (j != index)
			{
				mu[index][j] = (alpha * mu[index][j]) / (alpha * mu[index][j] + mu[j][index]);
				mu[j][index] = 1 - mu[index][j];
			}
		}

		p[index] *= alpha;
		if (alpha > 1 - ALPHA_DELTA && alpha < 1 + ALPHA_DELTA) { ++consecutiveAlpha; }
		else { consecutiveAlpha = 0; }
		index = (index + 1) % m_nClass;

#if HT_DEBUG
	std::cout << "Intermediate probability, MU and alpha" << std::endl << alpha << std::endl;
	for (size_t i = 0 ; i< m_nClass ; ++i) { std::cout << p[i] << " "; }
	std::cout << std::endl << std::endl;

	for (size_t i = 0 ; i< m_nClass ; ++i)
	{
		for (size_t j = 0 ; j<m_nClass ; ++j) { std::cout << mu[i][j] << " "; }
		std::cout << std::endl;
	}
	std::cout << std::endl;
#endif
	}


#if HT_DEBUG
	std::cout << "Result " << std::endl;
	for (size_t i = 0; i<m_nClass ; ++i) { std::cout << p[i] << " "; }
	std::cout << std::endl << std::endl;
#endif

	probabilities->setDimensionCount(1);
	probabilities->setDimensionSize(0, m_nClass);
	for (size_t i = 0; i < m_nClass; ++i) { probabilities->getBuffer()[i] = p[i]; }
	return true;
}

XML::IXMLNode* CAlgorithmPairwiseDecisionHT::saveConfig()
{
	XML::IXMLNode* node = XML::createNode(TYPE_NODE_NAME);

	TParameterHandler<CMatrix*> ip_repartition = this->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_SetRepartition);
	const size_t nClass                        = ip_repartition->getDimensionSize(0);

	std::stringstream ss;
	for (size_t i = 0; i < nClass; ++i) { ss << ip_repartition->getBuffer()[i] << " "; }
	XML::IXMLNode* repartition = XML::createNode(REPARTITION_NODE_NAME);
	repartition->setPCData(ss.str().c_str());
	node->addChild(repartition);

	return node;
}

bool CAlgorithmPairwiseDecisionHT::loadConfig(XML::IXMLNode& node)
{
	std::stringstream ss(node.getChildByName(REPARTITION_NODE_NAME)->getPCData());
	TParameterHandler<CMatrix*> ip_repartition = this->getInputParameter(OVP_Algorithm_Classifier_Pairwise_InputParameterId_SetRepartition);


	std::vector<double> repartition;
	while (!ss.eof())
	{
		size_t value;
		ss >> value;
		repartition.push_back(double(value));
	}

	ip_repartition->setDimensionCount(1);
	ip_repartition->setDimensionSize(0, repartition.size());
	for (size_t i = 0; i < repartition.size(); ++i) { ip_repartition->getBuffer()[i] = repartition[i]; }
	return true;
}
