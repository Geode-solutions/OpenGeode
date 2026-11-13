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

"""The behaviors stated by the docstrings of the mesh module are the real ones
(one check per claim whose truth is not obvious from the name)."""

import os
import tempfile

import geode_test_env

import opengeode_py_basic as basic
import opengeode_py_geometry as geometry
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


def point2(x, y):
    return geometry.Point2D([x, y])


def point3(x, y, z):
    return geometry.Point3D([x, y, z])


def triangle_pair():
    surface = mesh.TriangulatedSurface2D.create()
    builder = mesh.TriangulatedSurfaceBuilder2D.create(surface)
    for x, y in [(0, 0), (1, 0), (0, 1), (1, 1)]:
        builder.create_point(point2(x, y))
    builder.create_triangle([0, 1, 2])
    builder.create_triangle([1, 3, 2])
    builder.compute_polygon_adjacencies()
    return surface, builder


def test_builders_return_mappings():
    point_set = mesh.PointSet2D.create()
    builder = mesh.PointSetBuilder2D.create(point_set)
    for x in range(3):
        builder.create_point(point2(x, 0))
    # permute_vertices: the vertex at position i is the old vertex permutation[i]
    old2new = builder.permute_vertices([2, 0, 1])
    check(old2new == [1, 2, 0], "permute_vertices returns the old to new mapping")
    check(
        [point_set.point(i).value(0) for i in range(3)] == [2, 0, 1],
        "the vertex at position i is the old vertex permutation[i]",
    )
    # delete_vertices: NO_ID for the deleted vertices
    check(
        builder.delete_vertices([False, True, False]) == [0, basic.NO_ID, 1],
        "delete_vertices returns NO_ID for a deleted vertex",
    )
    check(point_set.nb_vertices() == 2, "delete_vertices deletes the flagged vertices")
    check(
        builder.delete_vertices([False, False]) == [0, 1],
        "delete_vertices without any flag keeps everything",
    )
    # create_vertices returns the index of the first created vertex
    check(builder.create_vertices(2) == 2, "create_vertices returns the first index")
    check(builder.create_vertex() == 4, "create_vertex returns the index")


def test_polygon_local_indexation():
    surface, _ = triangle_pair()
    for edge in range(3):
        start = surface.polygon_edge_vertex(mesh.PolygonEdge(0, edge), 0)
        end = surface.polygon_edge_vertex(mesh.PolygonEdge(0, edge), 1)
        vertices = surface.polygon_vertices(0)
        check(start == vertices[edge], "edge i starts at the vertex i")
        check(end == vertices[(edge + 1) % 3], "edge i ends at the vertex i + 1")
        check(
            surface.polygon_vertex(mesh.PolygonVertex(mesh.PolygonEdge(0, edge)))
            == start,
            "PolygonVertex(polygon_edge) is the start of the edge",
        )
        check(
            mesh.PolygonEdge(mesh.PolygonVertex(0, edge)) == mesh.PolygonEdge(0, edge),
            "PolygonEdge(polygon_vertex) starts at the vertex",
        )
    default = mesh.PolygonVertex()
    check(
        default.polygon_id == basic.NO_ID and default.vertex_id != 0,
        "a default polygon vertex is invalid",
    )
    check(mesh.EdgeVertex().edge_id == basic.NO_ID, "a default edge endpoint is invalid")
    check(mesh.MeshElement().element_id == basic.NO_ID, "a default element is invalid")
    # polygon_edge_from_vertices is ordered, from_edge_vertices is not
    check(
        surface.polygon_edge_from_vertices(0, 1) == mesh.PolygonEdge(0, 0),
        "polygon_edge_from_vertices finds the edge going from the first vertex",
    )
    check(
        surface.polygon_edge_from_vertices(1, 0) is None,
        "polygon_edge_from_vertices is ordered",
    )
    check(
        surface.polygon_edge_from_edge_vertices([1, 0]) == mesh.PolygonEdge(0, 0),
        "polygon_edge_from_edge_vertices accepts any order",
    )
    check(surface.polygon_adjacent(mesh.PolygonEdge(0, 0)) is None, "border has no adjacent")
    check(
        surface.polygon_adjacent_edge(mesh.PolygonEdge(0, 1)) == mesh.PolygonEdge(1, 2),
        "the adjacent edge is the matching edge of the neighbor",
    )
    # edges can be queried in any order
    surface.enable_edges()
    edges = surface.edges()
    check(
        edges.edge_from_vertices([0, 1]) == edges.edge_from_vertices([1, 0]) is not None,
        "edge_from_vertices accepts any order",
    )
    check(edges.edge_from_vertices([0, 3]) is None, "an unknown edge is None")
    check_raises(
        lambda: mesh.TriangulatedSurface2D.create().edges(),
        basic.OpenGeodeDataError,
        "edges() raises when the edges are not enabled",
    )
    surface.disable_edges()
    check(not surface.are_edges_enabled(), "disable_edges disables the edges")


def test_create_and_clone():
    classes = [
        (mesh.VertexSet, mesh.VertexSetBuilder),
        (mesh.Graph, mesh.GraphBuilder),
        (mesh.PointSet2D, mesh.PointSetBuilder2D),
        (mesh.PointSet3D, mesh.PointSetBuilder3D),
        (mesh.EdgedCurve2D, mesh.EdgedCurveBuilder2D),
        (mesh.EdgedCurve3D, mesh.EdgedCurveBuilder3D),
        (mesh.PolygonalSurface2D, mesh.PolygonalSurfaceBuilder2D),
        (mesh.TriangulatedSurface3D, mesh.TriangulatedSurfaceBuilder3D),
        (mesh.PolyhedralSolid3D, mesh.PolyhedralSolidBuilder3D),
        (mesh.TetrahedralSolid3D, mesh.TetrahedralSolidBuilder3D),
        (mesh.HybridSolid3D, mesh.HybridSolidBuilder3D),
        (mesh.RegularGrid2D, mesh.RegularGridBuilder2D),
        (mesh.RegularGrid3D, mesh.RegularGridBuilder3D),
    ]
    for cls, builder_type in classes:
        created = cls.create()
        check(created.nb_vertices() == 0, f"{cls.__name__}.create is empty")
        impl = mesh.MeshFactory.default_impl(created.type_name())
        check(created.impl_name() == impl, f"{cls.__name__}.create uses the default data structure")
        builder_type.create(created).set_name("named")
        clone = created.clone()
        check(clone.impl_name() == created.impl_name(), f"{cls.__name__}.clone keeps the data structure")
        check(clone.name() == "named", f"{cls.__name__}.clone keeps the name")
        check(clone.id() == created.id(), f"{cls.__name__}.clone keeps the identifier")
        check(
            mesh.MeshFactory.type(clone.impl_name()) == created.type_name(),
            f"{cls.__name__} type and data structure match",
        )
    surface, _ = triangle_pair()
    clone = surface.clone()
    check(clone.nb_polygons() == 2 and clone.point(3).value(0) == 1, "clone copies the polygons and points")


