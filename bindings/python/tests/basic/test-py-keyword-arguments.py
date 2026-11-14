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

import os
import tempfile

import geode_test_env

import opengeode_py_basic as basic
import opengeode_py_geometry as geometry
import opengeode_py_image as image
import opengeode_py_mesh as mesh
import opengeode_py_model as model


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
    # and the unknown extension is not saveable
    check(
        not image.is_raster_image_saveable2D(raster=raster, filename="a.og_img2d"),
        "is_raster_image_saveable2D keywords",
    )
    check_raises(
        lambda: image.load_raster_image2D(filename="does_not_exist.og_img2d"),
        basic.OpenGeodeDataError,
        "loading a missing file raises",
    )


def point2(x, y):
    return geometry.Point2D([x, y])


def point3(x, y, z):
    return geometry.Point3D([x, y, z])


def build_triangle_surface2D():
    surface = mesh.TriangulatedSurface2D.create()
    builder = mesh.TriangulatedSurfaceBuilder2D.create(mesh=surface)
    for x, y in [(0, 0), (1, 0), (0, 1), (1, 1)]:
        builder.create_point(point=point2(x, y))
    builder.create_triangle(vertices=[0, 1, 2])
    builder.create_triangle(vertices=[1, 3, 2])
    builder.compute_polygon_adjacencies()
    return surface, builder


def build_triangle_surface3D():
    surface = mesh.TriangulatedSurface3D.create()
    builder = mesh.TriangulatedSurfaceBuilder3D.create(mesh=surface)
    for x, y in [(0, 0), (1, 0), (0, 1), (1, 1)]:
        builder.create_point(point=point3(x, y, 0))
    builder.create_triangle(vertices=[0, 1, 2])
    builder.create_triangle(vertices=[1, 3, 2])
    builder.compute_polygon_adjacencies()
    return surface, builder


def build_tetrahedral_solid():
    solid = mesh.TetrahedralSolid3D.create()
    builder = mesh.TetrahedralSolidBuilder3D.create(mesh=solid)
    for point in [(0, 0, 0), (1, 0, 0), (0, 1, 0), (0, 0, 1), (1, 1, 1)]:
        builder.create_point(point=point3(*point))
    builder.create_tetrahedron(vertices=[0, 1, 2, 3])
    builder.create_tetrahedron(vertices=[1, 2, 3, 4])
    builder.compute_polyhedron_adjacencies()
    return solid, builder


def test_mesh_elements():
    edge_vertex = mesh.EdgeVertex(edge_id=2, vertex_id=1)
    check(edge_vertex.edge_id == 2 and edge_vertex.vertex_id == 1, "EdgeVertex ctor")
    check(edge_vertex.__eq__(other=mesh.EdgeVertex(2, 1)), "EdgeVertex.__eq__ keyword")
    check(edge_vertex.__ne__(other=mesh.EdgeVertex(2, 0)), "EdgeVertex.__ne__ keyword")
    edge_vertex.edge_id = 3
    check(edge_vertex.edge_id == 3, "EdgeVertex.edge_id setter")
    polygon_edge = mesh.PolygonEdge(polygon_id=1, edge_id=2)
    polygon_vertex = mesh.PolygonVertex(polygon_edge=polygon_edge)
    check(
        (polygon_vertex.polygon_id, polygon_vertex.vertex_id) == (1, 2),
        "PolygonVertex(polygon_edge=...)",
    )
    check(
        mesh.PolygonEdge(polygon_vertex=polygon_vertex) == polygon_edge,
        "PolygonEdge(polygon_vertex=...)",
    )
    check(
        mesh.PolygonVertex(polygon_id=1, vertex_id=2) == polygon_vertex,
        "PolygonVertex(polygon_id, vertex_id)",
    )
    facet = mesh.PolyhedronFacet(polyhedron_id=4, facet_id=1)
    check(facet.polyhedron_id == 4 and facet.facet_id == 1, "PolyhedronFacet ctor")
    check(
        mesh.PolyhedronVertex(polyhedron_id=4, vertex_id=3).vertex_id == 3,
        "PolyhedronVertex ctor",
    )
    facet_vertex = mesh.PolyhedronFacetVertex(facet=facet, vertex_id=2)
    check(facet_vertex.polyhedron_facet == facet, "PolyhedronFacetVertex ctor")
    facet_edge = mesh.PolyhedronFacetEdge(facet=facet, edge_id=1)
    check(facet_edge.polyhedron_facet == facet, "PolyhedronFacetEdge ctor")
    check(facet_edge.edge_id == 1, "PolyhedronFacetEdge.edge_id")
    mesh_id = basic.uuid()
    element = mesh.MeshElement(mesh_id=mesh_id, element_id=5)
    check(element.mesh_id == mesh_id and element.element_id == 5, "MeshElement ctor")
    check(
        mesh.MeshElement(mesh_id, 6).__lt__(other=mesh.MeshElement(mesh_id, 7)),
        "MeshElement.__lt__ keyword",
    )
    # Names of data structures and types
    impl = mesh.MeshImpl(value="OpenGeodeTriangulatedSurface2D")
    check(impl.get() == "OpenGeodeTriangulatedSurface2D", "MeshImpl(value=...)")
    mesh_type = mesh.MeshFactory.type(key=impl)
    check(mesh_type == mesh.MeshType(value="TriangulatedSurface2D"), "MeshFactory.type(key=...)")
    check(
        mesh.MeshFactory.default_impl(type=mesh_type) == impl,
        "MeshFactory.default_impl(type=...)",
    )
    crs_type = mesh.CRSType(value="points")
    check(crs_type.matches(other=mesh.CRSType("points")), "CRSType.matches keyword")
    check_raises(
        lambda: mesh.MeshFactory.type(key=mesh.MeshImpl(value="unknown")),
        basic.OpenGeodeDataError,
        "unknown data structure raises",
    )
    check_raises(
        lambda: mesh.MeshElement(mesh_id=mesh_id, element=1),
        TypeError,
        "unknown keyword is rejected",
    )


def test_mesh_vertex_sets_graphs_curves_points():
    vertex_set = mesh.VertexSet.create()
    builder = mesh.VertexSetBuilder.create(mesh=vertex_set)
    check(builder.create_vertices(nb=3) == 0, "create_vertices(nb=)")
    check(builder.create_vertex() == 3, "create_vertex")
    mapping = builder.delete_vertices(to_delete=[False, True, False, False])
    check(mapping == [0, basic.NO_ID, 1, 2], "delete_vertices(to_delete=)")
    check(builder.permute_vertices(permutation=[2, 1, 0]) == [2, 1, 0], "permute_vertices keyword")
    # Graph
    graph = mesh.Graph.create()
    graph_builder = mesh.GraphBuilder.create(mesh=graph)
    graph_builder.create_vertices(nb=4)
    check(graph_builder.create_edge_with_vertices(v0_id=0, v1_id=1) == 0, "create_edge_with_vertices")
    check(graph_builder.create_edges(nb=2) == 1, "create_edges(nb=)")
    graph_builder.set_edge_vertex(edge_vertex=mesh.EdgeVertex(1, 0), vertex_id=1)
    graph_builder.set_edge_vertex(edge_vertex=mesh.EdgeVertex(1, 1), vertex_id=2)
    check(graph.edge_vertex(edge_vertex=mesh.EdgeVertex(1, 1)) == 2, "Graph.edge_vertex keyword")
    check(graph.edge_vertices(edge_id=1) == [1, 2], "Graph.edge_vertices keyword")
    check(graph.is_vertex_isolated(vertex_id=3), "Graph.is_vertex_isolated keyword")
    check(len(graph.edges_around_vertex(vertex_id=1)) == 2, "Graph.edges_around_vertex keyword")
    check_raises(
        lambda: graph.edge_vertices(edge_id=10), IndexError, "edge_id out of range"
    )
    check_raises(
        lambda: graph_builder.set_edge_vertex(
            edge_vertex=mesh.EdgeVertex(1, 0), vertex_id=99
        ),
        IndexError,
        "vertex_id out of range in set_edge_vertex",
    )
    check(
        graph_builder.delete_edges(to_delete=[False, False, True]) == [0, 1, basic.NO_ID],
        "GraphBuilder.delete_edges keyword",
    )
    check(graph_builder.permute_edges(permutation=[1, 0]) == [1, 0], "permute_edges keyword")
    check(len(graph_builder.delete_isolated_vertices()) == 4, "delete_isolated_vertices")
    # EdgedCurve in 2D and 3D
    for curve, builder, make_point in (
        (mesh.EdgedCurve2D.create(), None, point2),
        (mesh.EdgedCurve3D.create(), None, lambda x, y: point3(x, y, 0)),
    ):
        dimension_builder = (
            mesh.EdgedCurveBuilder2D if isinstance(curve, mesh.EdgedCurve2D) else mesh.EdgedCurveBuilder3D
        )
        builder = dimension_builder.create(mesh=curve)
        builder.create_point(point=make_point(0, 0))
        builder.create_point(point=make_point(3, 4))
        builder.create_vertex()
        builder.set_point(vertex=2, point=make_point(6, 8))
        builder.create_edge_with_vertices(v0_id=0, v1_id=1)
        builder.create_edge_with_vertices(v0_id=1, v1_id=2)
        check(curve.edge_length(edge_id=0) == 5, "EdgedCurve.edge_length keyword")
        check(
            curve.edge_barycenter(edge_id=0).inexact_equal(other=make_point(1.5, 2)),
            "EdgedCurve.edge_barycenter keyword",
        )
        check(curve.segment(edge_id=1).length() == 5, "EdgedCurve.segment keyword")
        check(curve.point(vertex_id=2).inexact_equal(other=make_point(6, 8)), "EdgedCurve.point keyword")
        check(sorted(curve.vertices_around_vertex(vertex_id=1)) == [0, 2], "vertices_around_vertex keyword")
        check_raises(
            lambda: curve.point(vertex_id=3), IndexError, "EdgedCurve.point out of range"
        )
        check_raises(
            lambda: builder.set_point(vertex=3, point=make_point(0, 0)),
            IndexError,
            "EdgedCurveBuilder.set_point out of range",
        )
        check(
            curve.texture_manager().find_or_create_texture(name="t").image() is not None,
            "texture keyword",
        )
    # PointSet in 2D and 3D
    for point_set, builder_type, make_point in (
        (mesh.PointSet2D.create(), mesh.PointSetBuilder2D, point2),
        (mesh.PointSet3D.create(), mesh.PointSetBuilder3D, lambda x, y: point3(x, y, 1)),
    ):
        builder = builder_type.create(mesh=point_set)
        builder.create_point(point=make_point(1, 2))
        builder.create_vertex()
        builder.set_point(vertex=1, point=make_point(3, 4))
        check(point_set.point(vertex_id=1).inexact_equal(other=make_point(3, 4)), "PointSet.point keyword")
        check(point_set.nb_vertices() == 2, "PointSet nb_vertices")
        check_raises(
            lambda: point_set.point(vertex_id=2), IndexError, "PointSet.point out of range"
        )


