#ifndef __SamplePlugin_Algorithms_CStreamedMatrixDecoder_H__
#define __SamplePlugin_Algorithms_CStreamedMatrixDecoder_H__

#include "ovpCEBMLBaseDecoder.h"

#define OVP_ClassId_Algorithm_StreamedMatrixStreamDecoder                                                  OpenViBE::CIdentifier(0x7359D0DB, 0x91784B21)
#define OVP_ClassId_Algorithm_StreamedMatrixStreamDecoderDesc                                              OpenViBE::CIdentifier(0x384529D5, 0xD8E0A728)
#define OVP_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix                                 OpenViBE::CIdentifier(0x79EF3123, 0x35E3EA4D)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CStreamedMatrixDecoder : public CEBMLBaseDecoder
		{
		public:

			CStreamedMatrixDecoder(void);

			virtual void release(void) { delete this; }

			virtual bool initialize(void);
			virtual bool uninitialize(void);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder, OVP_ClassId_Algorithm_StreamedMatrixStreamDecoder);

			// ebml callbacks
			virtual bool isMasterChild(const EBML::CIdentifier& rIdentifier);
			virtual void openChild(const EBML::CIdentifier& rIdentifier);
			virtual void processChildData(const void* pBuffer, const uint64_t ui64BufferSize);
			virtual void closeChild(void);

		protected:

			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_pMatrix;

		private:

			enum
			{
				Status_ParsingNothing,
				Status_ParsingHeader,
				Status_ParsingBuffer,
				Status_ParsingDimension,
			};

			std::stack<EBML::CIdentifier> m_vNodes;

			uint32_t m_ui32Status;
			uint32_t m_ui32DimensionIndex;
			uint32_t m_ui32DimensionEntryIndex;
			//			uint32_t m_ui32DimensionEntryIndexUnit;
			uint64_t m_ui64MatrixBufferSize;
		};

		class CStreamedMatrixDecoderDesc : public CEBMLBaseDecoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const { return OpenViBE::CString("Streamed matrix stream decoder"); }
			virtual OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const { return OpenViBE::CString("Stream codecs/Decoders"); }
			virtual OpenViBE::CString getVersion(void) const { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CString getSoftwareComponent(void) const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_Algorithm_StreamedMatrixStreamDecoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new CStreamedMatrixDecoder(); }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CEBMLBaseDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix, "Matrix", OpenViBE::Kernel::ParameterType_Matrix);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc, OVP_ClassId_Algorithm_StreamedMatrixStreamDecoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CStreamedMatrixDecoder_H__