def test_crs_manager_claims():
    surface, builder = triangle_pair()
    manager = surface.main_coordinate_reference_system_manager()
    main = builder.main_coordinate_reference_system_manager_builder()
    check(manager.active_coordinate_reference_system_name() == "points", "default crs is points")
    active = manager.active_coordinate_reference_system()
    check(
        surface.point(2).inexact_equal(active.point(2)),
        "mesh.point is the point of the active crs",
    )
    check_raises(
        lambda: manager.find_coordinate_reference_system("unknown"),
        basic.OpenGeodeDataError,
        "an unknown crs raises",
    )
    check_raises(
        lambda: main.coordinate_reference_system("unknown"),
        basic.OpenGeodeDataError,
        "an unknown crs to modify raises",
    )
    check_raises(
        lambda: main.set_active_coordinate_reference_system("unknown"),
        basic.OpenGeodeDataError,
        "activating an unknown crs raises",
    )
    # deleting something that does not exist is not an error
    main.delete_coordinate_reference_system("unknown")
    check(manager.nb_coordinate_reference_systems() == 1, "deleting an unknown crs changes nothing")
    mesh.create_surface_mesh_coordinate_system2D(
        surface,
        builder,
        "other",
        geometry.CoordinateSystem2D(),
        geometry.CoordinateSystem2D(),
    )
    check(
        manager.active_coordinate_reference_system_name() == "other",
        "the created crs is the active one",
    )
    # deleting the active crs leaves no active crs
    main.delete_coordinate_reference_system("other")
    check(
        manager.active_coordinate_reference_system_name() == "",
        "the active crs name is empty once the active crs is deleted",
    )
    check_raises(
        lambda: manager.active_coordinate_reference_system(),
        basic.OpenGeodeDataError,
        "no active crs raises",
    )
    check_raises(
        lambda: main.active_coordinate_reference_system(),
        basic.OpenGeodeDataError,
        "no active crs to modify raises",
    )
    main.set_active_coordinate_reference_system("points")
    check(manager.active_coordinate_reference_system_name() == "points", "set_active")
    # point of the managers of the mesh: main manager, active crs
    check(
        surface.point(1).inexact_equal(point2(1, 0)),
        "CoordinateReferenceSystemManagers.point reads the active crs",
    )
    main2 = surface.main_coordinate_reference_system_manager()
    check(
        main2.nb_coordinate_reference_systems() == manager.nb_coordinate_reference_systems(),
        "main manager is the manager of the mesh dimension",
    )
    # the mesh vertices CRS raises for an out-of-range point
    crs = manager.find_coordinate_reference_system("points")
    check_raises(lambda: crs.point(4), IndexError, "crs.point out of range")
    check_raises(
        lambda: crs.set_point(4, point2(0, 0)), IndexError, "crs.set_point out of range"
    )
    grid_manager = mesh.LightRegularGrid2D(
        point2(0, 0), [1, 1], [1.0, 1.0]
    ).grid_coordinate_reference_system_manager()
    # a grid manager lists its registered grid crs
    check(
        grid_manager.nb_coordinate_reference_systems()
        == len(grid_manager.coordinate_reference_system_names()),
        "a grid manager lists its registered crs",
    )
    # a crs cannot be registered twice under the same name
    grid = mesh.LightRegularGrid2D(point2(0, 0), [1, 1], [1.0, 1.0])
    grid_builder = mesh.GridCoordinateReferenceSystemManagerBuilder2D(grid)
    system = geometry.CoordinateSystem2D()
    grid_builder.register_coordinate_reference_system("a", mesh.GridCoordinateReferenceSystem2D(system))
    check_raises(
        lambda: grid_builder.register_coordinate_reference_system(
            "a", mesh.GridCoordinateReferenceSystem2D(system)
        ),
        basic.OpenGeodeDataError,
        "registering the same name twice raises",
    )
    # activating a grid crs moves the grid geometry to its coordinate system
    moved = geometry.CoordinateSystem2D(
        [geometry.Vector2D([2, 0]), geometry.Vector2D([0, 2])], point2(5, 5)
    )
    grid_builder.register_coordinate_reference_system(
        "moved", mesh.GridCoordinateReferenceSystem2D(moved)
    )
    grid_builder.set_active_coordinate_reference_system("moved")
    check(
        grid.grid_coordinate_system().origin().inexact_equal(point2(5, 5)),
        "activating a grid crs moves the grid origin",
    )
    check(grid.cell_length_in_direction(0) == 2, "activating a grid crs moves the directions")


def test_grid_claims():
    grid = mesh.LightRegularGrid3D(point3(1, 2, 3), [2, 3, 4], [1.0, 2.0, 0.5])
    check(grid.nb_cell_vertices() == 8, "a 3D cell has 8 vertices")
    check(mesh.LightRegularGrid2D(point2(0, 0), [1, 1], [1.0, 1.0]).nb_cell_vertices() == 4, "a 2D cell has 4 vertices")
    check(
        [grid.nb_vertices_in_direction(d) for d in range(3)] == [3, 4, 5],
        "nb_vertices_in_direction is the number of cells plus one",
    )
    check(grid.nb_grid_vertices() == 3 * 4 * 5, "nb_grid_vertices")
    check(grid.cell_size() == 1.0 * 2.0 * 0.5, "cell_size is the product of the lengths")
    check(
        grid.nb_vertices_on_borders() == 3 * 4 * 5 - 1 * 2 * 3,
        "nb_vertices_on_borders",
    )
    check(
        grid.grid_point([1, 1, 1]).inexact_equal(point3(2, 4, 3.5)),
        "grid_point",
    )
    check(
        grid.vertex_indices(grid.vertex_index([2, 3, 4])) == [2, 3, 4],
        "vertex_index and vertex_indices are inverse",
    )
    check(
        grid.cell_vertex_indices([1, 1, 1], 0) == [1, 1, 1]
        and grid.cell_vertex_indices([1, 1, 1], 7) == [2, 2, 2],
        "cell_vertex_indices local vertex 0 and 7",
    )
    check(
        grid.cell_barycenter([0, 0, 0]).inexact_equal(point3(1.5, 3, 3.25)),
        "cell_barycenter",
    )
    check(grid.next_vertex([2, 0, 0], 0) is None, "next_vertex at the end is None")
    check(grid.previous_vertex([0, 0, 0], 2) is None, "previous_vertex at the start is None")
    check(grid.next_vertex([0, 0, 0], 1) == [0, 1, 0], "next_vertex")
    check(grid.previous_vertex([0, 1, 0], 1) == [0, 0, 0], "previous_vertex")
    check(grid.is_grid_vertex_on_border([0, 1, 1]), "border vertex")
    check(not grid.is_grid_vertex_on_border([1, 1, 1]), "inner vertex")
    check(grid.contains(point3(1, 2, 3)), "the origin is inside")
    check(not grid.contains(point3(0, 2, 3)), "a point outside is not contained")
    check(
        grid.closest_vertex(point3(1.9, 2.1, 3.0)) == [1, 0, 0],
        "closest_vertex",
    )
    check(grid.closest_vertex(point3(-10, -10, -10)) == [0, 0, 0], "closest_vertex outside")
    check(grid.cells(point3(-10, -10, -10)) == [], "no cell contains an outside point")
    check(len(grid.cells(point3(1.5, 3.0, 3.25))) == 1, "one cell contains an inner point")
    near_limit = grid.cells(point3(2.0, 3.0, 3.25))
    check(len(near_limit) == 2, "two cells contain a point on their common limit")
    bounding_box = grid.grid_bounding_box()
    check(
        bounding_box.min().inexact_equal(point3(1, 2, 3))
        and bounding_box.max().inexact_equal(point3(3, 8, 5)),
        "grid_bounding_box",
    )
    check(
        grid.native_extension() == "og_lrgd3d", "light regular grid native extension"
    )
    # regular grid: update_origin moves the vertices
    regular = mesh.RegularGrid2D.create()
    builder = mesh.RegularGridBuilder2D.create(regular)
    builder.initialize_cartesian_grid(point2(0, 0), [2, 2], 1.0)
    check(regular.nb_vertices() == 9 and regular.nb_polygons() == 4, "one polygon per cell")
    builder.update_origin(point2(3, 4))
    check(regular.point(0).inexact_equal(point2(3, 4)), "update_origin moves the vertices")
    check(
        regular.grid_coordinate_system().origin().inexact_equal(point2(3, 4)),
        "update_origin moves the origin",
    )
    check(
        regular.bounding_box().max().inexact_equal(point2(5, 6)), "regular grid bounding box"
    )


def test_texture_claims():
    surface, _ = triangle_pair()
    manager = surface.texture_manager()
    nb_attributes = len(surface.polygon_attribute_manager().attribute_ids())
    first = manager.find_or_create_texture("t")
    check(
        len(surface.polygon_attribute_manager().attribute_ids()) == nb_attributes + 1,
        "a texture is stored as an attribute",
    )
    second = manager.find_or_create_texture("t")
    check(manager.texture_names() == ["t"], "find_or_create_texture does not duplicate")
    check(first is not None and second is not None, "textures")
    check_raises(
        lambda: manager.find_texture("other"), basic.OpenGeodeDataError, "find_texture raises"
    )
    # deleting a texture also deletes its attribute
    first.set_texture_coordinates(mesh.PolygonVertex(0, 0), point2(0.5, 0.5))
    manager.delete_texture("t")
    check(
        len(surface.polygon_attribute_manager().attribute_ids()) == nb_attributes,
        "delete_texture deletes the attribute of the texture",
    )
    # the manager keeps the mesh alive
    import gc

    def manager_of_temporary():
        temporary, _ = triangle_pair()
        return temporary.texture_manager()

    kept = manager_of_temporary()
    gc.collect()
    kept.find_or_create_texture("alive")
    check(kept.texture_exists("alive"), "the texture manager keeps the mesh alive")


