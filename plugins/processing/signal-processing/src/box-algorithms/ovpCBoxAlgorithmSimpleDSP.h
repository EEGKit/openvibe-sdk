#pragma once

#include "../ovp_defines.h"
#include <toolkit/ovtk_all.h>
#include "../ovpCSimpleDSP/ovpCEquationParser.h"

#include <vector>
#include <cstdio>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmSimpleDSP final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			CBoxAlgorithmSimpleDSP();
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const size_t index) override;
			bool process() override;
			void evaluate();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_SimpleDSP)

		protected:
			std::vector<OpenViBE::Kernel::IAlgorithmProxy*> m_decoders;
			OpenViBE::Kernel::IAlgorithmProxy* m_encoder = nullptr;
			std::vector<OpenViBE::IMatrix*> m_matrices;

			CEquationParser* m_parser = nullptr;

			uint64_t m_equationType       = OP_USERDEF;
			double m_equationParam = 0;
			double** m_variables           = nullptr;

			bool m_checkDates = false;
		};

		class CBoxAlgorithmSimpleDSPListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onInputAdded(OpenViBE::Kernel::IBox& box, const size_t index) override
			{
				char name[1024];
				sprintf(name, "Input - %c", char('A' + index));
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getOutputType(0, typeID);
				box.setInputType(index, typeID);
				box.setInputName(index, name);
				return true;
			}

			bool onInputTypeChanged(OpenViBE::Kernel::IBox& box, const size_t index) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getInputType(index, typeID);
				box.setOutputType(0, typeID);
				for (size_t i = 0; i < box.getInputCount(); ++i) { box.setInputType(i, typeID); }
				return true;
			}

			bool onOutputTypeChanged(OpenViBE::Kernel::IBox& box, const size_t index) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getOutputType(index, typeID);
				box.setOutputType(0, typeID);
				for (size_t i = 0; i < box.getInputCount(); ++i) { box.setInputType(i, typeID); }
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmSimpleDSPDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Simple DSP"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Bruno Renier / Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA / IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Apply mathematical formulaes to matrices."); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Basic"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_SimpleDSP; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmSimpleDSP(); }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmSimpleDSPListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input - A", OV_TypeId_Signal);
				prototype.addOutput("Output", OV_TypeId_Signal);
				prototype.addSetting("Equation", OV_TypeId_String, "x");

				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);

				prototype.addInputSupport(OV_TypeId_StreamedMatrix);
				prototype.addInputSupport(OV_TypeId_FeatureVector);
				prototype.addInputSupport(OV_TypeId_Signal);
				prototype.addInputSupport(OV_TypeId_Spectrum);

				prototype.addOutputSupport(OV_TypeId_StreamedMatrix);
				prototype.addOutputSupport(OV_TypeId_FeatureVector);
				prototype.addOutputSupport(OV_TypeId_Signal);
				prototype.addOutputSupport(OV_TypeId_Spectrum);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SimpleDSPDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins
