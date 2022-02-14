# Install

## Dependencies

### Linux Ubuntu (14.04 and 16.04)

- Install the dependencies
  - run `linux-install-dependencies.pl` script; you will require sudo rights
  - run `unix-get-dependencies.sh` script with an existing `DEPENDENCY_CACHE` environment variable or with --cache parameter (OPTIONAL - for tests), or with `PROXYPASS`

### Windows 7 and later

- Install the dependencies:
  - copy `windows-install-dependencies.cmd` and rename copy to `windows-install-dependencies-custom.cmd`
  - edit line `set PROXYPASS=XXX:XXX` with appropriate username and password
  - run it whenever you want to update dependencies.

## Install

### Linux Ubuntu (14.04)

- Run unit tests
  - run `ctest-launcher.sh` from the build directory

### Windows 7 and later

- Run unit tests
  - run `ctest-launcher.cmd` from the build directory
