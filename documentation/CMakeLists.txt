PROJECT(openvibe-documentation)
IF(NOT(${SKIP_DOCUMENTATION}))

	IF(WIN32)
		FIND_PROGRAM(DOXYGEN_BIN "doxygen" PATHS "C:/Program Files/doxygen/bin" "C:/Program Files (x86)/doxygen/bin")
		IF(DOXYGEN_BIN)
			GET_FILENAME_COMPONENT(DOXYGEN_PATH ${DOXYGEN_BIN} PATH)
			STRING(REPLACE "/" "\\" DOXYGEN_PATH ${DOXYGEN_PATH})
			MESSAGE(STATUS "Found Doxygen at ${DOXYGEN_PATH}")
		ELSE()
			MESSAGE(STATUS "FAILED to find Doxygen, no documentation can be produced !")
		ENDIF()
		
		IF(DOXYGEN_BIN)
			MESSAGE(STATUS "  Looking for additional documentation supports...")
			FIND_PROGRAM(HHC_BIN "hhc" PATHS "C:/Program Files/HTML Help Workshop" "C:/Program Files (x86)/HTML Help Workshop" NO_DEFAULT_PATH)
			IF(HHC_BIN)
				GET_FILENAME_COMPONENT(HHC_PATH ${HHC_BIN} PATH)
				STRING(REPLACE "/" "\\" HHC_PATH ${HHC_PATH})
				MESSAGE(STATUS "Found HHC at ${HHC_PATH}, Windows help (.chm) file will be produced.")
			ELSE(HHC_BIN)
				MESSAGE(STATUS "FAILED to find HHC, Windows help (.chm) file will not be produced. (optional).")
			ENDIF(HHC_BIN)
			
			FIND_PROGRAM(LATEX_BIN "latex" PATHS "C:/Program Files (x86)/*/miktex/bin" "C:/Program Files/*/miktex/bin" NO_DEFAULT_PATH)
			IF(LATEX_BIN)
				GET_FILENAME_COMPONENT(MIKTEX_PATH ${LATEX_BIN} PATH)
				STRING(REPLACE "/" "\\" MIKTEX_PATH ${MIKTEX_PATH})
				MESSAGE(STATUS "Found MiKTeX (latex) at ${MIKTEX_PATH} for PDF generation.")
			ELSE()
				MESSAGE(STATUS "FAILED to find MiKTeX, no pdf file can be produced (optional).")
			ENDIF()

			FIND_PROGRAM(GHOSTSCRIPT_BIN "gswin32c" PATHS "C:/Program Files (x86)/gs/*/bin" "C:/Program Files/gs/*/bin" NO_DEFAULT_PATH)
			FIND_PROGRAM(GHOSTSCRIPT_BIN "gswin64c" PATHS "C:/Program Files (x86)/gs/*/bin" "C:/Program Files/gs/*/bin" NO_DEFAULT_PATH)
			IF(GHOSTSCRIPT_BIN)
				GET_FILENAME_COMPONENT(GHOSTSCRIPT_PATH ${GHOSTSCRIPT_BIN} PATH)
				STRING(REPLACE "/" "\\" GHOSTSCRIPT_PATH ${GHOSTSCRIPT_PATH})
				MESSAGE(STATUS "Found Ghost Script at ${GHOSTSCRIPT_PATH} for PDF generation.")
			ELSE()
				MESSAGE(STATUS "FAILED to find Ghost Script, no pdf file can be produced (optional).")
			ENDIF()
		ENDIF()
	ELSE()
		MESSAGE(STATUS "Building documentation is only supported on Windows")
	ENDIF()

	IF(DOXYGEN_BIN)
		FILE(WRITE "${DOCUMENTATION_TEMP_DIRECTORY}/build-documentation.cmd"
			"SET PATH=${OV_CUSTOM_DEPENDENCIES_PATH}\\cmake\\bin;${MIKTEX_PATH};${GHOSTSCRIPT_PATH};%PATH%\n")

		IF(NOT(${SKIP_DOC_OPENVIBE}))
			MESSAGE(STATUS "Now building ${PROJECT_NAME} ${PROJECT_VERSION} (${PROJECT_BRANCH}/${PROJECT_COMMITHASH})" )
			SET(PROJECT_PRODUCT_NAME "OpenViBE")

			FILE(MAKE_DIRECTORY "${DOCUMENTATION_TEMP_DIRECTORY}/openvibe")
			FILE(MAKE_DIRECTORY "${DOCUMENTATION_TEMP_DIRECTORY}/openvibe/box-algorithm-doc/dox")
			FILE(MAKE_DIRECTORY "${DOCUMENTATION_TEMP_DIRECTORY}/openvibe/box-algorithm-doc/dox-part")
			FILE(MAKE_DIRECTORY "${DOCUMENTATION_TEMP_DIRECTORY}/openvibe/box-algorithm-doc/opt")
			FILE(WRITE "${DOCUMENTATION_TEMP_DIRECTORY}/openvibe/source-list.txt" "")

			FILE(COPY doc/ DESTINATION ${DOCUMENTATION_TEMP_DIRECTORY}/openvibe/)

			SET(DOC_PROJECT_NAME "openvibe")
			SET(DOC_PROJECT_VERSION ${PROJECT_VERSION})
			SET(DOC_PROJECT_PRODUCT_NAME "${BRAND_NAME} Box Plugins Documentation")
			SET(DOC_PROJECT_COMMITHASH ${PROJECT_COMMITHASH})
			SET(DOC_PROJECT_BRANCH ${PROJECT_BRANCH})
			# this file also uses DOXYGEN_BIN, HHC_BIN, LATEX_BIN and GHOSTSCRIPT_BIN
			CONFIGURE_FILE(src/build-documentation.cmake-skeleton "${DOCUMENTATION_TEMP_DIRECTORY}/build-documentation-openvibe.cmake" @ONLY)
			FILE(APPEND "${DOCUMENTATION_TEMP_DIRECTORY}/build-documentation.cmd"
				"call ../neurort-plugin-inspector.cmd --no-pause --box-snapshot-directory=openvibe/box-algorithm-doc/opt --box-doc-directory=openvibe/box-algorithm-doc/dox\n"
				"cmake -P build-documentation-openvibe.cmake\n")
		ENDIF()


	ENDIF()
ENDIF()

# -----------------------------
# Install files
# -----------------------------
#INSTALL(TARGETS ${PROJECT_NAME}
#	RUNTIME DESTINATION ${CMAKE_INSTALL_FULL_BINDIR}
#	LIBRARY DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR}
#	ARCHIVE DESTINATION ${CMAKE_INSTALL_FULL_LIBDIR})

#INSTALL(DIRECTORY doc/   DESTINATION ${CMAKE_INSTALL_FULL_DOCDIR}     PATTERN ".svn" EXCLUDE)

#INSTALL(DIRECTORY src/   DESTINATION ${CMAKE_INSTALL_FULL_INCLUDEDIR} FILES_MATCHING PATTERN "*.h" PATTERN ".svn" EXCLUDE PATTERN "doc" EXCLUDE)
