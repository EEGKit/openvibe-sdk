#include "ovpCAlgorithmClassifierNULL.h"

#include <cstdlib>
#include <sstream>
#include <iostream>
#include <cstring>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Classification;

using namespace OpenViBEToolkit;

CAlgorithmClassifierNULL::CAlgorithmClassifierNULL() {}


bool CAlgorithmClassifierNULL::initialize()
{
	TParameterHandler<bool> ip_bParameter1(this->getInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter1));
	TParameterHandler<double> ip_f64Parameter2(this->getInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter2));
	TParameterHandler<uint64_t> ip_ui64Parameter3(this->getInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter3));

	ip_bParameter1    = true;
	ip_f64Parameter2  = 3.141592654;
	ip_ui64Parameter3 = OVTK_StimulationId_Label_00;

	TParameterHandler<XML::IXMLNode*> op_pConfiguration(this->getOutputParameter(OVTK_Algorithm_Classifier_OutputParameterId_Configuration));
	op_pConfiguration = nullptr;

	return CAlgorithmClassifier::initialize();
}

bool CAlgorithmClassifierNULL::train(const IFeatureVectorSet& /*featureVectorSet*/)
{
	TParameterHandler<bool> ip_bParameter1(this->getInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter1));
	TParameterHandler<double> ip_f64Parameter2(this->getInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter2));
	TParameterHandler<uint64_t> ip_ui64Parameter3(this->getInputParameter(OVP_Algorithm_ClassifierNULL_InputParameterId_Parameter3));

	OV_WARNING_K("Parameter 1 : " << ip_bParameter1);
	OV_WARNING_K("Parameter 2 : " << ip_f64Parameter2);
	OV_WARNING_K("Parameter 3 : " << ip_ui64Parameter3);

	return true;
}

bool CAlgorithmClassifierNULL::classify(const IFeatureVector& /*featureVector*/, double& classId, IVector& rDistanceValue, IVector& rProbabilityValue)
{
	classId = 1 + (rand() % 3);

	rDistanceValue.setSize(1);
	rProbabilityValue.setSize(1);
	if (classId == 1)
	{
		rDistanceValue[0]    = -1;
		rProbabilityValue[0] = 1;
	}
	else
	{
		rDistanceValue[0]    = 1;
		rProbabilityValue[0] = 0;
	}
	return true;
}

XML::IXMLNode* CAlgorithmClassifierNULL::saveConfiguration() { return nullptr; }

bool CAlgorithmClassifierNULL::loadConfiguration(XML::IXMLNode* pConfigurationNode) { return true; }

uint32_t CAlgorithmClassifierNULL::getOutputProbabilityVectorLength() { return 1; }

uint32_t CAlgorithmClassifierNULL::getOutputDistanceVectorLength() { return 1; }
