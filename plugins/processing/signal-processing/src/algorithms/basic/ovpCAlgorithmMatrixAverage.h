#pragma once

#include <vector>
#include "../../ovp_defines.h"

#include <openvibe/ov_all.h>

#include <toolkit/ovtk_all.h>

#include <deque>

#define OVP_ClassId_Algorithm_MatrixAverage                                            OpenViBE::CIdentifier(0x5E5A6C1C, 0x6F6BEB03)
#define OVP_ClassId_Algorithm_MatrixAverageDesc                                        OpenViBE::CIdentifier(0x1992881F, 0xC938C0F2)

#define OVP_Algorithm_MatrixAverage_InputParameterId_Matrix                            OpenViBE::CIdentifier(0x913E9C3B, 0x8A62F5E3)
#define OVP_Algorithm_MatrixAverage_InputParameterId_MatrixCount                       OpenViBE::CIdentifier(0x08563191, 0xE78BB265)
#define OVP_Algorithm_MatrixAverage_InputParameterId_AveragingMethod                   OpenViBE::CIdentifier(0xE63CD759, 0xB6ECF6B7)
#define OVP_Algorithm_MatrixAverage_OutputParameterId_AveragedMatrix                   OpenViBE::CIdentifier(0x03CE5AE5, 0xBD9031E0)
#define OVP_Algorithm_MatrixAverage_InputTriggerId_Reset                               OpenViBE::CIdentifier(0x670EC053, 0xADFE3F5C)
#define OVP_Algorithm_MatrixAverage_InputTriggerId_FeedMatrix                          OpenViBE::CIdentifier(0x50B6EE87, 0xDC42E660)
#define OVP_Algorithm_MatrixAverage_InputTriggerId_ForceAverage                        OpenViBE::CIdentifier(0xBF597839, 0xCD6039F0)
#define OVP_Algorithm_MatrixAverage_OutputTriggerId_AveragePerformed                   OpenViBE::CIdentifier(0x2BFF029B, 0xD932A613)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CAlgorithmMatrixAverage : public OpenViBEToolkit::TAlgorithm<OpenViBE::Plugins::IAlgorithm>
		{
		public:

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();

			virtual bool process();

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_MatrixAverage);

		protected:

			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64AveragingMethod;
			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_ui64MatrixCount;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> ip_pMatrix;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_pAveragedMatrix;

			std::deque<OpenViBE::IMatrix*> m_vHistory;
			std::vector<double> m_CumulativeAverageMatrix;
			unsigned long long m_CumulativeAverageSampleCount;
		};

		class CAlgorithmMatrixAverageDesc : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Matrix average"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Signal processing/Averaging"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.1"); }

			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_Algorithm_MatrixAverage; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CAlgorithmMatrixAverage(); }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmProto) const
			{
				rAlgorithmProto.addInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_Matrix, "Matrix", OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmProto.addInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_MatrixCount, "Matrix count", OpenViBE::Kernel::ParameterType_UInteger);
				rAlgorithmProto.addInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_AveragingMethod, "Averaging Method", OpenViBE::Kernel::ParameterType_UInteger);

				rAlgorithmProto.addOutputParameter(OVP_Algorithm_MatrixAverage_OutputParameterId_AveragedMatrix, "Averaged matrix", OpenViBE::Kernel::ParameterType_Matrix);

				rAlgorithmProto.addInputTrigger(OVP_Algorithm_MatrixAverage_InputTriggerId_Reset, "Reset");
				rAlgorithmProto.addInputTrigger(OVP_Algorithm_MatrixAverage_InputTriggerId_FeedMatrix, "Feed matrix");
				rAlgorithmProto.addInputTrigger(OVP_Algorithm_MatrixAverage_InputTriggerId_ForceAverage, "Force average");

				rAlgorithmProto.addOutputTrigger(OVP_Algorithm_MatrixAverage_OutputTriggerId_AveragePerformed, "Average performed");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_MatrixAverageDesc);
		};
	};
};


