#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <vector>
#include <deque>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CAlgorithmMatrixAverage final : public OpenViBEToolkit::TAlgorithm<OpenViBE::Plugins::IAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TAlgorithm < OpenViBE::Plugins::IAlgorithm >, OVP_ClassId_Algorithm_MatrixAverage)

		protected:

			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_averagingMethod;
			OpenViBE::Kernel::TParameterHandler<uint64_t> ip_matrixCount;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> ip_matrix;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_averagedMatrix;

			std::deque<OpenViBE::IMatrix*> m_history;
			std::vector<double> m_averageMatrices;
			size_t m_nAverageSamples = 0;
		};

		class CAlgorithmMatrixAverageDesc final : public OpenViBE::Plugins::IAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Matrix average"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Averaging"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.1"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_MatrixAverage; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CAlgorithmMatrixAverage(); }

			bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& prototype) const override
			{
				prototype.addInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_Matrix, "Matrix", OpenViBE::Kernel::ParameterType_Matrix);
				prototype.addInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_MatrixCount, "Matrix count", OpenViBE::Kernel::ParameterType_UInteger);
				prototype.addInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_AveragingMethod, "Averaging Method", OpenViBE::Kernel::ParameterType_UInteger);

				prototype.addOutputParameter(OVP_Algorithm_MatrixAverage_OutputParameterId_AveragedMatrix, "Averaged matrix", OpenViBE::Kernel::ParameterType_Matrix);

				prototype.addInputTrigger(OVP_Algorithm_MatrixAverage_InputTriggerId_Reset, "Reset");
				prototype.addInputTrigger(OVP_Algorithm_MatrixAverage_InputTriggerId_FeedMatrix, "Feed matrix");
				prototype.addInputTrigger(OVP_Algorithm_MatrixAverage_InputTriggerId_ForceAverage, "Force average");

				prototype.addOutputTrigger(OVP_Algorithm_MatrixAverage_OutputTriggerId_AveragePerformed, "Average performed");

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IAlgorithmDesc, OVP_ClassId_Algorithm_MatrixAverageDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins
