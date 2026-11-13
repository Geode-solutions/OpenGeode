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
