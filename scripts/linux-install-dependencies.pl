#!/usr/bin/perl

=pod
This script will install dependencies for CertiViBE.

The installer uses the native package manager.

Currently supported Linux Distributions are:
- Ubuntu 14.04 LTS
=cut

use strict;
use English;

# Check for the release version and set the update and install commands

my $distribution = 'Unknown';
my $update_packages_command = '';
my $package_install_command = '';

my $lsb_distributor = `lsb_release --id --short`;
my $lsb_release = `lsb_release --release --short`;

if ($lsb_distributor =~ 'Ubuntu') {
  $update_packages_command = 'sudo apt-get update';
  $package_install_command = 'sudo apt-get install';

  if ($lsb_release =~ '14.04') {
    $distribution = 'Ubuntu 14.04';
  }
}

$distribution eq 'Unknown' and die('This distribution is unsupported');

print "Installing dependencies for: $distribution\n";

# Add additional repositories for newer packages

if ($distribution eq 'Ubuntu 14.04') {
  system('sudo add-apt-repository ppa:george-edison55/cmake-3.x');
  ($CHILD_ERROR != 0) and die("Adding PPA repository for CMake failed [$CHILD_ERROR]");
}

# Create the list of packages to install

my @packages = ();

if ($distribution eq 'Ubuntu 14.04') {
  push @packages, "doxygen";
  push @packages, "make";
  push @packages, "cmake";
  push @packages, "gcc";
  push @packages, "g++";
  push @packages, "libgtk2.0-dev";
  push @packages, "libglade2-dev";
  push @packages, "libexpat1-dev";
  push @packages, "libpcre3-dev";
  push @packages, "libncurses5-dev";
  push @packages, "libeigen3-dev";
  push @packages, "libboost-dev";
  push @packages, "libboost-thread-dev";
  push @packages, "libboost-regex-dev";
  push @packages, "libboost-chrono-dev";
  push @packages, "libboost-filesystem1.54-dev";
  push @packages, "ninja-build";
  push @packages, "libzzip-dev";
  push @packages, "libfreeimage-dev";
}

# Update package list
print 'Updating package database';
system($update_packages_command);
($CHILD_ERROR != 0) and die('Failed to update the package databases');

# Install the packages
print "Will install following packages:\n";
print (join ' ', @packages), "\n";

system("$package_install_command " . (join ' ', @packages));
($CHILD_ERROR != 0) and die('Failed to install the required packages');

print("CertiViBE dependencies were successfully installed");
