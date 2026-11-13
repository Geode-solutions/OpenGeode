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


"""Every extension module can be imported alone, in a fresh interpreter: the
modules it depends on are imported by the module itself."""

import subprocess
import sys

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
    failures = []
    for module in MODULES:
        result = subprocess.run(
            [
                sys.executable,
                "-c",
                f"import geode_test_env; import {module}; "
                f"assert {module}.__doc__",
            ],
            capture_output=True,
            text=True,
        )
        if result.returncode != 0:
            failures.append(
                f"{module}: {result.stderr.strip().splitlines()[-1] if result.stderr.strip() else result.returncode}"
            )
    if failures:
        print("Modules which cannot be imported alone:")
        for failure in failures:
            print("  " + failure)
        sys.exit(1)
    print("All the modules can be imported alone")
