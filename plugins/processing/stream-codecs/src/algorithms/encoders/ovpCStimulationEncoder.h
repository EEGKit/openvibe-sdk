#pragma once

#include "../../ovp_defines.h"
#include "ovpCEBMLBaseEncoder.h"

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CStimulationEncoder final : public CEBMLBaseEncoder
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processBuffer() override;

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoder, OVP_ClassId_Algorithm_StimulationEncoder)

		protected:

			OpenViBE::Kernel::TParameterHandler<OpenViBE::IStimulationSet*> ip_pStimulationSet;
		};

		class CStimulationEncoderDesc final : public CEBMLBaseEncoderDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Stimulation stream encoder"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Stream codecs/Encoders"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_StimulationEncoder; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CStimulationEncoder(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& prototype) const override
			{
				CEBMLBaseEncoderDesc::getAlgorithmPrototype(prototype);

				prototype.addInputParameter(
					OVP_Algorithm_StimulationEncoder_InputParameterId_StimulationSet, "Stimulation set", OpenViBE::Kernel::ParameterType_StimulationSet);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CEBMLBaseEncoderDesc, OVP_ClassId_Algorithm_StimulationEncoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
