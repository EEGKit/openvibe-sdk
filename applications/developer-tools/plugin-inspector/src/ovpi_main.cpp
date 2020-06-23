#include "ovpiCPluginObjectDescEnumBoxTemplateGenerator.h"

#include <iostream>
#include <vector>
#include <cstring>

#include <fs/Files.h>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace std;

int main(int argc, char** argv)
{
	//___________________________________________________________________//
	//                                                                   //
	/*
	USAGE:
	plugin-inspector <plugin1 plugin2 ...>
	                 <--box-doc-directory dir>
	*/

	vector<string> pluginFilestoLoad;

	string docTemplateDir;
	bool ignoreMetaboxes = false;
	vector<string> metaboxExtensionsToLoad;

	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0)
		{
			cout << "[ USAGE ]\n"
					<< "plugin-inspector <plugin1 plugin2 ...>\n"
					<< "  <--box-doc-directory dir>\n";
			return 0;
		}
		cout << "Analyze parameter: [" << i << " : " << argv[i] << "]." << endl;

		if (strcmp(argv[i], "--ignore-metaboxes") == 0) { ignoreMetaboxes = true; }
		else if (i < argc && strcmp(argv[i], "--box-doc-directory") == 0)
		{
			if (++i >= argc)
			{
				cout << "[ FAILED ] Error while parsing arguments: --box-doc-directory flag found but no path specified afterwards." << endl;
				return 0;
			}
			docTemplateDir = argv[i];
			cout << "Templates will be generated in folder: [" << docTemplateDir << "]." << endl;
		}
		else if (i < argc) { pluginFilestoLoad.push_back(string(argv[i])); }
	}

	CKernelLoader kernelLoader;

	cout << "[  INF  ] Created kernel loader, trying to load kernel module" << endl;
	CString errorMsg;
#if defined TARGET_OS_Windows
	const CString kernelFile = Directories::getLibDir() + "/openvibe-kernel.dll";
#elif defined TARGET_OS_Linux
	const CString kernelFile = Directories::getLibDir() + "/libopenvibe-kernel.so";
#elif defined TARGET_OS_MacOS
	const CString kernelFile = Directories::getLibDir() + "/libopenvibe-kernel.dylib";
#endif

	if (!kernelLoader.load(kernelFile, &errorMsg)) { cout << "[ FAILED ] Error loading kernel (" << errorMsg << ")" << " from [" << kernelFile << "]\n"; }
	else
	{
		cout << "[  INF  ] Kernel module loaded, trying to get kernel descriptor" << endl;
		IKernelDesc* kernelDesc = nullptr;
		kernelLoader.initialize();
		kernelLoader.getKernelDesc(kernelDesc);
		if (!kernelDesc) { cout << "[ FAILED ] No kernel descriptor" << endl; }
		else
		{
			cout << "[  INF  ] Got kernel descriptor, trying to create kernel" << endl;

			IKernelContext* ctx = kernelDesc->createKernel("plugin-inspector", Directories::getDataDir() + "/kernel/openvibe.conf");
			if (!ctx) { cout << "[ FAILED ] No kernel created by kernel descriptor" << endl; }
			else
			{
				ctx->initialize();
				Toolkit::initialize(*ctx);

				IConfigurationManager& configurationManager = ctx->getConfigurationManager();

				if (pluginFilestoLoad.empty()) { ctx->getPluginManager().addPluginsFromFiles(configurationManager.expand("${Kernel_Plugins}")); }
				else
				{
					for (const string& file : pluginFilestoLoad)
					{
						ctx->getPluginManager().addPluginsFromFiles(configurationManager.expand(CString(file.c_str())));
					}
				}

				ctx->getLogManager() << LogLevel_Info << "[  INF  ] Generate boxes templates in [" << docTemplateDir << "]\n";

				CPluginObjectDescEnumBoxTemplateGenerator boxTemplateGenerator(*ctx, string(docTemplateDir));
				if (!boxTemplateGenerator.initialize())
				{
					cout << "[ FAILED ] Could not initialize boxTemplateGenerator" << endl;
					return 0;
				}
				boxTemplateGenerator.enumeratePluginObjectDesc(OV_ClassId_Plugins_BoxAlgorithmDesc);

				if (!ignoreMetaboxes)
				{
					ctx->getLogManager() << LogLevel_Info << "[  INF  ] Generate metaboxes templates in [" << docTemplateDir << "]\n";
					// Do not load the binary metaboxes as they would only be duplicated
					//ctx->getScenarioManager().unregisterScenarioImporter(OV_ScenarioImportContext_OnLoadMetaboxImport, ".mbb");
					configurationManager.addOrReplaceConfigurationToken("Kernel_Metabox", "${Path_Data}/metaboxes/");

					ctx->getMetaboxManager().addMetaboxesFromFiles(configurationManager.expand("${Kernel_Metabox}"));

					// Create a list of metabox descriptors from the Map provided by the MetaboxLoader and enumerate all algorithms within
					std::vector<const IPluginObjectDesc*> metaboxPluginObjectDescriptors;
					CIdentifier id;
					while ((id = ctx->getMetaboxManager().getNextMetaboxObjectDescIdentifier(id)) != CIdentifier::undefined())
					{
						metaboxPluginObjectDescriptors.push_back(ctx->getMetaboxManager().getMetaboxObjectDesc(id));
					}
					boxTemplateGenerator.enumeratePluginObjectDesc(metaboxPluginObjectDescriptors);
				}

				if (!boxTemplateGenerator.uninitialize())
				{
					cout << "[ FAILED ] Could not uninitialize boxTemplateGenerator" << endl;
					return 0;
				}
				ctx->getLogManager() << LogLevel_Info << "Application terminated, releasing allocated objects \n";

				Toolkit::uninitialize(*ctx);

				kernelDesc->releaseKernel(ctx);
			}
		}
		kernelLoader.uninitialize();
		kernelLoader.unload();
	}

	return 0;
}
