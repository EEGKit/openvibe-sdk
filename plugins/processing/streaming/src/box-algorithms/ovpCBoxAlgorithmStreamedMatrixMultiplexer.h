#pragma once

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>
#include <vector>
#include <cstdio>

#define OVP_ClassId_BoxAlgorithm_StreamedMatrixMultiplexer     OpenViBE::CIdentifier(0x7A12298B, 0x785F4D42)
#define OVP_ClassId_BoxAlgorithm_StreamedMatrixMultiplexerDesc OpenViBE::CIdentifier(0x0B420425, 0x3F602DE7)


namespace OpenViBEPlugins
{
	namespace Streaming
	{
		class CBoxAlgorithmStreamedMatrixMultiplexer : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:

			virtual void release(void) { delete this; }

			virtual bool initialize(void);
			virtual bool uninitialize(void);
			virtual bool processInput(uint32_t ui32InputIndex);
			virtual bool process(void);

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_StreamedMatrixMultiplexer);

		protected:

			uint64_t m_lastStartTime;
			uint64_t m_lastEndTime;
			bool m_bHeaderSent;
		};

		class CBoxAlgorithmStreamedMatrixMultiplexerListener : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			bool check(OpenViBE::Kernel::IBox& rBox)
			{
				char l_sName[1024];

				for (uint32_t i = 0; i < rBox.getInputCount(); i++)
				{
					sprintf(l_sName, "Input stream %u", i + 1);
					rBox.setInputName(i, l_sName);
				}

				return true;
			}

			virtual bool onInputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getOutputType(0, l_oTypeIdentifier);
				rBox.setInputType(ui32Index, l_oTypeIdentifier);
				return this->check(rBox);
			};

			virtual bool onInputRemoved(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getOutputType(0, l_oTypeIdentifier);

				while (rBox.getInputCount() < 2)
				{
					rBox.addInput("", l_oTypeIdentifier);
				}

				return this->check(rBox);
			}

			virtual bool onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getInputType(ui32Index, l_oTypeIdentifier);

				if (this->getTypeManager().isDerivedFromStream(l_oTypeIdentifier, OV_TypeId_StreamedMatrix))
				{
					for (uint32_t i = 0; i < rBox.getInputCount(); i++)
					{
						rBox.setInputType(i, l_oTypeIdentifier);
					}

					rBox.setOutputType(0, l_oTypeIdentifier);
				}
				else
				{
					rBox.getOutputType(0, l_oTypeIdentifier);
					rBox.setInputType(ui32Index, l_oTypeIdentifier);
				}

				return true;
			}

			virtual bool onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index)
			{
				OpenViBE::CIdentifier l_oTypeIdentifier;
				rBox.getOutputType(0, l_oTypeIdentifier);

				if (this->getTypeManager().isDerivedFromStream(l_oTypeIdentifier, OV_TypeId_StreamedMatrix))
				{
					for (uint32_t i = 0; i < rBox.getInputCount(); i++)
					{
						rBox.setInputType(i, l_oTypeIdentifier);
					}
				}
				else
				{
					rBox.getInputType(0, l_oTypeIdentifier);
					rBox.setOutputType(0, l_oTypeIdentifier);
				}

				return true;
			};

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier);
		};

		class CBoxAlgorithmStreamedMatrixMultiplexerDesc : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:

			virtual void release(void) { }

			virtual OpenViBE::CString getName(void) const { return OpenViBE::CString("Streamed matrix multiplexer"); }
			virtual OpenViBE::CString getAuthorName(void) const { return OpenViBE::CString("Yann Renard"); }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return OpenViBE::CString("INRIA/IRISA"); }
			virtual OpenViBE::CString getShortDescription(void) const { return OpenViBE::CString("Multiplexes streamed matrix buffers in a new stream"); }
			virtual OpenViBE::CString getDetailedDescription(void) const { return OpenViBE::CString(""); }
			virtual OpenViBE::CString getCategory(void) const { return OpenViBE::CString("Streaming"); }
			virtual OpenViBE::CString getVersion(void) const { return OpenViBE::CString("1.0"); }
			virtual OpenViBE::CString getSoftwareComponent(void) const { return OpenViBE::CString("openvibe-sdk"); }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }
			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return OpenViBE::CString("0.0.0"); }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_BoxAlgorithm_StreamedMatrixMultiplexer; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return new CBoxAlgorithmStreamedMatrixMultiplexer; }
			virtual OpenViBE::Plugins::IBoxListener* createBoxListener(void) const { return new CBoxAlgorithmStreamedMatrixMultiplexerListener; }
			virtual void releaseBoxListener(OpenViBE::Plugins::IBoxListener* pBoxListener) const { delete pBoxListener; }

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				rBoxAlgorithmPrototype.addInput("Input stream 1", OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addInput("Input stream 2", OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addOutput("Multiplexed streamed matrix", OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				rBoxAlgorithmPrototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);

				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_StreamedMatrix);
				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_ChannelLocalisation);
				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_FeatureVector);
				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_Signal);
				rBoxAlgorithmPrototype.addInputSupport(OV_TypeId_Spectrum);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StreamedMatrixMultiplexerDesc);
		};
	};
};


