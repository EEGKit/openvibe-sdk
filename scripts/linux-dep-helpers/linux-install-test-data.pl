#!/usr/bin/perl

# Intended to be run from linux-install_dependencies.pl which defines the dependencies_dir variable.
#
# Variables are wrt that parent scope
#

use strict;
use warnings;

my $install_dir = "$dependencies_dir/test-input";
my $url = "http://openvibe.inria.fr/dependencies/win32/3.2.0/data";

my $old_dir = Cwd::getcwd();
chdir "$dependencies_arch_dir";

my @data_archives = ("bci-motor-imagery.zip",
                    "openvibe-classification-lda.zip",
                    "openvibe-regularized-csp-2.0.0.zip");

if (! -d $install_dir) {
    mkdir($install_dir) or die("Failed to create directory [$install_dir]");
}

# Download
my $arch;
foreach $arch (@data_archives) {
    if (! -e $arch) {
        system("wget $url/$arch") == 0
            or die "Could not download archive $url/$arch - err $?";
    }
    # Extract
    system("unzip -qn $arch -d $install_dir") == 0
        or die ("Could not extract archive $arch - err $?");
}

# Go back to previous folder
chdir $old_dir;