def test_helper_claims():
    # rasterization of a segment partly outside the grid
    grid = mesh.RegularGrid2D.create()
    mesh.RegularGridBuilder2D.create(grid).initialize_cartesian_grid(point2(0, 0), [3, 3], 1.0)
    inside = mesh.rasterize_segment2D(
        grid, geometry.Segment2D(point2(-5, 0.5), point2(1.5, 0.5))
    )
    check(sorted(inside) == [[0, 0], [1, 0]], "only the part inside the grid is rasterized")
    check(
        mesh.rasterize_segment2D(grid, geometry.Segment2D(point2(-5, -5), point2(-4, -4))) == [],
        "a segment outside gives no cell",
    )
    # a conservative rasterization takes every touched cell
    touched = mesh.conservative_rasterize_segment2D(
        grid, geometry.Segment2D(point2(0.5, 0.5), point2(1.0, 0.5))
    )
    check([1, 0] in touched and [0, 0] in touched, "conservative rasterization of a segment")
    # the closed surface rasterization includes the interior
    grid3d = mesh.RegularGrid3D.create()
    mesh.RegularGridBuilder3D.create(grid3d).initialize_cartesian_grid(point3(0, 0, 0), [5, 5, 5], 1.0)
    closed = mesh.TriangulatedSurface3D.create()
    builder = mesh.TriangulatedSurfaceBuilder3D.create(closed)
    for x, y, z in [(0.5, 0.5, 0.5), (4.5, 0.5, 0.5), (0.5, 4.5, 0.5), (0.5, 0.5, 4.5)]:
        builder.create_point(point3(x, y, z))
    for triangle in [[0, 2, 1], [0, 1, 3], [0, 3, 2], [3, 1, 2]]:
        builder.create_triangle(triangle)
    cells = mesh.rasterize_closed_surface(grid3d, closed)
    check([1, 1, 1] in cells, "an interior cell is covered by a closed surface")
    check([4, 4, 4] not in cells, "a far cell is not covered")
    # triangulate_surface_mesh replaces the polygons with more than 3 vertices
    quad = mesh.PolygonalSurface2D.create()
    quad_builder = mesh.PolygonalSurfaceBuilder2D.create(quad)
    for x, y in [(0, 0), (1, 0), (1, 1), (0, 1)]:
        quad_builder.create_point(point2(x, y))
    quad_builder.create_polygon([0, 1, 2, 3])
    mesh.triangulate_surface_mesh2D(quad)
    check(
        quad.nb_polygons() == 2
        and all(quad.nb_polygon_vertices(p) == 3 for p in range(2)),
        "triangulate_surface_mesh replaces the polygons by triangles",
    )
    check(mesh.convert_surface_mesh_into_triangulated_surface2D(quad) is not None, "triangles")
    quad_builder.create_polygon([0, 1, 2, 3])
    check(
        mesh.convert_surface_mesh_into_triangulated_surface2D(quad) is None,
        "a surface with a quad has no triangulated version",
    )
    # solid conversion
    hybrid = mesh.HybridSolid3D.create()
    hybrid_builder = mesh.HybridSolidBuilder3D.create(hybrid)
    for point in [(0, 0, 0), (1, 0, 0), (0, 1, 0), (0, 0, 1), (1, 1, 0), (1, 1, 1), (0, 1, 1), (1, 0, 1)]:
        hybrid_builder.create_point(point3(*point))
    hybrid_builder.create_tetrahedron([0, 1, 2, 3])
    check(
        mesh.convert_solid_mesh_into_tetrahedral_solid(hybrid).nb_polyhedra() == 1,
        "a hybrid solid with only tetrahedra is convertible",
    )
    hybrid_builder.create_hexahedron([0, 1, 4, 2, 3, 7, 5, 6])
    check(
        mesh.convert_solid_mesh_into_tetrahedral_solid(hybrid) is None,
        "a solid with a hexahedron has no tetrahedral version",
    )
    # gradient attribute name
    surface, _ = triangle_pair()
    attribute_id = surface.vertex_attribute_manager().create_attribute_variable_double("temperature", 0.0)
    attribute = surface.vertex_attribute_manager().find_attribute_variable_double(attribute_id)
    for vertex in range(4):
        attribute.set_value(vertex, 2 * surface.point(vertex).value(0))
    gradient_id = mesh.compute_surface_scalar_function_gradient2D(surface, attribute_id)
    check(
        surface.vertex_attribute_manager().attribute_ids_matching_name("temperature_gradient")
        == [gradient_id],
        "the returned id is the one of the gradient attribute",
    )
    # distance transform: units of the grid, max double without objects
    grid = mesh.RegularGrid2D.create()
    mesh.RegularGridBuilder2D.create(grid).initialize_cartesian_grid(point2(0, 0), [3, 3], 2.0)
    distance = mesh.euclidean_distance_transform2D(grid, [[0, 0]], "d")
    check(distance.value(1) == 2.0 and distance.value(4) == 2.0 * 2 ** 0.5, "distance in the units of the grid")
    empty = mesh.euclidean_distance_transform2D(grid, [], "e")
    check(empty.value(0) == 1.7976931348623157e308, "no object: the maximum double value")
    # repair orientations
    surface = mesh.TriangulatedSurface2D.create()
    builder = mesh.TriangulatedSurfaceBuilder2D.create(surface)
    for x, y in [(0, 0), (1, 0), (0, 1)]:
        builder.create_point(point2(x, y))
    builder.create_triangle([0, 2, 1])
    mesh.repair_polygon_orientations2D(surface, builder)
    check(surface.polygon_area(0) > 0, "negative area polygons are reversed")
    # all the polygons have a null area: nothing to orient
    flat = mesh.TriangulatedSurface2D.create()
    flat_builder = mesh.TriangulatedSurfaceBuilder2D.create(flat)
    for x in range(3):
        flat_builder.create_point(point2(x, 0))
    flat_builder.create_triangle([0, 1, 2])
    check_raises(
        lambda: mesh.repair_polygon_orientations2D(flat, flat_builder),
        basic.OpenGeodeDataError,
        "repair_polygon_orientations2D raises when every polygon has a null area",
    )
    # 3D: adjacent polygons end up with opposite directions on their common edge
    surface3d = mesh.TriangulatedSurface3D.create()
    builder3d = mesh.TriangulatedSurfaceBuilder3D.create(surface3d)
    for x, y in [(0, 0), (1, 0), (0, 1), (1, 1)]:
        builder3d.create_point(point3(x, y, 0))
    builder3d.create_triangle([0, 1, 2])
    builder3d.create_triangle([1, 2, 3])
    builder3d.compute_polygon_adjacencies()
    mesh.repair_polygon_orientations3D(surface3d, builder3d)
    shared_edge = mesh.PolygonEdge(0, 1)
    adjacent = surface3d.polygon_adjacent_edge(shared_edge)
    check(
        surface3d.polygon_edge_vertex(shared_edge, 0)
        == surface3d.polygon_edge_vertex(adjacent, 1),
        "repair_polygon_orientations3D makes the orientations consistent",
    )
    # axis conversions
    solid3d = mesh.PointSet3D.create()
    mesh.PointSetBuilder3D.create(solid3d).create_point(point3(1, 2, 3))
    for axis, expected in [(0, (2, 3)), (1, (1, 3)), (2, (1, 2))]:
        converted = mesh.convert_point_set3d_into_2d(solid3d, axis)
        check(
            converted.point(0).inexact_equal(point2(*expected)),
            f"the coordinate of the axis {axis} is removed",
        )


def tetrahedra_pair():
    solid = mesh.TetrahedralSolid3D.create()
    builder = mesh.TetrahedralSolidBuilder3D.create(solid)
    for point in [(0, 0, 0), (1, 0, 0), (0, 1, 0), (0, 0, 1), (1, 1, 1)]:
        builder.create_point(point3(*point))
    builder.create_tetrahedron([0, 1, 2, 3])
    builder.create_tetrahedron([1, 2, 3, 4])
    builder.compute_polyhedron_adjacencies()
    return solid, builder


def test_polyhedra_around_edge_hint():
    solid, builder = tetrahedra_pair()
    # a hint containing the edge gives all the polyhedra around it
    for hint in (0, 1):
        check(
            sorted(solid.polyhedra_around_edge_with_hint([1, 2], hint)) == [0, 1],
            f"polyhedra_around_edge_with_hint with the valid hint {hint}",
        )
    check(solid.polyhedra_around_edge([0, 1]) == [0], "edge of one polyhedron only")
    # a hint that does not contain the edge is returned as is
    check(
        solid.polyhedra_around_edge_with_hint([0, 4], 1) == [1],
        "a hint is returned even if no polyhedron contains the edge",
    )
    check(
        1 in solid.polyhedra_around_edge_with_hint([0, 1], 1),
        "a hint without the edge is still in the result",
    )
    check_raises(
        lambda: solid.polyhedra_around_edge_with_hint([1, 2], 5),
        IndexError,
        "hint out of range",
    )
    check_raises(
        lambda: solid.polyhedra_around_edge_with_hint([1, 9], 0),
        IndexError,
        "vertex out of range",
    )


