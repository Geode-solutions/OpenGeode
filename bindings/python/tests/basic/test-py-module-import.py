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
modules it depends on are imported by the module itself. Their signatures then
name the Python classes: a module which forgets to import the module defining a
type it uses shows the raw C++ type (geode::Point<3u>) in its signatures."""

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

RAW_CPP_TYPES = """
import geode_test_env
import inspect
import {module} as module

raw = []
for name in dir(module):
    member = getattr(module, name)
    members = [(name, member)]
    if inspect.isclass(member):
        members = [
            (name + "." + attribute, getattr(member, attribute, None))
            for attribute in dir(member)
        ]
    for member_name, function in members:
        doc = getattr(function, "__doc__", None) or ""
        if "geode::" in doc or "absl::" in doc:
            raw.append(member_name)
assert not raw, "raw C++ types in the signatures of " + ", ".join(raw[:5])
"""

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
        if result.returncode == 0:
            result = subprocess.run(
                [sys.executable, "-c", RAW_CPP_TYPES.format(module=module)],
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
