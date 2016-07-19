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
		class OV_API IAlgorithmScenarioExporter : public OpenViBE::Plugins::IAlgorithm
		{
		public:
			virtual OpenViBE::boolean exportStart(OpenViBE::IMemoryBuffer& rMemoryBuffer, const OpenViBE::CIdentifier& rIdentifier) = 0;
			virtual OpenViBE::boolean exportIdentifier(OpenViBE::IMemoryBuffer& rMemoryBuffer, const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::CIdentifier& rValue) = 0;
			virtual OpenViBE::boolean exportString(OpenViBE::IMemoryBuffer& rMemoryBuffer, const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::CString& rValue) = 0;
			virtual OpenViBE::boolean exportUInteger(OpenViBE::IMemoryBuffer& rMemoryBuffer, const OpenViBE::CIdentifier& rIdentifier, const OpenViBE::uint64 ui64Value) = 0;
			virtual OpenViBE::boolean exportStop(OpenViBE::IMemoryBuffer& rMemoryBuffer) = 0;

			_IsDerivedFromClass_(OpenViBE::Plugins::IAlgorithm, OV_UndefinedIdentifier)
		};

		class OV_API IAlgorithmScenarioExporterDesc : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				rAlgorithmPrototype.addInputParameter (OV_Algorithm_ScenarioExporter_InputParameterId_Scenario,      "Scenario",      OpenViBE::Kernel::ParameterType_Object);
				rAlgorithmPrototype.addOutputParameter(OV_Algorithm_ScenarioExporter_OutputParameterId_MemoryBuffer, "Memory buffer", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				return true;
			}

			_IsDerivedFromClass_(OpenViBE::Plugins::IAlgorithmDesc, OV_UndefinedIdentifier);
		};
	}
}
