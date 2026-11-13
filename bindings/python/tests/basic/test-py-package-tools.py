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


def test_check_rpath():
    """The rpath of a real extension module of the build tree contains
    $ORIGIN/<libdir> (@loader_path on macOS)."""
    import importlib.util
    import platform
    import shutil

    if platform.system() not in ("Linux", "Darwin"):
        return
    tool = "readelf" if platform.system() == "Linux" else "otool"
    if shutil.which(tool) is None:
        print("[Test] %s not found: rpath check not tested" % tool)
        return
    origin = importlib.util.find_spec("opengeode_py_basic").origin
    binary_folder = os.path.basename(os.path.dirname(origin))
    for folder, valid in ((binary_folder, True), ("not_the_folder", False)):
        with tempfile.TemporaryDirectory() as directory:
            package = make_checked_package(directory, "fakegeode")
            os.makedirs(os.path.join(package, folder), exist_ok=True)
            shutil.copy(origin, os.path.join(package, "fakegeode_py_basic" + SUFFIX))
            status, output = run(["check", package, folder], pythonpath=directory)
            if valid:
                expect(status == 0, "the build rpath should be accepted", output)
            else:
                expect(status == 1, "a wrong folder should be rejected", output)
                expect(
                    "does not contain $ORIGIN/not_the_folder" in output
                    or "does not contain @loader_path/not_the_folder" in output,
                    "message",
                    output,
                )


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


GEODE = ["opengeode-core", "opengeode-io", "geode-common", "geode-simplex"]


def requirements(*contents, geode_distributions=GEODE):
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
            + ["--geode=" + name for name in geode_distributions]
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
            + ["--geode=" + name for name in geode_distributions]
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
        geode-not-found-by-cmake>=2
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
        geode
        == [
            "opengeode-core",
            "opengeode-io",
            "geode-common",
            "geode-simplex",
            "geode-not-found-by-cmake",
        ],
        "geode requirements: %s" % geode,
    )


