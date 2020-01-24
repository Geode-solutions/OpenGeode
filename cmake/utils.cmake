include(GNUInstallDirs)
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)
include(InstallRequiredSystemLibraries)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_program(CLANG_TIDY "clang-tidy-6.0" "clang-tidy-5.0" "clang-tidy")
if(CLANG_TIDY)
    option(USE_CLANG_TIDY "Toggle clang-tidy while compiling" OFF)
    if(USE_CLANG_TIDY)
        message(STATUS "Using clang-tidy")
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY};-checks=*,-abseil*,-fuchsia*,-google*,-llvm*")
    endif()
endif()

#-------------------------------------------------------------------------------
# Turn on the ability to create folders to organize projects and files
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#------------------------------------------------------------------------------------------------
# Build configuration
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

#------------------------------------------------------------------------------------------------
# Platform dependent settings
add_compile_options(
	$<$<CXX_COMPILER_ID:MSVC>:/bigobj>
	$<$<CXX_COMPILER_ID:MSVC>:/DNOMINMAX>
	$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wall>
	$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wextra>
	$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wpedantic>
	$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wno-attributes>
)

if(WIN32)
    string(REPLACE "/MDd" "/MD" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
    string(REPLACE "/MDd" "/MD" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
endif()

#------------------------------------------------------------------------------------------------
# Install configuration    
if(APPLE)
    set(OS_RPATH "@executable_path")
else()
    set(OS_RPATH "$ORIGIN")
endif()
set(CMAKE_MACOSX_RPATH ON)
set(CMAKE_INSTALL_RPATH "${OS_RPATH}")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)

if(EXISTS ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in)
    set(OUTPUT_CONFIG_FILE ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}/${PROJECT_NAME}Config.cmake)
    configure_package_config_file(
        ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in 
        ${OUTPUT_CONFIG_FILE}
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )
    install(
        FILES ${OUTPUT_CONFIG_FILE}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )
endif()

if(EXISTS ${PROJECT_SOURCE_DIR}/LICENSE)
    install(
        FILES ${PROJECT_SOURCE_DIR}/LICENSE
        DESTINATION .
    )
endif()

