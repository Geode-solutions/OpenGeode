# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2023 Geode-solutions
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies and substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import os
import sys
import platform
if sys.version_info >= (3, 8, 0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if x]:
        os.add_dll_directory(path)

import opengeode_py_basic as basic
import opengeode_py_model as model


def find_uuid_in_list(uuids, uuid):
    for cur_uuid in uuids:
        if cur_uuid.string() != uuid.string():
            return True
    return False


def add_corners(brep, builder):
    nb = 6
    uuids = []
    for i in range(nb):
        uuids.append(builder.add_corner())
        builder.set_corner_name(uuids[-1], "corner" + str(len(uuids)))

    temp_corner = brep.corner(builder.add_corner())
    builder.remove_corner(temp_corner)
    if brep.nb_corners() != nb:
        raise ValueError("[Test] BRep should have 6 corners")
    if brep.corner(uuids[3]).name() != "corner4":
        raise ValueError("[Test] Wrong Corner name")
    return uuids


def add_lines(brep, builder):
    nb = 9
    uuids = []
    for i in range(nb):
        uuids.append(builder.add_line())
        builder.set_line_name(uuids[-1], "line" + str(len(uuids)))

    temp_line = brep.line(builder.add_line())
    builder.remove_line(temp_line)
    if brep.nb_lines() != nb:
        raise ValueError("[Test] BRep should have 9 lines")
    if brep.line(uuids[3]).name() != "line4":
        raise ValueError("[Test] Wrong Line name")
    return uuids


def add_surfaces(brep, builder):
    nb = 5
    uuids = []
    for i in range(nb):
        uuids.append(builder.add_surface())
        builder.set_surface_name(uuids[-1], "surface" + str(len(uuids)))

    temp_surface = brep.surface(builder.add_surface())
    builder.remove_surface(temp_surface)
    if brep.nb_surfaces() != nb:
        raise ValueError("[Test] BRep should have 5 surfaces")
    if brep.surface(uuids[1]).name() != "surface2":
        raise ValueError("[Test] Wrong Surface name")
    return uuids


def add_blocks(brep, builder):
    nb = 1
    uuids = []
    for i in range(nb):
        uuids.append(builder.add_block())
        builder.set_block_name(uuids[-1], "block" + str(len(uuids)))

    temp_block = brep.block(builder.add_block())
    builder.remove_block(temp_block)
    if brep.nb_blocks() != nb:
        raise ValueError("[Test] BRep should have 1 blocks")
    if brep.block(uuids[0]).name() != "block1":
        raise ValueError("[Test] Wrong Block name")
    return uuids


def add_model_boundaries(brep, builder):
    nb = 3
    uuids = []
    for i in range(nb):
        uuids.append(builder.add_model_boundary())
        builder.set_model_boundary_name(
            uuids[-1], "boundary" + str(len(uuids)))

    temp_boundary = brep.model_boundary(builder.add_model_boundary())
    builder.remove_model_boundary(temp_boundary)
    if brep.nb_model_boundaries() != nb:
        raise ValueError("[Test]BRep should have 3 model boundaries")
    if brep.model_boundary(uuids[0]).name() != "boundary1":
        raise ValueError("[Test] Wrong ModelBoundary name")
    return uuids


