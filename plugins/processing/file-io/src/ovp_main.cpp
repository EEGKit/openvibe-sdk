#include "ovp_defines.h"

#include "algorithms/ovpCAlgorithmOVMatrixFileReader.h"
#include "algorithms/ovpCAlgorithmOVMatrixFileWriter.h"

#include "algorithms/xml-scenario/ovpCAlgorithmXMLScenarioExporter.h"
#include "algorithms/xml-scenario/ovpCAlgorithmXMLScenarioImporter.h"


#include "box-algorithms/csv/ovpCBoxAlgorithmCSVFileWriter.h"
#include "box-algorithms/csv/ovpCBoxAlgorithmCSVFileReader.h"

#include "box-algorithms/openvibe/ovpCBoxAlgorithmGenericStreamReader.h"
#include "box-algorithms/openvibe/ovpCBoxAlgorithmGenericStreamWriter.h"

#include "box-algorithms/ovpCBoxAlgorithmElectrodeLocalizationFileReader.h"

#include "box-algorithms/csv/ovpCBoxAlgorithmOVCSVFileWriter.h"
#include "box-algorithms/csv/ovpCBoxAlgorithmOVCSVFileReader.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Plugins;

OVP_Declare_Begin()
	OVP_Declare_New(FileIO::CAlgorithmOVMatrixFileReaderDesc)
	OVP_Declare_New(FileIO::CAlgorithmOVMatrixFileWriterDesc)

	OVP_Declare_New(FileIO::CAlgorithmXMLScenarioExporterDesc)
	OVP_Declare_New(FileIO::CAlgorithmXMLScenarioImporterDesc)

	OVP_Declare_New(FileIO::CBoxAlgorithmCSVFileWriterDesc)
	OVP_Declare_New(FileIO::CBoxAlgorithmCSVFileReaderDesc)

	OVP_Declare_New(FileIO::CBoxAlgorithmGenericStreamReaderDesc)
	OVP_Declare_New(FileIO::CBoxAlgorithmGenericStreamWriterDesc)

	OVP_Declare_New(FileIO::CBoxAlgorithmElectrodeLocalisationFileReaderDesc)

	OVP_Declare_New(FileIO::CBoxAlgorithmOVCSVFileWriterDesc)
	OVP_Declare_New(FileIO::CBoxAlgorithmOVCSVFileReaderDesc)

	context.getScenarioManager().registerScenarioImporter(OV_ScenarioImportContext_SchedulerMetaboxImport, ".mxb", OVP_ClassId_Algorithm_XMLScenarioImporter);
	context.getConfigurationManager().createConfigurationToken("ScenarioFileNameExtension.xml", "OpenViBE XML Scenario");
	context.getConfigurationManager().createConfigurationToken("ScenarioFileNameExtension.mxs", "Mensia XML Scenario");
	context.getConfigurationManager().createConfigurationToken("ScenarioFileNameExtension.mxb", "Mensia XML Component");

OVP_Declare_End()
