#ifndef __SamplePlugin_Algorithms_CChannelUnitsDecoder_H__
#define __SamplePlugin_Algorithms_CChannelUnitsDecoder_H__

#include "ovpCStreamedMatrixDecoder.h"

#define OVP_ClassId_Algorithm_ChannelUnitsStreamDecoder                                             OpenViBE::CIdentifier(0x5F973DDF, 0x4A582DAF)
#define OVP_ClassId_Algorithm_ChannelUnitsStreamDecoderDesc                                         OpenViBE::CIdentifier(0x2D59257D, 0x3B1915DA)
#define OVP_Algorithm_ChannelUnitsStreamDecoder_OutputParameterId_Dynamic                           OpenViBE::CIdentifier(0x31CF1C7A, 0x17475323)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CChannelUnitsDecoder : public OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoder
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoder, OVP_ClassId_Algorithm_ChannelUnitsStreamDecoder);

			// ebml callbacks
			virtual EBML::boolean isMasterChild(const EBML::CIdentifier& rIdentifier);
			virtual void openChild(const EBML::CIdentifier& rIdentifier);
			virtual void processChildData(const void* pBuffer, const EBML::uint64 ui64BufferSize);
			virtual void closeChild(void);

		protected:

			OpenViBE::Kernel::TParameterHandler<OpenViBE::boolean> op_bDynamic;

		private:

			std::stack<EBML::CIdentifier> m_vNodes;
			//			OpenViBE::uint32 m_ui32UnitIndex;
		};

		class CChannelUnitsDecoderDesc : public OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const { return OpenViBE::CString("Channel units stream decoder"); }
			virtual OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const { return OpenViBE::CString("Stream codecs/Decoders"); }
			virtual OpenViBE::CString getVersion(void) const { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CString getSoftwareComponent(void) const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_Algorithm_ChannelUnitsStreamDecoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new OpenViBEPlugins::StreamCodecs::CChannelUnitsDecoder(); }

			virtual OpenViBE::boolean getAlgorithmPrototype(
				OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_ChannelUnitsStreamDecoder_OutputParameterId_Dynamic, "Dynamic", OpenViBE::Kernel::ParameterType_Boolean);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoderDesc, OVP_ClassId_Algorithm_ChannelUnitsStreamDecoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CChannelUnitsDecoder_H__
