export OV_PATH_ROOT="${PWD}/.."
export LD_LIBRARY_PATH="${OV_PATH_ROOT}/lib"
export DYLD_LIBRARY_PATH="${OV_PATH_ROOT}/lib"
mkdir -p sphinx/templates
"${OV_PATH_ROOT}/bin/openvibe-plugin-inspector" --box-doc-directory sphinx/templates
mv sphinx/templates/index-boxes.rst sphinx/source/boxes

pushd sphinx
make html
popd
rm -fr "../doc/NeuroRT Manual"
mkdir -p ../doc
mv sphinx/build/html "../doc/OpenViBE Manual"

