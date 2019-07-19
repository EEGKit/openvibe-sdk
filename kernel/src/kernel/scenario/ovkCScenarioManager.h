#pragma once

#include "../ovkTKernelObject.h"

#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CScenario;

		class CScenarioManager final : public TKernelObject<IScenarioManager>
		{
		public:

			explicit CScenarioManager(const IKernelContext& rKernelContext);
			virtual ~CScenarioManager();

			virtual void cloneScenarioImportersAndExporters(const IScenarioManager& scenarioManager);

			virtual CIdentifier getNextScenarioIdentifier(
				const CIdentifier& rPreviousIdentifier) const;

			virtual bool isScenario(const CIdentifier& scenarioIdentifier) const;

			virtual bool createScenario(
				CIdentifier& rScenarioIdentifier);

			virtual bool importScenario(
				CIdentifier& newScenarioIdentifier,
				const IMemoryBuffer& inputMemoryBuffer,
				const CIdentifier& scenarioImporterAlgorithmIdentifier
			);

			virtual bool importScenarioFromFile(
				CIdentifier& newScenarioIdentifier,
				const CString& fileName,
				const CIdentifier& scenarioImporterAlgorithmIdentifier
			);

			virtual bool importScenarioFromFile(
				CIdentifier& newScenarioIdentifier,
				const CIdentifier& importContext,
				const CString& fileName);

			virtual bool registerScenarioImporter(
				const CIdentifier& importContext,
				const CString& fileNameExtension,
				const CIdentifier& scenarioImporterAlgorithmIdentifier
			);

			virtual bool unregisterScenarioImporter(
				const CIdentifier& importContext,
				const CString& fileNameExtension
			);

			virtual CIdentifier getNextScenarioImportContext(const CIdentifier& importContext) const;
			virtual CString getNextScenarioImporter(const CIdentifier& importContext, const CString& fileNameExtension) const;
			virtual CIdentifier getScenarioImporterAlgorithmIdentifier(const CIdentifier& importContext, const CString& fileNameExtension) const;

			virtual bool exportScenario(
				IMemoryBuffer& outputMemoryBuffer,
				const CIdentifier& scenarioIdentifier,
				const CIdentifier& scenarioExporterAlgorithmIdentifier
			) const;

			virtual bool exportScenarioToFile(
				const CString& fileName,
				const CIdentifier& scenarioIdentifier,
				const CIdentifier& scenarioExporterAlgorithmIdentifier) const;

			virtual bool exportScenarioToFile(
				const CIdentifier& exportContext,
				const CString& fileName,
				const CIdentifier& scenarioIdentifier);

			virtual bool registerScenarioExporter(
				const CIdentifier& exportContext,
				const CString& fileNameExtension,
				const CIdentifier& scenarioExporterAlgorithmIdentifier);

			virtual bool unregisterScenarioExporter(
				const CIdentifier& exportContext,
				const CString& fileNameExtension);

			virtual CIdentifier getNextScenarioExportContext(const CIdentifier& exportContext) const;
			virtual CString getNextScenarioExporter(const CIdentifier& exportContext, const CString& fileNameExtension) const;
			virtual CIdentifier getScenarioExporterAlgorithmIdentifier(const CIdentifier& exportContext, const CString& fileNameExtension) const;

			virtual bool releaseScenario(
				const CIdentifier& rScenarioIdentifier);
			virtual IScenario& getScenario(
				const CIdentifier& rScenarioIdentifier);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IScenarioManager>, OVK_ClassId_Kernel_Scenario_ScenarioManager);

		protected:

			virtual CIdentifier getUnusedIdentifier() const;

		protected:

			std::map<CIdentifier, CScenario*> m_vScenario;
		private:
			/// Scenario Import Context -> File Name Extension -> Scenario Importer Identifier
			std::map<CIdentifier, std::map<std::string, CIdentifier>> m_ScenarioImporters;
			/// Scenario Export Context -> File Name Extension -> Scenario Exporter Identifier
			std::map<CIdentifier, std::map<std::string, CIdentifier>> m_ScenarioExporters;

			virtual IScenario& getScenario(
				const CIdentifier& rScenarioIdentifier) const;
		};
	}
}
