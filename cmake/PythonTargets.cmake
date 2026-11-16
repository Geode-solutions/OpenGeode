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
    # The wheel puts the extension modules at the root of the package and the
    # shared libraries in its ${CMAKE_INSTALL_LIBDIR} subfolder (see
    # add_geode_python_wheel), the install and build trees next to them.
    # On Windows, the __init__.py of the wheel adds the DLL directory instead.
    if(APPLE)
        set(origin "@loader_path")
    else()
        set(origin "$ORIGIN")
    endif()
    set_target_properties(${GEODE_BINDING_NAME}
        PROPERTIES
            OUTPUT_NAME ${project_name}_${GEODE_BINDING_NAME}
            CXX_VISIBILITY_PRESET "default"
            FOLDER "Bindings/Python"
            BUILD_RPATH "${origin}/${CMAKE_INSTALL_LIBDIR}"
            INSTALL_RPATH "${origin};${origin}/${CMAKE_INSTALL_LIBDIR}"
    )
    _export_library(${GEODE_BINDING_NAME} OPTIONAL)
endfunction()

function(add_geode_python_test)
    cmake_parse_arguments(GEODE_TEST
        ""
        "SOURCE;NAME"
        "DEPENDENCIES;ARGUMENTS"
        ${ARGN}
    )
    get_filename_component(absolute_path ${GEODE_TEST_SOURCE} ABSOLUTE)
    if(GEODE_TEST_NAME)
        set(test_name ${GEODE_TEST_NAME})
    else()
        get_filename_component(test_name ${GEODE_TEST_SOURCE} NAME_WE)
    endif()
    add_custom_target(${test_name} SOURCES ${GEODE_TEST_SOURCE})
    set_target_properties(${test_name}
        PROPERTIES
            FOLDER "Bindings/Python/Tests"
    )
    geode_python_test_executable(test_python)
    add_test(NAME ${test_name}
        COMMAND ${test_python} ${absolute_path} ${GEODE_TEST_ARGUMENTS}
    )
    set_tests_properties(${test_name} PROPERTIES TIMEOUT 300) 
    _find_dependency_directories(directories ${GEODE_TEST_DEPENDENCIES})
    # Module imported by the tests before any binding, shipped with these CMake files
    set(test_env_directory "${PROJECT_BINARY_DIR}/geode_test_env")
    configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/geode_test_env.py"
        "${test_env_directory}/geode_test_env.py" COPYONLY)
    # Pure-Python modules (e.g. basic.py) copied by add_geode_python_wheel
    get_property(python_modules_directory
        GLOBAL PROPERTY ${PROJECT_NAME}_PYTHON_MODULES_DIRECTORY
    )
    if(python_modules_directory)
        list(APPEND directories "${python_modules_directory}")
    endif()
    if(WIN32)
        list(JOIN directories "\\;" directories)
        _escaped_environment_variable(path Path)
        _escaped_environment_variable(python_path PYTHONPATH)
        # Python does not search the Path for DLLs: geode_test_env adds these directories only
        set_tests_properties(${test_name}
            PROPERTIES
                ENVIRONMENT "Path=${directories}\\;${path};PYTHONPATH=${test_env_directory}\\;${directories}\\;${python_path};GEODE_DLL_DIRECTORIES=${directories}"
        )
    else()
        list(JOIN directories ":" directories)
        set_tests_properties(${test_name}
            PROPERTIES
                ENVIRONMENT "LD_LIBRARY_PATH=${directories}:$ENV{LD_LIBRARY_PATH};PYTHONPATH=${test_env_directory}:${directories}:$ENV{PYTHONPATH}"
        )
    endif()
endfunction()

# The stub gates match messages of these exact versions of the tools (the
# pybind11 artefacts ignored by stubtest are recognised by their text): update
# them together with check_python_stubs.py.
set(GEODE_STUBGEN_REQUIREMENT "pybind11-stubgen==3.0.0")
set(GEODE_MYPY_REQUIREMENT "mypy==2.4.1")

# Configuration dependencies, not in script mode (cmake -P, used by the tests)
function(_geode_configure_depends)
    if(NOT CMAKE_SCRIPT_MODE_FILE AND ARGN)
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${ARGN})
    endif()
endfunction()

