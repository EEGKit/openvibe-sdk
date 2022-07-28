#pragma once

#include "../../defines.hpp"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <ebml/IReader.h>
#include <ebml/IReaderHelper.h>
#include <ebml/TReaderCallbackProxy.h>

namespace OpenViBE {
namespace Plugins {
namespace StreamCodecs {
class CEBMLBaseDecoder : public Toolkit::TAlgorithm<IAlgorithm>
{
public:
	CEBMLBaseDecoder()
		: m_callbackProxy(*this, &CEBMLBaseDecoder::isMasterChild, &CEBMLBaseDecoder::openChild,
						  &CEBMLBaseDecoder::processChildData, &CEBMLBaseDecoder::closeChild) {}
	void release() override { delete this; }
	bool initialize() override;
	bool uninitialize() override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TAlgorithm<IAlgorithm>, Algorithm_EBMLBaseDecoder)

	// ebml callbacks
	virtual bool isMasterChild(const EBML::CIdentifier& identifier);
	virtual void openChild(const EBML::CIdentifier& identifier);
	virtual void processChildData(const void* /*buffer*/, const size_t /*size*/) { }
	virtual void closeChild() { }

protected:
	EBML::IReaderHelper* m_readerHelper = nullptr;
	EBML::IReader* m_reader             = nullptr;
	EBML::TReaderCallbackProxy1<CEBMLBaseDecoder> m_callbackProxy;

	Kernel::TParameterHandler<CMemoryBuffer*> ip_bufferToDecode;
};

class CEBMLBaseDecoderDesc : public IAlgorithmDesc
{
public:
	bool getAlgorithmPrototype(Kernel::IAlgorithmProto& prototype) const override
	{
		prototype.addInputParameter(EBMLDecoder_InputParameterId_MemoryBufferToDecode, "Memory buffer to decode", Kernel::ParameterType_MemoryBuffer);

		prototype.addOutputTrigger(EBMLDecoder_OutputTriggerId_ReceivedHeader, "Received header");
		prototype.addOutputTrigger(EBMLDecoder_OutputTriggerId_ReceivedBuffer, "Received buffer");
		prototype.addOutputTrigger(EBMLDecoder_OutputTriggerId_ReceivedEnd, "Received end");

		return true;
	}

	_IsDerivedFromClass_(IAlgorithmDesc, Algorithm_EBMLBaseDecoderDesc)
};
}  // namespace StreamCodecs
}  // namespace Plugins
}  // namespace OpenViBE