def test_adjacency_dependent_queries():
    surface = mesh.TriangulatedSurface2D.create()
    builder = mesh.TriangulatedSurfaceBuilder2D.create(surface)
    for x, y in [(0, 0), (1, 0), (0, 1), (1, 1)]:
        builder.create_point(point2(x, y))
    builder.create_triangle([0, 1, 2])
    builder.create_triangle([1, 3, 2])
    # without adjacencies the polygons around a vertex are incomplete
    check(
        len(surface.polygons_around_vertex(1)) == 1,
        "without adjacencies only one polygon is found around a vertex",
    )
    check(
        surface.polygon_edge_from_vertices(1, 2) is None
        and surface.polygon_edge_from_vertices(2, 1) == mesh.PolygonEdge(1, 2),
        "without adjacencies the edge of the first polygon is silently not found",
    )
    check(
        len(surface.polygons_from_edge_vertices([1, 2])) == 1,
        "without adjacencies the polygons around an edge are incomplete",
    )
    builder.compute_polygon_adjacencies()
    check(len(surface.polygons_around_vertex(1)) == 2, "with adjacencies")
    check(surface.polygon_edge_from_vertices(2, 1) == mesh.PolygonEdge(1, 2), "edge found")
    check(surface.polygon_edge_from_vertices(1, 2) == mesh.PolygonEdge(0, 1), "other edge found")
    check(
        surface.polygon_edge_from_edge_vertices([1, 2]) is not None
        and len(surface.polygons_from_edge_vertices([1, 2])) == 2,
        "with adjacencies",
    )
    # polyhedra_from_facet_vertices needs the adjacencies to find both polyhedra
    solid = mesh.TetrahedralSolid3D.create()
    solid_builder = mesh.TetrahedralSolidBuilder3D.create(solid)
    for point in [(0, 0, 0), (1, 0, 0), (0, 1, 0), (0, 0, 1), (1, 1, 1)]:
        solid_builder.create_point(point3(*point))
    solid_builder.create_tetrahedron([0, 1, 2, 3])
    solid_builder.create_tetrahedron([1, 2, 3, 4])
    check(
        len(solid.polyhedra_from_facet_vertices([1, 2, 3])) <= 1,
        "without adjacencies at most one polyhedron facet is found",
    )
    solid_builder.compute_polyhedron_adjacencies()
    check(
        len(solid.polyhedra_from_facet_vertices([1, 2, 3])) == 2,
        "with adjacencies both polyhedron facets are found",
    )


def test_graph_builder_results():
    graph = mesh.Graph.create()
    builder = mesh.GraphBuilder.create(graph)
    builder.create_vertices(3)
    for vertices in [(0, 1), (1, 2), (0, 2)]:
        builder.create_edge_with_vertices(*vertices)
    check(graph.nb_edges() == 3, "create_edge_with_vertices creates edges")
    check(graph.edge_vertices(2) == [0, 2], "create_edge_with_vertices sets the vertices")
    check(builder.permute_edges([2, 0, 1]) == [1, 2, 0], "permute_edges returns old to new")
    check(
        [graph.edge_vertices(i) for i in range(3)] == [[0, 2], [0, 1], [1, 2]],
        "the edge at position i is the old edge permutation[i]",
    )
    check(
        builder.delete_edges([False, True, False]) == [0, basic.NO_ID, 1],
        "delete_edges returns NO_ID for a deleted edge",
    )
    check(graph.nb_edges() == 2, "delete_edges removes the edges")
    check(
        [graph.edge_vertices(i) for i in range(2)] == [[0, 2], [1, 2]],
        "delete_edges keeps the other edges in order",
    )
    check(graph.edge_vertex(mesh.EdgeVertex(0, 1)) == 2, "edge endpoints after deletion")
    check(len(graph.edges_around_vertex(0)) == 1, "edges around a vertex after deletion")
    # Error paths, on a graph that is not used afterwards (a rejected call
    # may leave the edges around the vertices inconsistent)
    other = mesh.Graph.create()
    other_builder = mesh.GraphBuilder.create(other)
    other_builder.create_vertices(3)
    for vertices in [(0, 1), (1, 2), (0, 2)]:
        other_builder.create_edge_with_vertices(*vertices)
    builder = other_builder
    check_raises(
        lambda: builder.permute_edges([0, 0, 1]),
        basic.OpenGeodeDataError,
        "permute_edges needs a permutation",
    )
    check_raises(
        lambda: builder.permute_edges([0, 1]),
        basic.OpenGeodeDataError,
        "permute_edges needs one value per edge",
    )
    builder = mesh.GraphBuilder.create(other)
    check_raises(
        lambda: builder.delete_edges([True]),
        basic.OpenGeodeDataError,
        "delete_edges needs one flag per edge",
    )


def test_edges_and_facets_builders():
    surface = mesh.TriangulatedSurface2D.create()
    builder = mesh.TriangulatedSurfaceBuilder2D.create(surface)
    for x, y in [(0, 0), (1, 0), (0, 1), (1, 1)]:
        builder.create_point(point2(x, y))
    builder.create_triangle([0, 1, 2])
    builder.create_triangle([1, 3, 2])
    builder.compute_polygon_adjacencies()
    surface.enable_edges()
    edges_builder = builder.edges_builder()
    check(
        edges_builder.delete_isolated_edges() == [0, 1, 2, 3, 4],
        "no isolated edge to delete",
    )
    check(surface.edges().nb_edges() == 5, "edges kept")
    builder.delete_polygons([False, True])
    check(surface.edges().nb_edges() == 5, "deleting a polygon keeps its edges")
    mapping = edges_builder.delete_isolated_edges()
    check(
        len(mapping) == 5 and mapping.count(basic.NO_ID) == 2,
        "the two edges only used by the deleted polygon are deleted",
    )
    check(surface.edges().nb_edges() == 3, "isolated edges deleted")
    check(
        sorted(sorted(surface.edges().edge_vertices(i)) for i in range(3))
        == [[0, 1], [0, 2], [1, 2]],
        "the remaining edges are the edges of the kept polygon",
    )
    check(
        surface.edges().edge_from_vertices([1, 3]) is None,
        "a deleted edge is not found anymore",
    )
    # solid
    solid, solid_builder = tetrahedra_pair()
    solid.enable_edges()
    solid.enable_facets()
    edges_builder = solid_builder.edges_builder()
    facets_builder = solid_builder.facets_builder()
    check(solid.edges().nb_edges() == 9 and solid.facets().nb_facets() == 7, "counts")
    check(
        edges_builder.delete_isolated_edges() == list(range(9)),
        "no isolated solid edge",
    )
    check(
        facets_builder.delete_isolated_facets() == list(range(7)),
        "no isolated solid facet",
    )
    solid_builder.delete_polyhedra([False, True])
    facet_mapping = facets_builder.delete_isolated_facets()
    check(
        len(facet_mapping) == 7 and facet_mapping.count(basic.NO_ID) == 3,
        "the three facets only used by the deleted tetrahedron are deleted",
    )
    check(solid.facets().nb_facets() == 4, "facets of the kept tetrahedron")
    edge_mapping = edges_builder.delete_isolated_edges()
    check(
        len(edge_mapping) == 9 and edge_mapping.count(basic.NO_ID) == 3,
        "the three edges only used by the deleted tetrahedron are deleted",
    )
    check(solid.edges().nb_edges() == 6, "edges of the kept tetrahedron")
    check(
        solid.facets().facet_from_vertices([1, 2, 4]) is None,
        "a deleted facet is not found anymore",
    )


def test_grid_directions():
    for dimension in (2, 3):
        grid_type = mesh.RegularGrid2D if dimension == 2 else mesh.RegularGrid3D
        builder_type = (
            mesh.RegularGridBuilder2D if dimension == 2 else mesh.RegularGridBuilder3D
        )
        vector_type = geometry.Vector2D if dimension == 2 else geometry.Vector3D
        make_point = point2 if dimension == 2 else (lambda x, y: point3(x, y, 0))
        directions = [vector_type([2, 0] + [0] * (dimension - 2)), vector_type([0, 3] + [0] * (dimension - 2))]
        if dimension == 3:
            directions.append(vector_type([0, 0, 5]))
        grid = grid_type.create()
        builder_type.create(grid).initialize_grid_with_directions(
            make_point(1, 1), [2] * dimension, directions
        )
        check(grid.cell_length_in_direction(0) == 2, "length along direction 0")
        check(grid.cell_length_in_direction(1) == 3, "length along direction 1")
        check(grid.nb_grid_vertices() == 3 ** dimension, "number of vertices")
        check(grid.cell_size() == (6 if dimension == 2 else 30), "cell size")
        check(
            grid.grid_point([1] * dimension).inexact_equal(
                make_point(3, 4) if dimension == 2 else point3(3, 4, 5)
            ),
            "grid point along the directions",
        )
        check(
            grid.point(grid.vertex_index([2] * dimension)).inexact_equal(
                make_point(5, 7) if dimension == 2 else point3(5, 7, 10)
            ),
            "mesh point along the directions",
        )
        check(
            grid.grid_coordinate_system().origin().inexact_equal(make_point(1, 1)),
            "origin",
        )


