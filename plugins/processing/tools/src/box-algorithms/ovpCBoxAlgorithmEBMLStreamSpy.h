#pragma once

#include "../ovp_defines.h"
#include <toolkit/ovtk_all.h>
#include <ebml/IReader.h>
#include <ebml/IReaderHelper.h>

#include <stack>
#include <map>
#include <string>

namespace OpenViBEPlugins
{
	namespace Tools
	{
		class CBoxAlgorithmEBMLStreamSpy final : public OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, virtual public EBML::IReaderCallback
		{
		public:

			CBoxAlgorithmEBMLStreamSpy() { }
			void release() override { delete this; }
			bool initialize() override;
			bool uninitialize() override;
			bool isMasterChild(const EBML::CIdentifier& identifier) override;
			void openChild(const EBML::CIdentifier& identifier) override;
			void processChildData(const void* buffer, const size_t size) override;
			void closeChild() override { m_nodes.pop(); }
			bool processInput(const size_t index) override;
			bool process() override;

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxAlgorithm<OpenViBE::Plugins::IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_EBMLStreamSpy)

		protected:

			template <class T>
			void processBinaryBlock(const void* buffer, size_t size);

			std::stack<EBML::CIdentifier> m_nodes;
			std::map<EBML::CIdentifier, std::string> m_names;
			std::map<EBML::CIdentifier, std::string> m_types;
			size_t m_nExpandValues                  = 0;
			OpenViBE::Kernel::ELogLevel m_logLevel = OpenViBE::Kernel::ELogLevel::LogLevel_None;
			EBML::IReader* m_reader                 = nullptr;
			EBML::IReaderHelper* m_helper           = nullptr;
		};

		class CBoxAlgorithmEBMLStreamSpyListener final : public OpenViBEToolkit::TBoxListener<OpenViBE::Plugins::IBoxListener>
		{
		public:

			bool check(OpenViBE::Kernel::IBox& box) const
			{
				for (size_t i = 0; i < box.getInputCount(); ++i)
				{
					box.setInputName(i, ("Spied EBML stream " + std::to_string(i + 1)).c_str());
					box.setInputType(i, OV_TypeId_EBMLStream);
				}

				return true;
			}

			bool onInputRemoved(OpenViBE::Kernel::IBox& box, const size_t /*index*/) override { return this->check(box); }
			bool onInputAdded(OpenViBE::Kernel::IBox& box, const size_t /*index*/) override { return this->check(box); }

			_IsDerivedFromClass_Final_(OpenViBEToolkit::TBoxListener < OpenViBE::Plugins::IBoxListener >, OV_UndefinedIdentifier)
		};

		class CBoxAlgorithmEBMLStreamSpyDesc final : public OpenViBE::Plugins::IBoxAlgorithmDesc
		{
		public:
			void release() override { }
			OpenViBE::CString getName() const override { return OpenViBE::CString("EBML stream spy"); }
			OpenViBE::CString getAuthorName() const override { return OpenViBE::CString("Yann Renard"); }
			OpenViBE::CString getAuthorCompanyName() const override { return OpenViBE::CString("INRIA/IRISA"); }
			OpenViBE::CString getShortDescription() const override { return OpenViBE::CString("EBML stream tree viewer"); }

			OpenViBE::CString getDetailedDescription() const override
			{
				return OpenViBE::CString("This sample EBML stream analyzer prints the EBML tree structure to the console");
			}

			OpenViBE::CString getCategory() const override { return OpenViBE::CString("Tools"); }
			OpenViBE::CString getVersion() const override { return OpenViBE::CString("1.0"); }
			OpenViBE::CString getSoftwareComponent() const override { return OpenViBE::CString("openvibe-sdk"); }
			OpenViBE::CString getAddedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CString getUpdatedSoftwareVersion() const override { return OpenViBE::CString("0.0.0"); }
			OpenViBE::CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_EBMLStreamSpy; }
			OpenViBE::Plugins::IPluginObject* create() override { return new CBoxAlgorithmEBMLStreamSpy(); }
			OpenViBE::Plugins::IBoxListener* createBoxListener() const override { return new CBoxAlgorithmEBMLStreamSpyListener; }
			void releaseBoxListener(OpenViBE::Plugins::IBoxListener* listener) const override { delete listener; }

			bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& prototype) const override
			{
				prototype.addInput("Spied EBML stream 1", OV_TypeId_EBMLStream);
				prototype.addSetting("EBML nodes description", OV_TypeId_Filename, "${Path_Data}/plugins/tools/config-ebml-stream-spy.txt");
				prototype.addSetting("Log level to use", OV_TypeId_LogLevel, "Information");
				prototype.addSetting("Expand binary blocks", OV_TypeId_Boolean, "false");
				prototype.addSetting("Number of values in expanded blocks", OV_TypeId_Integer, "4");
				prototype.addFlag(OpenViBE::Kernel::BoxFlag_CanAddInput);
				return true;
			}

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_EBMLStreamSpyDesc)
		};
	} // namespace Tools
} // namespace OpenViBEPlugins
