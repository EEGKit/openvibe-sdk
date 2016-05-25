#include "ovpiCPluginObjectDescEnumAlgorithmGlobalDefinesGenerator.h"
#include "ovpiCPluginObjectDescEnumBoxAlgorithmSnapshotGenerator.h"
#include "ovpiCPluginObjectDescEnumAlgorithmSnapshotGenerator.h"
#include "ovpiCPluginObjectDescEnumManifestGenerator.h"

// Pull in the designer's MetaboxLoader
#include <mCMetaboxLoader.h>
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstring>

#include <fs/Files.h>

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;
using namespace std;

int main(int argc, char ** argv)
{
//___________________________________________________________________//
//                                                                   //
	/*
	USAGE:
	plugin-inspector <plugin1 plugin2 ...>
	                 <--generate-manifest manifest>
	                 <--manifest manifest1 <manifest 2 manifest 3 ...> >
	                 <--global-defines-directory dir>
	                 <--algorithm-snapshot-directory dir>
	                 <--algorithm-doc-directory dir>
	                 <--box-snapshot-directory dir>
	                 <--box-doc-directory dir>
	*/

	vector<string> l_vPluginFilestoLoad;
	// uint32 l_ui32PluginfileIndex = 0;

	vector<string> l_vPluginManifestFiles;
	boolean l_bParsingManifests = false;

	string l_sSpecifiedGlobalDefinesDirectory;
	string l_sSpecifiedAlgorithmDocTemplateDirectory;
	string l_sSpecifiedBoxAlgorithmSnapshotDirectory;
	string l_sSpecifiedBoxAlgorithmDocTemplateDirectory;
	string l_sSpecifiedTargetManifest;
	bool l_bKernelPathOverload = false;
//	bool l_bConfigPathOverload = false;
	bool l_bIgnoreMetaboxes = false;
	vector<string> l_vMetaboxExtensionsToLoad;
	string l_sKernelPath;


	for(int32 i = 1; i < argc; i++)
	{
		if(::strcmp(argv[i], "--help") == 0 || ::strcmp(argv[i], "-h") == 0)
		{
			::cout << "[ USAGE ]\n"
					<< "plugin-inspector <plugin1 plugin2 ...>\n"
					<< "  <--generate-manifest manifest>\n"
					<< "  <--manifest manifest1 <manifest 2 manifest 3 ...> >\n"
					<< "  <--global-defines-directory dir>\n"
					<< "  <--algorithm-snapshot-directory dir>\n"
					<< "  <--algorithm-doc-directory dir>\n"
					<< "  <--box-snapshot-directory dir>\n"
					<< "  <--box-doc-directory dir>\n"
					<< "  <--kernel-path dir>\n"
					<< "  <--config-path dir>\n"
					<< "  <--ignore-metaboxes>\n"
					<< "  <--metabox-extension [xml|mxb|mbb]> (Can be specified multiple times)\n";
			return 0;
		}

		if(::strcmp(argv[i], "--generate-manifest") == 0)
		{
			if(++i >= argc)
			{
				::cout << "[ FAILED ] Error while parsing arguments: --generate-manifest flag found but no path specified afterwards."<<endl;
				return 0;
			}
			l_bParsingManifests = false;
			l_sSpecifiedTargetManifest = argv[i];
		}
		else if(::strcmp(argv[i], "--ignore-metaboxes") == 0)
		{
			l_bIgnoreMetaboxes = true;
		}
		else if(::strcmp(argv[i], "--metabox-extension") == 0)
		{
			if(++i >= argc)
			{
				::cout << "[ FAILED ] Error while parsing arguments: --metabox-extension parameter found but no extension specified afterwards."<<endl;
				return 0;
			}

			if (!(strcmp(argv[i], "xml") == 0 || strcmp(argv[i], "mxb") == 0 || strcmp(argv[i], "mbb") == 0))
			{
				::cout << "[ FAILED ] Error while parsing arguments: --metabox-extension parameter value must be xml, mxs or mbb."<<endl;
				return 0;
			}

			l_bParsingManifests = false;
			l_vMetaboxExtensionsToLoad.push_back(argv[i]);
		}
		else if(::strcmp(argv[i], "--global-defines-directory") == 0)
		{
			if(++i >= argc)
			{
				::cout << "[ FAILED ] Error while parsing arguments: --global-defines-directory flag found but no path specified afterwards."<<endl;
				return 0;
			}
			l_bParsingManifests = false;
			l_sSpecifiedGlobalDefinesDirectory = argv[i];
		}
		else if(i < argc && ::strcmp(argv[i], "--algorithm-snapshot-directory") == 0)
		{
			if(++i >= argc)
			{
				::cout << "[ FAILED ] Error while parsing arguments: --algorithm-snapshot-directory flag found but no path specified afterwards."<<endl;
				return 0;
			}
			l_bParsingManifests = false;
			//l_sSpecifiedAlgorithmSnapshotDirectory = argv[i];
		}
		else if(i < argc && ::strcmp(argv[i], "--algorithm-doc-directory") == 0)
		{
			if(++i >= argc)
			{
				::cout << "[ FAILED ] Error while parsing arguments: --algorithm-doc-directory flag found but no path specified afterwards."<<endl;
				return 0;
			}
			l_bParsingManifests = false;
			l_sSpecifiedAlgorithmDocTemplateDirectory = argv[i];
		}
		else if(i < argc && ::strcmp(argv[i], "--box-snapshot-directory") == 0)
		{
			if(++i >= argc)
			{
				::cout << "[ FAILED ] Error while parsing arguments: --box-snapshot-directory flag found but no path specified afterwards."<<endl;
				return 0;
			}
			l_bParsingManifests = false;
			l_sSpecifiedBoxAlgorithmSnapshotDirectory = argv[i];
		}
		else if(i < argc && ::strcmp(argv[i], "--box-doc-directory") == 0)
		{
			if(++i >= argc)
			{
				::cout << "[ FAILED ] Error while parsing arguments: --box-doc-directory flag found but no path specified afterwards."<<endl;
				return 0;
			}
			l_bParsingManifests = false;
			l_sSpecifiedBoxAlgorithmDocTemplateDirectory = argv[i];
		}
		else if(i < argc && ::strcmp(argv[i], "--manifest") == 0)
		{
			if(i >= argc)
			{
				::cout << "[ FAILED ] Error while parsing arguments: --manifest flag found but no manifest file specified afterwards."<<endl;
				return 0;
			}
			::cout << "[  INF  ] Manifest file(s) specified."<<endl;
			l_bParsingManifests = true;
		}
		// Configuration path
		else if (i < argc && ::strcmp(argv[i], "--config-path") == 0)
		{
			if(++i >= argc)
			{
				::cout << "[ FAILED ] Error while parsing arguments: --config-path flag found but no path specified afterwards."<<endl;
				return 0;
			}
			// l_bConfigPathOverload = true;
			//l_sConfigPath = argv[i];
		}
		// Kernel path
		else if (i < argc && ::strcmp(argv[i], "--kernel-path") == 0)
		{
			if(++i >= argc)
			{
				::cout << "[ FAILED ] Error while parsing arguments: --config-path flag found but no path specified afterwards."<<endl;
				return 0;
			}
			l_bKernelPathOverload = true;
			l_sKernelPath = argv[i];
		}
		else if(i < argc)
		{
			if(l_bParsingManifests)
			{
				l_vPluginManifestFiles.push_back(string(argv[i]));
			}
			else
			{
				l_vPluginFilestoLoad.push_back(string(argv[i]));
			}
		}
	}
	/* We have now the two vectors containing plugin files to load and their corresponding manifests */
	vector < uint64 > l_vAuthorizedOpenViBEPlugins;
	if(l_vPluginManifestFiles.size() == 0)
	{
		::cout << "[  INF  ] No manifest file specified, every plugin will be selected."<<endl;
	}
	for(uint32 i=0; i<l_vPluginManifestFiles.size(); i++)
	{
		if(l_vPluginManifestFiles[i].size() != 0)
		{
			ifstream l_oManifestFile;
			::cout << "[  INF  ] Opening manifest file ["<<l_vPluginManifestFiles[i]<<"] for Plugin ["<<l_vPluginFilestoLoad[i]<<"]"<<endl;
			FS::Files::openIFStream(l_oManifestFile, l_vPluginManifestFiles[i].c_str());
			if(!l_oManifestFile.good())
			{
				::cout << "[ FAILED ] Could not open file " << l_vPluginManifestFiles[i].c_str() << "\n";
				return 0;
			}

			string l_sLine;
			uint32 l_iLineIndex = 1;
			while(!l_oManifestFile.eof())
			{
				getline(l_oManifestFile, l_sLine, '\n');
				if(! (l_sLine.size() == 0 || l_sLine[0]=='#')) // comments start with a #
				{
#if 0
					try
					{
						uint64 l_ui64Identifier = stoull(l_sLine, nullptr, 0); // expect an uint64 in hexadecimal
						::cout << "[  INF  ] Manifest entry found: "<<l_sLine<<endl;
						l_vAuthorizedOpenViBEPlugins.push_back(l_ui64Identifier);
					}
					catch(...)
					{
						::cout << "[ FAILED ] An error happenend while parsing manifest file [" << l_vPluginManifestFiles[i].c_str() << "] line #"<<l_iLineIndex<<"\n";
						return 0;
					}
#else
					uint64 l_ui64Identifier=0;
					if(::sscanf(l_sLine.c_str(), "%llx", &l_ui64Identifier) == 1)
					{
						::cout << "[  INF  ] Manifest entry found: "<<l_sLine<<endl;
						l_vAuthorizedOpenViBEPlugins.push_back(l_ui64Identifier);
					}
					else
					{
						::cout << "[ FAILED ] An error happenend while parsing manifest file [" << l_vPluginManifestFiles[i].c_str() << "] line #"<<l_iLineIndex<<"\n   " << l_sLine << "\n";
						return 0;
					}
#endif
				}
				l_iLineIndex++;
			}
		}
	}

	CKernelLoader l_oKernelLoader;

	::cout<<"[  INF  ] Created kernel loader, trying to load kernel module"<<endl;
	CString l_sError;
#if defined TARGET_OS_Windows
	CString l_sKernelFile = "/openvibe-kernel.dll";
#elif defined TARGET_OS_Linux
	CString l_sKernelFile = "/libopenvibe-kernel.so";
#elif defined TARGET_OS_MacOS
	CString l_sKernelFile = "/libopenvibe-kernel.dylib";
#endif

	if (l_bKernelPathOverload)
	{
		std::cout << "[  INF  ] Kernel path overriden, loading from [" << l_sKernelFile << "]" << std::endl;
		l_sKernelFile = CString(l_sKernelPath.c_str()) + l_sKernelFile;
	}
	else
	{
		l_sKernelFile = OpenViBE::Directories::getLibDir() + l_sKernelFile;
	}

	if(!l_oKernelLoader.load(l_sKernelFile, &l_sError))
	{
			cout<<"[ FAILED ] Error loading kernel ("<<l_sError<<")" << " from [" << l_sKernelFile << "]\n";
	}
	else
	{
		cout<<"[  INF  ] Kernel module loaded, trying to get kernel descriptor"<<endl;
		IKernelDesc* l_pKernelDesc=NULL;
		IKernelContext* l_pKernelContext=NULL;
		l_oKernelLoader.initialize();
		l_oKernelLoader.getKernelDesc(l_pKernelDesc);
		if(!l_pKernelDesc)
		{
			::cout<<"[ FAILED ] No kernel descriptor"<<endl;
		}
		else
		{
			::cout<<"[  INF  ] Got kernel descriptor, trying to create kernel"<<endl;
			CString l_sConfigPath = OpenViBE::Directories::getDataDir() + "/openvibe.conf";
			l_pKernelContext=l_pKernelDesc->createKernel("plugin-inspector", l_sConfigPath );
			if(!l_pKernelContext)
			{
				::cout<<"[ FAILED ] No kernel created by kernel descriptor"<<endl;
			}
			else
			{
				OpenViBEToolkit::initialize(*l_pKernelContext);

// For Mister Vincent !
#ifdef OVAS_OS_Windows
#ifndef NDEBUG
				//_asm int 3;
#endif
#endif

				// Load metaboxes from the distribution folder
				Mensia::CMetaboxLoader l_oMetaboxLoader(*l_pKernelContext);

				IConfigurationManager& l_rConfigurationManager=l_pKernelContext->getConfigurationManager();

				if(l_vPluginFilestoLoad.size() == 0)
				{
					l_pKernelContext->getPluginManager().addPluginsFromFiles(l_rConfigurationManager.expand("${Kernel_Plugins}"));
				}
				else
				{
					for(uint32 i=0; i<l_vPluginFilestoLoad.size(); i++)
					{
						l_pKernelContext->getPluginManager().addPluginsFromFiles(l_rConfigurationManager.expand(CString(l_vPluginFilestoLoad[i].c_str())));
					}
				}

				//initialise Gtk before 3D context
				gtk_init(&argc, &argv);
				// gtk_rc_parse(OpenViBE::Directories::getDataDir() + "/applications/designer/interface.gtkrc");

				if (!l_bIgnoreMetaboxes)
				{
					// by default (if no parameters are specified) we load only mxb type metaboxes
					if (l_vMetaboxExtensionsToLoad.empty()) {
						l_vMetaboxExtensionsToLoad.push_back("mxb");
					}
					for (auto it = l_vMetaboxExtensionsToLoad.cbegin(); it != l_vMetaboxExtensionsToLoad.cend(); it++)
					{
						CString l_sMetaboxSearchPath = CString("${Path_Data}/metaboxes/*.") + it->c_str();
						l_oMetaboxLoader.loadPluginDescriptorsFromWildcard(l_pKernelContext->getConfigurationManager().expand(l_sMetaboxSearchPath));
					}
				}



				CString l_sGlobalDefinesDirectory          =l_rConfigurationManager.expand("${PluginInspector_DumpGlobalDefinesDirectory}");
				CString l_sAlgorithmSnapshotDirectory      =l_rConfigurationManager.expand("${PluginInspector_DumpAlgorithmSnapshotDirectory}");
				CString l_sAlgorithmDocTemplateDirectory   =l_rConfigurationManager.expand("${PluginInspector_DumpAlgorithmDocTemplateDirectory}");
				CString l_sBoxAlgorithmSnapshotDirectory   =l_rConfigurationManager.expand("${PluginInspector_DumpBoxAlgorithmSnapshotDirectory}");
				CString l_sBoxAlgorithmDocTemplateDirectory=l_rConfigurationManager.expand("${PluginInspector_DumpBoxAlgorithmDocTemplateDirectory}");
				if(l_sSpecifiedGlobalDefinesDirectory.size() != 0)
				{
					l_sGlobalDefinesDirectory = CString(l_sSpecifiedGlobalDefinesDirectory.c_str());
				}
				if(l_sSpecifiedBoxAlgorithmSnapshotDirectory.size() != 0)
				{
					l_sAlgorithmSnapshotDirectory = CString(l_sSpecifiedBoxAlgorithmSnapshotDirectory.c_str());
				}
				if(l_sSpecifiedAlgorithmDocTemplateDirectory.size() != 0)
				{
					l_sSpecifiedAlgorithmDocTemplateDirectory = CString(l_sSpecifiedAlgorithmDocTemplateDirectory.c_str());
				}
				if(l_sSpecifiedBoxAlgorithmSnapshotDirectory.size() != 0)
				{
					l_sBoxAlgorithmSnapshotDirectory = CString(l_sSpecifiedBoxAlgorithmSnapshotDirectory.c_str());
				}
				if(l_sSpecifiedBoxAlgorithmDocTemplateDirectory.size() != 0)
				{
					l_sBoxAlgorithmDocTemplateDirectory = CString(l_sSpecifiedBoxAlgorithmDocTemplateDirectory.c_str());
				}

				if(l_sGlobalDefinesDirectory!=CString(""))
				{
					::cout<<"[  INF  ] Saving Global Defines in ["<<l_sGlobalDefinesDirectory.toASCIIString() <<"]"<< endl;
					CPluginObjectDescEnumAlgorithmGlobalDefinesGenerator l_oGlobalDefinesGenerator(*l_pKernelContext, l_sGlobalDefinesDirectory);
					l_oGlobalDefinesGenerator.enumeratePluginObjectDesc(OV_ClassId_Plugins_AlgorithmDesc, (l_vAuthorizedOpenViBEPlugins.size() > 0 ? &l_vAuthorizedOpenViBEPlugins : NULL));
				}

				if(l_sSpecifiedTargetManifest != "")
				{
					l_pKernelContext->getLogManager() << LogLevel_Info << "Loading plugins as specified by kernel\n";
					CPluginObjectDescEnumManifestGenerator l_oManifestGenerator(*l_pKernelContext, l_sSpecifiedTargetManifest.c_str());
					l_oManifestGenerator.enumeratePluginObjectDesc(l_vAuthorizedOpenViBEPlugins.size() > 0 ? &l_vAuthorizedOpenViBEPlugins : NULL);
				}

				if(l_sAlgorithmSnapshotDirectory!=CString("") && l_sAlgorithmDocTemplateDirectory!=CString(""))
				{
					::cout<<"[  INF  ] Saving Algorithm Snapshots in ["<<l_sAlgorithmSnapshotDirectory.toASCIIString() <<"]" << endl;
					::cout<<"[  INF  ] Saving Algorithm Templates in ["<<l_sAlgorithmDocTemplateDirectory.toASCIIString() <<"]" << endl;
					CPluginObjectDescEnumAlgorithmSnapshotGenerator l_oAlgorithmSnapshotGenerator(*l_pKernelContext, l_sAlgorithmSnapshotDirectory, l_sAlgorithmDocTemplateDirectory);
					l_oAlgorithmSnapshotGenerator.enumeratePluginObjectDesc(OV_ClassId_Plugins_AlgorithmDesc, (l_vAuthorizedOpenViBEPlugins.size() > 0 ? &l_vAuthorizedOpenViBEPlugins : NULL));
				}
				else 
				{
					l_pKernelContext->getLogManager() << LogLevel_Info << "Skipped, related PluginInspector tokens are empty in openvibe.conf\n";
				}

				if(l_sBoxAlgorithmSnapshotDirectory!=CString("") && l_sBoxAlgorithmDocTemplateDirectory!=CString(""))
				{
					::cout<<"[  INF  ] Saving Box Snapshots in ["<<l_sBoxAlgorithmSnapshotDirectory.toASCIIString() <<"]" << endl;
					::cout<<"[  INF  ] Saving Box Templates in ["<<l_sBoxAlgorithmDocTemplateDirectory.toASCIIString() <<"]" << endl;
					CPluginObjectDescEnumBoxAlgorithmSnapshotGenerator l_oBoxAlgorithmSnapshotGenerator(*l_pKernelContext, l_sBoxAlgorithmSnapshotDirectory, l_sBoxAlgorithmDocTemplateDirectory);
					l_oBoxAlgorithmSnapshotGenerator.enumeratePluginObjectDesc(OV_ClassId_Plugins_BoxAlgorithmDesc, (l_vAuthorizedOpenViBEPlugins.size() > 0 ? &l_vAuthorizedOpenViBEPlugins : NULL));

					// Create a list of metabox descriptors from the Map provided by the MetaboxLoader and enumerate all algorithms within
					auto& l_rMetaboxPluginObjectDescriptorsMap = l_oMetaboxLoader.getPluginObjectDescMap();
					std::vector<const OpenViBE::Plugins::IPluginObjectDesc*> l_vMetaboxPluginObjectDescriptors;

//					for (const auto& l_rMetabox : l_rMetaboxPluginObjectDescriptorsMap)
					for (auto it = l_rMetaboxPluginObjectDescriptorsMap.cbegin(); it != l_rMetaboxPluginObjectDescriptorsMap.cend(); it++)
					{
						const auto& l_rMetabox = *it;
						l_vMetaboxPluginObjectDescriptors.push_back(l_rMetabox.second);
					}
					l_oBoxAlgorithmSnapshotGenerator.enumeratePluginObjectDesc(l_vMetaboxPluginObjectDescriptors);
				}
				else
				{
					l_pKernelContext->getLogManager() << LogLevel_Info << "Skipped, related PluginInspector tokens are empty in openvibe.conf\n";
				}

				l_pKernelContext->getLogManager() << LogLevel_Info << "Application terminated, releasing allocated objects \n";

				OpenViBEToolkit::uninitialize(*l_pKernelContext);

				l_pKernelDesc->releaseKernel(l_pKernelContext);
			}
		}
		l_oKernelLoader.uninitialize();
		l_oKernelLoader.unload();
	}

	return 0;
}
