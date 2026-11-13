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

# Python requirements of the package: bindings/python/requirements.in lists the
# runtime dependencies (numpy...) and exists in every checkout, unlike
# requirements.txt which is generated. Empty when the project has none.
function(geode_python_requirements_file output)
    set(requirements_file "${CMAKE_SOURCE_DIR}/bindings/python/requirements.in")
    if(EXISTS "${requirements_file}")
        set(${output} "${requirements_file}" PARENT_SCOPE)
    else()
        set(${output} "" PARENT_SCOPE)
    endif()
endfunction()

# Install the requirements of the package and the given extra packages in the
# virtual environment, and fail with the command shown if pip fails.
function(_geode_pip_install venv_bin venv_path)
    cmake_parse_arguments(GEODE_PIP "" "" "PACKAGES" ${ARGN})
    geode_python_requirements_file(requirements_file)
    set(pip_command ${venv_bin}/python -m pip install)
    if(requirements_file)
        list(APPEND pip_command -r "${requirements_file}")
    endif()
    list(APPEND pip_command ${GEODE_PIP_PACKAGES})
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
    set(requirements_content "")
    if(requirements_file)
        file(READ "${requirements_file}" requirements_content)
        set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${requirements_file}")
    endif()
    string(MD5 environment_hash
        "${PYTHON_EXECUTABLE}|${requirements_content}|${GEODE_STUBGEN_REQUIREMENT}|${GEODE_MYPY_REQUIREMENT}"
    )
    set(stamp_file "${venv_path}/geode_environment_hash")
    set(stamp "")
    if(EXISTS "${stamp_file}")
        file(READ "${stamp_file}" stamp)
    endif()
    if(NOT "${stamp}" STREQUAL "${environment_hash}" OR NOT EXISTS "${venv_bin}/python${CMAKE_EXECUTABLE_SUFFIX}")
        message(STATUS "Creating the Python test environment at: ${venv_path}")
        execute_process(
            COMMAND ${PYTHON_EXECUTABLE} -m venv --clear "${venv_path}"
            RESULT_VARIABLE venv_result
        )
        if(venv_result)
            message(FATAL_ERROR
                "Cannot create the Python test environment with:\n"
                "  ${PYTHON_EXECUTABLE} -m venv --clear ${venv_path}"
            )
        endif()
        _geode_pip_install("${venv_bin}" "${venv_path}"
            PACKAGES ${GEODE_STUBGEN_REQUIREMENT} ${GEODE_MYPY_REQUIREMENT}
        )
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

function(add_geode_python_wheel)
    cmake_parse_arguments(GEODE_WHEEL
        "SUPERBUILD"
        "NAME;DESCRIPTION;LICENSE"
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
    if(NOT WHEEL_VERSION)
        set(WHEEL_VERSION "0.0.0")
    endif()
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
    add_custom_target(wheel
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/README.md" "${wheel_output_path}"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/bindings/python/requirements.txt" "${wheel_output_path}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${wheel_build_directory}/${binary_folder}/${wheel_config_folder}" "${wheel_output_directory}/${binary_folder}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${wheel_build_directory}/share" "${wheel_output_directory}/share"
        COMMAND ${venv_bin}/python "${package_script}" layout "${wheel_output_directory}" "${binary_folder}"
        COMMAND ${venv_bin}/pip install --no-deps .
        COMMAND ${venv_bin}/pybind11-stubgen --exit-code -o "${wheel_output_path}" "${project_name}"
        COMMAND ${venv_bin}/python "${check_stubs_script}" fix "${wheel_output_directory}"
        COMMAND ${venv_bin}/python "${check_stubs_script}" stubs "${wheel_output_directory}"
        COMMAND ${venv_bin}/python "${check_stubs_script}" stubtest
            --mypy-config-file "${stubtest_config}"
            --stubs-from "${wheel_output_directory}" ${project_name}
        COMMAND ${venv_bin}/python "${package_script}" check "${wheel_output_directory}" "${binary_folder}"
        COMMAND ${venv_bin}/python -m build
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/wheel
    )
    string(CONCAT import_test "import " "${project_name}")
    add_custom_target(test-wheel
        COMMAND ${venv_bin}/pip install --force-reinstall --no-deps ${wheel_file}
        COMMAND ${venv_bin}/python -c ${import_test}
    )
endfunction()
