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

// Will hold the id of the Echo pipeline once it is created
var echoPipelineId = null;

var startLoop = function (condition) {
  return new Promise( function (fulfill, reject) {

    var loop = function () {
      // If we are still running then call the loop again in 10ms, otherwise stop
      if (condition()) {
        setTimeout(loop, 10);
      } else {
        fulfill();
      }

      // Perform a loop of the Engine, this will update buffers in Archway
      if (!archway.mainloop()) {
        reject(new Error(archway.getLastError()));
      }

      if (echoPipelineId !== null) {
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
      }
    }

    loop();
  });
}

// This method will be called when atom-shell has done everything
// initialization and ready for creating browser windows.
app.on('ready', function() {

  // Log the version of the node.js runtime
  console.log(process.version);

  // Get the currently used Archway version
  console.log(archway.getVersionDescription());

  archway.asyncInitialize('login', 'password', 'console-project-async', 'config.conf').then( function () {
    console.log('Archway initialized');
    console.log('Engine type: ', archway.getEngineType());
    return archway.asyncStartAllAcquisitionDevices();

  }).then( function () {
    console.log('Acquisition devices started');
    echoPipelineId = archway.createPipeline(0x0, '');
    if (echoPipelineId == 0) {
      return Promise.reject(archway.getLastError());
    }
    return Promise.resolve();

  }).then( function () {
    console.log('Pipeline created');

    // Start the Engine
    return archway.asyncStartEngine();

  }).then( function () {
    console.log('Engine started');

    // Create a function that will returns true as long as we want to continue processing
    var startDate = new Date();
    var stillRunning = function () {
      var timeElapsedSinceStart = (new Date()) - startDate;
      return timeElapsedSinceStart < 2000;
    }
    // Launch the signal processing
    return startLoop(stillRunning);

  }).then( function () {
    console.log('Processing finished');

    // Stop the Engine
    return archway.asyncStopEngine();

  }).then( function () {
    console.log('Engine stopped');

    // Stop the acquisition devices
    return archway.asyncStopAllAcquisitionDevices();

  }).then( function () {
    console.log('Acquisition devices stopped');

    // Uninitialize Archway
    return archway.asyncUninitialize();

  }).then( function () {
    console.log('Archway uninitialized');
    app.exit();

  }).catch( function (err) {
    var errorCode = parseInt(err);
    console.error("Caught Error [", errorCode, "] ", archway.getErrorString(errorCode));
  });


});
