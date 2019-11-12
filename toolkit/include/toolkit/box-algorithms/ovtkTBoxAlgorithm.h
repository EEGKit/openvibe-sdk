#pragma once

#include "../ovtkIObject.h"
#include <limits>

namespace OpenViBEToolkit
{
	template <class TBoxAlgorithmParentClass>
	class TBoxAlgorithm : public TBoxAlgorithmParentClass
	{
	public:

		TBoxAlgorithm() { }

		// ====================================================================================================================================

	private:

		virtual uint64_t getClockFrequency(OpenViBE::Kernel::IBoxAlgorithmContext& boxAlgorithmCtx)
		{
			CScopedBoxAlgorithm scopedBoxAlgorithm(m_boxAlgorithmCtx, &boxAlgorithmCtx);
			return getClockFrequency();
		}

		virtual bool initialize(OpenViBE::Kernel::IBoxAlgorithmContext& boxAlgorithmCtx)
		{
			CScopedBoxAlgorithm scopedBoxAlgorithm(m_boxAlgorithmCtx, &boxAlgorithmCtx);
			return initialize();
		}

		virtual bool uninitialize(OpenViBE::Kernel::IBoxAlgorithmContext& boxAlgorithmCtx)
		{
			CScopedBoxAlgorithm scopedBoxAlgorithm(m_boxAlgorithmCtx, &boxAlgorithmCtx);
			return uninitialize();
		}

		virtual bool processEvent(OpenViBE::Kernel::IBoxAlgorithmContext& boxAlgorithmCtx, OpenViBE::CMessageEvent& messageEvent)
		{
			CScopedBoxAlgorithm scopedBoxAlgorithm(m_boxAlgorithmCtx, &boxAlgorithmCtx);
			return processEvent(messageEvent);
		}

		virtual bool processSignal(OpenViBE::Kernel::IBoxAlgorithmContext& boxAlgorithmCtx, OpenViBE::CMessageSignal& messageSignal)
		{
			CScopedBoxAlgorithm scopedBoxAlgorithm(m_boxAlgorithmCtx, &boxAlgorithmCtx);
			return processSignal(messageSignal);
		}

		virtual bool processClock(OpenViBE::Kernel::IBoxAlgorithmContext& boxAlgorithmCtx, OpenViBE::CMessageClock& messageClock)
		{
			CScopedBoxAlgorithm scopedBoxAlgorithm(m_boxAlgorithmCtx, &boxAlgorithmCtx);
			return processClock(messageClock);
		}

		virtual bool processInput(OpenViBE::Kernel::IBoxAlgorithmContext& boxAlgorithmCtx, const size_t index)
		{
			CScopedBoxAlgorithm scopedBoxAlgorithm(m_boxAlgorithmCtx, &boxAlgorithmCtx);
			return processInput(index);
		}

		virtual bool process(OpenViBE::Kernel::IBoxAlgorithmContext& boxAlgorithmCtx)
		{
			CScopedBoxAlgorithm oScopedBoxAlgorithm(m_boxAlgorithmCtx, &boxAlgorithmCtx);
			return process();
		}

		// ====================================================================================================================================

	public:

		virtual uint64_t getClockFrequency() { return 0; }
		virtual bool initialize() { return true; }
		virtual bool uninitialize() { return true; }
		virtual bool processEvent(OpenViBE::CMessageEvent& /*messageEvent*/) { return false; }
		virtual bool processSignal(OpenViBE::CMessageSignal& /*messageSignal*/) { return false; }
		virtual bool processClock(OpenViBE::CMessageClock& /*messageClock*/) { return false; }
		virtual bool processInput(const size_t /*index*/) { return false; }
		virtual bool process() = 0;

		// ====================================================================================================================================

