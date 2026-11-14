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
import opengeode_py_image as image
import opengeode_py_mesh as mesh
import opengeode_py_model as model


def check(condition, message):
    if not condition:
        raise ValueError("[Test] " + message)


def check_index_error(function, expected_message, label):
    try:
        function()
    except IndexError as error:
        check(
            expected_message in str(error),
            label
            + ": unexpected message '"
            + str(error)
            + "', expected '"
            + expected_message
            + "'",
        )
        return
    except Exception as error:
        raise ValueError(
            "[Test] " + label + ": expected IndexError, got " + repr(error)
        )
    raise ValueError("[Test] " + label + ": expected IndexError, nothing raised")


def create_triangle():
    surface = mesh.TriangulatedSurface3D.create()
    builder = mesh.TriangulatedSurfaceBuilder3D.create(surface)
    builder.create_point(geom.Point3D([0, 0, 0]))
    builder.create_point(geom.Point3D([1, 0, 0]))
    builder.create_point(geom.Point3D([0, 1, 0]))
    builder.create_triangle([0, 1, 2])
    return surface, builder


def create_tetrahedron():
    solid = mesh.TetrahedralSolid3D.create()
    builder = mesh.TetrahedralSolidBuilder3D.create(solid)
    builder.create_point(geom.Point3D([0, 0, 0]))
    builder.create_point(geom.Point3D([1, 0, 0]))
    builder.create_point(geom.Point3D([0, 1, 0]))
    builder.create_point(geom.Point3D([0, 0, 1]))
    builder.create_tetrahedron([0, 1, 2, 3])
    return solid, builder


def test_point():
    point_set = mesh.PointSet3D.create()
    builder = mesh.PointSetBuilder3D.create(point_set)
    builder.create_point(geom.Point3D([1, 2, 3]))
    check(point_set.point(0).value(2) == 3, "Valid point should be unchanged")
    check_index_error(
        lambda: point_set.point(5), "[point] vertex 5 out of range [0, 1)", "point"
    )
    check_index_error(
        lambda: builder.set_point(5, geom.Point3D([0, 0, 0])),
        "[set_point] vertex 5 out of range [0, 1)",
        "set_point",
    )
    builder.set_point(0, geom.Point3D([4, 5, 6]))
    check(point_set.point(0).value(0) == 4, "Valid set_point should apply")
    crs = point_set.main_coordinate_reference_system_manager().active_coordinate_reference_system()
    check(crs.point(0).value(1) == 5, "Valid CRS point should be unchanged")
    check_index_error(
        lambda: crs.point(5), "[point] point 5 out of range [0, 1)", "crs point"
    )
    check_index_error(
        lambda: crs.set_point(5, geom.Point3D([0, 0, 0])),
        "[set_point] point 5 out of range [0, 1)",
        "crs set_point",
    )


def test_surface():
    surface, builder = create_triangle()
    check(surface.polygon_vertex(mesh.PolygonVertex(0, 2)) == 2, "Valid polygon_vertex")
    check_index_error(
        lambda: surface.polygon_vertex(mesh.PolygonVertex(4, 0)),
        "[polygon_vertex] polygon 4 out of range [0, 1)",
        "polygon_vertex polygon",
    )
    check_index_error(
        lambda: surface.polygon_vertex(mesh.PolygonVertex(0, 3)),
        "[polygon_vertex] local vertex 3 out of range [0, 3)",
        "polygon_vertex local vertex",
    )
    check_index_error(
        lambda: surface.polygon_adjacent(mesh.PolygonEdge(1, 0)),
        "[polygon_adjacent] polygon 1 out of range [0, 1)",
        "polygon_adjacent",
    )
    check_index_error(
        lambda: surface.polygon_area(7), "[polygon_area] polygon 7", "polygon_area"
    )
    check_index_error(
        lambda: surface.polygons_around_vertex(9),
        "[polygons_around_vertex] vertex 9 out of range [0, 3)",
        "polygons_around_vertex",
    )
    check_index_error(
        lambda: surface.edge_length([0, 8]),
        "[edge_length] vertex 8 out of range [0, 3)",
        "edge_length",
    )
    check_index_error(
        lambda: surface.polygon_normal(3), "[polygon_normal] polygon 3", "normal"
    )
    check_index_error(
        lambda: builder.set_polygon_vertex(mesh.PolygonVertex(0, 0), 10),
        "[set_polygon_vertex] vertex 10 out of range [0, 3)",
        "set_polygon_vertex vertex",
    )
    check_index_error(
        lambda: builder.set_polygon_vertex(mesh.PolygonVertex(2, 0), 1),
        "[set_polygon_vertex] polygon 2 out of range [0, 1)",
        "set_polygon_vertex polygon",
    )
    check_index_error(
        lambda: builder.set_polygon_adjacent(mesh.PolygonEdge(0, 0), 4),
        "[set_polygon_adjacent] polygon 4 out of range [0, 1)",
        "set_polygon_adjacent",
    )
    check(surface.polygon_vertex(mesh.PolygonVertex(0, 0)) == 0, "Surface unchanged")
    check(surface.polygon_area(0) == 0.5, "Valid polygon_area")


