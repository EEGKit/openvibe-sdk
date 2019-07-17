#ifndef __OpenViBEPlugins_BoxAlgorithm_SignalMerger_H__
#define __OpenViBEPlugins_BoxAlgorithm_SignalMerger_H__

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

			virtual void release(void) { delete this; }

			virtual bool initialize(void);
			virtual bool uninitialize(void);
			virtual bool processInput(OpenViBE::uint32 ui32InputIndex);
			virtual bool process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_SignalMerger);

		protected:

			std::vector<OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmSignalMerger>*> m_vStreamDecoder;
			OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmSignalMerger>* m_pStreamEncoder;
		};

		class CBoxAlgorithmSignalMergerListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			//			virtual bool initialize(void) { return true; }
			//			virtual bool uninitialize(void) { return true; }
			//			virtual bool onInitialized(OpenViBE::Kernel::IBox& rBox) { return true; };
			//			virtual bool onNameChanged(OpenViBE::Kernel::IBox& rBox) { return true; };
			//			virtual bool onIdentifierChanged(OpenViBE::Kernel::IBox& rBox) { return true; };
			//			virtual bool onAlgorithmClassIdentifierChanged(OpenViBE::Kernel::IBox& rBox) { return true; };
			//			virtual bool onInputConnected(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//			virtual bool onInputDisconnected(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			virtual bool onInputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				char l_sInputName[1024];
				::sprintf(l_sInputName, "Input %i", ui32Index + 1);
				rBox.setInputName(ui32Index, l_sInputName);
				rBox.setInputType(ui32Index, OV_TypeId_Signal);
				return true;
			}
			//			virtual bool onInputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//			virtual bool onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//			virtual bool onInputNameChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//			virtual bool onOutputConnected(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//			virtual bool onOutputDisconnected(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//			virtual bool onOutputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//			virtual bool onOutputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//			virtual bool onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//			virtual bool onOutputNameChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//			virtual bool onSettingAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//			virtual bool onSettingRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//			virtual bool onSettingTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//			virtual bool onSettingNameChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//			virtual bool onSettingDefaultValueChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };
			//			virtual bool onSettingValueChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index) { return true; };

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CBoxAlgorithmSignalMergerDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const { return OpenViBE::CString("Signal Merger"); }
			virtual OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString("Merges several input streams into a single output stream"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const { return OpenViBE::CString("Streaming"); }
			virtual OpenViBE::CString getVersion(void) const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent(void) const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_BoxAlgorithm_SignalMerger; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new OpenViBEPlugins::Streaming::CBoxAlgorithmSignalMerger; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const { return new CBoxAlgorithmSignalMergerListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual bool getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Input 1", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addInput("Input 2", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput("Merged", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SignalMergerDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_SignalMerger_H__
