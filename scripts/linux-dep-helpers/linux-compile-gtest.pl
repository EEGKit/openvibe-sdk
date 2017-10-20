#!/usr/bin/perl

# Intended to be run from linux-install_dependencies.pl
# 
# Variables are wrt that parent scope
#

if (!$no_gtest) {
  my $old_dir = Cwd::getcwd();

  my $gtest_build_folder = $dependencies_dir . "/gtest-build";	
  my $gtest_lib_folder = $dependencies_dir . "/libgtest/";	
  if (! -e $dependencies_dir) {
    mkdir($dependencies_dir) or die("Failed to create directory [$dependencies_dir]");
  }
  if (! -e $gtest_build_folder) {
    mkdir($gtest_build_folder) or die("Failed to create directory [$gtest_build_folder]");
  }
  if (! -e $gtest_lib_folder) {
    mkdir($gtest_lib_folder) or die("Failed to create directory [$gtest_lib_folder]");
  }

  # build gtest
  chdir $gtest_build_folder;
  system("cmake -GNinja /usr/src/gtest");
  system("ninja all");
  system("rm CMakeCache.txt");
  my @lib_files = glob "*.a";

  foreach my $lib_cur (@lib_files) {
    copy($lib_cur, $gtest_lib_folder) or die "Could not copy lib $lib_cur $!\n";
  }
  chdir $old_dir;

}

