#pragma once

#include "../../ovp_defines.h"
#include <toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmEpochAverage final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const size_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_EpochAverage)

		protected:

			OpenViBE::Kernel::IAlgorithmProxy* m_decoder       = nullptr;
			OpenViBE::Kernel::IAlgorithmProxy* m_encoder       = nullptr;
			OpenViBE::Kernel::IAlgorithmProxy* m_matrixAverage = nullptr;

			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_matrixCount;
			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_averagingMethod;
		};

		class CBoxAlgorithmEpochAverageListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onInputTypeChanged(OpenViBE::Kernel::IBox& box, const size_t index) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getInputType(index, typeID);
				box.setOutputType(index, typeID);
				return true;
			}

			bool onOutputTypeChanged(OpenViBE::Kernel::IBox& box, const size_t index) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getOutputType(index, typeID);
				box.setInputType(index, typeID);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmEpochAverageDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Epoch average"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }

			OpenViBE::CString getShortDescription() const override
			{
				return OpenViBE::CString("Averages matrices among time, this can be used to enhance ERPs");
			}

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString("This box can average matrices of different types including signal, spectrum or feature vectors");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Averaging"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_EpochAverage; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmEpochAverage(); }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmEpochAverageListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input epochs", OV_TypeId_StreamedMatrix);
				prototype.addOutput("Averaged epochs", OV_TypeId_StreamedMatrix);
				prototype.addSetting("Averaging type", OVP_TypeId_EpochAverageMethod, "Moving epoch average");
				prototype.addSetting("Epoch count", OV_TypeId_Integer, "4");
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);

				prototype.addInputSupport(OV_TypeId_Signal);
				prototype.addInputSupport(OV_TypeId_Spectrum);
				prototype.addInputSupport(OV_TypeId_StreamedMatrix);
				prototype.addInputSupport(OV_TypeId_FeatureVector);
				prototype.addInputSupport(OV_TypeId_TimeFrequency);

				prototype.addOutputSupport(OV_TypeId_Signal);
				prototype.addOutputSupport(OV_TypeId_Spectrum);
				prototype.addOutputSupport(OV_TypeId_StreamedMatrix);
				prototype.addOutputSupport(OV_TypeId_FeatureVector);
				prototype.addOutputSupport(OV_TypeId_TimeFrequency);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_EpochAverageDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins
