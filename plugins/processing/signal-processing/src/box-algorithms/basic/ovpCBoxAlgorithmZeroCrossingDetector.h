/*********************************************************************
 * Software License Agreement (AGPL-3 License)
 *
 * OpenViBE SDK
 * Based on OpenViBE V1.1.0, Copyright (C) Inria, 2006-2015
 * Copyright (C) Inria, 2015-2017,V1.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <vector>

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmZeroCrossingDetector final : public OpenViBE::Toolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const size_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ZeroCrossingDetector)

		protected:

			OpenViBE::Toolkit::TGenericDecoder<CBoxAlgorithmZeroCrossingDetector> m_decoder;
			OpenViBE::Toolkit::TGenericEncoder<CBoxAlgorithmZeroCrossingDetector> m_encoder0;
			OpenViBE::Toolkit::TStimulationEncoder<CBoxAlgorithmZeroCrossingDetector> m_encoder1;
			OpenViBE::Toolkit::TStreamedMatrixEncoder<CBoxAlgorithmZeroCrossingDetector> m_encoder2;

			std::vector<double> m_signals;
			std::vector<int> m_states;
			double m_hysteresis = 0;
			uint64_t m_nChunk            = 0;

			size_t m_sampling   = 0;
			double m_windowTimeD = 0;
			size_t m_windowTime = 0;
			std::vector<std::vector<size_t>> m_chunks;
			std::vector<std::vector<size_t>> m_samples;

			uint64_t m_stimId1 = 0;
			uint64_t m_stimId2 = 0;
		};

		class CBoxAlgorithmZeroCrossingDetectorListener final : public OpenViBE::Toolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onInputTypeChanged(OpenViBE::Kernel::IBox& box, const size_t index) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getInputType(index, typeID);
				return onConnectorTypeChanged(box, index, typeID, false);
			}

			bool onOutputTypeChanged(OpenViBE::Kernel::IBox& box, const size_t index) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getOutputType(index, typeID);
				return onConnectorTypeChanged(box, index, typeID, true);
			}

			static bool onConnectorTypeChanged(OpenViBE::Kernel::IBox& box, const size_t index, const OpenViBE::CIdentifier& typeID, const bool outputChanged)
			{
				if (index == 0)
				{
					if (typeID == OV_TypeId_Signal)
					{
						box.setInputType(0, OV_TypeId_Signal);
						box.setOutputType(0, OV_TypeId_Signal);
					}
					else if (typeID == OV_TypeId_StreamedMatrix)
					{
						box.setInputType(0, OV_TypeId_StreamedMatrix);
						box.setOutputType(0, OV_TypeId_StreamedMatrix);
					}
					else
					{
						// Invalid i/o type identifier
						OpenViBE::CIdentifier originalTypeID = OV_UndefinedIdentifier;
						if (outputChanged)
						{
							// Restores output
							box.getInputType(0, originalTypeID);
							box.setOutputType(0, originalTypeID);
						}
						else
						{
							// Restores input
							box.getOutputType(0, originalTypeID);
							box.setInputType(0, originalTypeID);
						}
					}
				}
				if (index == 1) { box.setOutputType(1, OV_TypeId_Stimulations); }
				if (index == 2) { box.setOutputType(2, OV_TypeId_StreamedMatrix); }

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Toolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmZeroCrossingDetectorDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Zero-Crossing Detector"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Quentin Barthelemy"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("Mensia Technologies SA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Detects zero-crossings of the signal"); }

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString(
					"Detects zero-crossings of the signal for each channel, with 1 for positive zero-crossings (negative-to-positive), -1 for negatives ones (positive-to-negative), 0 otherwise. For all channels, stimulations mark positive and negatives zero-crossings. For each channel, the rythm is computed in events per min.");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Temporal Filtering"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_ZeroCrossingDetector; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmZeroCrossingDetector; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmZeroCrossingDetectorListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input signal", OV_TypeId_Signal);
				prototype.addOutput("Zero-crossing signal", OV_TypeId_Signal);
				prototype.addOutput("Zero-crossing stimulations", OV_TypeId_Stimulations);
				prototype.addOutput("Events rythm (per min)", OV_TypeId_StreamedMatrix);
				prototype.addSetting("Hysteresis threshold", OV_TypeId_Float, "0.01");
				prototype.addSetting("Rythm estimation window (in sec)", OV_TypeId_Float, "10");
				prototype.addSetting("Negative-to-positive stimulation", OV_TypeId_Stimulation, "OVTK_StimulationId_ThresholdPassed_Positive");
				prototype.addSetting("Positive-to-negative stimulation", OV_TypeId_Stimulation, "OVTK_StimulationId_ThresholdPassed_Negative");
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ZeroCrossingDetectorDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins
