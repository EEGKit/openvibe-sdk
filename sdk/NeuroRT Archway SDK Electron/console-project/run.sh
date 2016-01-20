#!/bin/bash
CWD=$(pwd)

if [[ -d "$CWD/../../../dist-Release" ]]; then
  export OV_PATH_ROOT="$CWD/../../../dist-Release"
else
  export OV_PATH_ROOT="$CWD/../../../dist"
fi

export OV_PATH_BIN="$OV_PATH_ROOT/bin"
export OV_PATH_LIB="$OV_PATH_ROOT/lib"
export OV_PATH_DATA="$OV_PATH_ROOT/share/openvibe"
export MENSIA_PATH_DATA="$OV_PATH_ROOT/share/mensia"
export PATH="$OV_PATH_BIN:$PATH"

platform=$(uname)
if [[ $platform == "Darwin" ]]; then
  export DYLD_LIBRARY_PATH="$OV_PATH_ROOT/lib"
elif [[ $platform == "Linux" ]]; then
  export LD_LIBRARY_PATH="$OV_PATH_ROOT/lib"
fi

pushd src
electron .
popd
