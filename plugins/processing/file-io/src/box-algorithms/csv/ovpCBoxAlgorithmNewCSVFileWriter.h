#pragma once

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>
#include "csv/ovICSV.h"

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>

#define OVP_ClassId_BoxAlgorithm_NewCSVFileWriter     							   OpenViBE::CIdentifier(0x428375E8, 0x325F2DB9)
#define OVP_ClassId_BoxAlgorithm_NewCSVFileWriterDesc 							   OpenViBE::CIdentifier(0x4B5C1D8F, 0x570E45FD)

namespace OpenViBEPlugins
{
	namespace FileIO
	{
		class CBoxAlgorithmNewCSVFileWriter : public OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >
		{
		public:

			CBoxAlgorithmNewCSVFileWriter(void);
			virtual void release(void) { delete this; }

			virtual bool initialize(void);
			virtual bool uninitialize(void);
			virtual bool processInput(unsigned int inputIndex);
			virtual bool process(void);

			bool processStreamedMatrix(void);
			bool processStimulation(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_NewCSVFileWriter);


		private:
			OpenViBE::CSV::ICSVLib* m_WriterLib;
			bool initializeFile();

			OpenViBE::CString m_Separator;
			OpenViBE::CIdentifier m_TypeIdentifier;
			bool (OpenViBEPlugins::FileIO::CBoxAlgorithmNewCSVFileWriter::*m_RealProcess)(void);

			OpenViBEToolkit::TDecoder < CBoxAlgorithmNewCSVFileWriter >* m_StreamDecoder;
			OpenViBE::CMatrix m_Matrix;		// This represents the properties of the input, no data

			unsigned long long m_Epoch;

			bool m_IsHeaderReceived;
			bool m_IsFileOpen;

		};

		class CBoxAlgorithmNewCSVFileWriterListener : public OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >
		{
		public:

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CBoxAlgorithmNewCSVFileWriterDesc : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const                { return OpenViBE::CString("New CSV File Writer"); }
			virtual OpenViBE::CString getAuthorName(void) const          { return OpenViBE::CString("Victor Herlin"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const   { return OpenViBE::CString("Mensia Technologies SA"); }
			virtual OpenViBE::CString getShortDescription(void) const    { return OpenViBE::CString("Writes signal in a CSV (text based) file"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const            { return OpenViBE::CString("File reading and writing/CSV"); }
			virtual OpenViBE::CString getVersion(void) const             { return OpenViBE::CString("1.0"); }
			
			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_NewCSVFileWriter; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return new OpenViBEPlugins::FileIO::CBoxAlgorithmNewCSVFileWriter; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const               { return new CBoxAlgorithmNewCSVFileWriterListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* boxListener) const { delete boxListener; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& boxAlgorithmPrototype) const
			{
				boxAlgorithmPrototype.addInput  ("Input stream",     OV_TypeId_Signal);
				boxAlgorithmPrototype.addSetting("Filename",         OV_TypeId_Filename, "record-[$core{date}-$core{time}].csv");
				boxAlgorithmPrototype.addSetting("Column separator", OV_TypeId_String, ";");
				boxAlgorithmPrototype.addSetting("Precision",        OV_TypeId_Integer, "10");
				boxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);

				boxAlgorithmPrototype.addInputSupport(OV_TypeId_Signal);
				boxAlgorithmPrototype.addInputSupport(OV_TypeId_StreamedMatrix);
				boxAlgorithmPrototype.addInputSupport(OV_TypeId_Spectrum);
				boxAlgorithmPrototype.addInputSupport(OV_TypeId_Stimulations);
				boxAlgorithmPrototype.addInputSupport(OV_TypeId_FeatureVector);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_NewCSVFileWriterDesc);
		};
	};
};
