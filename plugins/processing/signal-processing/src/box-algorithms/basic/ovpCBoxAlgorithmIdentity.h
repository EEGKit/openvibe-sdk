#pragma once

#include <toolkit/ovtk_all.h>
#include <cstdio>

#define OVP_ClassId_BoxAlgorithm_Identity                                              OpenViBE::CIdentifier(0x5DFFE431, 0x35215C50)
#define OVP_ClassId_BoxAlgorithm_IdentityDesc                                          OpenViBE::CIdentifier(0x54743810, 0x6A1A88CC)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmIdentity final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_BoxAlgorithm_Identity)
		};

		class CBoxAlgorithmIdentityListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			static bool check(OpenViBE::Kernel::IBox& box)
			{
				char name[1024];
				uint32_t i;
				for (i = 0; i < box.getInputCount(); i++)
				{
					sprintf(name, "Input stream %u", i + 1);
					box.setInputName(i, name);
				}
				for (i = 0; i < box.getOutputCount(); i++)
				{
					sprintf(name, "Output stream %u", i + 1);
					box.setOutputName(i, name);
				}
				return true;
			}

			bool onDefaultInitialized(OpenViBE::Kernel::IBox& box) override
			{
				box.setInputType(0, OV_TypeId_Signal);
				box.setOutputType(0, OV_TypeId_Signal);
				return true;
			}

			bool onInputAdded(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				box.setInputType(index, OV_TypeId_Signal);
				box.addOutput("", OV_TypeId_Signal, box.getUnusedInputIdentifier());
				this->check(box);
				return true;
			}

			bool onInputRemoved(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				box.removeOutput(index);
				this->check(box);
				return true;
			}

			bool onInputTypeChanged(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getInputType(index, typeID);
				box.setOutputType(index, typeID);
				return true;
			}

			bool onOutputAdded(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				box.setOutputType(index, OV_TypeId_Signal);
				box.addInput("", OV_TypeId_Signal, box.getUnusedOutputIdentifier());
				this->check(box);
				return true;
			}

			bool onOutputRemoved(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				box.removeInput(index);
				this->check(box);
				return true;
			}

			bool onOutputTypeChanged(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getOutputType(index, typeID);
				box.setInputType(index, typeID);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmIdentityDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
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
