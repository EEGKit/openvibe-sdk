#pragma once

#include "../ovp_defines.h"

#include <toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_StreamDecoderAlgorithmTest                                                OpenViBE::CIdentifier(0x3C2EF355, 0xFE495C3D)
#define OVP_ClassId_BoxAlgorithm_StreamDecoderAlgorithmTestDesc                                            OpenViBE::CIdentifier(0xE5176EB9, 0xD6E47D7F)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CDecoderAlgorithmTest final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			CDecoderAlgorithmTest() { }
			~CDecoderAlgorithmTest() override { }
			void release() override { delete this; }
			bool initialize() override;
			virtual bool uininitialize();
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_StreamDecoderAlgorithmTest)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder[7];
			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_pMemoryBuffer[7];
		};

		class CDecoderAlgorithmTestDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Decoder algorithm test"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }

			OpenViBE::CString getShortDescription() const override
			{
				return OpenViBE::CString("Decodes various types of streams and outputs some of the content parameters the log");
			}

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString("Note: Warnings are normal as the algorithm polls the decoders for structures they may not contain.");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Tests/Algorithms"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_StreamDecoderAlgorithmTest; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CDecoderAlgorithmTest(); }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const override
			{
				rPrototype.addInput("Experiment information", OV_TypeId_ExperimentInformation);
				rPrototype.addInput("Feature vector", OV_TypeId_FeatureVector);
				rPrototype.addInput("Signal", OV_TypeId_Signal);
				rPrototype.addInput("Spectrum", OV_TypeId_Spectrum);
				rPrototype.addInput("Stimulation", OV_TypeId_Stimulations);
				rPrototype.addInput("Streamed matrix", OV_TypeId_StreamedMatrix);
				rPrototype.addInput("Channel localisation", OV_TypeId_ChannelLocalisation);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StreamDecoderAlgorithmTestDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
