#pragma once

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <memory>

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include "csv/ovICSV.h"

#define OVP_ClassId_BoxAlgorithm_OVCSVFileWriter     							   OpenViBE::CIdentifier(0x428375E8, 0x325F2DB9)
#define OVP_ClassId_BoxAlgorithm_OVCSVFileWriterDesc 							   OpenViBE::CIdentifier(0x4B5C1D8F, 0x570E45FD)

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CBoxAlgorithmOVCSVFileWriter final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			CBoxAlgorithmOVCSVFileWriter();
			void release() override { delete this; }

			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_OVCSVFileWriter)

		private:
			bool processStreamedMatrix();
			bool processStimulation();

			std::unique_ptr<OpenViBE::CSV::ICSVHandler, decltype(&OpenViBE::CSV::releaseCSVHandler)> m_WriterLib;

			OpenViBE::CIdentifier m_TypeIdentifier = OV_UndefinedIdentifier;

			OpenViBEToolkit::TGenericDecoder<CBoxAlgorithmOVCSVFileWriter> m_StreamDecoder;
			OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmOVCSVFileWriter> m_StimulationDecoder;

			unsigned long long m_Epoch = 0;

			bool m_IsHeaderReceived = false;
			bool m_IsFileOpen       = false;
			bool m_AppendData       = false;
			bool m_LastMatrixOnly   = false;
			bool m_WriteHeader      = true;
		};

		class CBoxAlgorithmOVCSVFileWriterListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onInputTypeChanged(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				if (index == 1)
				{
					OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
					box.getInputType(1, typeID);
					if (typeID != OV_TypeId_Stimulations)
					{
						box.setInputType(1, OV_TypeId_Stimulations);
						return true;
					}
				}

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmOVCSVFileWriterDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("CSV File Writer"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Victor Herlin"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Mensia Technologies SA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Writes signal in a CSV (text based) file"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("File reading and writing/CSV"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.1.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.1.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_OVCSVFileWriter; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmOVCSVFileWriter; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmOVCSVFileWriterListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* boxListener) const override { delete boxListener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& boxAlgorithmPrototype) const override
			{
				boxAlgorithmPrototype.addInput("Input stream", OV_TypeId_Signal);
				boxAlgorithmPrototype.addInput("Stimulations stream", OV_TypeId_Stimulations);
				boxAlgorithmPrototype.addSetting("Filename", OV_TypeId_Filename, "record-[$core{date}-$core{time}].csv");
				boxAlgorithmPrototype.addSetting("Precision", OV_TypeId_Integer, "10");
				boxAlgorithmPrototype.addSetting("Append data", OV_TypeId_Boolean, "false");
				boxAlgorithmPrototype.addSetting("Only last matrix", OV_TypeId_Boolean, "false");
				boxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);

				boxAlgorithmPrototype.addInputSupport(OV_TypeId_Signal);
				boxAlgorithmPrototype.addInputSupport(OV_TypeId_StreamedMatrix);
				boxAlgorithmPrototype.addInputSupport(OV_TypeId_Spectrum);
				boxAlgorithmPrototype.addInputSupport(OV_TypeId_FeatureVector);
				boxAlgorithmPrototype.addInputSupport(OV_TypeId_CovarianceMatrix);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_OVCSVFileWriterDesc)
		};
	} // namespace FileIO
} // namespace OpenViBEPlugins
