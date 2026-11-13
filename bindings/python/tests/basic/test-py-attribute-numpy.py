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

import numpy

import geode_test_env

import opengeode_py_basic as basic
import opengeode_py_geometry as geom
import opengeode_py_mesh as mesh


def check(condition, message):
    if not condition:
        raise ValueError("[Test] " + message)


def check_raises(error_type, expected_message, function, label):
    try:
        function()
    except error_type as error:
        check(
            expected_message in str(error),
            f"{label}: unexpected message '{error}', expected '{expected_message}'",
        )
        check(
            type(error) is error_type,
            f"{label}: {type(error).__name__} is not exactly {error_type.__name__}",
        )
        return
    raise ValueError(f"[Test] {label}: {error_type.__name__} not raised")


def check_array(array, dtype, shape, label):
    check(isinstance(array, numpy.ndarray), f"{label}: not a numpy array")
    check(array.dtype == dtype, f"{label}: dtype {array.dtype}, expected {dtype}")
    check(array.shape == shape, f"{label}: shape {array.shape}, expected {shape}")


NB_ELEMENTS = 6

# suffix, default value, dtype, sample values
SCALAR_CASES = [
    ("bool", False, numpy.bool_, [True, False, True, True, False, True]),
    ("int", -1, numpy.int32, [5, -7, 0, 2147483647, -2147483648, 12]),
    ("uint", 0, numpy.uint32, [0, 1, 4294967295, 7, 8, 9]),
    ("float", 0.0, numpy.float32, [0.5, -1.25, 3.0, 1e10, 0.0, 2.5]),
    ("double", 0.0, numpy.float64, [0.1, -1e300, 3.0, 1e-300, 0.0, 2.5]),
]


def create_attribute(manager, suffix, default):
    create = getattr(manager, f"create_attribute_variable_{suffix}")
    find = getattr(manager, f"find_attribute_variable_{suffix}")
    return find(create("values", default))


def test_scalar_attributes():
    for suffix, default, dtype, sample in SCALAR_CASES:
        manager = basic.AttributeManager()
        manager.resize(NB_ELEMENTS)
        attribute = create_attribute(manager, suffix, default)
        label = suffix
        defaults = attribute.values_array()
        check_array(defaults, dtype, (NB_ELEMENTS,), label + " defaults")
        check(
            all(value == default for value in defaults.tolist()),
            label + " default values",
        )
        values = numpy.array(sample, dtype=dtype)
        attribute.set_values_array(values)
        result = attribute.values_array()
        check_array(result, dtype, (NB_ELEMENTS,), label)
        check(numpy.array_equal(result, values), label + " round trip")
        for element in range(NB_ELEMENTS):
            check(
                attribute.value(element) == values[element],
                label + " value() agrees with the array",
            )
        # Copy semantics
        result[:] = values[0]
        check(
            numpy.array_equal(attribute.values_array(), values),
            label + " output is a copy",
        )
        source = values.copy()
        attribute.set_values_array(source)
        source[:] = values[1]
        check(
            numpy.array_equal(attribute.values_array(), values),
            label + " input is a copy",
        )
        # Set through the loop interface, read through the array
        attribute.set_value(2, values[1])
        check(attribute.values_array()[2] == values[1], label + " set_value then array")
        # Shape errors
        for bad_shape in [(NB_ELEMENTS + 1,), (NB_ELEMENTS - 1,), (0,),
                          (NB_ELEMENTS, 1), (1, NB_ELEMENTS)]:
            check_raises(
                ValueError,
                f"[set_values_array] values must have shape ({NB_ELEMENTS},), got",
                lambda bad_shape=bad_shape: attribute.set_values_array(
                    numpy.zeros(bad_shape, dtype=dtype)
                ),
                f"{label} shape {bad_shape}",
            )
        check(
            attribute.value(2) == values[1], label + " errors do not modify the attribute"
        )
        attribute.set_values_array(values)
        # The keyword name is values
        attribute.set_values_array(values=values)


def test_empty_attribute():
    manager = basic.AttributeManager()
    attribute = create_attribute(manager, "double", 0.0)
    check_array(attribute.values_array(), numpy.float64, (0,), "empty")
    attribute.set_values_array(numpy.empty(0))
    vectors = create_vector_attribute(manager, "arraydouble3")
    check_array(vectors.values_array(), numpy.float64, (0, 3), "empty vectors")
    vectors.set_values_array(numpy.empty((0, 3)))


