#pragma once

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <iostream>
#include <cstdio>
#include <cstdlib>

#define OVP_ClassId_BoxAlgorithm_CSVFileReaderDesc 							   OpenViBE::CIdentifier(0x193F22E9, 0x26A67233)
#define OVP_ClassId_BoxAlgorithm_CSVFileReader     							   OpenViBE::CIdentifier(0x641D0717, 0x02884107)

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CBoxAlgorithmCSVFileReader : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			CBoxAlgorithmCSVFileReader();
			void release() override { delete this; }
			uint64_t getClockFrequency() override;
			bool initialize() override;
			bool uninitialize() override;
			bool processClock(OpenViBE::CMessageClock& rMessageClock) override;
			bool process() override;

			bool process_streamedMatrix();
			bool process_stimulation();
			bool process_signal();
			bool process_channelLocalisation();
			bool process_featureVector();
			bool process_spectrum();
			bool convertVectorDataToMatrix(OpenViBE::IMatrix* matrix);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_CSVFileReader)


		protected:
			bool initializeFile();

			FILE* m_pFile = nullptr;
			std::string m_sSeparator;
			bool m_bDoNotUseFileTime = false;
			OpenViBE::CString m_sFilename;

			OpenViBE::CIdentifier m_oTypeIdentifier = OV_UndefinedIdentifier;
			uint32_t m_ui32ColumnCount              = 0;
			uint64_t m_ui64SamplingRate             = 0;
			uint32_t m_ui32SamplesPerBuffer         = 0;
			uint32_t m_ui32ChannelNumberPerBuffer   = 0;

			bool (CBoxAlgorithmCSVFileReader::*m_fpRealProcess)();

			OpenViBEToolkit::TEncoder<CBoxAlgorithmCSVFileReader>* m_pAlgorithmEncoder = nullptr;

			bool m_bHeaderSent = false;
			std::vector<std::string> m_vLastLineSplit;
			std::vector<std::string> m_vHeaderFile;
			std::vector<std::vector<std::string>> m_vDataMatrix;

			double m_f64NextTime = 0;

			uint64_t m_ui64ChunkStartTime = 0;
			uint64_t m_ui64ChunkEndTime   = 0;

			static const uint32_t m_ui32bufferLen = 16384; // Side-effect: a maximum allowed length for a line of a CSV file
		};

		class CBoxAlgorithmCSVFileReaderListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t index) override
			{
				OpenViBE::CIdentifier l_oTypeIdentifier = OV_UndefinedIdentifier;
				rBox.getOutputType(index, l_oTypeIdentifier);
				if (l_oTypeIdentifier == OV_TypeId_Spectrum)
				{
					rBox.setSettingName(3, "Unused parameter");
					rBox.setSettingValue(3, "0");
				}
				else if (l_oTypeIdentifier == OV_TypeId_ChannelLocalisation)
				{
					rBox.setSettingName(3, "Channels number");
					rBox.setSettingValue(3, "32");
				}
				else if (l_oTypeIdentifier == OV_TypeId_FeatureVector)
				{
					rBox.setSettingName(3, "Unused parameter");
					rBox.setSettingValue(3, "0");
				}
				else if (l_oTypeIdentifier == OV_TypeId_StreamedMatrix)
				{
					rBox.setSettingName(3, "Samples per buffer");
					rBox.setSettingValue(3, "32");
				}
				else if (l_oTypeIdentifier == OV_TypeId_Stimulations)
				{
					rBox.setSettingName(3, "Unused parameter");
					rBox.setSettingValue(3, "0");
				}
				else
				{
					rBox.setOutputType(index, OV_TypeId_Signal);
					rBox.setSettingName(3, "Samples per buffer");
					rBox.setSettingValue(3, "32");

					OV_ERROR_KRF("Unsupported stream type " << l_oTypeIdentifier.toString(), OpenViBE::Kernel::ErrorType::BadOutput);
				}
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmCSVFileReaderDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("CSV File Reader (Deprecated)"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Baptiste Payan"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Read signal in a CSV (text based) file"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("File reading and writing/CSV"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_CSVFileReader; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmCSVFileReader; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmCSVFileReaderListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const override { delete pBoxListener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const override
			{
				rBoxAlgorithmPrototype.addOutput("Output stream", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addSetting("Filename", OV_TypeId_Filename, "");
				rBoxAlgorithmPrototype.addSetting("Column separator", OV_TypeId_String, ";");
				rBoxAlgorithmPrototype.addSetting("Don't use the file time",OV_TypeId_Boolean, "false");
				rBoxAlgorithmPrototype.addSetting("Samples per buffer", OV_TypeId_Integer, "32");

				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);

				rBoxAlgorithmPrototype.addOutputSupport(OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addOutputSupport(OV_TypeId_FeatureVector);
				rBoxAlgorithmPrototype.addOutputSupport(OV_TypeId_ChannelLocalisation);
				rBoxAlgorithmPrototype.addOutputSupport(OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutputSupport(OV_TypeId_Spectrum);
				rBoxAlgorithmPrototype.addOutputSupport(OV_TypeId_Stimulations);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_CSVFileReaderDesc)
		};
	} // namespace FileIO
} // namespace OpenViBEPlugins