def test_mesh_surfaces():
    for surface, builder in (build_triangle_surface2D(), build_triangle_surface3D()):
        edge = mesh.PolygonEdge(polygon_id=0, edge_id=1)
        vertex = mesh.PolygonVertex(polygon_id=0, vertex_id=1)
        check(surface.nb_polygon_vertices(polygon_id=1) == 3, "nb_polygon_vertices keyword")
        check(surface.nb_polygon_edges(polygon_id=1) == 3, "nb_polygon_edges keyword")
        check(surface.polygon_vertex(polygon_vertex=vertex) == 1, "polygon_vertex keyword")
        check(surface.polygon_vertices(polygon_id=1) == [1, 3, 2], "polygon_vertices keyword")
        check(
            surface.polygon_edge_vertex(polygon_edge=edge, vertex_id=1) == 2,
            "polygon_edge_vertex keyword",
        )
        check(
            surface.next_polygon_vertex(polygon_vertex=vertex) == mesh.PolygonVertex(0, 2),
            "next_polygon_vertex keyword",
        )
        check(
            surface.previous_polygon_vertex(polygon_vertex=vertex) == mesh.PolygonVertex(0, 0),
            "previous_polygon_vertex keyword",
        )
        check(
            surface.next_polygon_edge(polygon_edge=edge) == mesh.PolygonEdge(0, 2),
            "next_polygon_edge keyword",
        )
        check(surface.polygon_adjacent(polygon_edge=edge) == 1, "polygon_adjacent keyword")
        check(
            surface.polygon_adjacent_edge(polygon_edge=edge) == mesh.PolygonEdge(1, 2),
            "polygon_adjacent_edge keyword",
        )
        check(not surface.is_edge_on_border(polygon_edge=edge), "is_edge_on_border keyword")
        check(surface.is_edge_on_border(polygon_edge=mesh.PolygonEdge(0, 0)), "border edge")
        check(
            len(surface.polygon_edges_on_border(polygon_id=0)) == 2,
            "polygon_edges_on_border keyword",
        )
        check(
            surface.next_on_border(polygon_edge=mesh.PolygonEdge(0, 0)) == mesh.PolygonEdge(1, 0),
            "next_on_border keyword",
        )
        check(
            surface.previous_on_border(polygon_edge=mesh.PolygonEdge(0, 0)) == mesh.PolygonEdge(0, 2),
            "previous_on_border keyword",
        )
        check(surface.polygon_edge_length(polygon_edge=edge) == 2 ** 0.5, "polygon_edge_length keyword")
        check(surface.edge_length(edge_vertices=[0, 1]) == 1, "edge_length keyword")
        check(
            surface.polygon_edge_barycenter(polygon_edge=mesh.PolygonEdge(0, 0)).value(0) == 0.5,
            "polygon_edge_barycenter keyword",
        )
        check(surface.edge_barycenter(edge_vertices=[0, 1]).value(0) == 0.5, "edge_barycenter keyword")
        check(surface.polygon_area(polygon_id=0) == 0.5, "polygon_area keyword")
        check(surface.polygon_barycenter(polygon_id=0).value(1) == 1 / 3, "polygon_barycenter keyword")
        check(sorted(surface.vertices_around_vertex(vertex_id=1)) == [0, 2, 3], "vertices_around_vertex")
        check(surface.polygon_around_vertex(vertex_id=1) is not None, "polygon_around_vertex")
        check(len(surface.polygons_around_vertex(vertex_id=1)) == 2, "polygons_around_vertex")
        check(
            len(surface.polygons_around_polygon_vertex(polygon_vertex=vertex)) == 2,
            "polygons_around_polygon_vertex keyword",
        )
        check(
            surface.polygon_edge_from_vertices(from_vertex_id=0, to_vertex_id=1) == mesh.PolygonEdge(0, 0),
            "polygon_edge_from_vertices keywords",
        )
        check(
            surface.polygon_edge_from_edge_vertices(edge_vertices=[1, 0]) == mesh.PolygonEdge(0, 0),
            "polygon_edge_from_edge_vertices keyword",
        )
        check(
            len(surface.polygons_from_edge_vertices(edge_vertices=[1, 2])) == 2,
            "polygons_from_edge_vertices keyword",
        )
        check(surface.is_vertex_on_border(vertex_id=0), "is_vertex_on_border keyword")
        check(surface.polygon_attribute_manager().nb_elements() == 2, "polygon attributes")
        check(
            surface.main_coordinate_reference_system_manager().active_coordinate_reference_system_name()
            == "points",
            "default crs name",
        )
        crs = surface.main_coordinate_reference_system_manager().find_coordinate_reference_system(
            name="points"
        )
        check(crs.point(point_id=1).value(0) == 1, "CoordinateReferenceSystem.point keyword")
        check_raises(lambda: crs.point(point_id=4), IndexError, "crs point out of range")
        crs.set_point(point_id=1, point=surface.point(vertex_id=0))
        check(surface.point(vertex_id=1).value(0) == 0, "CoordinateReferenceSystem.set_point keyword")
        check(
            surface.is_triangulated_type() and not surface.is_polygonal_type(),
            "type predicates",
        )
        # edges
        surface.enable_edges()
        edges = surface.edges()
        check(edges.nb_edges() == 5, "SurfaceEdges.nb_edges")
        index = edges.edge_from_vertices(vertices=[1, 0])
        check(edges.edge_vertices(edge_id=index) == [0, 1] or edges.edge_vertices(edge_id=index) == [1, 0], "edges")
        check(edges.edge_attribute_manager().nb_elements() == 5, "edge attributes")
        # error paths with keywords
        check_raises(lambda: surface.nb_polygon_vertices(polygon_id=2), IndexError, "polygon_id out of range")
        check_raises(
            lambda: surface.polygon_vertex(polygon_vertex=mesh.PolygonVertex(0, 3)),
            IndexError,
            "local vertex out of range",
        )
        check_raises(
            lambda: surface.polygon_edge_from_vertices(from_vertex_id=0, to_vertex_id=9),
            IndexError,
            "vertex out of range in polygon_edge_from_vertices",
        )
        check_raises(
            lambda: builder.set_polygon_vertex(polygon_vertex=mesh.PolygonVertex(5, 0), vertex_id=0),
            IndexError,
            "polygon out of range in set_polygon_vertex",
        )
        check_raises(
            lambda: builder.set_polygon_adjacent(
                polygon_edge=mesh.PolygonEdge(0, 0), adjacent_id=9
            ),
            IndexError,
            "adjacent_id out of range in set_polygon_adjacent",
        )
        surface.disable_edges()
        check(not surface.are_edges_enabled(), "disable_edges")
        check_raises(lambda: surface.edges(), basic.OpenGeodeDataError, "edges not enabled")
    # 3D only
    surface, builder = build_triangle_surface3D()
    check(surface.polygon_normal(polygon_id=0).value(2) == 1, "polygon_normal keyword")
    check(
        abs(surface.polygon_vertex_normal(vertex_id=0).value(2) - 1) < 1e-12,
        "polygon_vertex_normal keyword",
    )
    # polygonal surfaces
    for polygonal, builder_type, make_point in (
        (mesh.PolygonalSurface2D.create(), mesh.PolygonalSurfaceBuilder2D, point2),
        (mesh.PolygonalSurface3D.create(), mesh.PolygonalSurfaceBuilder3D, lambda x, y: point3(x, y, 0)),
    ):
        builder = builder_type.create(mesh=polygonal)
        for x, y in [(0, 0), (1, 0), (1, 1), (0, 1)]:
            builder.create_point(point=make_point(x, y))
        builder.create_polygon(vertices=[0, 1, 2, 3])
        builder.create_polygon(vertices=[0, 2, 3])
        builder.set_polygon_vertex(polygon_vertex=mesh.PolygonVertex(1, 0), vertex_id=1)
        check(polygonal.polygon_vertices(polygon_id=1) == [1, 2, 3], "set_polygon_vertex keyword")
        check(polygonal.is_polygonal_type(), "is_polygonal_type")
        mapping = builder.permute_polygons(permutation=[1, 0])
        check(mapping == [1, 0], "permute_polygons keyword")
        mapping = builder.delete_polygons(to_delete=[False, True])
        check(mapping == [0, basic.NO_ID], "delete_polygons keyword")
        mapping = builder.delete_isolated_vertices()
        check(mapping == [basic.NO_ID, 0, 1, 2], "delete_isolated_vertices keyword")
        polygonal.enable_edges()
        edges_builder = builder.edges_builder()
        check(edges_builder.delete_isolated_edges() == [0, 1, 2], "delete_isolated_edges")
    # Adjacencies set by hand on both sides of an edge
    surface = mesh.TriangulatedSurface2D.create()
    builder = mesh.TriangulatedSurfaceBuilder2D.create(mesh=surface)
    for x, y in [(0, 0), (1, 0), (0, 1), (1, 1)]:
        builder.create_point(point=point2(x, y))
    builder.create_triangle(vertices=[0, 1, 2])
    builder.create_triangle(vertices=[1, 3, 2])
    builder.set_polygon_adjacent(polygon_edge=mesh.PolygonEdge(0, 1), adjacent_id=1)
    builder.set_polygon_adjacent(polygon_edge=mesh.PolygonEdge(1, 2), adjacent_id=0)
    check(
        surface.polygon_adjacent(polygon_edge=mesh.PolygonEdge(0, 1)) == 1,
        "set_polygon_adjacent keywords",
    )
    check(surface.polygon_adjacent(polygon_edge=mesh.PolygonEdge(0, 0)) is None, "no adjacent")


