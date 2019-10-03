#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace Tools
	{
		class CBoxAlgorithmMatrixValidityChecker final : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_MatrixValidityChecker)

		protected:

			std::vector<OpenViBEToolkit::TStreamedMatrixDecoder<CBoxAlgorithmMatrixValidityChecker>> m_vStreamDecoder;
			std::vector<OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmMatrixValidityChecker>> m_vStreamEncoder;
			OpenViBE::Kernel::ELogLevel m_eLogLevel = OpenViBE::Kernel::ELogLevel::LogLevel_None;
			uint64_t m_validityCheckerType      = 0;

			std::vector<uint32_t> m_nTotalInterpolatedSample;
			std::vector<uint32_t> m_nTotalInterpolatedChunk;
			std::vector<std::vector<double>> m_vLastValidSample;
		};

		class CBoxAlgorithmMatrixValidityCheckerListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			bool check(OpenViBE::Kernel::IBox& box)
			{
				for (uint32_t i = 0; i < box.getInputCount(); i++)
				{
					box.setInputName(i, ("Stream " + std::to_string(i + 1)).c_str());
					box.setInputType(i, OV_TypeId_StreamedMatrix);
				}
				for (uint32_t i = 0; i < box.getOutputCount(); i++)
				{
					box.setOutputName(i, ("Output stream " + std::to_string(i + 1)).c_str());
					box.setInputType(i, OV_TypeId_StreamedMatrix);
				}

				return true;
			}

			bool onInputAdded(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				box.setInputType(index, OV_TypeId_StreamedMatrix);
				if (box.getSettingCount() > 1) { box.addOutput("", OV_TypeId_StreamedMatrix); }
				this->check(box);
				return true;
			}

			bool onInputRemoved(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				box.removeOutput(index);
				this->check(box);
				return true;
			}

			bool onOutputAdded(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				box.setOutputType(index, OV_TypeId_StreamedMatrix);
				box.addInput("", OV_TypeId_StreamedMatrix);
				this->check(box);
				return true;
			}

			bool onOutputRemoved(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				box.removeInput(index);
				this->check(box);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmMatrixValidityCheckerDesc final : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Matrix validity checker"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }

			OpenViBE::CString getShortDescription() const override
			{
				return OpenViBE::CString("Checks if a matrix contains \"not a number\" or \"infinity\" elements");
			}

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString(
					"This box is for debugging purposes and allows an author to check the validity of a streamed matrix and derived stream. This box can log a message, stop the player or interpolate data.");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Tools"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_MatrixValidityChecker; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmMatrixValidityChecker; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmMatrixValidityCheckerListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Stream 1", OV_TypeId_StreamedMatrix);
				prototype.addOutput("Output stream 1", OV_TypeId_StreamedMatrix);
				prototype.addSetting("Log level", OV_TypeId_LogLevel, "Warning");
				prototype.addSetting("Action to do", OVP_TypeId_ValidityCheckerType, OVP_TypeId_ValidityCheckerType_LogWarning.toString());
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddOutput);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_MatrixValidityCheckerDesc)
		};
	} // namespace Tools
} // namespace OpenViBEPlugins