# Version of the C++ package recorded for the Python distribution: the first
# release version (1.2.3, 1.2.3-rc.4 or 1.2.3rc4, not 0.0.0) of
# CPACK_PACKAGE_VERSION then WHEEL_VERSION; else 0.0.0 when one of them is
# 0.0.0 (development build); else "unknown" (e.g. the "master" default of the
# superbuild).
function(geode_python_distribution_version output)
    set(release "^v?[0-9]+\\.[0-9]+\\.[0-9]+(-rc\\.[0-9]+|rc[0-9]+)?$")
    set(result "unknown")
    foreach(candidate IN ITEMS "${CPACK_PACKAGE_VERSION}" "${WHEEL_VERSION}")
        if(NOT candidate MATCHES "${release}")
            continue()
        endif()
        if(candidate MATCHES "^v?0\\.0\\.0$")
            set(result "0.0.0")
        else()
            set(result "${candidate}")
            break()
        endif()
    endforeach()
    set(${output} "${result}" PARENT_SCOPE)
endfunction()

# Files of the Python distribution of the project, next to its CMake files, for
# the Python environments of the downstream projects:
# ${PROJECT_NAME}_python_distribution.txt (name of the wheel and version of the
# C++ package, see _geode_python_found_distributions) and
# ${PROJECT_NAME}_python_requirements.in (runtime requirements, see
# geode_python_requirements_file). The names carry the project because the
# projects can share one install prefix (and its CMake directory). Provided by every
# configuration of a project with Python bindings, with or without them, so
# that the C++ packages carry them.
function(geode_python_distribution_files name)
    get_property(written GLOBAL PROPERTY ${PROJECT_NAME}_PYTHON_DISTRIBUTION_NAME)
    if("${written}" STREQUAL "${name}")
        return()
    endif()
    geode_python_distribution_version(version)
    set(directory "${PROJECT_BINARY_DIR}/${CMAKE_CONFIG_DESTINATION}")
    file(CONFIGURE OUTPUT "${directory}/${PROJECT_NAME}_python_distribution.txt"
        CONTENT "${name}\n${version}\n"
    )
    set(requirements "${PROJECT_SOURCE_DIR}/bindings/python/requirements.in")
    if(EXISTS "${requirements}")
        configure_file("${requirements}"
            "${directory}/${PROJECT_NAME}_python_requirements.in" COPYONLY
        )
    endif()
    if(NOT written)
        install(FILES "${directory}/${PROJECT_NAME}_python_distribution.txt"
            DESTINATION ${CMAKE_CONFIG_DESTINATION}
            COMPONENT public
        )
        if(EXISTS "${requirements}")
            install(FILES "${directory}/${PROJECT_NAME}_python_requirements.in"
                DESTINATION ${CMAKE_CONFIG_DESTINATION}
                COMPONENT public
            )
        endif()
    endif()
    set_property(GLOBAL PROPERTY ${PROJECT_NAME}_PYTHON_DISTRIBUTION_NAME "${name}")
endfunction()

# Name of the wheel of the project, read from the add_geode_python_wheel call
# of bindings/python/CMakeLists.txt, empty when not found
function(_geode_python_wheel_name output)
    set(name "")
    set(bindings "${PROJECT_SOURCE_DIR}/bindings/python/CMakeLists.txt")
    if(EXISTS "${bindings}")
        file(READ "${bindings}" content)
        if(content MATCHES "add_geode_python_wheel[ \t]*\\([^)]*NAME[ \t\r\n]+\"([^\"]+)\"")
            set(name "${CMAKE_MATCH_1}")
        elseif(content MATCHES "add_geode_python_wheel[ \t]*\\([^)]*NAME[ \t\r\n]+([^ \t\r\n)\"]+)")
            set(name "${CMAKE_MATCH_1}")
        endif()
    endif()
    set(${output} "${name}" PARENT_SCOPE)
endfunction()

