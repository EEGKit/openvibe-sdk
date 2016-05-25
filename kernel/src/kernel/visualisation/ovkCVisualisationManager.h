#ifndef __OpenViBEKernel_Kernel_Visualisation_CVisualisationManager_H__
#define __OpenViBEKernel_Kernel_Visualisation_CVisualisationManager_H__

#include "../ovkTKernelObject.h"
#include "../ovkIGtkOVCustomHandler.h"

#include <map>

typedef struct _GtkWidget GtkWidget;

namespace OpenViBE
{
	namespace Kernel
	{
		class CVisualisationManager : public OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IVisualisationManager>, public IGtkOVCustomHandler
		{
		public:

			CVisualisationManager(
				const OpenViBE::Kernel::IKernelContext& rKernelContext);

			virtual ~CVisualisationManager();

			virtual OpenViBE::boolean createVisualisationTree(
				OpenViBE::CIdentifier& rVisualisationTreeIdentifier);
			virtual OpenViBE::boolean releaseVisualisationTree(
				const OpenViBE::CIdentifier& rVisualisationTreeIdentifier);
			virtual OpenViBE::Kernel::IVisualisationTree& getVisualisationTree(
				const OpenViBE::CIdentifier& rVisualisationTreeIdentifier);
			virtual OpenViBE::boolean enumerateVisualisationTrees(
				OpenViBE::Kernel::IVisualisationManager::IVisualisationTreeEnum& rCallBack) const;

			virtual OpenViBE::boolean setToolbar(
				const CIdentifier& rVisualisationTreeIdentifier,
				const CIdentifier& rBoxIdentifier,
				::GtkWidget* pToolbar);
			virtual OpenViBE::boolean setWidget(
				const CIdentifier& rVisualisationTreeIdentifier,
				const CIdentifier& rBoxIdentifier,
				::GtkWidget* pTopmostWidget);

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::TKernelObject<OpenViBE::Kernel::IVisualisationManager>, OVK_ClassId_Kernel_Visualisation_VisualisationManager);

		protected:

			virtual OpenViBE::CIdentifier getUnusedIdentifier(void) const;

		protected:

			/// Map of visualisation trees (one per scenario, storing visualisation widgets arrangement in space)
			std::map<OpenViBE::CIdentifier, OpenViBE::Kernel::IVisualisationTree*> m_vVisualisationTree;
		};
	};
};

#endif // __OpenViBEKernel_Kernel_Visualisation_CVisualisationManager_H__
