#pragma once

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#define OVP_ClassId_Algorithm_OVMatrixFileWriter                               OpenViBE::CIdentifier(0x739158FC, 0x1E8240CC)
#define OVP_ClassId_Algorithm_OVMatrixFileWriterDesc                           OpenViBE::CIdentifier(0x44CF6DD0, 0x329D47F9)

#define OVP_Algorithm_OVMatrixFileWriter_InputParameterId_Filename             OpenViBE::CIdentifier(0x330D2D0B, 0x175271E6)
#define OVP_Algorithm_OVMatrixFileWriter_InputParameterId_Matrix               OpenViBE::CIdentifier(0x6F6402EE, 0x493044F3)

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CAlgorithmOVMatrixFileWriter : public OpenViBEToolkit::TAlgorithm<OpenViBE::Plugins::IAlgorithm>
		{
		public:

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();
			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_OVMatrixFileWriter)

		protected:

			OpenViBE::Kernel::TParameterHandler<OpenViBE::CString*> ip_sFilename;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::CMatrix*> ip_pMatrix;
		};

		class CAlgorithmOVMatrixFileWriterDesc : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("OpenViBE Matrix file writer"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Vincent Delannoy"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("File reading and writing"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.1"); }

			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_Algorithm_OVMatrixFileWriter; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CAlgorithmOVMatrixFileWriter; }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_OVMatrixFileWriter_InputParameterId_Filename, "Filename", OpenViBE::Kernel::ParameterType_String);
				rAlgorithmPrototype.addInputParameter(OVP_Algorithm_OVMatrixFileWriter_InputParameterId_Matrix, "Matrix", OpenViBE::Kernel::ParameterType_Matrix);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_OVMatrixFileWriterDesc)
		};
	};
};