def add_corner_line_boundary_relation(brep, builder, corner_uuids, line_uuids):
    builder.add_corner_line_boundary_relationship(
        brep.corner(corner_uuids[0]), brep.line(line_uuids[0]))
    builder.add_corner_line_boundary_relationship(
        brep.corner(corner_uuids[0]), brep.line(line_uuids[5]))
    builder.add_corner_line_boundary_relationship(
        brep.corner(corner_uuids[0]), brep.line(line_uuids[2]))
    builder.add_corner_line_boundary_relationship(
        brep.corner(corner_uuids[1]), brep.line(line_uuids[0]))
    builder.add_corner_line_boundary_relationship(
        brep.corner(corner_uuids[1]), brep.line(line_uuids[1]))
    builder.add_corner_line_boundary_relationship(
        brep.corner(corner_uuids[1]), brep.line(line_uuids[3]))
    builder.add_corner_line_boundary_relationship(
        brep.corner(corner_uuids[2]), brep.line(line_uuids[1]))
    builder.add_corner_line_boundary_relationship(
        brep.corner(corner_uuids[2]), brep.line(line_uuids[2]))
    builder.add_corner_line_boundary_relationship(
        brep.corner(corner_uuids[2]), brep.line(line_uuids[4]))
    builder.add_corner_line_boundary_relationship(
        brep.corner(corner_uuids[3]), brep.line(line_uuids[5]))
    builder.add_corner_line_boundary_relationship(
        brep.corner(corner_uuids[3]), brep.line(line_uuids[6]))
    builder.add_corner_line_boundary_relationship(
        brep.corner(corner_uuids[3]), brep.line(line_uuids[8]))
    builder.add_corner_line_boundary_relationship(
        brep.corner(corner_uuids[4]), brep.line(line_uuids[3]))
    builder.add_corner_line_boundary_relationship(
        brep.corner(corner_uuids[4]), brep.line(line_uuids[6]))
    builder.add_corner_line_boundary_relationship(
        brep.corner(corner_uuids[4]), brep.line(line_uuids[7]))
    builder.add_corner_line_boundary_relationship(
        brep.corner(corner_uuids[5]), brep.line(line_uuids[4]))
    builder.add_corner_line_boundary_relationship(
        brep.corner(corner_uuids[5]), brep.line(line_uuids[7]))
    builder.add_corner_line_boundary_relationship(
        brep.corner(corner_uuids[5]), brep.line(line_uuids[8]))

    for corner_id in corner_uuids:
        for incidence in brep.incident_lines(brep.corner(corner_id)):
            if not find_uuid_in_list(line_uuids, incidence.id()):
                raise ValueError(
                    "[Test] All Corners incidences should be Lines")
        if brep.nb_incidences(corner_id) != 3:
            raise ValueError(
                "[Test] All Corners should be connected to 3 Lines")

    for line_id in line_uuids:
        for boundary in brep.boundary_corners(brep.line(line_id)):
            if not find_uuid_in_list(corner_uuids, boundary.id()):
                raise ValueError(
                    "[Test] All Lines incidences should be Corners")
        if brep.nb_boundaries(line_id) != 2:
            raise ValueError(
                "[Test] All Lines should be connected to 2 Corners")


def add_line_surface_boundary_relation(brep, builder, line_uuids, surface_uuids):
    builder.add_line_surface_boundary_relationship(
        brep.line(line_uuids[0]), brep.surface(surface_uuids[0]))
    builder.add_line_surface_boundary_relationship(
        brep.line(line_uuids[0]), brep.surface(surface_uuids[1]))
    builder.add_line_surface_boundary_relationship(
        brep.line(line_uuids[1]), brep.surface(surface_uuids[0]))
    builder.add_line_surface_boundary_relationship(
        brep.line(line_uuids[1]), brep.surface(surface_uuids[2]))
    builder.add_line_surface_boundary_relationship(
        brep.line(line_uuids[2]), brep.surface(surface_uuids[0]))
    builder.add_line_surface_boundary_relationship(
        brep.line(line_uuids[2]), brep.surface(surface_uuids[3]))
    builder.add_line_surface_boundary_relationship(
        brep.line(line_uuids[3]), brep.surface(surface_uuids[1]))
    builder.add_line_surface_boundary_relationship(
        brep.line(line_uuids[3]), brep.surface(surface_uuids[2]))
    builder.add_line_surface_boundary_relationship(
        brep.line(line_uuids[4]), brep.surface(surface_uuids[2]))
    builder.add_line_surface_boundary_relationship(
        brep.line(line_uuids[4]), brep.surface(surface_uuids[3]))
    builder.add_line_surface_boundary_relationship(
        brep.line(line_uuids[5]), brep.surface(surface_uuids[1]))
    builder.add_line_surface_boundary_relationship(
        brep.line(line_uuids[5]), brep.surface(surface_uuids[3]))
    builder.add_line_surface_boundary_relationship(
        brep.line(line_uuids[6]), brep.surface(surface_uuids[1]))
    builder.add_line_surface_boundary_relationship(
        brep.line(line_uuids[6]), brep.surface(surface_uuids[4]))
    builder.add_line_surface_boundary_relationship(
        brep.line(line_uuids[7]), brep.surface(surface_uuids[2]))
    builder.add_line_surface_boundary_relationship(
        brep.line(line_uuids[7]), brep.surface(surface_uuids[4]))
    builder.add_line_surface_boundary_relationship(
        brep.line(line_uuids[8]), brep.surface(surface_uuids[3]))
    builder.add_line_surface_boundary_relationship(
        brep.line(line_uuids[8]), brep.surface(surface_uuids[4]))

    for line_id in line_uuids:
        for incidence in brep.incident_surfaces(brep.line(line_id)):
            if not find_uuid_in_list(surface_uuids, incidence.id()):
                raise ValueError(
                    "[Test] All Lines incidences should be Surfaces")
        if brep.nb_incidences(line_id) != 2:
            raise ValueError(
                "[Test] All Lines should be connected to 2 Surfaces")