def test_solid():
    solid, builder = create_tetrahedron()
    check(
        solid.polyhedron_vertex(mesh.PolyhedronVertex(0, 3)) == 3,
        "Valid polyhedron_vertex",
    )
    check_index_error(
        lambda: solid.polyhedron_vertex(mesh.PolyhedronVertex(2, 0)),
        "[polyhedron_vertex] polyhedron 2 out of range [0, 1)",
        "polyhedron_vertex",
    )
    check_index_error(
        lambda: solid.polyhedron_vertex(mesh.PolyhedronVertex(0, 4)),
        "[polyhedron_vertex] local vertex 4 out of range [0, 4)",
        "polyhedron_vertex local",
    )
    check_index_error(
        lambda: solid.polyhedron_adjacent(mesh.PolyhedronFacet(0, 4)),
        "[polyhedron_adjacent] local facet 4 out of range [0, 4)",
        "polyhedron_adjacent",
    )
    check_index_error(
        lambda: solid.polyhedron_volume(1),
        "[polyhedron_volume] polyhedron 1",
        "polyhedron_volume",
    )
    check_index_error(
        lambda: builder.set_polyhedron_vertex(mesh.PolyhedronVertex(0, 0), 8),
        "[set_polyhedron_vertex] vertex 8 out of range [0, 4)",
        "set_polyhedron_vertex",
    )
    check_index_error(
        lambda: builder.set_point(6, geom.Point3D([0, 0, 0])),
        "[set_point] vertex 6 out of range [0, 4)",
        "solid set_point",
    )


def test_no_id():
    # NO_ID is not accepted as a vertex by the C++ setters (it crashes), so it
    # is reported as out of range instead.
    surface, builder = create_triangle()
    check_index_error(
        lambda: builder.set_polygon_vertex(mesh.PolygonVertex(0, 0), basic.NO_ID),
        "[set_polygon_vertex] vertex 4294967295 out of range [0, 3)",
        "set_polygon_vertex NO_ID",
    )
    check_index_error(
        lambda: surface.polygon_vertex(mesh.PolygonVertex(basic.NO_ID, 0)),
        "[polygon_vertex] polygon 4294967295 out of range [0, 1)",
        "polygon_vertex NO_ID",
    )
    solid, solid_builder = create_tetrahedron()
    check_index_error(
        lambda: solid_builder.set_polyhedron_vertex(
            mesh.PolyhedronVertex(0, 0), basic.NO_ID
        ),
        "[set_polyhedron_vertex] vertex 4294967295 out of range [0, 4)",
        "set_polyhedron_vertex NO_ID",
    )
    check(surface.polygon_vertex(mesh.PolygonVertex(0, 0)) == 0, "Surface unchanged")


def test_model_builder():
    brep = model.BRep()
    brep_builder = model.BRepBuilder(brep)
    surface_id = brep_builder.add_surface()
    builder = brep_builder.surface_mesh_builder(brep.surface(surface_id))
    builder.create_point(geom.Point3D([0, 0, 0]))
    builder.create_point(geom.Point3D([1, 0, 0]))
    builder.create_point(geom.Point3D([0, 1, 0]))
    builder.create_polygon([0, 1, 2])
    check_index_error(
        lambda: builder.set_point(8, geom.Point3D([0, 0, 0])),
        "[set_point] vertex 8 out of range [0, 3)",
        "model builder set_point",
    )
    check_index_error(
        lambda: builder.set_polygon_vertex(mesh.PolygonVertex(0, 0), 8),
        "[set_polygon_vertex] vertex 8 out of range [0, 3)",
        "model builder set_polygon_vertex",
    )
    builder.set_point(1, geom.Point3D([2, 0, 0]))
    check(
        brep.surface(surface_id).mesh().point(1).value(0) == 2,
        "Valid model builder set_point should apply",
    )


def test_graph():
    curve = mesh.EdgedCurve3D.create()
    builder = mesh.EdgedCurveBuilder3D.create(curve)
    builder.create_point(geom.Point3D([0, 0, 0]))
    builder.create_point(geom.Point3D([1, 0, 0]))
    builder.create_edge_with_vertices(0, 1)
    check(curve.edge_vertex(mesh.EdgeVertex(0, 1)) == 1, "Valid edge_vertex")
    check(curve.edge_length(0) == 1, "Valid edge_length")
    check_index_error(
        lambda: curve.edge_vertex(mesh.EdgeVertex(3, 0)),
        "[edge_vertex] edge 3 out of range [0, 1)",
        "edge_vertex",
    )
    check_index_error(
        lambda: curve.edges_around_vertex(2),
        "[edges_around_vertex] vertex 2 out of range [0, 2)",
        "edges_around_vertex",
    )
    check_index_error(
        lambda: curve.segment(1), "[segment] edge 1 out of range [0, 1)", "segment"
    )
    check_index_error(
        lambda: builder.set_edge_vertex(mesh.EdgeVertex(0, 0), 5),
        "[set_edge_vertex] vertex 5 out of range [0, 2)",
        "set_edge_vertex",
    )
    check_index_error(
        lambda: builder.set_edge_vertex(mesh.EdgeVertex(1, 0), 1),
        "[set_edge_vertex] edge 1 out of range [0, 1)",
        "set_edge_vertex edge",
    )


