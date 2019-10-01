#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <ebml/IWriter.h>
#include <ebml/IWriterHelper.h>
#include <ebml/TWriterCallbackProxy.h>

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CEBMLBaseEncoder : public OpenViBEToolkit::TAlgorithm<OpenViBE::Plugins::IAlgorithm>
		{
		public:

			CEBMLBaseEncoder() : m_oEBMLWriterCallbackProxy(*this, &CEBMLBaseEncoder::write) {}
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool process() override;
			virtual bool processHeader() { return true; }
			virtual bool processBuffer() { return true; }
			virtual bool processEnd() { return true; }

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_EBMLBaseStreamEncoder)

			// ebml callbacks
			virtual void write(const void* buffer, uint64_t size);

		protected:

			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pMemoryBuffer;

			EBML::IWriterHelper* m_pEBMLWriterHelper = nullptr;
			EBML::IWriter* m_pEBMLWriter             = nullptr;
			EBML::TWriterCallbackProxy1<CEBMLBaseEncoder> m_oEBMLWriterCallbackProxy;
		};

		class CEBMLBaseEncoderDesc : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:
			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				rAlgorithmPrototype.addOutputParameter(
					OVP_Algorithm_EBMLStreamEncoder_OutputParameterId_EncodedMemoryBuffer, "Encoded memory buffer",
					OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addInputTrigger(OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeHeader, "Encode header");
				rAlgorithmPrototype.addInputTrigger(OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeBuffer, "Encode buffer");
				rAlgorithmPrototype.addInputTrigger(OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeEnd, "Encode end");

				return true;
			}

			_IsDerivedFromClass_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_EBMLBaseStreamEncoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
