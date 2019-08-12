#pragma once

#include "ovpCStreamedMatrixEncoder.h"

#define OVP_ClassId_Algorithm_ChannelUnitsStreamEncoder                                                    OpenViBE::CIdentifier(0x2CA034FD, 0x5C051E86)
#define OVP_ClassId_Algorithm_ChannelUnitsStreamEncoderDesc                                                OpenViBE::CIdentifier(0x08696DFC, 0x6D415262)
#define OVP_Algorithm_ChannelUnitsStreamEncoder_InputParameterId_Dynamic                                   OpenViBE::CIdentifier(0x615F03B9, 0x4F6A320A)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CChannelUnitsEncoder : public CStreamedMatrixEncoder
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processHeader() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoder, OVP_ClassId_Algorithm_ChannelUnitsStreamEncoder)

		protected:

			OpenViBE::Kernel::TParameterHandler<bool> ip_bDynamic;
		};


		class CChannelUnitsEncoderDesc : public CStreamedMatrixEncoderDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Channel units stream encoder"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stream codecs/Encoders"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_ChannelUnitsStreamEncoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CChannelUnitsEncoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				CStreamedMatrixEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ChannelUnitsStreamEncoder_InputParameterId_Dynamic, "Dynamic", OpenViBE::Kernel::ParameterType_Boolean);
				// rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ChannelUnitsStreamEncoder_InputParameterId_MeasurementUnits, "Measurement units", OpenViBE::Kernel::ParameterType_Matrix);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoderDesc, OVP_ClassId_Algorithm_ChannelUnitsStreamEncoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
