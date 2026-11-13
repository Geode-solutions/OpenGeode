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

"""Decisions of cmake/PythonTargets.cmake for the Python environments of a
downstream project, run in CMake script mode on synthetic found packages:
a geode package with python_distribution.txt and one without (C++ package
built before them or without Python), installed or not in the Python used to
configure, and the version recorded for the distribution.
Arguments: the cmake executable and cmake/PythonTargets.cmake."""

import os
import subprocess
import sys
import tempfile
import textwrap

CMAKE = sys.argv[1]
PYTHON_TARGETS = os.path.abspath(sys.argv[2])
SCRIPT = os.path.join(os.path.dirname(PYTHON_TARGETS), "geode_python_package.py")


def write(path, content=""):
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w") as file:
        file.write(textwrap.dedent(content))


def expect(condition, message, output=""):
    if not condition:
        raise ValueError("[Test] " + message + "\n" + output)


def cmake_path(path):
    return path.replace("\\", "/")


def run_cmake(directory, body, pythonpath=None):
    script = os.path.join(directory, "script.cmake")
    write(
        script,
        'set(PROJECT_NAME "Down")\n'
        'set(PROJECT_BINARY_DIR "%s")\n'
        'set(CMAKE_SOURCE_DIR "%s")\n'
        'set(PYTHON_EXECUTABLE "%s")\n'
        'include("%s")\n'
        % (
            cmake_path(os.path.join(directory, "build")),
            cmake_path(os.path.join(directory, "source")),
            cmake_path(sys.executable),
            cmake_path(PYTHON_TARGETS),
        )
        + body,
    )
    environment = dict(os.environ)
    environment["PYTHONPATH"] = pythonpath or ""
    result = subprocess.run(
        [CMAKE, "-P", script],
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        env=environment,
    )
    expect(result.returncode == 0, "cmake -P failed", result.stdout)
    values = {}
    for line in result.stdout.splitlines():
        if "=" in line:
            key, value = line.split("=", 1)
            values[key.strip()] = value
    return values, result.stdout


def fake_installed(directory, distribution, version):
    info = os.path.join(directory, "%s-%s.dist-info" % (distribution.replace("-", "_"), version))
    write(
        os.path.join(info, "METADATA"),
        "Metadata-Version: 2.1\nName: %s\nVersion: %s\n" % (distribution, version),
    )
    return directory


ENVIRONMENT_BODY = """
set_property(GLOBAL PROPERTY PACKAGES_FOUND "OpenGeode;OpenGeode-IO;Unrelated")
set(OpenGeode_DIR "{core}")
set(OpenGeode-IO_DIR "{io}")
geode_python_requirements_file(requirements)
geode_python_geode_packages(arguments missing)
_geode_python_test_venv_options(options installed "${{missing}}")
message("REQUIREMENTS=${{requirements}}")
message("ARGUMENTS=${{arguments}}")
message("MISSING=${{missing}}")
message("OPTIONS=${{options}}")
message("INSTALLED=${{installed}}")
"""


def make_packages(directory):
    # OpenGeode-IO provides its distribution files, OpenGeode (a C++ package
    # without Python, or built before them) does not
    core = os.path.join(directory, "OpenGeode", "lib", "cmake", "OpenGeode")
    io = os.path.join(directory, "OpenGeode-IO", "lib", "cmake", "OpenGeode-IO")
    write(os.path.join(core, "OpenGeodeConfig.cmake"))
    write(os.path.join(io, "python_distribution.txt"), "OpenGeode-IO\n2.0.0-rc.1\n")
    write(os.path.join(io, "python_requirements.in"), "OpenGeode-core\nscipy\n")
    write(
        os.path.join(directory, "source", "bindings", "python", "requirements.in"),
        "OpenGeode-IO\nOpenGeode-core>=15\nnumpy>=2\n",
    )
    return ENVIRONMENT_BODY.format(core=cmake_path(core), io=cmake_path(io)), io


def install_geode(arguments, pythonpath, extra=()):
    environment = dict(os.environ)
    environment["PYTHONPATH"] = pythonpath or ""
    result = subprocess.run(
        [sys.executable, SCRIPT, "install-geode"] + arguments + list(extra),
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        env=environment,
    )
    return result.returncode, result.stdout


def test_found_package_without_mapping(preinstalled):
    with tempfile.TemporaryDirectory() as directory:
        body, io = make_packages(directory)
        site = None
        if preinstalled:
            # CI: the wheel matching the C++ package is installed in the Python
            # used to configure
            site = fake_installed(os.path.join(directory, "site"), "OpenGeode-core", "0.0.0")
        values, output = run_cmake(directory, body, site)
        with open(values["REQUIREMENTS"]) as file:
            requirements = file.read().splitlines()
        # Never pip-installed from the index without a pin
        expect(requirements == ["scipy", "numpy>=2"], "requirements %s" % requirements, output)
        arguments = values["ARGUMENTS"].split(";")
        expect(
            arguments[:3] == ["--package", "opengeode-core", "unknown"]
            and arguments[5:8] == ["--package", "opengeode-io", "2.0.0-rc.1"]
            and arguments[8] == cmake_path(os.path.join(io, "python_wheels")),
            "install-geode arguments %s" % arguments,
            output,
        )
        expect(values["MISSING"] == "opengeode-core;opengeode-io", "missing", output)
        if preinstalled:
            expect(values["OPTIONS"] == "--system-site-packages", "options", output)
            expect(values["INSTALLED"] == "opengeode-core==0.0.0", "installed", output)
        else:
            expect(values["OPTIONS"] == "", "no system site packages", output)
            expect(values["INSTALLED"] == "", "nothing installed", output)
        # Wheel environment (error mode) and test environment (warn mode)
        for mode in ("error", "warn"):
            status, log = install_geode(
                arguments, site, ["--missing", mode, "--dry-run"]
            )
            if preinstalled:
                expect(status == 0, "preinstalled package kept (%s)" % mode, log)
                expect("opengeode-core: installed version 0.0.0 kept" in log, "kept", log)
                expect(
                    "opengeode-io==2.0.0rc1" in log and "opengeode-core==" not in log,
                    "only the mapped package is pinned",
                    log,
                )
            else:
                expect("no local wheel of opengeode-core" in log, "message", log)
                expect("opengeode-core==" not in log, "never installed unpinned", log)
                if mode == "error":
                    expect(status == 1, "the wheel environment fails", log)
                    expect("Would run" not in log, "nothing installed", log)
                else:
                    expect(status == 0, "the test environment warns", log)
                    expect("warning: no local wheel of opengeode-core" in log, "warning", log)


