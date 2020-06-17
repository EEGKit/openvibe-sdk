#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.hpp>
#include <toolkit/ovtk_all.h>
#include <cstdio>
#include <iostream>
#include <iomanip>

//#define OV_DEBUG_CLASSIFIER_LISTENER

#ifdef OV_DEBUG_CLASSIFIER_LISTENER
#define DEBUG_PRINT(x) x
#else
#define DEBUG_PRINT(x)
#endif

namespace OpenViBE {
namespace Plugins {
namespace Classification {
class CBoxAlgorithmCommonClassifierListener final : public Toolkit::TBoxListener<IBoxListener>
{
public:

	explicit CBoxAlgorithmCommonClassifierListener(const size_t customSettingBase) : m_customSettingBase(customSettingBase) { }

	bool initialize() override
	{
		//Even if everything should have been set in constructor, we still set everything in initialize (in case of)
		m_classifierClassID = OV_UndefinedIdentifier;
		m_classifier        = nullptr;

		//OV_UndefinedIdentifier is already use for the native, We initialize to an unused identifier in the strategy list
		m_strategyClassID = 0x0;
		m_strategy        = nullptr;

		//This value means that we need to calculate it
		m_strategyAmountSettings = -1;
		return true;
	}

	bool uninitialize() override
	{
		if (m_classifier)
		{
			m_classifier->uninitialize();
			this->getAlgorithmManager().releaseAlgorithm(*m_classifier);
			m_classifier = nullptr;
		}
		if (m_strategy)
		{
			m_strategy->uninitialize();
			this->getAlgorithmManager().releaseAlgorithm(*m_strategy);
			m_strategy = nullptr;
		}
		return true;
	}

	bool initializedStrategy(Kernel::IBox& box)
	{
		CString name;
		box.getSettingName(getStrategyIndex() + 1, name);
		if (name == CString(PAIRWISE_STRATEGY_ENUMERATION_NAME)) { m_strategyAmountSettings = 1; }
		else { m_strategyAmountSettings = 0; }
		return true;
	}

	//virtual bool onAlgorithmClassIdentifierChanged(Kernel::IBox &box)
	//{
	//this->initializedStrategy(box);
	//return true;
	//}

	int getStrategySettingsCount(Kernel::IBox& box)
	{
		if (m_strategyAmountSettings < 0) { initializedStrategy(box); }	//The value have never been initialized
		return m_strategyAmountSettings;
	}

	static bool onInputAddedOrRemoved(Kernel::IBox& box)
	{
		box.setInputType(0, OV_TypeId_Stimulations);
		box.setInputName(0, "Stimulations");
		for (size_t i = 1; i < box.getInputCount(); ++i)
		{
			box.setInputName(i, ("Features for class " + std::to_string(i)).c_str());
			box.setInputType(i, OV_TypeId_FeatureVector);
		}
		return true;
	}

	bool onInputAdded(Kernel::IBox& box, const size_t index) override
	{
		//index represent the number of the class (because of rejected offset)
		const std::string name = "Class " + std::to_string(index) + " label";
		std::stringstream stim;
		stim.fill('0');
		stim << "OVTK_StimulationId_Label_" << std::setw(2) << index;
		box.addSetting(name.c_str(), OV_TypeId_Stimulation, stim.str().c_str(), 3 - 1 + getStrategySettingsCount(box) + index);

		//Rename input
		return onInputAddedOrRemoved(box);
	}

	bool onInputRemoved(Kernel::IBox& box, const size_t index) override
	{
		//First remove the removed input from settings
		box.removeSetting(3 - 1 + getStrategySettingsCount(box) + index);

		//Then rename the remains inputs in settings
		for (size_t i = 1; i < box.getInputCount(); ++i)
		{
			const std::string name = "Class " + std::to_string(i) + " label";
			box.setSettingName(3 - 1 + getStrategySettingsCount(box) + i, name.c_str());
		}

		//Then rename input
		return onInputAddedOrRemoved(box);
	}

	bool onInitialized(Kernel::IBox& box) override
	{
		//We need to know if the box is already initialized (can be called after a restore state)
		CString strategyName;
		box.getSettingName(getStrategyIndex() + 2, strategyName);//this one is a class label
		const std::string settingName(strategyName.toASCIIString());

		if (settingName.find("Class ") == std::string::npos)//We haven't initialized the box so let's do it
		{
			//Now added Settings for classes
			for (size_t i = 1; i < box.getInputCount(); ++i)
			{
				const std::string name = "Class " + std::to_string(i) + " label";
				std::stringstream stim;
				stim.fill('0');
				stim << "OVTK_StimulationId_Label_" << std::setw(2) << i;
				box.addSetting(name.c_str(), OV_TypeId_Stimulation, stim.str().c_str(), 3 - 1 + getStrategySettingsCount(box) + i);
				DEBUG_PRINT(std::cout << "Add setting (type D) " << buffer << " " << stimulation << "\n";)
			}
			return this->onAlgorithmClassifierChanged(box);
		}
		return true;
		//return this->onAlgorithmClassifierChanged(box);
	}

