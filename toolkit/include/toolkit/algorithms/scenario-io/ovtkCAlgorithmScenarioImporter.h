#pragma once

#include "../ovtkTAlgorithm.h"
#include "ovtk_scenario_io.h"
#include <openvibe/kernel/scenario/ovIAlgorithmScenarioImporter.h>

#define OVP_ClassId_Algorithm_ScenarioImporter     OpenViBE::CIdentifier(0x1EE72169, 0x2BF146C1)
#define OVP_ClassId_Algorithm_ScenarioImporterDesc OpenViBE::CIdentifier(0x503C0DDE, 0x5D4C6CB2)

namespace OpenViBEToolkit
{
	class OVTK_API CAlgorithmScenarioImporter : public TAlgorithm<OpenViBE::Plugins::IAlgorithmScenarioImporter>
	{
	public:

		virtual void release() { delete this; }

		virtual bool process();

		_IsDerivedFromClass_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithmScenarioImporter >, OVP_ClassId_Algorithm_ScenarioImporter)
	};

	class OVTK_API CAlgorithmScenarioImporterDesc : public OpenViBE::Plugins::IAlgorithmScenarioImporterDesc
	{
	public:

		_IsDerivedFromClass_(OpenViBE::Plugins::IAlgorithmScenarioImporterDesc, OVP_ClassId_Algorithm_ScenarioImporterDesc)
	};
}  // namespace OpenViBEToolkit
