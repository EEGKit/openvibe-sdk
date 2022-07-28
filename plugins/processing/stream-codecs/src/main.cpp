#include "algorithms/decoders/CAcquisitionDecoder.hpp"
#include "algorithms/decoders/CStreamStructureDecoder.hpp"
#include "algorithms/decoders/CExperimentInfoDecoder.hpp"
#include "algorithms/decoders/CChannelLocalisationDecoder.hpp"
#include "algorithms/decoders/CChannelUnitsDecoder.hpp"
#include "algorithms/decoders/CFeatureVectorDecoder.hpp"
#include "algorithms/decoders/CSignalDecoder.hpp"
#include "algorithms/decoders/CSpectrumDecoder.hpp"
#include "algorithms/decoders/CStimulationDecoder.hpp"
#include "algorithms/decoders/CStreamedMatrixDecoder.hpp"
// #include "algorithms/decoders/ovpCMasterAcquisitionDecoder.h"

#include "algorithms/encoders/CAcquisitionEncoder.hpp"
#include "algorithms/encoders/CExperimentInfoEncoder.hpp"
#include "algorithms/encoders/CChannelLocalisationEncoder.hpp"
#include "algorithms/encoders/CChannelUnitsEncoder.hpp"
#include "algorithms/encoders/CFeatureVectorEncoder.hpp"
#include "algorithms/encoders/CSignalEncoder.hpp"
#include "algorithms/encoders/CSpectrumEncoder.hpp"
#include "algorithms/encoders/CStimulationEncoder.hpp"
#include "algorithms/encoders/CStreamedMatrixEncoder.hpp"
#include "algorithms/encoders/CMasterAcquisitionEncoder.hpp"

namespace OpenViBE {
namespace Plugins {
namespace StreamCodecs {

OVP_Declare_Begin()
	OVP_Declare_New(CAcquisitionDecoderDesc)
	OVP_Declare_New(CStreamStructureDecoderDesc)
	OVP_Declare_New(CExperimentInfoDecoderDesc)
	OVP_Declare_New(CChannelLocalisationDecoderDesc)
	OVP_Declare_New(CChannelUnitsDecoderDesc)
	OVP_Declare_New(CFeatureVectorDecoderDesc)
	OVP_Declare_New(CSignalDecoderDesc)
	OVP_Declare_New(CSpectrumDecoderDesc)
	OVP_Declare_New(CStimulationDecoderDesc)
	OVP_Declare_New(CStreamedMatrixDecoderDesc)
	// OVP_Declare_New(CMasterAcquisitionDecoderDesc)

	OVP_Declare_New(CAcquisitionEncoderDesc)
	OVP_Declare_New(CExperimentInfoEncoderDesc)
	OVP_Declare_New(CChannelLocalisationEncoderDesc)
	OVP_Declare_New(CChannelUnitsEncoderDesc)
	OVP_Declare_New(CFeatureVectorEncoderDesc)
	OVP_Declare_New(CSignalEncoderDesc)
	OVP_Declare_New(CSpectrumEncoderDesc)
	OVP_Declare_New(CStimulationEncoderDesc)
	OVP_Declare_New(CStreamedMatrixEncoderDesc)
	OVP_Declare_New(CMasterAcquisitionEncoderDesc)

OVP_Declare_End()

}  // namespace StreamCodecs
}  // namespace Plugins
}  // namespace OpenViBE
