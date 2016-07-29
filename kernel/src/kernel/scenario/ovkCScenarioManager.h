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

			virtual bool exportScenario(
			        OpenViBE::IMemoryBuffer& outputMemoryBuffer,
			        const OpenViBE::CIdentifier& scenarioIdentifier,
			        const OpenViBE::CIdentifier& scenarioExporterAlgorithmIdentifier
			        );

			virtual bool exportScenarioToFile(
			        const char* fileName,
			        const OpenViBE::CIdentifier& scenarioIdentifier,
			        const OpenViBE::CIdentifier& scenarioExporterAlgorithmIdentifier);

			virtual OpenViBE::boolean releaseScenario(
				const OpenViBE::CIdentifier& rScenarioIdentifier);
			virtual OpenViBE::Kernel::IScenario& getScenario(
				const OpenViBE::CIdentifier& rScenarioIdentifier);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IScenarioManager>, OVK_ClassId_Kernel_Scenario_ScenarioManager);

		protected:

			virtual OpenViBE::CIdentifier getUnusedIdentifier(void) const;

		protected:

			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::CScenario*> m_vScenario;

		};
	}
}

