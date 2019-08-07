#pragma once

#include "ovpCStreamedMatrixDecoder.h"
#include <iomanip>

#define OVP_ClassId_Algorithm_SpectrumStreamDecoder                                         OpenViBE::CIdentifier(0x128202DB, 0x449FC7A6)
#define OVP_ClassId_Algorithm_SpectrumStreamDecoderDesc                                     OpenViBE::CIdentifier(0x54D18EE8, 0x5DBD913A)
#define OVP_Algorithm_SpectrumStreamDecoder_OutputParameterId_FrequencyAbscissa             OpenViBE::CIdentifier(0x14A572E4, 0x5C405C8E)
#define OVP_Algorithm_SpectrumStreamDecoder_OutputParameterId_SamplingRate                  OpenViBE::CIdentifier(0x68442C12, 0x0D9A46DE)

namespace OpenViBEPlugins
{
	namespace StreamCodecs
	{
		class CSpectrumDecoder : public CStreamedMatrixDecoder
		{
		public:

			virtual void release() { delete this; }

			virtual bool initialize();
			virtual bool uninitialize();

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoder, OVP_ClassId_Algorithm_SpectrumStreamDecoder)

			// ebml callbacks
			virtual bool isMasterChild(const EBML::CIdentifier& rIdentifier);
			virtual void openChild(const EBML::CIdentifier& rIdentifier);
			virtual void processChildData(const void* pBuffer, uint64_t ui64BufferSize);
			virtual void closeChild();

		protected:

			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_pFrequencyAbscissa;
			OpenViBE::Kernel::TParameterHandler<uint64_t> op_pSamplingRate;


		private:

			std::stack<EBML::CIdentifier> m_vNodes;

			uint32_t m_ui32FrequencyBandIndex;

			// Value of the current lower frequency of the band. Only used to read old spectrum format.
			double m_lowerFreq;
		};

		class CSpectrumDecoderDesc : public CStreamedMatrixDecoderDesc
		{
		public:

			virtual void release() { }

			virtual OpenViBE::CString getName() const { return OpenViBE::CString("Spectrum stream decoder"); }
			virtual OpenViBE::CString getAuthorName() const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName() const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription() const { return OpenViBE::CString("Decodes the Spectrum type streams."); }
			virtual OpenViBE::CString getDetailedDescription() const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory() const { return OpenViBE::CString("Stream codecs/Decoders"); }
			virtual OpenViBE::CString getVersion() const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent() const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion() const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion() const { return OpenViBE::CString("0.1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass() const { return OVP_ClassId_Algorithm_SpectrumStreamDecoder; }
			virtual OpenViBE::Plugins::IPluginObject* create() { return new CSpectrumDecoder(); }

			virtual bool getAlgorithmPrototype(OpenViBE::Kernel::IAlgorithmProto& rAlgorithmPrototype) const
			{
				CStreamedMatrixDecoderDesc::getAlgorithmPrototype(rAlgorithmPrototype);

				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_SpectrumStreamDecoder_OutputParameterId_FrequencyAbscissa, "Frequency abscissa", OpenViBE::Kernel::ParameterType_Matrix);
				rAlgorithmPrototype.addOutputParameter(OVP_Algorithm_SpectrumStreamDecoder_OutputParameterId_SamplingRate, "Sampling rate", OpenViBE::Kernel::ParameterType_UInteger);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEPlugins::StreamCodecs::CStreamedMatrixDecoderDesc, OVP_ClassId_Algorithm_SpectrumStreamDecoderDesc)
		};
	} // namespace StreamCodecs
} // namespace OpenViBEPlugins
