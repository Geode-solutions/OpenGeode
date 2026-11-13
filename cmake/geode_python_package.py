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
    geode_python_package.py requirements --output <file> --geode-output <file>
                            <requirements file>...
    geode_python_package.py install-local [--exclude <distribution>]
                            [--require <distribution>]... <wheel directory>...
    geode_python_package.py publish <wheel> <wheel directory>

layout: moves the extension modules of the package (<package>_py_*) copied in the
binary folder (lib, lib64 or bin, with the shared libraries) to the root of the
package, so that their public path (opengeode.opengeode_py_mesh...) is the same
on every platform, and removes the Python files left in the binary folder by a
previous build (stubs and build-tree modules) and the published wheels.

check: once the package is installed and its stubs are generated, fails if an
extension module or a stub is still in the binary folder, if a stub refers to an
extension module through a binary folder (this package or another geode
package), or if a public type of the package is not defined in a module at the
root of the package.

requirements: merges the requirements files of the project and of the geode
projects it depends on into one file for pip, without the geode packages
(OpenGeode-core, Geode-Common...): they are built by the local build chain, the
released ones on PyPI may not match it. Their names are written to the
geode output file, one per line.

install-local: installs, without their dependencies and from the given
directories only (never from PyPI), the latest local wheel of every geode
package found there, except the excluded one (the package being built), and
fails if a required package has no local wheel.

publish: copies the wheel in the directory where the downstream projects look
for it (install-local), replacing the previous wheel of the same package for
the same platform.
"""

import argparse
import glob
import importlib
import importlib.machinery
import inspect
import os
import re
import shutil
import subprocess
import sys

# A geode extension module (<package>_py_<name>) reached through a subfolder of
# its package: opengeode.lib64.opengeode_py_mesh
SUBFOLDER_MODULE = re.compile(r"\b[A-Za-z_]\w*\.(?:lib|lib32|lib64|bin)\.[A-Za-z_]\w*_py_\w+")


# PEP 508 name at the start of a requirement line
REQUIREMENT_NAME = re.compile(r"^\s*([A-Za-z0-9](?:[A-Za-z0-9._-]*[A-Za-z0-9])?)")
# A comment of a requirements file (a # in a URL is not one)
COMMENT = re.compile(r"(?:^|\s)#.*")
# Packages of the geode build chain (normalized names)
GEODE_DISTRIBUTION = re.compile(r"^(?:open)?geode-")


def normalize(name):
    """PEP 503 normalized name of a distribution."""
    return re.sub(r"[-_.]+", "-", name).lower()


def is_geode_distribution(name):
    return GEODE_DISTRIBUTION.match(normalize(name)) is not None


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
        # Wheels published next to the CMake files of the project (publish)
        for name in [d for d in directories if d == "python_wheels"]:
            shutil.rmtree(os.path.join(root, name))
            directories.remove(name)
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


def run_requirements(files, output, geode_output):
    lines = []
    geode = []
    for path in files:
        with open(path, encoding="utf-8") as requirements:
            for line in requirements:
                line = COMMENT.sub("", line).strip()
                if not line:
                    continue
                match = REQUIREMENT_NAME.match(line)
                if match and is_geode_distribution(match.group(1)):
                    name = normalize(match.group(1))
                    if name not in geode:
                        geode.append(name)
                elif line not in lines:
                    lines.append(line)
    for path, content in ((output, lines), (geode_output, geode)):
        text = "".join(item + "\n" for item in content)
        previous = None
        if os.path.isfile(path):
            with open(path, encoding="utf-8") as file:
                previous = file.read()
        if previous != text:
            with open(path, "w", encoding="utf-8") as file:
                file.write(text)
    return 0


def wheel_distribution(path):
    """Normalized distribution name of a wheel file (name-version-...whl)."""
    return normalize(os.path.basename(path).split("-", 1)[0])


def wheel_tags(path):
    """The wheel file name without its distribution and version."""
    return os.path.basename(path).split("-", 2)[2]


def run_install_local(directories, exclude, required):
    excluded = normalize(exclude) if exclude else None
    found = {}
    searched = []
    for directory in directories:
        if not os.path.isdir(directory):
            continue
        searched.append(directory)
        for wheel in glob.glob(os.path.join(directory, "*.whl")):
            name = wheel_distribution(wheel)
            if name != excluded and is_geode_distribution(name):
                found.setdefault(name, []).append(wheel)
    missing = sorted({normalize(name) for name in required} - set(found))
    if missing:
        print(
            "error: no local wheel of %s in %s: build the wheel target of the "
            "geode project providing it first (the released package is not "
            "installed, it may not match the local build chain)"
            % (", ".join(missing), ", ".join(searched) or "any wheel directory")
        )
        return 1
    if not found:
        print("No local geode wheel to install")
        return 0
    # pip chooses the wheel compatible with this Python among the local ones
    command = [sys.executable, "-m", "pip", "install", "--no-index", "--no-deps"]
    command.append("--force-reinstall")
    for directory in searched:
        command.extend(["--find-links", directory])
    command.extend(sorted(found))
    print("Installing the local geode wheels: " + ", ".join(sorted(found)))
    return subprocess.call(command)


def run_publish(wheel, directory):
    os.makedirs(directory, exist_ok=True)
    name = wheel_distribution(wheel)
    tags = wheel_tags(wheel)
    for previous in glob.glob(os.path.join(directory, "*.whl")):
        if wheel_distribution(previous) == name and wheel_tags(previous) == tags:
            os.remove(previous)
    shutil.copy2(wheel, directory)
    print("Published %s in %s" % (os.path.basename(wheel), directory))
    return 0


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = parser.add_subparsers(dest="command", required=True)
    for command in ("layout", "check"):
        command_parser = sub.add_parser(command)
        command_parser.add_argument("package")
        command_parser.add_argument("binary_folder")
    requirements = sub.add_parser("requirements")
    requirements.add_argument("files", nargs="*")
    requirements.add_argument("--output", required=True)
    requirements.add_argument("--geode-output", required=True)
    install_local = sub.add_parser("install-local")
    install_local.add_argument("directories", nargs="*")
    install_local.add_argument("--exclude")
    install_local.add_argument("--require", action="append", default=[])
    publish = sub.add_parser("publish")
    publish.add_argument("wheel")
    publish.add_argument("directory")
    arguments = parser.parse_args()
    if arguments.command == "layout":
        return run_layout(arguments.package, arguments.binary_folder)
    if arguments.command == "check":
        return run_check(arguments.package, arguments.binary_folder)
    if arguments.command == "requirements":
        return run_requirements(
            arguments.files, arguments.output, arguments.geode_output
        )
    if arguments.command == "install-local":
        return run_install_local(
            arguments.directories, arguments.exclude, arguments.require
        )
    return run_publish(arguments.wheel, arguments.directory)


if __name__ == "__main__":
    sys.exit(main())
