#include "ovpCAlgorithmLDADiscriminantFunction.h"
#if defined TARGET_HAS_ThirdPartyEIGEN

#include <Eigen/Eigenvalues>

#include <sstream>
#include <vector>

namespace OpenViBE {
namespace Plugins {
namespace Classification {

static const char* const BASE_NODE_NAME   = "Class-config";
static const char* const WEIGHT_NODE_NAME = "Weights";
static const char* const BIAS_NODE_NAME   = "Bias";

bool CAlgorithmLDADiscriminantFunction::loadConfig(const XML::IXMLNode* configuration)
{
	std::stringstream bias(configuration->getChildByName(BIAS_NODE_NAME)->getPCData());
	bias >> m_bias;

	std::stringstream data(configuration->getChildByName(WEIGHT_NODE_NAME)->getPCData());
	std::vector<double> coefficients;
	while (!data.eof())
	{
		double value;
		data >> value;
		coefficients.push_back(value);
	}

	m_weight.resize(coefficients.size());
	for (size_t i = 0; i < coefficients.size(); ++i) { m_weight(i, 0) = coefficients[i]; }
	return true;
}

XML::IXMLNode* CAlgorithmLDADiscriminantFunction::getConfiguration()
{
	XML::IXMLNode* rootNode = XML::createNode(BASE_NODE_NAME);

	std::stringstream weigths, bias;

	weigths << std::scientific;
	for (int i = 0; i < m_weight.size(); ++i) { weigths << " " << m_weight(i, 0); }

	bias << m_bias;

	XML::IXMLNode* tempNode = XML::createNode(WEIGHT_NODE_NAME);
	tempNode->setPCData(weigths.str().c_str());
	rootNode->addChild(tempNode);

	tempNode = XML::createNode(BIAS_NODE_NAME);
	tempNode->setPCData(bias.str().c_str());
	rootNode->addChild(tempNode);

	return rootNode;
}

}  // namespace Classification
}  // namespace Plugins
}  // namespace OpenViBE
#endif