def test_attribute():
    manager = basic.AttributeManager()
    manager.resize(3)
    attribute_id = manager.create_attribute_variable_double("values", 0.5)
    attribute = manager.find_attribute_variable_double(attribute_id)
    read_only = manager.find_read_only_attribute_double(attribute_id)
    attribute.set_value(1, 2.0)
    check(read_only.value(1) == 2.0, "Valid attribute value should be unchanged")
    check_index_error(
        lambda: read_only.value(3),
        "[value] element 3 out of range [0, 3)",
        "attribute value",
    )
    check_index_error(
        lambda: attribute.set_value(7, 1.0),
        "[set_value] element 7 out of range [0, 3)",
        "attribute set_value",
    )
    constant_id = manager.create_attribute_constant_double("constant", 1.5)
    constant = manager.find_read_only_attribute_double(constant_id)
    check(constant.value(1000) == 1.5, "Constant attribute accepts any element")


def surface_of(mesh_class, builder_class, dimension):
    surface = mesh_class.create()
    builder = builder_class.create(surface)
    for point in [[0, 0, 0], [1, 0, 0], [0, 1, 0]]:
        builder.create_point(getattr(geom, "Point%dD" % dimension)(point[:dimension]))
    return surface, builder


def test_geometric_elements():
    for dimension in [2, 3]:
        surface, builder = surface_of(
            getattr(mesh, "TriangulatedSurface%dD" % dimension),
            getattr(mesh, "TriangulatedSurfaceBuilder%dD" % dimension),
            dimension,
        )
        builder.create_triangle([0, 1, 2])
        check(surface.triangle(0).vertices()[1].value(0) == 1, "Valid triangle")
        for triangle_id in [1, 10**7, basic.NO_ID]:
            check_index_error(
                lambda: surface.triangle(triangle_id),
                "[triangle] polygon %d out of range [0, 1)" % triangle_id,
                "triangle %dD" % dimension,
            )
    solid, _ = create_tetrahedron()
    check(solid.tetrahedron(0).vertices()[3].value(2) == 1, "Valid tetrahedron")
    check(
        solid.triangle(mesh.PolyhedronFacet(0, 3)).vertices()[0] is not None,
        "Valid tetrahedron facet triangle",
    )
    check_index_error(
        lambda: solid.tetrahedron(10**7),
        "[tetrahedron] polyhedron 10000000 out of range [0, 1)",
        "tetrahedron",
    )
    check_index_error(
        lambda: solid.triangle(mesh.PolyhedronFacet(1, 0)),
        "[triangle] polyhedron 1 out of range [0, 1)",
        "tetrahedron triangle polyhedron",
    )
    check_index_error(
        lambda: solid.triangle(mesh.PolyhedronFacet(0, 4)),
        "[triangle] local facet 4 out of range [0, 4)",
        "tetrahedron triangle local facet",
    )


def test_edges_and_facets():
    for dimension in [2, 3]:
        for prefix in ["TriangulatedSurface", "PolygonalSurface"]:
            surface, builder = surface_of(
                getattr(mesh, "%s%dD" % (prefix, dimension)),
                getattr(mesh, "%sBuilder%dD" % (prefix, dimension)),
                dimension,
            )
            builder.create_polygon([0, 1, 2])
            surface.enable_edges()
            edges = surface.edges()
            check(edges.nb_edges() == 3, "Surface edges")
            check(len(edges.edge_vertices(2)) == 2, "Valid surface edge_vertices")
            for edge_id in [3, 10**7]:
                check_index_error(
                    lambda: edges.edge_vertices(edge_id),
                    "[edge_vertices] edge %d out of range [0, 3)" % edge_id,
                    "%s%dD edge_vertices" % (prefix, dimension),
                )
            check(edges.edge_from_vertices([0, 99]) is None, "Unknown edge vertices")
    solids = [create_tetrahedron()[0]]
    polyhedral = mesh.PolyhedralSolid3D.create()
    polyhedral_builder = mesh.PolyhedralSolidBuilder3D.create(polyhedral)
    for point in [[0, 0, 0], [1, 0, 0], [0, 1, 0], [0, 0, 1]]:
        polyhedral_builder.create_point(geom.Point3D(point))
    polyhedral_builder.create_polyhedron(
        [0, 1, 2, 3], [[1, 3, 2], [0, 2, 3], [3, 1, 0], [0, 1, 2]]
    )
    solids.append(polyhedral)
    for solid in solids:
        label = type(solid).__name__
        solid.enable_edges()
        solid.enable_facets()
        edges = solid.edges()
        facets = solid.facets()
        check(len(edges.edge_vertices(5)) == 2, label + " valid edge_vertices")
        check(len(facets.facet_vertices(3)) == 3, label + " valid facet_vertices")
        check_index_error(
            lambda: edges.edge_vertices(6),
            "[edge_vertices] edge 6 out of range [0, 6)",
            label + " edge_vertices",
        )
        check_index_error(
            lambda: facets.facet_vertices(10**7),
            "[facet_vertices] facet 10000000 out of range [0, 4)",
            label + " facet_vertices",
        )


