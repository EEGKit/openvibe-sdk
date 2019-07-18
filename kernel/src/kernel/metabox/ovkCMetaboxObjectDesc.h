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
			virtual void release(void) { }

			virtual CString getMetaboxDescriptor(void) const { return m_MetaboxDescriptor; }

			virtual CString getName(void) const { return m_Name; }
			virtual CString getAuthorName(void) const { return m_AuthorName; }
			virtual CString getAuthorCompanyName(void) const { return m_AuthorCompanyName; }
			virtual CString getShortDescription(void) const { return m_ShortDescription; }
			virtual CString getDetailedDescription(void) const { return m_DetailedDescription; }
			virtual CString getCategory(void) const { return m_Category; }
			virtual CString getVersion(void) const { return m_Version; }
			virtual CString getStockItemName(void) const { return m_StockItemName; }
			virtual CString getAddedSoftwareVersion(void) const { return m_AddedSoftwareVersion; }

			virtual CString getUpdatedSoftwareVersion(void) const { return m_UpdatedSoftwareVersion; }

			virtual CIdentifier getCreatedClass(void) const { return OVP_ClassId_BoxAlgorithm_Metabox; }
			virtual Plugins::IPluginObject* create(void) { return NULL; }

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
				CIdentifier m_oTypeIdentifier;
				CIdentifier m_oIdentifier;
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
				CIdentifier m_oTypeIdentifier;
				CString m_sDefaultValue;
				CIdentifier m_oIdentifier;
			} SSetting;

			virtual bool getBoxPrototype(Kernel::IBoxProto& rBoxAlgorithmPrototype) const;

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_MetaboxDesc);

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
	};
};
