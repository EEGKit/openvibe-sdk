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
