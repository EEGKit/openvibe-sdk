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

This ReadMe file contains informations on how to use this Archway Electron SDK to build an application powered by the Mensia Archway.
You'll find the complete documentation for these example applications and the API under "<InstallationDirectory>/doc/NeuroRT Archway SDK Electron".

# The Electron SDK for Archway

## Requirement

In order to be able to use the example application some setup is
necessary.

The `archway-node` library has to be put in an `SDK` folder next to this
README file. The structure should be as follows:

- SDK
  - archway-node
    - archway-ARCH.node
    - loader.js
    - package.json

The environment has to be set properly before being able to use the
local instance of the Engine.

## Running of the test program

There are two example programs, called "console-project" and "startup-project", the first one is very basic and only makes simples calls, the second one gives a more detailed GUI. 
These test programs can be launched by using the supplied `run-console-project.cmd` and `run-startup-project.cmd` scripts.
