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

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processInput(const uint32_t ui32InputIndex);
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ChannelSelector)

		protected:

			OpenViBEToolkit::TDecoder<CBoxAlgorithmChannelSelector>* m_pDecoder;
			OpenViBEToolkit::TEncoder<CBoxAlgorithmChannelSelector>* m_pEncoder;

			OpenViBE::IMatrix* m_pInputMatrix;
			OpenViBE::IMatrix* m_pOutputMatrix;

			std::vector<uint32_t> m_vLookup;
		};

		class CBoxAlgorithmChannelSelectorListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			virtual bool onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
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

			virtual bool onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
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

			virtual bool onSettingValueChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				//we are only interested in the setting 0 and the type changes (select or reject)
				if ((ui32Index == 0 || ui32Index == 1) && (!m_bHasUserSetName))
				{
					OpenViBE::CString l_sChannels;
					rBox.getSettingValue(0, l_sChannels);

					OpenViBE::CString l_sSelectionMethod;
					OpenViBE::CIdentifier l_oSelectionEnumIdentifier;
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

			virtual bool onNameChanged(OpenViBE::Kernel::IBox& rBox)//when user set box name manually
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

			virtual bool initialize()
			{
				m_bHasUserSetName = false;//need to initialize this value
				return true;
			}

		private:
			bool m_bHasUserSetName;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmChannelSelectorDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Channel Selector"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Select a subset of signal channels"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("Selection can be based on channel name (case-sensitive) or index starting from 0"); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Signal processing/Channels"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_ChannelSelector; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmChannelSelector; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener() const { return new CBoxAlgorithmChannelSelectorListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Input signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput("Output signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("Channel List", OV_TypeId_String, ":");
				rBoxAlgorithmPrototype.addSetting("Action", OVP_TypeId_SelectionMethod, OVP_TypeId_SelectionMethod_Select.toString());
				rBoxAlgorithmPrototype.addSetting("Channel Matching Method", OVP_TypeId_MatchMethod, OVP_TypeId_MatchMethod_Smart.toString());

				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);

				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_Spectrum);
				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_StreamedMatrix);

				rBoxAlgorithmPrototype.addOutputSupport(OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutputSupport(OV_TypeId_Spectrum);
				rBoxAlgorithmPrototype.addOutputSupport(OV_TypeId_StreamedMatrix);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ChannelSelectorDesc)
		};
	}  // namespace SignalProcessing
}  // namespace OpenViBEPlugins
