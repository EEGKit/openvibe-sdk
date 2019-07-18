#ifndef __OpenViBEToolkit_Algorithm_Pairing_Strategy_H__
#define __OpenViBEToolkit_Algorithm_Pairing_Strategy_H__

#include "../ovtkTAlgorithm.h"
#include "../../ovtkIVector.h"
#include "../../ovtkIFeatureVector.h"
#include "../../ovtkIFeatureVectorSet.h"
#include "ovtkCAlgorithmClassifier.h"

#include <vector>


#define OVTK_ClassId_Algorithm_PairingStrategy                                      OpenViBE::CIdentifier(0xFD3CB444, 0x58F00765)
#define OVTK_ClassId_Algorithm_PairingStrategyDesc                                  OpenViBE::CIdentifier(0x4341B8D6, 0xC65B7BBB)

#define OVTK_Algorithm_PairingStrategy_InputParameterId_SubClassifierAlgorithm      OpenViBE::CIdentifier(0xD9E60DF9, 0x20EC8FC9)

#define OVTK_Algorithm_PairingStrategy_InputTriggerId_DesignArchitecture            OpenViBE::CIdentifier(0x784A9CDF, 0xA41C27F8)


typedef int32_t (*fClassifierComparison)(OpenViBE::IMatrix&, OpenViBE::IMatrix&);

namespace OpenViBEToolkit
{
	extern OVTK_API void registerClassificationComparisonFunction(const OpenViBE::CIdentifier& rClassIdentifier, fClassifierComparison pComparision);
	extern OVTK_API fClassifierComparison getClassificationComparisonFunction(const OpenViBE::CIdentifier& rClassIdentifier);


	class OVTK_API CAlgorithmPairingStrategy : public CAlgorithmClassifier
	{
	public:

		virtual bool process(void);
		virtual void release(void) { delete this; }

		virtual bool designArchitecture(const OpenViBE::CIdentifier& rId, uint32_t rClassCount) = 0;

		virtual bool train(const OpenViBEToolkit::IFeatureVectorSet& rFeatureVectorSet) = 0;
		virtual bool classify(const OpenViBEToolkit::IFeatureVector& rFeatureVector
										   , double& rf64Class
										   , OpenViBEToolkit::IVector& rDistanceValue
										   , OpenViBEToolkit::IVector& rProbabilityValue) = 0;

		virtual XML::IXMLNode* saveConfiguration(void) = 0;
		virtual bool loadConfiguration(XML::IXMLNode* pConfiguratioNode) = 0;
		_IsDerivedFromClass_(CAlgorithmClassifier, OVTK_ClassId_Algorithm_PairingStrategy)

		virtual uint32_t getOutputProbabilityVectorLength(void) = 0;
		virtual uint32_t getOutputDistanceVectorLength(void) = 0;


	protected:
		//  std::vector <double> m_fClasses;
		//The vector will be use when the user will be able to specify class label
		OpenViBE::CIdentifier m_oSubClassifierAlgorithmIdentifier;
	};

	class OVTK_API CAlgorithmPairingStrategyDesc : public OpenViBEToolkit::CAlgorithmClassifierDesc
	{
	public:

		virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
		{
			CAlgorithmClassifierDesc::getAlgorithmPrototype(rAlgorithmPrototype);

			rAlgorithmPrototype.addInputParameter(OVTK_Algorithm_PairingStrategy_InputParameterId_SubClassifierAlgorithm, "Algorithm Identifier", OpenViBE::Kernel::ParameterType_Identifier);

			rAlgorithmPrototype.addInputTrigger(OVTK_Algorithm_PairingStrategy_InputTriggerId_DesignArchitecture, "Design Architecture");
			return true;
		}

		_IsDerivedFromClass_(OpenViBEToolkit::CAlgorithmClassifierDesc, OVTK_ClassId_Algorithm_PairingStrategyDesc)
	};
}

#endif // OVTKCALGORITHMPAIRINGSTRATEGY_H
