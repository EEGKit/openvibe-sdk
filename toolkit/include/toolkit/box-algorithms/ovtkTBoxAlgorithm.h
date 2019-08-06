#pragma once

#include "../ovtkIObject.h"
#include <limits>

namespace OpenViBEToolkit
{
	template <class CBoxAlgorithmParentClass>
	class TBoxAlgorithm : public CBoxAlgorithmParentClass
	{
	public:

		TBoxAlgorithm() : m_pBoxAlgorithmContext(NULL) { }

		// ====================================================================================================================================

	private:

		virtual uint64_t getClockFrequency(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext)
		{
			CScopedBoxAlgorithm l_oScopedBoxAlgorithm(m_pBoxAlgorithmContext, &rBoxAlgorithmContext);
			return getClockFrequency();
		}

		virtual bool initialize(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext)
		{
			CScopedBoxAlgorithm l_oScopedBoxAlgorithm(m_pBoxAlgorithmContext, &rBoxAlgorithmContext);
			return initialize();
		}

		virtual bool uninitialize(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext)
		{
			CScopedBoxAlgorithm l_oScopedBoxAlgorithm(m_pBoxAlgorithmContext, &rBoxAlgorithmContext);
			return uninitialize();
		}

		virtual bool processEvent(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext, OpenViBE::CMessageEvent& rMessageEvent)
		{
			CScopedBoxAlgorithm l_oScopedBoxAlgorithm(m_pBoxAlgorithmContext, &rBoxAlgorithmContext);
			return processEvent(rMessageEvent);
		}

		virtual bool processSignal(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext, OpenViBE::CMessageSignal& rMessageSignal)
		{
			CScopedBoxAlgorithm l_oScopedBoxAlgorithm(m_pBoxAlgorithmContext, &rBoxAlgorithmContext);
			return processSignal(rMessageSignal);
		}

		virtual bool processClock(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext, OpenViBE::CMessageClock& rMessageClock)
		{
			CScopedBoxAlgorithm l_oScopedBoxAlgorithm(m_pBoxAlgorithmContext, &rBoxAlgorithmContext);
			return processClock(rMessageClock);
		}

		virtual bool processInput(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext, uint32_t ui32InputIndex)
		{
			CScopedBoxAlgorithm l_oScopedBoxAlgorithm(m_pBoxAlgorithmContext, &rBoxAlgorithmContext);
			return processInput(ui32InputIndex);
		}

		virtual bool process(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext)
		{
			CScopedBoxAlgorithm l_oScopedBoxAlgorithm(m_pBoxAlgorithmContext, &rBoxAlgorithmContext);
			return process();
		}

		// ====================================================================================================================================

	public:

		virtual uint64_t getClockFrequency() { return 0; }
		virtual bool initialize() { return true; }
		virtual bool uninitialize() { return true; }
		virtual bool processEvent(OpenViBE::CMessageEvent& rMessageEvent) { return false; }
		virtual bool processSignal(OpenViBE::CMessageSignal& rMessageSignal) { return false; }
		virtual bool processClock(OpenViBE::CMessageClock& rMessageClock) { return false; }
		virtual bool processInput(const uint32_t ui32InputIndex) { return false; }
		virtual bool process() = 0;

		// ====================================================================================================================================

		virtual OpenViBE::Kernel::IBoxAlgorithmContext* getBoxAlgorithmContext() { return m_pBoxAlgorithmContext; }
		// virtual OpenViBE::Kernel::IBoxAlgorithmContext& getBoxAlgorithmContext() { return *m_pBoxAlgorithmContext; } this one should replace !
		virtual const OpenViBE::Kernel::IBox& getStaticBoxContext() { return *m_pBoxAlgorithmContext->getStaticBoxContext(); }
		virtual OpenViBE::Kernel::IBoxIO& getDynamicBoxContext() { return *m_pBoxAlgorithmContext->getDynamicBoxContext(); }
		virtual OpenViBE::Kernel::IPlayerContext& getPlayerContext() { return *m_pBoxAlgorithmContext->getPlayerContext(); }

		virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager() { return getPlayerContext().getAlgorithmManager(); }
		virtual OpenViBE::Kernel::IConfigurationManager& getConfigurationManager() { return getPlayerContext().getConfigurationManager(); }
		virtual OpenViBE::Kernel::ILogManager& getLogManager() { return getPlayerContext().getLogManager(); }
		virtual OpenViBE::Kernel::IErrorManager& getErrorManager() { return getPlayerContext().getErrorManager(); }
		virtual OpenViBE::Kernel::IScenarioManager& getScenarioManager() { return getPlayerContext().getScenarioManager(); }
		virtual OpenViBE::Kernel::ITypeManager& getTypeManager() { return getPlayerContext().getTypeManager(); }

		virtual bool canCreatePluginObject(const OpenViBE::CIdentifier& pluginIdentifier)
		{
			return getPlayerContext().canCreatePluginObject(pluginIdentifier);
		}

		virtual OpenViBE::Plugins::IPluginObject* createPluginObject(const OpenViBE::CIdentifier& pluginIdentifier)
		{
			return getPlayerContext().createPluginObject(pluginIdentifier);
		}

		virtual bool releasePluginObject(OpenViBE::Plugins::IPluginObject* pluginObject)
		{
			return getPlayerContext().releasePluginObject(pluginObject);
		}

		// ====================================================================================================================================

		virtual void appendOutputChunkData(const uint32_t ui32OutputIndex, const void* pBuffer, const uint64_t ui64BufferSize)
		{
			OpenViBE::Kernel::IBoxAlgorithmContext* l_pBoxAlgorithmContext = this->getBoxAlgorithmContext();
			if (l_pBoxAlgorithmContext)
			{
				OpenViBE::Kernel::IBoxIO* l_pDynamicBoxContext = l_pBoxAlgorithmContext->getDynamicBoxContext();
				if (l_pDynamicBoxContext)
				{
					l_pDynamicBoxContext->appendOutputChunkData(ui32OutputIndex, static_cast<const uint8_t*>(pBuffer), ui64BufferSize);
				}
			}
		}

		template <uint32_t ui32OutputIndex>
		void appendOutputChunkData(const void* pBuffer, const uint64_t ui64BufferSize)
		{
			appendOutputChunkData(ui32OutputIndex, pBuffer, ui64BufferSize);
		}

		_IsDerivedFromClass_(CBoxAlgorithmParentClass, OVTK_ClassId_)

	protected:

		class FSettingValueAutoCast
		{
		public:
			FSettingValueAutoCast(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext, const uint32_t ui32Index)
				: m_rLogManager(rBoxAlgorithmContext.getPlayerContext()->getLogManager())
				  , m_rErrorManager(rBoxAlgorithmContext.getPlayerContext()->getErrorManager())
				  , m_rTypeManager(rBoxAlgorithmContext.getPlayerContext()->getTypeManager())
				  , m_rConfigurationManager(rBoxAlgorithmContext.getPlayerContext()->getConfigurationManager())
			{
				rBoxAlgorithmContext.getStaticBoxContext()->getSettingValue(ui32Index, m_sSettingValue);
				rBoxAlgorithmContext.getStaticBoxContext()->getSettingType(ui32Index, m_oSettingType);
			}

			FSettingValueAutoCast(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext, const OpenViBE::CString& rsName)
				: m_rLogManager(rBoxAlgorithmContext.getPlayerContext()->getLogManager())
				  , m_rErrorManager(rBoxAlgorithmContext.getPlayerContext()->getErrorManager())
				  , m_rTypeManager(rBoxAlgorithmContext.getPlayerContext()->getTypeManager())
				  , m_rConfigurationManager(rBoxAlgorithmContext.getPlayerContext()->getConfigurationManager())
			{
				rBoxAlgorithmContext.getStaticBoxContext()->getSettingValue(rsName, m_sSettingValue);
				rBoxAlgorithmContext.getStaticBoxContext()->getInterfacorType(OpenViBE::Kernel::BoxInterfacorType::Setting, rsName, m_oSettingType);
			}

