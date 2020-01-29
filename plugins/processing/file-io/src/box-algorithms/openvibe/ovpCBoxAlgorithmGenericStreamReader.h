#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <ebml/CReader.h>
#include <ebml/CReaderHelper.h>

#include <stack>
#include <map>

#include <cstdio>

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CBoxAlgorithmGenericStreamReader final : public OpenViBE::Toolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, public EBML::IReaderCallback
		{
		public:

			CBoxAlgorithmGenericStreamReader();
			void release() override { delete this; }
			uint64_t getClockFrequency() override;
			bool initialize() override;
			bool uninitialize() override;
			bool processClock(OpenViBE::CMessageClock& messageClock) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_GenericStreamReader)


		protected:

			OpenViBE::CString m_filename;

			EBML::CReader m_reader;
			EBML::CReaderHelper m_readerHelper;

			OpenViBE::CMemoryBuffer m_swap;
			OpenViBE::CMemoryBuffer m_pendingChunk;
			uint64_t m_startTime = 0;
			uint64_t m_endTime   = 0;
			size_t m_outputIdx   = 0;
			bool m_pending      = false;
			bool m_hasEBMLHeader = false;

			FILE* m_file = nullptr;
			std::stack<EBML::CIdentifier> m_nodes;
			std::map<size_t, size_t> m_streamIdxToOutputIdxs;
			std::map<size_t, OpenViBE::CIdentifier> m_streamIdxToTypeIDs;

		private:
			bool initializeFile();
			bool isMasterChild(const EBML::CIdentifier& identifier) override;
			void openChild(const EBML::CIdentifier& identifier) override;
			void processChildData(const void* buffer, const size_t size) override;
			void closeChild() override;
		};

		class CBoxAlgorithmGenericStreamReaderListener final : public OpenViBE::Toolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			bool check(OpenViBE::Kernel::IBox& box) const
			{
				for (size_t i = 0; i < box.getOutputCount(); ++i) { box.setOutputName(i, ("Output stream " + std::to_string(i + 1)).c_str()); }
				return true;
			}

			bool onDefaultInitialized(OpenViBE::Kernel::IBox& box) override
			{
				box.setOutputName(0, "Output Signal");
				box.setOutputType(0, OV_TypeId_Signal);
				box.addOutput("Output Stimulations", OV_TypeId_Stimulations);
				return true;
			}

			bool onOutputAdded(OpenViBE::Kernel::IBox& box, const size_t index) override
			{
				box.setOutputType(index, OV_TypeId_EBMLStream);
				this->check(box);
				return true;
			}

			bool onOutputRemoved(OpenViBE::Kernel::IBox& box, const size_t /*index*/) override
			{
				this->check(box);
				return true;
			}

			bool onOutputTypeChanged(OpenViBE::Kernel::IBox& box, const size_t /*index*/) override
			{
				this->check(box);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
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
