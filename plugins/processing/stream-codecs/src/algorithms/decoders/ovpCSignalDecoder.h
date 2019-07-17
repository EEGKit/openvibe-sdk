#ifndef __SamplePlugin_Algorithms_CSignalDecoder_H__
#define __SamplePlugin_Algorithms_CSignalDecoder_H__

#include "ovpCStreamedMatrixDecoder.h"

#define OVP_ClassId_Algorithm_SignalStreamDecoder                                                          OpenViBE::CIdentifier(0x7237C149, 0x0CA66DA7)
#define OVP_ClassId_Algorithm_SignalStreamDecoderDesc                                                      OpenViBE::CIdentifier(0xF1547D89, 0x49FFD0C2)
#define OVP_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate                                   OpenViBE::CIdentifier(0x363D8D79, 0xEEFB912C)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CSignalDecoder : public OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoder
		{
		public:

			virtual void release(void) { delete this; }

			virtual bool initialize(void);
			virtual bool uninitialize(void);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoder, OVP_ClassId_Algorithm_SignalStreamDecoder);

			// ebml callbacks
			virtual bool isMasterChild(const EBML::CIdentifier& rIdentifier);
			virtual void openChild(const EBML::CIdentifier& rIdentifier);
			virtual void processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
			virtual void closeChild(void);

		protected:

			OpenViBE::Kernel::TParameterHandler<OpenViBE::uint64> op_ui64SamplingRate;

		private:

			std::stack<EBML::CIdentifier> m_vNodes;
		};

		class CSignalDecoderDesc : public OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const { return OpenViBE::CString("Signal stream decoder"); }
			virtual OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const { return OpenViBE::CString("Stream codecs/Decoders"); }
			virtual OpenViBE::CString getVersion(void) const { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CString getSoftwareComponent(void) const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_Algorithm_SignalStreamDecoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new OpenViBEPlugins::StreamCodecs::CSignalDecoder(); }

			virtual bool getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_SignalStreamDecoder_OutputParameterId_SamplingRate, "Sampling rate", OpenViBE::Kernel::ParameterType_UInteger);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoderDesc, OVP_ClassId_Algorithm_SignalStreamDecoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CSignalDecoder_H__
