#pragma once

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>
#include "csv/ovICSV.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>

#define OVP_ClassId_BoxAlgorithm_OVCSVFileWriter     							   OpenViBE::CIdentifier(0x428375E8, 0x325F2DB9)
#define OVP_ClassId_BoxAlgorithm_OVCSVFileWriterDesc 							   OpenViBE::CIdentifier(0x4B5C1D8F, 0x570E45FD)

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CBoxAlgorithmOVCSVFileWriter final : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			CBoxAlgorithmOVCSVFileWriter(void);
			void release(void) override { delete this; }

			bool initialize(void) override;
			bool uninitialize(void) override;
			bool processInput(unsigned int inputIndex) override;
			bool process(void) override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_OVCSVFileWriter);


		private:
			OpenViBE::CSV::ICSVLib* m_WriterLib;

			OpenViBE::CString m_Separator;
			OpenViBE::CIdentifier m_TypeIdentifier;
			bool processStreamedMatrix(void);
			bool processStimulation(void);
			OpenViBEToolkit::TDecoder < CBoxAlgorithmOVCSVFileWriter >* m_StreamDecoder;

			unsigned long long m_Epoch;

			bool m_IsHeaderReceived;
			bool m_IsFileOpen;

		};

		class CBoxAlgorithmOVCSVFileWriterListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CBoxAlgorithmOVCSVFileWriterDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("New CSV File Writer"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Victor Herlin"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("Mensia Technologies SA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Writes signal in a CSV (text based) file"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("File reading and writing/CSV"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			
			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_OVCSVFileWriter; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::FileIO::CBoxAlgorithmOVCSVFileWriter; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmOVCSVFileWriterListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* boxListener) const { delete boxListener; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& boxAlgorithmPrototype) const
			{
				boxAlgorithmPrototype.addInput  ("Input stream",        OV_TypeId_Signal);
				boxAlgorithmPrototype.addInput  ("Stimulations stream", OV_TypeId_Stimulations);
				boxAlgorithmPrototype.addSetting("Filename",            OV_TypeId_Filename, "record-[$core{date}-$core{time}].csv");
				boxAlgorithmPrototype.addSetting("Column separator",    OV_TypeId_String, ";");
				boxAlgorithmPrototype.addSetting("Precision",           OV_TypeId_Integer, "10");
				boxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);

				boxAlgorithmPrototype.addInputSupport(OV_TypeId_Signal);
				boxAlgorithmPrototype.addInputSupport(OV_TypeId_StreamedMatrix);
				boxAlgorithmPrototype.addInputSupport(OV_TypeId_Spectrum);
				boxAlgorithmPrototype.addInputSupport(OV_TypeId_FeatureVector);
				boxAlgorithmPrototype.addInputSupport(OV_TypeId_Stimulations);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_OVCSVFileWriterDesc);
		};
	};
};
