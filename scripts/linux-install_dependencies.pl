#!/usr/bin/perl

=pod
This script will install dependencies for OpenViBE SDK.

The installer uses the native package manager.

Currently supported Linux Distributions are:
- Ubuntu 14.04 LTS
- Ubuntu 16.04 LTS
  
Operating on the specified manifest folder,

  1) Carries out preliminary steps by running scripts matching 
     ^linux-preinstall-.*pl from linux-dep-helpers/
  2) Installs packages from distro-specific files 
     (like linux-dependencies-ubuntu1404.txt)
  3) May compile/fetch packages with perl scripts matching 
     ^linux-compile-.*pl from linux-dep-helpers/

n.b. The script globs all .pl scripts present that match the prefixes. If you don't want some dependency, remove the corresponding script.

=cut

use strict;
use English;
use FindBin;
use File::Copy;
use File::Spec;
use Getopt::Long;

sub usage {
  print "$0 [-h][-y]\n";
  print "Install OpenViBE SDK build dependencies\n";
  print "    Options:\n";
  print "      -h: this help\n";
  print "      -y: assume 'yes' to all prompts. Make it possible to run non-interactively.\n";
  print "      --no-gtest: will not compile gtest if set.\n";
  print "      --no-install: will not do any installation if set.\n";
  print "      --dependencies-dir [folder]: path to the dependencies\n";
  print "      --manifest-dir     [folder]: path to the manifest files (default: scripts folder/linux-dep-helpers/)\n";
};


my $assume_yes = 0;
my $no_gtest = 0;
my $no_install = 0;
my $manifest_dir = "$FindBin::Bin";
my $print_help = 0;
my $dependencies_dir = "$FindBin::Bin/../dependencies/";

GetOptions (
            "assume-yes"         => \$assume_yes,
            "y"                  => \$assume_yes,
            "dependencies-dir=s" => \$dependencies_dir,
            "manifest-dir=s"     => \$manifest_dir,
            "h"                  => \$print_help,
            "help"               => \$print_help,
            "no-install"         => \$no_install, 
            "no-gtest"           => \$no_gtest)  
      or usage() and die("Error in command line arguments\n");

if($print_help) {
	usage();
    exit(1);
}

$dependencies_dir = File::Spec->rel2abs($dependencies_dir);
my $dependencies_arch_dir = "$dependencies_dir/arch";
my $helper_script_dir = "$manifest_dir/linux-dep-helpers/";

if (! -e $dependencies_dir) {
    mkdir($dependencies_dir) or die("Failed to create directory [$dependencies_dir]");
}
if (! -e $dependencies_arch_dir) {
    mkdir($dependencies_arch_dir) or die("Failed to create directory [$dependencies_arch_dir]");
}

# Check for the release version and set the update and install commands
my $unsupported_distribution = 'Unsupported';
my $distribution = $unsupported_distribution;
my $update_packages_command = '';
my $package_install_command = '';
my $add_repository_command  = '';

my $os_release = `cat /etc/os-release`;
# Check for NAME= and VERSION_ID=
# Take into account distros placing quotes around value
my ($lsb_distributor, $lsb_release) = ($os_release =~ m/NAME=["'\`]*([a-zA-Z]+).*VERSION_ID=["'\`]*([0-9.]+)/s);

if ($lsb_distributor =~ 'Ubuntu') {
  if (!$no_install) {
    $update_packages_command = 'sudo apt-get update';
    if ($assume_yes) {
      $package_install_command = 'sudo apt-get -y install';
      $add_repository_command  = 'sudo add-apt-repository -y universe';
    } else {
      $package_install_command = 'sudo apt-get install';
      $add_repository_command  = 'sudo add-apt-repository universe';
    }
  }
  $distribution = 'Ubuntu ' . $lsb_release;

} elsif ($lsb_distributor =~ 'Fedora') {
  if (!$no_install) {
    if ($assume_yes) {
      $package_install_command = 'sudo dnf -y install';
    } else {
      $package_install_command = 'sudo dnf install';
    }
  }
  $distribution = 'Fedora' . $lsb_release;
}

$distribution eq $unsupported_distribution and die('This distribution is unsupported');

print "Installing dependencies for: $distribution\n";
print "Install command: $package_install_command\n";
# Perform steps before installing packages
opendir(my $dir_handle, $helper_script_dir) or die("unable to open $helper_script_dir");
while(my $filename = readdir($dir_handle)) {
  if($filename =~ /^linux-preinstall.*pl/) {
	open(my $pl_file_handle, '<', "$helper_script_dir/$filename") 
    		or die "Unable to open file, $helper_script_dir/$filename";
	undef $/;
	my $program = <$pl_file_handle>;
	eval " $program "; warn $@ if $@;
  }
}
closedir($dir_handle);

# Create the list of packages to install

my $pkg_file = "";

if ($distribution =~ 'Ubuntu') {
  if (int(lsb_release) <= '14') {
    $pkg_file = "$manifest_dir/linux-dependencies-ubuntu1404.txt";
  } else {
    $pkg_file = "$manifest_dir/linux-dependencies-ubuntu16_plus.txt";
  }
} elsif ($distribution eq 'Fedora') {
  $pkg_file = "$manifest_dir/linux-dependencies-fedora.txt";
}

# Install actual packages
print "Opening package manifest file $pkg_file ...\n";

# read package list to memory
open(my $pkg_file_handle, '<:encoding(UTF-8)', $pkg_file) 
    or die "Unable to open file, $pkg_file";

my @packages = ();
local $INPUT_RECORD_SEPARATOR = "\n";
for my $line (<$pkg_file_handle>) {
    $line =~ s/\r?\n$//;
    push @packages, $line; 
}

close($pkg_file_handle) or warn "Unable to close $pkg_file";

if (!$no_install) {
  # Update package list
  if ($add_repository_command) {
    print "Updating package database...\n";
    system($add_repository_command);
    ($CHILD_ERROR != 0) and die('Failed to add additional repositories');
  }
  if ($update_packages_command) {
    system($update_packages_command);
    ($CHILD_ERROR != 0) and die('Failed to update the package databases');
  }

  # Install the packages
  print "Will install following packages:\n";
  print (join ' ', @packages), "\n";

  my $pkgs = (join ' ', @packages);
  system("$package_install_command " . (join ' ', @packages));
  ($CHILD_ERROR != 0) and die('Failed to install the required packages');
}

# Obtain specific dependencies that we dont get from packages
opendir(my $dir_handle, $helper_script_dir) or die("unable to open $helper_script_dir");
while(my $filename = readdir($dir_handle)) {
  if($filename =~ /^linux-compile.*pl/ || $filename =~ /^linux-install.*pl/) {
	print "Running $helper_script_dir/$filename ...\n";
	open(my $pl_file_handle, '<', "$helper_script_dir/$filename") 
    		or die "Unable to open file, $helper_script_dir/$filename";
	undef $/;
	my $program = <$pl_file_handle>;
	eval " $program "; warn $@ if $@;
  }
}
closedir($dir_handle);

print("OpenViBE SDK dependencies were successfully installed\n");

