#pragma once

#include <memory>

//___________________________________________________________________//
//                                                                   //
// Basic includes                                                    //
//___________________________________________________________________//
//                                                                   //

#include "ov_defines.h"
#include "ov_types.h"
#include "ov_directories.h"

#include "ovCIdentifier.h"
#include "ovCString.h"
#include "ovCNameValuePairList.h"

#include "ovIMatrix.h"
#include "ovCMatrix.h"

#include "ovIStimulationSet.h"
#include "ovCStimulationSet.h"

#include "ovIMemoryBuffer.h"
#include "ovCMemoryBuffer.h"

#include "ovIObject.h"
#include "ovIObjectVisitor.h"
#include "ovIKernelLoader.h"
#include "ovCKernelLoader.h"

#include "ovIObserver.h"
#include "ovCObservable.h"

#include "ovAssert.h"

//___________________________________________________________________//
//                                                                   //
// Kernel includes                                                   //
//___________________________________________________________________//
//                                                                   //

#include "kernel/ovIKernelContext.h"
#include "kernel/ovIKernelDesc.h"
#include "kernel/ovIKernelObject.h"
#include "kernel/ovIKernelObjectFactory.h"

#include "kernel/ovITypeManager.h"
#include "kernel/ovIParameter.h"
#include "kernel/ovIConfigurable.h"
#include "kernel/ovIObjectVisitorContext.h"
#include "kernel/ovTParameterHandler.h"

#include "kernel/algorithm/ovIAlgorithmContext.h"
#include "kernel/algorithm/ovIAlgorithmManager.h"
#include "kernel/algorithm/ovIAlgorithmProto.h"
#include "kernel/algorithm/ovIAlgorithmProxy.h"

#include "kernel/configuration/ovIConfigurationManager.h"
#include "kernel/configuration/ovIConfigurationKeywordExpandCallback.h"

#include "kernel/log/ovILogManager.h"
#include "kernel/log/ovILogListener.h"

#include "kernel/error/ovIErrorManager.h"
#include "kernel/error/ovIError.h"
#include "kernel/error/ovErrorType.h"

#include "kernel/player/ovIBoxAlgorithmContext.h"
#include "kernel/player/ovIMessage.h"
#include "kernel/player/ovIMessageClock.h"
#include "kernel/player/ovIMessageEvent.h"
#include "kernel/player/ovIMessageSignal.h"
#include "kernel/player/ovIPlayer.h"
#include "kernel/player/ovIPlayerContext.h"
#include "kernel/player/ovIPlayerManager.h"

#include "kernel/plugins/ovIPluginManager.h"
#include "kernel/plugins/ovIPluginModule.h"
#include "kernel/plugins/ovIPluginModuleContext.h"

#include "kernel/metabox/ovIMetaboxManager.h"

#include "kernel/scenario/ovIAttributable.h"
#include "kernel/scenario/ovIBox.h"
#include "kernel/scenario/ovIBoxIO.h"
#include "kernel/scenario/ovIBoxListenerContext.h"
#include "kernel/scenario/ovIBoxProto.h"
#include "kernel/scenario/ovIComment.h"
#include "kernel/scenario/ovIMetadata.h"
#include "kernel/scenario/ovILink.h"
#include "kernel/scenario/ovIScenario.h"
#include "kernel/scenario/ovIScenarioManager.h"
#include "kernel/scenario/ovIAlgorithmScenarioImporter.h"
#include "kernel/scenario/ovIAlgorithmScenarioExporter.h"

//___________________________________________________________________//
//                                                                   //
// Plugins includes                                                  //
//___________________________________________________________________//
//                                                                   //

#include "plugins/ovIPluginObject.h"
#include "plugins/ovIPluginObjectDesc.h"

#include "plugins/ovIAlgorithm.h"
#include "plugins/ovIAlgorithmDesc.h"

#include "plugins/ovIBoxAlgorithm.h"
#include "plugins/ovIBoxAlgorithmDesc.h"

namespace OpenViBE
{
	namespace Plugins
	{
		// Backward compatibility
		typedef Kernel::IBox IStaticBoxContext;
		typedef Kernel::IBoxIO IDynamicBoxContext;
	};
};

//___________________________________________________________________//
//                                                                   //
// Plugins includes                                                  //
//___________________________________________________________________//
//                                                                   //

#define OVP_Declare_Begin() \
	static std::vector<std::unique_ptr<OpenViBE::Plugins::IPluginObjectDesc>> g_descriptors; \
	extern "C" \
	{ \
		OVP_API bool onInitialize(const OpenViBE::Kernel::IPluginModuleContext& rPluginModuleContext) \
		{

#define OVP_Declare_New(Class) \
			g_descriptors.emplace_back(new Class);

#define OVP_Declare_End() \
			return true; \
		} \
		OVP_API bool onUninitialize(const OpenViBE::Kernel::IPluginModuleContext& rPluginModuleContext) \
		{ \
			return true; \
		} \
		OVP_API bool onGetPluginObjectDescription(const OpenViBE::Kernel::IPluginModuleContext& rPluginModuleContext, uint32_t ui32Index, OpenViBE::Plugins::IPluginObjectDesc*& rpPluginObjectDescription) \
		{ \
			if(ui32Index>=g_descriptors.size()) \
			{ \
				rpPluginObjectDescription=NULL; \
				return false; \
			} \
			rpPluginObjectDescription=g_descriptors[ui32Index].get(); \
			return true; \
		} \
	}
