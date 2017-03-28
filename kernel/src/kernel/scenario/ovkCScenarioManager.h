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

			virtual bool cloneScenarioImportersAndExporters(const IScenarioManager& scenarioManager);

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
			        const char* fileName,
			        const OpenViBE::CIdentifier& scenarioImporterAlgorithmIdentifier
			        );

			virtual bool importScenarioFromFile(
			        OpenViBE::CIdentifier& newScenarioIdentifier,
			        const OpenViBE::CIdentifier& importContext,
			        const char* fileName);

			virtual bool registerScenarioImporter(
			        const OpenViBE::CIdentifier& importContext,
			        const char* fileNameExtension,
			        const OpenViBE::CIdentifier& scenarioImporterAlgorithmIdentifier
			        );

			virtual bool unregisterScenarioImporter(
			        const OpenViBE::CIdentifier& importContext,
			        const char* fileNameExtension
			        );

			virtual OpenViBE::uint32 getRegisteredScenarioImportContextsCount() const;
			virtual OpenViBE::uint32 getRegisteredScenarioImportersCount(const OpenViBE::CIdentifier& importContext) const;

			virtual bool getRegisteredScenarioImportContextDetails(
			        OpenViBE::uint32 index,
			        OpenViBE::CIdentifier& importContext
			        ) const;
			virtual bool getRegisteredScenarioImporterDetails(
			        const OpenViBE::CIdentifier& importContext,
			        OpenViBE::uint32 index,
			        const char** fileNameExtension,
			        OpenViBE::CIdentifier& scenarioImporterAlgorithmIdentifier) const;

			virtual bool exportScenario(
			        OpenViBE::IMemoryBuffer& outputMemoryBuffer,
			        const OpenViBE::CIdentifier& scenarioIdentifier,
			        const OpenViBE::CIdentifier& scenarioExporterAlgorithmIdentifier
			        ) const;

			virtual bool exportScenarioToFile(
			        const char* fileName,
			        const OpenViBE::CIdentifier& scenarioIdentifier,
			        const OpenViBE::CIdentifier& scenarioExporterAlgorithmIdentifier) const;

			virtual bool exportScenarioToFile(
			        const OpenViBE::CIdentifier& exportContext,
			        const char* fileName,
			        const OpenViBE::CIdentifier& scenarioIdentifier);

			virtual bool registerScenarioExporter(
			        const OpenViBE::CIdentifier& exportContext,
			        const char* fileNameExtension,
			        const OpenViBE::CIdentifier& scenarioExporterAlgorithmIdentifier);

			virtual bool unregisterScenarioExporter(
			        const OpenViBE::CIdentifier& exportContext,
			        const char* fileNameExtension);

			virtual OpenViBE::uint32 getRegisteredScenarioExportContextsCount() const;
			virtual OpenViBE::uint32 getRegisteredScenarioExportersCount(const OpenViBE::CIdentifier& exportContext) const;

			virtual bool getRegisteredScenarioExportContextDetails(
			        OpenViBE::uint32 index,
			        OpenViBE::CIdentifier& exportContext
			        ) const;
			virtual bool getRegisteredScenarioExporterDetails(
			        const OpenViBE::CIdentifier& exportContext,
			        OpenViBE::uint32 index,
			        const char** fileNameExtension,
			        OpenViBE::CIdentifier& scenarioExporterAlgorithmIdentifier) const;

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

