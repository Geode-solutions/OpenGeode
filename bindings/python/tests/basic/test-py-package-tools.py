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


def requirements(*contents):
    with tempfile.TemporaryDirectory() as directory:
        files = []
        for index, content in enumerate(contents):
            path = os.path.join(directory, "requirements%d.in" % index)
            write(path, content)
            files.append(path)
        output = os.path.join(directory, "out", "requirements.txt")
        geode_output = os.path.join(directory, "out", "geode.txt")
        os.makedirs(os.path.dirname(output))
        status, log = run(
            ["requirements", "--output", output, "--geode-output", geode_output]
            + files
        )
        expect(status == 0, "requirements should succeed", log)
        with open(output) as file:
            kept = file.read().splitlines()
        with open(geode_output) as file:
            geode = file.read().splitlines()
        # Not rewritten when unchanged (no reconfiguration loop)
        stamp = os.stat(output).st_mtime_ns
        status, log = run(
            ["requirements", "--output", output, "--geode-output", geode_output]
            + files
        )
        expect(os.stat(output).st_mtime_ns == stamp, "unchanged output rewritten", log)
        return kept, geode


def test_requirements():
    # OpenGeode runtime requirements installed next to the CMake files, then
    # the requirements of a downstream project
    kept, geode = requirements(
        """
        # runtime requirements
        numpy>=2
        """,
        """
        OpenGeode-core
        opengeode_io>=1.2 ; python_version >= "3.10"
        Geode.Common[extra]==3.0  # comment
        geode-simplex
        numpy>=2
        pygeode-like
        mygeode-tools==1
        requests @ https://example.com/requests.whl#sha256=abc
        """,
    )
    expect(
        kept
        == [
            "numpy>=2",
            "pygeode-like",
            "mygeode-tools==1",
            "requests @ https://example.com/requests.whl#sha256=abc",
        ],
        "kept requirements: %s" % kept,
    )
    expect(
        geode == ["opengeode-core", "opengeode-io", "geode-common", "geode-simplex"],
        "geode requirements: %s" % geode,
    )


def test_requirements_empty():
    kept, geode = requirements("", "OpenGeode-core\n")
    expect(kept == [] and geode == ["opengeode-core"], "%s %s" % (kept, geode))
    kept, geode = requirements()
    expect(kept == [] and geode == [], "no file: %s %s" % (kept, geode))


def make_wheel(directory, distribution, module, tags="py3-none-any"):
    """A minimal valid wheel of a pure-Python module."""
    import zipfile

    name = distribution.replace("-", "_")
    info = "%s-0.0.0.dist-info" % name
    path = os.path.join(directory, "%s-0.0.0-%s.whl" % (name, tags))
    os.makedirs(directory, exist_ok=True)
    files = {
        module + ".py": "VALUE = %r\n" % directory,
        info + "/METADATA": "Metadata-Version: 2.1\nName: %s\nVersion: 0.0.0\n"
        % distribution,
        info + "/WHEEL": "Wheel-Version: 1.0\nGenerator: test\nRoot-Is-Purelib: true\n"
        "Tag: %s\n" % tags,
    }
    with zipfile.ZipFile(path, "w") as archive:
        for file, content in files.items():
            archive.writestr(file, content)
        archive.writestr(
            info + "/RECORD",
            "".join("%s,,\n" % file for file in list(files) + [info + "/RECORD"]),
        )
    return path


def install_local(arguments, target):
    environment = dict(os.environ)
    # Installed in a directory, not in the Python running the tests
    environment["PIP_TARGET"] = target
    environment["PIP_DISABLE_PIP_VERSION_CHECK"] = "1"
    result = subprocess.run(
        [sys.executable, SCRIPT, "install-local"] + arguments,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        env=environment,
    )
    return result.returncode, result.stdout


def test_install_local():
    with tempfile.TemporaryDirectory() as directory:
        upstream = os.path.join(directory, "OpenGeode", "python_wheels")
        common = os.path.join(directory, "GeodeCommon", "python_wheels")
        own = os.path.join(directory, "Self", "python_wheels")
        make_wheel(upstream, "OpenGeode-core", "fake_opengeode")
        make_wheel(common, "Geode-Common", "fake_geode_common")
        # Not a geode package: never installed from these directories
        make_wheel(common, "numpy", "fake_numpy")
        # The package being built
        make_wheel(own, "Geode-Self", "fake_geode_self")
        target = os.path.join(directory, "target")
        status, output = install_local(
            [
                "--exclude",
                "Geode_Self",
                "--require",
                "OpenGeode-core",
                upstream,
                common,
                own,
                os.path.join(directory, "missing"),
            ],
            target,
        )
        expect(status == 0, "install-local should succeed", output)
        expect(
            "Installing the local geode wheels: geode-common, opengeode-core" in output,
            "installed wheels",
            output,
        )
        for module in ("fake_opengeode", "fake_geode_common"):
            expect(os.path.isfile(os.path.join(target, module + ".py")), module, output)
        for module in ("fake_numpy", "fake_geode_self"):
            expect(not os.path.exists(os.path.join(target, module + ".py")), module, output)


