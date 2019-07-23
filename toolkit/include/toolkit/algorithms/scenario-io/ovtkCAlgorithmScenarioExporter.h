#pragma once

#include "../ovtkTAlgorithm.h"
#include "ovtk_scenario_io.h"

#include <openvibe/kernel/scenario/ovIAlgorithmScenarioExporter.h>

#define OVTK_ClassId_Algorithm_ScenarioExporter     OpenViBE::CIdentifier(0x7C281FEA, 0x40B66277)
#define OVTK_ClassId_Algorithm_ScenarioExporterDesc OpenViBE::CIdentifier(0x49A9778E, 0x7BB377F9)

namespace OpenViBEToolkit
{
	class OVTK_API CAlgorithmScenarioExporter : public TAlgorithm<OpenViBE::Plugins::IAlgorithmScenarioExporter>
	{
	public:

		virtual void release() { delete this; }

		virtual bool process();

		_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithmScenarioExporter >, OVTK_ClassId_Algorithm_ScenarioExporter)
	};

	class OVTK_API CAlgorithmScenarioExporterDesc : public OpenViBE::Plugins::IAlgorithmScenarioExporterDesc
	{
	public:

		_IsDerivedFromClass_(OpenViBE::Plugins::IAlgorithmDesc, OVTK_ClassId_Algorithm_ScenarioExporterDesc)
	};
}  // namespace OpenViBEToolkit
