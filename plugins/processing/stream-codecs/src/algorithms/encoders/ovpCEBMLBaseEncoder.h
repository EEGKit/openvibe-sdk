#ifndef __SamplePlugin_Algorithms_CEBMLBaseEncoder_H__
#define __SamplePlugin_Algorithms_CEBMLBaseEncoder_H__

#include <openvibe/ov_all.h>

#include <toolkit/ovtk_all.h>

#include <ebml/IWriter.h>
#include <ebml/IWriterHelper.h>
#include <ebml/TWriterCallbackProxy.h>

#define OVP_ClassId_Algorithm_EBMLBaseStreamEncoder                                                        OpenViBE::CIdentifier(0x4272C178, 0x3FE84927)
#define OVP_ClassId_Algorithm_EBMLBaseStreamEncoderDesc                                                    OpenViBE::CIdentifier(0x47A9E701, 0x7C57BF3C)
#define OVP_Algorithm_EBMLStreamEncoder_OutputParameterId_EncodedMemoryBuffer                              OpenViBE::CIdentifier(0xA3D8B171, 0xF8734734)
#define OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeHeader                                        OpenViBE::CIdentifier(0x878EAF60, 0xF9D5303F)
#define OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeBuffer                                        OpenViBE::CIdentifier(0x1B7076FD, 0x449BC70A)
#define OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeEnd                                           OpenViBE::CIdentifier(0x3FC23508, 0x806753D8)
#define OVP_Algorithm_EBMLStreamEncoder_OutputTriggerId_MemoryBufferUpdated                                OpenViBE::CIdentifier(0xD46C7462, 0xD3407E5F)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CEBMLBaseEncoder : public OpenViBEToolkit::TAlgorithm<OpenViBE::Plugins::IAlgorithm>
		{
		public:

			CEBMLBaseEncoder(void);

			virtual void release(void) { delete this; }

			virtual bool initialize(void);
			virtual bool uninitialize(void);
			virtual bool process(void);
			virtual bool processHeader(void) { return true; }
			virtual bool processBuffer(void) { return true; }
			virtual bool processEnd(void) { return true; }

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_EBMLBaseStreamEncoder);

			// ebml callbacks
			virtual void write(const void* pBuffer, uint64_t ui64BufferSize);

		protected:

			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pMemoryBuffer;

			EBML::IWriterHelper* m_pEBMLWriterHelper;
			EBML::IWriter* m_pEBMLWriter;
			EBML::TWriterCallbackProxy1<CEBMLBaseEncoder> m_oEBMLWriterCallbackProxy;
		};

		class CEBMLBaseEncoderDesc : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_EBMLStreamEncoder_OutputParameterId_EncodedMemoryBuffer, "Encoded memory buffer", OpenViBE::Kernel::ParameterType_MemoryBuffer);
				rAlgorithmPrototype.addInputTrigger(OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeHeader, "Encode header");
				rAlgorithmPrototype.addInputTrigger(OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeBuffer, "Encode buffer");
				rAlgorithmPrototype.addInputTrigger(OVP_Algorithm_EBMLStreamEncoder_InputTriggerId_EncodeEnd, "Encode end");

				return true;
			}

			_IsDerivedFromClass_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_EBMLBaseStreamEncoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CEBMLBaseEncoder_H__
