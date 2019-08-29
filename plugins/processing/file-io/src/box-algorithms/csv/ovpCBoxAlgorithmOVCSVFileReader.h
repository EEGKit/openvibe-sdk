#pragma once

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <memory>

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include "csv/ovICSV.h"

#define OVP_ClassId_BoxAlgorithm_OVCSVFileReaderDesc 							   OpenViBE::CIdentifier(0x584E1948, 0x65E91650)
#define OVP_ClassId_BoxAlgorithm_OVCSVFileReader     							   OpenViBE::CIdentifier(0x336A3D9A, 0x753F1BA4)

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CBoxAlgorithmOVCSVFileReader final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			CBoxAlgorithmOVCSVFileReader();
			void release() override { delete this; }
			uint64_t getClockFrequency() override;
			bool initialize() override;
			bool uninitialize() override;
			bool processClock(OpenViBE::CMessageClock& messageClock) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_OVCSVFileReader)

		private:
			bool initializeFile();
			bool processStimulation(double startTime, double endTime);

			std::unique_ptr<OpenViBE::CSV::ICSVHandler, decltype(&OpenViBE::CSV::releaseCSVHandler)> m_ReaderLib;

			OpenViBEToolkit::TGenericEncoder<CBoxAlgorithmOVCSVFileReader> m_AlgorithmEncoder;
			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmOVCSVFileReader> m_StimulationEncoder;

			std::deque<OpenViBE::CSV::SMatrixChunk> m_SavedChunks;
			std::deque<OpenViBE::CSV::SStimulationChunk> m_SavedStimulations;

			uint64_t m_LastStimulationDate = 0;

			OpenViBE::CIdentifier m_TypeIdentifier = OV_UndefinedIdentifier;
			std::vector<std::string> m_ChannelNames;
			std::vector<uint32_t> m_DimensionSizes;
			uint32_t m_SamplingRate         = 0;
			uint32_t m_SampleCountPerBuffer = 0;

			bool m_IsHeaderSent;
			bool m_IsStimulationHeaderSent;
			std::vector<double> m_FrequencyAbscissa;
		};

		class CBoxAlgorithmOVCSVFileReaderListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onOutputTypeChanged(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				OpenViBE::CIdentifier typeIdentifier = OV_UndefinedIdentifier;
				box.getOutputType(index, typeIdentifier);

				if (index == 0 && typeIdentifier == OV_TypeId_Stimulations)
				{
					OV_ERROR_UNLESS_KRF(box.setOutputType(index, OV_TypeId_Signal),
										"Failed to reset output type to signal",
										OpenViBE::Kernel::ErrorType::Internal);
				}
				else if (index == 1 && typeIdentifier != OV_TypeId_Stimulations)
				{
					OV_ERROR_UNLESS_KRF(box.setOutputType(index, OV_TypeId_Stimulations),
										"Failed to reset output type to stimulations",
										OpenViBE::Kernel::ErrorType::Internal);
				}
				else if (index > 1)
				{
					OV_ERROR_UNLESS_KRF(false, "The index of the output does not exist", OpenViBE::Kernel::ErrorType::Internal);
				}

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmOVCSVFileReaderDesc final : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("CSV File Reader"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Victor Herlin"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Mensia Technologies SA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Read signal in a CSV (text based) file"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("File reading and writing/CSV"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.1"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.1.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.3.3"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_OVCSVFileReader; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmOVCSVFileReader; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmOVCSVFileReaderListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& BoxAlgorithmPrototype) const override
			{
				BoxAlgorithmPrototype.addOutput("Output stream", OV_TypeId_Signal);
				BoxAlgorithmPrototype.addOutput("Output stimulation", OV_TypeId_Stimulations);
				BoxAlgorithmPrototype.addSetting("Filename", OV_TypeId_Filename, "");

				BoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);

				BoxAlgorithmPrototype.addOutputSupport(OV_TypeId_Signal);
				BoxAlgorithmPrototype.addOutputSupport(OV_TypeId_Spectrum);
				BoxAlgorithmPrototype.addOutputSupport(OV_TypeId_FeatureVector);
				BoxAlgorithmPrototype.addOutputSupport(OV_TypeId_StreamedMatrix);
				BoxAlgorithmPrototype.addOutputSupport(OV_TypeId_CovarianceMatrix);
				BoxAlgorithmPrototype.addOutputSupport(OV_TypeId_Stimulations);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_OVCSVFileReaderDesc)
		};
	} // namespace FileIO
} // namespace OpenViBEPlugins
