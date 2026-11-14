# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2026 Geode-solutions
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.


"""The reusable helpers of the bindings are installed for the bindings of the
downstream projects, in include/geode/bindings/python: the install script of
bindings/python/helpers (which lists the installed helpers) is run in a
temporary prefix, and every relative include of an installed helper must be
installed too. Then each helper is compiled alone from that prefix, as a
downstream binding includes it (<geode/bindings/python/...>, without the
OpenGeode sources of the bindings), in a small CMake project using the compile
flags of a downstream binding.
Arguments: the cmake executable, the cmake_install.cmake of
bindings/python/helpers, the source directory of the helpers
(bindings/python/src), the file of the compile flags and the CMake generator."""

import os
import re
import subprocess
import sys
import tempfile

CMAKE, SCRIPT, SOURCE, FLAGS, GENERATOR = sys.argv[1:6]
HELPERS = [
    "common.hpp",
    "numpy_access.hpp",
    "basic/factory.hpp",
    "basic/mapping.hpp",
    "mesh/index_check.hpp",
    "model/model_vertex_check.hpp",
    "model/model_mapping.hpp",
]
# Each translation unit includes one helper first, then uses it
UNITS = {
    "common.hpp": """
#include <geode/basic/common.hpp>
namespace geode
{
    void define( pybind11::module_& module )
    {
        define_library< OpenGeodeBasicLibrary >( module, "A", "doc", "doc" );
    }
}
""",
    "numpy_access.hpp": "",
    "basic/factory.hpp": """
#include <geode/mesh/io/triangulated_surface_input.hpp>
namespace geode
{
    void define( pybind11::module& module )
    {
        PYTHON_FACTORY_CLASS( TriangulatedSurfaceInputFactory3D );
    }
}
""",
    "basic/mapping.hpp": """
#include <geode/mesh/core/mesh_element.hpp>
namespace geode
{
    void define( pybind11::module& module )
    {
        define_generic_mapping< MeshVertex >( module, "A", "doc" );
        define_bijective_mapping< MeshVertex, index_t >( module, "B", "doc" );
    }
}
""",
    "mesh/index_check.hpp": "",
    "model/model_vertex_check.hpp": "",
    "model/model_mapping.hpp": """
namespace geode
{
    void define( pybind11::module& module )
    {
        define_model_mapping< GenericMapping< MeshVertex > >(
            module, "A", "doc", "B" );
    }
}
""",
}
# Every helper brings the stl casters (list_creators of PYTHON_FACTORY_CLASS
# returns a list of str, ...): without them, a translation unit including only
# that helper binds std::vector< std::string > as an unregistered class (wrong
# return value at run time, and an ODR violation with the units including
# pybind11/stl.h)
STL_CHECK = """
static_assert( !std::is_base_of_v< pybind11::detail::type_caster_generic,
                   pybind11::detail::make_caster< std::vector< std::string > > >,
    "the helper must provide the stl casters" );
"""
PROJECT = """cmake_minimum_required(VERSION 3.15)
include("{flags}")
project(InstalledHelpersCheck CXX)
add_library(check OBJECT {sources})
target_include_directories(check PRIVATE "{include}" ${{CHECK_INCLUDES}})
target_compile_definitions(check PRIVATE ${{CHECK_DEFINITIONS}})
"""
RELATIVE_INCLUDE = re.compile(r'^\s*#include\s+"([^"]+)"', re.M)


def expect(condition, message):
    if not condition:
        raise ValueError("[Test] " + message)


def cmake_path(path):
    return path.replace("\\", "/")


def compile_helpers(prefix):
    project = os.path.join(prefix, "check")
    os.makedirs(project)
    sources = []
    for index, (helper, use) in enumerate(UNITS.items()):
        source = "unit%d.cpp" % index
        with open(os.path.join(project, source), "w") as file:
            file.write(
                "#include <geode/bindings/python/%s>\n%s%s" % (helper, STL_CHECK, use)
            )
        sources.append(source)
    with open(os.path.join(project, "CMakeLists.txt"), "w") as file:
        file.write(
            PROJECT.format(
                flags=cmake_path(FLAGS),
                sources=" ".join(sources),
                include=cmake_path(os.path.join(prefix, "include")),
            )
        )
    build = os.path.join(prefix, "check_build")
    for command in (
        [CMAKE, "-S", project, "-B", build, "-G", GENERATOR],
        [CMAKE, "--build", build, "--config", "Release"],
    ):
        result = subprocess.run(
            command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True
        )
        expect(
            result.returncode == 0,
            "the installed helpers do not compile alone:\n" + result.stdout[-6000:],
        )


if __name__ == "__main__":
    with tempfile.TemporaryDirectory() as prefix:
        result = subprocess.run(
            [
                CMAKE,
                "-DCMAKE_INSTALL_PREFIX=" + prefix,
                "-DCMAKE_INSTALL_COMPONENT=public",
                "-P",
                SCRIPT,
            ],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
        )
        expect(result.returncode == 0, "install script failed:\n" + result.stdout)
        installed = os.path.join(prefix, "include", "geode", "bindings", "python")
        for helper in HELPERS:
            path = os.path.join(installed, helper)
            expect(os.path.isfile(path), helper + " is not installed")
            with open(path) as file, open(os.path.join(SOURCE, helper)) as source:
                content = file.read()
                expect(content == source.read(), helper + " differs from its source")
            expect("#pragma once" in content, helper + " has no include guard")
            for include in RELATIVE_INCLUDE.findall(content):
                target = os.path.normpath(os.path.join(os.path.dirname(path), include))
                expect(
                    os.path.isfile(target),
                    "%s includes %s, which is not installed" % (helper, include),
                )
        compile_helpers(prefix)
    print("The binding helpers are installed and compile from the install")
