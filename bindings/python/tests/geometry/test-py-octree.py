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
import opengeode_py_geometry as geom
import opengeode_py_mesh as mesh


def check(condition, message):
    if not condition:
        raise ValueError(f"[Test] {message}")


def check_raises(function, error, message):
    try:
        function()
    except error:
        return
    raise ValueError(f"[Test] {message}")


def point(dimension, value):
    return geom.Point2D([value] * 2) if dimension == 2 else geom.Point3D([value] * 3)


def box(dimension, low, high):
    bbox = geom.BoundingBox2D() if dimension == 2 else geom.BoundingBox3D()
    bbox.add_point(point(dimension, low))
    bbox.add_point(point(dimension, high))
    return bbox


def octree_class(dimension):
    return geom.Octree2D if dimension == 2 else geom.Octree3D


def test_construction(dimension):
    cls = octree_class(dimension)
    check(cls.NB_CHILDREN == 2**dimension, "NB_CHILDREN")
    check(cls.NB_FACE_CHILDREN == 2 ** (dimension - 1), "NB_FACE_CHILDREN")
    empty = cls()
    check(empty.nb_cells() == 0 and empty.leaves() == [], "empty octree")
    check(empty.leaf_containing(point(dimension, 0)) is None, "empty leaf_containing")
    check_raises(lambda: empty.is_leaf(0), IndexError, "empty is_leaf index")

    octree = cls(box(dimension, 1, 3))
    check(octree.nb_cells() == 1, "one root cell")
    check(octree.nb_root_cells() == [1] * dimension, "nb_root_cells")
    check(octree.max_depth() == (31 if dimension == 2 else 21), "max_depth")
    check(octree.is_leaf(0) and octree.level(0) == 0, "root leaf")
    check(octree.parent(0) is None and octree.children(0) is None, "root links")
    check(octree.domain_bounding_box().min() == point(dimension, 1), "domain min")
    check(octree.domain_bounding_box().max() == point(dimension, 3), "domain max")
    check(octree.cell_anchor(0) == [0] * dimension, "root anchor")
    check(octree.cell_lengths(0) == [2.0] * dimension, "root lengths")
    check(octree.cell_center(0) == point(dimension, 2), "root center")

    degenerate_max = point(dimension, 1)
    degenerate_max.set_value(0, 0)
    flat = geom.BoundingBox2D() if dimension == 2 else geom.BoundingBox3D()
    flat.add_point(point(dimension, 0))
    flat.add_point(degenerate_max)
    check_raises(lambda: cls(flat), basic.OpenGeodeDataError, "degenerate box")


def test_refine(dimension):
    cls = octree_class(dimension)
    nb_children = cls.NB_CHILDREN
    octree = cls(box(dimension, 0, 2))
    first = octree.refine(0)
    check(first == 1 and octree.nb_cells() == 1 + nb_children, "first refine")
    check(octree.children(0) == 1 and not octree.is_leaf(0), "refined root")
    for child in range(first, first + nb_children):
        check(octree.parent(child) == 0 and octree.level(child) == 1, "child links")
    last = first + nb_children - 1
    check(octree.cell_bounding_box(last).min() == point(dimension, 1), "last child min")
    check(octree.cell_bounding_box(last).max() == point(dimension, 2), "last child max")
    half = 1 << (octree.max_depth() - 1)
    check(octree.cell_anchor(last) == [half] * dimension, "last child anchor")
    check(octree.cell(1, [half] * dimension) == last, "cell lookup")
    check(octree.cell(1, [1] * dimension) is None, "unaligned anchor")
    check(octree.cell(octree.max_depth() + 1, [0] * dimension) is None, "level too deep")

    second = octree.refine(last)
    check(octree.leaves(0) == list(range(1, last)) + list(range(second, second + nb_children)), "subtree leaves")
    check(octree.leaves(1) == [1], "leaves of a leaf")
    check(octree.leaves() == sorted(octree.leaves(0)), "all leaves")
    check(octree.leaf_containing(point(dimension, 1.9)) == second + nb_children - 1, "leaf_containing")
    check(octree.leaf_containing(point(dimension, 3)) is None, "outside point")

    check_raises(lambda: octree.refine(0), basic.OpenGeodeDataError, "refine non-leaf")
    for method in ("is_leaf", "level", "parent", "children", "cell_bounding_box", "cell_center", "cell_lengths", "cell_anchor", "leaves", "refine"):
        check_raises(lambda: getattr(octree, method)(octree.nb_cells()), IndexError, f"{method} index")
    check_raises(lambda: octree.refine(-1), TypeError, "negative index")


def test_traverse(dimension):
    octree = octree_class(dimension)(box(dimension, 0, 2))
    first = octree.refine(0)
    query = box(dimension, 0.1, 0.5)
    visited = []
    octree.traverse(lambda cell, bbox: bbox.intersects_bounding_box(query), visited.append)
    check(visited == [first], "filtered traversal")
    everything = []
    octree.traverse(lambda cell, bbox: True, everything.append)
    check(everything == octree.leaves(0), "full traversal")

    def failing_filter(cell, bbox):
        raise KeyError("filter")

    check_raises(lambda: octree.traverse(failing_filter, everything.append), KeyError, "filter exception propagates")


