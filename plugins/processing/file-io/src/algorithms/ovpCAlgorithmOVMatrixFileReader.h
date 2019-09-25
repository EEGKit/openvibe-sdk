#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>


namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CAlgorithmOVMatrixFileReader final : public OpenViBEToolkit::TAlgorithm<OpenViBE::Plugins::IAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_OVMatrixFileReader)

		protected:

			OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> ip_sFilename;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_pMatrix;
		};

		class CAlgorithmOVMatrixFileReaderDesc final : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("OpenViBE Matrix file reader"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Vincent Delannoy"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("File reading and writing"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.1"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_OVMatrixFileReader; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CAlgorithmOVMatrixFileReader; }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const override
			{
				rAlgorithmPrototype.addInputParameter(
					OVP_Algorithm_OVMatrixFileReader_InputParameterId_Filename, "Filename", OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addOutputParameter(
					OVP_Algorithm_OVMatrixFileReader_OutputParameterId_Matrix, "Matrix", OpenViBE::Kernel::ParameterType_Matrix);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_OVMatrixFileReaderDesc)
		};
	} // namespace FileIO
} // namespace OpenViBEPlugins
