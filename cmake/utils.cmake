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

if(WIN32)
    add_custom_target(windows_post_compilation ALL)
    string(REPLACE "/MDd" "/MD" CMAKE_CXX_FLAGS_DEBUG ${CMAKE_CXX_FLAGS_DEBUG})
endif()

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

function(add_geode_library folder_path)
    get_filename_component(target_name ${folder_path} NAME)
    set(lib_source_dir ${PROJECT_SOURCE_DIR}/src/${folder_path})
    set(lib_include_dir ${PROJECT_SOURCE_DIR}/include/${folder_path})
    add_library(${target_name} SHARED "${lib_source_dir}/common.cpp")
    add_library(${PROJECT_NAME}::${target_name} ALIAS ${target_name})
    if(WIN32)
        add_dependencies(windows_post_compilation ${target_name})
    endif()
    include(${lib_source_dir}/CMakeLists.txt)
    set(all_sources
        "${sources}"
        "${public_headers}"
        "${advanced_headers}"
    )
    target_sources(${target_name} PRIVATE "${all_sources}")
    set_target_properties(${target_name}
        PROPERTIES
            OUTPUT_NAME ${PROJECT_NAME}_${target_name}
            DEFINE_SYMBOL ${PROJECT_NAME}_${target_name}_EXPORTS
            CMAKE_CXX_VISIBILITY_PRESET hidden
            CMAKE_VISIBILITY_INLINES_HIDDEN ON
            FOLDER "Libraries"
    )
    # TODO: Use TREE keyword when we change to cmake 3.8
    source_group("Public Header Files" FILES ${public_headers})
    source_group("Advanced Header Files" FILES ${advanced_headers})
    source_group("Source Files" FILES ${sources})
    target_include_directories(${target_name}
        PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    export(TARGETS ${target_name}
        NAMESPACE ${PROJECT_NAME}::
        FILE ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}/${PROJECT_NAME}_${target_name}_target.cmake
    )
    string(TOLOWER ${PROJECT_NAME} project_name)
    generate_export_header(${target_name}
        EXPORT_MACRO_NAME ${project_name}_${target_name}_api
        EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/${folder_path}/${project_name}_${target_name}_export.h
    )
    install(FILES ${PROJECT_BINARY_DIR}/${folder_path}/${project_name}_${target_name}_export.h
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${folder_path}
    )
    install(DIRECTORY ${lib_include_dir}/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${folder_path}
    )
    install(TARGETS ${target_name}
        EXPORT ${target_name}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(EXPORT ${target_name}
        FILE ${PROJECT_NAME}_${target_name}_target.cmake
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
    )
endfunction()

macro(add_geode_executable exe_path folder_name)
    get_filename_component(exe_name ${exe_path} NAME_WE)

    # Set the target as an executable
    add_executable(${exe_name} ${exe_path})
    if(WIN32)
        add_dependencies(windows_post_compilation ${exe_name})
    endif()
    foreach(dependency ${ARGN})
        target_link_libraries(${exe_name} PRIVATE ${dependency})
    endforeach()
    
    # Add the project to a folder of projects for the tests
    set_target_properties(${exe_name}
        PROPERTIES
            FOLDER ${folder_name}
            INSTALL_RPATH "${OS_RPATH}/../${CMAKE_INSTALL_LIBDIR}"
    )
endmacro()

function(add_geode_binary bin_path)
    add_geode_executable(${bin_path} "Utilities" ${ARGN})
    install(TARGETS ${exe_name} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endfunction()

function(add_geode_test cpp_file_path)
    add_geode_executable(${cpp_file_path} "Tests" ${ARGN})
    add_test(NAME ${exe_name} COMMAND ${exe_name})
endfunction()

function(copy_windows_binaries dependency)
    if(WIN32)
        get_target_property(release_location ${dependency} IMPORTED_LOCATION_RELEASE)
        if(release_location)
            get_filename_component(release_location_directory ${release_location} DIRECTORY)
            add_custom_command(TARGET windows_post_compilation POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory 
                    "${release_location_directory}"
                    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}"
			)
        endif()
        get_target_property(debug_location ${dependency} IMPORTED_LOCATION_DEBUG)
        if(debug_location)
            get_filename_component(debug_location_directory ${debug_location} DIRECTORY)
            add_custom_command(TARGET windows_post_compilation POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory 
                    "${debug_location_directory}"
                    "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${CMAKE_CFG_INTDIR}"
			)
        endif()
    endif()
endfunction()
