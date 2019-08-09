#pragma once

#include "../ovkTKernelObject.h"

#include <vector>
#include <map>

namespace OpenViBE
{
	namespace Metabox
	{

		/**
		 * \brief The CMetaboxObjectDesc virtual BoxAlgorithmDesc for metaboxes
		 *
		 * This class provides a virtual algorithm descriptor for metaboxes. Each metabox-scenario
		 * will result in one of these descriptors. The prototype is created from scenario inputs,
		 * outputs and settings.
		 *
		 * Variables such as name, author etc are pulled from scenario information.
		 */
		class CMetaboxObjectDesc : virtual public IMetaboxObjectDesc
		{
		public:
			CMetaboxObjectDesc() { }

			CMetaboxObjectDesc(const CString& rMetaboxDescriptor, Kernel::IScenario& metaboxScenario);
			void release() override { }
			CString getMetaboxDescriptor() const override { return m_MetaboxDescriptor; }
			CString getName() const override { return m_Name; }
			CString getAuthorName() const override { return m_AuthorName; }
			CString getAuthorCompanyName() const override { return m_AuthorCompanyName; }
			CString getShortDescription() const override { return m_ShortDescription; }
			CString getDetailedDescription() const override { return m_DetailedDescription; }
			CString getCategory() const override { return m_Category; }
			CString getVersion() const override { return m_Version; }
			CString getStockItemName() const override { return m_StockItemName; }
			CString getAddedSoftwareVersion() const override { return m_AddedSoftwareVersion; }
			CString getUpdatedSoftwareVersion() const override { return m_UpdatedSoftwareVersion; }
			CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_Metabox; }
			Plugins::IPluginObject* create() override { return nullptr; }

			// Handling of the virtual prototype

			// Since we have to construct a prototype on the fly, the special Metabox descriptor
			// will also hold the information about the settings, inputs and outputs of the box
			typedef struct _SIOStream
			{
				_SIOStream()
					: m_sName(""),
					  m_oTypeIdentifier(OV_UndefinedIdentifier),
					  m_oIdentifier(OV_UndefinedIdentifier) {}

				_SIOStream(const CString& rName, const CIdentifier& rTypeIdentifier, const CIdentifier& rIdentifier)
					: m_sName(rName),
					  m_oTypeIdentifier(rTypeIdentifier),
					  m_oIdentifier(rIdentifier) {}

				CString m_sName;
				CIdentifier m_oTypeIdentifier = OV_UndefinedIdentifier;
				CIdentifier m_oIdentifier = OV_UndefinedIdentifier;
			} SIOStream;

			typedef struct _SSetting
			{
				_SSetting()
					: m_sName(""),
					  m_oTypeIdentifier(OV_UndefinedIdentifier),
					  m_sDefaultValue(""),
					  m_oIdentifier(OV_UndefinedIdentifier) {}

				_SSetting(const CString& rName, const CIdentifier& rTypeIdentifier, const CString& rDefaultValue, const CIdentifier& rIdentifier)
					: m_sName(rName),
					  m_oTypeIdentifier(rTypeIdentifier),
					  m_sDefaultValue(rDefaultValue),
					  m_oIdentifier(rIdentifier) {}

				CString m_sName;
				CIdentifier m_oTypeIdentifier = OV_UndefinedIdentifier;
				CString m_sDefaultValue;
				CIdentifier m_oIdentifier = OV_UndefinedIdentifier;
			} SSetting;

			bool getBoxPrototype(Kernel::IBoxProto& rBoxAlgorithmPrototype) const override;

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_MetaboxDesc)

		private:
			CString m_MetaboxDescriptor;

			CString m_Name;
			CString m_AuthorName;
			CString m_AuthorCompanyName;
			CString m_ShortDescription;
			CString m_DetailedDescription;
			CString m_Category;
			CString m_Version;
			CString m_StockItemName;
			CString m_AddedSoftwareVersion;
			CString m_UpdatedSoftwareVersion;
			CString m_MetaboxIdentifier;

			std::vector<SIOStream> m_Inputs;
			std::vector<SIOStream> m_Outputs;
			std::vector<SSetting> m_Settings;
		};
	}  // namespace Metabox
}  // namespace OpenViBE
