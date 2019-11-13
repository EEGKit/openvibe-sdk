#pragma once

#include "../../ovp_defines.h"
#include "ovpCStreamedMatrixEncoder.h"

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CChannelUnitsEncoder final : public CStreamedMatrixEncoder
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processHeader() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoder, OVP_ClassId_Algorithm_ChannelUnitsEncoder)

		protected:

			OpenViBE::Kernel::TParameterHandler<bool> ip_bDynamic;
		};


		class CChannelUnitsEncoderDesc final : public CStreamedMatrixEncoderDesc
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
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_ChannelUnitsEncoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CChannelUnitsEncoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& prototype) const override
			{
				CStreamedMatrixEncoderDesc::getAlgorithmPrototype(prototype);

				prototype.addInputParameter(
					OVP_Algorithm_ChannelUnitsEncoder_InputParameterId_Dynamic, "Dynamic", OpenViBE::Kernel::ParameterType_Boolean);
				// prototype.addInputParameter(OVP_Algorithm_ChannelUnitsEncoder_InputParameterId_MeasurementUnits, "Measurement units", OpenViBE::Kernel::ParameterType_Matrix);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixEncoderDesc, OVP_ClassId_Algorithm_ChannelUnitsEncoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