def test_element_builders():
    for dimension in [2, 3]:
        surface, builder = surface_of(
            getattr(mesh, "TriangulatedSurface%dD" % dimension),
            getattr(mesh, "TriangulatedSurfaceBuilder%dD" % dimension),
            dimension,
        )
        for vertices in [[0, 1, 3], [10**7, 1, 2], [0, basic.NO_ID, 2]]:
            check_index_error(
                lambda: builder.create_triangle(vertices),
                "[create_triangle] vertex %d out of range [0, 3)"
                % [v for v in vertices if v >= 3][0],
                "create_triangle %dD" % dimension,
            )
        check(surface.nb_polygons() == 0, "No triangle created")
        check(builder.create_triangle([0, 1, 2]) == 0, "Valid create_triangle")
    solid = mesh.TetrahedralSolid3D.create()
    builder = mesh.TetrahedralSolidBuilder3D.create(solid)
    for point in [[0, 0, 0], [1, 0, 0], [0, 1, 0], [0, 0, 1]]:
        builder.create_point(geom.Point3D(point))
    check_index_error(
        lambda: builder.create_tetrahedron([0, 1, 2, 4]),
        "[create_tetrahedron] vertex 4 out of range [0, 4)",
        "create_tetrahedron",
    )
    check(solid.nb_polyhedra() == 0, "No tetrahedron created")
    check(builder.create_tetrahedron([0, 1, 2, 3]) == 0, "Valid create_tetrahedron")
    hybrid = mesh.HybridSolid3D.create()
    hybrid_builder = mesh.HybridSolidBuilder3D.create(hybrid)
    for x in range(2):
        for y in range(2):
            for z in range(2):
                hybrid_builder.create_point(geom.Point3D([x, y, z]))
    for name, count in [
        ("create_tetrahedron", 4),
        ("create_hexahedron", 8),
        ("create_prism", 6),
        ("create_pyramid", 5),
    ]:
        function = getattr(hybrid_builder, name)
        check_index_error(
            lambda: function(list(range(count - 1)) + [8]),
            "[%s] vertex 8 out of range [0, 8)" % name,
            "hybrid " + name,
        )
        check_index_error(
            lambda: function([10**7] + list(range(1, count))),
            "[%s] vertex 10000000 out of range [0, 8)" % name,
            "hybrid " + name + " first",
        )
    check(hybrid.nb_polyhedra() == 0, "No hybrid polyhedron created")
    check(hybrid_builder.create_hexahedron([0, 4, 6, 2, 1, 5, 7, 3]) == 0, "Valid hexahedron")
    solid, builder = create_tetrahedron()
    check_index_error(
        lambda: builder.associate_polyhedron_vertex_to_vertex(
            mesh.PolyhedronVertex(10**7, 0), 0
        ),
        "[associate_polyhedron_vertex_to_vertex] polyhedron 10000000 out of range [0, 1)",
        "associate polyhedron",
    )
    check_index_error(
        lambda: builder.associate_polyhedron_vertex_to_vertex(
            mesh.PolyhedronVertex(0, 4), 0
        ),
        "[associate_polyhedron_vertex_to_vertex] local vertex 4 out of range [0, 4)",
        "associate local vertex",
    )
    check_index_error(
        lambda: builder.associate_polyhedron_vertex_to_vertex(
            mesh.PolyhedronVertex(0, 0), 4
        ),
        "[associate_polyhedron_vertex_to_vertex] vertex 4 out of range [0, 4)",
        "associate vertex",
    )
    builder.associate_polyhedron_vertex_to_vertex(mesh.PolyhedronVertex(0, 1), 1)
    check(
        solid.polyhedron_around_vertex(1) == mesh.PolyhedronVertex(0, 1),
        "Valid associate_polyhedron_vertex_to_vertex",
    )


def test_crs_managers():
    for dimension in [2, 3]:
        surface, _ = surface_of(
            getattr(mesh, "TriangulatedSurface%dD" % dimension),
            getattr(mesh, "TriangulatedSurfaceBuilder%dD" % dimension),
            dimension,
        )
        point_class = getattr(geom, "Point%dD" % dimension)
        crs_builder = getattr(
            mesh, "CoordinateReferenceSystemManagersBuilder%dD" % dimension
        )(surface)
        for vertex in [3, 10**7]:
            check_index_error(
                lambda: crs_builder.set_point(vertex, point_class([5] * dimension)),
                "[set_point] point %d out of range [0, 3)" % vertex,
                "bare CRS managers builder set_point %dD" % dimension,
            )
            check_index_error(
                lambda: getattr(
                    mesh, "CoordinateReferenceSystemManagers%dD" % dimension
                ).point(surface, vertex),
                "[point] point %d out of range [0, 3)" % vertex,
                "CRS managers point %dD" % dimension,
            )
        crs_builder.set_point(2, point_class([5] * dimension))
        check(surface.point(2).value(0) == 5, "Valid bare CRS managers set_point")
        check(
            getattr(mesh, "CoordinateReferenceSystemManagers%dD" % dimension)
            .point(surface, 2)
            .value(0)
            == 5,
            "Valid CRS managers point",
        )


