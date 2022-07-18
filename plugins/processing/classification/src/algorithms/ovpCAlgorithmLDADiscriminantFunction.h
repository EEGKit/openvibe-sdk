#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>

#include <Eigen/Eigenvalues>

#include "ovpCAlgorithmClassifierLDA.h"
#include <xml/IXMLNode.h>

namespace OpenViBE {
namespace Plugins {
namespace Classification {
//The purpose of this class is to compute the "membership" of a vector
class CAlgorithmLDADiscriminantFunction
{
public:
	CAlgorithmLDADiscriminantFunction() {}

	void setWeight(const Eigen::VectorXd& weigth) { m_weight = weigth; }
	void setBias(const double bias) { m_bias = bias; }

	//Return the class membership of the feature vector
	double getValue(const Eigen::VectorXd& featureVector) { return (m_weight.transpose() * featureVector)(0) + m_bias; }
	size_t getNWeight() const { return m_weight.size(); }


	bool loadConfig(const XML::IXMLNode* configuration);
	XML::IXMLNode* getConfiguration();

	const Eigen::VectorXd& getWeight() const { return m_weight; }
	double getBias() const { return m_bias; }

private:
	double m_bias = 0;
	Eigen::VectorXd m_weight;
};
}  // namespace Classification
}  // namespace Plugins
}  // namespace OpenViBE
