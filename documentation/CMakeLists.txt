find_package(Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" OFF)

if(BUILD_DOCUMENTATION)
    if(NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation.")
    endif()

	file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/html")

    set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/src/Doxyfile.in)
    set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    configure_file(${doxyfile_in} ${doxyfile} @ONLY)

    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM)

    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION ${DIST_DOCDIR})
    install(DIRECTORY doc/ DESTINATION ${DIST_DOCDIR}/html)

endif()

# Sphinx Documentation

FILE(MAKE_DIRECTORY "${DIST_DOCTMP}/sphinx")
FILE(MAKE_DIRECTORY "${DIST_DOCTMP}/sphinx/templates")

# Collect all sphinx documentation files from all projects

file(GLOB SPHINX_DOC_DIRS LIST_DIRECTORIES true LIST_FILES false "${PROJECT_SOURCE_DIR}/plugins/processing/*")
foreach(DOCDIR ${SPHINX_DOC_DIRS})
	if(IS_DIRECTORY "${DOCDIR}/doc/boxes")
		INSTALL(DIRECTORY "${DOCDIR}/doc/boxes" DESTINATION ${DIST_DOCTMP}/sphinx/source)
	endif()
endforeach()

INSTALL(DIRECTORY sphinx/source DESTINATION ${DIST_DOCTMP}/sphinx)
INSTALL(FILES sphinx/make.bat DESTINATION ${DIST_DOCTMP}/sphinx)
INSTALL(FILES sphinx/Makefile DESTINATION ${DIST_DOCTMP}/sphinx)

FILE(WRITE "${DIST_DOCTMP}/build-documentation.sh" "\
export OV_PATH_ROOT=\"\${PWD}/..\"\n\
export LD_LIBRARY_PATH=\"\${OV_PATH_ROOT}/lib\"\n\
export DYLD_LIBRARY_PATH=\"\${OV_PATH_ROOT}/lib\"\n\
mkdir -p sphinx/templates
\"\${OV_PATH_ROOT}/bin/openvibe-plugin-inspector\" --box-doc-directory sphinx/templates\n\
mv sphinx/templates/index-boxes.rst sphinx/source/boxes\n\
\n\
pushd sphinx\n\
make html\n\
popd\n\
rm -fr \"../doc/NeuroRT Manual\"\n\
mkdir -p ../doc\n\
mv sphinx/build/html \"../doc/NeuroRT Manual\"\n\
\n")


FILE(WRITE "${DIST_DOCTMP}/build-documentation.cmd" "\
setlocal\n\
setlocal enableextensions\n\
\n\
set \"OV_PATH_ROOT=%~dp0\..\"\n\
set \"PATH=%OV_PATH_ROOT%/bin;%PATH%\"\n\
set \"TEMPLATE_DIR=sphinx\templates\"\n\
if not exist %TEMPLATE_DIR% (md %TEMPLATE_DIR%)\n\
call openvibe-plugin-inspector --box-doc-directory sphinx/templates\n\
move sphinx\\templates\\index-boxes.rst sphinx\\source\\boxes\n\
pushd sphinx\n\
call make.bat html\n\
popd\n\
endlocal\n\
mkdir ..\\doc\n\
move sphinx\\build\\html \"..\\doc\\NeuroRT Manual\"\n\
\n")
