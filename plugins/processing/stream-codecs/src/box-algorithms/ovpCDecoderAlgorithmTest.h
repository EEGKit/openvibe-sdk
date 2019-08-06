#pragma once

#include "../ovp_defines.h"

#include <toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_StreamDecoderAlgorithmTest                                                OpenViBE::CIdentifier(0x3C2EF355, 0xFE495C3D)
#define OVP_ClassId_BoxAlgorithm_StreamDecoderAlgorithmTestDesc                                            OpenViBE::CIdentifier(0xE5176EB9, 0xD6E47D7F)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CDecoderAlgorithmTest : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			CDecoderAlgorithmTest();
			virtual ~CDecoderAlgorithmTest();

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uininitialize();

			virtual bool processInput(const uint32_t ui32InputIndex);
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_StreamDecoderAlgorithmTest)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder[7];
			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_pMemoryBuffer[7];
		};

		class CDecoderAlgorithmTestDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }
			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Decoder algorithm test"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Decodes various types of streams and outputs some of the content parameters the log"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString("Note: Warnings are normal as the algorithm polls the decoders for structures they may not contain."); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Tests/Algorithms"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_StreamDecoderAlgorithmTest; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CDecoderAlgorithmTest(); }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
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
	};
};
