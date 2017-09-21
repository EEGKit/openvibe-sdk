#!/usr/bin/perl

# Intended to be run from linux-install_dependencies.pl
# 
# Variables are wrt that parent scope
#

if (!$no_install && $distribution eq 'Ubuntu 14.04') {
  system('sudo add-apt-repository ppa:fkrull/deadsnakes');
  ($CHILD_ERROR != 0) and die("Adding PPA repository for Python 3.5 failed [$CHILD_ERROR]");
}