			FSettingValueAutoCast(OpenViBE::Kernel::IBoxAlgorithmContext& rBoxAlgorithmContext, const OpenViBE::CIdentifier& rIdentifier)
				: m_rLogManager(rBoxAlgorithmContext.getPlayerContext()->getLogManager())
				  , m_rErrorManager(rBoxAlgorithmContext.getPlayerContext()->getErrorManager())
				  , m_rTypeManager(rBoxAlgorithmContext.getPlayerContext()->getTypeManager())
				  , m_rConfigurationManager(rBoxAlgorithmContext.getPlayerContext()->getConfigurationManager())
			{
				rBoxAlgorithmContext.getStaticBoxContext()->getSettingValue(rIdentifier, m_sSettingValue);
				rBoxAlgorithmContext.getStaticBoxContext()->getInterfacorType(OpenViBE::Kernel::BoxInterfacorType::Setting, rIdentifier, m_oSettingType);
			}

			operator uint32_t()
			{
				double l_dResult;
				OpenViBE::CString l_sSettingValue = m_rConfigurationManager.expand(m_sSettingValue);

				OV_ERROR_UNLESS(m_rTypeManager.evaluateSettingValue(l_sSettingValue, l_dResult),
								"Could not expand numeric expression [" << m_sSettingValue << "] to unsigned integer 32bits.",
								OpenViBE::Kernel::ErrorType::BadParsing, std::numeric_limits<uint32_t>::max(), m_rErrorManager, m_rLogManager);

				return static_cast<uint32_t>(l_dResult);
			}

			operator uint64_t()
			{
				uint64_t l_ui64StimId             = std::numeric_limits<uint64_t>::max();
				OpenViBE::CString l_sSettingValue = m_rConfigurationManager.expand(m_sSettingValue);
				double l_dResult;
				if (m_rTypeManager.isEnumeration(m_oSettingType))
				{
					l_ui64StimId = m_rTypeManager.getEnumerationEntryValueFromName(m_oSettingType, l_sSettingValue);

					OV_ERROR_UNLESS(l_ui64StimId != std::numeric_limits<uint64_t>::max(),
									"Did not find an enumeration value for [" << m_rTypeManager.getTypeName(m_oSettingType) << "] = [" << m_sSettingValue << "]",
									OpenViBE::Kernel::ErrorType::BadParsing, std::numeric_limits<uint64_t>::max(), m_rErrorManager, m_rLogManager);
				}
				else if (m_rTypeManager.evaluateSettingValue(l_sSettingValue, l_dResult))
				{
					return static_cast<uint64_t>(l_dResult);
				}

				// Seems like currently some plugins use FSettingValueAutoCast without knowing then setting type.
				// In this case, to avoid to pollute the console with useless messages, throw a message only if the
				// setting should be an integer.
				OV_ERROR_UNLESS(l_ui64StimId != std::numeric_limits<uint64_t>::max() || m_oSettingType != OV_TypeId_Integer,
								"Could not expand numeric expression [" << m_sSettingValue << "] to unsigned integer 64bits.",
								OpenViBE::Kernel::ErrorType::BadParsing, std::numeric_limits<uint64_t>::max(), m_rErrorManager, m_rLogManager);

				return l_ui64StimId;
			}

			operator int32_t()
			{
				double l_dResult;
				OpenViBE::CString l_sSettingValue = m_rConfigurationManager.expand(m_sSettingValue);

				OV_ERROR_UNLESS(m_rTypeManager.evaluateSettingValue(l_sSettingValue, l_dResult),
								"Could not expand numeric expression [" << m_sSettingValue << "] to integer 32bits.",
								OpenViBE::Kernel::ErrorType::BadParsing, std::numeric_limits<int32_t>::max(), m_rErrorManager, m_rLogManager);

				return static_cast<int32_t>(l_dResult);
			}

			operator int64_t()
			{
				double l_dResult;
				OpenViBE::CString l_sSettingValue = m_rConfigurationManager.expand(m_sSettingValue);

				OV_ERROR_UNLESS(m_rTypeManager.evaluateSettingValue(l_sSettingValue, l_dResult),
								"Could not expand numeric expression [" << m_sSettingValue << "] to integer 64bits.",
								OpenViBE::Kernel::ErrorType::BadParsing, std::numeric_limits<int64_t>::max(), m_rErrorManager, m_rLogManager);

				return static_cast<int64_t>(l_dResult);
			}

			operator double()
			{
				double l_dResult;
				OpenViBE::CString l_sSettingValue = m_rConfigurationManager.expand(m_sSettingValue);

				OV_ERROR_UNLESS(m_rTypeManager.evaluateSettingValue(l_sSettingValue, l_dResult), "Could not expand numeric expression [" << m_sSettingValue << "] to float.",
								OpenViBE::Kernel::ErrorType::BadParsing, std::numeric_limits<double>::max(), m_rErrorManager, m_rLogManager);

				return static_cast<double>(l_dResult);
			}