		virtual OpenViBE::Kernel::IBoxAlgorithmContext* getBoxAlgorithmContext() { return m_boxAlgorithmCtx; }
		// virtual OpenViBE::Kernel::IBoxAlgorithmContext& getBoxAlgorithmContext() { return *m_boxAlgorithmCtx; } this one should replace !
		virtual const OpenViBE::Kernel::IBox& getStaticBoxContext() { return *m_boxAlgorithmCtx->getStaticBoxContext(); }
		virtual OpenViBE::Kernel::IBoxIO& getDynamicBoxContext() { return *m_boxAlgorithmCtx->getDynamicBoxContext(); }
		virtual OpenViBE::Kernel::IPlayerContext& getPlayerContext() { return *m_boxAlgorithmCtx->getPlayerContext(); }

		virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager() { return getPlayerContext().getAlgorithmManager(); }
		virtual OpenViBE::Kernel::IConfigurationManager& getConfigurationManager() { return getPlayerContext().getConfigurationManager(); }
		virtual OpenViBE::Kernel::ILogManager& getLogManager() { return getPlayerContext().getLogManager(); }
		virtual OpenViBE::Kernel::IErrorManager& getErrorManager() { return getPlayerContext().getErrorManager(); }
		virtual OpenViBE::Kernel::IScenarioManager& getScenarioManager() { return getPlayerContext().getScenarioManager(); }
		virtual OpenViBE::Kernel::ITypeManager& getTypeManager() { return getPlayerContext().getTypeManager(); }

		virtual bool canCreatePluginObject(const OpenViBE::CIdentifier& pluginID) { return getPlayerContext().canCreatePluginObject(pluginID); }

		virtual OpenViBE::Plugins::IPluginObject* createPluginObject(const OpenViBE::CIdentifier& pluginID)
		{
			return getPlayerContext().createPluginObject(pluginID);
		}

		virtual bool releasePluginObject(OpenViBE::Plugins::IPluginObject* pluginObject) { return getPlayerContext().releasePluginObject(pluginObject); }

		// ====================================================================================================================================

		virtual void appendOutputChunkData(const size_t outputIdx, const void* buffer, const size_t size)
		{
			OpenViBE::Kernel::IBoxAlgorithmContext* context = this->getBoxAlgorithmContext();
			if (context)
			{
				OpenViBE::Kernel::IBoxIO* boxContext = context->getDynamicBoxContext();
				if (boxContext) { boxContext->appendOutputChunkData(outputIdx, static_cast<const uint8_t*>(buffer), size); }
			}
		}

		template <size_t outputIdx>
		void appendOutputChunkData(const void* buffer, const size_t size) { appendOutputChunkData(outputIdx, buffer, size); }

		_IsDerivedFromClass_(TBoxAlgorithmParentClass, OVTK_ClassId_)

	protected:

		class FSettingValueAutoCast
		{
		public:
			FSettingValueAutoCast(OpenViBE::Kernel::IBoxAlgorithmContext& boxAlgorithmCtx, const size_t index)
				: m_logManager(boxAlgorithmCtx.getPlayerContext()->getLogManager()),
				  m_errorManager(boxAlgorithmCtx.getPlayerContext()->getErrorManager()),
				  m_typeManager(boxAlgorithmCtx.getPlayerContext()->getTypeManager()),
				  m_configManager(boxAlgorithmCtx.getPlayerContext()->getConfigurationManager())
			{
				boxAlgorithmCtx.getStaticBoxContext()->getSettingValue(index, m_settingValue);
				boxAlgorithmCtx.getStaticBoxContext()->getSettingType(index, m_settingType);
			}

			FSettingValueAutoCast(OpenViBE::Kernel::IBoxAlgorithmContext& boxAlgorithmCtx, const OpenViBE::CString& name)
				: m_logManager(boxAlgorithmCtx.getPlayerContext()->getLogManager()),
				  m_errorManager(boxAlgorithmCtx.getPlayerContext()->getErrorManager()),
				  m_typeManager(boxAlgorithmCtx.getPlayerContext()->getTypeManager()),
				  m_configManager(boxAlgorithmCtx.getPlayerContext()->getConfigurationManager())
			{
				boxAlgorithmCtx.getStaticBoxContext()->getSettingValue(name, m_settingValue);
				boxAlgorithmCtx.getStaticBoxContext()->getInterfacorType(OpenViBE::Kernel::EBoxInterfacorType::Setting, name, m_settingType);
			}

