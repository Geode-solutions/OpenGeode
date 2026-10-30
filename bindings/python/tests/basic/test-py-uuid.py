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

import opengeode_py_basic as basic

if __name__ == '__main__':
    id = basic.uuid()
    print(id.string())

    same = basic.uuid(id.string())
    other = basic.uuid()
    if not (id == same) or id != same:
        raise ValueError("[Test] uuids built from the same string should be equal")
    if id == other:
        raise ValueError("[Test] Different uuids should not be equal")
    if hash(id) != hash(same):
        raise ValueError("[Test] Equal uuids should have the same hash")
    if len({id: 0, same: 1, other: 2}) != 2:
        raise ValueError("[Test] uuid should be usable as dict key")
    if not ((id < other) != (other < id)):
        raise ValueError("[Test] uuids should be ordered")
    if str(id) != id.string() or repr(id) != id.string():
        raise ValueError("[Test] str/repr of uuid should return its string")
