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

import geode_test_env

import opengeode


def load_one_raises(objects):
    try:
        opengeode.load_one(objects, "file")
    except RuntimeError as error:
        print(error)
        return True
    return False


if __name__ == "__main__":
    if opengeode.load_one([42], "file") != 42:
        raise ValueError("[Test] Wrong single object")
    if not load_one_raises([]):
        raise ValueError("[Test] load_one should raise on an empty list")
    if not load_one_raises([1, 2]):
        raise ValueError("[Test] load_one should raise on two objects")
