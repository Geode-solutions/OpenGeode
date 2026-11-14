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
import opengeode_py_basic


def check_data_error():
    try:
        opengeode.load_graph("file.unknown_extension")
    except opengeode.OpenGeodeDataError as error:
        if not isinstance(error, RuntimeError):
            raise ValueError("[Test] OpenGeodeError should be a RuntimeError")
        if not isinstance(error, opengeode.OpenGeodeError):
            raise ValueError("[Test] Wrong exception base class")
        if error.type != "data":
            raise ValueError("[Test] Wrong exception type")
        if error.project != "OpenGeode":
            raise ValueError("[Test] Wrong exception project")
        if not error.library:
            raise ValueError("[Test] Empty exception library")
        if "unknown_extension" not in error.message:
            raise ValueError("[Test] The message should mention the extension")
        if error.message not in str(error):
            raise ValueError("[Test] str(error) should contain the message")
        if not error.details:
            raise ValueError("[Test] Empty exception details")
        if error.__cause__ is not None:
            raise ValueError("[Test] Unexpected __cause__")
        return
    raise ValueError("[Test] load_graph should raise OpenGeodeDataError")


def check_subclasses():
    for name in ["Data", "Internal", "Result"]:
        subclass = getattr(opengeode, "OpenGeode" + name + "Error")
        if not issubclass(subclass, opengeode.OpenGeodeError):
            raise ValueError("[Test] Wrong subclass of OpenGeodeError")
    if not issubclass(opengeode.OpenGeodeError, RuntimeError):
        raise ValueError("[Test] OpenGeodeError should derive RuntimeError")


def check_cause_chain():
    try:
        opengeode_py_basic._throw_nested_exception_for_test()
    except opengeode.OpenGeodeDataError as error:
        cause = error.__cause__
        if not isinstance(cause, opengeode.OpenGeodeInternalError):
            raise ValueError("[Test] Wrong __cause__ type")
        if "std::exception" not in cause.message:
            raise ValueError("[Test] Wrong __cause__ message")
        if cause.__cause__ is not None:
            raise ValueError("[Test] __cause__ chain should end here")
        return
    raise ValueError("[Test] _throw_nested_exception_for_test should raise")


def check_standard_exceptions():
    # A C++ standard exception that the library does not catch keeps the
    # translation of pybind11: it is not an OpenGeodeError
    documentation = opengeode.__doc__
    for kind, expected in [
        ("runtime_error", RuntimeError),
        ("out_of_range", IndexError),
        ("invalid_argument", ValueError),
        ("bad_alloc", MemoryError),
    ]:
        try:
            opengeode_py_basic._throw_standard_exception_for_test(kind)
        except opengeode.OpenGeodeError as error:
            raise ValueError("[Test] std::%s should not be an OpenGeodeError" % kind)
        except Exception as error:
            if type(error) is not expected:
                raise ValueError(
                    "[Test] std::%s should be a %s, got %s"
                    % (kind, expected.__name__, type(error).__name__)
                )
        else:
            raise ValueError("[Test] _throw_standard_exception_for_test should raise")
        claim = "%s for std::%s" % (expected.__name__, kind)
        if claim not in " ".join(documentation.split()):
            raise ValueError("[Test] The package documentation should say " + claim)
    if "(std::exception)" in documentation:
        raise ValueError(
            "[Test] The documentation should not say that OpenGeodeInternalError "
            "is any std::exception"
        )


if __name__ == "__main__":
    check_data_error()
    check_subclasses()
    check_cause_chain()
    check_standard_exceptions()
