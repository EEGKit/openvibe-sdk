#pragma once

#include "ovpCStreamedMatrixDecoder.h"

#define OVP_ClassId_Algorithm_SignalStreamDecoder                                                          OpenViBE::CIdentifier(0x7237C149, 0x0CA66DA7)
#define OVP_ClassId_Algorithm_SignalStreamDecoderDesc                                                      OpenViBE::CIdentifier(0xF1547D89, 0x49FFD0C2)
#define OVP_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate                                   OpenViBE::CIdentifier(0x363D8D79, 0xEEFB912C)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CSignalDecoder : public CStreamedMatrixDecoder
		{
		public:

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoder, OVP_ClassId_Algorithm_SignalStreamDecoder)

			// ebml callbacks
			virtual bool isMasterChild(const EBML::CIdentifier& rIdentifier);
			virtual void openChild(const EBML::CIdentifier& rIdentifier);
			virtual void processChildData(const void* pBuffer, uint64_t ui64BufferSize);
			virtual void closeChild();

		protected:

			OpenViBE::Kernel::TParameterHandler<uint64_t> op_ui64SamplingRate;

		private:

			std::stack<EBML::CIdentifier> m_vNodes;
		};

		class CSignalDecoderDesc : public CStreamedMatrixDecoderDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Signal stream decoder"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Stream codecs/Decoders"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_Algorithm_SignalStreamDecoder; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CSignalDecoder(); }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CStreamedMatrixDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate, "Sampling rate", OpenViBE::Kernel::ParameterType_UInteger);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoderDesc, OVP_ClassId_Algorithm_SignalStreamDecoderDesc)
		};
	};
};