			FSettingValueAutoCast(OpenViBE::Kernel::IBoxAlgorithmContext& boxAlgorithmCtx, const OpenViBE::CIdentifier& identifier)
				: m_logManager(boxAlgorithmCtx.getPlayerContext()->getLogManager()),
				  m_errorManager(boxAlgorithmCtx.getPlayerContext()->getErrorManager()),
				  m_typeManager(boxAlgorithmCtx.getPlayerContext()->getTypeManager()),
				  m_configManager(boxAlgorithmCtx.getPlayerContext()->getConfigurationManager())
			{
				boxAlgorithmCtx.getStaticBoxContext()->getSettingValue(identifier, m_settingValue);
				boxAlgorithmCtx.getStaticBoxContext()->getInterfacorType(OpenViBE::Kernel::EBoxInterfacorType::Setting, identifier, m_settingType);
			}

			operator uint32_t()
			{
				double result;
				const OpenViBE::CString value = m_configManager.expand(m_settingValue);

				OV_ERROR_UNLESS(m_typeManager.evaluateSettingValue(value, result),
								"Could not expand numeric expression [" << m_settingValue << "] to unsigned integer 32bits.",
								OpenViBE::Kernel::ErrorType::BadParsing, std::numeric_limits<uint32_t>::max(), m_errorManager, m_logManager);

				return uint32_t(result);
			}

			operator uint64_t()
			{
				uint64_t stimId               = std::numeric_limits<uint64_t>::max();
				const OpenViBE::CString value = m_configManager.expand(m_settingValue);
				double result;
				if (m_typeManager.isEnumeration(m_settingType))
				{
					stimId = m_typeManager.getEnumerationEntryValueFromName(m_settingType, value);

					OV_ERROR_UNLESS(stimId != std::numeric_limits<uint64_t>::max(),
									"Did not find an enumeration value for [" << m_typeManager.getTypeName(m_settingType) << "] = [" << m_settingValue << "]",
									OpenViBE::Kernel::ErrorType::BadParsing, std::numeric_limits<uint64_t>::max(), m_errorManager, m_logManager);
				}
				else if (m_typeManager.evaluateSettingValue(value, result)) { return uint64_t(result); }

				// Seems like currently some plugins use FSettingValueAutoCast without knowing then setting type.
				// In this case, to avoid to pollute the console with useless messages, throw a message only if the
				// setting should be an integer.
				OV_ERROR_UNLESS(stimId != std::numeric_limits<uint64_t>::max() || m_settingType != OV_TypeId_Integer,
								"Could not expand numeric expression [" << m_settingValue << "] to unsigned integer 64bits.",
								OpenViBE::Kernel::ErrorType::BadParsing, std::numeric_limits<uint64_t>::max(), m_errorManager, m_logManager);

				return stimId;
			}

			operator int()
			{
				double res;
				const OpenViBE::CString value = m_configManager.expand(m_settingValue);

				OV_ERROR_UNLESS(m_typeManager.evaluateSettingValue(value, res),
								"Could not expand numeric expression [" << m_settingValue << "] to integer 32bits.",
								OpenViBE::Kernel::ErrorType::BadParsing, std::numeric_limits<int>::max(), m_errorManager, m_logManager);

				return int(res);
			}

			operator int64_t()
			{
				double res;
				const OpenViBE::CString value = m_configManager.expand(m_settingValue);

				OV_ERROR_UNLESS(m_typeManager.evaluateSettingValue(value, res),
								"Could not expand numeric expression [" << m_settingValue << "] to integer 64bits.",
								OpenViBE::Kernel::ErrorType::BadParsing, std::numeric_limits<int64_t>::max(), m_errorManager, m_logManager);

				return int64_t(res);
			}

			operator double()
			{
				double res;
				const OpenViBE::CString value = m_configManager.expand(m_settingValue);

				OV_ERROR_UNLESS(m_typeManager.evaluateSettingValue(value, res),
								"Could not expand numeric expression [" << m_settingValue << "] to double.",
								OpenViBE::Kernel::ErrorType::BadParsing, std::numeric_limits<double>::max(), m_errorManager, m_logManager);

				return double(res);
			}

