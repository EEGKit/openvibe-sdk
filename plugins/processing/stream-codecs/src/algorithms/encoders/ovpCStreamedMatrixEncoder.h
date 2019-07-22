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

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processHeader();
			virtual bool processBuffer();

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoder, OVP_ClassId_Algorithm_StreamedMatrixStreamEncoder)

		protected:

			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> ip_pMatrix;

		private:

			uint64_t m_ui64MatrixBufferSize;
		};

		class CStreamedMatrixEncoderDesc : public CEBMLBaseEncoderDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Streamed matrix stream encoder"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Stream codecs/Encoders"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_Algorithm_StreamedMatrixStreamEncoder; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CStreamedMatrixEncoder(); }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CEBMLBaseEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_StreamedMatrixStreamEncoder_InputParameterId_Matrix, "Matrix", OpenViBE::Kernel::ParameterType_Matrix);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc, OVP_ClassId_Algorithm_StreamedMatrixStreamEncoderDesc)
		};
	};
};


