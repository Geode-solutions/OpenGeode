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


"""Steps of the wheel target of add_geode_python_wheel (cmake/PythonTargets.cmake).

    geode_python_package.py layout <package directory> <binary folder>
    geode_python_package.py check <package directory> <binary folder>

layout: moves the extension modules of the package (<package>_py_*) copied in the
binary folder (lib, lib64 or bin, with the shared libraries) to the root of the
package, so that their public path (opengeode.opengeode_py_mesh...) is the same
on every platform, and removes the Python files left in the binary folder by a
previous build (stubs and build-tree modules).

check: once the package is installed and its stubs are generated, fails if an
extension module or a stub is still in the binary folder, if a stub refers to an
extension module through a binary folder (this package or another geode
package), or if a public type of the package is not defined in a module at the
root of the package.
"""

import argparse
import importlib
import importlib.machinery
import inspect
import os
import re
import shutil
import sys

# A geode extension module (<package>_py_<name>) reached through a subfolder of
# its package: opengeode.lib64.opengeode_py_mesh
SUBFOLDER_MODULE = re.compile(r"\b[A-Za-z_]\w*\.(?:lib|lib32|lib64|bin)\.[A-Za-z_]\w*_py_\w+")


def package_name(package):
    return os.path.basename(os.path.normpath(package))


def is_extension_module(package, name):
    """An extension module of the package (<package>_py_<name>): the shared
    libraries may have an extension module suffix too (.so)."""
    return name.startswith(package_name(package) + "_py_") and any(
        name.endswith(suffix) for suffix in importlib.machinery.EXTENSION_SUFFIXES
    )


def run_layout(package, binary_folder):
    folder = os.path.join(package, binary_folder)
    moved = 0
    for name in sorted(os.listdir(folder)):
        path = os.path.join(folder, name)
        if not os.path.isfile(path):
            continue
        if is_extension_module(package, name):
            os.replace(path, os.path.join(package, name))
            moved += 1
        elif name.endswith((".py", ".pyi")):
            os.remove(path)
    for root, directories, files in os.walk(folder):
        for name in files:
            if name.endswith((".py", ".pyi")):
                os.remove(os.path.join(root, name))
    if moved == 0:
        print("error: no extension module %s_py_* in %s" % (package_name(package), folder))
        return 1
    print("Moved %d extension module(s) to %s" % (moved, package))
    return 0


def run_check(package, binary_folder):
    errors = []
    folder = os.path.join(package, binary_folder)
    for root, directories, files in os.walk(folder):
        for name in files:
            if is_extension_module(package, name) or name.endswith((".py", ".pyi")):
                errors.append(
                    "%s: Python module in the binary folder" % os.path.join(root, name)
                )
    for root, directories, files in os.walk(package):
        for name in files:
            if not name.endswith(".pyi"):
                continue
            path = os.path.join(root, name)
            with open(path, encoding="utf-8") as stub:
                for number, line in enumerate(stub, 1):
                    match = SUBFOLDER_MODULE.search(line)
                    if match:
                        errors.append(
                            "%s:%d: module reached through a binary folder: %s"
                            % (path, number, match.group(0))
                        )
    name = package_name(package)
    module = importlib.import_module(name)
    for attribute, value in sorted(vars(module).items()):
        if not inspect.isclass(value):
            continue
        # Types of other packages (numpy, upstream geode packages) are skipped
        defined_in = value.__module__
        if defined_in.startswith(name + ".") and defined_in.count(".") != 1:
            errors.append(
                "%s.%s: defined in %s, not in a module at the package root"
                % (name, attribute, defined_in)
            )
    for error in errors:
        print("error: " + error)
    print("Checked the layout of %s: %d error(s)" % (package, len(errors)))
    return 1 if errors else 0


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = parser.add_subparsers(dest="command", required=True)
    for command in ("layout", "check"):
        command_parser = sub.add_parser(command)
        command_parser.add_argument("package")
        command_parser.add_argument("binary_folder")
    arguments = parser.parse_args()
    if arguments.command == "layout":
        return run_layout(arguments.package, arguments.binary_folder)
    return run_check(arguments.package, arguments.binary_folder)


if __name__ == "__main__":
    sys.exit(main())
