Dependencies
============

Linux Ubuntu (14.04 and 16.04) 
-----------------------------
- Install the dependencies
  - run `linux-install-dependencies.pl` script; you will require sudo rights
  - run `unix-get-dependencies.sh` script with an existing CV_DEPENDENCIES_CACHE environment variable or with --cache parameter (OPTIONNAL - for tests)


Windows 7 and later
-------------------
- Install the dependencies
  - run `windows-initialize-environment.cmd` script
  - run `windows-install-dependencies-all.cmd` script


Install
=======

Linux Ubuntu (14.04)
--------------------
- Build the sofware
  - run `unix-build` script, and append `--build-unit` and/or `--build-validation` if you want to build tests
- Run unit tests
  - run `ctest-launcher.sh` from the build directory


Windows 7 and later
-------------------
- Build the sofware
  - run `windows-build.cmd` script, and append `--build-unit` and/or `--build-validation` if you want to build tests
- Run unit tests
  - run `ctest-launcher.cmd` from the build directory