# Python distributions of the geode projects found by find_package: each one
# provides, next to its CMake files, <Package>_python_distribution.txt (written by
# add_geode_python_wheel) with the name of its wheel and the version of its C++
# package. ${<Package>_VERSION} is used instead when defined (not 0.0.0).
# Stores the normalized names, versions ("unknown" when not known), directories
# of the published wheels and directories of the CMake files in global properties.
function(_geode_python_found_distributions)
    get_property(done GLOBAL PROPERTY ${PROJECT_NAME}_PYTHON_FOUND_DONE)
    if(done)
        return()
    endif()
    set(names)
    set(versions)
    set(wheel_directories)
    set(cmake_directories)
    get_property(packages GLOBAL PROPERTY PACKAGES_FOUND)
    foreach(package IN LISTS packages)
        set(file "${${package}_DIR}/${package}_python_distribution.txt")
        if(NOT ${package}_DIR OR NOT EXISTS "${file}")
            continue()
        endif()
        _geode_configure_depends("${file}")
        file(STRINGS "${file}" lines)
        list(LENGTH lines nb_lines)
        if(nb_lines EQUAL 0)
            continue()
        endif()
        list(GET lines 0 name)
        string(TOLOWER "${name}" name)
        string(REGEX REPLACE "[-_.]+" "-" name "${name}")
        set(version "unknown")
        if(nb_lines GREATER 1)
            list(GET lines 1 version)
        endif()
        if(DEFINED ${package}_VERSION AND NOT "${${package}_VERSION}" STREQUAL "0.0.0")
            set(version "${${package}_VERSION}")
        endif()
        if("${version}" STREQUAL "")
            set(version "unknown")
        endif()
        list(APPEND names "${name}")
        list(APPEND versions "${version}")
        list(APPEND wheel_directories "${${package}_DIR}/python_wheels")
        list(APPEND cmake_directories "${${package}_DIR}")
    endforeach()
    set_property(GLOBAL PROPERTY ${PROJECT_NAME}_PYTHON_FOUND_NAMES "${names}")
    set_property(GLOBAL PROPERTY ${PROJECT_NAME}_PYTHON_FOUND_VERSIONS "${versions}")
    set_property(GLOBAL PROPERTY ${PROJECT_NAME}_PYTHON_FOUND_WHEELS "${wheel_directories}")
    set_property(GLOBAL PROPERTY ${PROJECT_NAME}_PYTHON_FOUND_CMAKE "${cmake_directories}")
    set_property(GLOBAL PROPERTY ${PROJECT_NAME}_PYTHON_FOUND_DONE ON)
endfunction()

# Python requirements of the project, as a file for pip: bindings/python/
# requirements.in lists the runtime dependencies of a project (numpy...) and
# exists in every checkout, unlike requirements.txt which is generated. The
# runtime requirements of the geode projects found by find_package (installed
# next to their CMake files by add_geode_python_wheel) are added, so that a
# downstream project gets them without installing its upstream packages. The
# distributions of the geode projects found by find_package are removed: they
# must match the C++ packages of the build (see geode_python_geode_packages).
# Their normalized names are stored in the
# ${PROJECT_NAME}_PYTHON_GEODE_REQUIREMENTS global property.
function(geode_python_requirements_file output)
    get_property(requirements_file
        GLOBAL PROPERTY ${PROJECT_NAME}_PYTHON_REQUIREMENTS_FILE
    )
    if(requirements_file)
        set(${output} "${requirements_file}" PARENT_SCOPE)
        return()
    endif()
    _geode_python_found_distributions()
    get_property(found_names GLOBAL PROPERTY ${PROJECT_NAME}_PYTHON_FOUND_NAMES)
    set(files)
    get_property(packages GLOBAL PROPERTY PACKAGES_FOUND)
    foreach(package IN LISTS packages)
        set(file "${${package}_DIR}/${package}_python_requirements.in")
        if(${package}_DIR AND EXISTS "${file}")
            list(APPEND files "${file}")
        endif()
    endforeach()
    set(project_file "${PROJECT_SOURCE_DIR}/bindings/python/requirements.in")
    if(EXISTS "${project_file}")
        list(APPEND files "${project_file}")
    endif()
    list(REMOVE_DUPLICATES files)
    _geode_configure_depends(${files})
    set(requirements_file "${PROJECT_BINARY_DIR}/python_requirements.txt")
    set(geode_file "${PROJECT_BINARY_DIR}/python_geode_requirements.txt")
    set(command ${PYTHON_EXECUTABLE}
        "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/geode_python_package.py" requirements
        --output "${requirements_file}" --geode-output "${geode_file}"
    )
    foreach(name IN LISTS found_names)
        list(APPEND command --geode "${name}")
    endforeach()
    list(APPEND command ${files})
    execute_process(COMMAND ${command} RESULT_VARIABLE result)
    if(result)
        string(REPLACE ";" " " command_text "${command}")
        message(FATAL_ERROR
            "Cannot merge the Python requirements with:\n  ${command_text}"
        )
    endif()
    file(STRINGS "${geode_file}" geode_requirements)
    set_property(GLOBAL PROPERTY
        ${PROJECT_NAME}_PYTHON_GEODE_REQUIREMENTS "${geode_requirements}"
    )
    set_property(GLOBAL PROPERTY
        ${PROJECT_NAME}_PYTHON_REQUIREMENTS_FILE "${requirements_file}"
    )
    set(${output} "${requirements_file}" PARENT_SCOPE)
