Dependencies
============

Please refer to DEPENDENCIES.md file for instructions on how to build the
correct environment for the platform.

Install
=======

Linux Ubuntu (14.04)
--------------------

- Build the sofware
  - run `unix-build` script with correct parameters
- Run unit tests
  - run all unit tests by running `ninja openvibe-test` from the build directory

Windows 8
---------

- Build the sofware
  - run `windows-initialize-environment.cmd` script
  - run `windows-build.cmd` script with correct parameters
- Run unit tests
  - run all unit tests by running `ninja openvibe-test` from the build directory

Unit test
=========

Linux Ubuntu (14.04)
--------------------

- Build the sofware
  - run `unix-build` script with correct parameters
- Run unit tests
  - run all unit tests by running `sh ctest-launcher.sh -C Release|Debug -V` from the build directory

Windows 7 and later
-------------------

- Configure and generate the project with CMake
  - set `${BUILD_UNIT_TEST}` to 1 and `${BUILD_ROBOT_TEST}` to 0
  - set `${OV_ROOT_DIR}` to OpenViBE install root directory
  - set all gtest variables according to gtest install directory
  - set `${OVT_TEST_DATA_DIR}` to the path to data directory
- Build project
- Run unit tests
  - run all unit tests by running `ctest-launcher.cmd -C Release|Debug -V` from the build directory
