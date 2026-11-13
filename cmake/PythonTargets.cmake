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
    set_target_properties(${GEODE_BINDING_NAME}
        PROPERTIES
            OUTPUT_NAME ${project_name}_${GEODE_BINDING_NAME}
            CXX_VISIBILITY_PRESET "default"
            FOLDER "Bindings/Python"
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
    add_test(NAME ${test_name}
        COMMAND ${PYTHON_EXECUTABLE} ${absolute_path} ${GEODE_TEST_ARGUMENTS}
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

# Quality gates on the stubs of the binding modules, run by CTest: generate the
# stubs of the compiled modules with pybind11-stubgen, then check them with
# check_python_stubs.py (no unnamed argN parameter, no unresolved or raw C++
# type, mypy.stubtest unless NO_STUBTEST). The wheel target runs the same gates
# on the wheel, with stubtest on the whole package.
# Needs pybind11-stubgen and mypy in PYTHON_EXECUTABLE: skipped otherwise.
function(add_geode_python_stub_test)
    cmake_parse_arguments(GEODE_STUB_TEST
        "NO_STUBTEST"
        "NAME"
        "MODULES;DEPENDENCIES"
        ${ARGN}
    )
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -c "import pybind11_stubgen, mypy.stubtest"
        RESULT_VARIABLE stub_tools_missing
        OUTPUT_QUIET
        ERROR_QUIET
    )
    if(stub_tools_missing)
        message(STATUS
            "pybind11-stubgen or mypy not found in ${PYTHON_EXECUTABLE}: "
            "stub quality gates not tested"
        )
        return()
    endif()
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
    if(WIN32)
        file(APPEND ${wheel_init} "import os, pathlib\n")
        file(APPEND ${wheel_init} "os.add_dll_directory(pathlib.Path(__file__).parent.resolve().joinpath('bin'))\n\n")
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
        string(REPLACE "from ${project_name}" "from .${binary_folder}.${project_name}" FILE_CONTENTS "${FILE_CONTENTS}")
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
        COMMAND ${venv_bin}/pip install --upgrade wheel packaging setuptools build pybind11-stubgen mypy
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
    add_custom_target(wheel
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/README.md" "${wheel_output_path}"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/bindings/python/requirements.txt" "${wheel_output_path}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${wheel_build_directory}/${binary_folder}/${wheel_config_folder}" "${wheel_output_directory}/${binary_folder}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${wheel_build_directory}/share" "${wheel_output_directory}/share"
        COMMAND ${CMAKE_COMMAND} -E remove "${wheel_output_directory}/${binary_folder}/*.py"
        COMMAND ${venv_bin}/pip install --no-deps .
        COMMAND ${venv_bin}/pybind11-stubgen --exit-code -o "${wheel_output_path}" "${project_name}"
        COMMAND ${venv_bin}/python "${check_stubs_script}" stubs "${wheel_output_directory}"
        COMMAND ${venv_bin}/python "${check_stubs_script}" stubtest
            --mypy-config-file "${stubtest_config}"
            --stubs-from "${wheel_output_directory}" ${project_name}
        COMMAND ${venv_bin}/python -m build
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/wheel
    )
    string(CONCAT import_test "import " "${project_name}")
    add_custom_target(test-wheel
        COMMAND ${venv_bin}/pip install --force-reinstall --no-deps ${wheel_file}
        COMMAND ${venv_bin}/python -c ${import_test}
    )
endfunction()
