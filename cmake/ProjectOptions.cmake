string(TOLOWER ${PROJECT_NAME} project-name)
string(REGEX REPLACE "-" "_" project_name ${project-name})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

if(WIN32)
    set(CMAKE_CONFIG_DESTINATION cmake)
else()
    set(CMAKE_CONFIG_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
endif()

if(EXISTS ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in)
    set(OUTPUT_CONFIG_FILE ${PROJECT_BINARY_DIR}/${CMAKE_CONFIG_DESTINATION}/${PROJECT_NAME}Config.cmake)
    configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in 
        ${OUTPUT_CONFIG_FILE}
        INSTALL_DESTINATION ${CMAKE_CONFIG_DESTINATION}
    )
    install(
        FILES ${OUTPUT_CONFIG_FILE}
        DESTINATION ${CMAKE_CONFIG_DESTINATION}
        COMPONENT public
    )
endif()

if(EXISTS ${PROJECT_SOURCE_DIR}/LICENSE)
    install(
        FILES ${PROJECT_SOURCE_DIR}/LICENSE
        DESTINATION .
        COMPONENT public
    )
endif()

if(EXISTS ${PROJECT_SOURCE_DIR}/COPYLEFT)
    install(
        FILES ${PROJECT_SOURCE_DIR}/COPYLEFT
        DESTINATION .
        COMPONENT public
    )
endif()

find_package(Doxygen QUIET)
if(DOXYGEN_FOUND AND EXISTS ${PROJECT_SOURCE_DIR}/cmake/Doxyfile.in)
    # set input and output files
    set(DOXYGEN_IN ${PROJECT_SOURCE_DIR}/cmake/Doxyfile.in)
    set(DOXYGEN_OUT ${PROJECT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message(STATUS "Configuring Doxygen target")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
endif()
