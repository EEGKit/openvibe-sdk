#ifndef __OpenViBEToolkit_Offline_ISignalTrial_H__
#define __OpenViBEToolkit_Offline_ISignalTrial_H__

#include "../ovtkIObject.h"

namespace OpenViBEToolkit
{
	class OVTK_API ISignalTrial : public IObject
	{
	public:

		virtual bool setSamplingRate(const uint32_t ui32SamplingFrequency) = 0;
		virtual bool setChannelCount(const uint32_t ui32ChannelCount) = 0;
		virtual bool setChannelName(const uint32_t index, const char* name) = 0;
		virtual bool setLabelIdentifier(const OpenViBE::CIdentifier& rLabelIdentifier) = 0;
		virtual bool setSampleCount(const uint32_t ui32SampleCount, const bool bPreserve) = 0;

		virtual uint32_t getSamplingRate(void) const = 0;
		virtual uint32_t getChannelCount(void) const = 0;
		virtual const char* getChannelName(const uint32_t index) const = 0;
		virtual OpenViBE::CIdentifier getLabelIdentifier(void) const = 0;
		virtual uint32_t getSampleCount(void) const = 0;
		virtual uint64_t getDuration(void) const = 0;
		virtual double* getChannelSampleBuffer(const uint32_t index) const = 0;

		_IsDerivedFromClass_(OpenViBEToolkit::IObject, OVTK_ClassId_);
	};

	extern OVTK_API ISignalTrial* createSignalTrial(void);
	extern OVTK_API void releaseSignalTrial(ISignalTrial* trial);

	// operations
	extern OVTK_API ISignalTrial& copyHeader(ISignalTrial& trial, const ISignalTrial* srcTrial = nullptr);
	extern OVTK_API ISignalTrial& copy(ISignalTrial& trial, const ISignalTrial* srcTrial = nullptr);
	extern OVTK_API ISignalTrial& selectSamples(ISignalTrial& trial, const uint32_t sampleStart, const uint32_t sampleEnd, const ISignalTrial* srcTrial = nullptr);
	extern OVTK_API ISignalTrial& selectTime(ISignalTrial& trial, const uint64_t timeStart, const uint64_t timeEnd, const ISignalTrial* srcTrial = nullptr);
	extern OVTK_API ISignalTrial& removeSamples(ISignalTrial& trial, const uint32_t sampleStart, const uint32_t sampleEnd, const ISignalTrial* srcTrial = nullptr);
	extern OVTK_API ISignalTrial& removeTime(ISignalTrial& trial, const uint64_t timeStart, const uint64_t timeEnd, const ISignalTrial* srcTrial = nullptr);
	extern OVTK_API ISignalTrial& insertBufferSamples(ISignalTrial& trial, const uint32_t sampleStart, const uint32_t sampleCount, const double* buffer, const ISignalTrial* srcTrial = nullptr);
	extern OVTK_API ISignalTrial& insertBufferTime(ISignalTrial& trial, const uint64_t timeStart, const uint32_t sampleCount, const double* buffer, const ISignalTrial* srcTrial = nullptr);
};

#endif // __OpenViBEToolkit_Offline_ISignalTrial_H__
