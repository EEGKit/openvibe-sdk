#ifndef __OpenViBEKernel_Kernel_TKernelObject_H__
#define __OpenViBEKernel_Kernel_TKernelObject_H__

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

			explicit TKernelObject(const OpenViBE::Kernel::IKernelContext& rKernelContext)
				: m_kernelContext(rKernelContext) { }

			TKernelObject(const TKernelObject&)            = delete;
			TKernelObject& operator=(const TKernelObject&) = delete;

			const OpenViBE::Kernel::IKernelContext& getKernelContext(void) const
			{
				return m_kernelContext;
			}

			virtual OpenViBE::Kernel::IAlgorithmManager& getAlgorithmManager(void) const
			{
				return m_kernelContext.getAlgorithmManager();
			}

			virtual OpenViBE::Kernel::IConfigurationManager& getConfigurationManager(void) const
			{
				return m_kernelContext.getConfigurationManager();
			}

			virtual OpenViBE::Kernel::IKernelObjectFactory& getKernelObjectFactory(void) const
			{
				return m_kernelContext.getKernelObjectFactory();
			}

			virtual OpenViBE::Kernel::IPlayerManager& getPlayerManager(void) const
			{
				return m_kernelContext.getPlayerManager();
			}

			virtual OpenViBE::Kernel::IPluginManager& getPluginManager(void) const
			{
				return m_kernelContext.getPluginManager();
			}

			virtual OpenViBE::Kernel::IMetaboxManager& getMetaboxManager(void) const
			{
				return m_kernelContext.getMetaboxManager();
			}

			virtual OpenViBE::Kernel::IScenarioManager& getScenarioManager(void) const
			{
				return m_kernelContext.getScenarioManager();
			}

			virtual OpenViBE::Kernel::ITypeManager& getTypeManager(void) const
			{
				return m_kernelContext.getTypeManager();
			}

			virtual OpenViBE::Kernel::ILogManager& getLogManager(void) const
			{
				return m_kernelContext.getLogManager();
			}

			virtual OpenViBE::Kernel::IErrorManager& getErrorManager(void) const
			{
				return m_kernelContext.getErrorManager();
			}

			_IsDerivedFromClass_(T, OVK_ClassId_Kernel_KernelObjectT)

		private:

			const OpenViBE::Kernel::IKernelContext& m_kernelContext;

			// TKernelObject(void);
		};
	};
};

#endif // __OpenViBEKernel_Kernel_TKernelObject_H__
