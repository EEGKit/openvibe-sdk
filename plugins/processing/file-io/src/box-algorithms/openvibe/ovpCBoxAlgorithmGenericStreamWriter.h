#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <ebml/CWriter.h>
#include <ebml/CWriterHelper.h>
#include <ebml/CReader.h>

#include <cstdio>

#include <fstream>

// TODO:
// - please move the identifier definitions in ovp_defines.h
// - please include your desciptor in ovp_main.cpp

#define OVP_ClassId_BoxAlgorithm_GenericStreamWriter     OpenViBE::CIdentifier(0x09C92218, 0x7C1216F8)
#define OVP_ClassId_BoxAlgorithm_GenericStreamWriterDesc OpenViBE::CIdentifier(0x50AB506A, 0x54804437)

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CBoxAlgorithmGenericStreamWriter final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, public EBML::IWriterCallback
		{
		public:

			CBoxAlgorithmGenericStreamWriter();
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			bool generateFileHeader();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_GenericStreamWriter)

		protected:

			bool m_bIsHeaderGenerate = false;
			OpenViBE::CString m_sFilename;
			EBML::CWriter m_oWriter;
			EBML::CWriterHelper m_oWriterHelper;

		private:
			void write(const void* buffer, const uint64_t size) override;

			OpenViBE::CMemoryBuffer m_oSwap;
			std::ofstream m_oFile;
		};

		class CBoxAlgorithmGenericStreamWriterListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			//it seems the only purpose of the check was to give a name when adding an input
			//without it, the input configuration dialog display random characters in the name field
			//the check is unnecessary when removing/changing inputs and on already named inputs
			bool check(OpenViBE::Kernel::IBox& box)
			{
				char l_sName[1024];
				uint32_t i = box.getInputCount() - 1;
				//only check last input (we assume previous inputs have benn named, how could they not?)
				sprintf(l_sName, "Input stream %u", i + 1);
				box.setInputName(i, l_sName);
				/*
				for(i=0; i<box.getInputCount(); i++)
				{
					sprintf(l_sName, "Input stream %u", i+1);
					box.setInputName(i, l_sName);
				}
				//*/
				return true;
			}

			bool onDefaultInitialized(OpenViBE::Kernel::IBox& box) override
			{
				box.setInputName(0, "Input Signal");
				box.setInputType(0, OV_TypeId_Signal);
				box.addInput("Input Stimulations", OV_TypeId_Stimulations);
				return true;
			}

			bool onInputAdded(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				box.setInputType(index, OV_TypeId_EBMLStream);
				this->check(box);
				return true;
			}

			bool onInputRemoved(OpenViBE::Kernel::IBox& /*box*/, const uint32_t /*index*/) override
			{
				//this->check(box);
				return true;
			}

			bool onInputTypeChanged(OpenViBE::Kernel::IBox& /*box*/, const uint32_t /*index*/) override
			{
				//this->check(box);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmGenericStreamWriterDesc final : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Generic stream writer"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Writes any number of streams into an .ov file"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("File reading and writing/OpenViBE"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_GenericStreamWriter; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmGenericStreamWriter; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmGenericStreamWriterListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input stream 1", OV_TypeId_EBMLStream);
				prototype.addSetting("Filename", OV_TypeId_Filename, "record-[$core{date}-$core{time}].ov");
				prototype.addSetting("Use compression", OV_TypeId_Boolean, "false");
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_GenericStreamWriterDesc)
		};
	} // namespace FileIO
} // namespace OpenViBEPlugins