def test_requirements_exact_names():
    # The distributions of the projects found by CMake are removed by exact
    # name, whatever their name; the geode ones of projects without
    # python_distribution.txt too: never installed unpinned from the index
    kept, geode = requirements(
        "OpenGeode-core\nOpenGeode-IO\nmy-chain-package\nnumpy\n",
        geode_distributions=["OpenGeode_core", "My.Chain_Package"],
    )
    expect(kept == ["numpy"], "kept: %s" % kept)
    expect(
        geode == ["opengeode-core", "opengeode-io", "my-chain-package"],
        "geode: %s" % geode,
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


def install_geode(arguments, target=None, extra_paths=()):
    environment = dict(os.environ)
    if target:
        # Installed in a directory, not in the Python running the tests
        environment["PIP_TARGET"] = target
    environment["PIP_DISABLE_PIP_VERSION_CHECK"] = "1"
    # Distributions visible as installed in the Python of the script
    environment["PYTHONPATH"] = os.pathsep.join(
        list(extra_paths) + [environment.get("PYTHONPATH", "")]
    )
    result = subprocess.run(
        [sys.executable, SCRIPT, "install-geode"] + arguments,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        env=environment,
    )
    return result.returncode, result.stdout


def package(name, version, directory):
    return ["--package", name, version, os.path.join(directory, "python_wheels"), directory]


def fake_installed(directory, distribution, version):
    """A distribution installed in a site directory (its metadata only)."""
    info = os.path.join(
        directory, "%s-%s.dist-info" % (distribution.replace("-", "_"), version)
    )
    write(
        os.path.join(info, "METADATA"),
        "Metadata-Version: 2.1\nName: %s\nVersion: %s\n" % (distribution, version),
    )
    return directory


def test_install_geode_local_wheels():
    with tempfile.TemporaryDirectory() as directory:
        upstream = os.path.join(directory, "OpenGeode")
        common = os.path.join(directory, "GeodeCommon")
        own = os.path.join(directory, "Self")
        make_wheel(os.path.join(upstream, "python_wheels"), "OpenGeode-core", "fake_opengeode")
        make_wheel(os.path.join(common, "python_wheels"), "Geode-Common", "fake_geode_common")
        # Not required: never installed from these directories
        make_wheel(os.path.join(common, "python_wheels"), "numpy", "fake_numpy")
        # The package being built
        make_wheel(os.path.join(own, "python_wheels"), "Geode-Self", "fake_geode_self")
        target = os.path.join(directory, "target")
        status, output = install_geode(
            ["--exclude", "Geode_Self"]
            + package("OpenGeode-core", "unknown", upstream)
            + package("geode-common", "2.0.0", common)
            + package("geode-self", "unknown", own),
            target,
        )
        expect(status == 0, "install-geode should succeed", output)
        expect("opengeode-core: local wheel of" in output, "local wheel used", output)
        expect("--no-index" in output and "--no-deps" in output, "pip options", output)
        for module in ("fake_opengeode", "fake_geode_common"):
            expect(os.path.isfile(os.path.join(target, module + ".py")), module, output)
        for module in ("fake_numpy", "fake_geode_self"):
            expect(not os.path.exists(os.path.join(target, module + ".py")), module, output)
        expect("warning" not in output, "no warning", output)


def test_install_geode_stale_local_wheel():
    with tempfile.TemporaryDirectory() as directory:
        upstream = os.path.join(directory, "OpenGeode")
        wheel = make_wheel(os.path.join(upstream, "python_wheels"), "OpenGeode-core", "fake")
        config = os.path.join(upstream, "OpenGeodeConfig.cmake")
        write(config)
        os.utime(wheel, (1000, 1000))
        status, output = install_geode(
            package("OpenGeode-core", "unknown", upstream) + ["--dry-run"]
        )
        expect(status == 0, "a stale wheel is still installed", output)
        expect("is older than the CMake files" in output, "stale warning", output)
        os.utime(wheel, None)
        os.utime(config, (1000, 1000))
        status, output = install_geode(
            package("OpenGeode-core", "unknown", upstream) + ["--dry-run"]
        )
        expect("older" not in output, "fresh wheel without warning", output)


def test_install_geode_preinstalled():
    # CI: the matching wheel is installed before the build, no local wheel
    with tempfile.TemporaryDirectory() as directory:
        site = fake_installed(os.path.join(directory, "site"), "OpenGeode-core", "15.2.0rc3")
        upstream = os.path.join(directory, "OpenGeode")
        for version, warned in (
            ("unknown", False),
            ("0.0.0", False),
            ("15.2.0-rc.3", False),
            ("15.2.0rc3", False),
            ("15.2.0", True),
        ):
            status, output = install_geode(
                package("OpenGeode-core", version, upstream) + ["--dry-run"],
                extra_paths=[site],
            )
            expect(status == 0, "a preinstalled package satisfies " + version, output)
            expect("installed version 15.2.0rc3 kept" in output, "kept", output)
            expect("Would run" not in output, "nothing installed", output)
            expect(("warning" in output) == warned, "version warning " + version, output)


def test_install_geode_pinned_index():
    # No local wheel, nothing installed, known version: exact pin from the index
    with tempfile.TemporaryDirectory() as directory:
        upstream = os.path.join(directory, "OpenGeode")
        common = os.path.join(directory, "GeodeCommon")
        status, output = install_geode(
            package("OpenGeode-core", "15.2.0-rc.3", upstream)
            + package("Geode_Common", "v3.1.0", common)
            + ["--dry-run"]
        )
        expect(status == 0, "pinned install should be planned", output)
        expect(
            "-m pip install --no-deps opengeode-core==15.2.0rc3 geode-common==3.1.0" in output,
            "pinned command",
            output,
        )
        expect("--no-index" not in output, "from the index", output)


def test_install_geode_missing():
    with tempfile.TemporaryDirectory() as directory:
        upstream = os.path.join(directory, "OpenGeode")
        target = os.path.join(directory, "target")
        for version in ("unknown", "0.0.0", "master", "1.2"):
            status, output = install_geode(
                package("OpenGeode-core", version, upstream), target
            )
            expect(status == 1, "unknown version should fail: " + version, output)
            expect(
                "error: no local wheel of opengeode-core in " in output
                and "no released version to install" in output,
                "message",
                output,
            )
            expect("pip" not in output.split("error:")[0], "nothing installed", output)
            expect(not os.path.exists(target), "nothing installed", output)
        # Test environments only warn
        status, output = install_geode(
            package("OpenGeode-core", "unknown", upstream) + ["--missing", "warn"], target
        )
        expect(status == 0, "warn mode succeeds", output)
        expect("warning: no local wheel of opengeode-core" in output, "warning", output)


def test_install_geode_excluded():
    with tempfile.TemporaryDirectory() as directory:
        own = os.path.join(directory, "Self")
        make_wheel(os.path.join(own, "python_wheels"), "OpenGeode-core", "fake")
        status, output = install_geode(
            ["--exclude", "opengeode_core"] + package("OpenGeode-core", "unknown", own)
        )
        expect(status == 0, "excluded package is skipped", output)
        expect("No geode wheel to install" in output, "nothing to install", output)


def test_installed():
    with tempfile.TemporaryDirectory() as directory:
        site = fake_installed(os.path.join(directory, "site"), "OpenGeode-core", "1.0.0")
        environment = dict(os.environ)
        environment["PYTHONPATH"] = site
        result = subprocess.run(
            [sys.executable, SCRIPT, "installed", "OpenGeode_core", "geode-missing"],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            env=environment,
        )
        expect(result.returncode == 0, "installed should succeed", result.stdout)
        expect(result.stdout.strip() == "opengeode-core==1.0.0", "output", result.stdout)


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
    test_requirements_exact_names()
    test_requirements_empty()
    test_install_geode_local_wheels()
    test_install_geode_stale_local_wheel()
    test_install_geode_preinstalled()
    test_install_geode_pinned_index()
    test_install_geode_missing()
    test_install_geode_excluded()
    test_installed()
    test_publish()
    test_layout_removes_published_wheels()
    test_layout()
    test_layout_only_moves_own_modules()
    test_check_valid()
    test_check_stub_through_binary_folder()
    test_check_module_in_binary_folder()
    test_check_type_in_subfolder_module()
    test_check_rpath()
