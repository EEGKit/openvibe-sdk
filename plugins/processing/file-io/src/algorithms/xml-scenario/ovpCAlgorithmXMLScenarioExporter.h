#pragma once

#include "../../ovp_defines.h"

#include <openvibe/ov_all.h>

#include <toolkit/ovtk_all.h>

#include <xml/IWriter.h>

#define OVP_ClassId_Algorithm_XMLScenarioExporter                                        OpenViBE::CIdentifier(0x53693531, 0xB136CF3F)
#define OVP_ClassId_Algorithm_XMLScenarioExporterDesc                                    OpenViBE::CIdentifier(0x9709C9FA, 0xF126F74E)

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CAlgorithmXMLScenarioExporter : public OpenViBEToolkit::CAlgorithmScenarioExporter, public XML::IWriterCallback
		{
		public:

			CAlgorithmXMLScenarioExporter();
			virtual ~CAlgorithmXMLScenarioExporter();

			virtual bool exportStart(OpenViBE::IMemoryBuffer& rMemoryBuffer, const OpenViBE::CIdentifier& rIdentifier);
			virtual bool exportIdentifier(OpenViBE::IMemoryBuffer& rMemoryBuffer, const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::CIdentifier& rValue);
			virtual bool exportString(OpenViBE::IMemoryBuffer& rMemoryBuffer, const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::CString& rValue);
			virtual bool exportUInteger(OpenViBE::IMemoryBuffer& rMemoryBuffer, const OpenViBE::CIdentifier& rIdentifier, uint64_t ui64Value);
			virtual bool exportStop(OpenViBE::IMemoryBuffer& rMemoryBuffer);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::CAlgorithmScenarioExporter, OVP_ClassId_Algorithm_XMLScenarioExporter)

		protected:

			virtual void write(const char* sString); // XML::IWriterCallback

			XML::IWriter* m_pWriter;
			OpenViBE::IMemoryBuffer* m_pMemoryBuffer;
		};

		class CAlgorithmXMLScenarioExporterDesc : public OpenViBEToolkit::CAlgorithmScenarioExporterDesc
		{
		public:

			virtual void release() { }
			virtual OpenViBE::CString getName() const { return OpenViBE::CString("XML Scenario exporter"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("A sample XML scenario exporter"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("This scenario exporter uses simple XML format to output the scenario"); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("File reading and writing/XML Scenario"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			// virtual OpenViBE::CString getFileExtension() const       { return OpenViBE::CString("xml;XML"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_Algorithm_XMLScenarioExporter; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CAlgorithmXMLScenarioExporter(); }

			_IsDerivedFromClass_Final_(OpenViBEToolkit::CAlgorithmScenarioExporterDesc, OVP_ClassId_Algorithm_XMLScenarioExporterDesc)
		};
	};
};