if(EXISTS ${PROJECT_SOURCE_DIR}/COPYLEFT)
    install(
        FILES ${PROJECT_SOURCE_DIR}/COPYLEFT
        DESTINATION .
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

function(add_geode_library)
    cmake_parse_arguments(GEODE_LIB
        ""
        "NAME;FOLDER"
        "PUBLIC_HEADERS;ADVANCED_HEADERS;PRIVATE_HEADERS;SOURCES;PUBLIC_DEPENDENCIES;PRIVATE_DEPENDENCIES"
        ${ARGN}
    )
    foreach(file ${GEODE_LIB_SOURCES})
        list(APPEND ABSOLUTE_GEODE_LIB_SOURCES 
            "${PROJECT_SOURCE_DIR}/src/${GEODE_LIB_FOLDER}/${file}"
        )
    endforeach()
    foreach(file ${GEODE_LIB_PUBLIC_HEADERS})
        list(APPEND ABSOLUTE_GEODE_LIB_PUBLIC_HEADERS
            "${PROJECT_SOURCE_DIR}/include/${GEODE_LIB_FOLDER}/${file}"
        )
    endforeach()
    foreach(file ${GEODE_LIB_ADVANCED_HEADERS})
        list(APPEND ABSOLUTE_GEODE_LIB_ADVANCED_HEADERS
            "${PROJECT_SOURCE_DIR}/include/${GEODE_LIB_FOLDER}/${file}"
        )
    endforeach()
    foreach(file ${GEODE_LIB_PRIVATE_HEADERS})
        list(APPEND ABSOLUTE_GEODE_LIB_PRIVATE_HEADERS
            "${PROJECT_SOURCE_DIR}/include/${GEODE_LIB_FOLDER}/${file}"
        )
    endforeach()
    add_library(${GEODE_LIB_NAME} SHARED  
        "${ABSOLUTE_GEODE_LIB_SOURCES}"
        "${ABSOLUTE_GEODE_LIB_PUBLIC_HEADERS}"
        "${ABSOLUTE_GEODE_LIB_ADVANCED_HEADERS}"
        "${ABSOLUTE_GEODE_LIB_PRIVATE_HEADERS}"
    )
    add_library(${PROJECT_NAME}::${GEODE_LIB_NAME} ALIAS ${GEODE_LIB_NAME})
    string(TOLOWER ${PROJECT_NAME} project-name)
    string(REGEX REPLACE "-" "_" project_name ${project-name})
    set_target_properties(${GEODE_LIB_NAME}
        PROPERTIES
            OUTPUT_NAME ${PROJECT_NAME}_${GEODE_LIB_NAME}
            DEFINE_SYMBOL ${project_name}_${GEODE_LIB_NAME}_EXPORTS
            CXX_VISIBILITY_PRESET hidden
            VISIBILITY_INLINES_HIDDEN ON
            FOLDER "Libraries"
    )
    source_group("Public Header Files" FILES "${ABSOLUTE_GEODE_LIB_PUBLIC_HEADERS}")
    source_group("Advanced Header Files" FILES "${ABSOLUTE_GEODE_LIB_ADVANCED_HEADERS}")
    source_group("Private Header Files" FILES "${ABSOLUTE_GEODE_LIB_PRIVATE_HEADERS}")
    source_group("Source Files" FILES "${ABSOLUTE_GEODE_LIB_SOURCES}")
    target_include_directories(${GEODE_LIB_NAME}
        PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    target_link_libraries(${GEODE_LIB_NAME}
        PUBLIC ${GEODE_LIB_PUBLIC_DEPENDENCIES}
        PRIVATE ${GEODE_LIB_PRIVATE_DEPENDENCIES}
    )
    export(TARGETS ${GEODE_LIB_NAME}
        NAMESPACE ${PROJECT_NAME}::
        FILE ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}/${PROJECT_NAME}_${GEODE_LIB_NAME}_target.cmake
    )
    generate_export_header(${GEODE_LIB_NAME}
        EXPORT_MACRO_NAME ${project_name}_${GEODE_LIB_NAME}_api
        EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/${GEODE_LIB_FOLDER}/${project_name}_${GEODE_LIB_NAME}_export.h
    )
    install(FILES ${PROJECT_BINARY_DIR}/${GEODE_LIB_FOLDER}/${project_name}_${GEODE_LIB_NAME}_export.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${GEODE_LIB_FOLDER}
    )
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/${GEODE_LIB_FOLDER}/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${GEODE_LIB_FOLDER}
        PATTERN "*/private" EXCLUDE
    )
    install(TARGETS ${GEODE_LIB_NAME}
        EXPORT ${GEODE_LIB_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(EXPORT ${GEODE_LIB_NAME}
        FILE ${PROJECT_NAME}_${GEODE_LIB_NAME}_target.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )
endfunction()

macro(_add_geode_executable exe_path folder_name)
    get_filename_component(target_name ${exe_path} NAME_WE)

    # Set the target as an executable
    add_executable(${target_name} ${exe_path})
    foreach(dependency ${ARGN})
        target_link_libraries(${target_name} PRIVATE ${dependency})
    endforeach()
    
    # Add the project to a folder of projects for the tests
    set_target_properties(${target_name}
        PROPERTIES
            FOLDER ${folder_name}
            INSTALL_RPATH "${OS_RPATH}/../${CMAKE_INSTALL_LIBDIR}"
    )
endmacro()

function(add_geode_binary bin_path)
    _add_geode_executable(${bin_path} "Utilities" ${ARGN})
    install(TARGETS ${target_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endfunction()

macro(_find_dependency_directories directories)
    foreach(dependency ${ARGN})
        get_target_property(TARGET_TYPE ${dependency} TYPE)
        if(TARGET_TYPE STREQUAL "SHARED_LIBRARY")
            list(APPEND directories $<TARGET_FILE_DIR:${dependency}>)
            get_target_property(dependencies ${dependency} LINK_LIBRARIES)
            if(dependencies)
                _find_dependency_directories(directories ${dependencies})
            endif()
        endif()
    endforeach()
    list(REMOVE_DUPLICATES directories)
endmacro()

option(USE_BENCHMARK "Toggle benchmarking of tests" OFF)
function(add_geode_test cpp_file_path)
    _find_dependency_directories(directories ${ARGN})
    _add_geode_executable(${cpp_file_path} "Tests" ${ARGN})
    add_test(NAME ${target_name} COMMAND  ${target_name})
    if(WIN32)
        list(JOIN directories "\\;" directories)
        set_tests_properties(${target_name}
            PROPERTIES
                ENVIRONMENT "Path=${directories}\\;$ENV{Path}"
        )
    else()
        list(JOIN directories ":" directories)
        set_tests_properties(${target_name}
            PROPERTIES
                ENVIRONMENT "LD_LIBRARY_PATH=${directories}:$ENV{LD_LIBRARY_PATH}"
        )
    endif()
    if(USE_BENCHMARK)
        target_compile_definitions(${target_name} PRIVATE OPENGEODE_BENCHMARK)
    endif()
endfunction()
