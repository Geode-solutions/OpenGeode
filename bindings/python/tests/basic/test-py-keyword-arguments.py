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


"""The arguments named in the signatures can be used as keywords."""

import geode_test_env

import opengeode_py_basic as basic
import opengeode_py_geometry as geometry
import opengeode_py_image as image


def check(condition, message):
    if not condition:
        raise ValueError(f"[Test] {message}")


def check_raises(function, error, message):
    try:
        function()
    except error:
        return
    raise ValueError(f"[Test] {message}")


def test_geometry():
    point = geometry.Point3D([1, 2, 3])
    point.set_value(index=1, coordinate=5)
    check(point.value(index=1) == 5, "Point.value/set_value keywords")
    check(
        point.inexact_equal(other=geometry.Point3D([1, 5, 3])),
        "Point.inexact_equal keyword",
    )
    vector = geometry.Vector3D(from_point=geometry.Point3D(), to_point=point)
    check(vector.dot(other=vector) == vector.length2(), "Vector.dot keyword")
    check(
        vector.cross(other=vector).length() == 0, "Vector.cross keyword"
    )
    check(geometry.Vector2D(values=[3, 4]).length() == 5, "Vector values keyword")
    box = geometry.BoundingBox2D()
    box.add_point(point=geometry.Point2D([0, 0]))
    box.add_point(point=geometry.Point2D([2, 2]))
    check(box.contains_point(point=geometry.Point2D([1, 1])), "contains_point")
    check(
        box.signed_distance(point=geometry.Point2D([3, 2])) == 1,
        "BoundingBox.signed_distance keyword",
    )
    segment = geometry.Segment2D(
        point0=geometry.Point2D([0, 0]), point1=geometry.Point2D([2, 0])
    )
    distance = geometry.point_segment_distance2D(
        point=geometry.Point2D([1, 3]), segment=segment
    )
    check(distance == 3, "point_segment_distance2D keywords")
    other = geometry.Segment2D(
        point0=geometry.Point2D([1, -1]), point1=geometry.Point2D([1, 1])
    )
    result = geometry.segment_segment_intersection2D(
        segment0=segment, segment1=other
    )
    check(result.has_intersection(), "segment_segment_intersection2D keywords")
    check(
        geometry.triangle_area2D(
            triangle=geometry.Triangle2D(
                point0=geometry.Point2D([0, 0]),
                point1=geometry.Point2D([2, 0]),
                point2=geometry.Point2D([0, 2]),
            )
        )
        == 2,
        "triangle_area2D keyword",
    )
    rotated = geometry.rotate(
        point=geometry.Point3D([1, 0, 0]),
        axis=geometry.Vector3D([0, 0, 1]),
        angle=geometry.Angle.create_from_degrees(degrees=90).radians(),
    )
    check(
        rotated.inexact_equal(other=geometry.Point3D([0, 1, 0])),
        "rotate keywords",
    )
    search = geometry.NNSearch2D(
        points=[geometry.Point2D([0, 0]), geometry.Point2D([5, 0])]
    )
    check(
        search.closest_neighbor(point=geometry.Point2D([4, 0])) == 1,
        "NNSearch.closest_neighbor keyword",
    )
    check(
        search.radius_neighbors(
            point=geometry.Point2D([0, 0]), threshold_distance=1
        )
        == [0],
        "NNSearch.radius_neighbors keywords",
    )
    check(
        search.radius_colocated_index_mapping(epsilon=1).nb_unique_points() == 2,
        "NNSearch.radius_colocated_index_mapping keyword",
    )
    # Operators have named arguments too
    check(point.__add__(other=point).value(0) == 2, "Point.__add__ keyword")
    check(point.__mul__(scalar=2).value(0) == 2, "Point.__mul__ keyword")


def test_basic():
    manager = basic.AttributeManager()
    manager.resize(size=3)
    properties = basic.AttributeProperties()
    properties.assignable = True
    values = basic.AttributeValuesInt()
    values.default_value = 7
    attribute_id = manager.create_attribute_variable_int(
        attribute_name="score", default_values=values, properties=properties
    )
    attribute = manager.find_attribute_variable_int(attribute_id=attribute_id)
    attribute.set_value(element=1, value=9)
    check(attribute.value(element=1) == 9, "attribute set_value/value keywords")
    check(manager.attribute_exists(attribute_id=attribute_id), "attribute_exists")
    check_raises(
        lambda: attribute.value(element=3), IndexError, "out of range element"
    )
    mapping = basic.BijectiveMappinguuid()
    first, second = basic.uuid(), basic.uuid()
    check(not mapping.has_mapping_input(input=first), "has_mapping_input keyword")
    percentage = basic.Percentage(value=0.5)
    percentage.set_value(value=0.25)
    check(percentage.value() == 0.25, "Percentage keywords")
    check(
        basic.uuid(string=first.string()) == first, "uuid string keyword"
    )
    files = basic.AdditionalFiles()
    files.optional_files = [basic.AdditionalFile(filename="a.txt", is_missing=True)]
    check(files.has_additional_files(), "AdditionalFile keywords")
    check(files.optional_files[0].filename == "a.txt", "AdditionalFile.filename")
    files.mandatory_files.append(
        basic.AdditionalFile(filename="b.txt", is_missing=False)
    )
    check(len(files.mandatory_files) == 1, "in-place append on a file list")


def test_image():
    raster = image.RasterImage2D(cells_number=[2, 2])
    raster.set_color(index=3, color=image.RGBColor(red=1, green=2, blue=3))
    check(raster.color(index=3).blue() == 3, "RasterImage color keywords")
    check(
        raster.cell_index(index=[1, 1]) == 3,
        "CellArray.cell_index keyword",
    )
    check(
        raster.cell_indices(index=3) == [1, 1], "CellArray.cell_indices keyword"
    )
    check(
        raster.nb_cells_in_direction(direction=0) == 2,
        "CellArray.nb_cells_in_direction keyword",
    )
    check(
        raster.is_cell_on_border(cell_indices=[1, 1]), "is_cell_on_border keyword"
    )
    grey = image.GreyscaleColor(value=3)
    grey.set_value(greyscale=4)
    check(grey.value() == 4, "GreyscaleColor keywords")
    # No file format is registered in this module: the keywords are accepted
    # and the unknown extension is reported
    check_raises(
        lambda: image.is_raster_image_saveable2D(
            raster=raster, filename="a.og_img2d"
        ),
        basic.OpenGeodeDataError,
        "is_raster_image_saveable2D keywords",
    )
    check_raises(
        lambda: image.load_raster_image2D(filename="does_not_exist.og_img2d"),
        basic.OpenGeodeDataError,
        "loading a missing file raises",
    )


if __name__ == "__main__":
    test_geometry()
    test_basic()
    test_image()