endfunction()

# Arguments of geode_python_package.py install-geode for the geode
# distributions required by the project: name, version of the C++ package,
# directory of the published wheels and directory of the CMake files. The
# distributions without a local wheel are stored in ${without_local_wheel}.
function(geode_python_geode_packages output without_local_wheel)
    geode_python_requirements_file(requirements_file)
    get_property(required GLOBAL PROPERTY ${PROJECT_NAME}_PYTHON_GEODE_REQUIREMENTS)
    get_property(names GLOBAL PROPERTY ${PROJECT_NAME}_PYTHON_FOUND_NAMES)
    get_property(versions GLOBAL PROPERTY ${PROJECT_NAME}_PYTHON_FOUND_VERSIONS)
    get_property(wheel_directories GLOBAL PROPERTY ${PROJECT_NAME}_PYTHON_FOUND_WHEELS)
    get_property(cmake_directories GLOBAL PROPERTY ${PROJECT_NAME}_PYTHON_FOUND_CMAKE)
    set(arguments)
    set(missing)
    foreach(name IN LISTS required)
        list(FIND names "${name}" index)
        if(index EQUAL -1)
            # A geode package without <Package>_python_distribution.txt (an older C++
            # package): never installed unpinned, only an installed one is used
            set(version "unknown")
            set(wheel_directory "${PROJECT_BINARY_DIR}/no_python_wheels")
            set(cmake_directory "${PROJECT_BINARY_DIR}/no_python_wheels")
        else()
            list(GET versions ${index} version)
            list(GET wheel_directories ${index} wheel_directory)
            list(GET cmake_directories ${index} cmake_directory)
        endif()
        list(APPEND arguments
            --package "${name}" "${version}" "${wheel_directory}" "${cmake_directory}"
        )
        file(GLOB wheels "${wheel_directory}/*.whl")
        if(NOT wheels)
            list(APPEND missing "${name}")
        endif()
    endforeach()
    set(${output} "${arguments}" PARENT_SCOPE)
    set(${without_local_wheel} "${missing}" PARENT_SCOPE)
endfunction()

# Install the requirements of the package and the given extra packages in the
# virtual environment, and fail with the command shown if pip fails.
function(_geode_pip_install venv_bin venv_path)
    cmake_parse_arguments(GEODE_PIP "" "" "PACKAGES" ${ARGN})
    geode_python_requirements_file(requirements_file)
    set(pip_command ${venv_bin}/python -m pip install
        -r "${requirements_file}" ${GEODE_PIP_PACKAGES}
    )
    execute_process(
        COMMAND ${pip_command}
        RESULT_VARIABLE pip_result
    )
    if(pip_result)
        string(REPLACE ";" " " pip_command_text "${pip_command}")
        message(FATAL_ERROR
            "Cannot install the Python requirements in ${venv_path} with:\n"
            "  ${pip_command_text}"
        )
    endif()
endfunction()

# Options of the Python test environment: --system-site-packages when the
# Python used to configure has installed some of the given geode distributions
# (those without a local wheel). Sets ${installed} to them (name==version).
function(_geode_python_test_venv_options options installed without_local_wheel)
    set(venv_options)
    set(installed_in_python)
    if(without_local_wheel)
        execute_process(
            COMMAND ${PYTHON_EXECUTABLE}
                "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/geode_python_package.py"
                installed ${without_local_wheel}
            OUTPUT_VARIABLE installed_in_python
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(installed_in_python)
            set(venv_options --system-site-packages)
        endif()
    endif()
    set(${options} "${venv_options}" PARENT_SCOPE)
    set(${installed} "${installed_in_python}" PARENT_SCOPE)
endfunction()

# Set ${output} to the hash of everything the Python test environment is built
# from. It includes the version of the base interpreter (and not only its path)
# as an interpreter upgraded in place no longer matches its virtual environment.
function(_geode_python_environment_hash output
    requirements_content venv_options installed_in_python geode_packages
    local_wheels_state
)
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -c "import sys; print(sys.version)"
        OUTPUT_VARIABLE python_version
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE version_result
    )
    if(version_result)
        set(python_version "unknown")
    endif()
    string(MD5 environment_hash
        "${PYTHON_EXECUTABLE}|${python_version}|${requirements_content}|${GEODE_STUBGEN_REQUIREMENT}|${GEODE_MYPY_REQUIREMENT}|${venv_options}|${installed_in_python}|${geode_packages}|${local_wheels_state}"
    )
    set(${output} "${environment_hash}" PARENT_SCOPE)
endfunction()

# The Python tests run in a virtual environment created from PYTHON_EXECUTABLE
# in the build directory, with the requirements of the package (numpy...) and
# the pinned stub tools installed: the Python used to configure does not need
# any package. The environment is only rebuilt when the requirements change.
# Set ${output} to the Python of the environment.
function(geode_python_test_executable output)
    get_property(test_python GLOBAL PROPERTY ${PROJECT_NAME}_TEST_PYTHON)
    if(test_python)
        set(${output} "${test_python}" PARENT_SCOPE)
        return()
    endif()
    set(venv_path "${PROJECT_BINARY_DIR}/python_test_venv")
    if(WIN32)
        set(venv_bin "${venv_path}/Scripts")
    else()
        set(venv_bin "${venv_path}/bin")
    endif()
    geode_python_requirements_file(requirements_file)
    file(READ "${requirements_file}" requirements_content)
    # The geode distributions come from the local wheels first. Without one,
    # the CI installs the wheels matching the C++ packages in the Python used
    # to configure: the environment then sees its packages (step 2 of
    # install-geode), instead of downloading them again.
    set(package_script "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/geode_python_package.py")
    geode_python_geode_packages(geode_packages without_local_wheel)
    _geode_python_test_venv_options(venv_options installed_in_python
        "${without_local_wheel}"
    )
    set(local_wheels_state)
    foreach(argument IN LISTS geode_packages)
        if(IS_DIRECTORY "${argument}")
            file(GLOB wheels "${argument}/*.whl")
            foreach(wheel IN LISTS wheels)
                file(TIMESTAMP "${wheel}" timestamp)
                list(APPEND local_wheels_state "${wheel}@${timestamp}")
            endforeach()
        endif()
    endforeach()
    _geode_python_environment_hash(environment_hash
        "${requirements_content}" "${venv_options}" "${installed_in_python}"
        "${geode_packages}" "${local_wheels_state}"
    )
    set(stamp_file "${venv_path}/geode_environment_hash")
    set(stamp "")
    if(EXISTS "${stamp_file}")
        file(READ "${stamp_file}" stamp)
    endif()
    if(NOT "${stamp}" STREQUAL "${environment_hash}" OR NOT EXISTS "${venv_bin}/python${CMAKE_EXECUTABLE_SUFFIX}")
        message(STATUS "Creating the Python test environment at: ${venv_path}")
        execute_process(
            COMMAND ${PYTHON_EXECUTABLE} -m venv --clear ${venv_options} "${venv_path}"
            RESULT_VARIABLE venv_result
        )
        if(venv_result)
            message(FATAL_ERROR
                "Cannot create the Python test environment with:\n"
                "  ${PYTHON_EXECUTABLE} -m venv --clear ${venv_options} ${venv_path}"
            )
        endif()
        _geode_pip_install("${venv_bin}" "${venv_path}"
            PACKAGES ${GEODE_STUBGEN_REQUIREMENT} ${GEODE_MYPY_REQUIREMENT}
        )
        # Only a warning when a distribution is missing: the tests import the
        # geode modules of the found projects from their binary folder
        # (add_geode_python_test), the distribution is only a fallback
        execute_process(
            COMMAND ${venv_bin}/python "${package_script}" install-geode
                --missing warn ${geode_packages}
            RESULT_VARIABLE geode_result
        )
        if(geode_result)
            message(FATAL_ERROR
                "Cannot install the geode Python packages in ${venv_path}"
            )
        endif()
        file(WRITE "${stamp_file}" "${environment_hash}")
    endif()
    set(test_python "${venv_bin}/python")
    set_property(GLOBAL PROPERTY ${PROJECT_NAME}_TEST_PYTHON "${test_python}")
    set(${output} "${test_python}" PARENT_SCOPE)