			operator bool() { return m_rConfigurationManager.expandAsBoolean(m_sSettingValue); }

			operator OpenViBE::CString() { return m_rConfigurationManager.expand(m_sSettingValue); }

		private:
			OpenViBE::Kernel::ILogManager& m_rLogManager;
			OpenViBE::Kernel::IErrorManager& m_rErrorManager;
			OpenViBE::Kernel::ITypeManager& m_rTypeManager;
			OpenViBE::Kernel::IConfigurationManager& m_rConfigurationManager;
			OpenViBE::CString m_sSettingValue;
			OpenViBE::CIdentifier m_oSettingType;
		};

	private:

		class CScopedBoxAlgorithm
		{
		public:
			CScopedBoxAlgorithm(OpenViBE::Kernel::IBoxAlgorithmContext*& rpBoxAlgorithmContext, OpenViBE::Kernel::IBoxAlgorithmContext* pBoxAlgorithmContext)
				: m_rpBoxAlgorithmContext(rpBoxAlgorithmContext)
			{
				m_rpBoxAlgorithmContext = pBoxAlgorithmContext;
			}

			virtual ~CScopedBoxAlgorithm() { m_rpBoxAlgorithmContext = NULL; }

		protected:
			OpenViBE::Kernel::IBoxAlgorithmContext*& m_rpBoxAlgorithmContext;
		};

		OpenViBE::Kernel::IBoxAlgorithmContext* m_pBoxAlgorithmContext;
	};

	template <class CBoxListenerParentClass>
	class TBoxListener : public CBoxListenerParentClass
	{
	public:

		TBoxListener() : m_pBoxListenerContext(NULL) { }

	private:

		virtual bool initialize(OpenViBE::Kernel::IBoxListenerContext& rBoxListenerContext)
		{
			CScopedBoxListener l_oScopedBoxListener(m_pBoxListenerContext, &rBoxListenerContext);
			return initialize();
		}

		virtual bool uninitialize(OpenViBE::Kernel::IBoxListenerContext& rBoxListenerContext)
		{
			CScopedBoxListener l_oScopedBoxListener(m_pBoxListenerContext, &rBoxListenerContext);
			return uninitialize();
		}