def test_mesh_solids():
    solid, builder = build_tetrahedral_solid()
    facet = mesh.PolyhedronFacet(polyhedron_id=0, facet_id=0)
    vertex = mesh.PolyhedronVertex(polyhedron_id=0, vertex_id=1)
    check(solid.nb_polyhedron_vertices(polyhedron_id=0) == 4, "nb_polyhedron_vertices keyword")
    check(solid.nb_polyhedron_facets(polyhedron_id=0) == 4, "nb_polyhedron_facets keyword")
    check(solid.nb_polyhedron_facet_vertices(polyhedron_facet=facet) == 3, "nb_polyhedron_facet_vertices keyword")
    check(solid.polyhedron_vertex(polyhedron_vertex=vertex) == 1, "polyhedron_vertex keyword")
    check(solid.polyhedron_vertices(polyhedron_id=1) == [1, 2, 3, 4], "polyhedron_vertices keyword")
    check(
        solid.polyhedron_facet_vertex(
            polyhedron_facet_vertex=mesh.PolyhedronFacetVertex(facet=facet, vertex_id=0)
        )
        in range(5),
        "polyhedron_facet_vertex keyword",
    )
    check(len(solid.polyhedron_facet_vertices(polyhedron_facet=facet)) == 3, "polyhedron_facet_vertices keyword")
    adjacents = [
        solid.polyhedron_adjacent(polyhedron_facet=mesh.PolyhedronFacet(0, f)) for f in range(4)
    ]
    check(adjacents.count(1) == 1 and adjacents.count(None) == 3, "polyhedron_adjacent keyword")
    shared = adjacents.index(1)
    check(
        solid.polyhedron_adjacent_facet(polyhedron_facet=mesh.PolyhedronFacet(0, shared)).polyhedron_id == 1,
        "polyhedron_adjacent_facet keyword",
    )
    check(
        not solid.is_polyhedron_facet_on_border(polyhedron_facet=mesh.PolyhedronFacet(0, shared)),
        "is_polyhedron_facet_on_border keyword",
    )
    check(len(solid.polyhedron_facets_on_border(polyhedron_id=0)) == 3, "polyhedron_facets_on_border keyword")
    check(solid.edge_length(edge_vertices=[0, 1]) == 1, "edge_length keyword")
    check(solid.edge_barycenter(edge_vertices=[0, 1]).value(0) == 0.5, "edge_barycenter keyword")
    check(solid.polyhedron_barycenter(polyhedron_id=0).value(0) == 0.25, "polyhedron_barycenter keyword")
    check(
        solid.facet_barycenter(facet_vertices=[0, 1, 2]).value(0) == 1 / 3,
        "facet_barycenter keyword",
    )
    check(abs(solid.polyhedron_volume(polyhedron_id=0) - 1 / 6) < 1e-12, "polyhedron_volume keyword")
    check(
        solid.polyhedron_facet_normal(polyhedron_facet=facet) is not None,
        "polyhedron_facet_normal keyword",
    )
    check(sorted(solid.vertices_around_vertex(vertex_id=0)) == [1, 2, 3], "vertices_around_vertex")
    check(solid.polyhedron_around_vertex(vertex_id=4) is not None, "polyhedron_around_vertex keyword")
    check(len(solid.polyhedra_around_vertex(vertex_id=1)) == 2, "polyhedra_around_vertex keyword")
    check(
        len(solid.polyhedra_around_polyhedron_vertex(polyhedron_vertex=vertex)) == 2,
        "polyhedra_around_polyhedron_vertex keyword",
    )
    check(sorted(solid.polyhedra_around_edge(vertices=[1, 2])) == [0, 1], "polyhedra_around_edge keyword")
    check(
        sorted(solid.polyhedra_around_edge_with_hint(vertices=[1, 2], first_polyhedron=1)) == [0, 1],
        "polyhedra_around_edge_with_hint keywords",
    )
    check(
        len(solid.polyhedra_from_facet_vertices(facet_vertices=[1, 2, 3])) == 2,
        "polyhedra_from_facet_vertices keyword",
    )
    check(
        solid.edge_vertices_in_polyhedron(polyhedron_id=1, edge_vertices=[1, 4]) == [0, 3],
        "edge_vertices_in_polyhedron keywords",
    )
    check(solid.is_vertex_on_border(vertex_id=0), "is_vertex_on_border keyword")
    check(solid.is_tetrahedral_type() and not solid.is_hybrid_type(), "solid type predicates")
    check(
        solid.tetrahedron(tetrahedron_id=1).vertices()[3].inexact_equal(other=point3(1, 1, 1)),
        "tetrahedron keyword",
    )
    check(len(solid.triangle(facet=facet).vertices()) == 3, "triangle keyword")
    solid.enable_edges()
    solid.enable_facets()
    check(solid.edges().edge_from_vertices(vertices=[1, 0]) is not None, "SolidEdges.edge_from_vertices keyword")
    check(len(solid.edges().edge_vertices(edge_id=0)) == 2, "SolidEdges.edge_vertices keyword")
    check(solid.facets().nb_facets() == 7, "SolidFacets.nb_facets")
    facet_index = solid.facets().facet_from_vertices(vertices=[1, 2, 3])
    check(sorted(solid.facets().facet_vertices(facet_id=facet_index)) == [1, 2, 3], "SolidFacets keywords")
    check(len(solid.edges().edge_vertices(edge_id=0)) == 2, "edge_vertices")
    check(solid.edges().nb_edges() == 9, "SolidEdges.nb_edges")
    check(len(builder.edges_builder().delete_isolated_edges()) == 9, "SolidEdgesBuilder")
    check(len(builder.facets_builder().delete_isolated_facets()) == 7, "SolidFacetsBuilder")
    # error paths with keywords
    check_raises(lambda: solid.polyhedron_vertices(polyhedron_id=2), IndexError, "polyhedron_id out of range")
    check_raises(
        lambda: solid.polyhedron_adjacent(polyhedron_facet=mesh.PolyhedronFacet(0, 4)),
        IndexError,
        "local facet out of range",
    )
    check_raises(
        lambda: builder.set_polyhedron_vertex(polyhedron_vertex=mesh.PolyhedronVertex(0, 4), vertex_id=0),
        IndexError,
        "local vertex out of range in set_polyhedron_vertex",
    )
    check_raises(
        lambda: builder.set_polyhedron_adjacent(polyhedron_facet=facet, adjacent_id=9),
        IndexError,
        "adjacent_id out of range in set_polyhedron_adjacent",
    )
    check_raises(
        lambda: solid.edge_vertices_in_polyhedron(polyhedron_id=0, edge_vertices=[0, 9]),
        IndexError,
        "vertex out of range in edge_vertices_in_polyhedron",
    )
    # builder operations on a polyhedral solid and a hybrid solid
    polyhedral = mesh.PolyhedralSolid3D.create()
    polyhedral_builder = mesh.PolyhedralSolidBuilder3D.create(mesh=polyhedral)
    for point in [(0, 0, 0), (1, 0, 0), (0, 1, 0), (0, 0, 1)]:
        polyhedral_builder.create_point(point=point3(*point))
    index = polyhedral_builder.create_polyhedron(
        vertices=[0, 1, 2, 3], facets=[[1, 3, 2], [0, 2, 3], [3, 1, 0], [0, 1, 2]]
    )
    check(index == 0 and polyhedral.nb_polyhedra() == 1, "create_polyhedron keywords")
    polyhedral_builder.associate_polyhedron_vertex_to_vertex(
        polyhedron_vertex=mesh.PolyhedronVertex(0, 0), vertex_id=0
    )
    check(polyhedral.is_polyhedral_type(), "is_polyhedral_type")
    check(
        polyhedral_builder.permute_polyhedra(permutation=[0]) == [0],
        "permute_polyhedra keyword",
    )
    check(
        polyhedral_builder.delete_polyhedra(to_delete=[True]) == [basic.NO_ID],
        "delete_polyhedra keyword",
    )
    check(
        polyhedral_builder.delete_isolated_vertices() == [basic.NO_ID] * 4,
        "delete_isolated_vertices keyword",
    )
    hybrid = mesh.HybridSolid3D.create()
    hybrid_builder = mesh.HybridSolidBuilder3D.create(mesh=hybrid)
    cube = [(x, y, z) for z in (0, 1) for y in (0, 1) for x in (0, 1)]
    for point in cube:
        hybrid_builder.create_point(point=point3(*point))
    hybrid_builder.create_point(point=point3(0.5, 0.5, 2))
    hybrid_builder.create_point(point=point3(2, 0, 0))
    hybrid_builder.create_point(point=point3(2, 1, 0))
    hybrid_builder.create_point(point=point3(2, 0, 1))
    hybrid_builder.create_hexahedron(vertices=[0, 1, 3, 2, 4, 5, 7, 6])
    hybrid_builder.create_pyramid(vertices=[4, 5, 7, 6, 8])
    hybrid_builder.create_tetrahedron(vertices=[1, 9, 3, 5])
    hybrid_builder.create_prism(vertices=[1, 9, 10, 5, 11, 7][:6])
    check(hybrid.nb_polyhedra() == 4 and hybrid.is_hybrid_type(), "hybrid builder keywords")


def test_mesh_grids():
    for grid_type, builder_type, dimension in (
        (mesh.RegularGrid2D, mesh.RegularGridBuilder2D, 2),
        (mesh.RegularGrid3D, mesh.RegularGridBuilder3D, 3),
    ):
        make_point = point2 if dimension == 2 else lambda x, y: point3(x, y, 0)
        origin = make_point(1, 2)
        grid = grid_type.create()
        builder = builder_type.create(grid=grid)
        builder.initialize_grid(
            origin=origin, cells_number=[2] * dimension, cells_length=[1, 2, 3][:dimension]
        )
        check(grid.nb_grid_vertices() == 3 ** dimension, "initialize_grid keywords")
        check(grid.cell_length_in_direction(direction=1) == 2, "cell_length_in_direction keyword")
        check(grid.nb_vertices_in_direction(direction=1) == 3, "nb_vertices_in_direction keyword")
        grid2 = grid_type.create()
        builder_type.create(grid=grid2).initialize_cartesian_grid(
            origin=origin, cells_number=[2] * dimension, cells_length=2.0
        )
        check(grid2.cell_size() == 2.0 ** dimension, "initialize_cartesian_grid keywords")
        grid3 = grid_type.create()
        vectors = [
            geometry.Vector2D([1, 0]) if dimension == 2 else geometry.Vector3D([1, 0, 0]),
            geometry.Vector2D([0, 2]) if dimension == 2 else geometry.Vector3D([0, 2, 0]),
        ]
        if dimension == 3:
            vectors.append(geometry.Vector3D([0, 0, 3]))
        builder_type.create(grid=grid3).initialize_grid_with_directions(
            origin=origin, cells_number=[1] * dimension, cell_directions=vectors
        )
        check(grid3.cell_length_in_direction(direction=1) == 2, "initialize_grid_with_directions keywords")
        builder.update_origin(origin=make_point(5, 5))
        check(grid.point(vertex_id=0).value(0) == 5, "update_origin keyword")
        index = [1] * dimension
        vertex_id = grid.vertex_index(index=index)
        check(vertex_id == (4 if dimension == 2 else 13), "vertex_index keyword")
        check(grid.vertex_indices(index=vertex_id) == index, "vertex_indices keyword")
        check(
            grid.grid_point(index=index).value(0) == 5 + 1,
            "grid_point keyword",
        )
        check(
            len(grid.cell_vertices(cell_id=[0] * dimension)) == 2 ** dimension,
            "cell_vertices keyword",
        )
        check(
            grid.cell_vertex_indices(cell_id=[0] * dimension, vertex_id=1)[0] == 1,
            "cell_vertex_indices keywords",
        )
        check(
            grid.next_vertex(index=[0] * dimension, direction=0)[0] == 1,
            "next_vertex keywords",
        )
        check(grid.previous_vertex(index=[0] * dimension, direction=0) is None, "previous_vertex keywords")
        check(not grid.is_grid_vertex_on_border(index=index), "interior grid vertex")
        check(
            grid.is_grid_vertex_on_border(index=[0] * dimension),
            "is_grid_vertex_on_border keyword",
        )
        check(
            grid.cell_barycenter(cell_id=[0] * dimension).value(0) == 5.5,
            "cell_barycenter keyword",
        )
        inside = make_point(5.5, 6)
        outside = make_point(0, 0)
        check(grid.contains(query=inside) and not grid.contains(query=outside), "contains keyword")
        check(grid.closest_vertex(query=outside) == [0] * dimension, "closest_vertex keyword")
        check(grid.cells(query=inside) == [[0] * dimension], "cells keyword")
        check(grid.cells(query=outside) == [], "cells outside")
        check_raises(lambda: grid.point(vertex_id=3 ** dimension), IndexError, "grid point out of range")
        check(grid.nb_cells() == 2 ** dimension, "nb cells")
    # Light regular grids
    for light_type, vector_type, dimension in (
        (mesh.LightRegularGrid2D, geometry.Vector2D, 2),
        (mesh.LightRegularGrid3D, geometry.Vector3D, 3),
    ):
        origin = point2(0, 0) if dimension == 2 else point3(0, 0, 0)
        light = light_type(origin=origin, cells_number=[2] * dimension, cells_length=[1, 2, 3][:dimension])
        check(light.cell_length_in_direction(direction=1) == 2, "LightRegularGrid ctor keywords")
        directions = [vector_type([1, 0] + [0] * (dimension - 2)), vector_type([0, 3] + [0] * (dimension - 2))]
        if dimension == 3:
            directions.append(vector_type([0, 0, 1]))
        other = light_type(origin=origin, cells_number=[1] * dimension, directions=directions)
        check(other.cell_length_in_direction(direction=1) == 3, "LightRegularGrid directions keyword")
        manager = light.grid_coordinate_reference_system_manager()
        default_names = sorted(manager.coordinate_reference_system_names())
        crs_type = mesh.GridCoordinateReferenceSystem2D if dimension == 2 else mesh.GridCoordinateReferenceSystem3D
        coordinate_system = (
            geometry.CoordinateSystem2D() if dimension == 2 else geometry.CoordinateSystem3D()
        )
        crs = crs_type(coordinate_system=coordinate_system)
        builder_type = (
            mesh.GridCoordinateReferenceSystemManagerBuilder2D
            if dimension == 2
            else mesh.GridCoordinateReferenceSystemManagerBuilder3D
        )
        builder = builder_type(grid=light)
        builder.register_coordinate_reference_system(name="local", crs=crs)
        check(
            "local" in manager.coordinate_reference_system_names(),
            "grid crs registered",
        )
        builder.set_active_coordinate_reference_system(name="local")
        check(manager.active_coordinate_reference_system_name() == "local", "grid crs active")
        check(manager.coordinate_reference_system_exists(name="local"), "grid crs exists keyword")
        found = manager.find_coordinate_reference_system(name="local")
        check(found.type_name().matches(other=crs_type.type_name_static()), "grid crs type_name_static")
        check(found.coordinate_system().origin().value(0) == 0, "grid crs coordinate_system")
        builder.delete_coordinate_reference_system(name="local")
        check(
            "local" not in manager.coordinate_reference_system_names(),
            "grid crs deleted by name",
        )
        check(
            sorted(manager.coordinate_reference_system_names()) == default_names,
            "grid crs deleted",
        )
        check_raises(
            lambda: manager.find_coordinate_reference_system(name="local"),
            basic.OpenGeodeDataError,
            "unknown grid crs raises",
        )


