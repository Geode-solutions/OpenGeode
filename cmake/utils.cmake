cmake_minimum_required(VERSION 3.14)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING 
        "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

include(GNUInstallDirs)
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)
include(InstallRequiredSystemLibraries)

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
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wshadow>
    $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wnon-virtual-dtor>
)

if(WIN32)
    if(CMAKE_C_FLAGS_DEBUG)
        string(REPLACE "/MDd" "/MD" CMAKE_C_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
    endif()
    if(CMAKE_CXX_FLAGS_DEBUG)
        string(REPLACE "/MDd" "/MD" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
    endif()
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
set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)

set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY ON)
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
set(CPACK_COMPONENTS_ALL public)

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

function(_export_library library_name)
    export(TARGETS ${library_name}
        NAMESPACE ${PROJECT_NAME}::
        FILE ${PROJECT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}/${PROJECT_NAME}_${library_name}_target.cmake
    )
    install(TARGETS ${library_name}
        EXPORT ${library_name}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT public
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT public
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT public
    )
    install(EXPORT ${library_name}
        FILE ${PROJECT_NAME}_${library_name}_target.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
        COMPONENT public
    )
endfunction()

function(add_geode_library)
    cmake_parse_arguments(GEODE_LIB
        "STATIC"
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
    if(${GEODE_LIB_STATIC})
        add_library(${GEODE_LIB_NAME} STATIC  
            "${ABSOLUTE_GEODE_LIB_SOURCES}"
            "${ABSOLUTE_GEODE_LIB_PUBLIC_HEADERS}"
            "${ABSOLUTE_GEODE_LIB_ADVANCED_HEADERS}"
            "${ABSOLUTE_GEODE_LIB_PRIVATE_HEADERS}"
        )
    else()
        add_library(${GEODE_LIB_NAME} SHARED  
            "${ABSOLUTE_GEODE_LIB_SOURCES}"
            "${ABSOLUTE_GEODE_LIB_PUBLIC_HEADERS}"
            "${ABSOLUTE_GEODE_LIB_ADVANCED_HEADERS}"
            "${ABSOLUTE_GEODE_LIB_PRIVATE_HEADERS}"
        )
    endif()
    add_library(${PROJECT_NAME}::${GEODE_LIB_NAME} ALIAS ${GEODE_LIB_NAME})
    string(TOLOWER ${PROJECT_NAME} project-name)
    string(REGEX REPLACE "-" "_" project_name ${project-name})
    set_target_properties(${GEODE_LIB_NAME}
        PROPERTIES
            POSITION_INDEPENDENT_CODE ON
            OUTPUT_NAME ${PROJECT_NAME}_${GEODE_LIB_NAME}
            DEFINE_SYMBOL ${project_name}_${GEODE_LIB_NAME}_EXPORTS
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
    _export_library(${GEODE_LIB_NAME})
    generate_export_header(${GEODE_LIB_NAME}
        BASE_NAME ${project_name}_${GEODE_LIB_NAME}
        EXPORT_MACRO_NAME ${project_name}_${GEODE_LIB_NAME}_api
        EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/${GEODE_LIB_FOLDER}/${project_name}_${GEODE_LIB_NAME}_export.h
    )
    install(FILES ${PROJECT_BINARY_DIR}/${GEODE_LIB_FOLDER}/${project_name}_${GEODE_LIB_NAME}_export.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${GEODE_LIB_FOLDER}
        COMPONENT public
    )
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/${GEODE_LIB_FOLDER}/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${GEODE_LIB_FOLDER}
        COMPONENT public
        PATTERN "*/private" EXCLUDE
    )
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/${GEODE_LIB_FOLDER}/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${GEODE_LIB_FOLDER}
        COMPONENT private
        FILES_MATCHING
        PATTERN "*/private/*"
    )
endfunction()

macro(_find_dependency_directories directories projects)
    foreach(dependency ${ARGN})
        if(NOT TARGET ${dependency})
            continue()
        endif()
        get_target_property(TARGET_TYPE ${dependency} TYPE)
        if(TARGET_TYPE STREQUAL "SHARED_LIBRARY")
            get_target_property(name ${dependency} NAME)
            string(REPLACE "::" ";" name_list ${name})
            list(GET name_list 0 project)
            list(FIND projects ${project} output)
            if(${output} EQUAL -1)
                list(APPEND projects ${project})
                list(APPEND directories $<TARGET_FILE_DIR:${dependency}>)
            endif()
            get_target_property(dependencies ${dependency} LINK_LIBRARIES)
            if(dependencies)
                _find_dependency_directories(directories projects ${dependencies})
            endif()
        endif()
    endforeach()
endmacro()

function(_add_geode_executable exe_path folder_name)
    get_filename_component(target_name ${exe_path} NAME_WE)

    add_executable(${target_name} ${exe_path})
    foreach(dependency ${ARGN})
        target_link_libraries(${target_name} PRIVATE ${dependency})
    endforeach()
    
    if(WIN32)
        _find_dependency_directories(directories projects ${ARGN})
        list(JOIN directories "\\;" directories)
        set_target_properties(${target_name}
            PROPERTIES
                VS_DEBUGGER_ENVIRONMENT "PATH=${directories}\\;$ENV{Path}"
        )
    endif()
	
    set_target_properties(${target_name}
        PROPERTIES
            FOLDER ${folder_name}
            INSTALL_RPATH "${OS_RPATH}/../${CMAKE_INSTALL_LIBDIR}"
    )
    set(target_name ${target_name} PARENT_SCOPE)