def test_gradient_without_value():
    surface = mesh.TriangulatedSurface2D.create()
    builder = mesh.TriangulatedSurfaceBuilder2D.create(surface)
    for x, y in [(0, 0), (1, 0), (0, 1), (1, 1)]:
        builder.create_point(point2(x, y))
    builder.create_triangle([0, 1, 2])
    builder.create_triangle([1, 3, 2])
    builder.compute_polygon_adjacencies()
    manager = surface.vertex_attribute_manager()
    attribute_id = manager.create_attribute_variable_double("f", 0.0)
    attribute = manager.find_attribute_variable_double(attribute_id)
    for vertex in range(4):
        attribute.set_value(vertex, float(vertex))
    attribute.set_value(3, float("nan"))
    gradient_id = mesh.compute_surface_scalar_function_gradient2D(surface, attribute_id)
    gradient = manager.find_generic_attribute(gradient_id)
    import math

    def has_gradient(vertex):
        return all(
            math.isfinite(gradient.generic_item_value(vertex, item)) for item in range(2)
        )

    # vertex 3 has a NaN value and is the neighbor of vertices 1 and 2
    check(has_gradient(0), "the gradient is computed where there are values")
    check(
        all(
            math.isnan(gradient.generic_item_value(vertex, item))
            for vertex in (1, 2, 3)
            for item in range(2)
        ),
        "the vertices with a NaN scalar value or neighbor hold the NaN no_value",
    )


def test_names_claims():
    check(mesh.CRSType().get() == "", "a default CRSType has an empty name")
    check(mesh.CRSType("a").get() == "a", "CRSType(value)")
    check_raises(
        lambda: mesh.MeshFactory.default_impl(mesh.MeshType("Unknown")),
        basic.OpenGeodeError,
        "default_impl of an unknown type raises",
    )


def test_io_claims():
    with tempfile.TemporaryDirectory() as directory:
        surface, _ = triangle_pair()
        filename = os.path.join(directory, "surface.og_tsf2d")
        # save returns the written files, with the extension selecting the format
        check(
            mesh.save_triangulated_surface2D(surface, filename) == [filename],
            "save returns the list of the written files",
        )
        loaded = mesh.load_triangulated_surface2D(filename)
        check(len(loaded) == 1, "load returns a list")
        check(
            loaded[0].impl_name() == mesh.MeshFactory.default_impl(loaded[0].type_name()),
            "load uses the default data structure",
        )
        check(
            mesh.is_triangulated_surface_loadable2D(filename).value() > 0,
            "a native file is loadable",
        )
        check(
            mesh.is_triangulated_surface_loadable2D(os.path.join(directory, "a.unknown")).value() == 0,
            "an unknown extension has a null Percentage",
        )
        check(
            not mesh.is_triangulated_surface_saveable2D(surface, os.path.join(directory, "a.unknown")),
            "an unknown extension is not saveable",
        )
        check_raises(
            lambda: mesh.triangulated_surface_object_priority2D(os.path.join(directory, "a.unknown")),
            basic.OpenGeodeDataError,
            "the priority of a file without loader raises",
        )
        check_raises(
            lambda: mesh.triangulated_surface_additional_files2D(os.path.join(directory, "a.unknown")),
            basic.OpenGeodeDataError,
            "the additional files of a file without loader raise",
        )
        check_raises(
            lambda: mesh.save_triangulated_surface2D(surface, os.path.join(directory, "a.unknown")),
            basic.OpenGeodeError,
            "saving in an unknown format raises",
        )


# ----------------------------------------------------------------------- model
DATA_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), "../../../../tests/data"))


def box_brep(x=2, y=3, z=4):
    box = geometry.BoundingBox3D()
    box.add_point(point3(0, 0, 0))
    box.add_point(point3(x, y, z))
    return model.create_brep_from_bounding_box(box)


def topology_brep():
    """One block with a boundary surface, an internal surface, lines, corners
    and collections: every kind of relation."""
    brep = model.BRep()
    builder = model.BRepBuilder(brep)
    ids = {}
    ids["corners"] = [builder.add_corner() for _ in range(5)]
    ids["lines"] = [builder.add_line() for _ in range(2)]
    ids["surfaces"] = [builder.add_surface() for _ in range(2)]
    ids["block"] = builder.add_block()
    ids["boundary"] = builder.add_model_boundary()
    ids["corner_collection"] = builder.add_corner_collection()
    corner = lambda i: brep.corner(ids["corners"][i])
    line = lambda i: brep.line(ids["lines"][i])
    surface = lambda i: brep.surface(ids["surfaces"][i])
    block = brep.block(ids["block"])
    builder.add_corner_line_boundary_relationship(corner(0), line(0))
    builder.add_corner_line_boundary_relationship(corner(1), line(0))
    builder.add_line_surface_boundary_relationship(line(0), surface(0))
    builder.add_surface_block_boundary_relationship(surface(0), block)
    builder.add_corner_line_internal_relationship(corner(2), line(0))
    builder.add_corner_surface_internal_relationship(corner(3), surface(0))
    builder.add_line_surface_internal_relationship(line(1), surface(0))
    builder.add_corner_block_internal_relationship(corner(4), block)
    builder.add_surface_block_internal_relationship(surface(1), block)
    builder.add_surface_in_model_boundary(surface(0), brep.model_boundary(ids["boundary"]))
    builder.add_corner_in_corner_collection(corner(0), brep.corner_collection(ids["corner_collection"]))
    return brep, builder, ids


def names(components):
    return sorted((c.id() if callable(c.id) else c.id).string() for c in components)


def test_model_relationships():
    brep, builder, ids = topology_brep()
    corners, lines, surfaces = ids["corners"], ids["lines"], ids["surfaces"]
    line0 = brep.line(lines[0])
    # boundary is "bounds", incidence is the dual, internal is "lies inside"
    check(names(brep.boundary_corners(line0)) == sorted(c.string() for c in corners[:2]), "the boundaries of a line are its corners")
    check(names(brep.incident_lines(brep.corner(corners[0]))) == [lines[0].string()], "the incidences of a corner are the lines it bounds")
    check(names(brep.internal_corners_of_line(line0)) == [corners[2].string()], "an internal corner lies on the line without bounding it")
    check(names(brep.embedding_lines_of_corner(brep.corner(corners[2]))) == [lines[0].string()], "the embeddings of a corner are the lines it is internal to")
    check(brep.nb_embedding_blocks_of_corner(brep.corner(corners[4])) == 1, "a corner can be internal to a block")
    check(brep.nb_embedding_blocks_of_surface(brep.surface(surfaces[1])) == 1, "a surface can be internal to a block")
    check(brep.nb_internal_surfaces_of_block(brep.block(ids["block"])) == 1, "the internal surface of a block is not a boundary")
    check(brep.nb_boundaries(ids["block"]) == 1, "a block with one boundary surface")
    # Relationships work on the uuids: unknown uuids have no relation
    unknown = basic.uuid()
    check(brep.nb_boundaries(unknown) == 0 and brep.boundaries(unknown) == [], "an unknown uuid has no boundary")
    check(brep.nb_collections(unknown) == 0 and brep.collections(unknown) == [], "an unknown uuid has no collection")
    check(not brep.is_boundary(unknown, unknown), "an unknown uuid bounds nothing")
    # the order of the arguments matters in the queries
    check(brep.is_boundary(corners[0], lines[0]) and not brep.is_boundary(lines[0], corners[0]), "is_boundary(boundary, incidence) is ordered")
    check(brep.is_internal(corners[2], lines[0]) and not brep.is_internal(lines[0], corners[2]), "is_internal is ordered")
    check(brep.is_item(surfaces[0], ids["boundary"]) and not brep.is_item(ids["boundary"], surfaces[0]), "is_item is ordered")
    check(names(brep.items(ids["boundary"])) == [surfaces[0].string()], "items of a collection")
    check(len(brep.collections(surfaces[0])) == 1, "collections of an item")
    check(names(brep.model_boundary_items(brep.model_boundary(ids["boundary"]))) == [surfaces[0].string()], "model_boundary_items are surfaces in a BRep")
    # is_line_closed: fewer than two boundary corners, is_surface_closed: no boundary line
    check(not brep.is_line_closed(line0), "a line with two corners is not closed")
    check(brep.is_line_closed(brep.line(lines[1])), "a line without corner is closed")
    builder.add_corner_line_boundary_relationship(brep.corner(corners[3]), brep.line(lines[1]))
    check(brep.is_line_closed(brep.line(lines[1])), "a line with one corner (a loop) is closed")
    check(not brep.is_surface_closed(brep.surface(surfaces[0])), "a surface with a boundary line is not closed")
    check(brep.is_surface_closed(brep.surface(surfaces[1])), "a surface without boundary line is closed")
    # the component getter returns the class of the component
    check(isinstance(brep.brep_component(ids["boundary"]), model.ModelBoundary3D), "brep_component gives a ModelBoundary3D")
    check(isinstance(brep.brep_component(corners[0]), model.Corner3D), "brep_component gives a Corner3D")
    check_raises(lambda: brep.brep_component(unknown), basic.OpenGeodeDataError, "brep_component of an unknown uuid raises")
    # types
    check(brep.corner(corners[0]).component_type() == model.Corner3D.component_type_static(), "component_type_static")
    check(model.CornerCollection3D.component_type_static().get() == "CornerCollection", "the type of a collection")
    check(brep.corner(corners[0]).is_active(), "components are active by default")
    check(sorted(t.get() for t in brep.mesh_components()) == ["Block", "Corner", "Line", "Surface"], "mesh_components lists the mesh component types")
    check([t.get() for t in brep.collection_components()] == ["ModelBoundary", "CornerCollection"] or sorted(t.get() for t in brep.collection_components()) == ["CornerCollection", "ModelBoundary"], "collection_components lists the collection types")
    check(model.ComponentID().type.get() == "undefined", "a default ComponentID is undefined")
    check(model.ComponentType().get() == "", "a default ComponentType has an empty name")
    check(model.ComponentID(model.ComponentType("A"), unknown) < model.ComponentID(model.ComponentType("B"), unknown), "ComponentID is ordered by type name first")
    # RelationshipsBuilder
    relations = model.RelationshipsBuilder(brep)
    first = model.ComponentID(model.ComponentType("Corner"), corners[0])
    second = model.ComponentID(model.ComponentType("Line"), lines[0])
    index = relations.add_boundary_relation(first, second)
    check(relations.add_boundary_relation(first, second) == index, "a duplicated relation returns the index of the existing one")
    check(relations.add_boundary_relation(second, first) == index, "the existing relation is found in any order")
    check(relations.add_internal_relation(first, second) == index and brep.is_boundary(corners[0], lines[0]) and not brep.is_internal(corners[0], lines[0]), "an existing relation is left unchanged")
    relations.remove_relation(lines[0], corners[0])
    check(not brep.is_boundary(corners[0], lines[0]), "remove_relation accepts any order")
    relations.remove_relation(lines[0], corners[0])
    relations.unregister_component(lines[0])
    check(brep.nb_boundaries(lines[0]) == 0 and brep.nb_incidences(lines[0]) == 0, "unregister_component removes all the relations")
    check(brep.nb_lines() == 2, "unregister_component keeps the component")
    relations.unregister_component(unknown)


