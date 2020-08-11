#!/usr/bin/perl

# Intended to be run from linux-install_dependencies.pl which defines the dependencies_dir variable.
# 
# Variables are wrt that parent scope
#

use strict;
use warnings;
use Getopt::Long;

use English;
use FindBin;
use File::Copy;
use File::Spec;

# Version of CMake to install if needed (latest as of 08/2020)
my $version_major = 3;
my $version_minor = 18;
my $version_patch = 1;

# Minimum version needed for the project.
# 3.12 for FindPython functionalities.
my $minimum_major = 3;
my $minimum_minor = 15;

my $install_dir = $dependencies_dir;

# Updating path with potentially installed cmake before looking for it.
if ("$dependencies_dir/cmake") {
  print "Found cmake folder in deps\n";
  my $path = $ENV{'PATH'};
  $path = "$dependencies_dir/cmake/bin:$path";
  $ENV{'PATH'} = $path;
  #system("PATH=$dependencies_dir/cmake/bin:$PATH")
}

# Checking for CMake presence
my $major_found = 0;
my $minor_found = 0;

if (`which cmake`) {
  `cmake --version` =~ /.*(\d)\.(\d\d)\.*/;

  print "cmake version found: $1.$2\n";

  $major_found = $1;
  $minor_found = $2;
}

# Minimum cmake version required for the project: 3.12
if (int($major_found) < $minimum_major || int($minor_found) < $minimum_minor) {

  my $old_dir = Cwd::getcwd();

  my $cmake_archive = "cmake-${version_major}.${version_minor}.${version_patch}.tar.gz";
  my $cmake_folder = "cmake-${version_major}.${version_minor}.${version_patch}";

  print "CMake version too low. Installing newer one in $install_dir\n";

  if (! -d $install_dir) {
    mkdir($install_dir) or die("Failed to create directory [$install_dir]");
  }

  # Download
  print "Downloading ${cmake_archive}\n";
  system("wget http://www.cmake.org/files/v${version_major}.${version_minor}/${cmake_archive}") == 0
    or die "Could not download the CMake sources - err $?";

  # Extract
  system("tar -xzf ${cmake_archive}") == 0
    or die ("Could not extract the CMake archive - err $?");

  # Configure, compile and install
  chdir $cmake_folder;
  system("./configure --prefix=$install_dir/cmake") == 0
    or die("Failed to configure for cmake - err $?");
  system("make install") == 0
    or die ("Failed make install cmake - err $?");

  # Go back to previous folder
  chdir $old_dir;

  #Clear
  system("rm -r ${cmake_archive} ${cmake_folder}") == 0
    or die ("Failed to clear after install - err $?");

}

