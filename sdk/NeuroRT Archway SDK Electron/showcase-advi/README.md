# Advanced Visualization JS Library showcase with Electron SDK for Archway

Very short demonstration of the capabilities of the Advanced Visualization JS Library, using the Electron SDK of Archway.

## Content

This showcase is composed of the following files :
  + run.cmd
  + src
    * advi.min.js               -> the Advanced Visualization JS Library
    * array-methods-polyfill.js -> polyfill for missing methods in the SDK
    * config.conf               -> configuration of Archway
    * index.html                -> front-end of the application (where the code goes)
    * main.js                   -> back-end of the application
    * package.json              -> node configuration
    * usecases.json             -> configuration file
    * webgl-utils.js            -> tiny utils file for WebGL use

## Utilisation

Make sure the SDK is present in the upper folder (follow instructions in the README if it isn't the case). Run the `run.cmd` file to launch the application.

Once in the application, the user can view four visualizations, all coming from the `monitor-general` pipeline :
  + Raw EEG on oscilloscope
  + Spectrum display
  + Band power display on 2D topography (alpha band 8-12 Hz)
  + LORETA display (alpha band 8-12 Hz)

All visualizations can be paused/resumed, using the **Pause** and **Play** buttons. Using the right click, move the mouse and scale the data.

For the LORETA visualization, the only 3D one in this application, use the middle click and a mouse movement to (un)zoom the visualization, and a left click with a mouse movement to rotate the visualization in space.

### Configuration

Some parameters are configurable in the `usecases.json` file, feel free to change it. The `pipeline` parameter is set to `monitor-general`. Each use case has a channel (see `monitor-general`) and settings for a visualization. For more settings for each visualization, see the documentation of the Advanced Visualization JS Library.