#!/usr/bin/perl

# Intended to be run from linux-install_dependencies.pl
# 
# Variables are wrt that parent scope
#

# Installation of packages not available in the apt database or PPA
# Eigen installation
if (!$no_install && $distribution eq 'Ubuntu 16.04') {
  # Install eigen 3.2.9 from source if it is not already
  # This is because Ubuntu 16.04 has a beta version of the Eigen package by default
  # and it does not work
  if (-e "/usr/local/include/eigen3") {
    print STDERR "Warning: eigen3 is already installed in /usr/local\n";
  } else {
    my $old_dir = Cwd::getcwd();

    my $eigen_build_folder = $dependencies_dir . "/eigen-build";
    my $eigen_extracted_folder = $eigen_build_folder . "/eigen-eigen-dc6cfdf9bcec";

    if (! -e $dependencies_dir) {
      mkdir($dependencies_dir) or die("Failed to create directory [$dependencies_dir]");
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
    chdir $old_dir;
  }
}
