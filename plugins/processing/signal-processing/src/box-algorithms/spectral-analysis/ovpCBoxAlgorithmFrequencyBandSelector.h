#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <vector>
#include <map>
#include <algorithm>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		typedef std::pair<double, double> BandRange;

		class CBoxAlgorithmFrequencyBandSelector final : public OpenViBE::Toolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const size_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_FrequencyBandSelector)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_decoder = nullptr;
			OpenViBE::Kernel::TParameterHandler<const OpenViBE::IMemoryBuffer*> ip_buffer;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_matrix;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_bands;

			OpenViBE::Kernel::IAlgorithmProxy* m_encoder = nullptr;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> ip_matrix;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> ip_frequencyAbscissa;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMemoryBuffer*> op_buffer;

			OpenViBE::CMatrix m_oMatrix;
			std::vector<BandRange> m_selecteds;
			std::vector<double> m_selectionFactors;
		};

		class CBoxAlgorithmFrequencyBandSelectorDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Frequency Band Selector"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }

			OpenViBE::CString getShortDescription() const override
			{
				return OpenViBE::CString("Preserves some spectrum coefficients and puts the others to zero depending on a list of frequencies / frequency bands to select");
			}

			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Spectral Analysis"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_FrequencyBandSelector; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmFrequencyBandSelector; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input spectrum", OV_TypeId_Spectrum);
				prototype.addOutput("Output spectrum", OV_TypeId_Spectrum);
				prototype.addSetting("Frequencies to select", OV_TypeId_String, "8:12;16:24");
				// @fixme Use OV_Value_RangeStringSeparator / OV_Value_EnumeratedStringSeparator tokens above

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_FrequencyBandSelectorDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins
