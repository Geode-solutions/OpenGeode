if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING 
        "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

include(GNUInstallDirs)
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)
include(InstallRequiredSystemLibraries)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_COLOR_DIAGNOSTICS ON)

find_program(CLANG_TIDY "clang-tidy")
if(CLANG_TIDY)
    option(USE_CLANG_TIDY "Toggle clang-tidy while compiling" OFF)
    if(USE_CLANG_TIDY)
        message(STATUS "Using clang-tidy")
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY};--extra-arg=-std=c++11")
    endif()
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

if(NOT BUILD_SHARED_LIBS)
    add_link_options(
        $<$<CXX_COMPILER_ID:MSVC>:/INCREMENTAL:NO> 
    )
endif()

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
set(CMAKE_INSTALL_RPATH "$ORIGIN")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY ON)
set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)
set(CPACK_COMPONENT_INCLUDE_TOPLEVEL_DIRECTORY ON)
set(CPACK_COMPONENTS_GROUPING ALL_COMPONENTS_IN_ONE)
set(CPACK_COMPONENTS_ALL public)

add_custom_target(third_party)
add_custom_target(essential)
