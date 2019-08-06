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

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processInput(const uint32_t ui32InputIndex);
			virtual bool process();

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

			virtual bool onInputRemoved(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return this->check(rBox); }
			virtual bool onInputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return this->check(rBox); };

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmStimulationListenerDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Stimulation listener"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Prints stimulation codes in the log manager"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("Prints each received stimulationto the log using the log level specified in the box config."); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Tools"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_StimulationListener; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmStimulationListener; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener() const { return new CBoxAlgorithmStimulationListenerListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Stimulation stream 1", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addSetting("Log level to use", OV_TypeId_LogLevel, "Information");
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StimulationListenerDesc)
		};
	};
};
