#ifndef __SamplePlugin_Algorithms_CStimulationDecoder_H__
#define __SamplePlugin_Algorithms_CStimulationDecoder_H__

#include "ovpCEBMLBaseDecoder.h"

#define OVP_ClassId_Algorithm_StimulationStreamDecoder                                                     OpenViBE::CIdentifier(0xC8807F2B, 0x0813C5B1)
#define OVP_ClassId_Algorithm_StimulationStreamDecoderDesc                                                 OpenViBE::CIdentifier(0x391A615B, 0x71CD888A)
#define OVP_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet                            OpenViBE::CIdentifier(0xF46D0C19, 0x47306BEA)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CStimulationDecoder : public CEBMLBaseDecoder
		{
		public:

			virtual void release(void) { delete this; }

			virtual bool initialize(void);
			virtual bool uninitialize(void);

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder, OVP_ClassId_Algorithm_StimulationStreamDecoder);

			// ebml callbacks
			virtual bool isMasterChild(const EBML::CIdentifier& rIdentifier);
			virtual void openChild(const EBML::CIdentifier& rIdentifier);
			virtual void processChildData(const void* pBuffer, const uint64_t ui64BufferSize);
			virtual void closeChild(void);

		protected:

			OpenViBE::Kernel::TParameterHandler<OpenViBE::IStimulationSet*> op_pStimulationSet;

		private:

			std::stack<EBML::CIdentifier> m_vNodes;

			uint64_t m_ui64StimulationIndex;
			OpenViBE::CStimulationSet m_oStimulationSet;
		};

		class CStimulationDecoderDesc : public CEBMLBaseDecoderDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const { return OpenViBE::CString("Stimulation stream decoder"); }
			virtual OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const { return OpenViBE::CString("Stream codecs/Decoders"); }
			virtual OpenViBE::CString getVersion(void) const { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CString getSoftwareComponent(void) const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_Algorithm_StimulationStreamDecoder; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new CStimulationDecoder(); }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CEBMLBaseDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_StimulationStreamDecoder_OutputParameterId_StimulationSet, "Stimulation set", OpenViBE::Kernel::ParameterType_StimulationSet);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc, OVP_ClassId_Algorithm_StimulationStreamDecoderDesc);
		};
	};
};

#endif // __SamplePlugin_Algorithms_CStimulationDecoder_H__
