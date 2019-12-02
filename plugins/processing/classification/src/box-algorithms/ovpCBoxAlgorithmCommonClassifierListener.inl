#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>
#include <cstdio>
#include <iostream>

//#define OV_DEBUG_CLASSIFIER_LISTENER

#ifdef OV_DEBUG_CLASSIFIER_LISTENER
#define DEBUG_PRINT(x) x
#else
#define DEBUG_PRINT(x)
#endif

namespace OpenViBEPlugins
{
	namespace Classification
	{
		class CBoxAlgorithmCommonClassifierListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			explicit CBoxAlgorithmCommonClassifierListener(const uint32_t customSettingBase)
				: m_classifierClassID(OV_UndefinedIdentifier),
				  m_strategyClassID(
					  0x0),//OV_UndefinedIdentifier is already use for the native, We initialize to an unused identifier in the strategy list
				  m_customSettingBase(customSettingBase) { }

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

			bool initializedStrategy(OpenViBE::Kernel::IBox& box)
			{
				OpenViBE::CString strategyName;
				box.getSettingName(getStrategyIndex() + 1, strategyName);
				if (strategyName == OpenViBE::CString(PAIRWISE_STRATEGY_ENUMERATION_NAME)) { m_strategyAmountSettings = 1; }
				else { m_strategyAmountSettings = 0; }
				return true;
			}

			//			virtual bool onAlgorithmClassIdentifierChanged(OpenViBE::Kernel::IBox &box)
			//			{
			//				this->initializedStrategy(box);
			//				return true;
			//			}

			int getStrategySettingsCount(OpenViBE::Kernel::IBox& box)
			{
				if (m_strategyAmountSettings < 0) { initializedStrategy(box); }	//The value have never been initialized
				return m_strategyAmountSettings;
			}

			static bool onInputAddedOrRemoved(OpenViBE::Kernel::IBox& box)
			{
				box.setInputType(0, OV_TypeId_Stimulations);
				box.setInputName(0, "Stimulations");
				for (uint32_t i = 1; i < box.getInputCount(); ++i)
				{
					box.setInputName(i, ("Features for class " + std::to_string(i)).c_str());
					box.setInputType(i, OV_TypeId_FeatureVector);
				}
				return true;
			}

			bool onInputAdded(OpenViBE::Kernel::IBox& box, const size_t index) override
			{
				//index represent the number of the class (because of rejected offset)
				const std::string buffer = "Class " + std::to_string(index) + " label";
				char stimulation[64];
				sprintf(stimulation, "OVTK_StimulationId_Label_%02X", index);
				box.addSetting(buffer.c_str(), OV_TypeId_Stimulation, stimulation, 3 - 1 + getStrategySettingsCount(box) + index);

				//Rename input
				return onInputAddedOrRemoved(box);
			}

			bool onInputRemoved(OpenViBE::Kernel::IBox& box, const size_t index) override
			{
				//First remove the removed input from settings
				box.removeSetting(3 - 1 + getStrategySettingsCount(box) + index);

				//Then rename the remains inputs in settings
				for (uint32_t i = 1; i < box.getInputCount(); ++i)
				{
					const std::string buffer = "Class " + std::to_string(i) + " label";
					box.setSettingName(3 - 1 + getStrategySettingsCount(box) + i, buffer.c_str());
				}

				//Then rename input
				return this->onInputAddedOrRemoved(box);
			}

			bool onInitialized(OpenViBE::Kernel::IBox& box) override
			{
				//We need to know if the box is already initialized (can be called after a restore state)
				OpenViBE::CString strategyName;
				box.getSettingName(getStrategyIndex() + 2, strategyName);//this one is a class label
				const std::string settingName(strategyName.toASCIIString());

				if (settingName.find("Class ") == std::string::npos)//We haven't initialized the box so let's do it
				{
					//Now added Settings for classes
					for (uint32_t i = 1; i < box.getInputCount(); ++i)
					{
						const std::string buffer = "Class " + std::to_string(i) + " label";
						char stimulation[64];
						sprintf(stimulation, "OVTK_StimulationId_Label_%02X", i);
						box.addSetting(buffer.c_str(), OV_TypeId_Stimulation, stimulation, 3 - 1 + getStrategySettingsCount(box) + i);
						DEBUG_PRINT(std::cout << "Add setting (type D) " << buffer.c_str() << " " << stimulation << "\n";)
					}
					return this->onAlgorithmClassifierChanged(box);
				}
				return true;
				//return this->onAlgorithmClassifierChanged(box);
			}

			//Return the index of the combo box used to select the strategy (native/ OnevsOne...)
			static uint32_t getStrategyIndex() { return 2; }

			//Return the index of the combo box used to select the classification algorithm
			uint32_t getClassifierIndex(OpenViBE::Kernel::IBox& box) { return getStrategySettingsCount(box) + 3 + box.getInputCount() - 1; }