			operator bool() { return m_configManager.expandAsBoolean(m_settingValue); }

			operator OpenViBE::CString() { return m_configManager.expand(m_settingValue); }

		private:
			OpenViBE::Kernel::ILogManager& m_logManager;
			OpenViBE::Kernel::IErrorManager& m_errorManager;
			OpenViBE::Kernel::ITypeManager& m_typeManager;
			OpenViBE::Kernel::IConfigurationManager& m_configManager;
			OpenViBE::CString m_settingValue;
			OpenViBE::CIdentifier m_settingType = OV_UndefinedIdentifier;
		};

	private:

		class CScopedBoxAlgorithm final
		{
		public:
			CScopedBoxAlgorithm(OpenViBE::Kernel::IBoxAlgorithmContext*& rBoxAlgorithmCtx, OpenViBE::Kernel::IBoxAlgorithmContext* boxAlgorithmCtx)
				: m_boxAlgorithmCtx(rBoxAlgorithmCtx) { m_boxAlgorithmCtx = boxAlgorithmCtx; }

			~CScopedBoxAlgorithm() { m_boxAlgorithmCtx = nullptr; }

		protected:
			OpenViBE::Kernel::IBoxAlgorithmContext*& m_boxAlgorithmCtx;
		};

		OpenViBE::Kernel::IBoxAlgorithmContext* m_boxAlgorithmCtx = nullptr;
	};

	template <class CBoxListenerParentClass>
	class TBoxListener : public CBoxListenerParentClass
	{
	public:

		TBoxListener() { }

	private:

		virtual bool initialize(OpenViBE::Kernel::IBoxListenerContext& boxListenerCtx)
		{
			CScopedBoxListener scopedBoxListener(m_boxListenerCtx, &boxListenerCtx);
			return initialize();
		}

		virtual bool uninitialize(OpenViBE::Kernel::IBoxListenerContext& boxListenerCtx)
		{
			CScopedBoxListener scopedBoxListener(m_boxListenerCtx, &boxListenerCtx);
			return uninitialize();
		}

