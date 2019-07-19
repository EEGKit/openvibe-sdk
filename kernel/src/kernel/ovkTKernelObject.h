#pragma once

#include "../ovk_base.h"

#include <openvibe/ov_all.h>

namespace OpenViBE
{
	namespace Kernel
	{
		template <class T>
		class TKernelObject : public T
		{
		public:

			explicit TKernelObject(const IKernelContext& rKernelContext)
				: m_kernelContext(rKernelContext) { }

			TKernelObject(const TKernelObject&)            = delete;
			TKernelObject& operator=(const TKernelObject&) = delete;

			const IKernelContext& getKernelContext(void) const
			{
				return m_kernelContext;
			}

			virtual IAlgorithmManager& getAlgorithmManager(void) const
			{
				return m_kernelContext.getAlgorithmManager();
			}

			virtual IConfigurationManager& getConfigurationManager(void) const
			{
				return m_kernelContext.getConfigurationManager();
			}

			virtual IKernelObjectFactory& getKernelObjectFactory(void) const
			{
				return m_kernelContext.getKernelObjectFactory();
			}

			virtual IPlayerManager& getPlayerManager(void) const
			{
				return m_kernelContext.getPlayerManager();
			}

			virtual IPluginManager& getPluginManager(void) const
			{
				return m_kernelContext.getPluginManager();
			}

			virtual IMetaboxManager& getMetaboxManager(void) const
			{
				return m_kernelContext.getMetaboxManager();
			}

			virtual IScenarioManager& getScenarioManager(void) const
			{
				return m_kernelContext.getScenarioManager();
			}

			virtual ITypeManager& getTypeManager(void) const
			{
				return m_kernelContext.getTypeManager();
			}

			virtual ILogManager& getLogManager(void) const
			{
				return m_kernelContext.getLogManager();
			}

			virtual IErrorManager& getErrorManager(void) const
			{
				return m_kernelContext.getErrorManager();
			}

			_IsDerivedFromClass_(T, OVK_ClassId_Kernel_KernelObjectT)

		private:

			const IKernelContext& m_kernelContext;

			// TKernelObject(void);
		};
	};
};


