#pragma once

#include "../ovp_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

namespace OpenViBE
{
	namespace Plugins
	{
		namespace Streaming
		{
			class CBoxAlgorithmStreamedMatrixMultiplexer final : virtual public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
			{
			public:
				void release() override { delete this; }
				bool initialize() override;
				bool uninitialize() override;
				bool processInput(const size_t index) override;
				bool process() override;

				_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_StreamedMatrixMultiplexer)

			protected:

				CTime m_lastStartTime = 0;
				CTime m_lastEndTime   = 0;
				bool m_headerSent        = false;
			};

			class CBoxAlgorithmStreamedMatrixMultiplexerListener final : public Toolkit::TBoxListener<IBoxListener>
			{
			public:

				bool check(Kernel::IBox& box) const
				{
					for (size_t i = 0; i < box.getInputCount(); ++i) { box.setInputName(i, ("Input stream " + std::to_string(i + 1)).c_str()); }
					return true;
				}

				bool onInputAdded(Kernel::IBox& box, const size_t index) override
				{
					CIdentifier typeID = OV_UndefinedIdentifier;
					box.getOutputType(0, typeID);
					box.setInputType(index, typeID);
					return this->check(box);
				}

				bool onInputRemoved(Kernel::IBox& box, const size_t /*index*/) override
				{
					CIdentifier typeID = OV_UndefinedIdentifier;
					box.getOutputType(0, typeID);

					while (box.getInputCount() < 2) { box.addInput("", typeID); }

					return this->check(box);
				}

				bool onInputTypeChanged(Kernel::IBox& box, const size_t index) override
				{
					CIdentifier typeID = OV_UndefinedIdentifier;
					box.getInputType(index, typeID);

					if (this->getTypeManager().isDerivedFromStream(typeID, OV_TypeId_StreamedMatrix))
					{
						for (size_t i = 0; i < box.getInputCount(); ++i) { box.setInputType(i, typeID); }

						box.setOutputType(0, typeID);
					}
					else
					{
						box.getOutputType(0, typeID);
						box.setInputType(index, typeID);
					}

					return true;
				}

				bool onOutputTypeChanged(Kernel::IBox& box, const size_t /*index*/) override
				{
					CIdentifier typeID = OV_UndefinedIdentifier;
					box.getOutputType(0, typeID);

					if (this->getTypeManager().isDerivedFromStream(typeID, OV_TypeId_StreamedMatrix))
					{
						for (size_t i = 0; i < box.getInputCount(); ++i) { box.setInputType(i, typeID); }
					}
					else
					{
						box.getInputType(0, typeID);
						box.setOutputType(0, typeID);
					}

					return true;
				}

				_IsDerivedFromClass_Final_(Toolkit::TBoxListener<IBoxListener>, OV_UndefinedIdentifier)
			};

			class CBoxAlgorithmStreamedMatrixMultiplexerDesc final : virtual public IBoxAlgorithmDesc
			{
			public:
				void release() override { }
				CString getName() const override { return CString("Streamed matrix multiplexer"); }
				CString getAuthorName() const override { return CString("Yann Renard"); }
				CString getAuthorCompanyName() const override { return CString("INRIA/IRISA"); }
				CString getShortDescription() const override { return CString("Multiplexes streamed matrix buffers in a new stream"); }
				CString getDetailedDescription() const override { return CString(""); }
				CString getCategory() const override { return CString("Streaming"); }
				CString getVersion() const override { return CString("1.0"); }
				CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
				CString getAddedSoftwareVersion() const override { return CString("0.0.0"); }
				CString getUpdatedSoftwareVersion() const override { return CString("0.0.0"); }
				CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_StreamedMatrixMultiplexer; }
				IPluginObject* create() override { return new CBoxAlgorithmStreamedMatrixMultiplexer; }
				IBoxListener* createBoxListener() const override { return new CBoxAlgorithmStreamedMatrixMultiplexerListener; }
				void releaseBoxListener(IBoxListener* listener) const override { delete listener; }

				bool getBoxPrototype(Kernel::IBoxProto& prototype) const override
				{
					prototype.addInput("Input stream 1", OV_TypeId_StreamedMatrix);
					prototype.addInput("Input stream 2", OV_TypeId_StreamedMatrix);
					prototype.addOutput("Multiplexed streamed matrix", OV_TypeId_StreamedMatrix);
					prototype.addFlag(Kernel::BoxFlag_CanAddInput);
					prototype.addFlag(Kernel::BoxFlag_CanModifyInput);
					prototype.addFlag(Kernel::BoxFlag_CanModifyOutput);

					prototype.addInputSupport(OV_TypeId_StreamedMatrix);
					prototype.addInputSupport(OV_TypeId_ChannelLocalisation);
					prototype.addInputSupport(OV_TypeId_FeatureVector);
					prototype.addInputSupport(OV_TypeId_Signal);
					prototype.addInputSupport(OV_TypeId_Spectrum);

					return true;
				}

				_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_StreamedMatrixMultiplexerDesc)
			};
		} // namespace Streaming
	}  // namespace Plugins
}  // namespace OpenViBE
