#ifndef __OpenViBEPlugins_BoxAlgorithm_MatrixValidityChecker_H__
#define __OpenViBEPlugins_BoxAlgorithm_MatrixValidityChecker_H__

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>
#include <cstdio>

#define OVP_ClassId_BoxAlgorithm_MatrixValidityChecker     OpenViBE::CIdentifier(0x60210579, 0x6F7519B6)
#define OVP_ClassId_BoxAlgorithm_MatrixValidityCheckerDesc OpenViBE::CIdentifier(0x6AFC2671, 0x1D8C493C)

#define OVP_TypeId_ValidityCheckerType             OpenViBE::CIdentifier(0x32EA493A, 0x11E56D82)
#define OVP_TypeId_ValidityCheckerType_LogWarning  OpenViBE::CIdentifier(0x747A0F84, 0x1097253A)
#define OVP_TypeId_ValidityCheckerType_StopPlayer  OpenViBE::CIdentifier(0x4EC06D50, 0x5B131CE2)
#define OVP_TypeId_ValidityCheckerType_Interpolate OpenViBE::CIdentifier(0x1DE96E02, 0x53767550)

namespace OpenViBEPlugins
{
	namespace Tools
	{
		class CBoxAlgorithmMatrixValidityChecker : virtual public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_MatrixValidityChecker);

		protected:

			std::vector < OpenViBEToolkit::TStreamedMatrixDecoder < CBoxAlgorithmMatrixValidityChecker > > m_vStreamDecoder;
			std::vector < OpenViBEToolkit::TStreamedMatrixEncoder < CBoxAlgorithmMatrixValidityChecker > > m_vStreamEncoder;
			OpenViBE::Kernel::ELogLevel m_eLogLevel;
			OpenViBE::uint64 m_ui64ValidityCheckerType;

			std::vector < OpenViBE::uint32 > m_ui32TotalInterpolatedSampleCount;
			std::vector < OpenViBE::uint32 > m_ui32TotalInterpolatedChunkCount;
			std::vector < std::vector < OpenViBE::float64 > > m_vLastValidSample;
		};

		class CBoxAlgorithmMatrixValidityCheckerListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			OpenViBE::boolean check(OpenViBE::Kernel::IBox& rBox)
			{
				char l_sName[1024];
				OpenViBE::uint32 i;

				for(i=0; i<rBox.getInputCount(); i++)
				{
					sprintf(l_sName, "Stream %u", i+1);
					rBox.setInputName(i, l_sName);
					rBox.setInputType(i, OV_TypeId_StreamedMatrix);
				}
				for(i=0; i<rBox.getOutputCount(); i++)
				{
					sprintf(l_sName, "Output stream %u", i+1);
					rBox.setOutputName(i, l_sName);
					rBox.setInputType(i, OV_TypeId_StreamedMatrix);
				}

				return true;
			}

			virtual OpenViBE::boolean onInputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.setInputType(ui32Index, OV_TypeId_StreamedMatrix);
				if(rBox.getSettingCount()>1) rBox.addOutput("", OV_TypeId_StreamedMatrix);
				this->check(rBox);
				return true;
			}

			virtual OpenViBE::boolean onInputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.removeOutput(ui32Index);
				this->check(rBox);
				return true;
			}

			virtual OpenViBE::boolean onOutputAdded(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.setOutputType(ui32Index, OV_TypeId_StreamedMatrix);
				rBox.addInput("", OV_TypeId_StreamedMatrix);
				this->check(rBox);
				return true;
			}

			virtual OpenViBE::boolean onOutputRemoved(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				rBox.removeInput(ui32Index);
				this->check(rBox);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CBoxAlgorithmMatrixValidityCheckerDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("Matrix validity checker"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Checks if a matrix contains \"not a number\" or \"infinity\" elements"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("This box is for debugging purposes and allows an author to check the validity of a streamed matrix and derived stream. This box can log a message, stop the player or interpolate data."); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("Tools"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			
			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_MatrixValidityChecker; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::Tools::CBoxAlgorithmMatrixValidityChecker; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmMatrixValidityCheckerListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Stream 1",            OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addOutput ("Output stream 1",     OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addSetting("Log level",           OV_TypeId_LogLevel, "Important warning");
				rBoxAlgorithmPrototype.addSetting("Action to do",        OVP_TypeId_ValidityCheckerType, OVP_TypeId_ValidityCheckerType_LogWarning.toString());
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanAddInput);
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanAddOutput);
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_IsMensia);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_MatrixValidityCheckerDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_MatrixValidityChecker_H__
