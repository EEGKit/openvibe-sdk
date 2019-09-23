#pragma once

#include <system/ovCTime.h>
#include <toolkit/ovtk_all.h>

#include "../ovpCSimpleDSP/ovpCEquationParser.h"

#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>

#define OVP_ClassId_BoxAlgorithm_SimpleDSP                                             OpenViBE::CIdentifier(0x00E26FA1, 0x1DBAB1B2)
#define OVP_ClassId_BoxAlgorithm_SimpleDSPDesc                                         OpenViBE::CIdentifier(0x00C44BFE, 0x76C9269E)

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
			bool processInput(const uint32_t index) override;
			bool process() override;
			virtual void evaluate();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_SimpleDSP)

			std::vector<OpenViBE::IMatrix*> m_vMatrix;
			std::vector<OpenViBE::Kernel::IAlgorithmProxy*> m_vStreamDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder = nullptr;

			CEquationParser* m_pEquationParser = nullptr;

			uint64_t m_ui64EquationType          = OP_USERDEF;
			double m_f64SpecialEquationParameter = 0;
			double** m_ppVariable                = nullptr;

			bool m_bCheckChunkDates = false;
		};

		class CBoxAlgorithmSimpleDSPListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onInputAdded(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				char l_sName[1024];
				sprintf(l_sName, "Input - %c", 'A' + index);
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getOutputType(0, typeID);
				box.setInputType(index, typeID);
				box.setInputName(index, l_sName);
				return true;
			}

			bool onInputTypeChanged(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getInputType(index, typeID);
				box.setOutputType(0, typeID);
				for (uint32_t i = 0; i < box.getInputCount(); i++) { box.setInputType(i, typeID); }
				return true;
			}

			bool onOutputTypeChanged(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getOutputType(index, typeID);
				box.setOutputType(0, typeID);
				for (uint32_t i = 0; i < box.getInputCount(); i++) { box.setInputType(i, typeID); }
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

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const override
			{
				rPrototype.addInput("Input - A", OV_TypeId_Signal);
				rPrototype.addOutput("Output", OV_TypeId_Signal);
				rPrototype.addSetting("Equation", OV_TypeId_String, "x");

				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);
				rPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);

				rPrototype.addInputSupport(OV_TypeId_StreamedMatrix);
				rPrototype.addInputSupport(OV_TypeId_FeatureVector);
				rPrototype.addInputSupport(OV_TypeId_Signal);
				rPrototype.addInputSupport(OV_TypeId_Spectrum);

				rPrototype.addOutputSupport(OV_TypeId_StreamedMatrix);
				rPrototype.addOutputSupport(OV_TypeId_FeatureVector);
				rPrototype.addOutputSupport(OV_TypeId_Signal);
				rPrototype.addOutputSupport(OV_TypeId_Spectrum);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_SimpleDSPDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins
