#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <vector>
#include <map>
#include <algorithm>

namespace OpenViBE
{
	namespace Plugins
	{
		namespace SignalProcessing
		{
			typedef std::pair<double, double> BandRange;

			class CBoxAlgorithmFrequencyBandSelector final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
			{
			public:
				void release() override { delete this; }
				bool initialize() override;
				bool uninitialize() override;
				bool processInput(const size_t index) override;
				bool process() override;

				_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_FrequencyBandSelector)

			protected:

				Kernel::IAlgorithmProxy* m_decoder = nullptr;
				Kernel::TParameterHandler<const IMemoryBuffer*> ip_buffer;
				Kernel::TParameterHandler<IMatrix*> op_matrix;
				Kernel::TParameterHandler<IMatrix*> op_bands;

				Kernel::IAlgorithmProxy* m_encoder = nullptr;
				Kernel::TParameterHandler<IMatrix*> ip_matrix;
				Kernel::TParameterHandler<IMatrix*> ip_frequencyAbscissa;
				Kernel::TParameterHandler<IMemoryBuffer*> op_buffer;

				CMatrix m_oMatrix;
				std::vector<BandRange> m_selecteds;
				std::vector<double> m_selectionFactors;
			};

			class CBoxAlgorithmFrequencyBandSelectorDesc final : public IBoxAlgorithmDesc
			{
			public:
				void release() override { }
				CString getName() const override { return CString("Frequency Band Selector"); }
				CString getAuthorName() const override { return CString("Yann Renard"); }
				CString getAuthorCompanyName() const override { return CString("INRIA"); }

				CString getShortDescription() const override
				{
					return CString(
						"Preserves some spectrum coefficients and puts the others to zero depending on a list of frequencies / frequency bands to select");
				}

				CString getDetailedDescription() const override { return CString(""); }
				CString getCategory() const override { return CString("Signal processing/Spectral Analysis"); }
				CString getVersion() const override { return CString("1.0"); }
				CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
				CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
				CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
				CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_FrequencyBandSelector; }
				IPluginObject* create() override { return new CBoxAlgorithmFrequencyBandSelector; }

				bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
				{
					prototype.addInput("Input spectrum", OV_TypeId_Spectrum);
					prototype.addOutput("Output spectrum", OV_TypeId_Spectrum);
					prototype.addSetting("Frequencies to select", OV_TypeId_String, "8:12;16:24");
					// @fixme Use OV_Value_RangeStringSeparator / OV_Value_EnumeratedStringSeparator tokens above

					return true;
				}

				_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_FrequencyBandSelectorDesc)
			};
		} // namespace SignalProcessing
	}  // namespace Plugins
}  // namespace OpenViBE
