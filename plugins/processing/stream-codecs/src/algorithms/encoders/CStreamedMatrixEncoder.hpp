#pragma once

#include "../../defines.hpp"
#include "CEBMLBaseEncoder.hpp"

namespace OpenViBE {
namespace Plugins {
namespace StreamCodecs {
class CStreamedMatrixEncoder : public CEBMLBaseEncoder
{
public:
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool processHeader() override;
	bool processBuffer() override;

	_IsDerivedFromClass_Final_(StreamCodecs::CEBMLBaseEncoder, Algorithm_StreamedMatrixEncoder)

protected:
	Kernel::TParameterHandler<CMatrix*> ip_pMatrix;

private:
	size_t m_size = 0;
};

class CStreamedMatrixEncoderDesc : public CEBMLBaseEncoderDesc
{
public:
	void release() override { }

	CString getName() const override { return "Streamed matrix stream encoder"; }
	CString getAuthorName() const override { return "Yann Renard"; }
	CString getAuthorCompanyName() const override { return "INRIA/IRISA"; }
	CString getShortDescription() const override { return ""; }
	CString getDetailedDescription() const override { return ""; }
	CString getCategory() const override { return "Stream codecs/Encoders"; }
	CString getVersion() const override { return "1.0"; }

	CIdentifier getCreatedClass() const override { return Algorithm_StreamedMatrixEncoder; }
	IPluginObject* create() override { return new CStreamedMatrixEncoder(); }

	bool getAlgorithmPrototype(Kernel::IAlgorithmProto& prototype) const override
	{
		CEBMLBaseEncoderDesc::getAlgorithmPrototype(prototype);
		prototype.addInputParameter(StreamedMatrixEncoder_InputParameterId_Matrix, "Matrix", Kernel::ParameterType_Matrix);
		return true;
	}

	_IsDerivedFromClass_Final_(StreamCodecs::CEBMLBaseEncoderDesc, Algorithm_StreamedMatrixEncoderDesc)
};
}  // namespace StreamCodecs
}  // namespace Plugins
}  // namespace OpenViBE
