#ifndef __SamplePlugin_Algorithms_CChannelUnitsEncoder_H__
#define __SamplePlugin_Algorithms_CChannelUnitsEncoder_H__

#include "ovpCStreamedMatrixEncoder.h"

#define OVP_ClassId_Algorithm_ChannelUnitsStreamEncoder                                                    OpenViBE::CIdentifier(0x2CA034FD, 0x5C051E86)
#define OVP_ClassId_Algorithm_ChannelUnitsStreamEncoderDesc                                                OpenViBE::CIdentifier(0x08696DFC, 0x6D415262)
#define OVP_Algorithm_ChannelUnitsStreamEncoder_InputParameterId_Dynamic                                   OpenViBE::CIdentifier(0x615F03B9, 0x4F6A320A)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CChannelUnitsEncoder : public OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoder
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processHeader(void);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoder, OVP_ClassId_Algorithm_ChannelUnitsStreamEncoder);

		protected:

			OpenViBE::Kernel::TParameterHandler < OpenViBE::boolean > ip_bDynamic;
		};


		class CChannelUnitsEncoderDesc : public OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Channel units stream encoder"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Stream codecs/Encoders"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CString getSoftwareComponent(void) const   { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const   { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_Algorithm_ChannelUnitsStreamEncoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::StreamCodecs::CChannelUnitsEncoder(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ChannelUnitsStreamEncoder_InputParameterId_Dynamic, "Dynamic", OpenViBE::Kernel::ParameterType_Boolean);
				// rAlgorithmPrototype.addInputParameter(OVP_Algorithm_ChannelUnitsStreamEncoder_InputParameterId_MeasurementUnits, "Measurement units", OpenViBE::Kernel::ParameterType_Matrix);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoderDesc, OVP_ClassId_Algorithm_ChannelUnitsStreamEncoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CChannelUnitsEncoder_H__