def create_vector_attribute(manager, suffix):
    size = int(suffix[-1])
    create = getattr(manager, f"create_attribute_variable_{suffix}")
    find = getattr(manager, f"find_attribute_variable_{suffix}")
    return find(create("vectors" + suffix, [0.0] * size))


def test_vector_attributes():
    for suffix, size in [("arraydouble2", 2), ("arraydouble3", 3)]:
        manager = basic.AttributeManager()
        manager.resize(NB_ELEMENTS)
        attribute = create_vector_attribute(manager, suffix)
        check_array(
            attribute.values_array(), numpy.float64, (NB_ELEMENTS, size), suffix
        )
        values = numpy.arange(NB_ELEMENTS * size, dtype=numpy.float64).reshape(
            NB_ELEMENTS, size
        ) * 0.5
        attribute.set_values_array(values)
        result = attribute.values_array()
        check(numpy.array_equal(result, values), suffix + " round trip")
        for element in range(NB_ELEMENTS):
            check(
                list(attribute.value(element)) == values[element].tolist(),
                suffix + " value() agrees",
            )
        result[:] = -1
        check(
            numpy.array_equal(attribute.values_array(), values),
            suffix + " copy semantics",
        )
        attribute.set_value(1, [9.0] * size)
        check(
            attribute.values_array()[1].tolist() == [9.0] * size,
            suffix + " set_value then array",
        )
        # Input in other dtypes and layouts
        attribute.set_values_array(values.astype(numpy.float32))
        check(
            numpy.array_equal(attribute.values_array(), values), suffix + " float32"
        )
        attribute.set_values_array(numpy.asfortranarray(values))
        check(
            numpy.array_equal(attribute.values_array(), values), suffix + " fortran"
        )
        wide = numpy.zeros((NB_ELEMENTS, size * 2))
        wide[:, ::2] = values
        attribute.set_values_array(wide[:, ::2])
        check(
            numpy.array_equal(attribute.values_array(), values), suffix + " strided"
        )
        for bad_shape in [(NB_ELEMENTS,), (NB_ELEMENTS, size + 1),
                          (NB_ELEMENTS + 1, size), (NB_ELEMENTS, size, 1)]:
            check_raises(
                ValueError,
                f"[set_values_array] values must have shape ({NB_ELEMENTS}, {size}), got",
                lambda bad_shape=bad_shape: attribute.set_values_array(
                    numpy.zeros(bad_shape)
                ),
                f"{suffix} shape {bad_shape}",
            )
        check_raises(
            ValueError,
            "values must be an array of floats or integers",
            lambda: attribute.set_values_array(
                numpy.zeros((NB_ELEMENTS, size), dtype=bool)
            ),
            suffix + " dtype",
        )
        check(
            numpy.array_equal(attribute.values_array(), values),
            suffix + " errors do not modify the attribute",
        )