def add_surface_block_relation(brep, builder, surface_uuids, block_uuids):
    for surface_id in range(3):
        builder.add_surface_block_boundary_relationship(
            brep.surface(surface_uuids[surface_id]), brep.block(block_uuids[0]))

    for surface_id in range(3):
        for incidence in brep.incident_blocks(brep.surface(surface_uuids[surface_id])):
            if block_uuids[0].string() != incidence.id().string():
                raise ValueError(
                    "[Test] All Surfaces incidences should be Blocks")
        if brep.nb_incidences(surface_uuids[surface_id]) != 1:
            raise ValueError(
                "[Test] All Surfaces should be boundary to the first Block")

    if brep.nb_boundaries(block_uuids[0]) != 3:
        raise ValueError(
            "[Test] The Block should be connected to all Surfaces")


def add_surfaces_in_model_boundaries(brep, builder, surface_uuids, boundary_uuids):
    builder.add_surface_in_model_boundary(brep.surface(
        surface_uuids[0]), brep.model_boundary(boundary_uuids[0]))
    for i in range(1, 4):
        builder.add_surface_in_model_boundary(brep.surface(
            surface_uuids[i]), brep.model_boundary(boundary_uuids[1]))
    builder.add_surface_in_model_boundary(brep.surface(
        surface_uuids[4]), brep.model_boundary(boundary_uuids[2]))

    for surface_id in surface_uuids:
        if brep.nb_collections(surface_id) != 1:
            raise ValueError("[Test] All Surfaces should be in 1 collection")
        for collection in brep.collections(surface_id):
            if not collection.type().matches(model.ModelBoundary3D.component_type_static()):
                raise ValueError(
                    "[Test] This surface should be in 1 collection of type Boundary")


def add_internal_corner_relations(brep, builder, corner_uuids, surface_uuids, block_uuids):
    for corner_id in corner_uuids:
        builder.add_corner_surface_internal_relationship(
            brep.corner(corner_id), brep.surface(surface_uuids[0]))
        builder.add_corner_block_internal_relationship(
            brep.corner(corner_id), brep.block(block_uuids[0]))

    for corner_id in corner_uuids:
        for embedding in brep.embedding_surfaces_of_corner(brep.corner(corner_id)):
            if surface_uuids[0].string() != embedding.id().string():
                raise ValueError(
                    "[Test] All Corners embedded surfaces should be Surfaces")
            if brep.nb_internal_corners_of_surface(embedding) != len(corner_uuids):
                raise ValueError("[Test] Surface should embed all Lines")
        for embedding in brep.embedding_blocks_of_corner(brep.corner(corner_id)):
            if block_uuids[0].string() != embedding.id().string():
                raise ValueError(
                    "[Test] All Corners embedded blocks should be Blocks")
            if brep.nb_internal_corners_of_block(embedding) != len(corner_uuids):
                raise ValueError("[Test] Block should embed all Corners")
        if brep.nb_embeddings(corner_id) != 2:
            raise ValueError(
                "[Test] All Corners should be embedded to 1 Block and 1 Surface")
        if brep.nb_embedding_surfaces_of_corner(brep.corner(corner_id)) != 1:
            raise ValueError(
                "[Test] All Corners should be embedded to 1 Surface")
        if brep.nb_embedding_blocks_of_corner(brep.corner(corner_id)) != 1:
            raise ValueError(
                "[Test] All Corners should be embedded to 1 Block")