endfunction()

function(add_geode_binary)
    cmake_parse_arguments(GEODE_BINARY
        ""
        "SOURCE"
        "DEPENDENCIES"
        ${ARGN}
    )
    _add_geode_executable(${GEODE_BINARY_SOURCE} "Utilities" ${GEODE_BINARY_DEPENDENCIES})
    install(TARGETS ${target_name}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT public
    )
endfunction()

function(_add_dependency_directories test_name)
    _find_dependency_directories(directories projects ${ARGN})
    if(WIN32)
        list(JOIN directories "\\;" directories)
        file(TO_NATIVE_PATH "${directories}" native_directories)
        message(STATUS "directories = ${directories}")
        message(STATUS "native_directories = ${native_directories}")
        set_tests_properties(${test_name}
            PROPERTIES
                ENVIRONMENT "Path=$ENV{Path}\\;${directories}"
        )
    else()
        list(JOIN directories ":" directories)
        set_tests_properties(${test_name}
            PROPERTIES
                ENVIRONMENT "LD_LIBRARY_PATH=$ENV{LD_LIBRARY_PATH}:${directories}"
        )
    endif()
endfunction()

option(USE_BENCHMARK "Toggle benchmarking of tests" OFF)
function(add_geode_test)
    cmake_parse_arguments(GEODE_TEST
        ""
        "SOURCE"
        "DEPENDENCIES"
        ${ARGN}
    )
    _add_geode_executable(${GEODE_TEST_SOURCE} "Tests" ${GEODE_TEST_DEPENDENCIES})
    add_test(NAME ${target_name} COMMAND ${target_name})
    _add_dependency_directories(${target_name} ${GEODE_TEST_DEPENDENCIES})
    if(USE_BENCHMARK)
        target_compile_definitions(${target_name} PRIVATE OPENGEODE_BENCHMARK)
    endif()
endfunction()

function(add_geode_python_binding)
    set(PYTHON_VERSION "" CACHE STRING "Python version to use for compiling modules")
    set(PYBIND11_PYTHON_VERSION "${PYTHON_VERSION}" CACHE INTERNAL "")
    find_package(pybind11 REQUIRED)
    cmake_parse_arguments(GEODE_BINDING
        ""
        "NAME"
        "SOURCES;DEPENDENCIES"
        ${ARGN}
    )
    pybind11_add_module(${GEODE_BINDING_NAME} 
        SHARED "${GEODE_BINDING_SOURCES}"
    )
    add_library(${PROJECT_NAME}::${GEODE_BINDING_NAME} ALIAS ${GEODE_BINDING_NAME})
    target_link_libraries(${GEODE_BINDING_NAME} 
        PRIVATE "${GEODE_BINDING_DEPENDENCIES}"
    )
    string(TOLOWER ${PROJECT_NAME} project-name)
    string(REGEX REPLACE "-" "_" project_name ${project-name})
    set_target_properties(${GEODE_BINDING_NAME}
        PROPERTIES
            OUTPUT_NAME ${project_name}_${GEODE_BINDING_NAME}
            CXX_VISIBILITY_PRESET "default"
            FOLDER "Bindings/Python"
    )
    _export_library(${GEODE_BINDING_NAME})
endfunction()

function(add_geode_python_test)
    cmake_parse_arguments(GEODE_TEST
        ""
        "SOURCE"
        "DEPENDENCIES"
        ${ARGN}
    )
    get_filename_component(absolute_path ${GEODE_TEST_SOURCE} ABSOLUTE)
    get_filename_component(test_name ${GEODE_TEST_SOURCE} NAME_WE)
    add_custom_target(${test_name} SOURCES ${GEODE_TEST_SOURCE})
    set_target_properties(${test_name}
        PROPERTIES
            FOLDER "Bindings/Python/Tests"
    )
    add_test(NAME ${test_name} COMMAND ${PYTHON_EXECUTABLE} ${absolute_path})
    _add_dependency_directories(${test_name} ${GEODE_TEST_DEPENDENCIES})
    foreach(dependency ${GEODE_TEST_DEPENDENCIES})
        list(APPEND directories $<TARGET_FILE_DIR:${dependency}>)
    endforeach()
    if(WIN32)
        list(JOIN directories "\\;" directories)
        set_property(
            TEST
                ${test_name}
            APPEND
            PROPERTY
                ENVIRONMENT "PYTHONPATH=${directories}\\;$ENV{PYTHONPATH}"
        )
    else()
        list(JOIN directories ":" directories)
        set_property(
            TEST
                ${test_name}
            APPEND
            PROPERTY
                ENVIRONMENT "PYTHONPATH=${directories}:$ENV{PYTHONPATH}"
        )
    endif()
endfunction()