def test_face_neighbors(dimension):
    cls = octree_class(dimension)
    octree = cls(box(dimension, 0, 2))
    first = octree.refine(0)
    upper = first + 1
    finer = octree.refine(upper)
    lower, side_upper = cls.FaceSide.lower, cls.FaceSide.upper
    check(octree.face_neighbor_leaves(first, 0, lower) == [], "domain border")
    # The lower children of upper along axis 0 touch the face of first
    expected = [finer + c for c in range(cls.NB_CHILDREN) if not c & 1]
    check(octree.face_neighbor_leaves(first, 0, side_upper) == expected, "finer neighbors")
    check(len(expected) == cls.NB_FACE_CHILDREN, "finer neighbor count")
    check(octree.face_neighbor_leaves(finer, 0, lower) == [first], "coarser neighbor")
    check_raises(lambda: octree.face_neighbor_leaves(first, dimension, lower), IndexError, "axis out of range")
    check_raises(lambda: octree.face_neighbor_leaves(octree.nb_cells(), 0, lower), IndexError, "cell out of range")

    deep = octree.refine(finer)
    nb_cells = octree.nb_cells()
    octree.balance(cls.BalanceType.faces)
    check(octree.nb_cells() > nb_cells, "balance refined the coarse leaf")
    check(not octree.is_leaf(first), "balance refined the coarse face neighbor")
    check(octree.level(deep) == 3, "balance kept the existing cells")
    nb_cells = octree.nb_cells()
    octree.balance(cls.BalanceType.faces)
    check(octree.nb_cells() == nb_cells, "balance is idempotent")
    octree.balance(cls.BalanceType.full)
    check(octree.nb_cells() >= nb_cells, "full balance")


def test_attributes(dimension):
    octree = octree_class(dimension)(box(dimension, 0, 2))
    manager = octree.cell_attribute_manager()
    attribute_id = manager.create_attribute_variable_double("metric", -1.0)
    attribute = manager.find_attribute_variable_double(attribute_id)
    attribute.set_value(0, 4.0)
    first = octree.refine(0)
    check(manager.nb_elements() == octree.nb_cells(), "attribute size follows refine")
    for child in range(first, first + octree.NB_CHILDREN):
        check(attribute.value(child) == 4.0, "children inherit the value")
    attribute.set_value(first, 2.0)
    del octree
    check(manager.nb_elements() == 1 + 2**dimension, "manager keeps the octree alive")


def leaf_metric(octree):
    manager = octree.cell_attribute_manager()
    attribute_id = manager.create_attribute_variable_double("metric", -1.0)
    attribute = manager.find_attribute_variable_double(attribute_id)
    for leaf in octree.leaves():
        attribute.set_value(leaf, 0.5 + leaf)


def check_converted(octree, converted, nb_elements, element_manager, nb_vertices):
    leaves = octree.leaves()
    check(nb_elements == len(leaves), "one element per leaf")
    check(converted.nb_vertices() == nb_vertices, "shared corners")
    manager = element_manager
    attribute_id = manager.attribute_ids_matching_name("metric")[0]
    attribute = manager.find_read_only_attribute_double(attribute_id)
    for element, leaf in enumerate(leaves):
        check(attribute.value(element) == 0.5 + leaf, "leaf attribute transferred")


def test_conversions():
    quadtree = geom.Octree2D(box(2, 0, 2))
    quadtree.refine(quadtree.refine(0))
    leaf_metric(quadtree)
    surface = mesh.convert_octree_into_polygonal_surface(octree=quadtree)
    check(isinstance(surface, mesh.PolygonalSurface2D), "surface type")
    # 3x3 corners of the first level, 3x3 - 1 more in the refined child
    check_converted(quadtree, surface, surface.nb_polygons(), surface.polygon_attribute_manager(), 9 + 5)
    for polygon in range(surface.nb_polygons()):
        check(surface.nb_polygon_vertices(polygon) == 4, "quadrangles")

    octree = geom.Octree3D(box(3, 0, 2))
    octree.refine(octree.refine(0))
    leaf_metric(octree)
    solid = mesh.convert_octree_into_hybrid_solid(octree=octree)
    check(isinstance(solid, mesh.HybridSolid3D), "solid type")
    check_converted(octree, solid, solid.nb_polyhedra(), solid.polyhedron_attribute_manager(), 27 + 19)
    for polyhedron in range(solid.nb_polyhedra()):
        check(solid.nb_polyhedron_vertices(polyhedron) == 8, "hexahedra")

    check_raises(lambda: mesh.convert_octree_into_hybrid_solid(quadtree), TypeError, "2D octree into a solid")
    empty = mesh.convert_octree_into_polygonal_surface(geom.Octree2D())
    check(empty.nb_polygons() == 0 and empty.nb_vertices() == 0, "empty octree conversion")


if __name__ == "__main__":
    basic.OpenGeodeBasicLibrary.initialize()
    geom.OpenGeodeGeometryLibrary.initialize()
    mesh.OpenGeodeMeshLibrary.initialize()
    for dimension in (2, 3):
        test_construction(dimension)
        test_refine(dimension)
        test_traverse(dimension)
        test_face_neighbors(dimension)
        test_attributes(dimension)
    test_conversions()