def add_internal_line_relations(brep, builder, line_uuids, surface_uuids, block_uuids):
    nb_boundaries = 0
    for line_id in line_uuids:
        if brep.is_surface_boundary(brep.line(line_id),brep.surface(surface_uuids[0])):
            nb_boundaries+=1
        else:
            builder.add_line_surface_internal_relationship(
                brep.line(line_id), brep.surface(surface_uuids[0]))
        builder.add_line_block_internal_relationship(
            brep.line(line_id), brep.block(block_uuids[0]))

    for line_id in line_uuids:
        for embedding in brep.embedding_surfaces_of_line(brep.line(line_id)):
            if surface_uuids[0].string() != embedding.id().string():
                raise ValueError("[Test] All Line embedded surfaces should be Surfaces")
            if brep.nb_internal_lines_of_surface(embedding) != len(line_uuids)-nb_boundaries:
                raise ValueError("[Test] Surface should embed all Lines that are not its boundaries")
        for embedding in brep.embedding_blocks_of_line(brep.line(line_id)):
            if block_uuids[0].string() != embedding.id().string():
                raise ValueError("[Test] All Lines embedded blocks should be Blocks")
            if brep.nb_internal_lines_of_block(embedding) != len(line_uuids):
                raise ValueError("[Test] Block should embed all Lines")
        if brep.is_surface_boundary(brep.line(line_id),brep.surface(surface_uuids[0])):
            if brep.nb_embeddings(line_id) != 1:
                raise ValueError("[Test] Line should be embedded to 1 Block")
            if brep.nb_embedding_surfaces_of_line(brep.line(line_id)) != 0:
                raise ValueError("[Test] Line should be embedded to 0 Surface")
        else:
            if brep.nb_embeddings(line_id) != 2:
                raise ValueError(
                    "[Test] Line should be embedded to 1 Block and 1 Surface")
            if brep.nb_embedding_surfaces_of_line(brep.line(line_id)) != 1:
                raise ValueError(
                    "[Test] Line should be embedded to 1 Surface")
        if brep.nb_embedding_blocks_of_line(brep.line(line_id)) != 1:
            raise ValueError("[Test] All Surfaces should be embedded to 1 Block")


def add_internal_surface_relations(brep, builder, surface_uuids, block_uuids):
    for surface_id in range(3, len(surface_uuids)):
        builder.add_surface_block_internal_relationship(
            brep.surface(surface_uuids[surface_id]), brep.block(block_uuids[0]))

    for surface_id in range(3, len(surface_uuids)):
        for embedding in brep.embedding_blocks_of_surface(brep.surface(surface_uuids[surface_id])):
            if brep.nb_internal_surfaces_of_block(embedding) != len(surface_uuids)-3:
                raise ValueError("[Test] Block should embed 2 Surfaces")
            if block_uuids[0].string() != embedding.id().string():
                raise ValueError("[Test] Surfaces embeddings should be Blocks")
        if brep.nb_embeddings(surface_uuids[surface_id]) != 1:
            raise ValueError("[Test] Surfaces should be embedded to 1 Block")
        if brep.nb_embedding_blocks_of_surface(brep.surface(surface_uuids[surface_id])) != 1:
            raise ValueError("[Test] Surfaces should be embedded to 1 Block")


