#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>
#include <vector>
#include <cstdio>

#define OVP_ClassId_BoxAlgorithm_StimulationListener     OpenViBE::CIdentifier(0x65731E1D, 0x47DE5276)
#define OVP_ClassId_BoxAlgorithm_StimulationListenerDesc OpenViBE::CIdentifier(0x0EC013FD, 0x5DD23E44)

namespace OpenViBEPlugins
{
	namespace Tools
	{
		class CBoxAlgorithmStimulationListener : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_StimulationListener)

		protected:

			OpenViBE::Kernel::ELogLevel m_eLogLevel;
			std::vector<OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmStimulationListener>*> m_vStimulationDecoder;
		};

		class CBoxAlgorithmStimulationListenerListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			bool check(OpenViBE::Kernel::IBox& rBox)
			{
				char l_sName[1024];

				for (uint32_t i = 0; i < rBox.getInputCount(); i++)
				{
					sprintf(l_sName, "Stimulation stream %u", i + 1);
					rBox.setInputName(i, l_sName);
					rBox.setInputType(i, OV_TypeId_Stimulations);
				}

				return true;
			}

			bool onInputRemoved(OpenViBE::Kernel::IBox& rBox, const uint32_t /*index*/) override { return this->check(rBox); }
			bool onInputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t /*index*/) override { return this->check(rBox); }

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmStimulationListenerDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Stimulation listener"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Prints stimulation codes in the log manager"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("Prints each received stimulationto the log using the log level specified in the box config."); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Tools"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_StimulationListener; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmStimulationListener; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmStimulationListenerListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Stimulation stream 1", OV_TypeId_Stimulations);
				prototype.addSetting("Log level to use", OV_TypeId_LogLevel, "Information");
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StimulationListenerDesc)
		};
	} // namespace Tools
} // namespace OpenViBEPlugins
