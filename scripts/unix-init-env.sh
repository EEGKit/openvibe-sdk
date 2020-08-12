#!/bin/bash

# This script performs any environment initialisation needed for the build of OpenViBE.

function usage()
{
		echo "usage: ./unix-init-env.sh [options]"
		echo ""
		echo "-h | --help : usage"
		echo "-v | --verbose : verbose output at building step"
		echo "--dependencies-dir <dirname> : directory where dependencies are located"
}

# variable inits
dependencies_dir=""

# arg parsing
while [[ $# -gt 0 ]]; do
	key="$1"
	case $key in
		-h | --help)
			usage
			exit 0
			;;
		-v | --verbose)
			verbose=1
			;;
		--dependencies-dir)
			dependencies_dir=$2
			shift
			;;
		*)
			echo "ERROR: Unknown parameter $key"
			exit 1
			;;
	esac
	shift # past argument or value
done

if [ -z "$dependencies_dir" ]
then
  echo "Missing --dependencies-dir"
  usage
else
  echo "unix-init-env: adding dependencies to path"
  # Add to the front of PATH any needed dependency.
  export PATH=$dependencies_dir/cmake/bin:$PATH
fi
