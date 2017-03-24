#!/usr/bin/perl

=pod
This script will install dependencies for CertiViBE.

The installer uses the native package manager.

Currently supported Linux Distributions are:
- Ubuntu 14.04 LTS
- Ubuntu 16.04 LTS
=cut

use strict;
use English;
use FindBin;

sub usage {
  print "$0 [-h][-y]\n";
  print "Install CertiViBE build dependencies\n";
  print "    Options:\n";
  print "      -h: this help\n";
  print "      -y: assume 'yes' to all prompts. Make it possible to run non-interactively.\n";
};


if ($#ARGV > 0) {
  usage();
  exit(1);
}

my $assume_yes = 0;

if ($#ARGV == 0) {
  if ($ARGV[0] eq "-h") {
    usage();
    exit(0);
  } elsif ($ARGV[0] eq "-y") {
     $assume_yes = 1;
  } else {
    usage();
    exit(1);
  }
}

# Check for the release version and set the update and install commands

my $distribution = 'Unknown';
my $update_packages_command = '';
my $package_install_command = '';
my $add_repository_command  = '';

my $lsb_distributor = `lsb_release --id --short`;
my $lsb_release = `lsb_release --release --short`;

if ($lsb_distributor =~ 'Ubuntu') {
  $update_packages_command = 'sudo apt-get update';
  if ($assume_yes) {
    $package_install_command = 'sudo apt-get -y install';
    $add_repository_command  = 'sudo add-apt-repository -y universe';
  } else {
    $package_install_command = 'sudo apt-get install';
    $add_repository_command  = 'sudo add-apt-repository universe';
  }
  if ($lsb_release =~ '14.04') {
    $distribution = 'Ubuntu 14.04';
  } elsif ($lsb_release =~ '16.04') {
    $distribution = 'Ubuntu 16.04';
  }
}

$distribution eq 'Unknown' and die('This distribution is unsupported');

print "Installing dependencies for: $distribution\n";

# Add additional repositories for newer packages

if ($distribution eq 'Ubuntu 14.04') {
  system('sudo add-apt-repository ppa:fkrull/deadsnakes');
  ($CHILD_ERROR != 0) and die("Adding PPA repository for Python 3.5 failed [$CHILD_ERROR]");
}

# Create the list of packages to install

my @packages = ();

if ($distribution eq 'Ubuntu 14.04') {
  push @packages, "doxygen";
  push @packages, "make";
  push @packages, "gcc";
  push @packages, "g++";
  push @packages, "libexpat1-dev";
  push @packages, "libncurses5-dev";
  push @packages, "libeigen3-dev";
  push @packages, "libboost-dev";
  push @packages, "libboost-thread-dev";
  push @packages, "libboost-regex-dev";
  push @packages, "libboost-chrono-dev";
  push @packages, "libboost-filesystem1.54-dev";
  push @packages, "ninja-build";
  push @packages, "libzzip-dev";
  push @packages, "libxerces-c-dev";
  push @packages, "libgtest-dev";
} elsif ($distribution eq 'Ubuntu 16.04') {
  # common packages with certivibe
  push @packages, "doxygen";
  push @packages, "make";
  push @packages, "cmake";
  push @packages, "gcc";
  push @packages, "g++";
  push @packages, "libexpat1-dev";
  push @packages, "libncurses5-dev";
  push @packages, "libboost-dev";
  push @packages, "libboost-thread-dev";
  push @packages, "libboost-regex-dev";
  push @packages, "libboost-chrono-dev";
  push @packages, "libboost-filesystem1.58-dev";
  push @packages, "ninja-build";
  push @packages, "libzzip-dev";
  push @packages, "libxerces-c-dev";
  push @packages, "libgtest-dev";
}

# Update package list
print "Updating package database...\n";
system($add_repository_command);
($CHILD_ERROR != 0) and die('Failed to add additional repositories');
system($update_packages_command);
($CHILD_ERROR != 0) and die('Failed to update the package databases');

# Install the packages
print "Will install following packages:\n";
print (join ' ', @packages), "\n";

system("$package_install_command " . (join ' ', @packages));
($CHILD_ERROR != 0) and die('Failed to install the required packages');