def test_distribution_version():
    cases = [
        ("15.2.0", "", "15.2.0"),
        ("master", "15.2.0-rc.3", "15.2.0-rc.3"),
        ("master", "15.2.0rc3", "15.2.0rc3"),
        ("master", "", "unknown"),
        ("", "", "unknown"),
        ("0.0.0", "0.0.0", "0.0.0"),
        ("0.0.0", "15.2.0", "15.2.0"),
        ("master", "0.0.0", "0.0.0"),
        ("15.2.0", "16.0.0", "15.2.0"),
        ("v1.2.3", "", "v1.2.3"),
        ("1.2", "1.2.3-beta", "unknown"),
    ]
    body = ""
    for index, (cpack, wheel, _) in enumerate(cases):
        body += (
            'set(CPACK_PACKAGE_VERSION "%s")\nset(WHEEL_VERSION "%s")\n'
            "geode_python_distribution_version(version)\n"
            'message("VERSION%d=${version}")\n' % (cpack, wheel, index)
        )
    with tempfile.TemporaryDirectory() as directory:
        values, output = run_cmake(directory, body)
    for index, (cpack, wheel, expected) in enumerate(cases):
        expect(
            values.get("VERSION%d" % index) == expected,
            "version of (%s, %s): %s" % (cpack, wheel, values.get("VERSION%d" % index)),
            output,
        )


def fake_interpreter(directory, name, version):
    """An executable that prints the given version whatever its arguments."""
    if os.name == "nt":
        path = os.path.join(directory, name + ".bat")
        write(path, "@echo off\r\necho %s\r\n" % version)
    else:
        path = os.path.join(directory, name)
        write(path, "#!/bin/sh\necho '%s'\n" % version)
        os.chmod(path, 0o755)
    return cmake_path(path)


def environment_hash(directory, interpreter, version):
    path = fake_interpreter(directory, interpreter, version)
    body = (
        'set(PYTHON_EXECUTABLE "%s")\n'
        'set(GEODE_STUBGEN_REQUIREMENT "stubgen")\n'
        'set(GEODE_MYPY_REQUIREMENT "mypy")\n'
        '_geode_python_environment_hash(hash "numpy" "" "" "pkg" "")\n'
        'message("HASH=${hash}")\n' % path
    )
    values, output = run_cmake(directory, body)
    expect(len(values.get("HASH", "")) == 32, "hash of the environment", output)
    return values["HASH"]


def test_environment_hash():
    # The test environment is rebuilt when the base interpreter changes, even
    # if it keeps its path (the same file is rewritten with another version)
    with tempfile.TemporaryDirectory() as directory:
        first = environment_hash(directory, "python_a", "3.13.1 (fake)")
        second = environment_hash(directory, "python_a", "3.13.2 (fake)")
        third = environment_hash(directory, "python_a", "3.13.1 (fake)")
        fourth = environment_hash(directory, "python_b", "3.13.1 (fake)")
    expect(first != second, "the interpreter version is in the hash")
    expect(first == third, "the hash is stable")
    expect(first != fourth, "the interpreter path is in the hash")


def test_distribution_files():
    with tempfile.TemporaryDirectory() as directory:
        source = os.path.join(directory, "source", "bindings", "python")
        write(os.path.join(source, "requirements.in"), "numpy>=2\n")
        write(
            os.path.join(source, "CMakeLists.txt"),
            'add_subdirectory(src)\nadd_geode_python_wheel(\n    NAME "Geode-Fake"\n'
            '    DESCRIPTION "Fake (with parentheses)"\n    MODULES "fake.py"\n)\n',
        )
        body = (
            'set(PROJECT_SOURCE_DIR "%s")\n'
            'set(CMAKE_CONFIG_DESTINATION "lib/cmake/Down")\n'
            'set(CPACK_PACKAGE_VERSION "master")\nset(WHEEL_VERSION "3.0.0")\n'
            "_geode_python_wheel_name(name)\n"
            'message("NAME=${name}")\n'
            "function(install)\nendfunction()\n"
            'geode_python_distribution_files("${name}")\n'
        ) % cmake_path(os.path.join(directory, "source"))
        values, output = run_cmake(directory, body)
        expect(values.get("NAME") == "Geode-Fake", "wheel name", output)
        provided = os.path.join(directory, "build", "lib", "cmake", "Down")
        with open(os.path.join(provided, "python_distribution.txt")) as file:
            expect(file.read() == "Geode-Fake\n3.0.0\n", "distribution file", output)
        with open(os.path.join(provided, "python_requirements.in")) as file:
            expect(file.read() == "numpy>=2\n", "requirements file", output)


if __name__ == "__main__":
    test_found_package_without_mapping(preinstalled=True)
    test_found_package_without_mapping(preinstalled=False)
    test_distribution_version()
    test_distribution_files()
    test_environment_hash()
