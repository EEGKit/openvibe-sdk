#pragma once

#if defined TARGET_HAS_ThirdPartyEIGEN

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>

#include <Eigen/Eigenvalues>

#include "ovpCAlgorithmClassifierLDA.h"
#include <xml/IXMLNode.h>

namespace OpenViBEPlugins
{
	namespace Classification
	{
		//The purpose of this class is to compute the "membership" of a vector
		class CAlgorithmLDADiscriminantFunction
		{
		public:
			CAlgorithmLDADiscriminantFunction() {}

			void setWeight(const Eigen::VectorXd& weigth) { m_weight = weigth; }
			void setBias(const double bias) { m_bias = bias; }

			//Return the class membership of the feature vector
			double getValue(const Eigen::VectorXd& featureVector) { return (m_weight.transpose() * featureVector)(0) + m_bias; }
			uint32_t getWeightVectorSize() const { return m_weight.size(); }


			bool loadConfiguration(const XML::IXMLNode* configuration);
			XML::IXMLNode* getConfiguration();

			const Eigen::VectorXd& getWeight() const { return m_weight; }
			double getBias() const { return m_bias; }

		private:
			double m_bias = 0;
			Eigen::VectorXd m_weight;
		};
	} // namespace Classification
} // namespace OpenViBEPlugins

#endif
