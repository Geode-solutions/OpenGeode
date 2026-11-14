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
# all copies and substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import json
import os
import subprocess
import sys
import tempfile

import geode_test_env


def run_import(module_name, broken_module):
    """Import module_name in a fresh interpreter where broken_module (and the
    'opengeode' fallback) fail to import with distinct markers."""
    with tempfile.TemporaryDirectory() as fake_dir:
        with open(os.path.join(fake_dir, broken_module + ".py"), "w") as f:
            f.write("raise ImportError('real-dependency-failure')\n")
        with open(os.path.join(fake_dir, "opengeode.py"), "w") as f:
            f.write("raise ImportError('masking-fallback-failure')\n")
        code = (
            "import sys, json\n"
            f"sys.path[:0] = [{fake_dir!r}] + json.loads({json.dumps(json.dumps(sys.path))})\n"
            "try:\n"
            f"    import {module_name}\n"
            "except ImportError:\n"
            "    import traceback\n"
            "    print(traceback.format_exc())\n"
            "else:\n"
            "    print('imported')\n"
        )
        result = subprocess.run([sys.executable, "-c", code],
                                capture_output=True, text=True, check=True)
        return result.stdout


def test_import_error_surfaces():
    for module_name, broken in [("opengeode_py_mesh", "opengeode_py_basic"),
                                ("opengeode_py_model", "opengeode_py_basic"),
                                ("opengeode_py_model", "opengeode_py_mesh")]:
        report = run_import(module_name, broken)
        if "real-dependency-failure" not in report:
            raise ValueError(
                f"[Test] Importing {module_name} with broken {broken} should "
                f"surface the real ImportError, got: {report!r}")
        if "masking-fallback-failure" in report:
            raise ValueError(
                f"[Test] Importing {module_name} with broken {broken} should "
                f"not fall back to another import, got: {report!r}")


if __name__ == '__main__':
    test_import_error_surfaces()
