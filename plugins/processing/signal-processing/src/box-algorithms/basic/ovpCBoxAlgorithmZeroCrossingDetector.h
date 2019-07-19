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

#include <string>
#include <vector>
#include <map>

#define OVP_ClassId_BoxAlgorithm_ZeroCrossingDetector     OpenViBE::CIdentifier(0x0016663F, 0x096A46A6)
#define OVP_ClassId_BoxAlgorithm_ZeroCrossingDetectorDesc OpenViBE::CIdentifier(0x63AA73A7, 0x1F0419A2)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmZeroCrossingDetector : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			virtual void release(void) { delete this; }

			virtual bool initialize(void);
			virtual bool uninitialize(void);
			virtual bool processInput(uint32_t ui32InputIndex);
			virtual bool process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ZeroCrossingDetector);

		protected:

			OpenViBEToolkit::TGenericDecoder<CBoxAlgorithmZeroCrossingDetector> m_oDecoder;
			OpenViBEToolkit::TGenericEncoder<CBoxAlgorithmZeroCrossingDetector> m_oEncoder0;
			OpenViBEToolkit::TStimulationEncoder<CBoxAlgorithmZeroCrossingDetector> m_oEncoder1;
			OpenViBEToolkit::TStreamedMatrixEncoder<CBoxAlgorithmZeroCrossingDetector> m_oEncoder2;

			std::vector<double> m_vSignalHistory;
			std::vector<int> m_vStateHistory;
			double m_f64HysteresisThreshold;
			uint64_t m_ui64ChunkCount;

			uint32_t m_ui32SamplingRate;
			double m_f64WindowTime;
			uint32_t m_ui32WindowTime;
			std::vector<std::vector<uint64_t>> m_vMemoryChunk;
			std::vector<std::vector<uint32_t>> m_vMemorySample;

			uint64_t m_ui64StimulationId1;
			uint64_t m_ui64StimulationId2;
		};

		class CBoxAlgorithmZeroCrossingDetectorListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			virtual bool onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getInputType(ui32Index, l_oTypeIdentifier);
				return this->onConnectorTypeChanged(rBox, ui32Index, l_oTypeIdentifier, false);
			}

			virtual bool onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getOutputType(ui32Index, l_oTypeIdentifier);
				return this->onConnectorTypeChanged(rBox, ui32Index, l_oTypeIdentifier, true);
			}

			virtual bool onConnectorTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index, const OpenViBE::CIdentifier& rTypeIdentifier, bool bOutputChanged)
			{
				if (ui32Index == 0)
				{
					if (rTypeIdentifier == OV_TypeId_Signal)
					{
						rBox.setInputType(0, OV_TypeId_Signal);
						rBox.setOutputType(0, OV_TypeId_Signal);
					}
					else if (rTypeIdentifier == OV_TypeId_StreamedMatrix)
					{
						rBox.setInputType(0, OV_TypeId_StreamedMatrix);
						rBox.setOutputType(0, OV_TypeId_StreamedMatrix);
					}
					else
					{
						// Invalid i/o type identifier
						OpenViBE::CIdentifier l_oOriginalTypeIdentifier;
						if (bOutputChanged)
						{
							// Restores output
							rBox.getInputType(0, l_oOriginalTypeIdentifier);
							rBox.setOutputType(0, l_oOriginalTypeIdentifier);
						}
						else
						{
							// Restores input
							rBox.getOutputType(0, l_oOriginalTypeIdentifier);
							rBox.setInputType(0, l_oOriginalTypeIdentifier);
						}
					}
				}
				if (ui32Index == 1)
				{
					rBox.setOutputType(1, OV_TypeId_Stimulations);
				}
				if (ui32Index == 2)
				{
					rBox.setOutputType(2, OV_TypeId_StreamedMatrix);
				}

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CBoxAlgorithmZeroCrossingDetectorDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const { return OpenViBE::CString("Zero-Crossing Detector"); }
			virtual OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Quentin Barthelemy"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("Mensia Technologies SA"); }
			virtual OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString("Detects zero-crossings of the signal"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString("Detects zero-crossings of the signal for each channel, with 1 for positive zero-crossings (negative-to-positive), -1 for negatives ones (positive-to-negative), 0 otherwise. For all channels, stimulations mark positive and negatives zero-crossings. For each channel, the rythm is computed in events per min."); }
			virtual OpenViBE::CString getCategory(void) const { return OpenViBE::CString("Signal processing/Temporal Filtering"); }
			virtual OpenViBE::CString getVersion(void) const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent(void) const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_BoxAlgorithm_ZeroCrossingDetector; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new CBoxAlgorithmZeroCrossingDetector; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const { return new CBoxAlgorithmZeroCrossingDetectorListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Input signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput("Zero-crossing signal", OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput("Zero-crossing stimulations", OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addOutput("Events rythm (per min)", OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addSetting("Hysteresis threshold", OV_TypeId_Float, "0.01");
				rBoxAlgorithmPrototype.addSetting("Rythm estimation window (in sec)", OV_TypeId_Float, "10");
				rBoxAlgorithmPrototype.addSetting("Negative-to-positive stimulation", OV_TypeId_Stimulation, "OVTK_StimulationId_ThresholdPassed_Positive");
				rBoxAlgorithmPrototype.addSetting("Positive-to-negative stimulation", OV_TypeId_Stimulation, "OVTK_StimulationId_ThresholdPassed_Negative");
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ZeroCrossingDetectorDesc);
		};
	};
};