def test_model_builders():
    brep, builder, ids = topology_brep()
    check(len(brep.mesh_components()[model.Corner3D.component_type_static()]) == 5, "add_corner registers the component")
    check_raises(lambda: builder.add_corner(mesh.MeshImpl("Unknown")), basic.OpenGeodeDataError, "an unknown mesh implementation raises")
    surface_id = builder.add_surface(mesh.MeshImpl("OpenGeodePolygonalSurface3D"))
    check(isinstance(brep.surface(surface_id).polygonal_mesh(), mesh.PolygonalSurface3D), "add_surface(impl) uses the implementation")
    check_raises(lambda: brep.surface(surface_id).triangulated_mesh(), RuntimeError, "triangulated_mesh of a polygonal mesh raises")
    check_raises(lambda: brep.block(ids["block"]).hybrid_mesh(), RuntimeError, "hybrid_mesh of another solid raises")
    # names are set on the component and on its mesh
    corner = brep.corner(ids["corners"][0])
    builder.set_corner_name(corner, "name")
    check(corner.name() == "name" and corner.mesh().name() == "name", "set_corner_name names the component and its mesh")
    # mesh builder
    point_builder = builder.corner_mesh_builder(corner)
    check(point_builder.create_point(point3(1, 2, 3)) == 0 and corner.mesh().nb_vertices() == 1, "corner_mesh_builder modifies the corner mesh")
    # removal removes the relations
    line = brep.line(ids["lines"][0])
    builder.remove_surface(brep.surface(ids["surfaces"][0]))
    check(brep.nb_surfaces() == 2 and brep.nb_items(ids["boundary"]) == 0 and brep.nb_embeddings(ids["corners"][3]) == 0 and brep.nb_boundaries(ids["block"]) == 0, "remove_surface removes its relations")
    builder.remove_model_boundary(brep.model_boundary(ids["boundary"]))
    check(brep.nb_model_boundaries() == 0, "remove_model_boundary")
    # copy needs an empty BRep and returns a mapping
    source = box_brep()
    target = model.BRep()
    mapping = model.BRepBuilder(target).copy(source)
    corner_type = model.Corner3D.component_type_static()
    check(target.nb_corners() == 8 and target.nb_unique_vertices() == 8, "copy copies components and unique vertices")
    check(mapping.at(corner_type).in2out(source.corners()[0].id()) == target.corners()[0].id(), "copy returns the mapping from the source uuids")
    check_raises(lambda: model.BRepBuilder(target).copy(source), basic.OpenGeodeDataError, "copy into a non-empty BRep raises")
    moved = model.ModelCopyMapping(mapping)
    check(moved.at(corner_type).has_mapping_input(source.corners()[0].id()), "the move constructor takes the content")
    check_raises(lambda: mapping.at(corner_type), IndexError, "the moved mapping is emptied")
    check_raises(lambda: model.ModelCopyMapping().at(corner_type), IndexError, "at() of an unknown type raises IndexError")
    # concatenation appends, with new uuids, geometry and unique vertices
    concatener = model.BRepConcatener(target)
    added = concatener.concatenate(source)
    check(target.nb_corners() == 16 and target.nb_unique_vertices() == 16 and target.nb_surfaces() == 12, "concatenate adds components and unique vertices")
    check(added.at(corner_type).in2out(source.corners()[0].id()) not in [c.id() for c in source.corners()], "concatenate creates new uuids")
    # Section builder
    section_copy = model.Section()
    model.SectionBuilder(section_copy).copy(model.convert_brep_into_section(source, 2)[0])
    check_raises(lambda: model.SectionBuilder(section_copy).copy(model.Section()), basic.OpenGeodeDataError, "Section copy into a non-empty Section raises")
    check(section_copy.nb_corners() == 8, "SectionBuilder.copy")


def test_model_vertex_identifier():
    brep = model.BRep()
    builder = model.BRepBuilder(brep)
    corner0, corner1, line = builder.add_corner(), builder.add_corner(), builder.add_line()
    for corner, x in ((corner0, 0), (corner1, 1)):
        builder.corner_mesh_builder(brep.corner(corner)).create_point(point3(x, 0, 0))
    line_builder = builder.line_mesh_builder(brep.line(line))
    for x in (0, 0.5, 1):
        line_builder.create_point(point3(x, 0, 0))
    vertex = lambda component, index: model.ComponentMeshVertex(component, index)
    corner0_id, corner1_id = brep.corner(corner0).component_id(), brep.corner(corner1).component_id()
    line_id = brep.line(line).component_id()
    check(builder.create_unique_vertex() == 0 and builder.create_unique_vertices(2) == 1 and brep.nb_unique_vertices() == 3, "create_unique_vertices returns the first index")
    check(brep.unique_vertex(vertex(corner0_id, 0)) == basic.NO_ID, "an unidentified vertex has no unique vertex")
    builder.set_unique_vertex(vertex(corner0_id, 0), 0)
    builder.set_unique_vertex(vertex(line_id, 0), 0)
    builder.set_unique_vertex(vertex(line_id, 1), 1)
    builder.set_unique_vertex(vertex(corner1_id, 0), 2)
    builder.set_unique_vertex(vertex(line_id, 2), 2)
    check(len(brep.component_mesh_vertices(0)) == 2, "two component vertices share the unique vertex")
    builder.set_unique_vertex(vertex(line_id, 1), 0)
    check(len(brep.component_mesh_vertices(0)) == 3 and brep.component_mesh_vertices(1) == [], "set_unique_vertex detaches the vertex from its previous unique vertex")
    check_raises(lambda: brep.component_mesh_vertices(3), IndexError, "component_mesh_vertices checks the unique vertex index")
    check_raises(lambda: builder.register_corner3D_component(brep.corner(corner0)), basic.OpenGeodeDataError, "add_corner already registered the corner")
    old2new = builder.delete_isolated_vertices()
    check(old2new == [0, basic.NO_ID, 1] and brep.nb_unique_vertices() == 2, "delete_isolated_vertices returns old2new with NO_ID for the deleted ones")
    check(builder.delete_isolated_vertices() == [0, 1], "without isolated vertex the mapping is the identity")
    builder.update_unique_vertices(line_id, [basic.NO_ID, 0, 1])
    check([v.vertex for v in brep.component_mesh_vertices(0) if v.component_id == line_id] == [0] and [(v.component_id == line_id, v.vertex) for v in brep.component_mesh_vertices(1)] == [(False, 0), (True, 1)], "update_unique_vertices renumbers")
    builder.remove_line(brep.line(line))
    check(brep.nb_unique_vertices() == 2, "remove_line keeps the unique vertices")
    builder.unregister_corner3D_component(brep.corner(corner0))
    check(all(v.component_id != corner0_id for u in range(2) for v in brep.component_mesh_vertices(u)), "unregister removes the component vertices from the unique vertices")
    check(brep.nb_unique_vertices() == 2, "unregister keeps the unique vertices")
    check_raises(lambda: builder.unregister_corner3D_component(brep.corner(corner0)), basic.OpenGeodeDataError, "unregistering twice raises")
    builder.register_corner3D_component(brep.corner(corner0))


