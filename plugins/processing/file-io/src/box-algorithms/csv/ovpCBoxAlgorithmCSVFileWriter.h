#pragma once

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>

#define OVP_ClassId_BoxAlgorithm_CSVFileWriter     							   OpenViBE::CIdentifier(0x2C9312F1, 0x2D6613E5)
#define OVP_ClassId_BoxAlgorithm_CSVFileWriterDesc 							   OpenViBE::CIdentifier(0x65075FF7, 0x2B555E97)

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CBoxAlgorithmCSVFileWriter : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			CBoxAlgorithmCSVFileWriter();
			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processInput(uint32_t ui32InputIndex);
			virtual bool process();

			bool process_streamedMatrix();
			bool process_stimulation();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_CSVFileWriter)


		protected:
			bool initializeFile();

			std::ofstream m_oFileStream;

			OpenViBE::CString m_sSeparator;
			OpenViBE::CIdentifier m_oTypeIdentifier;
			bool m_bFirstBuffer;
			bool (CBoxAlgorithmCSVFileWriter::*m_fpRealProcess)();

			OpenViBEToolkit::TDecoder<CBoxAlgorithmCSVFileWriter>* m_pStreamDecoder;
			OpenViBE::CMatrix m_oMatrix;		// This represents the properties of the input, no data

			uint64_t m_ui64SampleCount;

			bool m_bHeaderReceived;
		};

		class CBoxAlgorithmCSVFileWriterListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmCSVFileWriterDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("CSV File Writer (Deprecated)"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Writes signal in a CSV (text based) file"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("File reading and writing/CSV"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_CSVFileWriter; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmCSVFileWriter; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener() const { return new CBoxAlgorithmCSVFileWriterListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Input stream", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("Filename", OV_TypeId_Filename, "record-[$core{date}-$core{time}].csv");
				rBoxAlgorithmPrototype.addSetting("Column separator", OV_TypeId_String, ";");
				rBoxAlgorithmPrototype.addSetting("Precision", OV_TypeId_Integer, "10");
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);

				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_Spectrum);
				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_FeatureVector);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_CSVFileWriterDesc)
		};
	};
};


