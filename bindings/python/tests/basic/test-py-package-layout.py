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


"""The wheel ships the extension modules inside a package (opengeode.lib.
opengeode_py_image...), not as top-level modules: a module which imports another
one by its top-level name only works from the build tree. Each extension module
is imported alone, in a fresh interpreter, from such a package."""

import importlib.util
import os
import subprocess
import sys
import tempfile

import geode_test_env

MODULES = [
    "opengeode_py_basic",
    "opengeode_py_geometry",
    "opengeode_py_image",
    "opengeode_py_gdal",
    "opengeode_py_mesh",
    "opengeode_py_model",
]

if __name__ == "__main__":
    with tempfile.TemporaryDirectory() as directory:
        library = os.path.join(directory, "package", "lib")
        os.makedirs(library)
        for init in (
            os.path.join(directory, "package", "__init__.py"),
            os.path.join(library, "__init__.py"),
        ):
            open(init, "w").close()
        for module in MODULES:
            origin = importlib.util.find_spec(module).origin
            os.symlink(origin, os.path.join(library, os.path.basename(origin)))
        # The directories of the extension modules are not on the path anymore
        environment = dict(os.environ)
        environment["PYTHONPATH"] = os.pathsep.join(
            [directory, os.path.dirname(geode_test_env.__file__)]
        )
        failures = []
        for module in MODULES:
            result = subprocess.run(
                [
                    sys.executable,
                    "-c",
                    f"import geode_test_env; import package.lib.{module} as m; "
                    "assert m.__doc__",
                ],
                cwd=directory,
                env=environment,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True,
            )
            if result.returncode != 0:
                failures.append(f"{module}:\n{result.stdout}")
        if failures:
            raise ValueError(
                "[Test] Modules not importable from a package:\n"
                + "\n".join(failures)
            )
