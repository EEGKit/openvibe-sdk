#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>
#include <xml/IReader.h>
#include <stack>
#include <string>


namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CAlgorithmXMLScenarioImporter final : public OpenViBEToolkit::CAlgorithmScenarioImporter, public XML::IReaderCallback
		{
		public:

			CAlgorithmXMLScenarioImporter();
			~CAlgorithmXMLScenarioImporter() override;
			bool import(OpenViBE::Plugins::IAlgorithmScenarioImporterContext& rContext, const OpenViBE::IMemoryBuffer& memoryBuffer) override;
			void openChild(const char* name, const char** attributeName, const char** sAttributeValue, const size_t nAttribute) override; // XML::IReaderCallback
			void processChildData(const char* data) override; // XML::IReaderCallback
			void closeChild() override; // XML::IReaderCallback

			_IsDerivedFromClass_Final_(OpenViBEToolkit::CAlgorithmScenarioImporter, OVP_ClassId_Algorithm_XMLScenarioImporter)

		protected:

			bool validateXML(const unsigned char* buffer, size_t size);
			bool validateXMLAgainstSchema(const char* validationSchema, const unsigned char* buffer, size_t size);

			OpenViBE::Plugins::IAlgorithmScenarioImporterContext* m_ctx = nullptr;
			size_t m_status                                             = 0;
			XML::IReader* m_reader                                      = nullptr;
			std::stack<std::string> m_nodes;
		};

		class CAlgorithmXMLScenarioImporterDesc final : public OpenViBEToolkit::CAlgorithmScenarioImporterDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("XML Scenario importer"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("A sample XML scenario importer"); }

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString("This scenario importer uses simple XML format to input the scenario");
			}

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