	//Return the index of the combo box used to select the strategy (native/ OnevsOne...)
	static size_t getStrategyIndex() { return 2; }

	//Return the index of the combo box used to select the classification algorithm
	size_t getClassifierIndex(Kernel::IBox& box) { return getStrategySettingsCount(box) + 3 + box.getInputCount() - 1; }

	bool onSettingValueChanged(Kernel::IBox& box, const size_t index) override
	{
		if (index == getClassifierIndex(box)) { return this->onAlgorithmClassifierChanged(box); }
		if (index == getStrategyIndex()) { return this->onStrategyChanged(box); }
		return true;
	}


	bool updateDecision(Kernel::IBox& box)
	{
		const size_t i = getStrategyIndex() + 1;
		if (m_strategyClassID == OVP_ClassId_Algorithm_ClassifierOneVsOne)
		{
			CString classifierName = "Unknown";
			box.getSettingValue(getClassifierIndex(box), classifierName);
			const CIdentifier typeID = this->getTypeManager().getEnumerationEntryValueFromName(
				OVP_TypeId_OneVsOne_DecisionAlgorithms, classifierName);

			OV_ERROR_UNLESS_KRF(typeID != OV_UndefinedIdentifier,
								"Unable to find Pairwise Decision for the algorithm [" << m_classifierClassID.str() << "] (" << classifierName <<
								")",
								Kernel::ErrorType::BadConfig);

			Kernel::IParameter* param = m_strategy->getInputParameter(OVP_Algorithm_OneVsOneStrategy_InputParameterId_DecisionType);
			Kernel::TParameterHandler<uint64_t> ip_parameter(param);

			const CString entry = this->getTypeManager().getTypeName(typeID);
			uint64_t value      = ip_parameter;
			uint64_t idx;
			CString name;

			box.getSettingValue(i, name);

			const uint64_t oldID = this->getTypeManager().getEnumerationEntryValueFromName(typeID, name);
			if (oldID == OV_UndefinedIdentifier
			) { idx = 0; }	//The previous strategy does not exists in the new enum, let's switch to the default value (the first)
			else { idx = oldID; }

			this->getTypeManager().getEnumerationEntry(typeID, idx, name, value);
			ip_parameter = value;

			box.setSettingType(i, typeID);
			box.setSettingName(i, entry);
			box.setSettingValue(i, name);
		}
		return true;
	}

	bool onStrategyChanged(Kernel::IBox& box)
	{
		CString name;

		box.getSettingValue(getStrategyIndex(), name);

		const CIdentifier id = this->getTypeManager().getEnumerationEntryValueFromName(
			OVTK_TypeId_ClassificationStrategy, name);
		if (id != m_strategyClassID)
		{
			if (m_strategy)
			{
				m_strategy->uninitialize();
				this->getAlgorithmManager().releaseAlgorithm(*m_strategy);
				m_strategy        = nullptr;
				m_strategyClassID = OV_UndefinedIdentifier;
			}
			if (id != OV_UndefinedIdentifier)
			{
				m_strategy = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(id));
				m_strategy->initialize();
				m_strategyClassID = id;
			}

			for (size_t i = getStrategyIndex() + getStrategySettingsCount(box); i > getStrategyIndex(); --i)
			{
				DEBUG_PRINT(std::cout << "Remove pairing strategy setting at idx " << i-1 << "\n";)
				box.removeSetting(i);
			}
			m_strategyAmountSettings = 0;
		}
		else { return true; }	//If we don't change the strategy we just have to return

		if (m_strategy)
		{
			box.getSettingValue(getClassifierIndex(box), name);
			const size_t i = getStrategyIndex() + 1;
			if (m_strategyClassID == OVP_ClassId_Algorithm_ClassifierOneVsOne)
			{
				const CIdentifier typeID = this->getTypeManager().getEnumerationEntryValueFromName(
					OVP_TypeId_OneVsOne_DecisionAlgorithms, name);
				OV_ERROR_UNLESS_KRF(typeID != OV_UndefinedIdentifier,
									"Unable to find Pairwise Decision for the algorithm [" << m_classifierClassID.str() << "]",
									Kernel::ErrorType::BadConfig);

				//As we just switch to this strategy, we take the default value set in the strategy to initialize the value
				Kernel::IParameter* param = m_strategy->getInputParameter(OVP_Algorithm_OneVsOneStrategy_InputParameterId_DecisionType);
				const Kernel::TParameterHandler<uint64_t> ip_param(param);
				const uint64_t value = ip_param;
				name                 = this->getTypeManager().getEnumerationEntryNameFromValue(typeID, value);

				const CString paramName = this->getTypeManager().getTypeName(typeID);

				DEBUG_PRINT(std::cout << "Adding setting (case C) " << paramName << " : '" << name << "' to index " << i << "\n";)
				box.addSetting(paramName, typeID, name, i);

				m_strategyAmountSettings = 1;
			}
		}

