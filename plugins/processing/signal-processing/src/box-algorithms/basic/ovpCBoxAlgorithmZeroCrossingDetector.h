/*
 * MENSIA TECHNOLOGIES CONFIDENTIAL
 * ________________________________
 *
 *  [2012] - [2013] Mensia Technologies SA
 *  Copyright, All Rights Reserved.
 *
 * NOTICE: All information contained herein is, and remains
 * the property of Mensia Technologies SA.
 * The intellectual and technical concepts contained
 * herein are proprietary to Mensia Technologies SA
 * and are covered copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Mensia Technologies SA.
 */

#ifndef __OpenViBEPlugins_BoxAlgorithm_ZeroCrossingDetector_H__
#define __OpenViBEPlugins_BoxAlgorithm_ZeroCrossingDetector_H__

#include "../../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include "ovpTGenericCodec.h"

#include <string>
#include <vector>
#include <map>

#define OVP_ClassId_BoxAlgorithm_ZeroCrossingDetector     OpenViBE::CIdentifier(0x0016663F, 0x096A46A6)
#define OVP_ClassId_BoxAlgorithm_ZeroCrossingDetectorDesc OpenViBE::CIdentifier(0x63AA73A7, 0x1F0419A2)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmZeroCrossingDetector : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			virtual void release(void) { delete this; }

			virtual OpenViBE::boolean initialize(void);
			virtual OpenViBE::boolean uninitialize(void);
			virtual OpenViBE::boolean processInput(OpenViBE::uint32 ui32InputIndex);
			virtual OpenViBE::boolean process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ZeroCrossingDetector);

		protected:

			OpenViBEPlugins::TGenericDecoder < CBoxAlgorithmZeroCrossingDetector > m_oDecoder;
			OpenViBEPlugins::TGenericEncoder < CBoxAlgorithmZeroCrossingDetector > m_oEncoder0;
			OpenViBEToolkit::TStimulationEncoder < CBoxAlgorithmZeroCrossingDetector > m_oEncoder1;
			OpenViBEToolkit::TStreamedMatrixEncoder < CBoxAlgorithmZeroCrossingDetector > m_oEncoder2;
			
			std::vector < OpenViBE::float64 > m_vSignalHistory;
			std::vector < int > m_vStateHistory;
			OpenViBE::float64 m_f64HysteresisThreshold;
			OpenViBE::uint64 m_ui64ChunkCount;

			OpenViBE::uint32 m_ui32SamplingRate;
			OpenViBE::float64 m_f64WindowTime;
			OpenViBE::uint32 m_ui32WindowTime;
			std::vector < std::vector < OpenViBE::uint64 > > m_vMemoryChunk;
			std::vector < std::vector < OpenViBE::uint32 > > m_vMemorySample;

			OpenViBE::uint64 m_ui64StimulationId1;
			OpenViBE::uint64 m_ui64StimulationId2;

		};

		class CBoxAlgorithmZeroCrossingDetectorListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			virtual OpenViBE::boolean onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getInputType(ui32Index, l_oTypeIdentifier);
				return this->onConnectorTypeChanged(rBox, ui32Index, l_oTypeIdentifier, false);
			}

			virtual OpenViBE::boolean onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getOutputType(ui32Index, l_oTypeIdentifier);
				return this->onConnectorTypeChanged(rBox, ui32Index, l_oTypeIdentifier, true);
			}

			virtual OpenViBE::boolean onConnectorTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index, const OpenViBE::CIdentifier& rTypeIdentifier, bool bOutputChanged)
			{
				if(ui32Index==0)
				{	
				if( rTypeIdentifier==OV_TypeId_Signal)
				{
					rBox.setInputType(0, OV_TypeId_Signal);
					rBox.setOutputType(0, OV_TypeId_Signal);
				}
				else if(rTypeIdentifier==OV_TypeId_StreamedMatrix)
				{
					rBox.setInputType(0, OV_TypeId_StreamedMatrix);
					rBox.setOutputType(0, OV_TypeId_StreamedMatrix);
				}
				else
				{
					// Invalid i/o type identifier
					OpenViBE::CIdentifier l_oOriginalTypeIdentifier;
					if(bOutputChanged)
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
				if(ui32Index==1)
				{
					rBox.setOutputType(1, OV_TypeId_Stimulations);
				}
				if(ui32Index==2)
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

			virtual OpenViBE::CString getName(void) const                    { return OpenViBE::CString("Zero-Crossing Detector"); }
			virtual OpenViBE::CString getAuthorName(void) const              { return OpenViBE::CString("Quentin Barthelemy"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const       { return OpenViBE::CString("Mensia Technologies SA"); }
			virtual OpenViBE::CString getShortDescription(void) const        { return OpenViBE::CString("Detects zero-crossings of the signal"); }
			virtual OpenViBE::CString getDetailedDescription(void) const     { return OpenViBE::CString("Detects zero-crossings of the signal for each channel, with 1 for positive zero-crossings (negative-to-positive), -1 for negatives ones (positive-to-negative), 0 otherwise. For all channels, stimulations mark positive and negatives zero-crossings. For each channel, the rythm is computed in events per min."); }
			virtual OpenViBE::CString getCategory(void) const                { return OpenViBE::CString("Signal processing/Temporal Filtering"); }
			virtual OpenViBE::CString getVersion(void) const                 { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const    { return OpenViBE::CString("2.1.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const  { return OpenViBE::CString("2.2.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const        { return OVP_ClassId_BoxAlgorithm_ZeroCrossingDetector; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)           { return new OpenViBEPlugins::SignalProcessing::CBoxAlgorithmZeroCrossingDetector; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const { return new CBoxAlgorithmZeroCrossingDetectorListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual OpenViBE::boolean getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput  ("Input signal",                     OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput ("Zero-crossing signal",             OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addOutput ("Zero-crossing stimulations",       OV_TypeId_Stimulations);
				rBoxAlgorithmPrototype.addOutput ("Events rythm (per min)",           OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addSetting("Hysteresis threshold",             OV_TypeId_Float, "0.01");
				rBoxAlgorithmPrototype.addSetting("Rythm estimation window (in sec)", OV_TypeId_Float, "10");
				rBoxAlgorithmPrototype.addSetting("Negative-to-positive stimulation", OV_TypeId_Stimulation, "OVTK_StimulationId_ThresholdPassed_Positive");
				rBoxAlgorithmPrototype.addSetting("Positive-to-negative stimulation", OV_TypeId_Stimulation, "OVTK_StimulationId_ThresholdPassed_Negative");
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanModifyInput);
				rBoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanModifyOutput);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ZeroCrossingDetectorDesc);
		};
	};
};

#endif // __OpenViBEPlugins_BoxAlgorithm_ZeroCrossingDetector_H__
