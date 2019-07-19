#pragma once

#include "../../ovp_defines.h"

#include <openvibe/ov_all.h>

#include <toolkit/ovtk_all.h>

#include <xml/IReader.h>

#include <stack>
#include <string>

#define OVP_ClassId_Algorithm_XMLScenarioImporter                                        OpenViBE::CIdentifier(0xE80C3EA2, 0x149C4A05)
#define OVP_ClassId_Algorithm_XMLScenarioImporterDesc                                    OpenViBE::CIdentifier(0xFF25D456, 0x721FCC57)

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CAlgorithmXMLScenarioImporter : public OpenViBEToolkit::CAlgorithmScenarioImporter, public XML::IReaderCallback
		{
		public:

			CAlgorithmXMLScenarioImporter(void);
			virtual ~CAlgorithmXMLScenarioImporter(void);

			virtual bool import(OpenViBE::Plugins::IAlgorithmScenarioImporterContext& rContext, const OpenViBE::IMemoryBuffer& rMemoryBuffer);

			virtual void openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64_t ui64AttributeCount); // XML::IReaderCallback
			virtual void processChildData(const char* sData); // XML::IReaderCallback
			virtual void closeChild(void); // XML::IReaderCallback

			_IsDerivedFromClass_Final_(OpenViBEToolkit::CAlgorithmScenarioImporter, OVP_ClassId_Algorithm_XMLScenarioImporter)

		protected:

			bool validateXML(const unsigned char* xmlBuffer, unsigned long xmlBufferSize);
			bool validateXMLAgainstSchema(const char* validationSchema, const unsigned char* xmlBuffer, unsigned long xmlBufferSize);

			OpenViBE::Plugins::IAlgorithmScenarioImporterContext* m_pContext;
			uint32_t m_ui32Status;
			XML::IReader* m_pReader;
			std::stack<std::string> m_vNodes;
		};

		class CAlgorithmXMLScenarioImporterDesc : public OpenViBEToolkit::CAlgorithmScenarioImporterDesc
		{
		public:

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const { return OpenViBE::CString("XML Scenario importer"); }
			virtual OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString("A sample XML scenario importer"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("This scenario importer uses simple XML format to input the scenario"); }
			virtual OpenViBE::CString getCategory(void) const { return OpenViBE::CString("Samples"); }
			virtual OpenViBE::CString getVersion(void) const { return OpenViBE::CString("1.0"); }
			// virtual OpenViBE::CString getFileExtension(void) const       { return OpenViBE::CString("xml;XML"); }
			virtual OpenViBE::CString getSoftwareComponent(void) const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_Algorithm_XMLScenarioImporter; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new CAlgorithmXMLScenarioImporter(); }

			_IsDerivedFromClass_Final_(OpenViBEToolkit::CAlgorithmScenarioImporterDesc, OVP_ClassId_Algorithm_XMLScenarioImporterDesc)
		};
	};
};


