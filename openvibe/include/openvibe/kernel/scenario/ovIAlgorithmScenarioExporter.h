#pragma once

#include "../../plugins/ovIAlgorithm.h"
#include "../../plugins/ovIAlgorithmDesc.h"
#include "../algorithm/ovIAlgorithmProto.h"
#include "../../ovIMemoryBuffer.h"

#define OV_Algorithm_ScenarioExporter_InputParameterId_Scenario      OpenViBE::CIdentifier(0x5B9C0D54, 0x04BA2957)
#define OV_Algorithm_ScenarioExporter_OutputParameterId_MemoryBuffer OpenViBE::CIdentifier(0x64030633, 0x419E3A33)

namespace OpenViBE
{
	namespace Plugins
	{
		class OV_API IAlgorithmScenarioExporter : public IAlgorithm
		{
		public:
			virtual bool exportStart(IMemoryBuffer& rMemoryBuffer, const CIdentifier& rIdentifier) = 0;
			virtual bool exportIdentifier(IMemoryBuffer& rMemoryBuffer, const CIdentifier& rIdentifier, const CIdentifier& rValue) = 0;
			virtual bool exportString(IMemoryBuffer& rMemoryBuffer, const CIdentifier& rIdentifier, const CString& rValue) = 0;
			virtual bool exportUInteger(IMemoryBuffer& rMemoryBuffer, const CIdentifier& rIdentifier, uint64_t ui64Value) = 0;
			virtual bool exportStop(IMemoryBuffer& rMemoryBuffer) = 0;

			_IsDerivedFromClass_(OpenViBE::Plugins::IAlgorithm, OV_UndefinedIdentifier)
		};

		class OV_API IAlgorithmScenarioExporterDesc : public IAlgorithmDesc
		{
		public:

			virtual bool getAlgorithmPrototype(Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				rAlgorithmPrototype.addInputParameter(OV_Algorithm_ScenarioExporter_InputParameterId_Scenario, "Scenario", Kernel::ParameterType_Object);
				rAlgorithmPrototype.addOutputParameter(OV_Algorithm_ScenarioExporter_OutputParameterId_MemoryBuffer, "Memory buffer", Kernel::ParameterType_MemoryBuffer);
				return true;
			}

			_IsDerivedFromClass_(OpenViBE::Plugins::IAlgorithmDesc, OV_UndefinedIdentifier);
		};
	}
}
