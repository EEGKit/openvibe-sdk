#pragma once

#include "../../ovp_defines.h"
#include <cstdio>
#include <memory>

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include "csv/ovICSV.h"

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
			bool processInput(const size_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_OVCSVFileWriter)

		private:
			bool processStreamedMatrix();
			bool processStimulation();

			std::unique_ptr<OpenViBE::CSV::ICSVHandler, decltype(&OpenViBE::CSV::releaseCSVHandler)> m_writerLib;

			OpenViBE::CIdentifier m_typeID = OV_UndefinedIdentifier;

			OpenViBEToolkit::TGenericDecoder<CBoxAlgorithmOVCSVFileWriter> m_streamDecoder;
			OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmOVCSVFileWriter> m_stimulationDecoder;

			unsigned long long m_epoch = 0;

			bool m_isHeaderReceived = false;
			bool m_isFileOpen       = false;
			bool m_appendData       = false;
			bool m_lastMatrixOnly   = false;
			bool m_writeHeader      = true;
		};

		class CBoxAlgorithmOVCSVFileWriterListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onInputTypeChanged(OpenViBE::Kernel::IBox& box, const size_t index) override
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

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input stream", OV_TypeId_Signal);
				prototype.addInput("Stimulations stream", OV_TypeId_Stimulations);
				prototype.addSetting("Filename", OV_TypeId_Filename, "record-[$core{date}-$core{time}].csv");
				prototype.addSetting("Precision", OV_TypeId_Integer, "10");
				prototype.addSetting("Append data", OV_TypeId_Boolean, "false");
				prototype.addSetting("Only last matrix", OV_TypeId_Boolean, "false");
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);

				prototype.addInputSupport(OV_TypeId_Signal);
				prototype.addInputSupport(OV_TypeId_StreamedMatrix);
				prototype.addInputSupport(OV_TypeId_Spectrum);
				prototype.addInputSupport(OV_TypeId_FeatureVector);
				prototype.addInputSupport(OV_TypeId_CovarianceMatrix);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_OVCSVFileWriterDesc)
		};
	} // namespace FileIO
} // namespace OpenViBEPlugins
