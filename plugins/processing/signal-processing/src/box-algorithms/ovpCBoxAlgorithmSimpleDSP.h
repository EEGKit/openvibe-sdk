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
		class CBoxAlgorithmSimpleDSP : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			CBoxAlgorithmSimpleDSP();

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool processInput(uint32_t ui32InputIndex);
			virtual bool process();
			virtual void evaluate();

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithm, OVP_ClassId_BoxAlgorithm_SimpleDSP)

		public:

			std::vector<OpenViBE::IMatrix*> m_vMatrix;
			std::vector<OpenViBE::Kernel::IAlgorithmProxy*> m_vStreamDecoder;
			OpenViBE::Kernel::IAlgorithmProxy* m_pStreamEncoder;

			CEquationParser* m_pEquationParser;

			uint64_t m_ui64EquationType;
			double m_f64SpecialEquationParameter;
			double** m_ppVariable;

			bool m_bCheckChunkDates;
		};

		class CBoxAlgorithmSimpleDSPListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			virtual bool onInputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				char l_sName[1024];
				sprintf(l_sName, "Input - %c", 'A' + ui32Index);
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getOutputType(0, l_oTypeIdentifier);
				rBox.setInputType(ui32Index, l_oTypeIdentifier);
				rBox.setInputName(ui32Index, l_sName);
				return true;
			}

			virtual bool onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				uint32_t i;
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getInputType(ui32Index, l_oTypeIdentifier);
				rBox.setOutputType(0, l_oTypeIdentifier);
				for (i = 0; i < rBox.getInputCount(); i++)
				{
					rBox.setInputType(i, l_oTypeIdentifier);
				}
				return true;
			}

			virtual bool onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				uint32_t i;
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getOutputType(ui32Index, l_oTypeIdentifier);
				rBox.setOutputType(0, l_oTypeIdentifier);
				for (i = 0; i < rBox.getInputCount(); i++)
				{
					rBox.setInputType(i, l_oTypeIdentifier);
				}
				return true;
			};

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmSimpleDSPDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release() { }
			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Simple DSP"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Bruno Renier / Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA / IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Apply mathematical formulaes to matrices."); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Signal processing/Basic"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_BoxAlgorithm_SimpleDSP; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CBoxAlgorithmSimpleDSP(); }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener() const { return new CBoxAlgorithmSimpleDSPListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rPrototype) const
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
	}
}
