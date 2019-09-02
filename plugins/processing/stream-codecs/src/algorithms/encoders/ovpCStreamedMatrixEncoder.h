#pragma once

#include "ovpCEBMLBaseEncoder.h"

#define OVP_ClassId_Algorithm_StreamedMatrixStreamEncoder                                                  OpenViBE::CIdentifier(0x5CB32C71, 0x576F00A6)
#define OVP_ClassId_Algorithm_StreamedMatrixStreamEncoderDesc                                              OpenViBE::CIdentifier(0xEEEFE060, 0x646EE8AB)
#define OVP_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix                                  OpenViBE::CIdentifier(0xA3E9E5B0, 0xAE756303)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CStreamedMatrixEncoder : public CEBMLBaseEncoder
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processHeader() override;
			bool processBuffer() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoder, OVP_ClassId_Algorithm_StreamedMatrixStreamEncoder)

		protected:

			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> ip_pMatrix;

		private:

			uint64_t m_ui64MatrixBufferSize = 0;
		};

		class CStreamedMatrixEncoderDesc : public CEBMLBaseEncoderDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Streamed matrix stream encoder"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stream codecs/Encoders"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_StreamedMatrixStreamEncoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CStreamedMatrixEncoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				CEBMLBaseEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix, "Matrix", OpenViBE::Kernel::ParameterType_Matrix);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc, OVP_ClassId_Algorithm_StreamedMatrixStreamEncoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