endfunction()

# Quality gates on the stubs of the binding modules, run by CTest: generate the
# stubs of the compiled modules with pybind11-stubgen, then check them with
# check_python_stubs.py (fix of the TypeVars, no unnamed argN parameter, no
# unresolved or raw C++ type, mypy.stubtest unless NO_STUBTEST). The wheel target runs the same gates
# on the wheel, with stubtest on the whole package.
# Runs in the Python test environment, see geode_python_test_executable.
function(add_geode_python_stub_test)
    cmake_parse_arguments(GEODE_STUB_TEST
        "NO_STUBTEST"
        "NAME"
        "MODULES;DEPENDENCIES"
        ${ARGN}
    )
    if(NOT GEODE_STUB_TEST_NAME)
        set(GEODE_STUB_TEST_NAME "test-py-stubs")
    endif()
    if(GEODE_STUB_TEST_NO_STUBTEST)
        set(stubtest_arguments --no-stubtest)
    else()
        set(stubtest_arguments
            --mypy-config-file "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/stubtest_mypy.ini"
        )
    endif()
    add_geode_python_test(
        SOURCE "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_python_stubs.py"
        NAME ${GEODE_STUB_TEST_NAME}
        ARGUMENTS modules ${GEODE_STUB_TEST_MODULES} ${stubtest_arguments}
        DEPENDENCIES ${GEODE_STUB_TEST_DEPENDENCIES}
    )
endfunction()

# Built without superbuild, the wheel target publishes the wheel in the build
# tree (${publish_directory}), but the downstream projects usually find the
# CMake files of the install tree and look for the wheel next to them
# (<Package>_DIR/python_wheels): the published wheels are installed there
# (component python_wheels), and ${output} is the command of the wheel target
# publishing the wheel there directly when the project is already installed
# (see publish --if-installed of geode_python_package.py).
function(_geode_python_publish_in_install publish_directory wheel_file python package_script output)
    file(MAKE_DIRECTORY "${publish_directory}")
    # Own component: installed by a plain install, but not in the C++
    # packages (cpack of the public and private components)
    install(DIRECTORY "${publish_directory}/"
        DESTINATION "${CMAKE_CONFIG_DESTINATION}/python_wheels"
        COMPONENT python_wheels
    )
    set(${output}
        COMMAND ${python} "${package_script}" publish --if-installed "${wheel_file}"
            "${CMAKE_INSTALL_PREFIX}/${CMAKE_CONFIG_DESTINATION}/python_wheels"
        PARENT_SCOPE
    )
endfunction()

