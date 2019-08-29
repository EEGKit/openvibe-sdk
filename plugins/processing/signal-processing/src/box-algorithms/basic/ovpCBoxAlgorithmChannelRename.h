#pragma once

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include <string>
#include <vector>

#define OVP_ClassId_BoxAlgorithm_ChannelRename     OpenViBE::CIdentifier(0x1FE50479, 0x39040F40)
#define OVP_ClassId_BoxAlgorithm_ChannelRenameDesc OpenViBE::CIdentifier(0x20EA1F00, 0x7AED5645)

namespace OpenViBEPlugins
{
	namespace SignalProcessing
	{
		class CBoxAlgorithmChannelRename final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_ChannelRename)

		protected:

			OpenViBEToolkit::TGenericDecoder<CBoxAlgorithmChannelRename> m_StreamDecoder;
			OpenViBEToolkit::TGenericEncoder<CBoxAlgorithmChannelRename> m_StreamEncoder;

			OpenViBE::CIdentifier m_TypeIdentifier = OV_UndefinedIdentifier;

			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> ip_Matrix;
			OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*> op_Matrix;

			std::vector<std::string> m_ChannelNames;
		};

		class CBoxAlgorithmChannelRenameListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:
			bool onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t index) override
			{
				OpenViBE::CIdentifier l_oTypeIdentifier = OV_UndefinedIdentifier;
				rBox.getInputType(index, l_oTypeIdentifier);
				rBox.setOutputType(index, l_oTypeIdentifier);
				return true;
			}

			bool onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t index) override
			{
				OpenViBE::CIdentifier l_oTypeIdentifier = OV_UndefinedIdentifier;
				rBox.getOutputType(index, l_oTypeIdentifier);
				rBox.setInputType(index, l_oTypeIdentifier);
				return true;
			}


			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmChannelRenameDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Channel Rename"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Renames channels of different types of streamed matrices"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Signal processing/Channels"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.1"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("1.1.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_ChannelRename; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmChannelRename; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmChannelRenameListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* boxListener) const override { delete boxListener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input matrix", OV_TypeId_Signal);
				prototype.addOutput("Output matrix", OV_TypeId_Signal);
				prototype.addSetting("New channel names", OV_TypeId_String, "Channel 1;Channel 2");
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);

				prototype.addInputSupport(OV_TypeId_Signal);
				prototype.addInputSupport(OV_TypeId_Spectrum);
				prototype.addInputSupport(OV_TypeId_StreamedMatrix);
				prototype.addInputSupport(OV_TypeId_TimeFrequency);
				prototype.addInputSupport(OV_TypeId_CovarianceMatrix);

				prototype.addOutputSupport(OV_TypeId_Signal);
				prototype.addOutputSupport(OV_TypeId_Spectrum);
				prototype.addOutputSupport(OV_TypeId_StreamedMatrix);
				prototype.addOutputSupport(OV_TypeId_TimeFrequency);
				prototype.addOutputSupport(OV_TypeId_CovarianceMatrix);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_ChannelRenameDesc)
		};
	} // namespace SignalProcessing
} // namespace OpenViBEPlugins
