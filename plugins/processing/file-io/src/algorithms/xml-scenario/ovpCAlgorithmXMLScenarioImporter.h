#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>
#include <xml/IReader.h>
#include <stack>
#include <string>


namespace OpenViBE
{
	namespace Plugins
	{
	namespace FileIO
	{
		class CAlgorithmXMLScenarioImporter final : public Toolkit::CAlgorithmScenarioImporter, public XML::IReaderCallback
		{
		public:

			CAlgorithmXMLScenarioImporter();
			~CAlgorithmXMLScenarioImporter() override;
			bool import(IAlgorithmScenarioImporterContext& rContext, const IMemoryBuffer& memoryBuffer) override;
			void openChild(const char* name, const char** attributeName, const char** sAttributeValue, const size_t nAttribute) override; // XML::IReaderCallback
			void processChildData(const char* data) override; // XML::IReaderCallback
			void closeChild() override; // XML::IReaderCallback

			_IsDerivedFromClass_Final_(Toolkit::CAlgorithmScenarioImporter, OVP_ClassId_Algorithm_XMLScenarioImporter)

		protected:

			bool validateXML(const unsigned char* buffer, size_t size);
			bool validateXMLAgainstSchema(const char* validationSchema, const unsigned char* buffer, size_t size);

			IAlgorithmScenarioImporterContext* m_ctx = nullptr;
			size_t m_status                                             = 0;
			XML::IReader* m_reader                                      = nullptr;
			std::stack<std::string> m_nodes;
		};

		class CAlgorithmXMLScenarioImporterDesc final : public Toolkit::CAlgorithmScenarioImporterDesc
		{
		public:
			void release() override { }
			CString getName() const override { return CString("XML Scenario importer"); }
			CString getAuthorName() const override { return CString("Yann Renard"); }
			CString getAuthorCompanyName() const override { return CString("INRIA/IRISA"); }
			CString getShortDescription() const override { return CString("A sample XML scenario importer"); }

			CString getDetailedDescription() const override
			{
				return CString("This scenario importer uses simple XML format to input the scenario");
			}

			CString getCategory() const override { return CString("Samples"); }
			CString getVersion() const override { return CString("1.0"); }
			// virtual CString getFileExtension() const       { return CString("xml;XML"); }
			CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
			CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
			CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
			CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_XMLScenarioImporter; }
			IPluginObject* create() override { return new CAlgorithmXMLScenarioImporter(); }

			_IsDerivedFromClass_Final_(Toolkit::CAlgorithmScenarioImporterDesc, OVP_ClassId_Algorithm_XMLScenarioImporterDesc)
		};
	} // namespace FileIO
	}  // namespace Plugins
}  // namespace OpenViBE
