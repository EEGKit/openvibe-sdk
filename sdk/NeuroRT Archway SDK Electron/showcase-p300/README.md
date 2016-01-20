# P300 showcase with Electron SDK for Archway

Very short demonstration of a P300 using the Electron SDK of Archway.

## Content

This showcase is composed of the following files :
  + run.cmd
  + src
    * config-p300.json      -> configuration file
    * config.conf           -> configuration of Archway
    * index.html            -> front-end of the application, if you want to modify something in the application it's here
    * main.js               -> back-end of the application
    * package.json

## Utilisation

Make sure the SDK is present in the upper folder (follow instructions in the README if it isn't the case). Run the `run.cmd` file to launch the application.

Once in the application, all the user has to do is look at the screen. After hitting the **Play** button, a red or gray square will appear on the screen. This behavior occurs 100 times, for 20 randomly selected red squares and 80 randomly selected gray square.

### Configuration

Some parameters are configurable in the `config-p300.json` file, feel free to change it :

````js
{
  "pipeline": "0x00000024"    // The pipeline to connect to
  "delayBetweenFlash": 350,   // The delay between each (nothing displayed during this time)
  "durationFlash": 100,       // The duration of a flash (a square is displayed)
  "backgroundColor": "white", // Color of the background

  "target": {
    "number": 20,             // The number of target flash to display
    "color": "red",           // The color of a target flash
    "label": "0x00008205"     // The OVTK_StimulationId_Target tag
  },

  "nonTarget": {
    "number": 80,             // The number of non-target flash to display
    "color": "gray",          // The color of a non-target flash
    "label": "0x00008206"     // The OVTK_StimulationId_NonTarget tag
  }
}
````

## Analyze

Output of the session will be located in :
  + for a `local` engine, in `$HOME/mensia-data/showcase-p300`
  + for a `lan` engine, in `$HOME/mensia-data/showcase-p300` on the machine the server is running on

The output file contains the signal acquired during the session, and Stimulation tags marking *Target* and *NonTarget* (respectfully `OVTK_StimulationId_Target` and `OVTK_StimulationId_NonTarget`). An available scenario to view the output is `advanced-visualization-erp-2-replay`, located in the `Showcases/advanced-visualization-erp` folder.
