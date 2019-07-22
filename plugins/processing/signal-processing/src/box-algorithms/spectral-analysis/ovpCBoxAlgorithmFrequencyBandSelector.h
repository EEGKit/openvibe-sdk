#pragma once

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <vector>
#include <map>
#include <algorithm>

#define OVP_ClassId_BoxAlgorithm_FrequencyBandSelector     OpenViBE::CIdentifier(0x140C19C6, 0x4E6E187B)
#define OVP_ClassId_BoxAlgorithm_FrequencyBandSelectorDesc OpenViBE::CIdentifier(0x13462C56, 0x794E3C07)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		typedef std::pair<double, double> BandRange;

		class CBoxAlgorithmFrequencyBandSelector : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processInput(uint32_t ui32InputIndex);
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_FrequencyBandSelector)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamDecoder;
			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_pMemoryBuffer;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_pMatrix;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_pBands;

			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> ip_pMatrix;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> ip_pFrequencyAbscissa;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_pMemoryBuffer;

			OpenViBE::CMatrix m_oMatrix;
			std::vector<BandRange> m_vSelected;
			std::vector<double> m_vSelectionFactor;
		};

		class CBoxAlgorithmFrequencyBandSelectorDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Frequency Band Selector"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Preserves some spectrum coefficients and puts the others to zero depending on a list of frequencies / frequency bands to select"); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Signal processing/Spectral Analysis"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_FrequencyBandSelector; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmFrequencyBandSelector; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Input spectrum", OV_TypeId_Spectrum);
				rBoxAlgorithmPrototype.addOutput("Output spectrum", OV_TypeId_Spectrum);
				rBoxAlgorithmPrototype.addSetting("Frequencies to select", OV_TypeId_String, "8:12;16:24");
				// @fixme Use OV_Value_RangeStringSeparator / OV_Value_EnumeratedStringSeparator tokens above

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_FrequencyBandSelectorDesc)
		};
	};
};
