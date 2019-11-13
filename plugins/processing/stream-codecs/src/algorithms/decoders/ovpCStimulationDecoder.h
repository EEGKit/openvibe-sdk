#pragma once

#include "../../ovp_defines.h"
#include "ovpCEBMLBaseDecoder.h"
#include <stack>

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CStimulationDecoder final : public CEBMLBaseDecoder
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoder, OVP_ClassId_Algorithm_StimulationDecoder)

			// ebml callbacks
			bool isMasterChild(const EBML::CIdentifier& identifier) override;
			void openChild(const EBML::CIdentifier& identifier) override;
			void processChildData(const void* buffer, const size_t size) override;
			void closeChild() override;

		protected:

			OpenViBE::Kernel::TParameterHandler<OpenViBE::IStimulationSet*> op_pStimulationSet;

		private:

			std::stack<EBML::CIdentifier> m_nodes;

			size_t m_stimulationIdx = 0;
			OpenViBE::CStimulationSet m_stimSet;
		};

		class CStimulationDecoderDesc final : public CEBMLBaseDecoderDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Stimulation stream decoder"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stream codecs/Decoders"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_StimulationDecoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CStimulationDecoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& prototype) const override
			{
				CEBMLBaseDecoderDesc::getAlgorithmPrototype(prototype);

				prototype.addOutputParameter(
					OVP_Algorithm_StimulationDecoder_OutputParameterId_StimulationSet, "Stimulation set", OpenViBE::Kernel::ParameterType_StimulationSet);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseDecoderDesc, OVP_ClassId_Algorithm_StimulationDecoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