			bool onSettingValueChanged(OpenViBE::Kernel::IBox& box, const size_t index) override
			{
				if (index == getClassifierIndex(box)) { return this->onAlgorithmClassifierChanged(box); }
				if (index == getStrategyIndex()) { return this->onStrategyChanged(box); }
				return true;
			}


			bool updateDecision(OpenViBE::Kernel::IBox& box)
			{
				const uint32_t i = getStrategyIndex() + 1;
				if (m_strategyClassID == OVP_ClassId_Algorithm_ClassifierOneVsOne)
				{
					OpenViBE::CString classifierName = "Unknown";
					box.getSettingValue(getClassifierIndex(box), classifierName);
					const OpenViBE::CIdentifier entry = this->getTypeManager().getEnumerationEntryValueFromName(
						OVP_TypeId_OneVsOne_DecisionAlgorithms, classifierName);

					OV_ERROR_UNLESS_KRF(entry != OV_UndefinedIdentifier,
										"Unable to find Pairwise Decision for the algorithm [" << m_classifierClassID.toString() << "] (" <<
										classifierName.toASCIIString() << ")",
										OpenViBE::Kernel::ErrorType::BadConfig);

					OpenViBE::Kernel::IParameter* param = m_strategy->getInputParameter(OVP_Algorithm_OneVsOneStrategy_InputParameterId_DecisionType);
					OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64Parameter(param);

					const OpenViBE::CString enumTypeEntry = this->getTypeManager().getTypeName(entry);
					uint64_t value                        = ip_ui64Parameter;
					uint64_t enumIdx;
					OpenViBE::CString enumValue;

					box.getSettingValue(i, enumValue);

					const uint64_t oldId = this->getTypeManager().getEnumerationEntryValueFromName(entry, enumValue);
					if (oldId == OV_UndefinedIdentifier
					) { enumIdx = 0; }	//The previous strategy does not exists in the new enum, let's switch to the default value (the first)
					else { enumIdx = oldId; }

					this->getTypeManager().getEnumerationEntry(entry, enumIdx, enumValue, value);
					ip_ui64Parameter = value;

					box.setSettingType(i, entry);
					box.setSettingName(i, enumTypeEntry);
					box.setSettingValue(i, enumValue);
				}
				return true;
			}

			bool onStrategyChanged(OpenViBE::Kernel::IBox& box)
			{
				OpenViBE::CString strategyName;

				box.getSettingValue(getStrategyIndex(), strategyName);

				const OpenViBE::CIdentifier strategyId = this->getTypeManager().getEnumerationEntryValueFromName(
					OVTK_TypeId_ClassificationStrategy, strategyName);
				if (strategyId != m_strategyClassID)
				{
					if (m_strategy)
					{
						m_strategy->uninitialize();
						this->getAlgorithmManager().releaseAlgorithm(*m_strategy);
						m_strategy        = nullptr;
						m_strategyClassID = OV_UndefinedIdentifier;
					}
					if (strategyId != OV_UndefinedIdentifier)
					{
						m_strategy = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(strategyId));
						m_strategy->initialize();
						m_strategyClassID = strategyId;
					}

					for (uint32_t i = getStrategyIndex() + getStrategySettingsCount(box); i > getStrategyIndex(); --i)
					{
						DEBUG_PRINT(std::cout << "Remove pairing strategy setting at idx " << i-1 << "\n";)
						box.removeSetting(i);
					}
					m_strategyAmountSettings = 0;
				}
				else { return true; }	//If we don't change the strategy we just have to return

				if (m_strategy)
				{
					OpenViBE::CString classifierName;
					box.getSettingValue(getClassifierIndex(box), classifierName);
					const uint32_t i = getStrategyIndex() + 1;
					if (m_strategyClassID == OVP_ClassId_Algorithm_ClassifierOneVsOne)
					{
						const OpenViBE::CIdentifier entry = this->getTypeManager().getEnumerationEntryValueFromName(
							OVP_TypeId_OneVsOne_DecisionAlgorithms, classifierName);
						OV_ERROR_UNLESS_KRF(entry != OV_UndefinedIdentifier,
											"Unable to find Pairwise Decision for the algorithm [" << m_classifierClassID.toString() << "]",
											OpenViBE::Kernel::ErrorType::BadConfig);

						//As we just switch to this strategy, we take the default value set in the strategy to initialize the value
						OpenViBE::Kernel::IParameter* parameter = m_strategy->getInputParameter(OVP_Algorithm_OneVsOneStrategy_InputParameterId_DecisionType);
						const OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64Parameter(parameter);
						const uint64_t enumValue         = ip_ui64Parameter;
						const OpenViBE::CString enumName = this->getTypeManager().getEnumerationEntryNameFromValue(entry, enumValue);

						const OpenViBE::CString parameterName = this->getTypeManager().getTypeName(entry);

						DEBUG_PRINT(std::cout << "Adding setting (case C) " << parameterName << " : '" << enumName << "' to index " << i << "\n";)
						box.addSetting(parameterName, entry, enumName, i);

						m_strategyAmountSettings = 1;
					}
				}

