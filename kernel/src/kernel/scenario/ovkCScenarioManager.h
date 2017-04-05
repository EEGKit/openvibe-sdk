#pragma once

#include "../ovkTKernelObject.h"

#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CScenario;

		class CScenarioManager final: public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IScenarioManager>
		{
		public:

			explicit CScenarioManager(const OpenViBE::Kernel::IKernelContext& rKernelContext);
			virtual ~CScenarioManager(void);

			virtual void cloneScenarioImportersAndExporters(const IScenarioManager& scenarioManager);

			virtual OpenViBE::CIdentifier getNextScenarioIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const;

			virtual bool isScenario(const OpenViBE::CIdentifier& scenarioIdentifier) const;

			virtual OpenViBE::boolean createScenario(
				OpenViBE::CIdentifier& rScenarioIdentifier);

			virtual bool importScenario(
			        OpenViBE::CIdentifier& newScenarioIdentifier,
			        const OpenViBE::IMemoryBuffer& inputMemoryBuffer,
			        const OpenViBE::CIdentifier& scenarioImporterAlgorithmIdentifier
			        );

			virtual bool importScenarioFromFile(
			        OpenViBE::CIdentifier& newScenarioIdentifier,
			        const OpenViBE::CString& fileName,
			        const OpenViBE::CIdentifier& scenarioImporterAlgorithmIdentifier
			        );

			virtual bool importScenarioFromFile(
			        OpenViBE::CIdentifier& newScenarioIdentifier,
			        const OpenViBE::CIdentifier& importContext,
			        const OpenViBE::CString& fileName);

			virtual bool registerScenarioImporter(
			        const OpenViBE::CIdentifier& importContext,
			        const CString& fileNameExtension,
			        const OpenViBE::CIdentifier& scenarioImporterAlgorithmIdentifier
			        );

			virtual bool unregisterScenarioImporter(
			        const OpenViBE::CIdentifier& importContext,
			        const CString& fileNameExtension
			        );

			virtual OpenViBE::CIdentifier getNextScenarioImportContext(const CIdentifier& importContext) const;
			virtual OpenViBE::CString getNextScenarioImporter(const CIdentifier& importContext, const CString& fileNameExtension) const;
			virtual OpenViBE::CIdentifier getScenarioImporterAlgorithmIdentifier(const CIdentifier& importContext, const CString& fileNameExtension) const;

			virtual bool exportScenario(
			        OpenViBE::IMemoryBuffer& outputMemoryBuffer,
			        const OpenViBE::CIdentifier& scenarioIdentifier,
			        const OpenViBE::CIdentifier& scenarioExporterAlgorithmIdentifier
			        ) const;

			virtual bool exportScenarioToFile(
			        const OpenViBE::CString& fileName,
			        const OpenViBE::CIdentifier& scenarioIdentifier,
			        const OpenViBE::CIdentifier& scenarioExporterAlgorithmIdentifier) const;

			virtual bool exportScenarioToFile(
			        const OpenViBE::CIdentifier& exportContext,
			        const OpenViBE::CString& fileName,
			        const OpenViBE::CIdentifier& scenarioIdentifier);

			virtual bool registerScenarioExporter(
			        const OpenViBE::CIdentifier& exportContext,
			        const OpenViBE::CString& fileNameExtension,
			        const OpenViBE::CIdentifier& scenarioExporterAlgorithmIdentifier);

			virtual bool unregisterScenarioExporter(
			        const OpenViBE::CIdentifier& exportContext,
			        const OpenViBE::CString& fileNameExtension);

			virtual OpenViBE::CIdentifier getNextScenarioExportContext(const CIdentifier& exportContext) const;
			virtual OpenViBE::CString getNextScenarioExporter(const CIdentifier& exportContext, const CString& fileNameExtension) const;
			virtual OpenViBE::CIdentifier getScenarioExporterAlgorithmIdentifier(const CIdentifier& exportContext, const CString& fileNameExtension) const;

			virtual OpenViBE::boolean releaseScenario(
				const OpenViBE::CIdentifier& rScenarioIdentifier);
			virtual OpenViBE::Kernel::IScenario& getScenario(
				const OpenViBE::CIdentifier& rScenarioIdentifier);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IScenarioManager>, OVK_ClassId_Kernel_Scenario_ScenarioManager);

		protected:

			virtual OpenViBE::CIdentifier getUnusedIdentifier(void) const;

		protected:

			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CScenario*> m_vScenario;
		private:
			/// Scenario Import Context -> File Name Extension -> Scenario Importer Identifier
			std::map<OpenViBE::CIdentifier, std::map<std::string, OpenViBE::CIdentifier>> m_ScenarioImporters;
			/// Scenario Export Context -> File Name Extension -> Scenario Exporter Identifier
			std::map<OpenViBE::CIdentifier, std::map<std::string, OpenViBE::CIdentifier>> m_ScenarioExporters;

			virtual OpenViBE::Kernel::IScenario& getScenario(
				const OpenViBE::CIdentifier& rScenarioIdentifier) const;

		};
	}
}

