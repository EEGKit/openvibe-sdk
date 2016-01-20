# 2 Classes showcase with Electron SDK for Archway

Very short demonstration of a 2 Classes system using the Electron SDK of Archway.

## Content

This showcase is composed of the following files :
  + run.cmd
  + src
    * config-2classes.json  -> configuration file
    * config.conf           -> configuration of Archway
    * index.html            -> front-end of the application, if you want to modify something in the application it's here
    * main.js               -> back-end of the application
    * package.json

## Utilisation

Make sure the SDK is present in the upper folder (follow instructions in the README if it isn't the case). Run the `run.cmd` file to launch the application.

This showcase got three phases :
  + the first one is a calibration : the user is asked to do two tasks (focusing and relaxing), each during a certain amount of time
  + then a processing of the fetched data during the calibration is done, in order to create the two classes used for the classification
  + the third step is a game : the user should try to relax to reduce the lighting in the application, or focus to increase the lighting. The game ends when the light is almost off.

### Configuration

Some parameters are configurable in the `config-2classes.json` file, feel free to change it :

````js
{
  "calibration": {
    "pipeline": "0x00010011", // The pipeline used for the calibration
    "relax": {
      "message": "Relax...",  // The message during the relaxing phase
      "color": "green",       // The background color during the relaxing phase
      "duration": 30,         // The duration of the relaxing phase
      "label": "0x00008101"   // The label to tag with during the relaxing phase
    },
    "focus": {                // Below are same options than for the relaxing phase, but for the focusing phase
      "message": "Count backwards from 100, with steps of 7",
      "color": "gray",
      "duration": 30,
      "label": "0x00008102"
    },
    "waitMessage": "Please wait while the calibration is running" // Message displayed at the end of the calibration, while waiting the processing to be completed
  },
  "online": {
    "pipeline": "0x00010012", // The pipeline used for the game
    "valueToReach": 10,       // The value to reach (need to be inferior to it) to end the game
    "endMessage": "Congratulations !" // Message displayed at the end of the game
  }
}
````

## Analyze

Output of the session will be located in :
  + for a `local` engine, in `$HOME/mensia-data/showcase-2classes`
  + for a `lan` engine, in `$HOME/mensia-data/showcase-2classes` on the machine the server is running on

There are two output files for this application :
  + the acquisition one (suffixed with `ov-acquisition`), contains the signal acquired during the calibration, and Stimulation tags marking *class 1* and *class 2* (respectfully `OVTK_StimulationId_Label_01` and `OVTK_StimulationId_Label_02`).
  + the online one (suffixed with `ov-online`), contains the signal acquired during the calibration, with no Stimulation tags.

An available scenario to view the output of the online one is located in the `Showcases/advanced-visualization-2class` folder.