def test_install_local_missing_requirement():
    with tempfile.TemporaryDirectory() as directory:
        upstream = os.path.join(directory, "OpenGeode", "python_wheels")
        make_wheel(upstream, "OpenGeode-core", "fake_opengeode")
        target = os.path.join(directory, "target")
        status, output = install_local(
            ["--require", "OpenGeode-core", "--require", "OpenGeode_IO", upstream],
            target,
        )
        expect(status == 1, "missing local wheel should fail", output)
        expect("no local wheel of opengeode-io in " + upstream in output, "message", output)
        expect(not os.path.exists(target), "nothing installed", output)
        # The excluded package (the one being built) is not a local wheel
        status, output = install_local(
            ["--exclude", "OpenGeode-core", "--require", "OpenGeode-core", upstream],
            target,
        )
        expect(status == 1, "excluded package should not satisfy", output)


def test_install_local_nothing():
    with tempfile.TemporaryDirectory() as directory:
        status, output = install_local(
            [os.path.join(directory, "missing")], os.path.join(directory, "target")
        )
        expect(status == 0, "no wheel directory should succeed", output)
        expect("No local geode wheel to install" in output, "message", output)
        status, output = install_local([], os.path.join(directory, "target"))
        expect(status == 0, "no argument should succeed", output)


def test_publish():
    with tempfile.TemporaryDirectory() as directory:
        built = os.path.join(directory, "dist")
        published = os.path.join(directory, "lib", "cmake", "OpenGeode", "python_wheels")
        make_wheel(published, "OpenGeode-core", "old", tags="cp313-cp313-linux_x86_64")
        os.rename(
            os.path.join(published, "OpenGeode_core-0.0.0-cp313-cp313-linux_x86_64.whl"),
            os.path.join(published, "opengeode_core-1.0.0-cp313-cp313-linux_x86_64.whl"),
        )
        # Another platform of the same package and another package are kept
        make_wheel(published, "OpenGeode-core", "old", tags="cp312-cp312-linux_x86_64")
        make_wheel(published, "OpenGeode-IO", "io", tags="cp313-cp313-linux_x86_64")
        wheel = make_wheel(built, "OpenGeode-core", "new", tags="cp313-cp313-linux_x86_64")
        status, output = run(["publish", wheel, published])
        expect(status == 0, "publish should succeed", output)
        expect(
            sorted(os.listdir(published))
            == [
                "OpenGeode_IO-0.0.0-cp313-cp313-linux_x86_64.whl",
                "OpenGeode_core-0.0.0-cp312-cp312-linux_x86_64.whl",
                "OpenGeode_core-0.0.0-cp313-cp313-linux_x86_64.whl",
            ],
            "published wheels: %s" % sorted(os.listdir(published)),
        )
        # Into a new directory
        status, output = run(["publish", wheel, os.path.join(directory, "new")])
        expect(status == 0, "publish in a new directory", output)
        expect(os.listdir(os.path.join(directory, "new")), "published", output)


def test_layout_removes_published_wheels():
    with tempfile.TemporaryDirectory() as directory:
        package = os.path.join(directory, "opengeode")
        folder = os.path.join(package, "lib")
        write(os.path.join(folder, "opengeode_py_mesh" + SUFFIX))
        wheels = os.path.join(folder, "cmake", "OpenGeode", "python_wheels")
        make_wheel(wheels, "OpenGeode-core", "old")
        write(os.path.join(folder, "cmake", "OpenGeode", "OpenGeodeConfig.cmake"))
        status, output = run(["layout", package, "lib"])
        expect(status == 0, "layout should succeed", output)
        expect(not os.path.exists(wheels), "published wheels removed", output)
        expect(
            os.path.isfile(os.path.join(folder, "cmake", "OpenGeode", "OpenGeodeConfig.cmake")),
            "config kept",
        )


if __name__ == "__main__":
    test_requirements()
    test_requirements_empty()
    test_install_local()
    test_install_local_missing_requirement()
    test_install_local_nothing()
    test_publish()
    test_layout_removes_published_wheels()
    test_layout()
    test_layout_only_moves_own_modules()
    test_check_valid()
    test_check_stub_through_binary_folder()
    test_check_module_in_binary_folder()
    test_check_type_in_subfolder_module()
