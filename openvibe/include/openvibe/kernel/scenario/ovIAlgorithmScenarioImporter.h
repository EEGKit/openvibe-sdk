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
		class OV_API IAlgorithmScenarioImporterContext : public IObject
		{
		public:

			virtual bool processStart(const CIdentifier& rIdentifier) = 0;
			virtual bool processIdentifier(const CIdentifier& rIdentifier, const CIdentifier& rValue) = 0;
			virtual bool processString(const CIdentifier& rIdentifier, const CString& rValue) = 0;
			virtual bool processUInteger(const CIdentifier& rIdentifier, const uint64_t ui64Value) = 0;
			virtual bool processStop(void) = 0;

			_IsDerivedFromClass_(OpenViBE::IObject, OV_UndefinedIdentifier)
		};

		class OV_API IAlgorithmScenarioImporter : public IAlgorithm
		{
		public:
			virtual bool import(IAlgorithmScenarioImporterContext& rContext, const IMemoryBuffer& rMemoryBuffer) = 0;
		};


		class OV_API IAlgorithmScenarioImporterDesc : public IAlgorithmDesc
		{
			virtual bool getAlgorithmPrototype(Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				rAlgorithmPrototype.addOutputParameter(OV_Algorithm_ScenarioImporter_OutputParameterId_Scenario, "Scenario", Kernel::ParameterType_Object);
				rAlgorithmPrototype.addInputParameter(OV_Algorithm_ScenarioImporter_InputParameterId_MemoryBuffer, "Memory buffer", Kernel::ParameterType_MemoryBuffer);
				return true;
			}
		};
	}
}
