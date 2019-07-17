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
		class CMetaboxObjectDesc : virtual public OpenViBE::Metabox::IMetaboxObjectDesc
		{
		public:
			CMetaboxObjectDesc() { }

			CMetaboxObjectDesc(const OpenViBE::CString& rMetaboxDescriptor, OpenViBE::Kernel::IScenario& metaboxScenario);
			virtual void release(void) { }

			virtual OpenViBE::CString getMetaboxDescriptor(void) const { return m_MetaboxDescriptor; }

			virtual OpenViBE::CString getName(void) const { return m_Name; }
			virtual OpenViBE::CString getAuthorName(void) const { return m_AuthorName; }
			virtual OpenViBE::CString getAuthorCompanyName(void) const { return m_AuthorCompanyName; }
			virtual OpenViBE::CString getShortDescription(void) const { return m_ShortDescription; }
			virtual OpenViBE::CString getDetailedDescription(void) const { return m_DetailedDescription; }
			virtual OpenViBE::CString getCategory(void) const { return m_Category; }
			virtual OpenViBE::CString getVersion(void) const { return m_Version; }
			virtual OpenViBE::CString getStockItemName(void) const { return m_StockItemName; }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const { return m_AddedSoftwareVersion; }

			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return m_UpdatedSoftwareVersion; }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const { return OVP_ClassId_BoxAlgorithm_Metabox; }
			virtual OpenViBE::Plugins::IPluginObject* create(void) { return NULL; }

			// Handling of the virtual prototype

			// Since we have to construct a prototype on the fly, the special Metabox descriptor
			// will also hold the information about the settings, inputs and outputs of the box
			typedef struct _SIOStream
			{
				_SIOStream()
					: m_sName(""),
					  m_oTypeIdentifier(OV_UndefinedIdentifier),
					  m_oIdentifier(OV_UndefinedIdentifier) {}

				_SIOStream(const OpenViBE::CString& rName, const OpenViBE::CIdentifier& rTypeIdentifier, const OpenViBE::CIdentifier& rIdentifier)
					: m_sName(rName),
					  m_oTypeIdentifier(rTypeIdentifier),
					  m_oIdentifier(rIdentifier) {}

				OpenViBE::CString m_sName;
				OpenViBE::CIdentifier m_oTypeIdentifier;
				OpenViBE::CIdentifier m_oIdentifier;
			} SIOStream;

			typedef struct _SSetting
			{
				_SSetting()
					: m_sName(""),
					  m_oTypeIdentifier(OV_UndefinedIdentifier),
					  m_sDefaultValue(""),
					  m_oIdentifier(OV_UndefinedIdentifier) {}

				_SSetting(const OpenViBE::CString& rName, const OpenViBE::CIdentifier& rTypeIdentifier, const OpenViBE::CString& rDefaultValue, const OpenViBE::CIdentifier& rIdentifier)
					: m_sName(rName),
					  m_oTypeIdentifier(rTypeIdentifier),
					  m_sDefaultValue(rDefaultValue),
					  m_oIdentifier(rIdentifier) {}

				OpenViBE::CString m_sName;
				OpenViBE::CIdentifier m_oTypeIdentifier;
				OpenViBE::CString m_sDefaultValue;
				OpenViBE::CIdentifier m_oIdentifier;
			} SSetting;

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const;

			_IsDerivedFromClass_Final_(OpenViBE::Plugins::IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_MetaboxDesc);

		private:
			OpenViBE::CString m_MetaboxDescriptor;

			OpenViBE::CString m_Name;
			OpenViBE::CString m_AuthorName;
			OpenViBE::CString m_AuthorCompanyName;
			OpenViBE::CString m_ShortDescription;
			OpenViBE::CString m_DetailedDescription;
			OpenViBE::CString m_Category;
			OpenViBE::CString m_Version;
			OpenViBE::CString m_StockItemName;
			OpenViBE::CString m_AddedSoftwareVersion;
			OpenViBE::CString m_UpdatedSoftwareVersion;
			OpenViBE::CString m_MetaboxIdentifier;

			std::vector<SIOStream> m_Inputs;
			std::vector<SIOStream> m_Outputs;
			std::vector<SSetting> m_Settings;
		};
	};
};
