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
    Logger = basic.Logger
    initial_level = Logger.level()
    Logger.set_level(Logger.LEVEL.warning)
    if Logger.level() != Logger.LEVEL.warning:
        raise ValueError("[Test] Logger level should be warning")
    if Logger.is_enabled(Logger.LEVEL.info):
        raise ValueError("[Test] Info level should be disabled")
    if not Logger.is_enabled(Logger.LEVEL.error):
        raise ValueError("[Test] Error level should be enabled")
    Logger.info("This message should not be displayed")
    Logger.warning("Logger test warning")
    Logger.set_level(initial_level)
