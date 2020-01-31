#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>
#include <vector>

namespace OpenViBE
{
	namespace Plugins
	{
		namespace Tools
		{
			class CBoxAlgorithmStimulationListener final : virtual public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
			{
			public:
				void release() override { delete this; }
				bool initialize() override;
				bool uninitialize() override;
				bool processInput(const size_t index) override;
				bool process() override;

				_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_StimulationListener)

			protected:

				Kernel::ELogLevel m_logLevel = Kernel::LogLevel_None;
				std::vector<Toolkit::TStimulationDecoder<CBoxAlgorithmStimulationListener>*> m_stimulationDecoders;
			};

			class CBoxAlgorithmStimulationListenerListener final : public Toolkit::TBoxListener<IBoxListener>
			{
			public:

				bool check(Kernel::IBox& box) const
				{
					for (size_t i = 0; i < box.getInputCount(); ++i)
					{
						box.setInputName(i, ("Stimulation stream " + std::to_string(i + 1)).c_str());
						box.setInputType(i, OV_TypeId_Stimulations);
					}

					return true;
				}

				bool onInputRemoved(Kernel::IBox& box, const size_t /*index*/) override { return this->check(box); }
				bool onInputAdded(Kernel::IBox& box, const size_t /*index*/) override { return this->check(box); }

				_IsDerivedFromClass_Final_(Toolkit::TBoxListener<IBoxListener>, OV_UndefinedIdentifier)
			};

			class CBoxAlgorithmStimulationListenerDesc final : virtual public IBoxAlgorithmDesc
			{
			public:
				void release() override { }
				CString getName() const override { return CString("Stimulation listener"); }
				CString getAuthorName() const override { return CString("Yann Renard"); }
				CString getAuthorCompanyName() const override { return CString("INRIA/IRISA"); }
				CString getShortDescription() const override { return CString("Prints stimulation codes in the log manager"); }

				CString getDetailedDescription() const override
				{
					return CString("Prints each received stimulationto the log using the log level specified in the box config.");
				}

				CString getCategory() const override { return CString("Tools"); }
				CString getVersion() const override { return CString("1.0"); }
				CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
				CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
				CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
				CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_StimulationListener; }
				IPluginObject* create() override { return new CBoxAlgorithmStimulationListener; }
				IBoxListener* createBoxListener() const override { return new CBoxAlgorithmStimulationListenerListener; }
				void releaseBoxListener(IBoxListener* listener) const override { delete listener; }

				bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
				{
					prototype.addInput("Stimulation stream 1", OV_TypeId_Stimulations);
					prototype.addSetting("Log level to use", OV_TypeId_LogLevel, "Information");
					prototype.addFlag(Kernel::BoxFlag_CanAddInput);
					return true;
				}

				_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StimulationListenerDesc)
			};
		} // namespace Tools
	}  // namespace Plugins
}  // namespace OpenViBE