		virtual bool process(OpenViBE::Kernel::IBoxListenerContext& boxListenerCtx, const OpenViBE::Kernel::EBoxModification eBoxModificationType)
		{
			CScopedBoxListener scopedBoxListener(m_boxListenerCtx, &boxListenerCtx);
			switch (eBoxModificationType)
			{
				case OpenViBE::Kernel::BoxModification_Initialized: return this->onInitialized(m_boxListenerCtx->getBox());
				case OpenViBE::Kernel::BoxModification_DefaultInitialized: return this->onDefaultInitialized(m_boxListenerCtx->getBox());
				case OpenViBE::Kernel::BoxModification_NameChanged: return this->onNameChanged(m_boxListenerCtx->getBox());
				case OpenViBE::Kernel::BoxModification_IdentifierChanged: return this->onIdentifierChanged(m_boxListenerCtx->getBox());
				case OpenViBE::Kernel::BoxModification_AlgorithmClassIdentifierChanged: return this->onAlgorithmClassIdentifierChanged(m_boxListenerCtx->getBox());
				case OpenViBE::Kernel::BoxModification_InputConnected: return this->onInputConnected(m_boxListenerCtx->getBox(), m_boxListenerCtx->getIndex());
				case OpenViBE::Kernel::BoxModification_InputDisconnected: return this->onInputDisconnected(m_boxListenerCtx->getBox(), m_boxListenerCtx->getIndex());
				case OpenViBE::Kernel::BoxModification_InputAdded: return this->onInputAdded(m_boxListenerCtx->getBox(), m_boxListenerCtx->getIndex());
				case OpenViBE::Kernel::BoxModification_InputRemoved: return this->onInputRemoved(m_boxListenerCtx->getBox(), m_boxListenerCtx->getIndex());
				case OpenViBE::Kernel::BoxModification_InputTypeChanged: return this->onInputTypeChanged(m_boxListenerCtx->getBox(), m_boxListenerCtx->getIndex());
				case OpenViBE::Kernel::BoxModification_InputNameChanged: return this->onInputNameChanged(m_boxListenerCtx->getBox(), m_boxListenerCtx->getIndex());
				case OpenViBE::Kernel::BoxModification_OutputConnected: return this->onOutputConnected(m_boxListenerCtx->getBox(), m_boxListenerCtx->getIndex());
				case OpenViBE::Kernel::BoxModification_OutputDisconnected: return this->onOutputDisconnected(m_boxListenerCtx->getBox(), m_boxListenerCtx->getIndex());
				case OpenViBE::Kernel::BoxModification_OutputAdded: return this->onOutputAdded(m_boxListenerCtx->getBox(), m_boxListenerCtx->getIndex());
				case OpenViBE::Kernel::BoxModification_OutputRemoved: return this->onOutputRemoved(m_boxListenerCtx->getBox(), m_boxListenerCtx->getIndex());
				case OpenViBE::Kernel::BoxModification_OutputTypeChanged: return this->onOutputTypeChanged(m_boxListenerCtx->getBox(), m_boxListenerCtx->getIndex());
				case OpenViBE::Kernel::BoxModification_OutputNameChanged: return this->onOutputNameChanged(m_boxListenerCtx->getBox(), m_boxListenerCtx->getIndex());
				case OpenViBE::Kernel::BoxModification_SettingAdded: return this->onSettingAdded(m_boxListenerCtx->getBox(), m_boxListenerCtx->getIndex());
				case OpenViBE::Kernel::BoxModification_SettingRemoved: return this->onSettingRemoved(m_boxListenerCtx->getBox(), m_boxListenerCtx->getIndex());
				case OpenViBE::Kernel::BoxModification_SettingTypeChanged: return this->onSettingTypeChanged(m_boxListenerCtx->getBox(), m_boxListenerCtx->getIndex());
				case OpenViBE::Kernel::BoxModification_SettingNameChanged: return this->onSettingNameChanged(m_boxListenerCtx->getBox(), m_boxListenerCtx->getIndex());
				case OpenViBE::Kernel::BoxModification_SettingDefaultValueChanged: return this->onSettingDefaultValueChanged(m_boxListenerCtx->getBox(), m_boxListenerCtx->getIndex());
				case OpenViBE::Kernel::BoxModification_SettingValueChanged: return this->onSettingValueChanged(m_boxListenerCtx->getBox(), m_boxListenerCtx->getIndex());
				default: OV_ERROR_KRF("Unhandled box modification type " << (size_t)eBoxModificationType, OpenViBE::Kernel::ErrorType::BadArgument);
			}
			return false;
		}

		// ====================================================================================================================================

	public:

