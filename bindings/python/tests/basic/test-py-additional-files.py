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

import geode_test_env

import opengeode_py_basic as basic


def test_additional_files():
    files = basic.AdditionalFiles()
    if files.has_additional_files():
        raise ValueError("[Test] Empty AdditionalFiles should have no missing file")

    files.optional_files.append(basic.AdditionalFile("optional.txt", False))
    if len(files.optional_files) != 1:
        raise ValueError("[Test] optional_files.append should be visible")
    if files.optional_files[0].filename != "optional.txt":
        raise ValueError("[Test] Wrong optional file name")
    if files.has_additional_files():
        raise ValueError("[Test] No missing file yet")

    files.mandatory_files.append(basic.AdditionalFile("mandatory.txt", True))
    if len(files.mandatory_files) != 1:
        raise ValueError("[Test] mandatory_files.append should be visible")
    if not files.has_additional_files():
        raise ValueError("[Test] A missing mandatory file should be reported")

    files.mandatory_files[0].is_missing = False
    if files.has_additional_files():
        raise ValueError("[Test] In-place modification should be visible")

    files.optional_files = [basic.AdditionalFile("a", True),
                            basic.AdditionalFile("b", False)]
    if [file.filename for file in files.optional_files] != ["a", "b"]:
        raise ValueError("[Test] Assigning a list should replace the files")
    if not files.has_additional_files():
        raise ValueError("[Test] Missing optional file should be reported")


if __name__ == '__main__':
    test_additional_files()
