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

"""Tests of the wheel steps of cmake/geode_python_package.py on synthetic
packages. The script path is given as first argument."""

import importlib.machinery
import os
import subprocess
import sys
import tempfile
import textwrap

SCRIPT = os.path.abspath(sys.argv[1])
SUFFIX = importlib.machinery.EXTENSION_SUFFIXES[0]


def run(arguments, pythonpath=None):
    environment = dict(os.environ)
    if pythonpath:
        environment["PYTHONPATH"] = pythonpath
    result = subprocess.run(
        [sys.executable, SCRIPT] + arguments,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        env=environment,
    )
    return result.returncode, result.stdout


def write(path, content=""):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w") as file:
        file.write(textwrap.dedent(content))


def expect(condition, message, output=""):
    if not condition:
        raise ValueError("[Test] " + message + "\n" + output)


def test_layout():
    with tempfile.TemporaryDirectory() as directory:
        package = os.path.join(directory, "opengeode")
        folder = os.path.join(package, "lib64")
        write(os.path.join(package, "__init__.py"))
        write(os.path.join(folder, "opengeode_py_mesh" + SUFFIX))
        write(os.path.join(folder, "opengeode_py_basic" + SUFFIX))
        # A shared library with an extension module suffix stays
        write(os.path.join(folder, "libOpenGeode_mesh.so"))
        write(os.path.join(folder, "libOpenGeode_mesh.so.1"))
        write(os.path.join(folder, "OpenGeode_mesh.dll"))
        # Build-tree modules and stubs of a previous build are removed
        write(os.path.join(folder, "basic.py"))
        write(os.path.join(folder, "opengeode.py"))
        write(os.path.join(folder, "__init__.pyi"))
        write(os.path.join(folder, "cmake", "OpenGeode", "stale.pyi"))
        write(os.path.join(folder, "cmake", "OpenGeode", "OpenGeodeConfig.cmake"))
        status, output = run(["layout", package, "lib64"])
        expect(status == 0, "layout should succeed", output)
        expect("Moved 2 extension module(s)" in output, "2 modules moved", output)
        for name in ("opengeode_py_mesh" + SUFFIX, "opengeode_py_basic" + SUFFIX):
            expect(os.path.isfile(os.path.join(package, name)), name + " at the root")
            expect(not os.path.exists(os.path.join(folder, name)), name + " moved")
        for name in ("libOpenGeode_mesh.so", "libOpenGeode_mesh.so.1", "OpenGeode_mesh.dll"):
            expect(os.path.isfile(os.path.join(folder, name)), name + " kept")
        expect(
            os.path.isfile(os.path.join(folder, "cmake", "OpenGeode", "OpenGeodeConfig.cmake")),
            "non-Python files kept",
        )
        for name in ("basic.py", "opengeode.py", "__init__.pyi"):
            expect(not os.path.exists(os.path.join(folder, name)), name + " removed")
        expect(
            not os.path.exists(os.path.join(folder, "cmake", "OpenGeode", "stale.pyi")),
            "nested stub removed",
        )
        expect(os.path.isfile(os.path.join(package, "__init__.py")), "__init__.py kept")
        # Nothing left to move: the binary folder was not refreshed
        status, output = run(["layout", package, "lib64"])
        expect(status == 1, "layout without module should fail", output)
        expect("no extension module opengeode_py_*" in output, "error message", output)


def test_layout_only_moves_own_modules():
    with tempfile.TemporaryDirectory() as directory:
        package = os.path.join(directory, "geode_common")
        folder = os.path.join(package, "bin")
        write(os.path.join(folder, "geode_common_py_core" + SUFFIX))
        # Modules of an upstream package installed in the same prefix
        write(os.path.join(folder, "opengeode_py_mesh" + SUFFIX))
        status, output = run(["layout", package, "bin"])
        expect(status == 0, "layout should succeed", output)
        expect(os.path.isfile(os.path.join(package, "geode_common_py_core" + SUFFIX)), "moved")
        expect(os.path.isfile(os.path.join(folder, "opengeode_py_mesh" + SUFFIX)), "upstream kept")


def make_checked_package(directory, name, init="", stubs=None, binary_files=()):
    package = os.path.join(directory, name)
    write(os.path.join(package, "__init__.py"), init)
    for stub, content in (stubs or {}).items():
        write(os.path.join(package, stub), content)
    for binary_file in binary_files:
        write(os.path.join(package, "lib", binary_file))
    os.makedirs(os.path.join(package, "lib"), exist_ok=True)
    return package


def check(directory, package):
    return run(["check", package, "lib"], pythonpath=directory)


def test_check_valid():
    with tempfile.TemporaryDirectory() as directory:
        package = make_checked_package(
            directory,
            "fakegeode",
            init="""
            from collections import OrderedDict
            class Mesh:
                pass
            Mesh.__module__ = "fakegeode.fakegeode_py_mesh"
            class Pure:
                pass
            """,
            stubs={
                "fakegeode_py_mesh.pyi": """
                import numpy.lib.stride_tricks
                import opengeode.opengeode_py_mesh
                def f(mesh: opengeode.opengeode_py_mesh.TriangulatedSurface3D) -> None: ...
                """,
            },
            binary_files=["libfakegeode.so", "fakegeode.dll"],
        )
        status, output = check(directory, package)
        expect(status == 0, "a valid package should pass", output)
        expect("0 error(s)" in output, "error count", output)


def test_check_stub_through_binary_folder():
    for reference in (
        "opengeode.lib64.opengeode_py_mesh.TriangulatedSurface3D",
        "opengeode.lib.opengeode_py_mesh.TriangulatedSurface3D",
        "geode_common.bin.geode_common_py_core.Thing",
        "fakegeode.lib32.fakegeode_py_mesh.Mesh",
    ):
        with tempfile.TemporaryDirectory() as directory:
            package = make_checked_package(
                directory,
                "fakegeode",
                stubs={"sub/module.pyi": "def f(x: %s) -> None: ...\n" % reference},
            )
            status, output = check(directory, package)
            expect(status == 1, "should fail on " + reference, output)
            expect(
                "module reached through a binary folder: " + reference.rsplit(".", 1)[0]
                in output,
                "message for " + reference,
                output,
            )
            expect("module.pyi:1:" in output, "location", output)


def test_check_module_in_binary_folder():
    for name in ("fakegeode_py_mesh" + SUFFIX, "basic.py", "__init__.pyi"):
        with tempfile.TemporaryDirectory() as directory:
            package = make_checked_package(directory, "fakegeode", binary_files=[name])
            status, output = check(directory, package)
            expect(status == 1, "should fail on " + name, output)
            expect("Python module in the binary folder" in output, "message", output)


def test_check_type_in_subfolder_module():
    with tempfile.TemporaryDirectory() as directory:
        package = make_checked_package(
            directory,
            "fakegeode",
            init="""
            class Mesh:
                pass
            Mesh.__module__ = "fakegeode.lib.fakegeode_py_mesh"
            """,
        )
        status, output = check(directory, package)
        expect(status == 1, "should fail on a type of a subfolder module", output)
        expect(
            "fakegeode.Mesh: defined in fakegeode.lib.fakegeode_py_mesh" in output,
            "message",
            output,
        )


if __name__ == "__main__":
    test_layout()
    test_layout_only_moves_own_modules()
    test_check_valid()
    test_check_stub_through_binary_folder()
    test_check_module_in_binary_folder()
    test_check_type_in_subfolder_module()
