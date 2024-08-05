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
    set_tests_properties(${test_name} PROPERTIES TIMEOUT 300) 
    _find_dependency_directories(directories projects ${GEODE_TEST_DEPENDENCIES})
    if(WIN32)
        list(JOIN directories "\\;" directories)
        set_tests_properties(${test_name}
            PROPERTIES
                ENVIRONMENT "Path=${directories}\\;$ENV{Path};PYTHONPATH=${directories}\\;$ENV{PYTHONPATH}"
        )
    else()
        list(JOIN directories ":" directories)
        set_tests_properties(${test_name}
            PROPERTIES
                ENVIRONMENT "LD_LIBRARY_PATH=${directories}:$ENV{LD_LIBRARY_PATH};PYTHONPATH=${directories}:$ENV{PYTHONPATH}"
        )
    endif()
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
    set(header "# Copyright (c) 2019 - 2024 Geode-solutions\n\n")
    file(WRITE ${wheel_init} "#${header}")
    file(WRITE ${wheel_import} "${header}")
    if(WIN32)
        file(APPEND ${wheel_init} "import os, pathlib\n")
        file(APPEND ${wheel_init} "os.add_dll_directory(pathlib.Path(__file__).parent.resolve().joinpath('bin'))\n\n")
    endif()
    foreach(module ${GEODE_WHEEL_MODULES})
        string(REPLACE ".py" "" module_name "${module}")
        file(APPEND ${wheel_init} "from .${module_name} import *\n")
        file(APPEND ${wheel_import} "from ${module_name} import *\n")
        configure_file("${module}" "${wheel_build_directory}/${binary_folder}/${module}" COPYONLY)
        file(READ "${module}" FILE_CONTENTS)
        string(REPLACE "from ${project_name}" "from .${binary_folder}.${project_name}" FILE_CONTENTS "${FILE_CONTENTS}")
        file(WRITE "${wheel_output_directory}/${module}" ${FILE_CONTENTS})
    endforeach()
    if(NOT WHEEL_VERSION)
        set(WHEEL_VERSION "0.0.0")
    endif()
    configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/setup.py.in" "${wheel_output_directory}/../setup.py")
    configure_file("${CMAKE_CURRENT_FUNCTION_LIST_DIR}/pyproject.toml.in" "${wheel_output_directory}/../pyproject.toml")
    file(MAKE_DIRECTORY "${wheel_build_directory}/share")
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -m pip install --upgrade wheel setuptools build)
    execute_process(
        COMMAND ${PYTHON_EXECUTABLE} -c 
"from sysconfig import get_platform
from wheel.vendored.packaging import tags
name=tags.interpreter_name()
version=tags.interpreter_version()
platform=get_platform().replace("-", "_").replace(".", "_")
print(name + version + '-' + name + version + '-' + platform)"
        OUTPUT_VARIABLE wheel_sufix
        OUTPUT_STRIP_TRAILING_WHITESPACE
    ) 
    string(REGEX REPLACE "-" "_" wheel_name ${GEODE_WHEEL_NAME})
    set(wheel_file "${wheel_output_path}/dist/${wheel_name}-${WHEEL_VERSION}-${wheel_sufix}.whl")
    message(STATUS "Wheel file: ${wheel_file}")
    if(${GEODE_WHEEL_SUPERBUILD})
        set(wheel_config_folder "")
    else()
        set(wheel_config_folder "$<$<BOOL:${isMultiConfig}>:$<CONFIG>>")
    endif()
    add_custom_target(wheel
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/README.md" "${wheel_output_path}"
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/bindings/python/requirements.txt" "${wheel_output_path}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${wheel_build_directory}/${binary_folder}/${wheel_config_folder}" "${wheel_output_directory}/${binary_folder}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${wheel_build_directory}/share" "${wheel_output_directory}/share"
        COMMAND ${CMAKE_COMMAND} -E remove "${wheel_output_directory}/${binary_folder}/*.py"
        COMMAND ${PYTHON_EXECUTABLE} -m build
        WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/wheel
    )
    string(CONCAT import_test "import " "${project_name}")
    add_custom_target(test-wheel
        COMMAND ${PYTHON_EXECUTABLE} -m pip install --no-deps ${wheel_file}
        COMMAND ${PYTHON_EXECUTABLE} -c ${import_test}
    )
endfunction()