def test_mesh_crs():
    surface, builder = build_triangle_surface2D()
    managers_builder = builder
    main = builder.main_coordinate_reference_system_manager_builder()
    input_system = geometry.CoordinateSystem2D()
    output_system = geometry.CoordinateSystem2D(
        directions=[geometry.Vector2D([2, 0]), geometry.Vector2D([0, 2])], origin=point2(1, 1)
    )
    mesh.create_surface_mesh_coordinate_system2D(
        mesh=surface,
        builder=builder,
        new_coordinate_system_name="scaled",
        input=input_system,
        output=output_system,
    )
    manager = surface.main_coordinate_reference_system_manager()
    check(manager.active_coordinate_reference_system_name() == "scaled", "scaled crs is active")
    check(sorted(manager.coordinate_reference_system_names()) == ["points", "scaled"], "crs names")
    check(manager.nb_coordinate_reference_systems() == 2, "nb crs")
    check(surface.point(vertex_id=1).value(0) == 3, "scaled point")
    main.set_active_coordinate_reference_system(name="points")
    check(surface.point(vertex_id=1).value(0) == 1, "points active again")
    check(
        main.coordinate_reference_system(name="scaled").point(point_id=1).value(0) == 3,
        "builder crs keyword",
    )
    check(main.active_coordinate_reference_system().point(point_id=1).value(0) == 1, "active crs")
    managers_builder.set_point(vertex=0, point=point2(9, 9))
    check(surface.point(vertex_id=0).value(0) == 9, "set_point keyword")
    # The point of the managers and of the mesh take the same keyword
    managers_class = type(surface).__mro__[
        [c.__name__ for c in type(surface).__mro__].index("CoordinateReferenceSystemManagers2D")
    ]
    check(
        managers_class.point(surface, vertex_id=0) == surface.point(vertex_id=0),
        "CoordinateReferenceSystemManagers2D.point keyword",
    )
    check_raises(lambda: surface.point(vertex=0), TypeError, "no vertex keyword")
    check(
        surface.coordinate_reference_system_manager2D().nb_coordinate_reference_systems() == 2,
        "manager2D",
    )
    check(surface.main_coordinate_reference_system_manager().active_coordinate_reference_system().point(point_id=0).value(0) == 9, "main crs")
    main.delete_coordinate_reference_system(name="scaled")
    check(not manager.coordinate_reference_system_exists(name="scaled"), "crs deleted")
    check_raises(
        lambda: main.set_active_coordinate_reference_system(name="scaled"),
        basic.OpenGeodeDataError,
        "activating an unknown crs raises",
    )
    check_raises(
        lambda: manager.find_coordinate_reference_system(name="scaled"),
        basic.OpenGeodeDataError,
        "unknown crs raises",
    )
    check(
        surface.coordinate_reference_system_manager1D().nb_coordinate_reference_systems() >= 0
        and surface.coordinate_reference_system_manager3D().nb_coordinate_reference_systems() >= 0,
        "other dimensions managers",
    )
    # Point sets, edged curves and solids
    point_set = mesh.PointSet3D.create()
    point_builder = mesh.PointSetBuilder3D.create(mesh=point_set)
    point_builder.create_point(point=point3(1, 2, 3))
    mesh.create_point_set_coordinate_system3D(
        mesh=point_set,
        builder=point_builder,
        new_coordinate_system_name="shifted",
        input=input_system,
        output=output_system,
    )
    check(point_set.point(vertex_id=0).value(0) == 3 and point_set.point(vertex_id=0).value(2) == 3, "point set crs")
    curve = mesh.EdgedCurve2D.create()
    curve_builder = mesh.EdgedCurveBuilder2D.create(mesh=curve)
    curve_builder.create_point(point=point2(1, 1))
    mesh.create_edged_curve_coordinate_system2D(
        mesh=curve, builder=curve_builder, new_coordinate_system_name="c", input=input_system, output=output_system
    )
    check(curve.point(vertex_id=0).value(0) == 3, "edged curve crs")
    solid, solid_builder = build_tetrahedral_solid()
    mesh.create_solid_mesh_coordinate_system3D(
        mesh=solid, builder=solid_builder, new_coordinate_system_name="s", input=input_system, output=output_system
    )
    check(solid.point(vertex_id=1).value(0) == 3, "solid crs")


def test_mesh_textures():
    surface, _ = build_triangle_surface2D()
    manager = surface.texture_manager()
    texture = manager.find_or_create_texture(name="color")
    check(manager.texture_exists(name="color"), "texture_exists keyword")
    check(manager.find_texture(name="color") is not None, "find_texture keyword")
    check(manager.texture_names() == ["color"], "texture_names")
    texture.set_image(image=image.RasterImage2D(cells_number=[2, 2]))
    check(texture.image().nb_cells() == 4, "Texture.set_image keyword")
    vertex = mesh.PolygonVertex(polygon_id=0, vertex_id=1)
    texture.set_texture_coordinates(vertex=vertex, coordinates=point2(0.25, 0.75))
    check(texture.texture_coordinates(vertex=vertex).value(1) == 0.75, "texture coordinates keywords")
    manager.delete_texture(name="color")
    check(not manager.texture_exists(name="color"), "delete_texture keyword")
    check_raises(
        lambda: manager.find_texture(name="color"),
        basic.OpenGeodeDataError,
        "find_texture of a missing texture raises",
    )
    check_raises(
        lambda: manager.delete_texture(name="color"),
        basic.OpenGeodeDataError,
        "delete_texture of a missing texture raises",
    )
    # 1D and 3D textures
    curve = mesh.EdgedCurve3D.create()
    texture1d = curve.texture_manager().find_or_create_texture(name="t")
    texture1d.set_image(image=image.RasterImage1D(cells_number=[3]))
    solid, _ = build_tetrahedral_solid()
    texture3d = solid.texture_manager().find_or_create_texture(name="t")
    texture3d.set_texture_coordinates(
        vertex=mesh.PolyhedronVertex(polyhedron_id=0, vertex_id=0), coordinates=point3(1, 1, 1)
    )
    check(
        texture3d.texture_coordinates(vertex=mesh.PolyhedronVertex(0, 0)).value(2) == 1,
        "3D texture keywords",
    )


def save_load_cycle(directory, name, objects, dimension_suffix):
    """Check the keyword names of the save/load/loadable/priority functions."""
    save = getattr(mesh, f"save_{name}{dimension_suffix}")
    load = getattr(mesh, f"load_{name}{dimension_suffix}")
    saveable = getattr(mesh, f"is_{name}_saveable{dimension_suffix}")
    loadable = getattr(mesh, f"is_{name}_loadable{dimension_suffix}")
    priority = getattr(mesh, f"{name}_object_priority{dimension_suffix}")
    additional = getattr(mesh, f"{name}_additional_files{dimension_suffix}")
    filename = os.path.join(directory, f"{name}{dimension_suffix}.{objects.native_extension()}")
    check(saveable(**{name: objects, "filename": filename}), f"is_{name}_saveable keywords")
    written = save(**{name: objects, "filename": filename})
    check(filename in written, f"save_{name} keywords")
    loaded = load(filename=filename)
    check(len(loaded) == 1 and loaded[0].nb_vertices() == objects.nb_vertices(), f"load_{name} keyword")
    check(loadable(filename=filename).value() > 0, f"is_{name}_loadable keyword")
    check(isinstance(priority(filename=filename), int), f"{name}_object_priority keyword")
    check(not additional(filename=filename).has_additional_files(), f"{name}_additional_files keyword")
    check_raises(
        lambda: load(filename=os.path.join(directory, f"missing.{objects.native_extension()}")),
        basic.OpenGeodeDataError,
        f"load_{name} of a missing file raises",
    )
    check(loadable(filename=os.path.join(directory, "missing.unknown")).value() == 0, f"{name} unknown extension is not loadable")
    check(
        not saveable(**{name: objects, "filename": os.path.join(directory, "x.unknown")}),
        f"{name} unknown extension is not saveable",
    )
    check_raises(
        lambda: priority(filename=os.path.join(directory, "missing.unknown")),
        basic.OpenGeodeDataError,
        f"{name}_object_priority of an unknown extension raises",
    )
    check_raises(
        lambda: additional(filename=os.path.join(directory, "missing.unknown")),
        basic.OpenGeodeDataError,
        f"{name}_additional_files of an unknown extension raises",
    )


def test_mesh_io():
    with tempfile.TemporaryDirectory() as directory:
        vertex_set = mesh.VertexSet.create()
        mesh.VertexSetBuilder.create(mesh=vertex_set).create_vertices(nb=2)
        save_load_cycle(directory, "vertex_set", vertex_set, "")
        graph = mesh.Graph.create()
        graph_builder = mesh.GraphBuilder.create(mesh=graph)
        graph_builder.create_vertices(nb=2)
        graph_builder.create_edge_with_vertices(v0_id=0, v1_id=1)
        save_load_cycle(directory, "graph", graph, "")
        for dimension, make_point in ((2, point2), (3, lambda x, y: point3(x, y, 0))):
            suffix = f"{dimension}D"
            curve = mesh.EdgedCurve2D.create() if dimension == 2 else mesh.EdgedCurve3D.create()
            curve_builder = (mesh.EdgedCurveBuilder2D if dimension == 2 else mesh.EdgedCurveBuilder3D).create(mesh=curve)
            curve_builder.create_point(point=make_point(0, 0))
            curve_builder.create_point(point=make_point(1, 0))
            curve_builder.create_edge_with_vertices(v0_id=0, v1_id=1)
            save_load_cycle(directory, "edged_curve", curve, suffix)
            point_set = mesh.PointSet2D.create() if dimension == 2 else mesh.PointSet3D.create()
            (mesh.PointSetBuilder2D if dimension == 2 else mesh.PointSetBuilder3D).create(mesh=point_set).create_point(
                point=make_point(1, 1)
            )
            save_load_cycle(directory, "point_set", point_set, suffix)
            surface = (mesh.PolygonalSurface2D if dimension == 2 else mesh.PolygonalSurface3D).create()
            surface_builder = (mesh.PolygonalSurfaceBuilder2D if dimension == 2 else mesh.PolygonalSurfaceBuilder3D).create(mesh=surface)
            for x, y in [(0, 0), (1, 0), (1, 1)]:
                surface_builder.create_point(point=make_point(x, y))
            surface_builder.create_polygon(vertices=[0, 1, 2])
            save_load_cycle(directory, "polygonal_surface", surface, suffix)
            triangulated, _ = build_triangle_surface2D() if dimension == 2 else build_triangle_surface3D()
            save_load_cycle(directory, "triangulated_surface", triangulated, suffix)
            grid = (mesh.RegularGrid2D if dimension == 2 else mesh.RegularGrid3D).create()
            (mesh.RegularGridBuilder2D if dimension == 2 else mesh.RegularGridBuilder3D).create(grid=grid).initialize_cartesian_grid(
                origin=make_point(0, 0), cells_number=[2] * dimension, cells_length=1.0
            )
            save_load_cycle(directory, "regular_grid", grid, suffix)
            light = (mesh.LightRegularGrid2D if dimension == 2 else mesh.LightRegularGrid3D)(
                origin=make_point(0, 0), cells_number=[2] * dimension, cells_length=[1.0] * dimension
            )
            light_name = "light_regular_grid"
            filename = os.path.join(directory, f"light{suffix}.{light.native_extension()}")
            written = getattr(mesh, f"save_{light_name}{suffix}")(light_regular_grid=light, filename=filename)
            check(filename in written, "save_light_regular_grid keywords")
            check(
                len(getattr(mesh, f"load_{light_name}{suffix}")(filename=filename)) == 1,
                "load_light_regular_grid keyword",
            )
            check(
                getattr(mesh, f"is_{light_name}_saveable{suffix}")(light_regular_grid=light, filename=filename),
                "is_light_regular_grid_saveable keywords",
            )
            check(
                getattr(mesh, f"is_{light_name}_loadable{suffix}")(filename=filename).value() > 0,
                "is_light_regular_grid_loadable keyword",
            )
            getattr(mesh, f"{light_name}_object_priority{suffix}")(filename=filename)
            getattr(mesh, f"{light_name}_additional_files{suffix}")(filename=filename)
        solid, _ = build_tetrahedral_solid()
        save_load_cycle(directory, "tetrahedral_solid", solid, "3D")
        polyhedral = mesh.PolyhedralSolid3D.create()
        polyhedral_builder = mesh.PolyhedralSolidBuilder3D.create(mesh=polyhedral)
        for point in [(0, 0, 0), (1, 0, 0), (0, 1, 0), (0, 0, 1)]:
            polyhedral_builder.create_point(point=point3(*point))
        polyhedral_builder.create_polyhedron(vertices=[0, 1, 2, 3], facets=[[1, 3, 2], [0, 2, 3], [3, 1, 0], [0, 1, 2]])
        save_load_cycle(directory, "polyhedral_solid", polyhedral, "3D")
        hybrid = mesh.HybridSolid3D.create()
        hybrid_builder = mesh.HybridSolidBuilder3D.create(mesh=hybrid)
        for point in [(0, 0, 0), (1, 0, 0), (0, 1, 0), (0, 0, 1)]:
            hybrid_builder.create_point(point=point3(*point))
        hybrid_builder.create_tetrahedron(vertices=[0, 1, 2, 3])
        save_load_cycle(directory, "hybrid_solid", hybrid, "3D")
        # file format registries
        factory = mesh.TriangulatedSurfaceInputFactory3D
        check(factory.has_creator(key="og_tsf3d"), "factory has_creator keyword")
        check("og_tsf3d" in factory.list_creators(), "factory list_creators")


