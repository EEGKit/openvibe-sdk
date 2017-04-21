#ifndef __OpenViBEKernel_Kernel_Metabox_CMetaboxManager_H__
#define __OpenViBEKernel_Kernel_Metabox_CMetaboxManager_H__

#include "../ovkTKernelObject.h"

#include <vector>
#include <map>

#define OVP_ScenarioImportContext_OnLoadMetaboxImport OpenViBE::CIdentifier(0xED48480A, 0x9F6DBAA7)

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
			CMetaboxObjectDesc()
			{
			}

			CMetaboxObjectDesc(const OpenViBE::CString& rMetaboxDescriptor, OpenViBE::Kernel::IScenario& metaboxScenario)
				:	m_MetaboxDescriptor(rMetaboxDescriptor)
					, m_Name(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_Name))
					, m_AuthorName(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_Author))
					, m_AuthorCompanyName(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_Company))
					, m_ShortDescription(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_ShortDescription))
					, m_DetailedDescription(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_DetailedDescription))
					, m_Category( metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_Category))
					, m_Version(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_Version))
					, m_StockItemName("")
					, m_AddedSoftwareVersion(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_AddedSoftwareVersion))
					, m_UpdatedSoftwareVersion(metaboxScenario.getAttributeValue(OV_AttributeId_Scenario_UpdatedSoftwareVersion))
					, m_MetaboxIdentifier(metaboxScenario.getAttributeValue(OVP_AttributeId_Metabox_Identifier))
			{

				for (uint32 l_ui32ScenarioInputIndex = 0; l_ui32ScenarioInputIndex < metaboxScenario.getInputCount(); l_ui32ScenarioInputIndex++)
				{
					CString l_sInputName;
					CIdentifier l_oInputTypeIdentifier;

					metaboxScenario.getInputType(l_ui32ScenarioInputIndex, l_oInputTypeIdentifier);
					metaboxScenario.getInputName(l_ui32ScenarioInputIndex, l_sInputName);

					m_Inputs.push_back(SIOStream(l_sInputName, l_oInputTypeIdentifier));
				}

				for (uint32 l_ui32ScenarioOutputIndex = 0; l_ui32ScenarioOutputIndex < metaboxScenario.getOutputCount(); l_ui32ScenarioOutputIndex++)
				{
					CString l_sOutputName;
					CIdentifier l_oOutputTypeIdentifier;

					metaboxScenario.getOutputType(l_ui32ScenarioOutputIndex, l_oOutputTypeIdentifier);
					metaboxScenario.getOutputName(l_ui32ScenarioOutputIndex, l_sOutputName);

					m_Outputs.push_back(SIOStream(l_sOutputName, l_oOutputTypeIdentifier));
				}

				for (uint32 l_ui32ScenarioSettingIndex = 0; l_ui32ScenarioSettingIndex < metaboxScenario.getSettingCount(); l_ui32ScenarioSettingIndex++)
				{
					CString l_sSettingName;
					CIdentifier l_oSettingTypeIdentifier;
					CString l_sSettingDefaultValue;

					metaboxScenario.getSettingName(l_ui32ScenarioSettingIndex, l_sSettingName);
					metaboxScenario.getSettingType(l_ui32ScenarioSettingIndex, l_oSettingTypeIdentifier);
					metaboxScenario.getSettingDefaultValue(l_ui32ScenarioSettingIndex, l_sSettingDefaultValue);

					m_Settings.push_back(SSetting(l_sSettingName, l_oSettingTypeIdentifier, l_sSettingDefaultValue));
				}
			}

			virtual void release(void) { }

			virtual OpenViBE::CString getMetaboxDescriptor(void) const              { return m_MetaboxDescriptor; }

			virtual OpenViBE::CString getName(void) const                   { return m_Name; }
			virtual OpenViBE::CString getAuthorName(void) const             { return m_AuthorName; }
			virtual OpenViBE::CString getAuthorCompanyName(void) const      { return m_AuthorCompanyName; }
			virtual OpenViBE::CString getShortDescription(void) const       { return m_ShortDescription; }
			virtual OpenViBE::CString getDetailedDescription(void) const    { return m_DetailedDescription; }
			virtual OpenViBE::CString getCategory(void) const               { return m_Category; }
			virtual OpenViBE::CString getVersion(void) const                { return m_Version; }
			virtual OpenViBE::CString getStockItemName(void) const          { return m_StockItemName; }
			virtual OpenViBE::CString getAddedSoftwareVersion(void) const   { return m_AddedSoftwareVersion; }

			virtual OpenViBE::CString getUpdatedSoftwareVersion(void) const { return m_UpdatedSoftwareVersion; }

			virtual OpenViBE::CIdentifier getCreatedClass(void) const    { return OVP_ClassId_BoxAlgorithm_Metabox; }
			virtual OpenViBE::Plugins::IPluginObject* create(void)       { return NULL; }

			// Handling of the virtual prototype

			// Since we have to construct a prototype on the fly, the special Metabox descriptor
			// will also hold the information about the settings, inputs and outputs of the box
			typedef struct _SIOStream
			{
				_SIOStream()
					: m_sName(""),
					  m_oTypeIdentifier(OV_UndefinedIdentifier)
				{}

				_SIOStream(const OpenViBE::CString& rName, const OpenViBE::CIdentifier& rTypeIdentifier)
					: m_sName(rName),
					  m_oTypeIdentifier(rTypeIdentifier)
				{}

				OpenViBE::CString m_sName;
				OpenViBE::CIdentifier m_oTypeIdentifier;

			} SIOStream;

			typedef struct _SSetting
			{
				_SSetting()
					: m_sName(""),
					  m_oTypeIdentifier(OV_UndefinedIdentifier),
					  m_sDefaultValue("")
				{}

				_SSetting(const OpenViBE::CString& rName, const OpenViBE::CIdentifier& rTypeIdentifier, const OpenViBE::CString& rDefaultValue)
					: m_sName(rName),
					  m_oTypeIdentifier(rTypeIdentifier),
					  m_sDefaultValue(rDefaultValue)
				{}

				OpenViBE::CString m_sName;
				OpenViBE::CIdentifier m_oTypeIdentifier;
				OpenViBE::CString m_sDefaultValue;

			} SSetting;

			virtual bool getBoxPrototype(OpenViBE::Kernel::IBoxProto& rBoxAlgorithmPrototype) const
			{
				for (auto& input : m_Inputs)
				{
					rBoxAlgorithmPrototype.addInput(input.m_sName, input.m_oTypeIdentifier);
				}

				for (auto& output : m_Outputs)
				{
					rBoxAlgorithmPrototype.addOutput(output.m_sName, output.m_oTypeIdentifier);
				}

				for (auto& setting : m_Settings)
				{
					rBoxAlgorithmPrototype.addSetting(setting.m_sName, setting.m_oTypeIdentifier, setting.m_sDefaultValue);
				}


				return true;
			}

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

	namespace Kernel
	{
		class CMetaboxManager : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IMetaboxManager>
		{
		public:
			explicit CMetaboxManager(const OpenViBE::Kernel::IKernelContext& kernelContext);
			virtual ~CMetaboxManager(void);

			virtual bool addMetaboxFromFiles(const OpenViBE::CString& fileNameWildCard);
			virtual OpenViBE::CIdentifier getNextMetaboxObjectDescIdentifier(const OpenViBE::CIdentifier& previousIdentifier) const;

			virtual const OpenViBE::Plugins::IPluginObjectDesc* getMetaboxObjectDesc(const OpenViBE::CIdentifier& metaboxIdentifier) const;
			virtual void setMetaboxObjectDesc(const OpenViBE::CIdentifier& metaboxIdentifier, OpenViBE::Plugins::IPluginObjectDesc* metaboxDescriptor);

			virtual OpenViBE::CString getMetaboxFilePath(const OpenViBE::CIdentifier& metaboxIdentifier) const;
			virtual void setMetaboxFilePath(const OpenViBE::CIdentifier& metaboxIdentifier, const OpenViBE::CString& filePath);

			virtual OpenViBE::CIdentifier getMetaboxHash(const OpenViBE::CIdentifier& metaboxIdentifier) const;
			virtual void setMetaboxHash(const OpenViBE::CIdentifier& metaboxIdentifier, const OpenViBE::CIdentifier& hash);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IMetaboxManager, OVK_ClassId_Kernel_Metabox_MetaboxManager)

		protected:
			std::map<OpenViBE::CIdentifier, const OpenViBE::Plugins::IPluginObjectDesc*> m_MetaboxObjectDesc;
			std::map<OpenViBE::CIdentifier, OpenViBE::CString> m_MetaboxFilePath;
			std::map<OpenViBE::CIdentifier, OpenViBE::CIdentifier> m_MetaboxHash;
		};
	};
};



#endif // __OpenViBEKernel_Kernel_Metabox_CMetaboxManager_H__
