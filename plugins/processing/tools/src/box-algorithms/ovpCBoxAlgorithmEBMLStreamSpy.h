#pragma once

#include "../ovp_defines.h"
#include <toolkit/ovtk_all.h>
#include <ebml/IReader.h>
#include <ebml/IReaderHelper.h>

#include <stack>
#include <map>
#include <string>
#include <cstdio>

#define OVP_ClassId_BoxAlgorithm_EBMLStreamSpy     OpenViBE::CIdentifier(0x0ED76695, 0x01A69CC3)
#define OVP_ClassId_BoxAlgorithm_EBMLStreamSpyDesc OpenViBE::CIdentifier(0x354A6864, 0x06BC570C)

namespace OpenViBEPlugins
{
	namespace Tools
	{
		class CBoxAlgorithmEBMLStreamSpy : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, virtual public EBML::IReaderCallback
		{
		public:

			CBoxAlgorithmEBMLStreamSpy();

			virtual void release();

			virtual bool initialize();
			virtual bool uninitialize();

			virtual bool isMasterChild(const EBML::CIdentifier& rIdentifier);
			virtual void openChild(const EBML::CIdentifier& rIdentifier);
			virtual void processChildData(const void* pBuffer, uint64_t ui64BufferSize);
			virtual void closeChild();

			virtual bool processInput(uint32_t ui32InputIndex);
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_EBMLStreamSpy)

		protected:

			template <class T>
			void processBinaryBlock(const void* pBuffer, uint64_t ui64BufferSize);

			std::stack<EBML::CIdentifier> m_vNodes;
			std::map<EBML::CIdentifier, std::string> m_vName;
			std::map<EBML::CIdentifier, std::string> m_vType;
			uint64_t m_ui64ExpandValuesCount;
			OpenViBE::Kernel::ELogLevel m_eLogLevel;
			EBML::IReader* m_pReader;
			EBML::IReaderHelper* m_pReaderHelper;
		};

		class CBoxAlgorithmEBMLStreamSpyListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			bool check(OpenViBE::Kernel::IBox& rBox)
			{
				char l_sName[1024];
				uint32_t i;

				for (i = 0; i < rBox.getInputCount(); i++)
				{
					sprintf(l_sName, "Spied EBML stream %u", i + 1);
					rBox.setInputName(i, l_sName);
					rBox.setInputType(i, OV_TypeId_EBMLStream);
				}

				return true;
			}

			virtual bool onInputRemoved(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return this->check(rBox); }
			virtual bool onInputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return this->check(rBox); };

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmEBMLStreamSpyDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }
			virtual OpenViBE::CString getName() const { return OpenViBE::CString("EBML stream spy"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("EBML stream tree viewer"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("This sample EBML stream analyzer prints the EBML tree structure to the console"); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Tools"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_EBMLStreamSpy; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmEBMLStreamSpy(); }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener() const { return new CBoxAlgorithmEBMLStreamSpyListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput("Spied EBML stream 1", OV_TypeId_EBMLStream);
				rPrototype.addSetting("EBML nodes description", OV_TypeId_Filename, "${Path_Data}/plugins/tools/config-ebml-stream-spy.txt");
				rPrototype.addSetting("Log level to use", OV_TypeId_LogLevel, "Information");
				rPrototype.addSetting("Expand binary blocks", OV_TypeId_Boolean, "false");
				rPrototype.addSetting("Number of values in expanded blocks", OV_TypeId_Integer, "4");
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_EBMLStreamSpyDesc)
		};
	};
};
