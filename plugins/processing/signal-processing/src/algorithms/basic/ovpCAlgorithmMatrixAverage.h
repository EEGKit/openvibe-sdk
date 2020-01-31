#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <vector>
#include <deque>

namespace OpenViBE
{
	namespace Plugins
	{
		namespace SignalProcessing
		{
			class CAlgorithmMatrixAverage final : public Toolkit::TAlgorithm<IAlgorithm>
			{
			public:
				void release() override { delete this; }
				bool initialize() override;
				bool uninitialize() override;
				bool process() override;

				_IsDerivedFromClass_Final_(Toolkit::TAlgorithm<IAlgorithm>, OVP_ClassId_Algorithm_MatrixAverage)

			protected:

				Kernel::TParameterHandler<uint64_t> ip_averagingMethod;
				Kernel::TParameterHandler<uint64_t> ip_matrixCount;
				Kernel::TParameterHandler<IMatrix*> ip_matrix;
				Kernel::TParameterHandler<IMatrix*> op_averagedMatrix;

				std::deque<IMatrix*> m_history;
				std::vector<double> m_averageMatrices;
				size_t m_nAverageSamples = 0;
			};

			class CAlgorithmMatrixAverageDesc final : public IAlgorithmDesc
			{
			public:
				void release() override { }
				CString getName() const override { return CString("Matrix average"); }
				CString getAuthorName() const override { return CString("Yann Renard"); }
				CString getAuthorCompanyName() const override { return CString("INRIA/IRISA"); }
				CString getShortDescription() const override { return CString(""); }
				CString getDetailedDescription() const override { return CString(""); }
				CString getCategory() const override { return CString("Signal processing/Averaging"); }
				CString getVersion() const override { return CString("1.1"); }
				CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
				CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
				CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
				CIdentifier getCreatedClass() const override { return OVP_ClassId_Algorithm_MatrixAverage; }
				IPluginObject* create() override { return new CAlgorithmMatrixAverage(); }

				bool getAlgorithmPrototype(Kernel::IAlgorithmProto& prototype) const override
				{
					prototype.addInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_Matrix, "Matrix", Kernel::ParameterType_Matrix);
					prototype.addInputParameter(OVP_Algorithm_MatrixAverage_InputParameterId_MatrixCount, "Matrix count", Kernel::ParameterType_UInteger);
					prototype.addInputParameter(
						OVP_Algorithm_MatrixAverage_InputParameterId_AveragingMethod, "Averaging Method", Kernel::ParameterType_UInteger);

					prototype.addOutputParameter(OVP_Algorithm_MatrixAverage_OutputParameterId_AveragedMatrix, "Averaged matrix", Kernel::ParameterType_Matrix);

					prototype.addInputTrigger(OVP_Algorithm_MatrixAverage_InputTriggerId_Reset, "Reset");
					prototype.addInputTrigger(OVP_Algorithm_MatrixAverage_InputTriggerId_FeedMatrix, "Feed matrix");
					prototype.addInputTrigger(OVP_Algorithm_MatrixAverage_InputTriggerId_ForceAverage, "Force average");

					prototype.addOutputTrigger(OVP_Algorithm_MatrixAverage_OutputTriggerId_AveragePerformed, "Average performed");

					return true;
				}

				_IsDerivedFromClass_Final_(IAlgorithmDesc, OVP_ClassId_Algorithm_MatrixAverageDesc)
			};
		} // namespace SignalProcessing
	}  // namespace Plugins
}  // namespace OpenViBE