def test_boundary_ranges(brep, corner_uuids, line_uuids, surface_uuids, block_uuids):
    line_boundary_count = 0
    for line_boundary in brep.boundary_corners(brep.line(line_uuids[0])):
        line_boundary_count += 1
        if line_boundary.id().string() != corner_uuids[0].string() and line_boundary.id().string() != corner_uuids[1].string():
            raise ValueError("[Test] BoundaryCornerRange iteration result is not correct")
        if not brep.is_line_boundary(line_boundary, brep.line(line_uuids[0])):
            raise ValueError("[Test] Corner should be boundary of Line")
    if line_boundary_count != 2:
        raise ValueError("[Test] BoundaryCornerRange should iterates on 2 Corners")

    surface_boundary_count = 0
    for surface_boundary in brep.boundary_lines(brep.surface(surface_uuids[0])):
        surface_boundary_count += 1
        if surface_boundary.id().string() != line_uuids[0].string() and surface_boundary.id().string() != line_uuids[1].string() and surface_boundary.id().string() != line_uuids[2].string():
            raise ValueError(
                "[Test] BoundaryLineRange iteration result is not correct")
        if not brep.is_surface_boundary(surface_boundary, brep.surface(surface_uuids[0])):
            raise ValueError("[Test] Line should be boundary of Surface")
    if surface_boundary_count != 3:
        raise ValueError("[Test] BoundaryLineRange should iterates on 3 Lines")

    block_boundary_count = 0
    for block_boundary in brep.boundary_surfaces(brep.block(block_uuids[0])):
        block_boundary_count += 1
        if block_boundary.id().string() != surface_uuids[0].string() and block_boundary.id().string() != surface_uuids[1].string() and block_boundary.id().string() != surface_uuids[2].string():
            raise ValueError(
                "[Test] BoundarySurfaceRange iteration result is not correct")
        if not brep.is_block_boundary(block_boundary, brep.block(block_uuids[0])):
            raise ValueError("[Test] Surface should be boundary of Block")
    if block_boundary_count != 3:
        raise ValueError(
            "[Test] BoundarySurfaceRange should iterates on 3 Surfaces")


def test_incidence_ranges(brep, corner_uuids, line_uuids, surface_uuids, block_uuids):
    corner_incidence_count = 0
    for corner_incidence in brep.incident_lines(brep.corner(corner_uuids[0])):
        corner_incidence_count += 1
        if corner_incidence.id().string() != line_uuids[0].string() and corner_incidence.id().string() != line_uuids[2].string() and corner_incidence.id().string() != line_uuids[5].string():
            raise ValueError(
                "[Test] IncidentLineRange iteration result is not correct")
    if corner_incidence_count != 3:
        raise ValueError("[Test] IncidentLineRange should iterates on 3 Lines")

    line_incidence_count = 0
    for line_incidence in brep.incident_surfaces(brep.line(line_uuids[0])):
        line_incidence_count += 1
        if line_incidence.id().string() != surface_uuids[0].string() and line_incidence.id().string() != surface_uuids[1].string():
            raise ValueError(
                "[Test] IncidentSurfaceRange iteration result is not correct")
    if line_incidence_count != 2:
        raise ValueError(
            "[Test] IncidentSurfaceRange should iterates on 2 Surfaces")

    surface_incidence_count = 0
    for surface_incidence in brep.incident_blocks(brep.surface(surface_uuids[0])):
        surface_incidence_count += 1
        if surface_incidence.id().string() != block_uuids[0].string():
            raise ValueError(
                "[Test] IncidentBlockRange iteration result is not correct")
    if surface_incidence_count != 1:
        raise ValueError(
            "[Test] IncidentBlockRange should iterates on 1 Block")