				return true;
			}

			bool onAlgorithmClassifierChanged(OpenViBE::Kernel::IBox& box)
			{
				OpenViBE::CString classifierName;
				OpenViBE::CIdentifier id;

				box.getSettingValue(getClassifierIndex(box), classifierName);
				const OpenViBE::CIdentifier classifierId = this->getTypeManager().getEnumerationEntryValueFromName(
					OVTK_TypeId_ClassificationAlgorithm, classifierName);
				if (classifierId != m_classifierClassID)
				{
					if (m_classifier)
					{
						m_classifier->uninitialize();
						this->getAlgorithmManager().releaseAlgorithm(*m_classifier);
						m_classifier        = nullptr;
						m_classifierClassID = OV_UndefinedIdentifier;
					}
					if (classifierId != OV_UndefinedIdentifier)
					{
						m_classifier = &this->getAlgorithmManager().getAlgorithm(this->getAlgorithmManager().createAlgorithm(classifierId));
						m_classifier->initialize();
						m_classifierClassID = classifierId;
					}

					//Disable the graphical refresh to avoid abusive redraw (not really a problem)
					while (box.getSettingCount() >= m_customSettingBase + box.getInputCount() + getStrategySettingsCount(box)) { box.removeSetting(getClassifierIndex(box) + 1); }
				}
				else { return true; }//If we don't change the algorithm we just have to return

				if (m_classifier)
				{
					uint32_t i = getClassifierIndex(box) + 1;
					while ((id = m_classifier->getNextInputParameterIdentifier(id)) != OV_UndefinedIdentifier)
					{
						if ((id != OVTK_Algorithm_Classifier_InputParameterId_FeatureVector)
							&& (id != OVTK_Algorithm_Classifier_InputParameterId_FeatureVectorSet)
							&& (id != OVTK_Algorithm_Classifier_InputParameterId_Config)
							&& (id != OVTK_Algorithm_Classifier_InputParameterId_NClasses)
							&& (id != OVTK_Algorithm_Classifier_InputParameterId_ExtraParameter))
						{
							OpenViBE::CIdentifier typeID;
							OpenViBE::CString paramName         = m_classifier->getInputParameterName(id);
							OpenViBE::Kernel::IParameter* param = m_classifier->getInputParameter(id);
							OpenViBE::Kernel::TParameterHandler<int64_t> ip_i64Parameter(param);
							OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64Parameter(param);
							OpenViBE::Kernel::TParameterHandler<double> ip_f64Parameter(param);
							OpenViBE::Kernel::TParameterHandler<bool> ip_bParameter(param);
							OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> ip_sParameter(param);
							std::string buffer;
							bool valid = true;
							switch (param->getType())
							{
								case OpenViBE::Kernel::ParameterType_Enumeration:
									buffer = this->getTypeManager().getEnumerationEntryNameFromValue(param->getSubTypeIdentifier(), ip_ui64Parameter).toASCIIString();
									typeID = param->getSubTypeIdentifier();
									break;

								case OpenViBE::Kernel::ParameterType_Integer:
								case OpenViBE::Kernel::ParameterType_UInteger:
									buffer = std::to_string(int64_t(ip_i64Parameter));
									typeID = OV_TypeId_Integer;
									break;

								case OpenViBE::Kernel::ParameterType_Boolean:
									buffer = (bool(ip_bParameter)) ? "true" : "false";
									typeID = OV_TypeId_Boolean;
									break;

								case OpenViBE::Kernel::ParameterType_Float:
									buffer = std::to_string(double(ip_i64Parameter));
									typeID = OV_TypeId_Float;
									break;
								case OpenViBE::Kernel::ParameterType_String:
									buffer = static_cast<OpenViBE::CString*>(ip_sParameter)->toASCIIString();
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
								DEBUG_PRINT(std::cout << "Adding setting (case A) " << paramName << " : " << value.c_str() << " to slot " << box.getSettingCount()-2 << "\n";)
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

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)

		protected:

			OpenViBE::CIdentifier m_classifierClassID       = OV_UndefinedIdentifier;
			OpenViBE::CIdentifier m_strategyClassID         = OV_UndefinedIdentifier;
			OpenViBE::Kernel::IAlgorithmProxy* m_classifier = nullptr;
			OpenViBE::Kernel::IAlgorithmProxy* m_strategy   = nullptr;
			const uint32_t m_customSettingBase              = 0;
			int m_strategyAmountSettings                    = -1;
		};
	} // namespace Classification
} // namespace OpenViBEPlugins
