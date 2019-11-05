#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CAlgorithmOVMatrixFileWriter final : public OpenViBEToolkit::TAlgorithm<OpenViBE::Plugins::IAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_OVMatrixFileWriter)

		protected:

			OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> ip_sFilename;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::CMatrix*> ip_pMatrix;
		};

		class CAlgorithmOVMatrixFileWriterDesc final : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("OpenViBE Matrix file writer"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Vincent Delannoy"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("File reading and writing"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.1"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_OVMatrixFileWriter; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CAlgorithmOVMatrixFileWriter; }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& prototype) const override
			{
				prototype.addInputParameter(
					OVP_Algorithm_OVMatrixFileWriter_InputParameterId_Filename, "Filename", OpenViBE::Kernel::ParameterType_String);
				prototype.addInputParameter(
					OVP_Algorithm_OVMatrixFileWriter_InputParameterId_Matrix, "Matrix", OpenViBE::Kernel::ParameterType_Matrix);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_OVMatrixFileWriterDesc)
		};
	} // namespace FileIO
} // namespace OpenViBEPlugins