def test_mesh_helpers():
    # Conversions between dimensions
    surface3d, _ = build_triangle_surface3D()
    surface2d = mesh.convert_triangulated_surface3D_into_2D(surface3d=surface3d, axis_to_remove=2)
    check(surface2d.nb_polygons() == 2 and surface2d.point(vertex_id=3).value(1) == 1, "convert 3D into 2D keywords")
    back = mesh.convert_triangulated_surface2D_into_3D(surface2d=surface2d, axis_to_add=2, axis_coordinate=5)
    check(back.point(vertex_id=3).value(2) == 5, "convert 2D into 3D keywords")
    polygonal = mesh.PolygonalSurface3D.create()
    polygonal_builder = mesh.PolygonalSurfaceBuilder3D.create(mesh=polygonal)
    for x, y in [(0, 0), (1, 0), (1, 1), (0, 1)]:
        polygonal_builder.create_point(point=point3(x, y, 0))
    polygonal_builder.create_polygon(vertices=[0, 1, 2, 3])
    polygonal2d = mesh.convert_polygonal_surface3D_into_2D(surface3d=polygonal, axis_to_remove=2)
    check(polygonal2d.nb_polygon_vertices(polygon_id=0) == 4, "convert_polygonal_surface3D_into_2D keywords")
    check(
        mesh.convert_polygonal_surface2D_into_3D(surface2d=polygonal2d, axis_to_add=2, axis_coordinate=1)
        .point(vertex_id=0)
        .value(2)
        == 1,
        "convert_polygonal_surface2D_into_3D keywords",
    )
    generic2d = mesh.convert_surface_mesh3D_into_2D(surface3d=polygonal, axis_to_remove=0)
    check(generic2d.point(vertex_id=2).value(0) == 1, "convert_surface_mesh3D_into_2D keywords")
    check(
        mesh.convert_surface_mesh2D_into_3D(surface2d=generic2d, axis_to_add=0, axis_coordinate=7)
        .point(vertex_id=2)
        .value(0)
        == 7,
        "convert_surface_mesh2D_into_3D keywords",
    )
    curve = mesh.EdgedCurve3D.create()
    curve_builder = mesh.EdgedCurveBuilder3D.create(mesh=curve)
    curve_builder.create_point(point=point3(0, 1, 2))
    curve_builder.create_point(point=point3(3, 4, 5))
    curve_builder.create_edge_with_vertices(v0_id=0, v1_id=1)
    curve2d = mesh.convert_edged_curve3D_into_2D(curve3d=curve, axis_to_remove=0)
    check(curve2d.point(vertex_id=1).value(0) == 4, "convert_edged_curve3D_into_2D keywords")
    point_set = mesh.PointSet3D.create()
    mesh.PointSetBuilder3D.create(mesh=point_set).create_point(point=point3(0, 1, 2))
    check(
        mesh.convert_point_set3D_into_2D(point_set3d=point_set, axis_to_remove=1).point(vertex_id=0).value(1) == 2,
        "convert_point_set3D_into_2D keywords",
    )
    tri = mesh.convert_surface_mesh_into_triangulated_surface3D(surface=surface3d)
    check(tri is not None and tri.nb_polygons() == 2, "convert_surface_mesh_into_triangulated_surface3D keyword")
    check(
        mesh.convert_surface_mesh_into_triangulated_surface3D(surface=polygonal) is None,
        "a quad is not convertible into a triangulated surface",
    )
    check(
        mesh.convert_surface_mesh_into_triangulated_surface2D(surface=surface2d).nb_polygons() == 2,
        "convert_surface_mesh_into_triangulated_surface2D keyword",
    )
    mesh.triangulate_surface_mesh3D(surface=polygonal)
    check(all(polygonal.nb_polygon_vertices(polygon_id=p) == 3 for p in range(polygonal.nb_polygons())), "triangulate_surface_mesh3D keyword")
    quad2d = mesh.convert_polygonal_surface3D_into_2D(surface3d=polygonal, axis_to_remove=2)
    mesh.triangulate_surface_mesh2D(surface=quad2d)
    check(quad2d.nb_polygons() == 2, "triangulate_surface_mesh2D keyword")
    grid2d = mesh.RegularGrid2D.create()
    mesh.RegularGridBuilder2D.create(grid=grid2d).initialize_cartesian_grid(
        origin=point2(0, 0), cells_number=[2, 2], cells_length=1.0
    )
    check(mesh.convert_grid_into_triangulated_surface(grid=grid2d).nb_polygons() == 8, "convert_grid_into_triangulated_surface keyword")
    grid3d = mesh.RegularGrid3D.create()
    mesh.RegularGridBuilder3D.create(grid=grid3d).initialize_cartesian_grid(
        origin=point3(0, 0, 0), cells_number=[2, 2, 2], cells_length=1.0
    )
    check(mesh.convert_grid_into_tetrahedral_solid(grid=grid3d).nb_polyhedra() > 8, "convert_grid_into_tetrahedral_solid keyword")
    solid, _ = build_tetrahedral_solid()
    check(
        mesh.convert_solid_mesh_into_tetrahedral_solid(solid=solid).nb_polyhedra() == 2,
        "convert_solid_mesh_into_tetrahedral_solid keyword",
    )
    # Rasterization
    segment2d = geometry.Segment2D(point0=point2(0.5, 0.5), point1=point2(1.5, 0.5))
    check(
        [0, 0] in mesh.rasterize_segment2D(grid=grid2d, segment=segment2d),
        "rasterize_segment2D keywords",
    )
    segment3d = geometry.Segment3D(point0=point3(0.5, 0.5, 0.5), point1=point3(1.5, 0.5, 0.5))
    check(
        [1, 0, 0] in mesh.rasterize_segment3D(grid=grid3d, segment=segment3d),
        "rasterize_segment3D keywords",
    )
    check(
        len(mesh.conservative_rasterize_segment2D(grid=grid2d, segment=segment2d)) >= 2,
        "conservative_rasterize_segment2D keywords",
    )
    check(
        len(mesh.conservative_rasterize_segment3D(grid=grid3d, segment=segment3d)) >= 2,
        "conservative_rasterize_segment3D keywords",
    )
    triangle2d = geometry.Triangle2D(point0=point2(0.2, 0.2), point1=point2(1.8, 0.2), point2=point2(0.2, 1.8))
    check(
        len(mesh.conservative_rasterize_triangle2D(grid=grid2d, triangle=triangle2d)) >= 3,
        "conservative_rasterize_triangle2D keywords",
    )
    triangle3d = geometry.Triangle3D(
        point0=point3(0.2, 0.2, 0.5), point1=point3(1.8, 0.2, 0.5), point2=point3(0.2, 1.8, 0.5)
    )
    check(
        len(mesh.conservative_rasterize_triangle3D(grid=grid3d, triangle=triangle3d)) >= 3,
        "conservative_rasterize_triangle3D keywords",
    )
    tetrahedron = geometry.Tetrahedron(
        point0=point3(0.2, 0.2, 0.2),
        point1=point3(1.8, 0.2, 0.2),
        point2=point3(0.2, 1.8, 0.2),
        point3=point3(0.2, 0.2, 1.8),
    )
    check(
        [0, 0, 0] in mesh.rasterize_tetrahedron(grid=grid3d, tetrahedron=tetrahedron),
        "rasterize_tetrahedron keywords",
    )
    # Closed surface: the boundary of a tetrahedron
    closed = mesh.TriangulatedSurface3D.create()
    closed_builder = mesh.TriangulatedSurfaceBuilder3D.create(mesh=closed)
    for point in [(0.2, 0.2, 0.2), (1.8, 0.2, 0.2), (0.2, 1.8, 0.2), (0.2, 0.2, 1.8)]:
        closed_builder.create_point(point=point3(*point))
    for triangle in [[0, 2, 1], [0, 1, 3], [0, 3, 2], [3, 1, 2]]:
        closed_builder.create_triangle(vertices=triangle)
    closed_builder.compute_polygon_adjacencies()
    check(
        [0, 0, 0] in mesh.rasterize_closed_surface(grid=grid3d, closed_surface=closed),
        "rasterize_closed_surface keywords",
    )
    # Distance transform
    distance = mesh.euclidean_distance_transform2D(
        grid=grid2d, grid_cell_ids=[[0, 0]], distance_map_name="distance"
    )
    check(distance.value(3) == 2 ** 0.5, "euclidean_distance_transform2D keywords")
    distance = mesh.euclidean_distance_transform3D(
        grid=grid3d, grid_cell_ids=[[0, 0, 0]], distance_map_name="distance"
    )
    check(distance.value(1) == 1, "euclidean_distance_transform3D keywords")
    # Gradient of a scalar attribute
    surface, _ = build_triangle_surface2D()
    attribute_id = surface.vertex_attribute_manager().create_attribute_variable_double(
        attribute_name="f", default_value=0.0
    )
    attribute = surface.vertex_attribute_manager().find_attribute_variable_double(
        attribute_id=attribute_id
    )
    for vertex in range(4):
        attribute.set_value(vertex, surface.point(vertex).value(0))
    gradient_id = mesh.compute_surface_scalar_function_gradient2D(
        mesh=surface, scalar_function_id=attribute_id
    )
    check(
        surface.vertex_attribute_manager().attribute_exists(attribute_id=gradient_id),
        "compute_surface_scalar_function_gradient2D keywords",
    )
    surface3d, _ = build_triangle_surface3D()
    attribute_id = surface3d.vertex_attribute_manager().create_attribute_variable_double(
        attribute_name="f", default_value=0.0
    )
    attribute = surface3d.vertex_attribute_manager().find_attribute_variable_double(
        attribute_id=attribute_id
    )
    for vertex in range(4):
        attribute.set_value(vertex, surface3d.point(vertex).value(0))
    check(
        surface3d.vertex_attribute_manager().attribute_exists(
            attribute_id=mesh.compute_surface_scalar_function_gradient3D(
                mesh=surface3d, scalar_function_id=attribute_id
            )
        ),
        "compute_surface_scalar_function_gradient3D keywords",
    )
    solid, _ = build_tetrahedral_solid()
    attribute_id = solid.vertex_attribute_manager().create_attribute_variable_double(
        attribute_name="f", default_value=0.0
    )
    attribute = solid.vertex_attribute_manager().find_attribute_variable_double(
        attribute_id=attribute_id
    )
    for vertex in range(5):
        attribute.set_value(vertex, solid.point(vertex).value(0))
    check(
        solid.vertex_attribute_manager().attribute_exists(
            attribute_id=mesh.compute_solid_scalar_function_gradient3D(
                mesh=solid, scalar_function_id=attribute_id
            )
        ),
        "compute_solid_scalar_function_gradient3D keywords",
    )
    # Translation and rescaling
    surface, builder = build_triangle_surface2D()
    mesh.translate_triangulated_surface2D(
        mesh=surface, builder=builder, translation=geometry.Vector2D([1, 2])
    )
    check(surface.point(vertex_id=0).inexact_equal(other=point2(1, 2)), "translate keywords")
    mesh.rescale_triangulated_surface2D(mesh=surface, builder=builder, scale=[2, 3])
    check(surface.point(vertex_id=0).inexact_equal(other=point2(2, 6)), "rescale keywords")
    solid, solid_builder = build_tetrahedral_solid()
    mesh.translate_tetrahedral_solid3D(
        mesh=solid, builder=solid_builder, translation=geometry.Vector3D([1, 1, 1])
    )
    mesh.rescale_tetrahedral_solid3D(mesh=solid, builder=solid_builder, scale=[1, 1, 2])
    check(solid.point(vertex_id=0).inexact_equal(other=point3(1, 1, 2)), "solid translate and rescale")
    point_set = mesh.PointSet2D.create()
    point_set_builder = mesh.PointSetBuilder2D.create(mesh=point_set)
    point_set_builder.create_point(point=point2(1, 1))
    mesh.translate_point_set2D(mesh=point_set, builder=point_set_builder, translation=geometry.Vector2D([1, 0]))
    check(point_set.point(vertex_id=0).value(0) == 2, "translate_point_set2D keywords")
    # Polygon orientations
    surface = mesh.TriangulatedSurface2D.create()
    builder = mesh.TriangulatedSurfaceBuilder2D.create(mesh=surface)
    for x, y in [(0, 0), (1, 0), (0, 1)]:
        builder.create_point(point=point2(x, y))
    builder.create_triangle(vertices=[0, 2, 1])
    mesh.repair_polygon_orientations2D(mesh=surface, builder=builder)
    check(surface.polygon_vertices(polygon_id=0) != [0, 2, 1], "repair_polygon_orientations2D keywords")
    surface3d, builder3d = build_triangle_surface3D()
    mesh.repair_polygon_orientations3D(mesh=surface3d, builder=builder3d)