# Installation of packages not available in the apt database or PPA
# Eigen installation
if ($distribution eq 'Ubuntu 16.04') {
  # Install eigen 3.2.9 from source if it is not already
  # This is because Ubuntu 16.04 has a beta version of the Eigen package by default
  # and it does not work
  if (-e "/usr/local/include/eigen3") {
    print STDERR "Warning: eigen3 is already installed in /usr/local\n";
  } else {
    my $dependencies_folder = $FindBin::Bin . "/../dependencies";
    my $eigen_build_folder = $dependencies_folder . "/eigen-build";
    my $eigen_extracted_folder = $eigen_build_folder . "/eigen-eigen-dc6cfdf9bcec";

    if (! -e $dependencies_folder) {
      mkdir($dependencies_folder) or die("Failed to create directory [$dependencies_folder]");
    }
    if (! -e $eigen_build_folder) {
      mkdir($eigen_build_folder) or die("Failed to create directory [$eigen_build_folder]");
    }

    chdir $eigen_build_folder;

    if (! -e "3.2.9.tar.bz2") {
      system('wget "http://bitbucket.org/eigen/eigen/get/3.2.9.tar.bz2"');
      ($CHILD_ERROR != 0) and die ("Could not download the Eigen sources [$CHILD_ERROR]");
    }
    if (! -e $eigen_extracted_folder) {
      system('tar -xjf "3.2.9.tar.bz2"');
      ($CHILD_ERROR != 0) and die ("Could not extract the eigen archive");
    }

    chdir $eigen_extracted_folder;
    mkdir $eigen_extracted_folder . "/build" or die ("Failed to create directory [$eigen_extracted_folder/build]");
    chdir $eigen_extracted_folder . "/build";

    system("cmake ..");
    ($CHILD_ERROR != 0) and die("Failed to run CMake for Eigen [$CHILD_ERROR]");

    system("sudo make install");
    ($CHILD_ERROR != 0) and die("Failed install Eigen [$CHILD_ERROR]");

    # Go back to the scripts folder
    chdir $FindBin::Bin;
  }
}
if ($distribution eq 'Ubuntu 14.04') {
  # Install cmake 3.5rm .1 from source if it is not already
  # This is because Ubuntu 14.04 only has an old version of cmake
  if (-e "/usr/local/bin/cmake") {
    print STDERR "Warning: cmake is already installed in /usr/local\n";
  } else {
    my $dependencies_folder = $FindBin::Bin . "/../dependencies";
    my $cmake_build_folder = $dependencies_folder . "/cmake-build";
    my $cmake_extracted_folder = $cmake_build_folder . "/cmake-3.5.1";

    if (! -e $dependencies_folder) {
      mkdir($dependencies_folder) or die("Failed to create directory [$dependencies_folder]");
    }
    if (! -e $cmake_build_folder) {
      mkdir($cmake_build_folder) or die("Failed to create directory [$cmake_build_folder]");
    }

    chdir $cmake_build_folder;

    if (! -e "cmake-3.5.1.tar.gz") {
      system('wget "http://www.cmake.org/files/v3.5/cmake-3.5.1.tar.gz"');
      ($CHILD_ERROR != 0) and die ("Could not download the CMake sources [$CHILD_ERROR]");
    }
    if (! -e $cmake_extracted_folder) {
      system('tar -xzf "cmake-3.5.1.tar.gz"');
      ($CHILD_ERROR != 0) and die ("Could not extract the CMake archive");
    }
    chdir $cmake_extracted_folder;
    system("./configure");
    ($CHILD_ERROR != 0) and die("Failed to configure for cmake [$CHILD_ERROR]");

    system("sudo make install");
    ($CHILD_ERROR != 0) and die("Failed make install cmake [$CHILD_ERROR]");
	
    # Go back to the scripts folder
    chdir $FindBin::Bin;
  }
}
if (1) {
	my $dependencies_folder = $FindBin::Bin . "/../dependencies";
    my $gtest_build_folder = $dependencies_folder . "/gtest-build";	
	if (! -e $dependencies_folder) {
      mkdir($dependencies_folder) or die("Failed to create directory [$dependencies_folder]");
    }
    if (! -e $gtest_build_folder) {
      mkdir($gtest_build_folder) or die("Failed to create directory [$gtest_build_folder]");
    }

	# build gtest
	pushd $gtest_build_folder
	cmake -GNinja /usr/src/gtest
	ninja all
	rm CMakeCache.txt
	popd
}
print("CertiViBE dependencies were successfully installed\n");
