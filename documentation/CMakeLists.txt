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
install(DIRECTORY DESTINATION "${DIST_DOCTMP}/sphinx")
install(DIRECTORY DESTINATION "${DIST_DOCTMP}/sphinx/templates")

# Collect all sphinx documentation files from all projects

file(GLOB SPHINX_DOC_DIRS LIST_DIRECTORIES true LIST_FILES false "${PROJECT_SOURCE_DIR}/plugins/processing/*")
foreach(DOCDIR ${SPHINX_DOC_DIRS})
	if(IS_DIRECTORY "${DOCDIR}/doc/boxes")
		install(DIRECTORY "${DOCDIR}/doc/boxes" DESTINATION ${DIST_DOCTMP}/sphinx/source)
	endif()
endforeach()

install(DIRECTORY sphinx/source DESTINATION ${DIST_DOCTMP}/sphinx)
install(FILES sphinx/make.bat DESTINATION ${DIST_DOCTMP}/sphinx)
install(FILES sphinx/Makefile DESTINATION ${DIST_DOCTMP}/sphinx)
install(FILES sphinx/build-documentation.cmd DESTINATION ${DIST_DOCTMP})
install(FILES sphinx/build-documentation.sh DESTINATION ${DIST_DOCTMP})

