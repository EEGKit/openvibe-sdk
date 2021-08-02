#include "ovpCAlgorithmClassifierNULL.h"

#include <cstdlib>
#include <iostream>

namespace OpenViBE {
namespace Plugins {
namespace Classification {


bool CAlgorithmClassifierNULL::initialize()
{
	Kernel::TParameterHandler<bool> ip_bParameter1(this->getInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter1));
	Kernel::TParameterHandler<double> ip_Parameter2(this->getInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter2));
	Kernel::TParameterHandler<uint64_t> ip_parameter3(this->getInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter3));

	ip_bParameter1 = true;
	ip_Parameter2  = 3.141592654;
	ip_parameter3  = OVTK_StimulationId_Label_00;

	Kernel::TParameterHandler<XML::IXMLNode*> op_configuration(this->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Config));
	op_configuration = nullptr;

	return CAlgorithmClassifier::initialize();
}

bool CAlgorithmClassifierNULL::train(const Toolkit::IFeatureVectorSet& /*featureVectorSet*/)
{
	Kernel::TParameterHandler<bool> ip_bParameter1(this->getInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter1));
	Kernel::TParameterHandler<double> ip_Parameter2(this->getInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter2));
	Kernel::TParameterHandler<uint64_t> ip_parameter3(this->getInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter3));

	OV_WARNING_K("Parameter 1 : " << ip_bParameter1);
	OV_WARNING_K("Parameter 2 : " << ip_Parameter2);
	OV_WARNING_K("Parameter 3 : " << ip_parameter3);

	return true;
}

bool CAlgorithmClassifierNULL::classify(const Toolkit::IFeatureVector& /*featureVector*/, double& classId, Toolkit::IVector& distance, Toolkit::IVector& probability)
{
	classId = 1 + (rand() % 3);

	distance.setSize(1);
	probability.setSize(1);
	if (classId == 1)
	{
		distance[0]    = -1;
		probability[0] = 1;
	}
	else
	{
		distance[0]    = 1;
		probability[0] = 0;
	}
	return true;
}
}  // namespace Classification
}  // namespace Plugins
}  // namespace OpenViBE
