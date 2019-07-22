#pragma once

#include <toolkit/ovtk_all.h>

#include <string>
#include <vector>

#define OVP_ClassId_BoxAlgorithm_SignalAverage                                         OpenViBE::CIdentifier(0x00642C4D, 0x5DF7E50A)
#define OVP_ClassId_BoxAlgorithm_SignalAverageDesc                                     OpenViBE::CIdentifier(0x007CDCE9, 0x16034F77)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		/**
		*/
		class CBoxAlgorithmSignalAverage : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			CBoxAlgorithmSignalAverage();

			virtual void release();

			virtual bool initialize();
			virtual bool uninitialize();

			virtual bool processInput(uint32_t ui32InputIndex);

			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_BoxAlgorithm_SignalAverage)

		public:

			virtual void computeAverage();

		public:

			// Needed to read the input and write the output
			OpenViBEToolkit::TSignalDecoder<CBoxAlgorithmSignalAverage> m_oSignalDecoder;
			OpenViBEToolkit::TSignalEncoder<CBoxAlgorithmSignalAverage> m_oSignalEncoder;
		};

		/**
		* Description of the channel selection plugin
		*/
		class CSignalAverageDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }
			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Signal average"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Bruno Renier"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Computes the average of each input buffer."); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Signal processing/Averaging"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("0.5"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_SignalAverage; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmSignalAverage(); }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput("Input signal", OV_TypeId_Signal);
				rPrototype.addOutput("Filtered signal", OV_TypeId_Signal);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SignalAverageDesc)
		};
	}
}