def test_mesh_debug_helpers():
    previous = os.getcwd()
    with tempfile.TemporaryDirectory() as directory:
        os.chdir(directory)
        try:
            mesh.save_segment2D(segment=geometry.Segment2D(point2(0, 0), point2(1, 1)), suffix="a")
            mesh.save_segment3D(segment=geometry.Segment3D(point3(0, 0, 0), point3(1, 1, 1)), suffix="b")
            mesh.save_triangle2D(
                triangle=geometry.Triangle2D(point2(0, 0), point2(1, 0), point2(0, 1)), suffix="c"
            )
            mesh.save_triangle3D(
                triangle=geometry.Triangle3D(point3(0, 0, 0), point3(1, 0, 0), point3(0, 1, 0)), suffix="d"
            )
            surface, _ = build_triangle_surface2D()
            mesh.save_triangle2D(surface=surface, indices=[0, 1], suffix="e")
            surface3d, _ = build_triangle_surface3D()
            mesh.save_triangle3D(surface=surface3d, indices=[1], suffix="f")
            solid, _ = build_tetrahedral_solid()
            mesh.save_tetrahedron(tetrahedron=solid.tetrahedron(tetrahedron_id=0), suffix="g")
            mesh.save_tetrahedra(solid=solid, indices=[0, 1], suffix="h")
            files = set(os.listdir(directory))
        finally:
            os.chdir(previous)
    for name in (
        "segment_a.og_edc2d",
        "segment_b.og_edc3d",
        "triangle_c.og_tsf2d",
        "triangle_d.og_tsf3d",
        "triangle_0_e.og_tsf2d",
        "triangle_1_e.og_tsf2d",
        "triangle_1_f.og_tsf3d",
        "tet_g.og_tsf3d",
        "tet_0_h.og_tsf3d",
        "tet_1_h.og_tsf3d",
    ):
        check(name in files, f"debug helper wrote {name}")


def test_mesh():
    mesh.OpenGeodeMeshLibrary.initialize()
    test_mesh_elements()
    test_mesh_vertex_sets_graphs_curves_points()
    test_mesh_surfaces()
    test_mesh_solids()
    test_mesh_grids()
    test_mesh_crs()
    test_mesh_textures()
    test_mesh_io()
    test_mesh_helpers()
    test_mesh_debug_helpers()


def unit_box_brep():
    box = geometry.BoundingBox3D()
    box.add_point(geometry.Point3D([0, 0, 0]))
    box.add_point(geometry.Point3D([2, 3, 4]))
    return model.create_brep_from_bounding_box(box=box)


def test_model_components():
    model.OpenGeodeModelLibrary.initialize()
    surface_type = model.ComponentType(value="Surface")
    check(surface_type.get() == "Surface", "ComponentType(value=)")
    check(surface_type.matches(other=model.ComponentType("Surface")), "ComponentType.matches(other=)")
    identifier = model.ComponentID(component_type=surface_type, input_id=basic.uuid())
    check(identifier.type.get() == "Surface", "ComponentID(component_type=, input_id=)")
    identifier.type = model.ComponentType("Line")
    check(identifier.type.get() == "Line", "ComponentID.type setter")
    element = model.ComponentMeshElement(component_id=identifier, element_id=3)
    check(element.string().endswith(" 3]"), "ComponentMeshElement(component_id=, element_id=)")
    vertex = model.ComponentMeshVertex(component_id=identifier, vertex_id=2)
    check(vertex.vertex == 2, "ComponentMeshVertex(component_id=, vertex_id=)")
    vertex.vertex = 5
    check(vertex.vertex == 5, "ComponentMeshVertex.vertex setter")
    info = model.Info(component_type=surface_type, attribute_id=basic.uuid())
    info.component_type = model.ComponentType("Block")
    check(info.component_type.get() == "Block", "Info.component_type setter")
    check(model.RayTracingResult().position == model.RayTracingResult.Position.outside, "RayTracingResult.position")
    options = model.SectionExtruderOptions()
    options.axis_to_extrude = 1
    options.min_coordinate = -1.0
    options.max_coordinate = 2.0
    check((options.axis_to_extrude, options.min_coordinate, options.max_coordinate) == (1, -1.0, 2.0), "SectionExtruderOptions setters")


