#!/usr/bin/perl

# Intended to be run from linux-install_dependencies.pl
# 
# Variables are wrt that parent scope
#
if (!$no_install && $distribution eq 'Ubuntu 14.04') {
  # Install cmake 3.5.1 from source if it is not already
  # This is because Ubuntu 14.04 only has an old version of cmake
  if (-e "/usr/local/bin/cmake") {
    print STDERR "Warning: cmake is already installed in /usr/local\n";
  } else {
    my $old_dir = Cwd::getcwd();

    my $dependencies_dir = $FindBin::Bin . "/../dependencies";
    my $cmake_build_folder = $dependencies_dir . "/cmake-build";
    my $cmake_extracted_folder = $cmake_build_folder . "/cmake-3.5.1";

    if (! -e $dependencies_dir) {
      mkdir($dependencies_dir) or die("Failed to create directory [$dependencies_dir]");
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
    chdir $old_dir;
  }
}

