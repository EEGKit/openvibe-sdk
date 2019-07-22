#pragma once

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

			CStreamedMatrixDecoder();

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder, OVP_ClassId_Algorithm_StreamedMatrixStreamDecoder)

			// ebml callbacks
			virtual bool isMasterChild(const EBML::CIdentifier& rIdentifier);
			virtual void openChild(const EBML::CIdentifier& rIdentifier);
			virtual void processChildData(const void* pBuffer, uint64_t ui64BufferSize);
			virtual void closeChild();

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

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Streamed matrix stream decoder"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Stream codecs/Decoders"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_Algorithm_StreamedMatrixStreamDecoder; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CStreamedMatrixDecoder(); }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CEBMLBaseDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix, "Matrix", OpenViBE::Kernel::ParameterType_Matrix);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc, OVP_ClassId_Algorithm_StreamedMatrixStreamDecoderDesc)
		};
	};
};


