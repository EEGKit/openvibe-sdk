#pragma once

#include "../../ovp_defines.h"
#include "ovpCStreamedMatrixDecoder.h"
#include <stack>

namespace OpenViBE
{
	namespace Plugins
	{
		namespace StreamCodecs
		{
			class CChannelLocalisationDecoder final : public CStreamedMatrixDecoder
			{
			public:
				void release() override { delete this; }
				bool initialize() override;
				bool uninitialize() override;

				_IsDerivedFromClass_Final_(StreamCodecs::CStreamedMatrixDecoder, OVP_ClassId_Algorithm_ChannelLocalisationDecoder)

				// ebml callbacks
				bool isMasterChild(const EBML::CIdentifier& identifier) override;
				void openChild(const EBML::CIdentifier& identifier) override;
				void processChildData(const void* buffer, const size_t size) override;
				void closeChild() override;

			protected:

				Kernel::TParameterHandler<bool> op_bDynamic;

			private:

				std::stack<EBML::CIdentifier> m_nodes;
			};

			class CChannelLocalisationDecoderDesc final : public CStreamedMatrixDecoderDesc
			{
			public:
				void release() override { }
				CString getName() const override { return CString("Channel localisation stream decoder"); }
				CString getAuthorName() const override { return CString("Yann Renard"); }
				CString getAuthorCompanyName() const override { return CString("INRIA/IRISA"); }
				CString getShortDescription() const override { return CString(""); }
				CString getDetailedDescription() const override { return CString(""); }
				CString getCategory() const override { return CString("Stream codecs/Decoders"); }
				CString getVersion() const override { return CString("1.0"); }
				CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
				CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
				CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
				CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_ChannelLocalisationDecoder; }
				IPluginObject* create() override { return new CChannelLocalisationDecoder(); }

				bool getAlgorithmPrototype(Kernel::IAlgorithmProto& prototype) const override
				{
					CStreamedMatrixDecoderDesc::getAlgorithmPrototype(prototype);

					prototype.addOutputParameter(OVP_Algorithm_ChannelLocalisationDecoder_OutputParameterId_Dynamic, "Dynamic", Kernel::ParameterType_Boolean);

					return true;
				}

				_IsDerivedFromClass_Final_(StreamCodecs::CStreamedMatrixDecoderDesc, OVP_ClassId_Algorithm_ChannelLocalisationDecoderDesc)
			};
		} // namespace StreamCodecs
	}  // namespace Plugins
}  // namespace OpenViBE
