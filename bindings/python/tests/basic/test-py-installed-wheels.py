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

"""The downstream projects look for the published wheel of a geode project
next to the CMake files they found (<Package>_DIR/python_wheels), usually the
install tree. Built without superbuild, the wheel target publishes it in the
build tree, so the published wheels must be installed next to the installed
CMake files, and the wheel target must also publish the wheel there when the
project is already installed. They are installed with their own component
(python_wheels), out of the C++ packages (public and private components).
Checked on a synthetic project configured with
cmake/PythonTargets.cmake (_geode_python_publish_in_install, used by
add_geode_python_wheel without SUPERBUILD) and installed in a temporary prefix.
Arguments: the cmake executable, cmake/PythonTargets.cmake and the CMake
generator."""

import os
import subprocess
import sys
import tempfile
import textwrap

CMAKE, PYTHON_TARGETS, GENERATOR = sys.argv[1:4]
PROJECT = """cmake_minimum_required(VERSION 3.15)
project(Down NONE)
set(CMAKE_CONFIG_DESTINATION "lib/cmake/Down")
include("{targets}")
set(publish_directory "${{PROJECT_BINARY_DIR}}/${{CMAKE_CONFIG_DESTINATION}}/python_wheels")
_geode_python_publish_in_install("${{publish_directory}}" "/dist/down-0.0.0-py3-none-any.whl"
    "/venv/bin/python" "/cmake/geode_python_package.py" command
)
message(STATUS "PUBLISH_COMMAND=${{command}}")
"""


def expect(condition, message, output=""):
    if not condition:
        raise ValueError("[Test] " + message + "\n" + output)


def cmake_path(path):
    return path.replace("\\", "/")


def run(arguments):
    result = subprocess.run(
        [CMAKE] + arguments, stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True
    )
    expect(result.returncode == 0, "cmake %s failed" % arguments, result.stdout)
    return result.stdout


def wheels(directory):
    if not os.path.isdir(directory):
        return []
    return sorted(name for name in os.listdir(directory) if name.endswith(".whl"))


def main():
    with tempfile.TemporaryDirectory() as directory:
        source = os.path.join(directory, "source")
        build = os.path.join(directory, "build")
        prefix = os.path.join(directory, "install")
        os.makedirs(source)
        with open(os.path.join(source, "CMakeLists.txt"), "w") as file:
            file.write(PROJECT.format(targets=cmake_path(os.path.abspath(PYTHON_TARGETS))))
        output = run(
            ["-S", source, "-B", build, "-G", GENERATOR, "-DCMAKE_INSTALL_PREFIX=" + cmake_path(prefix)]
        )
        published = os.path.join(build, "lib", "cmake", "Down", "python_wheels")
        expect(os.path.isdir(published), "build publish directory created", output)
        command = [
            line.split("=", 1)[1] for line in output.splitlines() if "PUBLISH_COMMAND=" in line
        ]
        expect(len(command) == 1, "publish command printed", output)
        expected = ";".join(
            [
                "COMMAND",
                "/venv/bin/python",
                "/cmake/geode_python_package.py",
                "publish",
                "--if-installed",
                "/dist/down-0.0.0-py3-none-any.whl",
                cmake_path(prefix) + "/lib/cmake/Down/python_wheels",
            ]
        )
        expect(command[0] == expected, "publish command: %s" % command[0], output)
        installed = os.path.join(prefix, "lib", "cmake", "Down", "python_wheels")
        # Nothing published yet: the directory is installed empty
        output = run(["--install", build])
        expect(os.path.isdir(installed), "python_wheels installed", output)
        expect(not wheels(installed), "no wheel installed", output)
        # A wheel published in the build tree is installed next to the CMake files
        wheel = "down-0.0.0-cp313-cp313-linux_x86_64.whl"
        open(os.path.join(published, wheel), "wb").close()
        output = run(["--install", build])
        expect(wheels(installed) == [wheel], "installed: %s" % wheels(installed), output)
        # Also with another prefix given at install time
        other = os.path.join(directory, "other")
        output = run(["--install", build, "--prefix", other])
        expect(
            wheels(os.path.join(other, "lib", "cmake", "Down", "python_wheels")) == [wheel],
            "installed in the other prefix",
            output,
        )
        # The wheels have their own component: the C++ packages (cpack of the
        # public and private components) do not carry them
        for component in ("public", "private"):
            package = os.path.join(directory, component)
            output = run(["--install", build, "--prefix", package, "--component", component])
            expect(
                not os.path.exists(os.path.join(package, "lib", "cmake", "Down", "python_wheels")),
                "python_wheels installed with the %s component" % component,
                output,
            )
        alone = os.path.join(directory, "alone")
        output = run(["--install", build, "--prefix", alone, "--component", "python_wheels"])
        expect(
            wheels(os.path.join(alone, "lib", "cmake", "Down", "python_wheels")) == [wheel],
            "python_wheels component",
            output,
        )

if __name__ == "__main__":
    main()
