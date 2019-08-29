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
		class CAlgorithmXMLScenarioImporter final : public OpenViBEToolkit::CAlgorithmScenarioImporter, public XML::IReaderCallback
		{
		public:

			CAlgorithmXMLScenarioImporter();
			~CAlgorithmXMLScenarioImporter() override;
			bool import(OpenViBE::Plugins::IAlgorithmScenarioImporterContext& rContext, const OpenViBE::IMemoryBuffer& rMemoryBuffer) override;
			void openChild(const char* sName, const char** sAttributeName, const char** sAttributeValue, uint64_t ui64AttributeCount) override; // XML::IReaderCallback
			void processChildData(const char* sData) override; // XML::IReaderCallback
			void closeChild() override; // XML::IReaderCallback

			_IsDerivedFromClass_Final_(OpenViBEToolkit::CAlgorithmScenarioImporter, OVP_ClassId_Algorithm_XMLScenarioImporter)

		protected:

			bool validateXML(const unsigned char* xmlBuffer, unsigned long xmlBufferSize);
			bool validateXMLAgainstSchema(const char* validationSchema, const unsigned char* xmlBuffer, unsigned long xmlBufferSize);

			OpenViBE::Plugins::IAlgorithmScenarioImporterContext* m_pContext = nullptr;
			uint32_t m_ui32Status                                            = 0;
			XML::IReader* m_pReader                                          = nullptr;
			std::stack<std::string> m_vNodes;
		};

		class CAlgorithmXMLScenarioImporterDesc final : public OpenViBEToolkit::CAlgorithmScenarioImporterDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("XML Scenario importer"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("A sample XML scenario importer"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString("This scenario importer uses simple XML format to input the scenario"); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Samples"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			// virtual OpenViBE::CString getFileExtension() const       { return OpenViBE::CString("xml;XML"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_XMLScenarioImporter; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CAlgorithmXMLScenarioImporter(); }

			_IsDerivedFromClass_Final_(OpenViBEToolkit::CAlgorithmScenarioImporterDesc, OVP_ClassId_Algorithm_XMLScenarioImporterDesc)
		};
	} // namespace FileIO
} // namespace OpenViBEPlugins
