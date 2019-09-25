#pragma once

#include "../../ovp_defines.h"

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>
#include <xml/IWriter.h>

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CAlgorithmXMLScenarioExporter final : public OpenViBEToolkit::CAlgorithmScenarioExporter, public XML::IWriterCallback
		{
		public:

			CAlgorithmXMLScenarioExporter();
			~CAlgorithmXMLScenarioExporter() override;
			bool exportStart(OpenViBE::IMemoryBuffer& memoryBuffer, const OpenViBE::CIdentifier& id) override;
			bool exportIdentifier(OpenViBE::IMemoryBuffer& memoryBuffer, const OpenViBE::CIdentifier& id, const OpenViBE::CIdentifier& value) override;
			bool exportString(OpenViBE::IMemoryBuffer& memoryBuffer, const OpenViBE::CIdentifier& id, const OpenViBE::CString& value) override;
			bool exportUInteger(OpenViBE::IMemoryBuffer& memoryBuffer, const OpenViBE::CIdentifier& id, uint64_t value) override;
			bool exportStop(OpenViBE::IMemoryBuffer& memoryBuffer) override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::CAlgorithmScenarioExporter, OVP_ClassId_Algorithm_XMLScenarioExporter)

		protected:
			void write(const char* str) override; // XML::IWriterCallback

			XML::IWriter* m_pWriter                  = nullptr;
			OpenViBE::IMemoryBuffer* m_pMemoryBuffer = nullptr;
		};

		class CAlgorithmXMLScenarioExporterDesc final : public OpenViBEToolkit::CAlgorithmScenarioExporterDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("XML Scenario exporter"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("A sample XML scenario exporter"); }

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString("This scenario exporter uses simple XML format to output the scenario");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("File reading and writing/XML Scenario"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			// virtual OpenViBE::CString getFileExtension() const       { return OpenViBE::CString("xml;XML"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_XMLScenarioExporter; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CAlgorithmXMLScenarioExporter(); }

			_IsDerivedFromClass_Final_(OpenViBEToolkit::CAlgorithmScenarioExporterDesc, OVP_ClassId_Algorithm_XMLScenarioExporterDesc)
		};
	} // namespace FileIO
} // namespace OpenViBEPlugins