def test_input_dtypes():
    manager = basic.AttributeManager()
    manager.resize(3)
    # double and float accept floats and integers of any dtype
    for suffix, expected in [("double", numpy.float64), ("float", numpy.float32)]:
        attribute = create_attribute(manager, suffix, 0.0)
        for dtype in [numpy.float32, numpy.float64, numpy.int8, numpy.int64,
                      numpy.uint8, numpy.uint64]:
            values = numpy.array([1, 2, 3], dtype=dtype)
            attribute.set_values_array(values)
            check(
                numpy.array_equal(attribute.values_array(), values.astype(expected)),
                f"{suffix} from {dtype.__name__}",
            )
        for bad in [numpy.array([True, False, True]), numpy.array([1j, 2, 3]),
                    numpy.array(["a", "b", "c"]), numpy.array([None] * 3, dtype=object)]:
            check_raises(
                ValueError,
                "values must be an array of floats or integers, got dtype",
                lambda bad=bad: attribute.set_values_array(bad),
                f"{suffix} dtype {bad.dtype}",
            )
        # Lists are converted by numpy
        attribute.set_values_array([0.5, 1.5, 2.5])
        check(attribute.values_array().tolist() == [0.5, 1.5, 2.5], suffix + " list")
    # int: integers that fit in 32 bits
    attribute = create_attribute(manager, "int", 0)
    for dtype in [numpy.int8, numpy.int16, numpy.int32, numpy.int64, numpy.uint8,
                  numpy.uint16, numpy.uint32, numpy.uint64]:
        values = numpy.array([1, 2, 3], dtype=dtype)
        attribute.set_values_array(values)
        check(
            numpy.array_equal(attribute.values_array(), values), f"int from {dtype.__name__}"
        )
    attribute.set_values_array(numpy.array([-2 ** 31, 0, 2 ** 31 - 1], dtype=numpy.int64))
    check(
        attribute.values_array().tolist() == [-2 ** 31, 0, 2 ** 31 - 1], "int bounds"
    )
    before = attribute.values_array()
    for bad_value, message in [
        (2 ** 31, "value 2147483648 at position 1 of values does not fit in the 32-bit signed"),
        (-2 ** 31 - 1, "value -2147483649 at position 1 of values does not fit"),
    ]:
        check_raises(
            ValueError,
            message,
            lambda bad_value=bad_value: attribute.set_values_array(
                numpy.array([1, bad_value, 3], dtype=numpy.int64)
            ),
            f"int value {bad_value}",
        )
    check_raises(
        ValueError,
        "value 18446744073709551615 at position 0 of values does not fit",
        lambda: attribute.set_values_array(
            numpy.array([2 ** 64 - 1, 0, 0], dtype=numpy.uint64)
        ),
        "int huge unsigned",
    )
    check(numpy.array_equal(attribute.values_array(), before), "int errors atomic")
    for bad in [numpy.zeros(3, dtype=numpy.float64), numpy.zeros(3, dtype=bool)]:
        check_raises(
            ValueError,
            "values must be an array of integers, got dtype",
            lambda bad=bad: attribute.set_values_array(bad),
            f"int dtype {bad.dtype}",
        )
    # uint: non-negative integers that fit in 32 bits
    attribute = create_attribute(manager, "uint", 0)
    attribute.set_values_array(numpy.array([0, 2 ** 32 - 1, 5], dtype=numpy.int64))
    check(attribute.values_array().tolist() == [0, 2 ** 32 - 1, 5], "uint bounds")
    before = attribute.values_array()
    check_raises(
        ValueError,
        "value -1 at position 2 of values does not fit in the 32-bit unsigned",
        lambda: attribute.set_values_array(numpy.array([1, 2, -1])),
        "uint negative",
    )
    check_raises(
        ValueError,
        "value 4294967296 at position 0 of values does not fit",
        lambda: attribute.set_values_array(numpy.array([2 ** 32, 1, 1], dtype=numpy.uint64)),
        "uint too large",
    )
    check(numpy.array_equal(attribute.values_array(), before), "uint errors atomic")
    # bool: booleans only
    attribute = create_attribute(manager, "bool", False)
    for bad in [numpy.array([1, 0, 1]), numpy.array([1.0, 0.0, 1.0]),
                numpy.array([None] * 3, dtype=object)]:
        check_raises(
            ValueError,
            "values must be an array of booleans, got dtype",
            lambda bad=bad: attribute.set_values_array(bad),
            f"bool dtype {bad.dtype}",
        )
    attribute.set_values_array([True, False, True])
    check(attribute.values_array().tolist() == [True, False, True], "bool list")


def test_float_conversions():
    manager = basic.AttributeManager()
    manager.resize(4)
    attribute = create_attribute(manager, "float", 0.0)
    # Rounded to the nearest float32
    attribute.set_values_array(numpy.array([2.0 ** 24 + 1, 0.1, 1e-50, -3.5]))
    check(
        attribute.values_array().tolist()
        == numpy.array([2.0 ** 24, 0.1, 1e-50, -3.5], dtype=numpy.float32).tolist(),
        "float32 rounding",
    )
    attribute.set_values_array(numpy.array([2 ** 24 + 1, 1, 2, 3], dtype=numpy.int64))
    check(attribute.values_array()[0] == 2.0 ** 24, "integers above 2**24 lose precision")
    # Infinities and NaN are kept
    attribute.set_values_array(numpy.array([numpy.inf, -numpy.inf, numpy.nan, 0.0]))
    result = attribute.values_array()
    check(result[0] == numpy.inf and result[1] == -numpy.inf, "infinities kept")
    check(numpy.isnan(result[2]), "NaN kept")
    # Finite values too large for a float32 are rejected, nothing is modified
    attribute.set_values_array(numpy.array([1.0, 2.0, 3.0, 4.0]))
    for bad in [1e300, -1e39, 3.5e38]:
        check_raises(
            ValueError,
            "at position 1 of values is too large for a float attribute",
            lambda bad=bad: attribute.set_values_array(
                numpy.array([5.0, bad, 7.0, 8.0])
            ),
            f"float overflow {bad}",
        )
    check(attribute.values_array().tolist() == [1.0, 2.0, 3.0, 4.0], "overflow atomic")
    attribute.set_values_array(numpy.array([3.4e38] * 4))
    check(numpy.isfinite(attribute.values_array()).all(), "largest floats accepted")
    # A double attribute keeps every float64, and loses precision above 2**53
    double = create_attribute(manager, "double", 0.0)
    double.set_values_array(numpy.array([1e300, 1.0, 2.0, 3.0]))
    check(double.values_array()[0] == 1e300, "double keeps 1e300")
    double.set_values_array(numpy.array([2 ** 53 + 1, 0, 0, 0], dtype=numpy.int64))
    check(double.values_array()[0] == 2.0 ** 53, "integers above 2**53 lose precision")


