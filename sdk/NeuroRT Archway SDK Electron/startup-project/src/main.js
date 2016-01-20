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
var BrowserWindow = require('browser-window');

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the javascript object is GCed.
var mainWindow = null;

// Quit when all windows are closed.
app.on('window-all-closed', function() {
    app.quit();
});

// This method will be called when atom-shell has done everything
// initialization and ready for creating browser windows.
app.on('ready', function() {

  // Log the version of Node.js being used to run the application
  console.log(process.version);

  // Create a new browser window
  mainWindow = new BrowserWindow({ width: 1024, height: 768 });

  // Load the index.html file as the entry point to the application in the created window
  mainWindow.loadUrl('file://' + __dirname + '/index.html');

  // Open the developer tools console, this should not be used in production
  mainWindow.openDevTools();

  // Add a handler for closing the widnow
  mainWindow.on('closed', function() {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    mainWindow = null;
  });

  // We set the title to the application name and version
  // These are defined in the package.json file
  mainWindow.setTitle(app.getName() + " " + app.getVersion());

  // We prevent changing the window title
  // Otherwise the title would change if the page in the web application changes
  mainWindow.on('page-title-updated', function(event) {
    event.preventDefault();
  });

});