def test_model_physical_properties():
    brep = model.BRep()
    builder = model.PhysicalPropertiesBuilder(brep)
    porosity, density = model.PHYSICAL_PROPERTY_NAME.porosity, model.PHYSICAL_PROPERTY_NAME.density
    check(not brep.has_physical_property(porosity), "no property by default")
    check_raises(lambda: brep.physical_property_info(porosity), basic.OpenGeodeDataError, "info of an unset property raises")
    first, second = basic.uuid(), basic.uuid()
    builder.set_physical_property(porosity, model.ComponentType("Block"), first)
    builder.set_physical_property(porosity, model.ComponentType("Surface"), second)
    info = brep.physical_property_info(porosity)
    check(info.attribute_id == second and info.component_type.get() == "Surface", "set_physical_property replaces the previous association")
    other = model.BRep()
    other_builder = model.PhysicalPropertiesBuilder(other)
    other_builder.set_physical_property(density, model.ComponentType("Block"), first)
    other_builder.copy_physical_properties(brep)
    check(other.has_physical_property(porosity) and not other.has_physical_property(density), "copy_physical_properties replaces all the properties")


def test_model_measures_and_bounding_box():
    brep = model.BRep()
    builder = model.BRepBuilder(brep)
    block = brep.block(builder.add_block())
    check_raises(brep.bounding_box, basic.OpenGeodeDataError, "bounding_box of a BRep without vertex raises")
    check(model.block_volume(brep, block) == 0, "a block without mesh nor boundary has no volume")
    empty = model.block_bounding_box(brep, block)
    check(empty.min().value(0) > empty.max().value(0), "the bounding box of an empty block is an empty box")
    box = box_brep()
    check(box.blocks()[0].mesh().nb_polyhedra() == 0, "the box block is not meshed")
    check(abs(model.block_volume(box, box.blocks()[0]) - 24) < 1e-9, "the volume of an unmeshed block comes from its boundary surfaces")
    bounds = model.block_bounding_box(box, box.blocks()[0])
    check((bounds.max().value(0), bounds.max().value(1), bounds.max().value(2)) == (2, 3, 4), "the box of an unmeshed block comes from its boundary surfaces")
    check(box.bounding_box().max().value(2) == 4, "BRep.bounding_box covers the components meshes")
    check(sum(1 for s in box.surfaces() if s.mesh().nb_polygons() == 2) == 6, "the box surfaces have two triangles")
    check(sum(1 for line in box.lines() if line.mesh().nb_edges() == 1) == 12, "the box lines have one edge")
    check(box.nb_unique_vertices() == 8 and box.nb_model_boundaries() == 0, "the box has 8 unique vertices")
    check(isinstance(box.surfaces()[0].triangulated_mesh(), mesh.TriangulatedSurface3D), "the box surfaces are triangulated")
    check(isinstance(box.blocks()[0].tetrahedral_mesh(), mesh.TetrahedralSolid3D), "the box block is a tetrahedral solid")
    surface = box.surfaces()[0]
    check(abs(model.surface_area3D(surface) - sum(surface.mesh().polygon_area(p) for p in range(2))) < 1e-9, "surface_area3D sums the polygon areas")
    check(box.native_extension() == "og_brep" and model.Section().native_extension() == "og_sctn", "native extensions")


def test_model_conversions():
    box = box_brep()
    section, mapping = model.convert_brep_into_section(box, 2)
    check((section.nb_corners(), section.nb_lines(), section.nb_surfaces(), section.nb_unique_vertices()) == (8, 12, 6, 8), "convert_brep_into_section keeps corners, lines, surfaces and unique vertices")
    bounds = section.bounding_box()
    check((bounds.max().value(0), bounds.max().value(1)) == (2, 3), "the removed axis is z")
    check(mapping.at(model.Corner2D.component_type_static()).has_mapping_input(box.corners()[0].id()), "the mapping goes from the BRep uuids")
    back, _ = model.convert_section_into_brep(section, 2, 7.0)
    check(back.nb_blocks() == 0 and back.bounding_box().min().value(2) == 7 and back.bounding_box().max().value(2) == 7, "blocks are not converted and the added axis is constant")
    options = model.SectionExtruderOptions()
    check(options.axis_to_extrude == 255 and options.min_coordinate == 0 and options.max_coordinate == 0, "default extrusion options")
    check_raises(lambda: model.extrude_section_to_brep(section, options), basic.OpenGeodeDataError, "an extrusion without axis raises")
    options.axis_to_extrude, options.min_coordinate, options.max_coordinate = 2, 1.0, 5.0
    extruded = model.extrude_section_to_brep(section, options)
    check(extruded.nb_blocks() == 6 and extruded.bounding_box().min().value(2) == 1 and extruded.bounding_box().max().value(2) == 5, "extrude_section_to_brep sweeps between the two coordinates")
    # triangulation keeps the type, conversion changes it
    meshed = opengeode_load("test_mesh3.og_brep")
    model.triangulate_brep_surface_meshes(meshed)
    check_raises(lambda: meshed.surfaces()[0].triangulated_mesh(), RuntimeError, "triangulate_surface_meshes keeps the mesh type")
    model.convert_brep_surface_meshes_into_triangulated_surfaces(meshed)
    check(isinstance(meshed.surfaces()[0].triangulated_mesh(), mesh.TriangulatedSurface3D), "the conversion gives TriangulatedSurface")
    check(meshed.nb_unique_vertices() == 1317, "the conversion keeps the unique vertices")
    model.convert_brep_block_meshes_into_tetrahedral_solids(meshed)
    check(isinstance(meshed.blocks()[0].tetrahedral_mesh(), mesh.TetrahedralSolid3D), "the blocks are tetrahedral")
    # conversions to meshes
    curve, curve_mappings = model.convert_brep_into_curve(meshed)
    check(curve.nb_edges() == curve_mappings.line_edges_mapping.size() and curve.nb_vertices() == curve_mappings.unique_vertices_mapping.size(), "the curve mappings cover the edges and the unique vertices")
    surface, surface_mappings = model.convert_brep_into_surface(meshed)
    check(surface.nb_polygons() == surface_mappings.surface_polygons_mapping.size() and surface_mappings.solid_polyhedra_mapping.size() == 0, "the surface conversion maps no polyhedron")
    solid, solid_mappings = model.convert_brep_into_solid(meshed)
    check(solid.nb_polyhedra() == solid_mappings.solid_polyhedra_mapping.size() == 5709, "the solid conversion maps the polyhedra")
    check_raises(lambda: model.convert_brep_into_solid(box), basic.OpenGeodeDataError, "convert_brep_into_solid needs the unique vertices in blocks")
    check(model.convert_section_into_curve(section)[0].nb_edges() == 12 and model.convert_section_into_surface(section)[0].nb_polygons() == 12, "section conversions")
    # an extra corner on no line is not mapped in the curve conversion
    builder = model.BRepBuilder(box)
    extra = builder.add_corner()
    builder.corner_mesh_builder(box.corner(extra)).create_point(point3(9, 9, 9))
    _, extra_mappings = model.convert_brep_into_curve(box)
    check(extra_mappings.corner_vertices_mapping.size() == 8, "corners not on any line are not mapped")


def opengeode_load(name):
    import opengeode

    filename = os.path.join(DATA_DIR, name)
    return opengeode.load_one(model.load_brep(filename), filename)


def test_model_filter_and_crs_and_tracing():
    dangling = opengeode_load("dangling.og_brep")
    before = (dangling.nb_corners(), dangling.nb_lines(), dangling.nb_surfaces(), dangling.nb_blocks(), dangling.nb_unique_vertices())
    removed = model.filter_brep_components_with_regards_to_blocks(dangling)
    kinds = sorted(c.type.get() for c in removed)
    check(before == (13, 17, 7, 1, 13) and kinds == ["Corner"] * 5 + ["Line"] * 5 + ["Surface"], "the filter returns the removed components")
    check((dangling.nb_corners(), dangling.nb_lines(), dangling.nb_surfaces(), dangling.nb_blocks(), dangling.nb_unique_vertices()) == (8, 12, 6, 1, 8), "the filter removes dangling surfaces, lines, corners and isolated unique vertices")
    check(model.brep_coordinate_reference_systems(model.BRep()) == [] or len(model.brep_coordinate_reference_systems(model.BRep())) == 0, "no crs for a model without vertex")
    check(len(model.brep_active_coordinate_reference_systems(model.BRep())) == 0, "no active crs for a model without vertex")
    box = box_brep()
    check([n for _, n in model.brep_coordinate_reference_systems(box)] == ["points"], "crs shared by the components")
    builder = model.BRepBuilder(box)
    shifted = geometry.CoordinateSystem2D([geometry.Vector2D([1, 0]), geometry.Vector2D([0, 1])], point2(1, 1))
    model.create_brep_coordinate_system(box, builder, "shifted", geometry.CoordinateSystem2D(), shifted)
    check([n for _, n in model.brep_active_coordinate_reference_systems(box)] == ["shifted"], "the created crs is active on all the components")
    check(sorted(n for _, n in model.brep_coordinate_reference_systems(box)) == ["points", "shifted"], "the created crs is shared by all the components")
    model.set_brep_active_coordinate_system(box, builder, "points")
    check([n for _, n in model.brep_active_coordinate_reference_systems(box)] == ["points"], "set_brep_active_coordinate_system")
    check_raises(lambda: model.set_brep_active_coordinate_system(box, builder, "unknown"), basic.OpenGeodeError, "an unknown crs raises")
    # ray tracing
    tracing = model.BRepRayTracing(box)
    block = box.blocks()[0]
    inside = tracing.is_point_inside_block(point3(1, 1, 1), block)
    border = tracing.is_point_inside_block(point3(0, 1, 1), block)
    outside = tracing.is_point_inside_block(point3(5, 1, 1), block)
    check(inside and inside.position == model.POSITION.inside, "a point inside")
    check(not border and border.position == model.POSITION.on_border, "a point on the border is False")
    check(not outside and outside.position == model.POSITION.outside, "a point outside")
    check(tracing.block_containing_point(point3(1, 1, 1)) == block.id(), "block_containing_point")
    check(tracing.block_containing_point(point3(0, 1, 1)) is None and tracing.block_containing_point(point3(9, 9, 9)) is None, "no block for a border or outside point")
    check(model.RayTracingResult().position == model.POSITION.outside, "default result")