def test_generic_attribute():
    surface, _ = create_triangle()
    manager = surface.vertex_attribute_manager()
    double_id = manager.create_attribute_variable_double("double", 0.5)
    array_id = manager.create_attribute_variable_arraydouble3("array", [1.0, 2.0, 3.0])
    constant_id = manager.create_attribute_constant_double("constant", 1.5)
    sparse_id = manager.create_attribute_sparse_double("sparse", 2.5)
    generic = manager.find_generic_attribute(double_id)
    check(generic.generic_value(2) == 0.5, "Valid generic_value")
    for element in [3, 10**8]:
        check_index_error(
            lambda: generic.generic_value(element),
            "[generic_value] element %d out of range [0, 3)" % element,
            "generic_value",
        )
        check_index_error(
            lambda: generic.generic_item_value(element, 0),
            "[generic_item_value] element %d out of range [0, 3)" % element,
            "generic_item_value element",
        )
        check_index_error(
            lambda: manager.find_attribute_variable_double(double_id).generic_value(element),
            "[generic_value] element %d out of range [0, 3)" % element,
            "typed attribute generic_value",
        )
    check_index_error(
        lambda: generic.generic_item_value(0, 1),
        "[generic_item_value] item 1 out of range [0, 1)",
        "generic_item_value item of a scalar",
    )
    array = manager.find_generic_attribute(array_id)
    check(array.generic_item_value(1, 2) == 3.0, "Valid generic_item_value")
    check_index_error(
        lambda: array.generic_item_value(1, 3),
        "[generic_item_value] item 3 out of range [0, 3)",
        "generic_item_value item",
    )
    check_index_error(
        lambda: array.generic_item_value(1, 200),
        "[generic_item_value] item 200 out of range [0, 3)",
        "generic_item_value large item",
    )
    # Constant and sparse attributes have a value for any element
    check(
        manager.find_generic_attribute(constant_id).generic_value(1000) == 1.5,
        "Constant generic_value",
    )
    check(
        manager.find_generic_attribute(sparse_id).generic_value(1000) == 2.5,
        "Sparse generic_value",
    )
    # The coordinates have no binding: the size of their manager is used
    point_set = mesh.PointSet3D.create()
    builder = mesh.PointSetBuilder3D.create(point_set)
    builder.create_point(geom.Point3D([1, 2, 3]))
    builder.create_point(geom.Point3D([4, 5, 6]))
    points_manager = point_set.vertex_attribute_manager()
    points_id = [
        attribute_id
        for attribute_id in points_manager.attribute_ids()
        if points_manager.find_generic_attribute(attribute_id).name() == "points"
    ][0]
    points = points_manager.find_generic_attribute(points_id)
    check(type(points) is basic.AttributeBase, "Points attribute has no binding")
    check(points.generic_item_value(1, 2) == 6, "Valid points generic_item_value")
    check_index_error(
        lambda: points.generic_value(2),
        "[generic_value] element 2 out of range [0, 2)",
        "points generic_value",
    )
    builder.create_point(geom.Point3D([7, 8, 9]))
    check(points.generic_item_value(2, 0) == 7, "Points generic_value after resize")
    check_index_error(
        lambda: points.generic_item_value(1, 3),
        "[generic_item_value] item 3 out of range [0, 3)",
        "points generic_item_value item",
    )
    points_manager.delete_attribute(points_id)
    try:
        points.generic_value(0)
    except ValueError as error:
        check(
            "the attribute is no longer in its attribute manager" in str(error),
            "Deleted unbound attribute message: " + str(error),
        )
    else:
        raise ValueError("[Test] Deleted unbound attribute should not be checked")
    detached = basic.AttributeManager()
    detached.resize(2)
    detached_id = detached.create_attribute_variable_double("x", 1.0)
    attribute = detached.find_generic_attribute(detached_id)
    detached.delete_attribute(detached_id)
    detached.resize(10)
    check(attribute.generic_value(1) == 1.0, "Deleted bound attribute keeps its size")
    check_index_error(
        lambda: attribute.generic_value(5),
        "[generic_value] element 5 out of range [0, 2)",
        "deleted attribute generic_value",
    )


def test_time_series():
    manager = basic.AttributeManager()
    manager.resize(3)
    properties = basic.AttributeProperties()
    manager.create_time_step_attribute_variable_double(
        "series", 1.0, basic.AttributeValuesDouble(), properties
    )
    values = basic.AttributeValuesDouble()
    values.default_value = 4.0
    manager.create_time_step_attribute_constant_double("series", 2.0, values, properties)
    series = basic.AttributeTimeSeriesDouble(manager, "series")
    check(series.nb_time_steps() == 2, "Time steps")
    check(series.value(1, 1000) == 4.0, "Constant step accepts any element")
    check(list(series.element_values(2)) == [0.0, 4.0], "Valid element_values")
    for element in [3, 10**7]:
        check_index_error(
            lambda: series.value(0, element),
            "[value] element %d out of range [0, 3)" % element,
            "time series value",
        )
        check_index_error(
            lambda: series.element_values(element),
            "[element_values] element %d out of range [0, 3)" % element,
            "time series element_values",
        )
    check_index_error(
        lambda: series.value(2, 0), "[value] step 2 out of range [0, 2)", "time series step"
    )


def grids():
    result = []
    for dimension in [2, 3]:
        point = getattr(geom, "Point%dD" % dimension)([0] * dimension)
        grid = getattr(mesh, "RegularGrid%dD" % dimension).create()
        getattr(mesh, "RegularGridBuilder%dD" % dimension).create(grid).initialize_grid(
            point, [2] * dimension, [1.0] * dimension
        )
        result.append((grid, dimension))
        result.append(
            (
                getattr(mesh, "LightRegularGrid%dD" % dimension)(
                    point, [2] * dimension, [1.0] * dimension
                ),
                dimension,
            )
        )
    return result


