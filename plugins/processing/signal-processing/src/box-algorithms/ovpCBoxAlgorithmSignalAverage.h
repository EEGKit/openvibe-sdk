#ifndef __OpenViBEPlugins_BoxAlgorithm_SignalAverage_H__
#define __OpenViBEPlugins_BoxAlgorithm_SignalAverage_H__

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

			CBoxAlgorithmSignalAverage(void);

			virtual void release(void);

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);

			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);

			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_BoxAlgorithm_SignalAverage)

		public:

			virtual void computeAverage(void);

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

			virtual void release(void) { }
			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Signal average"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Bruno Renier"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Computes the average of each input buffer."); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Signal processing/Averaging"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("0.5"); }
			virtual OpenViBE::CString getSoftwareComponent(void) const   { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const   { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_SignalAverage; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::SignalProcessing::CBoxAlgorithmSignalAverage(); }

			virtual OpenViBE::boolean getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
			{
				rPrototype.addInput("Input signal", OV_TypeId_Signal);
				rPrototype.addOutput("Filtered signal", OV_TypeId_Signal);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SignalAverageDesc)
		};
	}
}

#endif // __OpenViBEPlugins_BoxAlgorithm_SignalAverage_H__