def test_model_polygon_helpers():
    brep = opengeode_load("test_mesh3.og_brep")
    block = brep.blocks()[0]
    surface = brep.boundary_surfaces(block)[0]
    block_mesh, surface_mesh = block.mesh(), surface.mesh()
    for polygon in range(0, surface_mesh.nb_polygons(), 50):
        unique = list(model.surface_polygon_unique_vertices(brep, surface, polygon))
        check(unique == [brep.unique_vertex(model.ComponentMeshVertex(surface.component_id(), surface_mesh.polygon_vertex(mesh.PolygonVertex(polygon, v)))) for v in range(3)], "surface_polygon_unique_vertices")
        facets = model.block_vertices_from_surface_polygon(brep, block, surface, polygon)
        oriented = model.oriented_block_vertices_from_surface_polygon(brep, block, surface, polygon)
        check(len(facets) == oriented.nb_facets() == len(model.block_mesh_polyhedra_from_surface_polygon(brep, block, surface, polygon)), "the helpers find the same facets")
        for facet in facets:
            for v in range(3):
                check(block_mesh.point(facet.vertices[v]).inexact_equal(surface_mesh.point(surface_mesh.polygon_vertex(mesh.PolygonVertex(polygon, v)))), "the block vertices follow the polygon order")
            check(list(model.block_polygon_unique_vertices(brep, block, facet.facet)) is not None, "block_polygon_unique_vertices")
        if facets:
            check((oriented.oriented_polyhedron_facet is None) != (oriented.opposite_polyhedron_facet is None), "one facet is oriented or opposite")
    line = brep.lines()[0]
    surface = brep.incident_surfaces(line)[0]
    edge = 0
    results = model.brep_surface_vertices_from_line_edge(brep, surface, line, edge)
    check(len(results) >= 1, "a surface incident to the line has an edge coincident with a line edge")
    line_mesh, surface_mesh = line.mesh(), surface.mesh()
    for result in results:
        for v in range(2):
            check(surface_mesh.point(result.vertices[v]).inexact_equal(line_mesh.point(line_mesh.edge_vertex(mesh.EdgeVertex(edge, v)))), "the surface vertices follow the line edge order")
    ordered = model.brep_oriented_surface_vertices_from_line_edge(brep, surface, line, edge)
    check(ordered.nb_edges() == len(results) and ((ordered.oriented_edge is None) != (ordered.opposite_edge is None) or ordered.nb_edges() == 2), "oriented and opposite edges")
    if ordered.oriented_edge is not None:
        e = ordered.oriented_edge
        check(surface_mesh.polygon_edge_vertex(e.facet, 0) == e.vertices[0], "the oriented edge goes the same way as the line edge")
    if ordered.opposite_edge is not None:
        e = ordered.opposite_edge
        check(surface_mesh.polygon_edge_vertex(e.facet, 0) == e.vertices[1], "the opposite edge goes the other way")


def test_model_time_series_and_io():
    points = mesh.PointSet3D.create()
    series = model.ExternalTimeSeries()
    series.set_unique_vertices(points)
    check_raises(points.nb_vertices, ValueError, "set_unique_vertices takes the point set over")
    check_raises(lambda: series.set_unique_vertices(None), basic.OpenGeodeDataError, "a null point set raises")
    check_raises(lambda: series.set_cell_barycenters(None), basic.OpenGeodeDataError, "a null barycenter set raises")
    check(model.ExternalTimeSeries.native_extension_static() == "og_ets", "time series extension")
    box = box_brep()
    with tempfile.TemporaryDirectory() as directory:
        filename = os.path.join(directory, "box.og_brep")
        check([os.path.basename(f) for f in model.save_brep(box, filename)] == ["box.og_brep"], "save_brep returns the written files")
        check(model.is_brep_saveable(box, filename) and not model.is_brep_saveable(box, filename + ".unknown"), "is_brep_saveable depends on the extension")
        check(str(model.is_brep_loadable(filename)) == "100%" and str(model.is_brep_loadable(filename + ".unknown")) == "0%", "is_brep_loadable is a Percentage")
        check(model.brep_object_priority(filename) == 0, "brep_object_priority")
        check(len(model.load_brep(filename)) == 1, "load_brep returns a list")
        check_raises(lambda: model.load_brep(filename + ".unknown"), basic.OpenGeodeDataError, "loading an unknown format raises")
        check_raises(lambda: model.brep_object_priority(filename + ".unknown"), basic.OpenGeodeDataError, "the priority without loader raises")
        check_raises(lambda: model.brep_additional_files(filename + ".unknown"), basic.OpenGeodeDataError, "the additional files without loader raise")
        check_raises(lambda: model.save_brep(box, os.path.join(directory, "box.unknown")), basic.OpenGeodeError, "saving an unknown format raises")
        check(str(model.is_brep_time_series_loadable(filename)) == "0%", "a brep file is not a time series file")
        check_raises(lambda: model.brep_time_series_additional_files(filename), basic.OpenGeodeDataError, "time series additional files of a wrong file raise")
        check_raises(lambda: model.load_external_time_series(os.path.join(directory, "x.txt")), basic.OpenGeodeDataError, "wrong time series extension")
        check_raises(lambda: model.save_external_time_series(series, os.path.join(directory, "x.txt")), basic.OpenGeodeDataError, "wrong time series extension when saving")
        section, _ = model.convert_brep_into_section(box, 2)
        section_file = os.path.join(directory, "box.og_sctn")
        check(len(model.save_section(section, section_file)) == 1 and len(model.load_section(section_file)) == 1, "section io")
        check(model.is_section_saveable(section, section_file) and str(model.is_section_loadable(section_file)) == "100%", "section io checks")


def test_model_section_topology():
    section, _ = model.convert_brep_into_section(box_brep(), 2)
    surface = section.surfaces()[0]
    line = section.boundary_lines(surface)[0]
    check(len(section.boundary_corners(line)) == 2 and not section.is_line_closed(line), "section boundaries")
    check(len(section.incident_surfaces(line)) >= 1 and section.is_surface_boundary(line, surface), "section incidences")
    builder = model.SectionBuilder(section)
    boundary = builder.add_model_boundary()
    builder.add_line_in_model_boundary(line, section.model_boundary(boundary))
    check(names(section.model_boundary_items(section.model_boundary(boundary))) == [line.id().string()] and section.is_model_boundary_item(line, section.model_boundary(boundary)), "model boundary items are lines in a Section")
    check(isinstance(section.section_component(boundary), model.ModelBoundary2D), "section_component")
    check(section.bounding_box().max().value(1) == 3, "section bounding box")
    check_raises(model.Section().bounding_box, basic.OpenGeodeDataError, "bounding_box of an empty Section raises")


def test_model():
    model.OpenGeodeModelLibrary.initialize()
    test_model_relationships()
    test_model_builders()
    test_model_vertex_identifier()
    test_model_physical_properties()
    test_model_measures_and_bounding_box()
    test_model_conversions()
    test_model_filter_and_crs_and_tracing()
    test_model_polygon_helpers()
    test_model_time_series_and_io()
    test_model_section_topology()


if __name__ == "__main__":
    mesh.OpenGeodeMeshLibrary.initialize()
    test_builders_return_mappings()
    test_polygon_local_indexation()
    test_create_and_clone()
    test_crs_manager_claims()
    test_grid_claims()
    test_texture_claims()
    test_helper_claims()
    test_polyhedra_around_edge_hint()
    test_adjacency_dependent_queries()
    test_graph_builder_results()
    test_edges_and_facets_builders()
    test_grid_directions()
    test_gradient_without_value()
    test_names_claims()
    test_io_claims()
    test_model()
