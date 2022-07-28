#pragma once

#include "../../defines.hpp"
#include "CEBMLBaseEncoder.hpp"

namespace OpenViBE {
namespace Plugins {
namespace StreamCodecs {
class CStimulationEncoder final : public CEBMLBaseEncoder
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processBuffer() override;

	_IsDerivedFromClass_Final_(StreamCodecs::CEBMLBaseEncoder, Algorithm_StimulationEncoder)

protected:
	Kernel::TParameterHandler<CStimulationSet*> ip_stimSet;
};

class CStimulationEncoderDesc final : public CEBMLBaseEncoderDesc
{
public:
	void release() override { }

	CString getName() const override { return "Stimulation stream encoder"; }
	CString getAuthorName() const override { return "Yann Renard"; }
	CString getAuthorCompanyName() const override { return "INRIA/IRISA"; }
	CString getShortDescription() const override { return ""; }
	CString getDetailedDescription() const override { return ""; }
	CString getCategory() const override { return "Stream codecs/Encoders"; }
	CString getVersion() const override { return "1.0"; }

	CIdentifier getCreatedClass() const override { return Algorithm_StimulationEncoder; }
	IPluginObject* create() override { return new CStimulationEncoder(); }

	bool getAlgorithmPrototype(Kernel::IAlgorithmProto& prototype) const override
	{
		CEBMLBaseEncoderDesc::getAlgorithmPrototype(prototype);
		prototype.addInputParameter(StimulationEncoder_InputParameterId_StimulationSet, "Stimulation set", Kernel::ParameterType_StimulationSet);
		return true;
	}

	_IsDerivedFromClass_Final_(StreamCodecs::CEBMLBaseEncoderDesc, Algorithm_StimulationEncoderDesc)
};
}  // namespace StreamCodecs
}  // namespace Plugins
}  // namespace OpenViBE
