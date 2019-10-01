#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>
#include <cstdio>

namespace OpenViBEPlugins
{
	namespace Streaming
	{
		class CBoxAlgorithmStreamedMatrixMultiplexer final : virtual public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>
		{
		public:
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool processInput(const uint32_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm < OpenViBE::Plugins::IBoxAlgorithm >, OVP_ClassId_BoxAlgorithm_StreamedMatrixMultiplexer)

		protected:

			uint64_t m_lastStartTime = 0;
			uint64_t m_lastEndTime   = 0;
			bool m_bHeaderSent       = false;
		};

		class CBoxAlgorithmStreamedMatrixMultiplexerListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			bool check(OpenViBE::Kernel::IBox& box)
			{
				for (uint32_t i = 0; i < box.getInputCount(); i++) { box.setInputName(i, ("Input stream " + std::to_string(i + 1)).c_str()); }
				return true;
			}

			bool onInputAdded(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getOutputType(0, typeID);
				box.setInputType(index, typeID);
				return this->check(box);
			}

			bool onInputRemoved(OpenViBE::Kernel::IBox& box, const uint32_t /*index*/) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getOutputType(0, typeID);

				while (box.getInputCount() < 2) { box.addInput("", typeID); }

				return this->check(box);
			}

			bool onInputTypeChanged(OpenViBE::Kernel::IBox& box, const uint32_t index) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getInputType(index, typeID);

				if (this->getTypeManager().isDerivedFromStream(typeID, OV_TypeId_StreamedMatrix))
				{
					for (uint32_t i = 0; i < box.getInputCount(); i++) { box.setInputType(i, typeID); }

					box.setOutputType(0, typeID);
				}
				else
				{
					box.getOutputType(0, typeID);
					box.setInputType(index, typeID);
				}

				return true;
			}

			bool onOutputTypeChanged(OpenViBE::Kernel::IBox& box, const uint32_t /*index*/) override
			{
				OpenViBE::CIdentifier typeID = OV_UndefinedIdentifier;
				box.getOutputType(0, typeID);

				if (this->getTypeManager().isDerivedFromStream(typeID, OV_TypeId_StreamedMatrix))
				{
					for (uint32_t i = 0; i < box.getInputCount(); i++) { box.setInputType(i, typeID); }
				}
				else
				{
					box.getInputType(0, typeID);
					box.setOutputType(0, typeID);
				}

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmStreamedMatrixMultiplexerDesc final : virtual public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("Streamed matrix multiplexer"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("Multiplexes streamed matrix buffers in a new stream"); }
			OpenViBE::CString getDetailedDescription() const override { return OpenViBE::CString(""); }
			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Streaming"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_StreamedMatrixMultiplexer; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmStreamedMatrixMultiplexer; }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmStreamedMatrixMultiplexerListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Input stream 1", OV_TypeId_StreamedMatrix);
				prototype.addInput("Input stream 2", OV_TypeId_StreamedMatrix);
				prototype.addOutput("Multiplexed streamed matrix", OV_TypeId_StreamedMatrix);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyInput);
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanModifyOutput);

				prototype.addInputSupport(OV_TypeId_StreamedMatrix);
				prototype.addInputSupport(OV_TypeId_ChannelLocalisation);
				prototype.addInputSupport(OV_TypeId_FeatureVector);
				prototype.addInputSupport(OV_TypeId_Signal);
				prototype.addInputSupport(OV_TypeId_Spectrum);

				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StreamedMatrixMultiplexerDesc)
		};
	} // namespace Streaming
}
// namespace OpenViBEPlugins
