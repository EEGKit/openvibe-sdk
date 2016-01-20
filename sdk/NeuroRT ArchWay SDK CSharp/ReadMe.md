MENSIA TECHNOLOGIES CONFIDENTIAL

[2012] - [2016] Mensia Technologies SA

Copyright, All Rights Reserved.

NOTICE: All information contained herein is, and remains the
property of Mensia Technologies SA. The intellectual and
technical concepts contained herein are proprietary to Mensia
Technologies SA and are covered copyright law. Dissemination of
this information or reproduction of this material is strictly forbidden unless prior written permission is obtained from
Mensia Technologies SA.

---

This ReadMe file contains informations on how to use this Archway C++ SDK to build an application powered by the Mensia Archway. 

The SDK itself is a file "mensia-engine.DLL", that can be used with C++. This folder also contains a Visual Studio solution that can be used to launch example projects:
- Application Template: 
- Example Synchronous API
- Example Asynchronous API
These projects basically do the same treatment, but introduce different ways of doing it with the API. 

# Requirements

The SDK requires at least a version 2010 of Visual Studio to be installed. You can download the community version following this link: https://www.visualstudio.com/products/visual-studio-express-vs

# Quick Start

You can open the solution with Visual Studio, by launching the "neurort-launch-vs.cmd" file. This file loads the environment and launches Visual Studio. For more information regarding the environment set by this script, see section "Environment".

By default, these projects opens a connection with the Mensia Engine, create a pipeline "Monitoring" start streaming data from the simulator, and open a visualization. These very simple projects gives you an example of how to start Archway, create and start a processing pipeline, create a visualization and uninitialize Archway. To have more information about the different functions available in the API, see the documentation "NeuroRT Engine Library and API" (available in start menu).

The example script will connect to Archway and start streaming data. By default it will connect to the "Mensia Engine Local" and will start streaming data from the simulator. If you want to change this behaviour, you should write a file "lib-mensia-engine.conf" under your HOME folder, see "Configure the Engine".

# Environment

Before using this example project, there are tools and programs that have to be installed, and a few environment variables that need to be set.

## Archway dependencies
Archway dependencies are present in the solution directory, under "NeuroRT SDK" folder. These dependencies are necessary to launch the projects with NeuroRT Engine Server.
In this folder you will find:
	- Lua 5.1: an Archway dependency
	- SFML.NET: Used in this project to define an OpenGL context and enable the use of Archway Visualizations.

To use the  Engine Local, there is a few environment variables that need to be set. The script "neurort-launch-vs.cmd" is used to set this environment variables.
- "OV_PATH_ROOT" is set as the path to your installation directory, it is used to set the Mensia Engine dependencies such as Lua.
- "OV_PATH_BIN" defines the path to the bin folder of your installation directory. It is needed to launch the "mensia-engine-local.exe".
- "OV_PATH_DATA" "MENSIA_PATH_DATA" are both used to load the engine pipelines (that are available under %MENSIA_PATH_DATA%\share\mensia\scenarios) and some configuration files.
- The Windows Path must be updated to add:
	* Expat: used by OpenViBE Module XML.
	* OpenAl and its dependencies FreeAlut, LibOgg and LibVorbis: used by the stimulation plugin for sound stimulation.
	* Gtk: used in OpenViBE Kernel.

## Mensia Engine Server

Depending on your license, the Python SDK can be used either with the Mensia Engine Local or with the Mensia Engine Server.
Studio license allows you to use the "Mensia Engine Local".

### Mensia Engine Local

In order to use the Mensia Engine Local, you just need to set a few environment variables as described in "Archway dependencies" section so that the executable "mensia-engine-local.exe" is found. Archway will then take care of launching it when needed. 

### Mensia Engine Server

If you wish to use the "Mensia Engine Server", then you need to launch it before starting to stream data.

In order to use the Example application you will need either a LAN version of the Mensia Engine Server running, either the Mensia Local Engine application present and available in the Path.

### Configure the Engine

By default, the Archway first tries to connect to the "Mensia Engine Local", then to the "Mensia Engine Server",  on the address '127.0.0.1' and port 1337.

You can change this behaviour by writing a configuration file named "lib-mensia-engine.conf" under your home directory (C:/Users/%USERNAME%). The file "lib-mensia-engine.conf-skeleton" available under the folder "share/mensia" of your installation directory introduces the possible configurations that can be chosen. To configure the acquisition with a real device, see the Device section in the "NeuroRT Engine Library and API" documentation
