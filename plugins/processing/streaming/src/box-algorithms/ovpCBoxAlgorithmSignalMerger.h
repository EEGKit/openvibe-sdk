#pragma once

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>
#include <vector>
#include <cstdio>

#define OVP_ClassId_BoxAlgorithm_SignalMerger     OpenViBE::CIdentifier(0x4BF9326F, 0x75603102)
#define OVP_ClassId_BoxAlgorithm_SignalMergerDesc OpenViBE::CIdentifier(0x7A684C44, 0x23BA70A5)

namespace OpenViBEPlugins
{
	namespace Streaming
	{
		class CBoxAlgorithmSignalMerger : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_SignalMerger)

		protected:

			std::vector<OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmSignalMerger>*> m_vStreamDecoder;
			OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmSignalMerger>* m_pStreamEncoder = nullptr;
		};

		class CBoxAlgorithmSignalMergerListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onInputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t index) override
			{
				char l_sInputName[1024];
				sprintf(l_sInputName, "Input %i", index + 1);
				rBox.setInputName(index, l_sInputName);
				rBox.setInputType(index, OV_TypeId_Signal);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmSignalMergerDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Signal Merger"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Merges several input streams into a single output stream"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Streaming"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_SignalMerger; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmSignalMerger; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmSignalMergerListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input 1", OV_TypeId_Signal);
				prototype.addInput("Input 2", OV_TypeId_Signal);
				prototype.addOutput("Merged", OV_TypeId_Signal);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SignalMergerDesc)
		};
	} // namespace Streaming
} // namespace OpenViBEPlugins
