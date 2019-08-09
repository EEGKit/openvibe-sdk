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
			~CScenarioManager() override;
			void cloneScenarioImportersAndExporters(const IScenarioManager& scenarioManager) override;
			CIdentifier getNextScenarioIdentifier(const CIdentifier& rPreviousIdentifier) const override;
			bool isScenario(const CIdentifier& scenarioIdentifier) const override;
			bool createScenario(CIdentifier& rScenarioIdentifier) override;
			bool importScenario(CIdentifier& newScenarioIdentifier, const IMemoryBuffer& inputMemoryBuffer, const CIdentifier& scenarioImporterAlgorithmIdentifier ) override;
			bool importScenarioFromFile(CIdentifier& newScenarioIdentifier, const CString& fileName, const CIdentifier& scenarioImporterAlgorithmIdentifier) override;
			bool importScenarioFromFile(CIdentifier& newScenarioIdentifier, const CIdentifier& importContext, const CString& fileName) override;
			bool registerScenarioImporter(const CIdentifier& importContext, const CString& fileNameExtension, const CIdentifier& scenarioImporterAlgorithmIdentifier) override;
			bool unregisterScenarioImporter(const CIdentifier& importContext, const CString& fileNameExtension) override;
			CIdentifier getNextScenarioImportContext(const CIdentifier& importContext) const override;
			CString getNextScenarioImporter(const CIdentifier& importContext, const CString& fileNameExtension) const override;
			CIdentifier getScenarioImporterAlgorithmIdentifier(const CIdentifier& importContext, const CString& fileNameExtension) const override;
			bool exportScenario(IMemoryBuffer& outputMemoryBuffer, const CIdentifier& scenarioIdentifier, const CIdentifier& scenarioExporterAlgorithmIdentifier) const override;
			bool exportScenarioToFile(const CString& fileName, const CIdentifier& scenarioIdentifier, const CIdentifier& scenarioExporterAlgorithmIdentifier) const override;
			bool exportScenarioToFile(const CIdentifier& exportContext, const CString& fileName, const CIdentifier& scenarioIdentifier) override;
			bool registerScenarioExporter(const CIdentifier& exportContext, const CString& fileNameExtension, const CIdentifier& scenarioExporterAlgorithmIdentifier) override;
			bool unregisterScenarioExporter(const CIdentifier& exportContext, const CString& fileNameExtension) override;
			CIdentifier getNextScenarioExportContext(const CIdentifier& exportContext) const override;
			CString getNextScenarioExporter(const CIdentifier& exportContext, const CString& fileNameExtension) const override;
			CIdentifier getScenarioExporterAlgorithmIdentifier(const CIdentifier& exportContext, const CString& fileNameExtension) const override;
			bool releaseScenario(const CIdentifier& rScenarioIdentifier) override;
			IScenario& getScenario(const CIdentifier& rScenarioIdentifier) override;

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IScenarioManager>, OVK_ClassId_Kernel_Scenario_ScenarioManager)

		protected:

			virtual CIdentifier getUnusedIdentifier() const;

			std::map<CIdentifier, CScenario*> m_vScenario;
		private:
			/// Scenario Import Context -> File Name Extension -> Scenario Importer Identifier
			std::map<CIdentifier, std::map<std::string, CIdentifier>> m_ScenarioImporters;
			/// Scenario Export Context -> File Name Extension -> Scenario Exporter Identifier
			std::map<CIdentifier, std::map<std::string, CIdentifier>> m_ScenarioExporters;

			virtual IScenario& getScenario(const CIdentifier& rScenarioIdentifier) const;
		};
	}  // namespace Kernel
}  // namespace OpenViBE
