#pragma once

#include "ovpCStreamedMatrixEncoder.h"

#define OVP_ClassId_Algorithm_SignalStreamEncoder                                                          OpenViBE::CIdentifier(0xC488AD3C, 0xEB2E36BF)
#define OVP_ClassId_Algorithm_SignalStreamEncoderDesc                                                      OpenViBE::CIdentifier(0x90AC1E0F, 0x01518200)
#define OVP_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate                                    OpenViBE::CIdentifier(0x998710FF, 0x2C5CCA82)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CSignalEncoder : public CStreamedMatrixEncoder
		{
		public:

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processHeader();

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoder, OVP_ClassId_Algorithm_SignalStreamEncoder)

		protected:

			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64SamplingRate;
		};


		class CSignalEncoderDesc : public CStreamedMatrixEncoderDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Signal stream encoder"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Stream codecs/Encoders"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_Algorithm_SignalStreamEncoder; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CSignalEncoder(); }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CStreamedMatrixEncoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_SignalStreamEncoder_InputParameterId_SamplingRate, "Sampling rate", OpenViBE::Kernel::ParameterType_UInteger);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoderDesc, OVP_ClassId_Algorithm_SignalStreamEncoderDesc)
		};
	};
};


