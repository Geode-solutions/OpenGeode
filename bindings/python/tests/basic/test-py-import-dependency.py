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


"""import_binding_dependency with the fully qualified name of a module of
another package (geode_test_upstream.opengeode_py_basic), as a downstream binding
imports an OpenGeode module: the top-level module is used in a build tree,
the module of the package in a wheel layout, and a missing package is
reported as it is. Argument: the test module (import_dependency_module.cpp)."""

import importlib.util
import os
import shutil
import subprocess
import sys
import tempfile

import geode_test_env

MODULE = os.path.abspath(sys.argv[1])
UPSTREAM = "geode_test_upstream"

CHECK = """
import sys
import geode_test_env
import {module} as module
print("UPSTREAM" if "{upstream}.opengeode_py_basic" in sys.modules else "NO_UPSTREAM")
print("TOP_LEVEL" if "opengeode_py_basic" in sys.modules else "NO_TOP_LEVEL")
uuid_class = sys.modules["{basic}"].uuid
value = uuid_class()
if module.same_uuid(value) != value:
    raise ValueError("same_uuid")
print("SIGNATURE " + module.same_uuid.__doc__.splitlines()[0])
for cls in (module.First, module.Second, module.First.Partner):
    if "cannot be constructed directly" not in cls.__init__.__doc__:
        raise ValueError("constructor of " + cls.__name__)
"""


def run(directory, paths, module, basic):
    environment = dict(os.environ)
    environment["PYTHONPATH"] = os.pathsep.join(
        paths + [os.path.dirname(geode_test_env.__file__)]
    )
    return subprocess.run(
        [sys.executable, "-c", CHECK.format(module=module, upstream=UPSTREAM, basic=basic)],
        cwd=directory,
        env=environment,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        timeout=120,
    )


def expect(condition, message, output):
    if not condition:
        raise ValueError("[Test] " + message + "\n" + output)


def make_package(directory, name, module):
    package = os.path.join(directory, name)
    os.makedirs(package)
    open(os.path.join(package, "__init__.py"), "w").close()
    shutil.copy(module, os.path.join(package, os.path.basename(module)))


if __name__ == "__main__":
    basic = importlib.util.find_spec("opengeode_py_basic").origin
    with tempfile.TemporaryDirectory() as directory:
        # Wheel layout: the upstream module only exists in its package
        make_package(directory, UPSTREAM, basic)
        make_package(directory, "downstream", MODULE)
        result = run(
            directory, [directory], "downstream.geode_test_import_dependency",
            UPSTREAM + ".opengeode_py_basic",
        )
        expect(result.returncode == 0, "wheel layout", result.stdout)
        expect("UPSTREAM" in result.stdout.split(), "upstream package imported", result.stdout)
        expect("NO_TOP_LEVEL" in result.stdout, "no top-level module", result.stdout)
        expect(
            UPSTREAM + ".opengeode_py_basic.uuid" in result.stdout,
            "the signature names the upstream class",
            result.stdout,
        )
    with tempfile.TemporaryDirectory() as directory:
        # Build tree: the top-level module is used, the package is never imported
        make_package(directory, "downstream", MODULE)
        result = run(
            directory, [directory, os.path.dirname(basic)],
            "downstream.geode_test_import_dependency", "opengeode_py_basic",
        )
        expect(result.returncode == 0, "build tree", result.stdout)
        expect("NO_UPSTREAM" in result.stdout, "no package import", result.stdout)
        expect("TOP_LEVEL" in result.stdout.split(), "top-level module", result.stdout)
        expect(
            "opengeode_py_basic.uuid" in result.stdout,
            "the signature names the class",
            result.stdout,
        )
        # Neither: the missing package is reported
        result = run(
            directory, [directory], "downstream.geode_test_import_dependency",
            "opengeode_py_basic",
        )
        expect(result.returncode != 0, "missing upstream", result.stdout)
        expect(
            "ModuleNotFoundError" in result.stdout and UPSTREAM in result.stdout,
            "the missing package is reported",
            result.stdout,
        )
    print("import_binding_dependency resolves qualified names")
