#pragma once

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#define OVP_ClassId_Algorithm_OVMatrixFileReader                               OpenViBE::CIdentifier(0x10661A33, 0x0B0F44A7)
#define OVP_ClassId_Algorithm_OVMatrixFileReaderDesc                           OpenViBE::CIdentifier(0x0E873B5E, 0x0A287FCB)


#define OVP_Algorithm_OVMatrixFileReader_InputParameterId_Filename             OpenViBE::CIdentifier(0x28F87B29, 0x0B09737E)
#define OVP_Algorithm_OVMatrixFileReader_OutputParameterId_Matrix              OpenViBE::CIdentifier(0x2F9521E0, 0x027D789F)
#define OVP_Algorithm_OVMatrixFileReader_InputTriggerId_Open                   OpenViBE::CIdentifier(0x2F996376, 0x2A942485)
#define OVP_Algorithm_OVMatrixFileReader_InputTriggerId_Load                   OpenViBE::CIdentifier(0x22841807, 0x102D681C)
#define OVP_Algorithm_OVMatrixFileReader_InputTriggerId_Close                  OpenViBE::CIdentifier(0x7FDE77DA, 0x384A0B3D)
#define OVP_Algorithm_OVMatrixFileReader_OutputTriggerId_Error                 OpenViBE::CIdentifier(0x6D4F2F4B, 0x05EC6CB9)
#define OVP_Algorithm_OVMatrixFileReader_OutputTriggerId_DataProduced          OpenViBE::CIdentifier(0x76F46051, 0x003B6FE8)

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
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_OVMatrixFileReader_InputParameterId_Filename, "Filename", OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_OVMatrixFileReader_OutputParameterId_Matrix, "Matrix", OpenViBE::Kernel::ParameterType_Matrix);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_OVMatrixFileReaderDesc)
		};
	} // namespace FileIO
} // namespace OpenViBEPlugins
