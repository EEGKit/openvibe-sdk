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
		class CBoxAlgorithmGenericStreamWriter : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, public EBML::IWriterCallback
		{
		public:

			CBoxAlgorithmGenericStreamWriter();
			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processInput(uint32_t ui32InputIndex);
			virtual bool process();

			bool generateFileHeader();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_GenericStreamWriter);

		protected:

			bool m_bIsHeaderGenerate;
			OpenViBE::CString m_sFilename;
			EBML::CWriter m_oWriter;
			EBML::CWriterHelper m_oWriterHelper;

		private:

			virtual void write(const void* pBuffer, uint64_t ui64BufferSize);

		private:

			OpenViBE::CMemoryBuffer m_oSwap;
			std::ofstream m_oFile;
		};

		class CBoxAlgorithmGenericStreamWriterListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			//it seems the only purpose of the check was to give a name when adding an input
			//without it, the input configuration dialog display random characters in the name field
			//the check is unnecessary when removing/changing inputs and on already named inputs
			bool check(OpenViBE::Kernel::IBox& rBox)
			{
				char l_sName[1024];
				uint32_t i = rBox.getInputCount() - 1;
				//only check last input (we assume previous inputs have benn named, how could they not?)
				sprintf(l_sName, "Input stream %u", i + 1);
				rBox.setInputName(i, l_sName);
				/*
				for(i=0; i<rBox.getInputCount(); i++)
				{
					sprintf(l_sName, "Input stream %u", i+1);
					rBox.setInputName(i, l_sName);
				}
				//*/
				return true;
			}

			virtual bool onDefaultInitialized(OpenViBE::Kernel::IBox& rBox)
			{
				rBox.setInputName(0, "Input Signal");
				rBox.setInputType(0, OV_TypeId_Signal);
				rBox.addInput("Input Stimulations", OV_TypeId_Stimulations);
				return true;
			}

			virtual bool onInputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				rBox.setInputType(ui32Index, OV_TypeId_EBMLStream);
				this->check(rBox);
				return true;
			}

			virtual bool onInputRemoved(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				//this->check(rBox);
				return true;
			}

			virtual bool onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				//this->check(rBox);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmGenericStreamWriterDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Generic stream writer"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Writes any number of streams into an .ov file"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("File reading and writing/OpenViBE"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_GenericStreamWriter; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmGenericStreamWriter; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener() const { return new CBoxAlgorithmGenericStreamWriterListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Input stream 1", OV_TypeId_EBMLStream);
				rBoxAlgorithmPrototype.addSetting("Filename", OV_TypeId_Filename, "record-[$core{date}-$core{time}].ov");
				rBoxAlgorithmPrototype.addSetting("Use compression", OV_TypeId_Boolean, "false");
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_GenericStreamWriterDesc);
		};
	};
};