def test_grid_directions():
    for grid, dimension in grids():
        label = type(grid).__name__
        origin = [0] * dimension
        check(grid.cell_length_in_direction(dimension - 1) == 1, label + " cell length")
        check(grid.nb_vertices_in_direction(0) == 3, label + " nb vertices")
        check(grid.nb_cells_in_direction(0) == 2, label + " nb cells")
        check(grid.next_cell(origin, 0) == [1] + [0] * (dimension - 1), label + " next_cell")
        check(grid.previous_cell(origin, 0) is None, label + " previous_cell")
        check(grid.next_vertex([2] * dimension, 0) is None, label + " next_vertex")
        check(
            grid.previous_vertex([2] * dimension, 0) == [1] + [2] * (dimension - 1),
            label + " previous_vertex",
        )
        check(
            grid.cell_vertex_indices(origin, 2**dimension - 1) == [1] * dimension,
            label + " cell_vertex_indices",
        )
        for direction in [dimension, 200]:
            expected = "direction %d out of range [0, %d)" % (direction, dimension)
            for name, call in [
                ("cell_length_in_direction", lambda: grid.cell_length_in_direction(direction)),
                ("nb_vertices_in_direction", lambda: grid.nb_vertices_in_direction(direction)),
                ("nb_cells_in_direction", lambda: grid.nb_cells_in_direction(direction)),
                ("next_cell", lambda: grid.next_cell(origin, direction)),
                ("previous_cell", lambda: grid.previous_cell(origin, direction)),
                ("next_vertex", lambda: grid.next_vertex(origin, direction)),
                ("previous_vertex", lambda: grid.previous_vertex(origin, direction)),
            ]:
                check_index_error(call, "[%s] %s" % (name, expected), label + " " + name)
        last = dimension - 1
        cell = [0] * last + [2]
        vertex = [0] * last + [3]
        for name, call, expected in [
            ("next_cell", lambda: grid.next_cell(cell, 0), "cell index 2 out of range [0, 2)"),
            ("previous_cell", lambda: grid.previous_cell(cell, 0), "cell index 2 out of range [0, 2)"),
            ("next_vertex", lambda: grid.next_vertex(vertex, 0), "vertex index 3 out of range [0, 3)"),
            ("previous_vertex", lambda: grid.previous_vertex(vertex, 0), "vertex index 3 out of range [0, 3)"),
        ]:
            check_index_error(
                call,
                "[%s] %s in direction %d" % (name, expected, last),
                label + " " + name + " index",
            )
        check_index_error(
            lambda: grid.cell_vertex_indices(origin, 2**dimension),
            "[cell_vertex_indices] local vertex %d out of range [0, %d)"
            % (2**dimension, 2**dimension),
            label + " cell_vertex_indices",
        )
    raster = image.RasterImage2D([2, 3])
    check(raster.nb_cells_in_direction(1) == 3, "Raster nb cells")
    check_index_error(
        lambda: raster.nb_cells_in_direction(2),
        "[nb_cells_in_direction] direction 2 out of range [0, 2)",
        "raster nb_cells_in_direction",
    )
    check_index_error(
        lambda: raster.next_cell([0, 3], 1),
        "[next_cell] cell index 3 out of range [0, 3) in direction 1",
        "raster next_cell",
    )


def test_euclidean_distance_transform():
    for grid, dimension in grids():
        if not isinstance(grid, mesh.RegularGrid2D) and not isinstance(grid, mesh.RegularGrid3D):
            continue
        function = getattr(mesh, "euclidean_distance_transform%dD" % dimension)
        nb_attributes = len(grid.cell_attribute_manager().attribute_ids())
        for cell, direction, index in [
            ([2] + [0] * (dimension - 1), 0, 2),
            ([0] * (dimension - 1) + [10**7], dimension - 1, 10**7),
        ]:
            check_index_error(
                lambda: function(grid, [[0] * dimension, cell], "distance"),
                "[euclidean_distance_transform] cell index %d out of range [0, 2) in direction %d"
                % (index, direction),
                "euclidean_distance_transform %dD" % dimension,
            )
        check(
            len(grid.cell_attribute_manager().attribute_ids()) == nb_attributes,
            "No distance attribute created",
        )
        distances = function(grid, [[0] * dimension], "distance")
        check(distances.value(0) == 0, "Valid euclidean_distance_transform")


