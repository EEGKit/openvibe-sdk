/*
 * MENSIA TECHNOLOGIES CONFIDENTIAL
 * ________________________________
 *
 *  [2012] - [2015] Mensia Technologies SA
 *  Copyright, All Rights Reserved.
 *
 * NOTICE: All information contained herein is, and remains
 * the property of Mensia Technologies SA.
 * The intellectual and technical concepts contained
 * herein are proprietary to Mensia Technologies SA
 * and are covered copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Mensia Technologies SA.
 */

var app = require('app');
var path = require('path');
var archway = require('../../SDK/archway-node');

// Get the last Archway error code and the accompanying text description
// print them and exit the application
function handleError() {
  var lastError = archway.getLastError();
  console.log("Last error", lastError, '[', archway.getErrorString(lastError), ']');
  app.exit();
}

// This method will be called when atom-shell has done everything
// initialization and ready for creating browser windows.
app.on('ready', function() {

  // Log the version of the node.js runtime
  console.log(process.version);

  // Get the currently used Archway version
  console.log(archway.getVersionDescription());

  // Initialize the application and load the configuration file
  if (!archway.initialize("login", "password", "console-application", "config.conf")) {
    handleError();
  }
  console.log("Archway initialized");

  // Start the acquisition on all devices
  if (!archway.startAllAcquisitionDevices()) {
    handleError();
  }

  // Create an Echo (0x0) pipeline which sends back raw data
  var echoPipelineId = archway.createPipeline(0x0, "");

  if (echoPipelineId === 0) {
    handleError();
  }

  // Start the Engine
  if (!archway.startEngine()) {
    handleError();
  }

  // Send an event to the Echo pipeline, this event will be returned
  if (!archway.triggerEvent(echoPipelineId, /* channelId */ 1, /* eventId */ 100)) {
    handleError();
  }


  // Note the start time of the loop
  var startTime = new Date();

  var running = true;

  var loop = function () {
    // If we are still running then call the loop again in 10ms, otherwise stop
    if (running) {
      setTimeout(loop, 10);
    } else {
      return;
    }

    // Perform a loop of the Engine, this will update buffers in Archway
    if (!archway.mainloop()) {
      running = false;
    }

    // Get the incoming values from the engine and print the first channel value
    while (archway.getPendingValueCount(echoPipelineId, 1) > 0) {
      var values = archway.getPendingValue(echoPipelineId, 1);
      console.log(values[0]);
    }

    // Get the incoming events from the engine, this will return the trigger we sent to the pipeline
    while (archway.getPendingEventCount(echoPipelineId, 1) > 0) {
      var eventId = archway.getPendingEvent(echoPipelineId, 1);
      console.log('Event received: ' + eventId);
    }

    // If the program has been running for an amount of time, stop it
    if ((new Date()) - startTime > 1000) {
      running = false;

      // Stop the Engine
      if (!archway.stopEngine()) {
        handleError();
      }

      // Stop the acquisition devices
      if (!archway.stopAllAcquisitionDevices()) {
        handleError();
      }

      // Uninitialize the Archway library
      if (!archway.uninitialize()) {
        handleError();
      }
      // exit the application
      app.exit();
    }
  }

  loop();


});
