# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2020 Geode-solutions
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

import opengeode_py_basic as basic
import opengeode_py_geometry as geometry
import opengeode_py_mesh as mesh
import opengeode_py_model as model

def _create_corner(brep, builder, point):
    corner_id = builder.add_corner()
    corner = brep.corner(corner_id)
    builder.corner_mesh_builder(corner_id).create_point(point)
    model_vertex_id = builder.create_unique_vertex()
    builder.set_unique_vertex(model.MeshComponentVertex(corner.component_id(), 0), model_vertex_id)
    return corner_id

def _add_point_from_corner(brep, builder, mesh, mesh_builder, corner_id):
    corner = brep.corner(corner_id)
    point = corner.mesh().point(0)
    vertex_id = brep.unique_vertex(model.MeshComponentVertex(corner.component_id(), 0))
    new_vertex = mesh_builder.create_point(point)
    builder.set_unique_vertex(model.MeshComponentVertex(mesh.component_id(), new_vertex), vertex_id)

def _create_line(brep, builder, corner_id0, corner_id1):
    line_id = builder.add_line()
    line = brep.line(line_id)
    line_builder = builder.line_mesh_builder(line_id)
    _add_point_from_corner(brep, builder, line, line_builder, corner_id0)
    _add_point_from_corner(brep, builder, line, line_builder, corner_id1)
    line_builder.create_edge_with_vertices(0,1)
    builder.add_corner_line_boundary_relationship(brep.corner(corner_id0), line)
    builder.add_corner_line_boundary_relationship(brep.corner(corner_id1), line)
    return line_id

def _create_surface(brep, builder, corner_ids, line_ids):
    surface_id = builder.add_surface()
    surface = brep.surface(surface_id)
    surface_builder = builder.surface_mesh_builder(surface_id)
    for corner_id in corner_ids:
        _add_point_from_corner(brep, builder, surface, surface_builder, corner_id)
    surface_builder.create_polygon([0,1,2,3])
    for line_id in line_ids:
        builder.add_line_surface_boundary_relationship(brep.line(line_id), surface)
    return surface_id

def _create_block(brep, builder, surface_ids):
    block_id = builder.add_block()
    block = brep.block(block_id)
    for surface_id in surface_ids:
        builder.add_surface_block_boundary_relationship(brep.surface(surface_id), block)
    return block_id

def _corner_mapping(brep0, brep1):
    corner_mapping = basic.BijectiveMappinguuid()
    for corner0 in brep0.corners():
        for corner1 in brep1.corners():
            if corner0.mesh().point(0).inexact_equal( corner1.mesh().point(0), 1e-8 ):
                corner_mapping.map(corner1.id(), corner0.id())
                break
    return corner_mapping

def _line_mapping(brep0, brep1, corner_mapping):
    line_mapping = basic.BijectiveMappinguuid()
    for line0 in brep0.lines():
        line_corners0 = brep0.boundary_corners(line0)
        if not corner_mapping.has_mapping_output(line_corners0[0].id()) or not corner_mapping.has_mapping_output(line_corners0[1].id()):
            continue
        for line1 in brep1.lines():
            found = True
            for corner1 in brep1.boundary_corners(line1):
                if corner_mapping.out2in(line_corners0[0].id()).string() != corner1.id().string() and corner_mapping.out2in(line_corners0[1].id()).string() != corner1.id().string():
                    found = False
                    break
            if found:
                line_mapping.map(line1.id(), line0.id())
                break
    return line_mapping

def _surface_mapping(brep0, brep1, line_mapping):
    surface_mapping = basic.BijectiveMappinguuid()
    for surface0 in brep0.surfaces():
        surface_lines0 = brep0.boundary_lines(surface0)
        found = True
        for surface_line0 in surface_lines0:
            if not line_mapping.has_mapping_output(surface_line0.id()):
                found = False
                break
        if not found:
            continue
        for surface1 in brep1.surfaces():
            done = True
            for line1 in brep1.boundary_lines(surface1):
                found = False
                for surface_line0 in surface_lines0:
                    if line_mapping.out2in(surface_line0.id()).string() == line1.id().string():
                        found = True
                        break
                if not found:
                    done = False
                    break
            if done:
                surface_mapping.map(surface1.id(), surface0.id())
                break
    return surface_mapping

def _brep_mapping(brep0, brep1):
    corner_mapping = _corner_mapping(brep0, brep1)
    line_mapping = _line_mapping(brep0, brep1, corner_mapping)
    surface_mapping = _surface_mapping(brep0, brep1, line_mapping)
    return corner_mapping, line_mapping, surface_mapping

def _corner_from_surface_vertex(brep, surface, vertex):
    vertex_id = brep.unique_vertex(model.MeshComponentVertex(surface.component_id(), vertex))
    unique_vertices = brep.filtered_mesh_component_vertices_by_type(vertex_id, model.Corner3D.component_type_static())
    return unique_vertices[0].component_id.id()

