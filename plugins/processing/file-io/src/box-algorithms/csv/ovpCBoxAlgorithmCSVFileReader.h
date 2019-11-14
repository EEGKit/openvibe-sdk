#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <iostream>
#include <cstdio>


namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CBoxAlgorithmCSVFileReader final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			CBoxAlgorithmCSVFileReader() {}
			void release() override { delete this; }
			uint64_t getClockFrequency() override { return 128LL << 32; } // the box clock frequency
			bool initialize() override;
			bool uninitialize() override;
			bool processClock(OpenViBE::CMessageClock& messageClock) override;
			bool process() override;

			bool processStreamedMatrix();
			bool processStimulation();
			bool processSignal();
			bool processChannelLocalisation();
			bool processFeatureVector();
			bool processSpectrum();
			bool convertVectorDataToMatrix(OpenViBE::IMatrix* matrix);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_CSVFileReader)


		protected:
			bool initializeFile();

			FILE* m_pFile = nullptr;
			std::string m_sSeparator;
			bool m_bDoNotUseFileTime = false;
			OpenViBE::CString m_sFilename;

			OpenViBE::CIdentifier m_typeID = OV_UndefinedIdentifier;
			size_t m_nCol                  = 0;
			size_t m_sampling              = 0;
			size_t m_samplesPerBuffer      = 0;
			size_t m_channelsPerBuffer     = 0;

			bool (CBoxAlgorithmCSVFileReader::*m_fpRealProcess)() = nullptr;

			OpenViBEToolkit::TEncoder<CBoxAlgorithmCSVFileReader>* m_encoder = nullptr;

			bool m_headerSent = false;
			std::vector<std::string> m_vLastLineSplit;
			std::vector<std::string> m_vHeaderFile;
			std::vector<std::vector<std::string>> m_vDataMatrix;

			double m_nextTime = 0;

			uint64_t m_chunkStartTime = 0;
			uint64_t m_chunkEndTime   = 0;

			static const size_t BUFFER_LEN = 16384; // Side-effect: a maximum allowed length for a line of a CSV file
		};

		class CBoxAlgorithmCSVFileReaderListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onOutputTypeChanged(OpenViBE::Kernel::IBox& box, const size_t index) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getOutputType(index, typeID);
				if (typeID == OV_TypeId_Spectrum)
				{
					box.setSettingName(3, "Unused parameter");
					box.setSettingValue(3, "0");
				}
				else if (typeID == OV_TypeId_ChannelLocalisation)
				{
					box.setSettingName(3, "Channels number");
					box.setSettingValue(3, "32");
				}
				else if (typeID == OV_TypeId_FeatureVector)
				{
					box.setSettingName(3, "Unused parameter");
					box.setSettingValue(3, "0");
				}
				else if (typeID == OV_TypeId_StreamedMatrix)
				{
					box.setSettingName(3, "Samples per buffer");
					box.setSettingValue(3, "32");
				}
				else if (typeID == OV_TypeId_Stimulations)
				{
					box.setSettingName(3, "Unused parameter");
					box.setSettingValue(3, "0");
				}
				else
				{
					box.setOutputType(index, OV_TypeId_Signal);
					box.setSettingName(3, "Samples per buffer");
					box.setSettingValue(3, "32");

					OV_ERROR_KRF("Unsupported stream type " << typeID.toString(), OpenViBE::Kernel::ErrorType::BadOutput);
				}
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmCSVFileReaderDesc final : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
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
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addOutput("Output stream", OV_TypeId_Signal);
				prototype.addSetting("Filename", OV_TypeId_Filename, "");
				prototype.addSetting("Column separator", OV_TypeId_String, ";");
				prototype.addSetting("Don't use the file time",OV_TypeId_Boolean, "false");
				prototype.addSetting("Samples per buffer", OV_TypeId_Integer, "32");

				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);

				prototype.addOutputSupport(OV_TypeId_StreamedMatrix);
				prototype.addOutputSupport(OV_TypeId_FeatureVector);
				prototype.addOutputSupport(OV_TypeId_ChannelLocalisation);
				prototype.addOutputSupport(OV_TypeId_Signal);
				prototype.addOutputSupport(OV_TypeId_Spectrum);
				prototype.addOutputSupport(OV_TypeId_Stimulations);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_CSVFileReaderDesc)
		};
	} // namespace FileIO
} // namespace OpenViBEPlugins
