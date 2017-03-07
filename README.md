Introduction
============

Certivibe is a core software platform for the design, test and use of
Brain-Computer Interfaces. It is a fork of OpenVIBE that aims to be used in
medical devices.

Repository organisation
=======================

The project repository is composed of several software modules.
The repository basically looks like this :

```
 + <openvibe> (API / specifications)
 + <kernel> (kernel implementation)
 + <toolkit> (development help components)
 + <applications> (OpenViBE user applications)
 + <plugins> (OpenViBE plugin collections)
    + <samples>
    + <acquisition>
    + ...
 + <modules> (abstraction and portability components)
    + <ebml>
    + <socket>
    + ...
	+-- cmake-modules (cmake FindXXX and helpers)
 + <data>
    + <openvibe> (configuration data)
    + <resources> (test resources)
 + <scripts>
 + <tools> (tools used by unit and validation tests)
    + <unit-toolkit> (internal unit testing framework)
    + <accuracy>
    + <python-toolkit>
        + <robot_lib> (Robot Framework keywords in python)
    + <robot-toolkit>
        + <keyword> (Robot Framework keywords in txt)
 + <unit-test> (code-level unit tests)
 + <validation-test> (integration/validation tests)
```

Each software module is organized as a UNIX-like tree (empty folders not
included):

```
+ <bin> (any pre-compiled binaries)
+ <include> (target folder for API headers)
+ <lib> (any pre-compiled libraries)
+ <share> (shared file folder for all data)
+ <src> (source code of the module)
+ <doc> (documentation files for the module)
+ <test> (unit test source code for the module)
```

Copying
=======

Please refer to the COPYING.md file to get details regarding the OpenViBE license.

Install
=======

Please refer to the INSTALL.md file for instructions on how to build the
platform.


Naming rules
============

WARNING: These are the initial naming rules of the project. For an
updated version, pleaser refer to Certivibe Coding Rules.

#### CMake

 - Openvibe-related variable: OV_MY_VARIABLE
 - Test-related variable: OVT_MY_VARIABLE

#### Tools

 - CMake project and target name: openvibe-test-*mytool* (e.g. *openvibe-test-accuracy*)
 - File name: ovt + OpenViBE naming rules
 - Main namespace: OpenViBETest
 - Macro naming: OVT_MYMACRO

#### Unit Tests

 - OpenViBE tested modules: module (e.g. *openvibe-kernel*)
 - Unit test directory name: module (e.g. *openvibe-kernel*)
 - CMake target name: module-test (e.g. *openvibe-kernel-test*)
 - Test file name: uo/ur + test identifier + Test (e.g. *uoKernelContextTest.cpp*)
	 - u = unit
	 - o = open (used for public test transferred from open-source OpenViBE)
	 - r = restricted (used for test implemented within CertiViBE project)
 - Test name: identical to file name

#### Validation Tests

 - OpenViBE tested module: module (e.g. *openvibe-plugins-classification*)
 - Validation test directory: module (e.g. *openvibe-plugins-classification*)
 - CMake target name: module-test (e.g *openvibe-plugins-classification-test*)
 - Test name: vo/vr + module identifier + test identifier + Test (e.g. *voClassificationLDATest*)
	 - v = validation
	 - o/r: see Unit Tests

#### Robot Framework Test
 - OpenViBE tested module: module (e.g. *channel-rename*)
 - Validation test directory: module (e.g. *channel-rename*)
 - Robot Framework test name: Define in the Robot Framework script (e.g. *Channel-rename-without-space*)

#### Headers

Use AGPL-3 header for public tests and  confidential header for private tests.

Contribution
============

### Adding Unit Test

#### Add Tests to Existing Driver

To add tests to a module that is already tested, just add the test file and modify the test driver CMakeLists.txt accordingly.

#### Creating a Test Driver

If you wish to test a new module, you will have to create a new test driver in unit-test/.

Create a new directory with some tests and a CMakeLists.txt (just look at other tests to get insights on tests structure).

Add driver directory to directory to be processed in root directory CMakeLists.txt.

### Updating CTest Validation Test

#### Enhance Existing Test

Just add data in the test directory and modify test CMakeLists.txt accordingly.

#### Add New Test

There should be no reason to add ctest validation tests. New validation tests use Robot Framework.
Thus check next section for more details.

### Adding Robot Framework Test

#### Enhance Existing Test

Just add data in the test directory and modify test *.robot accordingly

#### Add New Test

Create a new test directory with some data, a robot script and a all necessary keywords (just look at other tests to get details on tests structure).