		virtual bool process(OpenViBE::Kernel::IBoxListenerContext& rBoxListenerContext, const OpenViBE::Kernel::EBoxModification eBoxModificationType)
		{
			CScopedBoxListener l_oScopedBoxListener(m_pBoxListenerContext, &rBoxListenerContext);
			switch (eBoxModificationType)
			{
				case OpenViBE::Kernel::BoxModification_Initialized: return this->onInitialized(m_pBoxListenerContext->getBox());
				case OpenViBE::Kernel::BoxModification_DefaultInitialized: return this->onDefaultInitialized(m_pBoxListenerContext->getBox());
				case OpenViBE::Kernel::BoxModification_NameChanged: return this->onNameChanged(m_pBoxListenerContext->getBox());
				case OpenViBE::Kernel::BoxModification_IdentifierChanged: return this->onIdentifierChanged(m_pBoxListenerContext->getBox());
				case OpenViBE::Kernel::BoxModification_AlgorithmClassIdentifierChanged: return this->onAlgorithmClassIdentifierChanged(m_pBoxListenerContext->getBox());
				case OpenViBE::Kernel::BoxModification_InputConnected: return this->onInputConnected(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_InputDisconnected: return this->onInputDisconnected(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_InputAdded: return this->onInputAdded(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_InputRemoved: return this->onInputRemoved(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_InputTypeChanged: return this->onInputTypeChanged(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_InputNameChanged: return this->onInputNameChanged(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_OutputConnected: return this->onOutputConnected(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_OutputDisconnected: return this->onOutputDisconnected(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_OutputAdded: return this->onOutputAdded(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_OutputRemoved: return this->onOutputRemoved(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_OutputTypeChanged: return this->onOutputTypeChanged(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_OutputNameChanged: return this->onOutputNameChanged(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_SettingAdded: return this->onSettingAdded(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_SettingRemoved: return this->onSettingRemoved(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_SettingTypeChanged: return this->onSettingTypeChanged(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_SettingNameChanged: return this->onSettingNameChanged(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_SettingDefaultValueChanged: return this->onSettingDefaultValueChanged(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				case OpenViBE::Kernel::BoxModification_SettingValueChanged: return this->onSettingValueChanged(m_pBoxListenerContext->getBox(), m_pBoxListenerContext->getIndex());
				default: OV_ERROR_KRF("Unhandled box modification type " << (uint32_t)eBoxModificationType, OpenViBE::Kernel::ErrorType::BadArgument);
			}
			return false;
		}

		// ====================================================================================================================================

	public:

		virtual bool initialize() { return true; }
		virtual bool uninitialize() { return true; }
		virtual bool onInitialized(OpenViBE::Kernel::IBox& rBox) { return true; };
		virtual bool onDefaultInitialized(OpenViBE::Kernel::IBox& rBox) { return true; };
		virtual bool onNameChanged(OpenViBE::Kernel::IBox& rBox) { return true; };
		virtual bool onIdentifierChanged(OpenViBE::Kernel::IBox& rBox) { return true; };
		virtual bool onAlgorithmClassIdentifierChanged(OpenViBE::Kernel::IBox& rBox) { return true; };
		virtual bool onInputConnected(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
		virtual bool onInputDisconnected(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
		virtual bool onInputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
		virtual bool onInputRemoved(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
		virtual bool onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
		virtual bool onInputNameChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
		virtual bool onOutputConnected(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
		virtual bool onOutputDisconnected(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
		virtual bool onOutputAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
		virtual bool onOutputRemoved(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
		virtual bool onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
		virtual bool onOutputNameChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
		virtual bool onSettingAdded(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
		virtual bool onSettingRemoved(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
		virtual bool onSettingTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
		virtual bool onSettingNameChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
		virtual bool onSettingDefaultValueChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };
		virtual bool onSettingValueChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t ui32Index) { return true; };

		// ====================================================================================================================================

		virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager() const { return m_pBoxListenerContext->getAlgorithmManager(); }
		virtual OpenViBE::Kernel::IPlayerManager& getPlayerManager() const { return m_pBoxListenerContext->getPlayerManager(); }
		virtual OpenViBE::Kernel::IPluginManager& getPluginManager() const { return m_pBoxListenerContext->getPluginManager(); }
		virtual OpenViBE::Kernel::IMetaboxManager& getMetaboxManager() const { return m_pBoxListenerContext->getMetaboxManager(); }
		virtual OpenViBE::Kernel::IScenarioManager& getScenarioManager() const { return m_pBoxListenerContext->getScenarioManager(); }
		virtual OpenViBE::Kernel::ITypeManager& getTypeManager() const { return m_pBoxListenerContext->getTypeManager(); }
		virtual OpenViBE::Kernel::ILogManager& getLogManager() const { return m_pBoxListenerContext->getLogManager(); }
		virtual OpenViBE::Kernel::IErrorManager& getErrorManager() const { return m_pBoxListenerContext->getErrorManager(); }
		virtual OpenViBE::Kernel::IConfigurationManager& getConfigurationManager() const { return m_pBoxListenerContext->getConfigurationManager(); }

		virtual OpenViBE::Kernel::IScenario& getScenario() const { return m_pBoxListenerContext->getScenario(); }

		// ====================================================================================================================================

		_IsDerivedFromClass_(CBoxListenerParentClass, OVTK_ClassId_)

	private:

		class CScopedBoxListener
		{
		public:
			CScopedBoxListener(OpenViBE::Kernel::IBoxListenerContext*& rpBoxListenerContext, OpenViBE::Kernel::IBoxListenerContext* pBoxListenerContext)
				: m_rpBoxListenerContext(rpBoxListenerContext)
			{
				m_rpBoxListenerContext = pBoxListenerContext;
			}

			virtual ~CScopedBoxListener()
			{
				m_rpBoxListenerContext = NULL;
			}

		protected:
			OpenViBE::Kernel::IBoxListenerContext*& m_rpBoxListenerContext;
		};

		OpenViBE::Kernel::IBoxListenerContext* m_pBoxListenerContext;
	};
}  // namespace OpenViBEToolkit
