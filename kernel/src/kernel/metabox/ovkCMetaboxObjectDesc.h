#pragma once

#include "../ovkTKernelObject.h"

#include <vector>

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
		class CMetaboxObjectDesc final : virtual public IMetaboxObjectDesc
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
					: m_name(""),
					  m_typeID(OV_UndefinedIdentifier),
					  m_id(OV_UndefinedIdentifier) {}

				_SIOStream(const CString& name, const CIdentifier& typeID, const CIdentifier& identifier)
					: m_name(name),
					  m_typeID(typeID),
					  m_id(identifier) {}

				CString m_name;
				CIdentifier m_typeID = OV_UndefinedIdentifier;
				CIdentifier m_id     = OV_UndefinedIdentifier;
			} SIOStream;

			typedef struct _SSetting
			{
				_SSetting()
					: m_name(""), m_typeID(OV_UndefinedIdentifier), m_defaultValue(""), m_id(OV_UndefinedIdentifier) {}

				_SSetting(const CString& name, const CIdentifier& typeID, const CString& value, const CIdentifier& id)
					: m_name(name), m_typeID(typeID), m_defaultValue(value), m_id(id) { }

				CString m_name;
				CIdentifier m_typeID = OV_UndefinedIdentifier;
				CString m_defaultValue;
				CIdentifier m_id = OV_UndefinedIdentifier;
			} SSetting;

			bool getBoxPrototype(Kernel::IBoxProto& prototype) const override;

			_IsDerivedFromClass_Final_(IMetaboxObjectDesc, OVP_ClassId_BoxAlgorithm_MetaboxDesc)

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
			CString m_MetaboxID;

			std::vector<SIOStream> m_Inputs;
			std::vector<SIOStream> m_Outputs;
			std::vector<SSetting> m_Settings;
		};
	} // namespace Metabox
} // namespace OpenViBE