		virtual bool initialize() { return true; }
		virtual bool uninitialize() { return true; }
		virtual bool onInitialized(OpenViBE::Kernel::IBox& /*box*/) { return true; }
		virtual bool onDefaultInitialized(OpenViBE::Kernel::IBox& /*box*/) { return true; }
		virtual bool onNameChanged(OpenViBE::Kernel::IBox& /*box*/) { return true; }
		virtual bool onIdentifierChanged(OpenViBE::Kernel::IBox& /*box*/) { return true; }
		virtual bool onAlgorithmClassIdentifierChanged(OpenViBE::Kernel::IBox& /*box*/) { return true; }
		virtual bool onInputConnected(OpenViBE::Kernel::IBox& /*box*/, const size_t /*index*/) { return true; }
		virtual bool onInputDisconnected(OpenViBE::Kernel::IBox& /*box*/, const size_t /*index*/) { return true; }
		virtual bool onInputAdded(OpenViBE::Kernel::IBox& /*box*/, const size_t /*index*/) { return true; }
		virtual bool onInputRemoved(OpenViBE::Kernel::IBox& /*box*/, const size_t /*index*/) { return true; }
		virtual bool onInputTypeChanged(OpenViBE::Kernel::IBox& /*box*/, const size_t /*index*/) { return true; }
		virtual bool onInputNameChanged(OpenViBE::Kernel::IBox& /*box*/, const size_t /*index*/) { return true; }
		virtual bool onOutputConnected(OpenViBE::Kernel::IBox& /*box*/, const size_t /*index*/) { return true; }
		virtual bool onOutputDisconnected(OpenViBE::Kernel::IBox& /*box*/, const size_t /*index*/) { return true; }
		virtual bool onOutputAdded(OpenViBE::Kernel::IBox& /*box*/, const size_t /*index*/) { return true; }
		virtual bool onOutputRemoved(OpenViBE::Kernel::IBox& /*box*/, const size_t /*index*/) { return true; }
		virtual bool onOutputTypeChanged(OpenViBE::Kernel::IBox& /*box*/, const size_t /*index*/) { return true; }
		virtual bool onOutputNameChanged(OpenViBE::Kernel::IBox& /*box*/, const size_t /*index*/) { return true; }
		virtual bool onSettingAdded(OpenViBE::Kernel::IBox& /*box*/, const size_t /*index*/) { return true; }
		virtual bool onSettingRemoved(OpenViBE::Kernel::IBox& /*box*/, const size_t /*index*/) { return true; }
		virtual bool onSettingTypeChanged(OpenViBE::Kernel::IBox& /*box*/, const size_t /*index*/) { return true; }
		virtual bool onSettingNameChanged(OpenViBE::Kernel::IBox& /*box*/, const size_t /*index*/) { return true; }
		virtual bool onSettingDefaultValueChanged(OpenViBE::Kernel::IBox& /*box*/, const size_t /*index*/) { return true; }
		virtual bool onSettingValueChanged(OpenViBE::Kernel::IBox& /*box*/, const size_t /*index*/) { return true; }

		// ====================================================================================================================================

		virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager() const { return m_boxListenerCtx->getAlgorithmManager(); }
		virtual OpenViBE::Kernel::IPlayerManager& getPlayerManager() const { return m_boxListenerCtx->getPlayerManager(); }
		virtual OpenViBE::Kernel::IPluginManager& getPluginManager() const { return m_boxListenerCtx->getPluginManager(); }
		virtual OpenViBE::Kernel::IMetaboxManager& getMetaboxManager() const { return m_boxListenerCtx->getMetaboxManager(); }
		virtual OpenViBE::Kernel::IScenarioManager& getScenarioManager() const { return m_boxListenerCtx->getScenarioManager(); }
		virtual OpenViBE::Kernel::ITypeManager& getTypeManager() const { return m_boxListenerCtx->getTypeManager(); }
		virtual OpenViBE::Kernel::ILogManager& getLogManager() const { return m_boxListenerCtx->getLogManager(); }
		virtual OpenViBE::Kernel::IErrorManager& getErrorManager() const { return m_boxListenerCtx->getErrorManager(); }
		virtual OpenViBE::Kernel::IConfigurationManager& getConfigurationManager() const { return m_boxListenerCtx->getConfigurationManager(); }

		virtual OpenViBE::Kernel::IScenario& getScenario() const { return m_boxListenerCtx->getScenario(); }

		// ====================================================================================================================================

		_IsDerivedFromClass_(CBoxListenerParentClass, OVTK_ClassId_)

	private:

		class CScopedBoxListener final
		{
		public:
			CScopedBoxListener(OpenViBE::Kernel::IBoxListenerContext*& rBoxListenerCtx, OpenViBE::Kernel::IBoxListenerContext* boxListenerCtx)
				: m_boxListenerCtx(rBoxListenerCtx) { m_boxListenerCtx = boxListenerCtx; }

			~CScopedBoxListener() { m_boxListenerCtx = nullptr; }

		protected:
			OpenViBE::Kernel::IBoxListenerContext*& m_boxListenerCtx;
		};

		OpenViBE::Kernel::IBoxListenerContext* m_boxListenerCtx = nullptr;
	};
} // namespace OpenViBEToolkit
