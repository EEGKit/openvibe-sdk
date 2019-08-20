#pragma once

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#define OVP_ClassId_BoxAlgorithm_ElectrodeLocalisationFileReader               OpenViBE::CIdentifier(0x40704155, 0x19C50E8F)
#define OVP_ClassId_BoxAlgorithm_ElectrodeLocalisationFileReaderDesc           OpenViBE::CIdentifier(0x4796613F, 0x653A48D5)

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CBoxAlgorithmElectrodeLocalisationFileReader : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			uint64_t getClockFrequency() override;
			bool initialize() override;
			bool uninitialize() override;
			bool processClock(OpenViBE::CMessageClock& rMessageClock) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ElectrodeLocalisationFileReader)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pOVMatrixFileReader = nullptr;

			OpenViBEToolkit::TChannelLocalisationEncoder<CBoxAlgorithmElectrodeLocalisationFileReader>* m_pChannelLocalisationStreamEncoder;

			OpenViBE::CString m_sFilename;
			bool m_bHeaderSent = false;
			bool m_bBufferSent = false;
		};

		class CBoxAlgorithmElectrodeLocalisationFileReaderDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Electrode localisation file reader"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Vincent Delannoy"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Loads files containing the normalized coordinates of an electrode set"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("File reading and writing/OpenViBE"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_ElectrodeLocalisationFileReader; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmElectrodeLocalisationFileReader; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const override
			{
				// Adds box outputs
				rBoxAlgorithmPrototype.addOutput("Channel localisation", OV_TypeId_ChannelLocalisation);

				// Adds settings
				rBoxAlgorithmPrototype.addSetting("Filename", OV_TypeId_Filename, "");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ElectrodeLocalisationFileReaderDesc)
		};
	} // namespace FileIO
} // namespace OpenViBEPlugins
