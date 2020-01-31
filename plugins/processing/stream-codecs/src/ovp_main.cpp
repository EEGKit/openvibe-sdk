#include "algorithms/decoders/ovpCAcquisitionDecoder.h"
#include "algorithms/decoders/ovpCStreamStructureDecoder.h"
#include "algorithms/decoders/ovpCExperimentInfoDecoder.h"
#include "algorithms/decoders/ovpCChannelLocalisationDecoder.h"
#include "algorithms/decoders/ovpCChannelUnitsDecoder.h"
#include "algorithms/decoders/ovpCFeatureVectorDecoder.h"
#include "algorithms/decoders/ovpCSignalDecoder.h"
#include "algorithms/decoders/ovpCSpectrumDecoder.h"
#include "algorithms/decoders/ovpCStimulationDecoder.h"
#include "algorithms/decoders/ovpCStreamedMatrixDecoder.h"
// #include "algorithms/decoders/ovpCMasterAcquisitionDecoder.h"

#include "algorithms/encoders/ovpCAcquisitionEncoder.h"
#include "algorithms/encoders/ovpCExperimentInfoEncoder.h"
#include "algorithms/encoders/ovpCChannelLocalisationEncoder.h"
#include "algorithms/encoders/ovpCChannelUnitsEncoder.h"
#include "algorithms/encoders/ovpCFeatureVectorEncoder.h"
#include "algorithms/encoders/ovpCSignalEncoder.h"
#include "algorithms/encoders/ovpCSpectrumEncoder.h"
#include "algorithms/encoders/ovpCStimulationEncoder.h"
#include "algorithms/encoders/ovpCStreamedMatrixEncoder.h"
#include "algorithms/encoders/ovpCMasterAcquisitionEncoder.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Plugins;

OVP_Declare_Begin()
	OVP_Declare_New(StreamCodecs::CAcquisitionDecoderDesc)
	OVP_Declare_New(StreamCodecs::CStreamStructureDecoderDesc);
	OVP_Declare_New(StreamCodecs::CExperimentInfoDecoderDesc)
	OVP_Declare_New(StreamCodecs::CChannelLocalisationDecoderDesc)
	OVP_Declare_New(StreamCodecs::CChannelUnitsDecoderDesc)
	OVP_Declare_New(StreamCodecs::CFeatureVectorDecoderDesc)
	OVP_Declare_New(StreamCodecs::CSignalDecoderDesc)
	OVP_Declare_New(StreamCodecs::CSpectrumDecoderDesc)
	OVP_Declare_New(StreamCodecs::CStimulationDecoderDesc)
	OVP_Declare_New(StreamCodecs::CStreamedMatrixDecoderDesc)
	// OVP_Declare_New(StreamCodecs::CMasterAcquisitionDecoderDesc);

	OVP_Declare_New(StreamCodecs::CAcquisitionEncoderDesc)
	OVP_Declare_New(StreamCodecs::CExperimentInfoEncoderDesc)
	OVP_Declare_New(StreamCodecs::CChannelLocalisationEncoderDesc)
	OVP_Declare_New(StreamCodecs::CChannelUnitsEncoderDesc)
	OVP_Declare_New(StreamCodecs::CFeatureVectorEncoderDesc)
	OVP_Declare_New(StreamCodecs::CSignalEncoderDesc)
	OVP_Declare_New(StreamCodecs::CSpectrumEncoderDesc)
	OVP_Declare_New(StreamCodecs::CStimulationEncoderDesc)
	OVP_Declare_New(StreamCodecs::CStreamedMatrixEncoderDesc)
	OVP_Declare_New(StreamCodecs::CMasterAcquisitionEncoderDesc);

OVP_Declare_End()
