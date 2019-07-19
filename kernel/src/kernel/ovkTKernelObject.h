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

			const IKernelContext& getKernelContext() const
			{
				return m_kernelContext;
			}

			virtual IAlgorithmManager& getAlgorithmManager() const
			{
				return m_kernelContext.getAlgorithmManager();
			}

			virtual IConfigurationManager& getConfigurationManager() const
			{
				return m_kernelContext.getConfigurationManager();
			}

			virtual IKernelObjectFactory& getKernelObjectFactory() const
			{
				return m_kernelContext.getKernelObjectFactory();
			}

			virtual IPlayerManager& getPlayerManager() const
			{
				return m_kernelContext.getPlayerManager();
			}

			virtual IPluginManager& getPluginManager() const
			{
				return m_kernelContext.getPluginManager();
			}

			virtual IMetaboxManager& getMetaboxManager() const
			{
				return m_kernelContext.getMetaboxManager();
			}

			virtual IScenarioManager& getScenarioManager() const
			{
				return m_kernelContext.getScenarioManager();
			}

			virtual ITypeManager& getTypeManager() const
			{
				return m_kernelContext.getTypeManager();
			}

			virtual ILogManager& getLogManager() const
			{
				return m_kernelContext.getLogManager();
			}

			virtual IErrorManager& getErrorManager() const
			{
				return m_kernelContext.getErrorManager();
			}

			_IsDerivedFromClass_(T, OVK_ClassId_Kernel_KernelObjectT)

		private:

			const IKernelContext& m_kernelContext;

			// TKernelObject();
		};
	};
};


