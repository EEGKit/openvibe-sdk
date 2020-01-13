#pragma once

#include "../../ovp_defines.h"
#include "ovpCStreamedMatrixDecoder.h"
#include <stack>

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CChannelLocalisationDecoder final : public CStreamedMatrixDecoder
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoder, OVP_ClassId_Algorithm_ChannelLocalisationDecoder)

			// ebml callbacks
			bool isMasterChild(const EBML::CIdentifier& identifier) override;
			void openChild(const EBML::CIdentifier& identifier) override;
			void processChildData(const void* buffer, const size_t size) override;
			void closeChild() override;

		protected:

			OpenViBE::Kernel::TParameterHandler<bool> op_bDynamic;

		private:

			std::stack<EBML::CIdentifier> m_nodes;
		};

		class CChannelLocalisationDecoderDesc final : public CStreamedMatrixDecoderDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Channel localisation stream decoder"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stream codecs/Decoders"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_ChannelLocalisationDecoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CChannelLocalisationDecoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& prototype) const override
			{
				CStreamedMatrixDecoderDesc::getAlgorithmPrototype(prototype);

				prototype.addOutputParameter(OVP_Algorithm_ChannelLocalisationDecoder_OutputParameterId_Dynamic, "Dynamic", OpenViBE::Kernel::ParameterType_Boolean);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoderDesc, OVP_ClassId_Algorithm_ChannelLocalisationDecoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
