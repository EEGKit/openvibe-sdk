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

			virtual void release(void);
			virtual bool processInput(uint32_t ui32InputIndex);
			virtual bool process(void);

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_BoxAlgorithm_Identity)
		};

		class CBoxAlgorithmIdentityListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			bool check(OpenViBE::Kernel::IBox& rBox)
			{
				char l_sName[1024];
				uint32_t i;
				for (i = 0; i < rBox.getInputCount(); i++)
				{
					sprintf(l_sName, "Input stream %u", i + 1);
					rBox.setInputName(i, l_sName);
				}
				for (i = 0; i < rBox.getOutputCount(); i++)
				{
					sprintf(l_sName, "Output stream %u", i + 1);
					rBox.setOutputName(i, l_sName);
				}
				return true;
			}

			virtual bool onDefaultInitialized(OpenViBE::Kernel::IBox& rBox)
			{
				rBox.setInputType(0, OV_TypeId_Signal);
				rBox.setOutputType(0, OV_TypeId_Signal);
				return true;
			}

			virtual bool onInputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				rBox.setInputType(ui32Index, OV_TypeId_Signal);
				rBox.addOutput("", OV_TypeId_Signal, rBox.getUnusedInputIdentifier());
				this->check(rBox);
				return true;
			}

			virtual bool onInputRemoved(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				rBox.removeOutput(ui32Index);
				this->check(rBox);
				return true;
			}

			virtual bool onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getInputType(ui32Index, l_oTypeIdentifier);
				rBox.setOutputType(ui32Index, l_oTypeIdentifier);
				return true;
			}

			virtual bool onOutputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				rBox.setOutputType(ui32Index, OV_TypeId_Signal);
				rBox.addInput("", OV_TypeId_Signal, rBox.getUnusedOutputIdentifier());
				this->check(rBox);
				return true;
			}

			virtual bool onOutputRemoved(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				rBox.removeInput(ui32Index);
				this->check(rBox);
				return true;
			}

			virtual bool onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getOutputType(ui32Index, l_oTypeIdentifier);
				rBox.setInputType(ui32Index, l_oTypeIdentifier);
				return true;
			};

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CBoxAlgorithmIdentityDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const { return OpenViBE::CString("Identity"); }
			virtual OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString("Duplicates input to output"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("This simply duplicates intput on its output"); }
			virtual OpenViBE::CString getCategory(void) const { return OpenViBE::CString("Signal processing/Basic"); }
			virtual OpenViBE::CString getVersion(void) const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent(void) const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_BoxAlgorithm_Identity; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new CBoxAlgorithmIdentity(); }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const { return new CBoxAlgorithmIdentityListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
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
	};
};


