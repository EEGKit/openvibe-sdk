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

			virtual void release() { delete this; }

			virtual uint64_t getClockFrequency();
			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processClock(OpenViBE::CMessageClock& rMessageClock);
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ElectrodeLocalisationFileReader)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pOVMatrixFileReader;

			OpenViBEToolkit::TChannelLocalisationEncoder<CBoxAlgorithmElectrodeLocalisationFileReader>* m_pChannelLocalisationStreamEncoder;

			OpenViBE::CString m_sFilename;
			bool m_bHeaderSent;
			bool m_bBufferSent;
		};

		class CBoxAlgorithmElectrodeLocalisationFileReaderDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Electrode localisation file reader"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Vincent Delannoy"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Loads files containing the normalized coordinates of an electrode set"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("File reading and writing/OpenViBE"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_ElectrodeLocalisationFileReader; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmElectrodeLocalisationFileReader; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				// Adds box outputs
				rBoxAlgorithmPrototype.addOutput("Channel localisation", OV_TypeId_ChannelLocalisation);

				// Adds settings
				rBoxAlgorithmPrototype.addSetting("Filename", OV_TypeId_Filename, "");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ElectrodeLocalisationFileReaderDesc)
		};
	};
};
