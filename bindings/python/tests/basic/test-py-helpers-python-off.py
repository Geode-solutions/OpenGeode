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


"""The binding helpers are installed by every configure, whatever the value of
OPENGEODE_WITH_PYTHON (the C++-only packages feed the Python builds of the
downstream projects): the helpers directory is added outside of the Python
option, and it configures and installs alone, without pybind11 nor OpenGeode.
Arguments: the cmake executable, the helpers directory, the source directory
of the helpers, cmake/OpenGeode.cmake and the CMake generator."""

import os
import re
import subprocess
import sys
import tempfile

CMAKE, HELPERS_DIR, SOURCE, OPENGEODE_CMAKE, GENERATOR = sys.argv[1:6]
HELPERS = [
    "common.hpp",
    "numpy_access.hpp",
    "basic/factory.hpp",
    "basic/mapping.hpp",
    "mesh/index_check.hpp",
    "model/model_vertex_check.hpp",
    "model/model_mapping.hpp",
]


def expect(condition, message):
    if not condition:
        raise ValueError("[Test] " + message)


def run(command):
    result = subprocess.run(
        command, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True
    )
    expect(result.returncode == 0, " ".join(command) + " failed:\n" + result.stdout)


def check_added_outside_python_option():
    with open(OPENGEODE_CMAKE) as file:
        lines = [line.split("#")[0].strip() for line in file]
    depth = 0
    added = False
    for line in lines:
        if re.match(r"if\s*\(", line, re.I):
            depth += 1
        elif re.match(r"endif\s*\(", line, re.I):
            depth -= 1
        elif re.match(r"add_subdirectory\s*\(\s*bindings/python/helpers", line):
            expect(depth == 0, "the helpers are added under a condition")
            added = True
    expect(added, "the helpers directory is not added by OpenGeode.cmake")


if __name__ == "__main__":
    check_added_outside_python_option()
    with tempfile.TemporaryDirectory() as directory:
        build = os.path.join(directory, "build")
        prefix = os.path.join(directory, "prefix")
        run([CMAKE, "-S", HELPERS_DIR, "-B", build, "-G", GENERATOR])
        run(
            [CMAKE, "--install", build, "--prefix", prefix, "--component", "public"]
        )
        installed = os.path.join(prefix, "include", "geode", "bindings", "python")
        for helper in HELPERS:
            path = os.path.join(installed, helper)
            expect(os.path.isfile(path), helper + " is not installed")
            with open(path) as file, open(os.path.join(SOURCE, helper)) as source:
                expect(file.read() == source.read(), helper + " differs from source")
    print("The binding helpers are installed without the Python bindings")