def test_item_ranges(brep, surface_uuids, boundary_uuids):
    boundary_item_count = 0
    for boundary_item in brep.model_boundary_items(brep.model_boundary(boundary_uuids[1])):
        boundary_item_count += 1
        if boundary_item.id().string() != surface_uuids[1].string() and boundary_item.id().string() != surface_uuids[2].string() and boundary_item.id().string() != surface_uuids[3].string():
            raise ValueError(
                "[Test] ItemSurfaceRange iteration result is not correct")
        if not brep.is_model_boundary_item(boundary_item, brep.model_boundary(boundary_uuids[1])):
            raise ValueError("[Test] Surface should be item of ModelBoundary")
    if boundary_item_count != 3:
        raise ValueError("[Test] IncidentLineRange should iterates "
                         "on 3 Surfaces (Boundary 1)")


def test_reloaded_brep(brep):
    if brep.nb_corners() != 6:
        raise ValueError(
            "[Test] Number of Corners in reloaded BRep should be 6")
    if brep.nb_lines() != 9:
        raise ValueError("[Test] Number of Lines in reloaded BRep should be 9")
    if brep.nb_surfaces() != 5:
        raise ValueError(
            "[Test] Number of Surfaces in reloaded BRep should be 5")
    if brep.nb_blocks() != 1:
        raise ValueError(
            "[Test] Number of Blocks in reloaded BRep should be 1")
    if brep.nb_model_boundaries() != 3:
        raise ValueError(
            "[Test] Number of Boundaries in reloaded BRep should be 3")


def test_clone(brep):
    brep2 = model.BRep()
    builder = model.BRepBuilder(brep2)
    builder.copy(brep)
    if brep2.nb_corners() != 6:
        raise ValueError("[Test] BRep should have 6 corners")
    if brep2.nb_lines() != 9:
        raise ValueError("[Test] BRep should have 9 lines")
    if brep2.nb_surfaces() != 5:
        raise ValueError("[Test] BRep should have 5 surfaces")
    if brep2.nb_blocks() != 1:
        raise ValueError("[Test] BRep should have 1 block")
    if brep2.nb_model_boundaries() != 3:
        raise ValueError("[Test] BRep should have 3 model boundaries")


if __name__ == '__main__':
    model.OpenGeodeModelLibrary.initialize()
    brep = model.BRep()
    builder = model.BRepBuilder(brep)

    # This BRep represents a prism
    corner_uuids = add_corners(brep, builder)
    line_uuids = add_lines(brep, builder)
    surface_uuids = add_surfaces(brep, builder)
    block_uuids = add_blocks(brep, builder)
    model_boundary_uuids = add_model_boundaries(brep, builder)

    add_corner_line_boundary_relation(brep, builder, corner_uuids, line_uuids)
    add_line_surface_boundary_relation(
        brep, builder, line_uuids, surface_uuids)
    add_surface_block_relation(brep, builder, surface_uuids, block_uuids)
    add_surfaces_in_model_boundaries(
        brep, builder, surface_uuids, model_boundary_uuids)
    add_internal_corner_relations(
        brep, builder, corner_uuids, surface_uuids, block_uuids)
    add_internal_line_relations(
        brep, builder, line_uuids, surface_uuids, block_uuids)
    add_internal_surface_relations(brep, builder, surface_uuids, block_uuids)
    if brep.nb_internals(block_uuids[0]) != len(corner_uuids) + len(line_uuids) + len(surface_uuids)-3:
        raise ValueError(
            "[Test] The Block should embed all Corners & Lines & Surfaces (that are internal to the Block)")
    test_boundary_ranges(brep, corner_uuids, line_uuids,
                         surface_uuids, block_uuids)
    test_incidence_ranges(brep, corner_uuids, line_uuids,
                          surface_uuids, block_uuids)
    test_item_ranges(brep, surface_uuids, model_boundary_uuids)
    test_clone(brep)

    file_io = "test." + brep.native_extension()
    model.save_brep(brep, file_io)

    brep2 = model.load_brep(file_io)
    test_reloaded_brep(brep2)
