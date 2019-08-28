#pragma once

#include <toolkit/ovtk_all.h>
#include <cstdio>

#define OVP_ClassId_BoxAlgorithm_Identity                                              OpenViBE::CIdentifier(0x5DFFE431, 0x35215C50)
#define OVP_ClassId_BoxAlgorithm_IdentityDesc                                          OpenViBE::CIdentifier(0x54743810, 0x6A1A88CC)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmIdentity : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_BoxAlgorithm_Identity)
		};

		class CBoxAlgorithmIdentityListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			static bool check(OpenViBE::Kernel::IBox& rBox)
			{
				char name[1024];
				uint32_t i;
				for (i = 0; i < rBox.getInputCount(); i++)
				{
					sprintf(name, "Input stream %u", i + 1);
					rBox.setInputName(i, name);
				}
				for (i = 0; i < rBox.getOutputCount(); i++)
				{
					sprintf(name, "Output stream %u", i + 1);
					rBox.setOutputName(i, name);
				}
				return true;
			}

			bool onDefaultInitialized(OpenViBE::Kernel::IBox& rBox) override
			{
				rBox.setInputType(0, OV_TypeId_Signal);
				rBox.setOutputType(0, OV_TypeId_Signal);
				return true;
			}

			bool onInputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t index) override
			{
				rBox.setInputType(index, OV_TypeId_Signal);
				rBox.addOutput("", OV_TypeId_Signal, rBox.getUnusedInputIdentifier());
				this->check(rBox);
				return true;
			}

			bool onInputRemoved(OpenViBE::Kernel::IBox& rBox, const uint32_t index) override
			{
				rBox.removeOutput(index);
				this->check(rBox);
				return true;
			}

			bool onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t index) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				rBox.getInputType(index, typeID);
				rBox.setOutputType(index, typeID);
				return true;
			}

			bool onOutputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t index) override
			{
				rBox.setOutputType(index, OV_TypeId_Signal);
				rBox.addInput("", OV_TypeId_Signal, rBox.getUnusedOutputIdentifier());
				this->check(rBox);
				return true;
			}

			bool onOutputRemoved(OpenViBE::Kernel::IBox& rBox, const uint32_t index) override
			{
				rBox.removeInput(index);
				this->check(rBox);
				return true;
			}

			bool onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t index) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				rBox.getOutputType(index, typeID);
				rBox.setInputType(index, typeID);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmIdentityDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Identity"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Duplicates input to output"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("This simply duplicates intput on its output"); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Basic"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_Identity; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmIdentity(); }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmIdentityListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const override
			{
				rPrototype.addInput("Input stream", OV_TypeId_Signal);
				rPrototype.addOutput("Output stream", OV_TypeId_Signal);
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddOutput);
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_IdentityDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins
