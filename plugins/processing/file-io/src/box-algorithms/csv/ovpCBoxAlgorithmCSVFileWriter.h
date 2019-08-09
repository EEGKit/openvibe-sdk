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
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t ui32InputIndex) override;
			bool process() override;

			bool process_streamedMatrix();
			bool process_stimulation();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_CSVFileWriter)


		protected:
			bool initializeFile();

			std::ofstream m_oFileStream;

			OpenViBE::CString m_sSeparator;
			OpenViBE::CIdentifier m_oTypeIdentifier = OV_UndefinedIdentifier;
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
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("CSV File Writer (Deprecated)"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Writes signal in a CSV (text based) file"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("File reading and writing/CSV"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_CSVFileWriter; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmCSVFileWriter; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmCSVFileWriterListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const override { delete pBoxListener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const override
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