def create_cube(x_min, y_min, z_min, x_max, y_max, z_max):
    cube = model.BRep()
    builder = model.BRepBuilder(cube)
    point0 = geometry.Point3D([x_min, y_min, z_min])
    point1 = geometry.Point3D([x_max, y_min, z_min])
    point2 = geometry.Point3D([x_min, y_max, z_min])
    point3 = geometry.Point3D([x_max, y_max, z_min])
    point4 = geometry.Point3D([x_min, y_min, z_max])
    point5 = geometry.Point3D([x_max, y_min, z_max])
    point6 = geometry.Point3D([x_min, y_max, z_max])
    point7 = geometry.Point3D([x_max, y_max, z_max])
    corner0 = _create_corner(cube, builder, point0)
    corner1 = _create_corner(cube, builder, point1)
    corner2 = _create_corner(cube, builder, point2)
    corner3 = _create_corner(cube, builder, point3)
    corner4 = _create_corner(cube, builder, point4)
    corner5 = _create_corner(cube, builder, point5)
    corner6 = _create_corner(cube, builder, point6)
    corner7 = _create_corner(cube, builder, point7)
    line0 = _create_line(cube, builder, corner0, corner1)
    line1 = _create_line(cube, builder, corner1, corner3)
    line2 = _create_line(cube, builder, corner3, corner2)
    line3 = _create_line(cube, builder, corner2, corner0)
    line4 = _create_line(cube, builder, corner4, corner5)
    line5 = _create_line(cube, builder, corner5, corner7)
    line6 = _create_line(cube, builder, corner7, corner6)
    line7 = _create_line(cube, builder, corner6, corner4)
    line8 = _create_line(cube, builder, corner0, corner4)
    line9 = _create_line(cube, builder, corner1, corner5)
    line10 = _create_line(cube, builder, corner2, corner6)
    line11 = _create_line(cube, builder, corner3, corner7)
    bottom = _create_surface(cube, builder, [corner0, corner1, corner3, corner2], [line0, line1, line2, line3])
    top = _create_surface(cube, builder, [corner4, corner5, corner7, corner6], [line4, line5, line6, line7])
    front = _create_surface(cube, builder, [corner0, corner1, corner5, corner4], [line0, line9, line4, line8])
    back = _create_surface(cube, builder, [corner2, corner3, corner7, corner6], [line2, line11, line6, line10])
    left = _create_surface(cube, builder, [corner0, corner4, corner6, corner2], [line8, line7, line10, line3])
    right = _create_surface(cube, builder, [corner1, corner5, corner7, corner3], [line9, line5, line11, line1])
    block = _create_block(cube, builder, [bottom, top, front, back, left, right])
    return cube

def glue_model_surfaces(brep, brep1):
    builder = model.BRepBuilder(brep)
    corner_mapping, line_mapping, surface_mapping = _brep_mapping(brep, brep1)
    for corner in brep1.corners():
        if not corner_mapping.has_mapping_input(corner.id()):
            corner_id = _create_corner(brep, builder, corner.mesh().point(0))
            corner_mapping.map(corner.id(), corner_id)
    for line in brep1.lines():
        if not line_mapping.has_mapping_input(line.id()):
            line_corners = brep1.boundary_corners(line)
            line_id = _create_line(brep, builder, corner_mapping.in2out(line_corners[0].id()),corner_mapping.in2out(line_corners[1].id()))
            line_mapping.map(line.id(), line_id)
    for surface in brep1.surfaces():
        if not surface_mapping.has_mapping_input(surface.id()):
            mapped_corners = []
            for v in range(surface.mesh().nb_vertices()):
                corner_id = _corner_from_surface_vertex(brep1, surface, v)
                mapped_corners.append(corner_mapping.in2out(corner_id))
            mapped_lines = []
            for surface_line in brep1.boundary_lines(surface):
                mapped_lines.append(line_mapping.in2out(surface_line.id()))
            surface_id = _create_surface(brep, builder, mapped_corners, mapped_lines)
            surface_mapping.map(surface.id(), surface_id)
    block_mapping = basic.BijectiveMappinguuid()
    for block in brep1.blocks():
        mapped_surfaces = []
        for block_surface in brep1.boundary_surfaces(block):
            mapped_surfaces.append(surface_mapping.in2out(block_surface.id()))
        block_id = _create_block(brep, builder, mapped_surfaces)
        block_mapping.map(block.id(), block_id)

if __name__ == '__main__':
    result = create_cube(0,0,0, 1,1,1)
    cube1 = create_cube(0,0,1, 1,1,2)
    glue_model_surfaces(result, cube1)
    cube2 = create_cube(0,0,2, 1,1,3)
    glue_model_surfaces(result, cube2)
    model.save_brep(result, "result." + result.native_extension())

