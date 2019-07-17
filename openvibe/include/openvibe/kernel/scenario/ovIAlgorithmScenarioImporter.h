#pragma once

#include "../../plugins/ovIAlgorithm.h"
#include "../../plugins/ovIAlgorithmDesc.h"
#include "../algorithm/ovIAlgorithmProto.h"
#include "../../ovIMemoryBuffer.h"

#define OV_Algorithm_ScenarioImporter_OutputParameterId_Scenario    OpenViBE::CIdentifier(0x29574C87, 0x7BA77780)
#define OV_Algorithm_ScenarioImporter_InputParameterId_MemoryBuffer OpenViBE::CIdentifier(0x600463A3, 0x474B7F66)

namespace OpenViBE
{
	namespace Plugins
	{
		class OV_API IAlgorithmScenarioImporterContext : public OpenViBE::IObject
		{
		public:

			virtual bool processStart(const OpenViBE::CIdentifier& rIdentifier) = 0;
			virtual bool processIdentifier(const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::CIdentifier& rValue) = 0;
			virtual bool processString(const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::CString& rValue) = 0;
			virtual bool processUInteger(const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::uint64 ui64Value) = 0;
			virtual bool processStop(void) = 0;

			_IsDerivedFromClass_(OpenViBE::IObject, OV_UndefinedIdentifier)
		};

		class OV_API IAlgorithmScenarioImporter : public OpenViBE::Plugins::IAlgorithm
		{
		public:
			virtual bool import(OpenViBE::Plugins::IAlgorithmScenarioImporterContext& rContext, const OpenViBE::IMemoryBuffer& rMemoryBuffer) = 0;
		};


		class OV_API IAlgorithmScenarioImporterDesc : public OpenViBE::Plugins::IAlgorithmDesc
		{
			virtual bool getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				rAlgorithmPrototype.addOutputParameter(OV_Algorithm_ScenarioImporter_OutputParameterId_Scenario, "Scenario", OpenViBE::Kernel::ParameterType_Object);
				rAlgorithmPrototype.addInputParameter(OV_Algorithm_ScenarioImporter_InputParameterId_MemoryBuffer, "Memory buffer", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				return true;
			}
		};
	}
}
