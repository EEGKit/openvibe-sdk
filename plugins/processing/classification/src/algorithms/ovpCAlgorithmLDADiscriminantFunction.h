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
			CAlgorithmLDADiscriminantFunction();

			void setWeight(const Eigen::VectorXd& rWeigth);
			void setBias(double f64Bias);

			//Return the class membership of the feature vector
			double getValue(const Eigen::VectorXd& rFeatureVector);
			uint32_t getWeightVectorSize();

			bool loadConfiguration(const XML::IXMLNode* pConfiguration);
			XML::IXMLNode* getConfiguration();

			const Eigen::VectorXd& getWeight() const { return m_oWeight; }
			double getBias() const { return m_f64Bias; }

		private:
			double m_f64Bias = 0;
			Eigen::VectorXd m_oWeight;
		};
	} // namespace Classification
} // namespace OpenViBEPlugins

#endif
