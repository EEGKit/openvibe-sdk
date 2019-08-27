#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <string>

#define OVP_ClassId_BoxAlgorithm_ChannelSelector     OpenViBE::CIdentifier(0x361722E8, 0x311574E8)
#define OVP_ClassId_BoxAlgorithm_ChannelSelectorDesc OpenViBE::CIdentifier(0x67633C1C, 0x0D610CD8)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmChannelSelector : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ChannelSelector)

		protected:

			OpenViBEToolkit::TDecoder<CBoxAlgorithmChannelSelector>* m_pDecoder = nullptr;
			OpenViBEToolkit::TEncoder<CBoxAlgorithmChannelSelector>* m_pEncoder = nullptr;

			OpenViBE::IMatrix* m_pInputMatrix  = nullptr;
			OpenViBE::IMatrix* m_pOutputMatrix = nullptr;

			std::vector<uint32_t> m_vLookup;
		};

		class CBoxAlgorithmChannelSelectorListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t index) override
			{
				OpenViBE::CIdentifier l_oTypeIdentifier = OV_UndefinedIdentifier;
				rBox.getOutputType(0, l_oTypeIdentifier);
				if (l_oTypeIdentifier == OV_TypeId_Signal || l_oTypeIdentifier == OV_TypeId_Spectrum || l_oTypeIdentifier == OV_TypeId_StreamedMatrix)
				{
					rBox.setInputType(0, l_oTypeIdentifier);
					return true;
				}
				rBox.getInputType(0, l_oTypeIdentifier);
				rBox.setOutputType(0, l_oTypeIdentifier);

				OV_ERROR_KRF("Invalid output type [" << l_oTypeIdentifier.toString() << "] (expected Signal, Spectrum or Streamed Matrix)", OpenViBE::Kernel::ErrorType::BadOutput);
				return true;
			}

			bool onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t index) override
			{
				OpenViBE::CIdentifier l_oTypeIdentifier = OV_UndefinedIdentifier;
				rBox.getInputType(0, l_oTypeIdentifier);
				if (l_oTypeIdentifier == OV_TypeId_Signal || l_oTypeIdentifier == OV_TypeId_Spectrum
					|| l_oTypeIdentifier == OV_TypeId_StreamedMatrix)
				{
					rBox.setOutputType(0, l_oTypeIdentifier);
					return true;
				}
				rBox.getOutputType(0, l_oTypeIdentifier);
				rBox.setInputType(0, l_oTypeIdentifier);

				OV_ERROR_KRF("Invalid input type [" << l_oTypeIdentifier.toString() << "] (expected Signal, Spectrum or Streamed Matrix)", OpenViBE::Kernel::ErrorType::BadInput);
			}

			bool onSettingValueChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t index) override
			{
				//we are only interested in the setting 0 and the type changes (select or reject)
				if ((index == 0 || index == 1) && (!m_bHasUserSetName))
				{
					OpenViBE::CString l_sChannels;
					rBox.getSettingValue(0, l_sChannels);

					OpenViBE::CString l_sSelectionMethod;
					OpenViBE::CIdentifier l_oSelectionEnumIdentifier = OV_UndefinedIdentifier;
					rBox.getSettingValue(1, l_sSelectionMethod);
					rBox.getSettingType(1, l_oSelectionEnumIdentifier);

					const OpenViBE::CIdentifier l_oSelectionMethodIdentifier = this->getTypeManager().getEnumerationEntryValueFromName(l_oSelectionEnumIdentifier, l_sSelectionMethod);

					if (l_oSelectionMethodIdentifier == OVP_TypeId_SelectionMethod_Reject)
					{
						l_sChannels = OpenViBE::CString("!") + l_sChannels;
					}
					rBox.setName(l_sChannels);
				}
				return true;
			}

			bool onNameChanged(OpenViBE::Kernel::IBox& rBox) override
			//when user set box name manually
			{
				if (m_bHasUserSetName)
				{
					OpenViBE::CString l_sRename = rBox.getName();
					if (l_sRename == OpenViBE::CString("Channel Selector"))
					{//default name, we switch back to default behaviour
						m_bHasUserSetName = false;
					}
				}
				else
				{
					m_bHasUserSetName = true;
				}
				return true;
			}

			bool initialize() override
			{
				m_bHasUserSetName = false;//need to initialize this value
				return true;
			}

		private:
			bool m_bHasUserSetName = false;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmChannelSelectorDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Channel Selector"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Select a subset of signal channels"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("Selection can be based on channel name (case-sensitive) or index starting from 0"); }
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
				prototype.addSetting("Action", OVP_TypeId_SelectionMethod, OVP_TypeId_SelectionMethod_Select.toString());
				prototype.addSetting("Channel Matching Method", OVP_TypeId_MatchMethod, OVP_TypeId_MatchMethod_Smart.toString());

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