def test_model_brep_topology():
    brep = model.BRep()
    builder = model.BRepBuilder(brep=brep)
    impl = mesh.MeshImpl("OpenGeodePointSet3D")
    corner = builder.add_corner(impl=impl)
    corners = [corner] + [builder.add_corner() for _ in range(5)]
    line = builder.add_line(impl=mesh.MeshImpl("OpenGeodeEdgedCurve3D"))
    other_line = builder.add_line()
    surface = builder.add_surface(impl=mesh.MeshImpl("OpenGeodeTriangulatedSurface3D"))
    other_surface = builder.add_surface()
    block = builder.add_block(impl=mesh.MeshImpl("OpenGeodeTetrahedralSolid3D"))
    boundary = builder.add_model_boundary()
    corner_collection = builder.add_corner_collection()
    line_collection = builder.add_line_collection()
    surface_collection = builder.add_surface_collection()
    block_collection = builder.add_block_collection()
    c = lambda index: brep.corner(id=corners[index])
    builder.add_corner_line_boundary_relationship(corner=c(0), line=brep.line(id=line))
    builder.add_corner_line_boundary_relationship(corner=c(1), line=brep.line(id=line))
    builder.add_line_surface_boundary_relationship(line=brep.line(id=line), surface=brep.surface(id=surface))
    builder.add_surface_block_boundary_relationship(surface=brep.surface(id=surface), block=brep.block(id=block))
    builder.add_corner_line_internal_relationship(corner=c(2), line=brep.line(id=line))
    builder.add_corner_surface_internal_relationship(corner=c(3), surface=brep.surface(id=surface))
    builder.add_line_surface_internal_relationship(line=brep.line(id=other_line), surface=brep.surface(id=surface))
    builder.add_corner_block_internal_relationship(corner=c(4), block=brep.block(id=block))
    builder.add_line_block_internal_relationship(line=brep.line(id=other_line), block=brep.block(id=block))
    builder.add_surface_block_internal_relationship(surface=brep.surface(id=other_surface), block=brep.block(id=block))
    builder.add_surface_in_model_boundary(surface=brep.surface(id=surface), boundary=brep.model_boundary(id=boundary))
    builder.add_corner_in_corner_collection(corner=c(0), collection=brep.corner_collection(id=corner_collection))
    builder.add_line_in_line_collection(line=brep.line(id=line), collection=brep.line_collection(id=line_collection))
    builder.add_surface_in_surface_collection(surface=brep.surface(id=surface), collection=brep.surface_collection(id=surface_collection))
    builder.add_block_in_block_collection(block=brep.block(id=block), collection=brep.block_collection(id=block_collection))
    line_object = brep.line(id=line)
    surface_object = brep.surface(id=surface)
    block_object = brep.block(id=block)
    check(len(brep.boundary_corners(line=line_object)) == 2, "BRep.boundary_corners(line=)")
    check(len(brep.boundary_lines(surface=surface_object)) == 1, "BRep.boundary_lines(surface=)")
    check(len(brep.boundary_surfaces(block=block_object)) == 1, "BRep.boundary_surfaces(block=)")
    check(len(brep.incident_lines(corner=c(0))) == 1, "BRep.incident_lines(corner=)")
    check(len(brep.incident_surfaces(line=line_object)) == 1, "BRep.incident_surfaces(line=)")
    check(len(brep.incident_blocks(surface=surface_object)) == 1, "BRep.incident_blocks(surface=)")
    check(brep.nb_internal_corners_of_line(line=line_object) == 1, "nb_internal_corners_of_line(line=)")
    check(len(brep.internal_corners_of_line(line=line_object)) == 1, "internal_corners_of_line(line=)")
    check(brep.nb_internal_corners_of_surface(surface=surface_object) == 1, "nb_internal_corners_of_surface(surface=)")
    check(len(brep.internal_corners_of_surface(surface=surface_object)) == 1, "internal_corners_of_surface(surface=)")
    check(brep.nb_internal_lines_of_surface(surface=surface_object) == 1, "nb_internal_lines_of_surface(surface=)")
    check(len(brep.internal_lines_of_surface(surface=surface_object)) == 1, "internal_lines_of_surface(surface=)")
    check(brep.nb_internal_corners_of_block(block=block_object) == 1, "nb_internal_corners_of_block(block=)")
    check(len(brep.internal_corners_of_block(block=block_object)) == 1, "internal_corners_of_block(block=)")
    check(brep.nb_internal_lines_of_block(block=block_object) == 1, "nb_internal_lines_of_block(block=)")
    check(len(brep.internal_lines_of_block(block=block_object)) == 1, "internal_lines_of_block(block=)")
    check(brep.nb_internal_surfaces_of_block(block=block_object) == 1, "nb_internal_surfaces_of_block(block=)")
    check(len(brep.internal_surfaces_of_block(block=block_object)) == 1, "internal_surfaces_of_block(block=)")
    check(brep.nb_embedding_lines_of_corner(corner=c(2)) == 1, "nb_embedding_lines_of_corner(corner=)")
    check(len(brep.embedding_lines_of_corner(corner=c(2))) == 1, "embedding_lines_of_corner(corner=)")
    check(brep.nb_embedding_surfaces_of_corner(corner=c(3)) == 1, "nb_embedding_surfaces_of_corner(corner=)")
    check(len(brep.embedding_surfaces_of_corner(corner=c(3))) == 1, "embedding_surfaces_of_corner(corner=)")
    check(brep.nb_embedding_surfaces_of_line(line=brep.line(id=other_line)) == 1, "nb_embedding_surfaces_of_line(line=)")
    check(len(brep.embedding_surfaces_of_line(line=brep.line(id=other_line))) == 1, "embedding_surfaces_of_line(line=)")
    check(brep.nb_embedding_blocks_of_corner(corner=c(4)) == 1, "nb_embedding_blocks_of_corner(corner=)")
    check(len(brep.embedding_blocks_of_corner(corner=c(4))) == 1, "embedding_blocks_of_corner(corner=)")
    check(brep.nb_embedding_blocks_of_line(line=brep.line(id=other_line)) == 1, "nb_embedding_blocks_of_line(line=)")
    check(len(brep.embedding_blocks_of_line(line=brep.line(id=other_line))) == 1, "embedding_blocks_of_line(line=)")
    check(brep.nb_embedding_blocks_of_surface(surface=brep.surface(id=other_surface)) == 1, "nb_embedding_blocks_of_surface(surface=)")
    check(len(brep.embedding_blocks_of_surface(surface=brep.surface(id=other_surface))) == 1, "embedding_blocks_of_surface(surface=)")
    check(len(brep.model_boundary_items(boundary=brep.model_boundary(id=boundary))) == 1, "model_boundary_items(boundary=)")
    check(len(brep.corner_collection_items(collection=brep.corner_collection(id=corner_collection))) == 1, "corner_collection_items(collection=)")
    check(len(brep.line_collection_items(collection=brep.line_collection(id=line_collection))) == 1, "line_collection_items(collection=)")
    check(len(brep.surface_collection_items(collection=brep.surface_collection(id=surface_collection))) == 1, "surface_collection_items(collection=)")
    check(len(brep.block_collection_items(collection=brep.block_collection(id=block_collection))) == 1, "block_collection_items(collection=)")
    check(brep.is_line_closed(line=line_object) is False, "is_line_closed(line=)")
    check(brep.is_surface_closed(surface=brep.surface(id=other_surface)), "is_surface_closed(surface=)")
    check(brep.is_line_boundary(corner=c(0), line=line_object), "is_line_boundary")
    check(brep.is_surface_boundary(line=line_object, surface=surface_object), "is_surface_boundary")
    check(brep.is_block_boundary(surface=surface_object, block=block_object), "is_block_boundary")
    check(brep.is_corner_in_line_internals(corner=c(2), line=line_object), "is_corner_in_line_internals")
    check(brep.is_corner_in_surface_internals(corner=c(3), surface=surface_object), "is_corner_in_surface_internals")
    check(brep.is_line_in_surface_internals(line=brep.line(id=other_line), surface=surface_object), "is_line_in_surface_internals")
    check(brep.is_corner_in_block_internals(corner=c(4), block=block_object), "is_corner_in_block_internals")
    check(brep.is_line_in_block_internals(line=brep.line(id=other_line), block=block_object), "is_line_in_block_internals")
    check(brep.is_surface_in_block_internals(surface=brep.surface(id=other_surface), block=block_object), "is_surface_in_block_internals")
    check(brep.is_model_boundary_item(surface=surface_object, boundary=brep.model_boundary(id=boundary)), "is_model_boundary_item")
    check(brep.is_corner_collection_item(corner=c(0), corner_collection=brep.corner_collection(id=corner_collection)), "is_corner_collection_item")
    check(brep.is_line_collection_item(line=line_object, line_collection=brep.line_collection(id=line_collection)), "is_line_collection_item")
    check(brep.is_surface_collection_item(surface=surface_object, surface_collection=brep.surface_collection(id=surface_collection)), "is_surface_collection_item")
    check(brep.is_block_collection_item(block=block_object, block_collection=brep.block_collection(id=block_collection)), "is_block_collection_item")
    check(brep.brep_component(component_id=surface).id() == surface, "brep_component(component_id=)")
    # Relationships API on the uuids
    check(brep.nb_boundaries(component_id=line) == 2, "Relationships.nb_boundaries(component_id=)")
    check(len(brep.boundaries(component_id=line)) == 2, "Relationships.boundaries(component_id=)")
    check(brep.nb_incidences(component_id=line) == 1, "Relationships.nb_incidences")
    check(len(brep.incidences(component_id=line)) == 1, "Relationships.incidences")
    check(brep.nb_internals(component_id=line) == 1, "Relationships.nb_internals")
    check(len(brep.internals(component_id=line)) == 1, "Relationships.internals")
    check(brep.nb_embeddings(component_id=corners[2]) == 1, "Relationships.nb_embeddings")
    check(len(brep.embeddings(component_id=corners[2])) == 1, "Relationships.embeddings")
    check(brep.nb_items(component_id=boundary) == 1, "Relationships.nb_items")
    check(len(brep.items(component_id=boundary)) == 1, "Relationships.items")
    check(brep.nb_collections(id=surface) == 2, "Relationships.nb_collections(id=)")
    check(len(brep.collections(id=surface)) == 2, "Relationships.collections(id=)")
    check(brep.is_boundary(boundary=corners[0], incidence=line), "Relationships.is_boundary")
    check(brep.is_internal(internal=corners[2], embedding=line), "Relationships.is_internal")
    check(brep.is_item(item=surface, collection=boundary), "Relationships.is_item")
    # relationship builder
    relationships_builder = model.RelationshipsBuilder(relationships=brep)
    surface_id = surface_object.component_id()
    first = relationships_builder.add_item_in_collection(item=model.ComponentID(model.ComponentType("Corner"), corners[5]), collection=model.ComponentID(model.ComponentType("CornerCollection"), corner_collection))
    check(brep.is_item(item=corners[5], collection=corner_collection), "RelationshipsBuilder.add_item_in_collection")
    check(isinstance(first, int), "add_item_in_collection returns an index")
    relationships_builder.add_boundary_relation(boundary=model.ComponentID(model.ComponentType("Corner"), corners[5]), incidence=line_object.component_id())
    check(brep.is_boundary(corners[5], line), "RelationshipsBuilder.add_boundary_relation")
    relationships_builder.add_internal_relation(internal=model.ComponentID(model.ComponentType("Line"), line), embedding=block_object.component_id())
    check(brep.is_internal(line, block), "RelationshipsBuilder.add_internal_relation")
    relationships_builder.remove_relation(component_id1=corners[5], component_id2=line)
    check(not brep.is_boundary(corners[5], line), "RelationshipsBuilder.remove_relation")
    relationships_builder.unregister_component(component_id=corners[5])
    check(brep.nb_collections(id=corners[5]) == 0, "RelationshipsBuilder.unregister_component")
    # names
    builder.set_corner_name(corner=c(0), name="c0")
    builder.set_line_name(line=line_object, name="l")
    builder.set_surface_name(surface=surface_object, name="s")
    builder.set_block_name(block=block_object, name="b")
    builder.set_model_boundary_name(id=brep.model_boundary(id=boundary), name="mb")
    builder.set_corner_collection_name(corner_collection=brep.corner_collection(id=corner_collection), name="cc")
    builder.set_line_collection_name(line_collection=brep.line_collection(id=line_collection), name="lc")
    builder.set_surface_collection_name(surface_collection=brep.surface_collection(id=surface_collection), name="sc")
    builder.set_block_collection_name(block_collection=brep.block_collection(id=block_collection), name="bc")
    check([c(0).name(), line_object.name(), surface_object.name(), block_object.name()] == ["c0", "l", "s", "b"], "set_xxx_name keywords")
    # mesh builders
    check(builder.corner_mesh_builder(corner=c(0)).create_point(point=geometry.Point3D([0, 0, 0])) == 0, "corner_mesh_builder(corner=)")
    line_builder = builder.line_mesh_builder(line=line_object)
    line_builder.create_point(point=geometry.Point3D([0, 0, 0]))
    check(brep.line(id=line).mesh().nb_vertices() == 1, "line_mesh_builder(line=)")
    builder.surface_mesh_builder(surface=surface_object).create_point(point=geometry.Point3D([0, 0, 0]))
    check(surface_object.mesh().nb_vertices() == 1, "surface_mesh_builder(surface=)")
    builder.block_mesh_builder(block=block_object).create_point(point=geometry.Point3D([0, 0, 0]))
    check(block_object.mesh().nb_vertices() == 1, "block_mesh_builder(block=)")
    # removal
    builder.remove_corner(corner=c(1))
    builder.remove_line(line=brep.line(id=other_line))
    builder.remove_surface(surface=brep.surface(id=other_surface))
    builder.remove_model_boundary(boundary=brep.model_boundary(id=boundary))
    builder.remove_corner_collection(collection=brep.corner_collection(id=corner_collection))
    builder.remove_line_collection(collection=brep.line_collection(id=line_collection))
    builder.remove_surface_collection(collection=brep.surface_collection(id=surface_collection))
    builder.remove_block_collection(collection=brep.block_collection(id=block_collection))
    builder.remove_block(block=block_object)
    check((brep.nb_blocks(), brep.nb_surfaces(), brep.nb_lines(), brep.nb_corners()) == (0, 1, 1, 5), "remove_xxx keywords")
    check_raises(lambda: brep.line(idd=line), TypeError, "an unknown keyword raises TypeError")


def test_model_vertex_identifier_and_registry():
    brep = model.BRep()
    builder = model.BRepBuilder(brep=brep)
    corner = builder.add_corner()
    line = builder.add_line()
    builder.corner_mesh_builder(corner=brep.corner(corner)).create_point(point=geometry.Point3D([0, 0, 0]))
    line_builder = builder.line_mesh_builder(line=brep.line(line))
    for x in range(3):
        line_builder.create_point(point=geometry.Point3D([x, 0, 0]))
    first = builder.create_unique_vertices(nb=2)
    check(first == 0 and brep.nb_unique_vertices() == 2, "create_unique_vertices(nb=)")
    corner_id = brep.corner(corner).component_id()
    line_id = brep.line(line).component_id()
    builder.set_unique_vertex(component_vertex_id=model.ComponentMeshVertex(component_id=corner_id, vertex_id=0), unique_vertex_id=0)
    builder.set_unique_vertex(component_vertex_id=model.ComponentMeshVertex(component_id=line_id, vertex_id=0), unique_vertex_id=0)
    builder.set_unique_vertex(component_vertex_id=model.ComponentMeshVertex(component_id=line_id, vertex_id=2), unique_vertex_id=1)
    check(len(brep.component_mesh_vertices(unique_vertex_id=0)) == 2, "component_mesh_vertices(unique_vertex_id=)")
    check(brep.unique_vertex(component_vertex=model.ComponentMeshVertex(component_id=line_id, vertex_id=2)) == 1, "unique_vertex(component_vertex=)")
    check_raises(lambda: brep.component_mesh_vertices(unique_vertex_id=2), IndexError, "an unknown unique vertex raises IndexError")
    builder.update_unique_vertices(component_id=line_id, old2new=[basic.NO_ID, 0, 1])
    check(
        [v.vertex for v in brep.component_mesh_vertices(0) if v.component_id == line_id] == [],
        "update_unique_vertices(component_id=, old2new=) detaches the deleted vertex",
    )
    check(
        [v.vertex for v in brep.component_mesh_vertices(1)] == [1],
        "update_unique_vertices(component_id=, old2new=) renumbers the vertex",
    )
    mapping = builder.delete_isolated_vertices()
    check(len(mapping) == 2, "delete_isolated_vertices returns the mapping")
    # specific registration of each component kind with keywords
    other = model.BRep()
    other_builder = model.BRepBuilder(brep=other)
    identifier_builder = model.VertexIdentifierBuilder(vertex_identifier=other)
    registry_builder = model.ComponentRegistryBuilder(component_registry=other)
    registry_builder.add_mesh_component(type=model.ComponentType("Corner"), id=basic.uuid())
    registry_builder.add_collection_component(type=model.ComponentType("ModelBoundary"), id=basic.uuid())
    check(len(other.mesh_components()) == 1 and len(other.collection_components()) == 1, "ComponentRegistryBuilder.add_xxx_component(type=, id=)")
    mesh_id = list(other.mesh_components().values())[0][0]
    registry_builder.remove_mesh_component(type=model.ComponentType("Corner"), id=mesh_id)
    collection_id = list(other.collection_components().values())[0][0]
    registry_builder.remove_collection_component(type=model.ComponentType("ModelBoundary"), id=collection_id)
    check(sum(len(v) for v in other.mesh_components().values()) == 0, "ComponentRegistryBuilder.remove_xxx_component")
    for kind, adder in (
        ("corner3D", other_builder.add_corner),
        ("line3D", other_builder.add_line),
        ("surface3D", other_builder.add_surface),
        ("block3D", other_builder.add_block),
    ):
        uuid = adder()
        component = getattr(other, kind[:-2])(uuid)
        getattr(identifier_builder, f"unregister_{kind}_component")(component=component)
        getattr(identifier_builder, f"register_{kind}_component")(component=component)
    section = model.Section()
    section_builder = model.SectionBuilder(section=section)
    section_identifier = model.VertexIdentifierBuilder(vertex_identifier=section)
    for kind, adder in (
        ("corner2D", section_builder.add_corner),
        ("line2D", section_builder.add_line),
        ("surface2D", section_builder.add_surface),
    ):
        component = getattr(section, kind[:-2])(adder())
        getattr(section_identifier, f"unregister_{kind}_component")(component=component)
        getattr(section_identifier, f"register_{kind}_component")(component=component)
    unique = model.VertexIdentifierBuilder(vertex_identifier=other)
    check(unique.create_unique_vertex() == 0, "create_unique_vertex")


