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
		class CBoxAlgorithmGenericStreamReader : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, public EBML::IReaderCallback
		{
		public:

			CBoxAlgorithmGenericStreamReader(void);

			virtual void release(void) { delete this; }

			virtual uint64_t getClockFrequency(void);
			virtual bool initialize(void);
			virtual bool uninitialize(void);
			virtual bool processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual bool process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_GenericStreamReader);


		protected:

			OpenViBE::CString m_sFilename;

			EBML::CReader m_oReader;
			EBML::CReaderHelper m_oReaderHelper;

			OpenViBE::CMemoryBuffer m_oSwap;
			OpenViBE::CMemoryBuffer m_oPendingChunk;
			uint64_t m_ui64StartTime;
			uint64_t m_ui64EndTime;
			uint32_t m_ui32OutputIndex;
			bool m_bPending;
			bool m_bHasEBMLHeader;

			FILE* m_pFile;
			std::stack<EBML::CIdentifier> m_vNodes;
			std::map<uint32_t, uint32_t> m_vStreamIndexToOutputIndex;
			std::map<uint32_t, OpenViBE::CIdentifier> m_vStreamIndexToTypeIdentifier;

		private:
			bool initializeFile();

			virtual bool isMasterChild(const EBML::CIdentifier& rIdentifier);
			virtual void openChild(const EBML::CIdentifier& rIdentifier);
			virtual void processChildData(const void* pBuffer, uint64_t ui64BufferSize);
			virtual void closeChild(void);
		};

		class CBoxAlgorithmGenericStreamReaderListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			bool check(OpenViBE::Kernel::IBox& rBox)
			{
				char l_sName[1024];
				uint32_t i;
				for (i = 0; i < rBox.getOutputCount(); i++)
				{
					sprintf(l_sName, "Output stream %u", i + 1);
					rBox.setOutputName(i, l_sName);
				}
				return true;
			}

			virtual bool onDefaultInitialized(OpenViBE::Kernel::IBox& rBox)
			{
				rBox.setOutputName(0, "Output Signal");
				rBox.setOutputType(0, OV_TypeId_Signal);
				rBox.addOutput("Output Stimulations", OV_TypeId_Stimulations);
				return true;
			}

			virtual bool onOutputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				rBox.setOutputType(ui32Index, OV_TypeId_EBMLStream);
				this->check(rBox);
				return true;
			}

			virtual bool onOutputRemoved(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				this->check(rBox);
				return true;
			}

			virtual bool onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				this->check(rBox);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmGenericStreamReaderDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const { return OpenViBE::CString("Generic stream reader"); }
			virtual OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString("Reads OpenViBE streams saved in the .ov format"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Generic Stream Writer box can be used to store data in the format read by this box"); }
			virtual OpenViBE::CString getCategory(void) const { return OpenViBE::CString("File reading and writing/OpenViBE"); }
			virtual OpenViBE::CString getVersion(void) const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent(void) const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_BoxAlgorithm_GenericStreamReader; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new CBoxAlgorithmGenericStreamReader; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const { return new CBoxAlgorithmGenericStreamReaderListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addOutput("Output stream 1", OV_TypeId_EBMLStream);
				rBoxAlgorithmPrototype.addSetting("Filename", OV_TypeId_Filename, "");
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddOutput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_GenericStreamReaderDesc);
		};
	};
};


