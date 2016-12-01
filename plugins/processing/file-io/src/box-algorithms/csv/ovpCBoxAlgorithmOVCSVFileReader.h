#pragma once

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <memory>

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include "csv/ovICSV.h"

#define OVP_ClassId_BoxAlgorithm_OVCSVFileReaderDesc 							   OpenViBE::CIdentifier(0x584E1948, 0x65E91650)
#define OVP_ClassId_BoxAlgorithm_OVCSVFileReader     							   OpenViBE::CIdentifier(0x336A3D9A, 0x753F1BA4)

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CBoxAlgorithmOVCSVFileReader final : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			CBoxAlgorithmOVCSVFileReader(void);

			virtual void release(void) { delete this; }

			virtual unsigned long long getClockFrequency(void);
			virtual bool initialize(void);
			virtual bool uninitialize(void);
			virtual bool processClock(OpenViBE::CMessageClock& messageClock);
			virtual bool process(void);

			bool processSignal(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_OVCSVFileReader);


		private:
			bool initializeFile();
			std::unique_ptr<OpenViBE::CSV::ICSVLib, decltype(&OpenViBE::CSV::releaseCSVLib)>m_ReaderLib;

			OpenViBE::CIdentifier m_TypeIdentifier;
			unsigned int m_SamplingRate;
			unsigned int m_SampleCountPerBuffer;
			unsigned int m_ColumnCount;

			OpenViBEToolkit::TEncoder < CBoxAlgorithmOVCSVFileReader >* m_AlgorithmEncoder;

			bool m_IsHeaderSent;
			std::vector<std::string> m_ChannelNames;
		};

		class CBoxAlgorithmOVCSVFileReaderListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			virtual bool onOutputTypeChanged(OpenViBE::Kernel::IBox& Box, const unsigned int index)
			{
				OpenViBE::CIdentifier typeIdentifier;
				Box.getOutputType(index, typeIdentifier);
				if (typeIdentifier != OV_TypeId_Signal)
				{
					Box.setOutputType(index, OV_TypeId_Signal);
				}
				else
				{
					OV_ERROR_KRF("Unsupported stream type " << typeIdentifier.toString(),
						OpenViBE::Kernel::ErrorType::BadOutput
						);
				}
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmOVCSVFileReaderDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("New CSV File Reader"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Victor Herlin"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("Mensia Technologies SA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Read signal in a CSV (text based) file"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("File reading and writing/CSV"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_OVCSVFileReader; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::FileIO::CBoxAlgorithmOVCSVFileReader; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmOVCSVFileReaderListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual bool getBoxPrototype(
				OpenViBE::Kernel::IBoxProto& BoxAlgorithmPrototype) const
			{
				BoxAlgorithmPrototype.addOutput ("Output stream", OV_TypeId_Signal);
				BoxAlgorithmPrototype.addSetting("Filename", OV_TypeId_Filename, "");

				BoxAlgorithmPrototype.addFlag   (OpenViBE::Kernel::BoxFlag_CanModifyOutput);

				BoxAlgorithmPrototype.addOutputSupport(OV_TypeId_Signal);
				BoxAlgorithmPrototype.addOutputSupport(OV_TypeId_Spectrum);
				BoxAlgorithmPrototype.addOutputSupport(OV_TypeId_FeatureVector);
				BoxAlgorithmPrototype.addOutputSupport(OV_TypeId_StreamedMatrix);
				BoxAlgorithmPrototype.addOutputSupport(OV_TypeId_CovarianceMatrix);
				BoxAlgorithmPrototype.addOutputSupport(OV_TypeId_Stimulations);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_OVCSVFileReaderDesc);
		};
	};
};