def test_texture():
    surface, _ = create_triangle()
    curve = mesh.EdgedCurve2D.create()
    curve_builder = mesh.EdgedCurveBuilder2D.create(curve)
    curve_builder.create_point(geom.Point2D([0, 0]))
    curve_builder.create_point(geom.Point2D([1, 0]))
    curve_builder.create_edge_with_vertices(0, 1)
    solid, _ = create_tetrahedron()
    for target, element, name, dimension in [
        (surface, mesh.PolygonVertex, "polygon", 2),
        (curve, mesh.EdgeVertex, "edge", 1),
        (solid, mesh.PolyhedronVertex, "polyhedron", 3),
    ]:
        label = type(target).__name__
        manager = target.texture_manager()
        texture = manager.find_or_create_texture("texture")
        coordinates = getattr(geom, "Point%dD" % dimension)([0.5] * dimension)
        texture.set_texture_coordinates(element(0, 1), coordinates)
        check(texture.texture_coordinates(element(0, 1)) == coordinates, label + " texture")
        found = target.texture_manager().find_texture("texture")
        check(found.texture_coordinates(element(0, 1)) == coordinates, label + " found texture")
        for texture_object in [texture, found]:
            check_index_error(
                lambda: texture_object.texture_coordinates(element(1, 0)),
                "[texture_coordinates] %s 1 out of range [0, 1)" % name,
                label + " texture_coordinates",
            )
            check_index_error(
                lambda: texture_object.set_texture_coordinates(element(10**7, 0), coordinates),
                "[set_texture_coordinates] %s 10000000 out of range [0, 1)" % name,
                label + " set_texture_coordinates",
            )


def test_component_mesh_polygons():
    brep = model.BRep()
    builder = model.BRepBuilder(brep)
    surface = brep.surface(builder.add_surface())
    line = brep.line(builder.add_line())
    block = brep.block(builder.add_block())
    surface_builder = builder.surface_mesh_builder(surface)
    for point in [[0, 0, 0], [1, 0, 0], [0, 1, 0]]:
        surface_builder.create_point(geom.Point3D(point))
    surface_builder.create_polygon([0, 1, 2])
    line_builder = builder.line_mesh_builder(line)
    line_builder.create_point(geom.Point3D([0, 0, 0]))
    line_builder.create_point(geom.Point3D([1, 0, 0]))
    line_builder.create_edge_with_vertices(0, 1)
    for name, call, expected in [
        ("block_polygon_unique_vertices",
         lambda: model.block_polygon_unique_vertices(brep, block, mesh.PolyhedronFacet(0, 0)),
         "polyhedron 0 out of range [0, 0)"),
        ("surface_polygon_unique_vertices",
         lambda: model.surface_polygon_unique_vertices(brep, surface, 1),
         "polygon 1 out of range [0, 1)"),
        ("block_mesh_polyhedra_from_surface_polygon",
         lambda: model.block_mesh_polyhedra_from_surface_polygon(brep, block, surface, 1),
         "polygon 1 out of range [0, 1)"),
        ("block_vertices_from_surface_polygon",
         lambda: model.block_vertices_from_surface_polygon(brep, block, surface, 10**7),
         "polygon 10000000 out of range [0, 1)"),
        ("oriented_block_vertices_from_surface_polygon",
         lambda: model.oriented_block_vertices_from_surface_polygon(brep, block, surface, 1),
         "polygon 1 out of range [0, 1)"),
        ("brep_surface_vertices_from_line_edge",
         lambda: model.brep_surface_vertices_from_line_edge(brep, surface, line, 1),
         "edge 1 out of range [0, 1)"),
        ("brep_oriented_surface_vertices_from_line_edge",
         lambda: model.brep_oriented_surface_vertices_from_line_edge(brep, surface, line, 1),
         "edge 1 out of range [0, 1)"),
    ]:
        check_index_error(call, "[%s] %s" % (name, expected), name)
    check(
        list(model.surface_polygon_unique_vertices(brep, surface, 0)) == [basic.NO_ID] * 3,
        "Valid surface_polygon_unique_vertices",
    )
    check(
        len(model.brep_surface_vertices_from_line_edge(brep, surface, line, 0)) == 0,
        "Valid brep_surface_vertices_from_line_edge",
    )
    section = model.Section()
    section_builder = model.SectionBuilder(section)
    surface2 = section.surface(section_builder.add_surface())
    line2 = section.line(section_builder.add_line())
    for name in [
        "section_surface_vertices_from_line_edge",
        "section_oriented_surface_vertices_from_line_edge",
    ]:
        check_index_error(
            lambda: getattr(model, name)(section, surface2, line2, 0),
            "[%s] edge 0 out of range [0, 0)" % name,
            name,
        )