		return true;
	}

	bool onAlgorithmClassifierChanged(Kernel::IBox& box)
	{
		CString name;
		box.getSettingValue(getClassifierIndex(box), name);
		CIdentifier id = this->getTypeManager().getEnumerationEntryValueFromName(OVTK_TypeId_ClassificationAlgorithm, name);
		if (id != m_classifierClassID)
		{
			if (m_classifier)
			{
				m_classifier->uninitialize();
				this->getAlgorithmManager().releaseAlgorithm(*m_classifier);
				m_classifier        = nullptr;
				m_classifierClassID = OV_UndefinedIdentifier;
			}
			if (id != OV_UndefinedIdentifier)
			{
				m_classifier = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(id));
				m_classifier->initialize();
				m_classifierClassID = id;
			}

			//Disable the graphical refresh to avoid abusive redraw (not really a problem)
			while (box.getSettingCount() >= m_customSettingBase + box.getInputCount() + getStrategySettingsCount(box))
			{
				box.removeSetting(getClassifierIndex(box) + 1);
			}
		}
		else { return true; }//If we don't change the algorithm we just have to return

		if (m_classifier)
		{
			size_t i = getClassifierIndex(box) + 1;
			while ((id = m_classifier->getNextInputParameterIdentifier(id)) != OV_UndefinedIdentifier)
			{
				if ((id != OVTK_Algorithm_Classifier_InputParameterId_FeatureVector)
					&& (id != OVTK_Algorithm_Classifier_InputParameterId_FeatureVectorSet)
					&& (id != OVTK_Algorithm_Classifier_InputParameterId_Config)
					&& (id != OVTK_Algorithm_Classifier_InputParameterId_NClasses)
					&& (id != OVTK_Algorithm_Classifier_InputParameterId_ExtraParameter))
				{
					CIdentifier typeID;
					CString paramName         = m_classifier->getInputParameterName(id);
					Kernel::IParameter* param = m_classifier->getInputParameter(id);
					Kernel::TParameterHandler<int64_t> ip_iParameter(param);
					Kernel::TParameterHandler<uint64_t> ip_uiParameter(param);
					Kernel::TParameterHandler<double> ip_dParameter(param);
					Kernel::TParameterHandler<bool> ip_bParameter(param);
					Kernel::TParameterHandler<CString*> ip_sParameter(param);
					std::string buffer;
					bool valid = true;
					switch (param->getType())
					{
						case Kernel::ParameterType_Enumeration:
							buffer = this->getTypeManager().getEnumerationEntryNameFromValue(param->getSubTypeIdentifier(), ip_uiParameter).
										   toASCIIString();
							typeID = param->getSubTypeIdentifier();
							break;

						case Kernel::ParameterType_Integer:
						case Kernel::ParameterType_UInteger:
							buffer = std::to_string(int64_t(ip_iParameter));
							typeID = OV_TypeId_Integer;
							break;

						case Kernel::ParameterType_Boolean:
							buffer = (bool(ip_bParameter)) ? "true" : "false";
							typeID = OV_TypeId_Boolean;
							break;

						case Kernel::ParameterType_Float:
							buffer = std::to_string(double(ip_dParameter));
							typeID = OV_TypeId_Float;
							break;
						case Kernel::ParameterType_String:
							buffer = static_cast<CString*>(ip_sParameter)->toASCIIString();
							typeID = OV_TypeId_String;
							break;
						default:
							std::cout << "Invalid parameter type " << param->getType() << "\n";
							valid = false;
							break;
					}

					if (valid)
					{
						// @FIXME argh, the -2 is a hard coding that the classifier trainer has 2 settings after the classifier setting... ouch
						DEBUG_PRINT(std::cout << "Adding setting (case A) " << paramName << " : " << buffer << " to slot "
							<< box.getSettingCount() - 2 << "\n";)
						box.addSetting(paramName, typeID, buffer.c_str(), box.getSettingCount() - 2);
						i++;
					}
				}
			}
		}
		// This changes the pairwise strategy decision voting type of the box settings allowing
		// designer to list the correct choices in the combo box.
		updateDecision(box);
		return true;
	}

	_IsDerivedFromClass_Final_(Toolkit::TBoxListener<IBoxListener>, OV_UndefinedIdentifier)

protected:

	CIdentifier m_classifierClassID = OV_UndefinedIdentifier;
	CIdentifier m_strategyClassID   =
			0x0;	// OV_UndefinedIdentifier is already use, We initialize to an unused identifier in the strategy list
	Kernel::IAlgorithmProxy* m_classifier = nullptr;
	Kernel::IAlgorithmProxy* m_strategy   = nullptr;
	const size_t m_customSettingBase      = 0;
	int m_strategyAmountSettings          = -1;
};
}  // namespace Classification
}  // namespace Plugins
}  // namespace OpenViBE
