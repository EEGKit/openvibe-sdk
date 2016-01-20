#ifndef __OpenViBEKernel_Kernel_Visualisation_CVisualisationTreeNull_H__
#define __OpenViBEKernel_Kernel_Visualisation_CVisualisationTreeNull_H__

#include "../ovkTKernelObject.h"

#include "../scenario/ovkTAttributable.h"

#include <map>

namespace OpenViBE
{
	namespace Kernel
	{
		class CVisualisationTreeNull : public OpenViBE::Kernel::TAttributable < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IVisualisationTree > >
		{
		public:

			CVisualisationTreeNull(const OpenViBE::Kernel::IKernelContext& rKernelContext)
				:OpenViBE::Kernel::TAttributable < OpenViBE::Kernel::TKernelObject < OpenViBE::Kernel::IVisualisationTree > >(rKernelContext)
			{
			}

			virtual OpenViBE::boolean init(
				const OpenViBE::Kernel::IScenario* pScenario) { return true; }

			virtual OpenViBE::boolean getNextVisualisationWidgetIdentifier(
				OpenViBE::CIdentifier& rIdentifier) const { return false; }

			virtual OpenViBE::boolean getNextVisualisationWidgetIdentifier(
				OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::Kernel::EVisualisationWidgetType oType) const { return false; }

			virtual OpenViBE::boolean isVisualisationWidget(
				const OpenViBE::CIdentifier& rIdentifier) const { return false; }

			virtual OpenViBE::Kernel::IVisualisationWidget* getVisualisationWidget(
				const OpenViBE::CIdentifier& rIdentifier) const { return NULL; }

			virtual OpenViBE::Kernel::IVisualisationWidget* getVisualisationWidgetFromBoxIdentifier(
				const OpenViBE::CIdentifier& rBoxIdentifier) const { return NULL; }

			virtual OpenViBE::boolean addVisualisationWidget(
				OpenViBE::CIdentifier& rIdentifier,
				const OpenViBE::CString& rName,
				OpenViBE::Kernel::EVisualisationWidgetType oType,
				const OpenViBE::CIdentifier& rParentIdentifier,
				OpenViBE::uint32 ui32ParentIndex,
				const OpenViBE::CIdentifier& rBoxIdentifier,
				OpenViBE::uint32 ui32NbChildren,
				const OpenViBE::CIdentifier& rSuggestedIdentifier) { return false; }

			virtual OpenViBE::boolean getVisualisationWidgetIndex(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32& ui32Index) const { return false; }

			virtual OpenViBE::boolean unparentVisualisationWidget(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::uint32& ui32Index) { return false; }

			virtual OpenViBE::boolean parentVisualisationWidget(
				const OpenViBE::CIdentifier& rIdentifier,
				const OpenViBE::CIdentifier& rParentIdentifier,
				OpenViBE::uint32 ui32Index) { return false; }

			virtual OpenViBE::boolean destroyHierarchy(
				const OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::boolean bDestroyVisualisationBoxes=true) { return true; }

			virtual ::GtkTreeView* createTreeViewWithModel(void) { return NULL; }

			virtual OpenViBE::boolean setTreeViewCB(
				OpenViBE::Kernel::ITreeViewCB* pTreeViewCB) { return true; }

			virtual OpenViBE::boolean reloadTree(void) { return true; }

			virtual OpenViBE::boolean getTreeSelection(
				::GtkTreeView* pTreeView,
				::GtkTreeIter* pIter) { return false; }

			virtual GtkTreePath* getTreePath(
				::GtkTreeIter* pTreeIter) const { return NULL; }

			virtual unsigned long getULongValueFromTreeIter(
				::GtkTreeIter* pTreeIter,
				OpenViBE::Kernel::EVisualisationTreeColumn eVisualisationTreeColumn) const { return 0; }

			virtual OpenViBE::boolean getStringValueFromTreeIter(
				::GtkTreeIter* pTreeIter,
				char*& rString,
				OpenViBE::Kernel::EVisualisationTreeColumn eVisualisationTreeColumn) const { return false; }

			virtual OpenViBE::boolean getPointerValueFromTreeIter(
				::GtkTreeIter* pTreeIter,
				void*& rPointer,
				OpenViBE::Kernel::EVisualisationTreeColumn eVisualisationTreeColumn) const { return false; }

			virtual OpenViBE::boolean getIdentifierFromTreeIter(
				::GtkTreeIter* pTreeIter,
				OpenViBE::CIdentifier& rIdentifier,
				OpenViBE::Kernel::EVisualisationTreeColumn eVisualisationTreeColumn) const { return false; }

			virtual OpenViBE::boolean findChildNodeFromRoot(
				::GtkTreeIter* pIter,
				const char* pLabel,
				OpenViBE::Kernel::EVisualisationTreeNode eType) { return false; }

			virtual OpenViBE::boolean findChildNodeFromParent(
				::GtkTreeIter* pIter,
				const char* pLabel,
				OpenViBE::Kernel::EVisualisationTreeNode eType) { return false; }

			virtual OpenViBE::boolean findChildNodeFromRoot(
				::GtkTreeIter* pIter,
				void* pWidget) { return false; }

			virtual OpenViBE::boolean findChildNodeFromParent(
				::GtkTreeIter* pIter,
				void* pWidget) { return false; }

			virtual OpenViBE::boolean findChildNodeFromRoot(
				::GtkTreeIter* pIter,
				OpenViBE::CIdentifier oIdentifier) { return false; }

			virtual OpenViBE::boolean findChildNodeFromParent(
				::GtkTreeIter* pIter,
				OpenViBE::CIdentifier oIdentifier) { return false; }

			virtual OpenViBE::boolean findParentNode(
				::GtkTreeIter* pIter,
				OpenViBE::Kernel::EVisualisationTreeNode eType) { return false; }

			virtual OpenViBE::boolean dragDataReceivedInWidgetCB(
				const OpenViBE::CIdentifier& rSrcIdentifier,
				::GtkWidget* pDstWidget) { return false; }

			virtual OpenViBE::boolean dragDataReceivedOutsideWidgetCB(
				const OpenViBE::CIdentifier& rSrcIdentifier,
				::GtkWidget* pDstWidget,
				OpenViBE::Kernel::EDragDataLocation eLocation) { return false; }

			virtual OpenViBE::boolean setToolbar(
				const OpenViBE::CIdentifier& rBoxIdentifier,
				::GtkWidget* pToolbarWidget) { return true; }

			virtual OpenViBE::boolean setWidget(
				const OpenViBE::CIdentifier& rBoxIdentifier,
				::GtkWidget* pTopmostWidget) { return true; }

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IAttributable, OV_ClassId_Kernel_Visualisation_VisualisationTree)
		};
	};
};

#endif // __OpenViBE_Kernel_Visualisation_IVisualisationTreeNull_H__