def test_model_physical_properties_and_mappings():
    brep = model.BRep()
    builder = model.PhysicalPropertiesBuilder(physical_properties=brep)
    name = model.PhysicalPropertyName.porosity
    attribute_id = basic.uuid()
    builder.set_physical_property(name=name, component_type=model.ComponentType("Block"), attribute_id=attribute_id)
    check(brep.has_physical_property(name=name), "has_physical_property(name=)")
    check(brep.physical_property_info(name=name).attribute_id == attribute_id, "physical_property_info(name=)")
    other = model.BRep()
    model.PhysicalPropertiesBuilder(physical_properties=other).copy_physical_properties(other=brep)
    check(other.has_physical_property(name), "copy_physical_properties(other=)")
    source = unit_box_brep()
    copy = model.BRep()
    mapping = model.BRepBuilder(brep=copy).copy(brep=source)
    corner_type = model.Corner3D.component_type_static()
    check(mapping.at(type=corner_type).has_mapping_input(source.corners()[0].id()), "ModelCopyMapping.at(type=)")
    moved = model.ModelCopyMapping(mapping=mapping)
    check(moved.at(type=corner_type).has_mapping_input(source.corners()[0].id()), "ModelCopyMapping(mapping=)")
    check_raises(lambda: mapping.at(type=corner_type), KeyError, "a moved mapping is empty")
    generic = model.ModelGenericMapping()
    check_raises(lambda: generic.at(type=corner_type), KeyError, "no GenericMapping for an unknown type")
    model.ModelGenericMapping(mapping=generic)
    section = model.Section()
    section_mapping = model.SectionBuilder(section=section).copy(section=model.convert_brep_into_section(brep=source, axis_to_remove=2)[0])
    check(section.nb_corners() == 8, "SectionBuilder.copy(section=)")
    check(
        section_mapping.at(type=model.Corner2D.component_type_static()).has_mapping_output(section.corners()[0].id()),
        "SectionBuilder.copy maps the corners",
    )
    concatener = model.BRepConcatener(model=copy)
    concatener.concatenate(other_model=source)
    check(copy.nb_corners() == 16, "BRepConcatener(model=).concatenate(other_model=)")
    section_concatener = model.SectionConcatener(model=section)
    section_concatener.concatenate(other_model=model.convert_brep_into_section(brep=source, axis_to_remove=2)[0])
    check(section.nb_corners() == 16, "SectionConcatener(model=).concatenate(other_model=)")


def test_model_helpers():
    # opengeode registers the file formats of the other modules: import it
    # after the checks assuming that no format is registered
    import opengeode

    brep = unit_box_brep()
    block = brep.blocks()[0]
    check(abs(model.block_volume(brep=brep, block=block) - 24) < 1e-9, "block_volume(brep=, block=)")
    box = model.block_bounding_box(brep=brep, block=block)
    check(box.max().value(2) == 4, "block_bounding_box(brep=, block=)")
    first_surface = brep.surfaces()[0]
    polygon_areas = sum(first_surface.mesh().polygon_area(p) for p in range(first_surface.mesh().nb_polygons()))
    check(abs(model.surface_area3D(surface=first_surface) - polygon_areas) < 1e-9, "surface_area3D(surface=)")
    section, _ = model.convert_brep_into_section(brep=brep, axis_to_remove=2)
    check(model.surface_area2D(surface=section.surfaces()[0]) > 0, "surface_area2D(surface=)")
    check(model.convert_section_into_brep(section=section, axis_to_add=2, axis_coordinate=1.0)[0].nb_corners() == 8, "convert_section_into_brep keywords")
    options = model.SectionExtruderOptions()
    options.axis_to_extrude = 2
    options.max_coordinate = 1.0
    check(model.extrude_section_to_brep(section=section, options=options).nb_blocks() == 6, "extrude_section_to_brep keywords")
    model.triangulate_brep_surface_meshes(model=brep)
    model.triangulate_section_surface_meshes(model=section)
    model.convert_brep_surface_meshes_into_triangulated_surfaces(model=brep)
    model.convert_section_surface_meshes_into_triangulated_surfaces(model=section)
    model.convert_brep_block_meshes_into_tetrahedral_solids(model=brep)
    check(model.convert_brep_into_curve(brep=brep)[0].nb_edges() == 12, "convert_brep_into_curve(brep=)")
    check(model.convert_brep_into_surface(brep=brep)[0].nb_polygons() == 12, "convert_brep_into_surface(brep=)")
    check_raises(
        lambda: model.convert_brep_into_solid(brep=brep),
        basic.OpenGeodeDataError,
        "convert_brep_into_solid(brep=) needs the unique vertices in Blocks",
    )
    check(model.convert_section_into_curve(section=section)[0].nb_edges() == 12, "convert_section_into_curve(section=)")
    check(model.convert_section_into_surface(section=section)[0].nb_polygons() == 12, "convert_section_into_surface(section=)")
    check(model.filter_brep_components_with_regards_to_blocks(brep=brep) == [], "filter_brep_components_with_regards_to_blocks(brep=)")
    check(model.filter_section_components_with_regards_to_surfaces(section=section) == [], "filter_section_components_with_regards_to_surfaces(section=)")
    # ray tracing
    tracing = model.BRepRayTracing(brep=brep)
    block = brep.blocks()[0]
    inside = tracing.is_point_inside_block(point=geometry.Point3D([1, 1, 1]), block=block)
    check(inside and inside.position == model.RayTracingResult.Position.inside, "is_point_inside_block keywords")
    check(tracing.block_containing_point(point=geometry.Point3D([1, 1, 1])) == block.id(), "block_containing_point(point=)")
    # coordinate reference systems
    check(len(model.brep_coordinate_reference_systems(brep=brep)) == 1, "brep_coordinate_reference_systems(brep=)")
    check(len(model.section_coordinate_reference_systems(section=section)) == 1, "section_coordinate_reference_systems(section=)")
    check(len(model.brep_active_coordinate_reference_systems(brep=brep)) == 1, "brep_active_coordinate_reference_systems(brep=)")
    check(len(model.section_active_coordinate_reference_systems(section=section)) == 1, "section_active_coordinate_reference_systems(section=)")
    system = geometry.CoordinateSystem2D()
    shifted = geometry.CoordinateSystem2D(directions=[geometry.Vector2D([1, 0]), geometry.Vector2D([0, 1])], origin=geometry.Point2D([1, 1]))
    builder = model.BRepBuilder(brep=brep)
    model.create_brep_coordinate_system(model=brep, builder=builder, new_coordinate_system_name="shifted", input=system, output=shifted)
    model.set_brep_active_coordinate_system(model=brep, builder=builder, coordinate_system_name="shifted")
    check(model.brep_active_coordinate_reference_systems(brep)[0][1] == "shifted", "set_brep_active_coordinate_system keywords")
    section_builder = model.SectionBuilder(section=section)
    model.create_section_coordinate_system(model=section, builder=section_builder, new_coordinate_system_name="shifted", input=system, output=shifted)
    model.set_section_active_coordinate_system(model=section, builder=section_builder, coordinate_system_name="shifted")
    check(model.section_active_coordinate_reference_systems(section)[0][1] == "shifted", "set_section_active_coordinate_system keywords")
    # polygon helpers on a meshed BRep
    data_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../../../tests/data"))
    filename = os.path.join(data_dir, "test_mesh3.og_brep")
    meshed = opengeode.load_one(model.load_brep(filename=filename), filename)
    block = meshed.blocks()[0]
    surface = meshed.boundary_surfaces(block=block)[0]
    facet = mesh.PolyhedronFacet(0, 0)
    check(len(model.block_polygon_unique_vertices(model=meshed, block=block, facet=facet)) == 3, "block_polygon_unique_vertices keywords")
    check(len(model.surface_polygon_unique_vertices(model=meshed, surface=surface, polygon_id=0)) in (3, 4), "surface_polygon_unique_vertices keywords")
    check(len(model.block_mesh_polyhedra_from_surface_polygon(model=meshed, block=block, surface=surface, polygon_id=0)) <= 2, "block_mesh_polyhedra_from_surface_polygon keywords")
    check(len(model.block_vertices_from_surface_polygon(model=meshed, block=block, surface=surface, polygon_id=0)) <= 2, "block_vertices_from_surface_polygon keywords")
    check(model.oriented_block_vertices_from_surface_polygon(model=meshed, block=block, surface=surface, polygon_id=0).nb_facets() <= 2, "oriented_block_vertices_from_surface_polygon keywords")
    line = meshed.boundary_lines(surface=surface)[0]
    check(len(model.brep_surface_vertices_from_line_edge(model=meshed, surface=surface, line=line, edge_id=0)) <= 2, "brep_surface_vertices_from_line_edge keywords")
    check(model.brep_oriented_surface_vertices_from_line_edge(model=meshed, surface=surface, line=line, edge_id=0).nb_edges() <= 2, "brep_oriented_surface_vertices_from_line_edge keywords")
    section_filename = os.path.join(data_dir, "fractures.og_sctn")
    fractures = opengeode.load_one(model.load_section(filename=section_filename), section_filename)
    fracture = fractures.surfaces()[0]
    fracture_line = fractures.boundary_lines(surface=fracture)[0]
    check(len(model.section_surface_vertices_from_line_edge(model=fractures, surface=fracture, line=fracture_line, edge_id=0)) <= 2, "section_surface_vertices_from_line_edge keywords")
    check(model.section_oriented_surface_vertices_from_line_edge(model=fractures, surface=fracture, line=fracture_line, edge_id=0).nb_edges() <= 2, "section_oriented_surface_vertices_from_line_edge keywords")


def test_model_io():
    brep = unit_box_brep()
    section, _ = model.convert_brep_into_section(brep=brep, axis_to_remove=2)
    with tempfile.TemporaryDirectory() as directory:
        brep_file = os.path.join(directory, "box.og_brep")
        section_file = os.path.join(directory, "box.og_sctn")
        check(len(model.save_brep(brep=brep, filename=brep_file)) >= 1, "save_brep keywords")
        check(len(model.save_section(section=section, filename=section_file)) >= 1, "save_section keywords")
        check(len(model.load_brep(filename=brep_file)) == 1, "load_brep(filename=)")
        check(len(model.load_section(filename=section_file)) == 1, "load_section(filename=)")
        check(str(model.is_brep_loadable(filename=brep_file)) == "100%", "is_brep_loadable(filename=)")
        check(str(model.is_section_loadable(filename=section_file)) == "100%", "is_section_loadable(filename=)")
        check(model.is_brep_saveable(brep=brep, filename=brep_file), "is_brep_saveable keywords")
        check(model.is_section_saveable(section=section, filename=section_file), "is_section_saveable keywords")
        check(model.brep_object_priority(filename=brep_file) >= 0, "brep_object_priority(filename=)")
        check(model.section_object_priority(filename=section_file) >= 0, "section_object_priority(filename=)")
        model.brep_additional_files(filename=brep_file)
        model.section_additional_files(filename=section_file)
        check(str(model.is_brep_time_series_loadable(filename=brep_file)) == "0%", "is_brep_time_series_loadable(filename=)")
        check_raises(lambda: model.brep_time_series_additional_files(filename=brep_file), basic.OpenGeodeDataError, "brep_time_series_additional_files(filename=)")
        series = model.ExternalTimeSeries()
        check_raises(lambda: model.load_brep_time_series(brep=brep, filename=os.path.join(directory, "none.og_ets")), basic.OpenGeodeError, "load_brep_time_series keywords, missing file")
        check_raises(lambda: model.load_external_time_series(filename=os.path.join(directory, "none.txt")), basic.OpenGeodeError, "load_external_time_series(filename=), wrong extension")
        check_raises(lambda: model.save_external_time_series(time_series=series, filename=os.path.join(directory, "none.txt")), basic.OpenGeodeError, "save_external_time_series keywords, wrong extension")
        check_raises(lambda: model.load_brep(filename=os.path.join(directory, "none.og_brep")), basic.OpenGeodeError, "load_brep of a missing file raises")
        check_raises(lambda: model.load_section(filename=os.path.join(directory, "none.og_sctn")), basic.OpenGeodeError, "load_section of a missing file raises")
    points = mesh.PointSet3D.create()
    series = model.ExternalTimeSeries()
    series.set_unique_vertices(vertices=points)
    series.set_cell_barycenters(barycenters=mesh.PointSet3D.create())
    series.apply_to(model=brep)  # no point to match: the Blocks are left untouched


def test_model():
    test_model_components()
    test_model_brep_topology()
    test_model_vertex_identifier_and_registry()
    test_model_physical_properties_and_mappings()
    test_model_helpers()
    test_model_io()


if __name__ == "__main__":
    test_geometry()
    test_basic()
    test_image()
    test_mesh()
    test_model()