def test_empty_values():
    manager = basic.AttributeManager()
    for suffix, default in [("bool", False), ("int", 0), ("uint", 0),
                            ("float", 0.0), ("double", 0.0)]:
        attribute = create_attribute(manager, suffix, default)
        attribute.set_values_array([])
        attribute.set_values_array(numpy.empty(0, dtype=object))
        check(attribute.values_array().shape == (0,), suffix + " empty")
    vectors = create_vector_attribute(manager, "arraydouble2")
    vectors.set_values_array([])
    check(vectors.values_array().shape == (0, 2), "empty vectors")
    # A list that is empty does not hide a size mismatch
    manager.resize(2)
    check_raises(
        ValueError,
        "values must have shape (2,), got (0,)",
        lambda: create_attribute(manager, "double", 0.0).set_values_array([]),
        "empty values for a non empty attribute",
    )


def test_empty_columns_are_not_empty_values():
    manager = basic.AttributeManager()
    attribute = create_attribute(manager, "double", 0.0)
    check_raises(
        ValueError,
        "values must have shape (0,), got (5, 0)",
        lambda: attribute.set_values_array(numpy.empty((5, 0))),
        "values with no column for an empty attribute",
    )
    vectors = create_vector_attribute(manager, "arraydouble2")
    check_raises(
        ValueError,
        "values must have shape (0, 2), got (5, 0)",
        lambda: vectors.set_values_array(numpy.empty((5, 0))),
        "vectors with no column for an empty attribute",
    )
    # No row stays accepted, with any dtype
    vectors.set_values_array(numpy.empty((0, 2), dtype=object))
    check(vectors.values_array().shape == (0, 2), "no row")


def test_attribute_of_a_mesh():
    point_set = mesh.PointSet3D.create()
    builder = mesh.PointSetBuilder3D.create(point_set)
    points = numpy.random.default_rng(0).random((10, 3))
    builder.create_points(points)
    manager = point_set.vertex_attribute_manager()
    attribute = create_attribute(manager, "double", -1.0)
    check_array(attribute.values_array(), numpy.float64, (10,), "vertex attribute")
    attribute.set_values_array(points[:, 0])
    check(
        numpy.array_equal(attribute.values_array(), point_set.points_array()[:, 0]),
        "vertex attribute from coordinates",
    )
    # The attribute grows with the mesh
    builder.create_points(points[:2])
    check_array(attribute.values_array(), numpy.float64, (12,), "grown attribute")
    check(
        attribute.values_array()[10:].tolist() == [-1.0, -1.0],
        "new elements have the default value",
    )
    check_raises(
        ValueError,
        "values must have shape (12,), got (10,)",
        lambda: attribute.set_values_array(points[:, 0]),
        "outdated size",
    )
    # Vector attributes on a surface
    surface = mesh.TriangulatedSurface2D.create()
    surface_builder = mesh.TriangulatedSurfaceBuilder2D.create(surface)
    surface_builder.create_points(points[:, :2])
    surface_builder.create_triangles([[0, 1, 2], [3, 4, 5]])
    polygon_manager = surface.polygon_attribute_manager()
    vectors = create_vector_attribute(polygon_manager, "arraydouble2")
    vectors.set_values_array(numpy.array([[1.0, 2.0], [3.0, 4.0]]))
    check(vectors.value(1)[1] == 4.0, "polygon attribute")


if __name__ == "__main__":
    mesh.OpenGeodeMeshLibrary.initialize()
    test_scalar_attributes()
    test_empty_attribute()
    test_vector_attributes()
    test_input_dtypes()
    test_float_conversions()
    test_empty_values()
    test_empty_columns_are_not_empty_values()
    test_attribute_of_a_mesh()
