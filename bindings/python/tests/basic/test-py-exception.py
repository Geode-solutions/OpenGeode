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

import os
import tempfile

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


def expect_save_error(save, filename, expected, cause, words):
    try:
        save(filename)
    except opengeode.OpenGeodeError as error:
        if type(error) is not expected:
            raise ValueError(
                "[Test] Saving %s should raise %s, got %s: %s"
                % (filename, expected.__name__, type(error).__name__, error.message)
            )
        if not error.message.startswith("Cannot save"):
            raise ValueError("[Test] Wrong save message: " + error.message)
        for word in words:
            if word not in error.message:
                raise ValueError("[Test] %s not in the message: %s" % (word, error.message))
        if cause is None:
            if error.__cause__ is not None:
                raise ValueError("[Test] Unexpected __cause__ for " + filename)
        else:
            if type(error.__cause__) is not cause:
                raise ValueError(
                    "[Test] The error of the writer should be the __cause__ (%s), got %r"
                    % (cause.__name__, error.__cause__)
                )
            if error.__cause__.message not in error.message:
                raise ValueError("[Test] The message should contain the writer error")
        return
    raise ValueError("[Test] Saving %s should raise" % filename)


def check_save_errors():
    # The error of a writer keeps its type: a file that cannot be opened is a
    # data error, not an internal one
    surface = opengeode.TriangulatedSurface3D.create()
    brep = opengeode.BRep()
    savers = [
        (lambda name: opengeode.save_triangulated_surface3D(surface, name), "og_tsf3d"),
        (lambda name: opengeode.save(surface, name), "og_tsf3d"),
        (lambda name: opengeode.save_point_set3D(opengeode.PointSet3D.create(), name), "og_pts3d"),
        # The zip writer of the models reports the move of its archive onto
        # a directory as an internal OpenGeodeException: kept internal
        (lambda name: opengeode.save_brep(brep, name), None),
    ]
    with tempfile.TemporaryDirectory() as directory:
        not_a_directory = os.path.join(directory, "file")
        open(not_a_directory, "w").close()
        for index, (save, extension) in enumerate(savers):
            if extension is not None:
                # A directory in place of the file: the writer cannot open it
                occupied = os.path.join(directory, "occupied%d.%s" % (index, extension))
                os.mkdir(occupied)
                expect_save_error(
                    save, occupied, opengeode.OpenGeodeDataError,
                    opengeode.OpenGeodeDataError, [occupied, "Failed to open file"],
                )
            else:
                extension = "og_brep"
                occupied = os.path.join(directory, "occupied%d.%s" % (index, extension))
                os.mkdir(occupied)
                expect_save_error(
                    save, occupied, opengeode.OpenGeodeInternalError,
                    opengeode.OpenGeodeInternalError, [occupied, "ZipFile"],
                )
            # Unknown extension: data error, raised by the factory
            unknown = os.path.join(directory, "file%d.unknown_extension" % index)
            expect_save_error(
                save, unknown, opengeode.OpenGeodeDataError,
                opengeode.OpenGeodeDataError, ["unknown_extension"],
            )
            # A standard exception (the folder cannot be created) is not an
            # OpenGeodeException: unchanged, an internal error without cause
            nested = os.path.join(not_a_directory, "sub", "file%d.%s" % (index, extension))
            expect_save_error(
                save, nested, opengeode.OpenGeodeInternalError, None, ["sub"]
            )


if __name__ == "__main__":
    check_data_error()
    check_save_errors()
    check_subclasses()
    check_cause_chain()
    check_standard_exceptions()
