#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <string>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmChannelSelector final : public OpenViBE::Toolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const size_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ChannelSelector)

		protected:

			OpenViBE::Toolkit::TDecoder<CBoxAlgorithmChannelSelector>* m_decoder = nullptr;
			OpenViBE::Toolkit::TEncoder<CBoxAlgorithmChannelSelector>* m_encoder = nullptr;

			OpenViBE::IMatrix* m_iMatrix = nullptr;
			OpenViBE::IMatrix* m_oMatrix = nullptr;

			std::vector<size_t> m_vLookup;
		};

		class CBoxAlgorithmChannelSelectorListener final : public OpenViBE::Toolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onOutputTypeChanged(OpenViBE::Kernel::IBox& box, const size_t /*index*/) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getOutputType(0, typeID);
				if (typeID == OV_TypeId_Signal || typeID == OV_TypeId_Spectrum || typeID == OV_TypeId_StreamedMatrix)
				{
					box.setInputType(0, typeID);
					return true;
				}
				box.getInputType(0, typeID);
				box.setOutputType(0, typeID);
				OV_ERROR_KRF("Invalid output type [" << typeID.str() << "] (expected Signal, Spectrum or Streamed Matrix)",
							 OpenViBE::Kernel::ErrorType::BadOutput);
			}

			bool onInputTypeChanged(OpenViBE::Kernel::IBox& box, const size_t /*index*/) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getInputType(0, typeID);
				if (typeID == OV_TypeId_Signal || typeID == OV_TypeId_Spectrum
					|| typeID == OV_TypeId_StreamedMatrix)
				{
					box.setOutputType(0, typeID);
					return true;
				}
				box.getOutputType(0, typeID);
				box.setInputType(0, typeID);

				OV_ERROR_KRF("Invalid input type [" << typeID.str() << "] (expected Signal, Spectrum or Streamed Matrix)",
							 OpenViBE::Kernel::ErrorType::BadInput);
			}

			bool onSettingValueChanged(OpenViBE::Kernel::IBox& box, const size_t index) override
			{
				//we are only interested in the setting 0 and the type changes (select or reject)
				if ((index == 0 || index == 1) && (!m_hasUserSetName))
				{
					OpenViBE::CString channels;
					box.getSettingValue(0, channels);

					OpenViBE::CString method;
					OpenViBE::CIdentifier enumID = OV_UndefinedIdentifier;
					box.getSettingValue(1, method);
					box.getSettingType(1, enumID);

					const OpenViBE::CIdentifier methodID = this->getTypeManager().getEnumerationEntryValueFromName(enumID, method);

					if (methodID == Reject) { channels = OpenViBE::CString("!") + channels; }
					box.setName(channels);
				}
				return true;
			}

			bool onNameChanged(OpenViBE::Kernel::IBox& box) override
			//when user set box name manually
			{
				if (m_hasUserSetName)
				{
					const OpenViBE::CString rename = box.getName();
					if (rename == OpenViBE::CString("Channel Selector"))
					{//default name, we switch back to default behaviour
						m_hasUserSetName = false;
					}
				}
				else { m_hasUserSetName = true; }
				return true;
			}

			bool initialize() override
			{
				m_hasUserSetName = false;//need to initialize this value
				return true;
			}

		private:
			bool m_hasUserSetName = false;

			_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmChannelSelectorDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Channel Selector"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Select a subset of signal channels"); }

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString("Selection can be based on channel name (case-sensitive) or index starting from 0");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Channels"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_ChannelSelector; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmChannelSelector; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmChannelSelectorListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input signal", OV_TypeId_Signal);
				prototype.addOutput("Output signal", OV_TypeId_Signal);
				prototype.addSetting("Channel List", OV_TypeId_String, ":");
				prototype.addSetting("Action", OVP_TypeId_SelectionMethod, "Select");
				prototype.addSetting("Channel Matching Method", OVP_TypeId_MatchMethod, "Smart");

				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);

				prototype.addInputSupport(OV_TypeId_Signal);
				prototype.addInputSupport(OV_TypeId_Spectrum);
				prototype.addInputSupport(OV_TypeId_StreamedMatrix);

				prototype.addOutputSupport(OV_TypeId_Signal);
				prototype.addOutputSupport(OV_TypeId_Spectrum);
				prototype.addOutputSupport(OV_TypeId_StreamedMatrix);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ChannelSelectorDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins
