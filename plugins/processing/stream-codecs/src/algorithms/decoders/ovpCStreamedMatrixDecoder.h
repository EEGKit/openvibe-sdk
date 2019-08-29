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
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder, OVP_ClassId_Algorithm_StreamedMatrixStreamDecoder)

			// ebml callbacks
			bool isMasterChild(const EBML::CIdentifier& rIdentifier) override;
			void openChild(const EBML::CIdentifier& rIdentifier) override;
			void processChildData(const void* buffer, uint64_t size) override;
			void closeChild() override;

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

			uint32_t m_ui32Status              = 0;
			uint32_t m_ui32DimensionIndex      = 0;
			uint32_t m_ui32DimensionEntryIndex = 0;
			//			uint32_t m_ui32DimensionEntryIndexUnit = 0;
			uint64_t m_ui64MatrixBufferSize = 0;
		};

		class CStreamedMatrixDecoderDesc : public CEBMLBaseDecoderDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Streamed matrix stream decoder"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stream codecs/Decoders"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_StreamedMatrixStreamDecoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CStreamedMatrixDecoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				CEBMLBaseDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_StreamedMatrixStreamDecoder_OutputParameterId_Matrix, "Matrix", OpenViBE::Kernel::ParameterType_Matrix);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc, OVP_ClassId_Algorithm_StreamedMatrixStreamDecoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
