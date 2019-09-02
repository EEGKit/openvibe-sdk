#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <ebml/CReader.h>
#include <ebml/CReaderHelper.h>

#include <stack>
#include <map>

#include <cstdio>
#include <cstdlib>

// TODO:
// - please move the identifier definitions in ovp_defines.h
// - please include your desciptor in ovp_main.cpp

#define OVP_ClassId_BoxAlgorithm_GenericStreamReader     OpenViBE::CIdentifier(0x6468099F, 0x0370095A)
#define OVP_ClassId_BoxAlgorithm_GenericStreamReaderDesc OpenViBE::CIdentifier(0x1F1E3A53, 0x6CA07237)

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CBoxAlgorithmGenericStreamReader final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, public EBML::IReaderCallback
		{
		public:

			CBoxAlgorithmGenericStreamReader();
			void release() override { delete this; }
			uint64_t getClockFrequency() override;
			bool initialize() override;
			bool uninitialize() override;
			bool processClock(OpenViBE::CMessageClock& messageClock) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_GenericStreamReader)


		protected:

			OpenViBE::CString m_sFilename;

			EBML::CReader m_oReader;
			EBML::CReaderHelper m_oReaderHelper;

			OpenViBE::CMemoryBuffer m_oSwap;
			OpenViBE::CMemoryBuffer m_oPendingChunk;
			uint64_t m_ui64StartTime   = 0;
			uint64_t m_ui64EndTime     = 0;
			uint32_t m_ui32OutputIndex = 0;
			bool m_bPending            = false;
			bool m_bHasEBMLHeader      = false;

			FILE* m_pFile = nullptr;
			std::stack<EBML::CIdentifier> m_vNodes;
			std::map<uint32_t, uint32_t> m_vStreamIndexToOutputIndex;
			std::map<uint32_t, OpenViBE::CIdentifier> m_vStreamIndexToTypeIdentifier;

		private:
			bool initializeFile();
			bool isMasterChild(const EBML::CIdentifier& rIdentifier) override;
			void openChild(const EBML::CIdentifier& rIdentifier) override;
			void processChildData(const void* buffer, const uint64_t size) override;
			void closeChild() override;
		};

		class CBoxAlgorithmGenericStreamReaderListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			bool check(OpenViBE::Kernel::IBox& box)
			{
				char l_sName[1024];
				for (uint32_t i = 0; i < box.getOutputCount(); i++)
				{
					sprintf(l_sName, "Output stream %u", i + 1);
					box.setOutputName(i, l_sName);
				}
				return true;
			}

			bool onDefaultInitialized(OpenViBE::Kernel::IBox& box) override
			{
				box.setOutputName(0, "Output Signal");
				box.setOutputType(0, OV_TypeId_Signal);
				box.addOutput("Output Stimulations", OV_TypeId_Stimulations);
				return true;
			}

			bool onOutputAdded(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				box.setOutputType(index, OV_TypeId_EBMLStream);
				this->check(box);
				return true;
			}

			bool onOutputRemoved(OpenViBE::Kernel::IBox& box, const uint32_t /*index*/) override
			{
				this->check(box);
				return true;
			}

			bool onOutputTypeChanged(OpenViBE::Kernel::IBox& box, const uint32_t /*index*/) override
			{
				this->check(box);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmGenericStreamReaderDesc final : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Generic stream reader"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Reads OpenViBE streams saved in the .ov format"); }

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString("Generic Stream Writer box can be used to store data in the format read by this box");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("File reading and writing/OpenViBE"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_GenericStreamReader; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmGenericStreamReader; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmGenericStreamReaderListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addOutput("Output stream 1", OV_TypeId_EBMLStream);
				prototype.addSetting("Filename", OV_TypeId_Filename, "");
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddOutput);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_GenericStreamReaderDesc)
		};
	} // namespace FileIO
} // namespace OpenViBEPlugins
