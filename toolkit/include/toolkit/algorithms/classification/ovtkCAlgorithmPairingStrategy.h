#pragma once

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


typedef int (*fClassifierComparison)(OpenViBE::IMatrix&, OpenViBE::IMatrix&);

namespace OpenViBEToolkit
{
	extern OVTK_API void registerClassificationComparisonFunction(const OpenViBE::CIdentifier& classID, fClassifierComparison comparision);
	extern OVTK_API fClassifierComparison getClassificationComparisonFunction(const OpenViBE::CIdentifier& classID);


	class OVTK_API CAlgorithmPairingStrategy : public CAlgorithmClassifier
	{
	public:
		bool process() override;
		void release() override { delete this; }

		virtual bool designArchitecture(const OpenViBE::CIdentifier& rId, uint32_t rClassCount) = 0;
		bool train(const IFeatureVectorSet& rFeatureVectorSet) override = 0;
		bool classify(const IFeatureVector& rFeatureVector
					  , double& rf64Class
					  , IVector& rDistanceValue
					  , IVector& rProbabilityValue) override = 0;
		XML::IXMLNode* saveConfiguration() override = 0;
		bool loadConfiguration(XML::IXMLNode* pConfiguratioNode) override = 0;
		_IsDerivedFromClass_(CAlgorithmClassifier, OVTK_ClassId_Algorithm_PairingStrategy)
		uint32_t getOutputProbabilityVectorLength() override = 0;
		uint32_t getOutputDistanceVectorLength() override = 0;


	protected:
		//  std::vector <double> m_fClasses;
		//The vector will be use when the user will be able to specify class label
		OpenViBE::CIdentifier m_subClassifierAlgorithmID = OV_UndefinedIdentifier;
	};

	class OVTK_API CAlgorithmPairingStrategyDesc : public CAlgorithmClassifierDesc
	{
	public:
		bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
		{
			CAlgorithmClassifierDesc::getAlgorithmPrototype(rAlgorithmPrototype);

			rAlgorithmPrototype.addInputParameter(
				OVTK_Algorithm_PairingStrategy_InputParameterId_SubClassifierAlgorithm, "Algorithm Identifier", OpenViBE::Kernel::ParameterType_Identifier);

			rAlgorithmPrototype.addInputTrigger(OVTK_Algorithm_PairingStrategy_InputTriggerId_DesignArchitecture, "Design Architecture");
			return true;
		}

		_IsDerivedFromClass_(OpenViBEToolkit::CAlgorithmClassifierDesc, OVTK_ClassId_Algorithm_PairingStrategyDesc)
	};
} // namespace OpenViBEToolkit