def test_grid_indices():
    for grid, dimension in grids():
        label = type(grid).__name__
        rest = [0] * (dimension - 1)
        nb_cells = 2**dimension
        nb_vertices = 3**dimension
        check(grid.cell_index([1] * dimension) == nb_cells - 1, label + " cell_index")
        check(grid.cell_indices(nb_cells - 1) == [1] * dimension, label + " cell_indices")
        check(grid.vertex_index([2] * dimension) == nb_vertices - 1, label + " vertex_index")
        check(grid.vertex_indices(nb_vertices - 1) == [2] * dimension, label + " vertex_indices")
        check(grid.grid_point([2] * dimension).value(0) == 2, label + " grid_point")
        check(len(grid.cell_vertices([1] * dimension)) == nb_cells, label + " cell_vertices")
        check(grid.cell_barycenter([1] * dimension).value(0) == 1.5, label + " cell_barycenter")
        check(grid.is_cell_on_border([1] * dimension), label + " is_cell_on_border")
        check(grid.is_grid_vertex_on_border([2] * dimension), label + " is_grid_vertex_on_border")
        check(
            grid.cell_vertex_indices([1] * dimension, 0) == [1] * dimension,
            label + " cell_vertex_indices",
        )
        for cell in [[2] + rest, rest + [10**7]]:
            direction = 0 if cell[0] == 2 else dimension - 1
            expected = "cell index %d out of range [0, 2) in direction %d" % (
                cell[direction],
                direction,
            )
            for name, call in [
                ("cell_index", lambda: grid.cell_index(cell)),
                ("cell_vertices", lambda: grid.cell_vertices(cell)),
                ("cell_vertex_indices", lambda: grid.cell_vertex_indices(cell, 0)),
                ("cell_barycenter", lambda: grid.cell_barycenter(cell)),
                ("is_cell_on_border", lambda: grid.is_cell_on_border(cell)),
            ]:
                check_index_error(call, "[%s] %s" % (name, expected), label + " " + name)
        for vertex in [[3] + rest, rest + [10**7]]:
            direction = 0 if vertex[0] == 3 else dimension - 1
            expected = "vertex index %d out of range [0, 3) in direction %d" % (
                vertex[direction],
                direction,
            )
            for name, call in [
                ("vertex_index", lambda: grid.vertex_index(vertex)),
                ("grid_point", lambda: grid.grid_point(vertex)),
                ("is_grid_vertex_on_border", lambda: grid.is_grid_vertex_on_border(vertex)),
            ]:
                check_index_error(call, "[%s] %s" % (name, expected), label + " " + name)
        check_index_error(
            lambda: grid.cell_indices(nb_cells),
            "[cell_indices] cell %d out of range [0, %d)" % (nb_cells, nb_cells),
            label + " cell_indices",
        )
        check_index_error(
            lambda: grid.vertex_indices(nb_vertices),
            "[vertex_indices] vertex %d out of range [0, %d)" % (nb_vertices, nb_vertices),
            label + " vertex_indices",
        )


def test_raster_indices():
    for dimension in [1, 2, 3]:
        raster = getattr(image, "RasterImage%dD" % dimension)([2] * dimension)
        label = type(raster).__name__
        nb_cells = 2**dimension
        raster.set_color(nb_cells - 1, image.RGBColor(1, 2, 3))
        check(raster.color(nb_cells - 1).red() == 1, label + " valid color")
        for cell in [nb_cells, 10**7]:
            check_index_error(
                lambda: raster.color(cell),
                "[color] cell %d out of range [0, %d)" % (cell, nb_cells),
                label + " color",
            )
            check_index_error(
                lambda: raster.set_color(cell, image.RGBColor(1, 2, 3)),
                "[set_color] cell %d out of range [0, %d)" % (cell, nb_cells),
                label + " set_color",
            )
        check_index_error(
            lambda: raster.cell_index([2] + [0] * (dimension - 1)),
            "[cell_index] cell index 2 out of range [0, 2) in direction 0",
            label + " cell_index",
        )


def check_key_error(function, expected_message, label):
    try:
        function()
    except KeyError as error:
        check(
            expected_message in str(error),
            label + ": unexpected message '" + str(error) + "'",
        )
        return
    except Exception as error:
        raise ValueError("[Test] " + label + ": expected KeyError, got " + repr(error))
    raise ValueError("[Test] " + label + ": expected KeyError, nothing raised")


def test_mapping_keys():
    unknown = basic.uuid()
    for mapping in [basic.BijectiveMappinguuid(), basic.GenericMappinguuid()]:
        label = type(mapping).__name__
        for name in ["in2out", "out2in"]:
            check_key_error(
                lambda: getattr(mapping, name)(unknown),
                "[%s] %r is not mapped" % (name, unknown),
                label + " " + name,
            )
    corner_type = model.Corner3D.component_type_static()
    for mapping in [model.ModelCopyMapping(), model.ModelGenericMapping()]:
        check_key_error(
            lambda: mapping.at(corner_type),
            "[at] no mapping for %r" % corner_type,
            type(mapping).__name__ + " at",
        )
    _, mappings = model.convert_brep_into_curve(model.BRep())
    element = mesh.MeshElement(unknown, 3)
    for name, value in [("in2out", element), ("out2in", 5)]:
        check_key_error(
            lambda: getattr(mappings.line_edges_mapping, name)(value),
            "[%s] %r is not mapped" % (name, value),
            "MeshElementToIndexMapping " + name,
        )
    for name in ["in2out", "out2in"]:
        check_key_error(
            lambda: getattr(mappings.unique_vertices_mapping, name)(7),
            "[%s] 7 is not mapped" % name,
            "IndexToIndexMapping " + name,
        )
    # A mapped key still gives its value
    source = model.BRep()
    model.BRepBuilder(source).add_corner()
    copy = model.BRepBuilder(model.BRep()).copy(source)
    corner = source.corners()[0].id()
    copied = copy.at(corner_type).in2out(corner)
    check(copy.at(corner_type).out2in(copied) == corner, "Valid mapping lookups")


if __name__ == "__main__":
    mesh.OpenGeodeMeshLibrary.initialize()
    test_point()
    test_surface()
    test_solid()
    test_graph()
    test_no_id()
    test_model_builder()
    test_attribute()
    test_geometric_elements()
    test_edges_and_facets()
    test_element_builders()
    test_crs_managers()
    test_generic_attribute()
    test_time_series()
    test_grid_directions()
    test_euclidean_distance_transform()
    test_texture()
    test_component_mesh_polygons()
    test_grid_indices()
    test_raster_indices()
    test_mapping_keys()