function(add_geode_python_wheel)
    cmake_parse_arguments(GEODE_WHEEL
        "SUPERBUILD"
        "NAME;DESCRIPTION;LICENSE;DOCUMENTATION"
        "MODULES"
        ${ARGN}
    )
    set(wheel_output_path "${PROJECT_BINARY_DIR}/wheel")
    set(wheel_output_directory "${wheel_output_path}/${project_name}")
    file(WRITE "${wheel_output_directory}/py.typed" "")
    set(wheel_init "${wheel_output_directory}/__init__.py")
    if(WIN32)
        set(binary_folder "${CMAKE_INSTALL_BINDIR}")
    else()
        set(binary_folder "${CMAKE_INSTALL_LIBDIR}")
    endif()
    if(${GEODE_WHEEL_SUPERBUILD})
        set(wheel_build_directory "${CMAKE_INSTALL_PREFIX}")
    else()
        set(wheel_build_directory "${PROJECT_BINARY_DIR}")
    endif()
    set(wheel_import "${wheel_build_directory}/${binary_folder}/${project_name}.py")
    set(header "# Copyright (c) 2019 - 2026 Geode-solutions\n\n")
    file(WRITE ${wheel_init} "#${header}")
    file(WRITE ${wheel_import} "${header}")
    # Text of the package docstring (help(opengeode), opengeode.__doc__): the
    # first statement of both the __init__.py of the wheel and the flat module
    # imported from the build and install trees
    if(GEODE_WHEEL_DOCUMENTATION)
        # Regenerated when the text changes
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${GEODE_WHEEL_DOCUMENTATION}")
        file(READ "${GEODE_WHEEL_DOCUMENTATION}" documentation)
        string(FIND "${documentation}" "\"\"\"" triple_quote)
        string(FIND "${documentation}" "\\" backslash)
        if(NOT triple_quote EQUAL -1 OR NOT backslash EQUAL -1)
            message(FATAL_ERROR "${GEODE_WHEEL_DOCUMENTATION} cannot contain triple quotes or backslashes")
        endif()
        file(APPEND ${wheel_init} "\"\"\"${documentation}\"\"\"\n\n")
        file(APPEND ${wheel_import} "\"\"\"${documentation}\"\"\"\n\n")
    endif()
    # The extension modules are at the root of the package, whatever the
    # platform, so that their public path (opengeode.opengeode_py_mesh...) does
    # not depend on it: the shared libraries stay in ${binary_folder}
    if(WIN32)
        file(APPEND ${wheel_init} "import os, pathlib\n")
        file(APPEND ${wheel_init} "os.add_dll_directory(pathlib.Path(__file__).parent.resolve().joinpath('${binary_folder}'))\n\n")
    endif()
    # Build-tree copy of the pure-Python modules, imported by the Python tests
    set(python_modules_directory "${PROJECT_BINARY_DIR}/python_modules")
    set_property(GLOBAL PROPERTY
        ${PROJECT_NAME}_PYTHON_MODULES_DIRECTORY "${python_modules_directory}"
    )
    foreach(module ${GEODE_WHEEL_MODULES})
        string(REPLACE ".py" "" module_name "${module}")
        file(APPEND ${wheel_init} "from .${module_name} import *\n")
        file(APPEND ${wheel_import} "from ${module_name} import *\n")
        configure_file("${module}" "${wheel_build_directory}/${binary_folder}/${module}" COPYONLY)
        configure_file("${module}" "${python_modules_directory}/${module}" COPYONLY)
        file(READ "${module}" FILE_CONTENTS)
        string(REPLACE "from ${project_name}_" "from .${project_name}_" FILE_CONTENTS "${FILE_CONTENTS}")
        file(WRITE "${wheel_output_directory}/${module}" ${FILE_CONTENTS})
    endforeach()
    configure_file("${wheel_import}" "${python_modules_directory}/${project_name}.py" COPYONLY)
    # The flat modules (${project_name}.py and the pure-Python modules) next to
    # the extension modules, imported by the Python tests of the downstream
    # projects from the binary folder of this one (add_geode_python_test)
    set(flat_modules "${python_modules_directory}/${project_name}.py")
    foreach(module ${GEODE_WHEEL_MODULES})
        list(APPEND flat_modules "${python_modules_directory}/${module}")
    endforeach()
    install(FILES ${flat_modules}
        DESTINATION ${binary_folder}
        COMPONENT public
    )
    if(NOT WHEEL_VERSION)
        set(WHEEL_VERSION "0.0.0")
    endif()
    # Normally already provided when PythonTargets.cmake was included: the
    # name given here is the one of the wheel
    geode_python_distribution_files("${GEODE_WHEEL_NAME}")
    configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/setup.py.in" "${wheel_output_directory}/../setup.py")
    configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/pyproject.toml.in" "${wheel_output_directory}/../pyproject.toml")
    file(MAKE_DIRECTORY "${wheel_build_directory}/share")
    set(venv_path "${wheel_output_path}/venv")
    message(STATUS "Creating virtual environment at: ${venv_path}")
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -m venv ${venv_path}
    )
    if(UNIX)
        set(venv_bin "${venv_path}/bin")
    elseif(WIN32)
        set(venv_bin "${venv_path}/Scripts")
    endif()
    execute_process(
        COMMAND ${venv_bin}/python -m pip install --upgrade pip
    )
    execute_process(
        COMMAND ${venv_bin}/pip install --upgrade wheel packaging setuptools build
    )
    # The requirements of the package are needed to import the modules (the
    # stubs refer to numpy...), then the stub quality gates need their tools
    _geode_pip_install("${venv_bin}" "${venv_path}"
        PACKAGES ${GEODE_STUBGEN_REQUIREMENT} ${GEODE_MYPY_REQUIREMENT}
    )
    execute_process(
        COMMAND ${venv_bin}/python -c 
"from sysconfig import get_platform
from packaging import tags
name=tags.interpreter_name()
version=tags.interpreter_version()
platform=get_platform().replace('-', '_').replace('.', '_')
print(name + version + '-' + name + version + '-' + platform)"
        OUTPUT_VARIABLE wheel_suffix
        ERROR_VARIABLE python_error
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_STRIP_TRAILING_WHITESPACE
    ) 
    message(STATUS "Wheel Suffix: ${wheel_suffix}")
    if(NOT "${python_error}" STREQUAL "")
        message(ERROR "=== Python Error === ${python_error}")
    endif()
    string(TOLOWER ${GEODE_WHEEL_NAME} wheel_name)
    string(REGEX REPLACE "-" "_" wheel_name ${wheel_name})
    set(wheel_file "${wheel_output_path}/dist/${wheel_name}-${WHEEL_VERSION}-${wheel_suffix}.whl")
    message(STATUS "Wheel file: ${wheel_file}")
    if(${GEODE_WHEEL_SUPERBUILD})
        set(wheel_config_folder "")
    else()
        set(wheel_config_folder "$<$<BOOL:${isMultiConfig}>:$<CONFIG>>")
    endif()
    set(check_stubs_script "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/check_python_stubs.py")
    set(stubtest_config "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/stubtest_mypy.ini")
    set(package_script "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/geode_python_package.py")
    # The upstream geode packages must match the C++ packages of the build
    geode_python_geode_packages(geode_packages without_local_wheel)
    # Where the downstream projects find this wheel, next to the CMake files
    set(publish_directory "${wheel_build_directory}/${CMAKE_CONFIG_DESTINATION}/python_wheels")
    set(install_publish_command)
    if(NOT ${GEODE_WHEEL_SUPERBUILD})
        _geode_python_publish_in_install("${publish_directory}" "${wheel_file}"
            "${venv_bin}/python" "${package_script}" install_publish_command
        )
    endif()
    # Dependencies of the wheel: requirements.txt (pinned by the CI, not in a
    # fresh checkout), else requirements.in
    add_custom_target(wheel
        COMMAND ${CMAKE_COMMAND} -E copy "${PROJECT_SOURCE_DIR}/README.md" "${wheel_output_path}"
        COMMAND ${venv_bin}/python "${package_script}" wheel-requirements
            "${wheel_output_path}/requirements.txt"
            "${PROJECT_SOURCE_DIR}/bindings/python/requirements.txt"
            "${PROJECT_SOURCE_DIR}/bindings/python/requirements.in"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${wheel_build_directory}/${binary_folder}/${wheel_config_folder}" "${wheel_output_directory}/${binary_folder}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${wheel_build_directory}/share" "${wheel_output_directory}/share"
        COMMAND ${venv_bin}/python "${package_script}" layout "${wheel_output_directory}" "${binary_folder}"
        COMMAND ${venv_bin}/python "${package_script}" install-geode --exclude "${GEODE_WHEEL_NAME}" ${geode_packages}
        COMMAND ${venv_bin}/pip install --no-deps .
        COMMAND ${venv_bin}/pybind11-stubgen --exit-code -o "${wheel_output_path}" "${project_name}"
        COMMAND ${venv_bin}/python "${check_stubs_script}" fix "${wheel_output_directory}"
        COMMAND ${venv_bin}/python "${check_stubs_script}" stubs "${wheel_output_directory}"
        COMMAND ${venv_bin}/python "${check_stubs_script}" stubtest
            --mypy-config-file "${stubtest_config}"
            --stubs-from "${wheel_output_directory}" ${project_name}
        COMMAND ${venv_bin}/python "${package_script}" check "${wheel_output_directory}" "${binary_folder}"
        COMMAND ${venv_bin}/python -m build
        COMMAND ${venv_bin}/python "${package_script}" publish "${wheel_file}" "${publish_directory}"
        ${install_publish_command}
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/wheel
    )
    string(CONCAT import_test "import " "${project_name}")
    add_custom_target(test-wheel
        COMMAND ${venv_bin}/pip install --force-reinstall --no-deps ${wheel_file}
        COMMAND ${venv_bin}/python -c ${import_test}
    )
endfunction()

# Every configuration of a project with Python bindings provides its
# distribution files, even without the bindings (C++ packages)
if(PROJECT_NAME AND CMAKE_CONFIG_DESTINATION AND NOT CMAKE_SCRIPT_MODE_FILE)
    _geode_python_wheel_name(_geode_wheel_name)
    if(_geode_wheel_name)
        geode_python_distribution_files("${_geode_wheel_name}")
    endif()
    unset(_geode_wheel_name)
endif()
