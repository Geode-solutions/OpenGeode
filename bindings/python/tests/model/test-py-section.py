# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2024 Geode-solutions
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
        if cur_uuid.string() == uuid.string():
            return True
    return False


def add_corners(section, builder):
    nb = 5
    uuids = []
    for i in range(nb):
        uuids.append(builder.add_corner())

    temp_corner = section.corner(builder.add_corner())
    builder.remove_corner(temp_corner)
    if section.nb_corners() != nb:
        raise ValueError("[Test] Section should have 5 corners")
    return uuids


def add_lines(section, builder):
    nb = 6
    uuids = []
    for i in range(nb):
        uuids.append(builder.add_line())

    temp_line = section.line(builder.add_line())
    builder.remove_line(temp_line)
    if section.nb_lines() != nb:
        raise ValueError("[Test] Section should have 6 lines")
    return uuids


def add_surfaces(section, builder):
    nb = 2
    uuids = []
    for i in range(nb):
        uuids.append(builder.add_surface())

    temp_surface = section.surface(builder.add_surface())
    builder.remove_surface(temp_surface)
    if section.nb_surfaces() != nb:
        raise ValueError("[Test] Section should have 2 surfaces")
    return uuids


def add_model_boundaries(section, builder):
    nb = 2
    uuids = []
    for i in range(nb):
        uuids.append(builder.add_model_boundary())
        builder.set_model_boundary_name(
            uuids[-1], "boundary" + str(len(uuids)))

    temp_boundary = section.model_boundary(builder.add_model_boundary())
    builder.remove_model_boundary(temp_boundary)
    if section.nb_model_boundaries() != nb:
        raise ValueError("[Test] Section should have 2 model boundaries")
    if section.model_boundary(uuids[0]).name() != "boundary1":
        raise ValueError("[Test] Wrong ModelBoundary name")
    return uuids

def add_corner_collections(section, builder):
    nb = 2
    uuids = []
    for i in range(nb):
        uuids.append(builder.add_corner_collection())
        builder.set_corner_collection_name(
            uuids[-1], "collection" + str(len(uuids)))

    temp_collection = section.corner_collection(builder.add_corner_collection())
    builder.remove_corner_collection(temp_collection)
    if section.nb_corner_collections() != nb:
        raise ValueError("[Test] Section should have 2 corner collections")
    if section.corner_collection(uuids[0]).name() != "collection1":
        raise ValueError("[Test] Wrong CornerCollection name")
    return uuids

def add_line_collections(section, builder):
    nb = 2
    uuids = []
    for i in range(nb):
        uuids.append(builder.add_line_collection())
        builder.set_line_collection_name(
            uuids[-1], "collection" + str(len(uuids)))

    temp_collection = section.line_collection(builder.add_line_collection())
    builder.remove_line_collection(temp_collection)
    if section.nb_line_collections() != nb:
        raise ValueError("[Test] Section should have 2 line collections")
    if section.line_collection(uuids[0]).name() != "collection1":
        raise ValueError("[Test] Wrong LineCollection name")
    return uuids

def add_surface_collections(section, builder):
    nb = 2
    uuids = []
    for i in range(nb):
        uuids.append(builder.add_surface_collection())
        builder.set_surface_collection_name(
            uuids[-1], "collection" + str(len(uuids)))

    temp_collection = section.surface_collection(builder.add_surface_collection())
    builder.remove_surface_collection(temp_collection)
    if section.nb_surface_collections() != nb:
        raise ValueError("[Test] Section should have 2 surface collections")
    if section.surface_collection(uuids[0]).name() != "collection1":
        raise ValueError("[Test] Wrong SurfaceCollection name")
    return uuids


def add_corner_line_boundary_relation(section, builder, corner_uuids, line_uuids):
    builder.add_corner_line_boundary_relationship(
        section.corner(corner_uuids[0]), section.line(line_uuids[0]))
    builder.add_corner_line_boundary_relationship(
        section.corner(corner_uuids[0]), section.line(line_uuids[1]))
    builder.add_corner_line_boundary_relationship(
        section.corner(corner_uuids[1]), section.line(line_uuids[0]))
    builder.add_corner_line_boundary_relationship(
        section.corner(corner_uuids[1]), section.line(line_uuids[2]))
    builder.add_corner_line_boundary_relationship(
        section.corner(corner_uuids[1]), section.line(line_uuids[3]))
    builder.add_corner_line_boundary_relationship(
        section.corner(corner_uuids[2]), section.line(line_uuids[1]))
    builder.add_corner_line_boundary_relationship(
        section.corner(corner_uuids[2]), section.line(line_uuids[2]))
    builder.add_corner_line_boundary_relationship(
        section.corner(corner_uuids[2]), section.line(line_uuids[4]))
    builder.add_corner_line_boundary_relationship(
        section.corner(corner_uuids[3]), section.line(line_uuids[3]))
    builder.add_corner_line_boundary_relationship(
        section.corner(corner_uuids[3]), section.line(line_uuids[5]))
    builder.add_corner_line_boundary_relationship(
        section.corner(corner_uuids[4]), section.line(line_uuids[4]))
    builder.add_corner_line_boundary_relationship(
        section.corner(corner_uuids[4]), section.line(line_uuids[5]))

    for corner_id in corner_uuids:
        for incidence in section.incident_lines(section.corner(corner_id)):
            if not find_uuid_in_list(line_uuids, incidence.id()):
                raise ValueError(
                    "[Test] All Corners incidences should be Lines")

    for line_id in line_uuids:
        for boundary in section.boundary_corners(section.line(line_id)):
            if not find_uuid_in_list(corner_uuids, boundary.id()):
                raise ValueError(
                    "[Test] All Lines boundaries should be Corners")


def add_line_surface_boundary_relation(section, builder, line_uuids, surface_uuids):
    builder.add_line_surface_boundary_relationship(
        section.line(line_uuids[0]), section.surface(surface_uuids[0]))
    builder.add_line_surface_boundary_relationship(
        section.line(line_uuids[1]), section.surface(surface_uuids[0]))
    builder.add_line_surface_boundary_relationship(
        section.line(line_uuids[2]), section.surface(surface_uuids[0]))
    builder.add_line_surface_boundary_relationship(
        section.line(line_uuids[2]), section.surface(surface_uuids[1]))
    builder.add_line_surface_boundary_relationship(
        section.line(line_uuids[3]), section.surface(surface_uuids[1]))
    builder.add_line_surface_boundary_relationship(
        section.line(line_uuids[4]), section.surface(surface_uuids[1]))
    builder.add_line_surface_boundary_relationship(
        section.line(line_uuids[5]), section.surface(surface_uuids[1]))

    for line_id in line_uuids:
        for incidence in section.incident_surfaces(section.line(line_id)):
            if surface_uuids[0].string() != incidence.id().string() and surface_uuids[1].string() != incidence.id().string():
                raise ValueError(
                    "[Test] All Lines incidences should be Surfaces")
    if section.nb_boundaries(surface_uuids[0]) != 3:
        raise ValueError("[Test] Surface 0 should have 3 Lines as boundaries")
    if section.nb_boundaries(surface_uuids[1]) != 4:
        raise ValueError("[Test] Surface 1 should have 4 Lines as boundaries")


def add_lines_in_model_boundaries(section, builder, line_uuids, boundary_uuids):
    builder.add_line_in_model_boundary(section.line(
        line_uuids[0]), section.model_boundary(boundary_uuids[0]))
    for i in range(1, 3):
        builder.add_line_in_model_boundary(section.line(
            line_uuids[i]), section.model_boundary(boundary_uuids[1]))

    for i in range(3):
        if section.nb_collections(line_uuids[i]) != 1:
            raise ValueError("[Test] This Line should be in 1 collection")
        for collection in section.collections(line_uuids[i]):
            if not collection.type().matches(model.ModelBoundary2D.component_type_static()):
                raise ValueError(
                    "[Test] This Line should be in 1 collection of type Boundary")
    if section.nb_collections(line_uuids[4]) != 0:
        raise ValueError(
            "[Test] Last Line should be in no collection (of type Boundary)")

def add_corners_in_corner_collections(section, builder, corner_uuids, collection_uuids):
    for i in range(2):
        builder.add_corner_in_corner_collection(section.corner(
            corner_uuids[i]), section.corner_collection(collection_uuids[0]))
    for i in range(2,len(corner_uuids)):
        builder.add_corner_in_corner_collection(section.corner(
            corner_uuids[i]), section.corner_collection(collection_uuids[1]))

    for corner_id in corner_uuids:
        if section.nb_collections(corner_id) != 1:
            raise ValueError("[Test] All Corners should be in 1 collection")
        for collection in section.collections(corner_id):
            if not collection.type().matches(model.CornerCollection2D.component_type_static()):
                raise ValueError(
                    "[Test] This corner should be in 1 collection of type CornerCollection")

def add_lines_in_line_collections(section, builder, line_uuids, collection_uuids):
    for i in range(2):
        builder.add_line_in_line_collection(section.line(
            line_uuids[i]), section.line_collection(collection_uuids[0]))
    for i in range(2,len(line_uuids)):
        builder.add_line_in_line_collection(section.line(
            line_uuids[i]), section.line_collection(collection_uuids[1]))

    for i in range(3):
        if section.nb_collections(line_uuids[i]) != 2:
            raise ValueError("[Test] This Line should be in 2 collections")
        for collection in section.collections(line_uuids[i]):
            if not collection.type().matches(model.LineCollection2D.component_type_static()) and not collection.type().matches(model.ModelBoundary2D.component_type_static()):
                raise ValueError(
                    "[Test] This line should be in 2 collection of type LineCollection and ModelBoundary")
    for i in range(3,5):
        if section.nb_collections(line_uuids[i]) != 1:
            raise ValueError(
                "[Test] Last Lines should be in 1 collection (of type LineCollection)")
        for collection in section.collections(line_uuids[i]):
            if not collection.type().matches(model.LineCollection2D.component_type_static()):
                raise ValueError(
                    "[Test] This line should be in 1 collection of type LineCollection")

def add_surfaces_in_surface_collections(section, builder, surface_uuids, collection_uuids):
    for i in range(2):
        builder.add_surface_in_surface_collection(section.surface(
            surface_uuids[i]), section.surface_collection(collection_uuids[i]))

    for surface_id in surface_uuids:
        if section.nb_collections(surface_id) != 1:
            raise ValueError("[Test] All Surfaces should be in 1 collections")
        for collection in section.collections(surface_id):
            if not collection.type().matches(model.SurfaceCollection2D.component_type_static()):
                raise ValueError(
                    "[Test] This surface should be in 2 collections of type SurfaceCollection")


def add_internal_corner_relations(section, builder, corner_uuids, surface_uuids):
    for corner_id in corner_uuids:
        builder.add_corner_surface_internal_relationship(
            section.corner(corner_id), section.surface(surface_uuids[0]))

    for corner_id in corner_uuids:
        for embedding in section.embedding_surfaces_of_corner(section.corner(corner_id)):
            if surface_uuids[0].string() != embedding.id().string():
                raise ValueError(
                    "[Test] All Corners embeddings should be Surfaces")
            if section.nb_internal_corners_of_surface(embedding) != len(corner_uuids):
                raise ValueError("[Test] Surface should embed all Corners")
        if section.nb_embeddings(corner_id) != 1:
            raise ValueError(
                "[Test] All Corners should be embedded to 1 Surface")
        if section.nb_embedding_surfaces_of_corner(section.corner(corner_id)) != 1:
            raise ValueError(
                "[Test] All Corners should be embedded to 1 Surface")


def add_internal_line_relations(section, builder, line_uuids, surface_uuids):
    builder.add_line_surface_internal_relationship(section.line(line_uuids[3]), section.surface(surface_uuids[0]))
    builder.add_line_surface_internal_relationship(section.line(line_uuids[4]), section.surface(surface_uuids[0]))
    builder.add_line_surface_internal_relationship(section.line(line_uuids[5]), section.surface(surface_uuids[0]))
    builder.add_line_surface_internal_relationship(section.line(line_uuids[0]), section.surface(surface_uuids[1]))
    builder.add_line_surface_internal_relationship(section.line(line_uuids[1]), section.surface(surface_uuids[1]))

    for line_id in range(0,1):
        for embedding in section.embedding_surfaces_of_line(section.line(line_uuids[line_id])):
            if embedding.id().string() != surface_uuids[1].string():
                raise ValueError("[Test] Lines embeddings are wrong")
        if section.nb_embeddings(line_uuids[line_id]) != 1:
            raise ValueError("[Test] All Lines should be embedded to 1 Surface")
        if section.nb_embedding_surfaces_of_line(section.line(line_uuids[line_id])) != 1:
            raise ValueError("[Test] All Lines should be embedded to 1 Surface")
    if section.nb_internal_lines_of_surface(section.surface(surface_uuids[0])) != 3:
        raise ValueError("[Test] Surface 0 should embed 3 Lines")
    if section.nb_internal_lines_of_surface(section.surface(surface_uuids[1])) != 2:
        raise ValueError("[Test] Surface 1 should embed 2 Lines")


def test_boundary_ranges(section, corner_uuids, line_uuids, surface_uuids):
    line_boundary_count = 0
    for line_boundary in section.boundary_corners(section.line(line_uuids[0])):
        line_boundary_count += 1
        if line_boundary.id().string() != corner_uuids[0].string() and line_boundary.id().string() != corner_uuids[1].string():
            raise ValueError(
                "[Test] BoundaryCornerRange iteration result is not correct")
        if not section.is_line_boundary(line_boundary, section.line(line_uuids[0])):
            raise ValueError("[Test] Corner should be boundary of Line")
    if line_boundary_count != 2:
        raise ValueError(
            "[Test] BoundaryCornerRange should iterates on 2 Corners")

    surface_boundary_count = 0
    for surface_boundary in section.boundary_lines(section.surface(surface_uuids[0])):
        surface_boundary_count += 1
        if surface_boundary.id().string() != line_uuids[0].string() and surface_boundary.id().string() != line_uuids[1].string() and surface_boundary.id().string() != line_uuids[2].string():
            raise ValueError(
                "[Test] BoundaryLineRange iteration result is not correct")
        if not section.is_surface_boundary(surface_boundary, section.surface(surface_uuids[0])):
            raise ValueError("[Test] Line should be boundary of Surface")
    if surface_boundary_count != 3:
        raise ValueError("[Test] BoundaryLineRange should iterates on 3 Lines")


def test_incidence_ranges(section, corner_uuids, line_uuids, surface_uuids):
    corner_incidence_count = 0
    for corner_incidence in section.incident_lines(section.corner(corner_uuids[0])):
        corner_incidence_count += 1
        if corner_incidence.id().string() != line_uuids[0].string() and corner_incidence.id().string() != line_uuids[1].string():
            raise ValueError(
                "[Test] IncidentLineRange iteration result is not correct")
    if corner_incidence_count != 2:
        raise ValueError("[Test] IncidentLineRange should iterates on 2 Lines")

    line_incidence_count = 0
    for line_incidence in section.incident_surfaces(section.line(line_uuids[0])):
        line_incidence_count += 1
        if line_incidence.id().string() != surface_uuids[0].string():
            raise ValueError(
                "[Test] IncidentSurfaceRange iteration result is not correct")
    if line_incidence_count != 1:
        raise ValueError(
            "[Test] IncidentSurfaceRange should iterates on 1 Surface")


def test_item_ranges(section, line_uuids, boundary_uuids):
    boundary_item_count = 0
    for boundary_item in section.model_boundary_items(section.model_boundary(boundary_uuids[1])):
        boundary_item_count += 1
        if boundary_item.id().string() != line_uuids[1].string() and boundary_item.id().string() != line_uuids[2].string():
            raise ValueError(
                "[Test] ItemLineRange iteration result is not correct")
        if not section.is_model_boundary_item(boundary_item, section.model_boundary(boundary_uuids[1])):
            raise ValueError("[Test] Surface should be item of ModelBoundary")
    if boundary_item_count != 2:
        raise ValueError(
            "[Test] IncidentLineRange should iterates on 2 Lines (Boundary 1)")

def test_corner_item_ranges(section, corner_uuids, collection_uuids):
    collection_item_count = 0
    for collection_item in section.corner_collection_items(section.corner_collection(collection_uuids[1])):
        collection_item_count += 1
        if collection_item.id().string() != corner_uuids[2].string() and collection_item.id().string() != corner_uuids[3].string() and collection_item.id().string() != corner_uuids[4].string():
            raise ValueError(
                "[Test] ItemCornerRange iteration result is not correct")
        if not section.is_corner_collection_item(collection_item, section.corner_collection(collection_uuids[1])):
            raise ValueError("[Test] Corner should be item of CornerCollection")
    if collection_item_count != 3:
        raise ValueError("[Test] ItemCornerRange should iterate on 3 Corners (Collection 1)")

def test_line_item_ranges(section, line_uuids, collection_uuids):
    collection_item_count = 0
    for collection_item in section.line_collection_items(section.line_collection(collection_uuids[1])):
        collection_item_count += 1
        if collection_item.id().string() != line_uuids[2].string() and collection_item.id().string() != line_uuids[3].string() and collection_item.id().string() != line_uuids[4].string() and collection_item.id().string() != line_uuids[5].string():
            raise ValueError(
                "[Test] ItemLineRange iteration result is not correct")
        if not section.is_line_collection_item(collection_item, section.line_collection(collection_uuids[1])):
            raise ValueError("[Test] Line should be item of LineCollection")
    if collection_item_count != 4:
        raise ValueError("[Test] ItemLineRange should iterate on 4 Lines (Collection 1)")

def test_surface_item_ranges(section, surface_uuids, collection_uuids):
    collection_item_count = 0
    for collection_item in section.surface_collection_items(section.surface_collection(collection_uuids[1])):
        collection_item_count += 1
        if collection_item.id().string() != surface_uuids[1].string():
            raise ValueError(
                "[Test] ItemSurfaceRange iteration result is not correct")
        if not section.is_surface_collection_item(collection_item, section.surface_collection(collection_uuids[1])):
            raise ValueError("[Test] Surface should be item of SurfaceCollection")
    if collection_item_count != 1:
        raise ValueError("[Test] ItemSurfaceRange should iterate on 1 Surfaces (Collection 1)")


def test_clone(section):
    section2 = model.Section()
    builder = model.SectionBuilder(section2)
    builder.copy(section)
    if section2.nb_corners() != 5:
        raise ValueError("[Test] Section should have 5 corners")
    if section2.nb_lines() != 6:
        raise ValueError("[Test] Section should have 6 lines")
    if section2.nb_surfaces() != 2:
        raise ValueError("[Test] Section should have 2 surfaces")
    if section2.nb_model_boundaries() != 2:
        raise ValueError("[Test] Section should have 2 model boundaries")
    if section2.nb_corner_collections() != 2:
        raise ValueError("[Test] Section should have 2 corner collections")
    if section2.nb_line_collections() != 2:
        raise ValueError("[Test] Section should have 2 line collections")
    if section2.nb_surface_collections() != 2:
        raise ValueError("[Test] Section should have 2 surface collections")


if __name__ == '__main__':
    model.OpenGeodeModelLibrary.initialize()
    section = model.Section()
    builder = model.SectionBuilder(section)

    # This Section represents a house (with one triangle and one square as in children sketches)
    corner_uuids = add_corners(section, builder)
    line_uuids = add_lines(section, builder)
    surface_uuids = add_surfaces(section, builder)
    model_boundary_uuids = add_model_boundaries(section, builder)
    corner_collection_uuids = add_corner_collections(section, builder)
    line_collection_uuids = add_line_collections(section, builder)
    surface_collection_uuids = add_surface_collections(section, builder)

    add_corner_line_boundary_relation(
        section, builder, corner_uuids, line_uuids)
    add_line_surface_boundary_relation(
        section, builder, line_uuids, surface_uuids)
    add_lines_in_model_boundaries(
        section, builder, line_uuids, model_boundary_uuids)
    add_corners_in_corner_collections(
        section, builder, corner_uuids, corner_collection_uuids)
    add_lines_in_line_collections(
        section, builder, line_uuids, line_collection_uuids)
    add_surfaces_in_surface_collections(
        section, builder, surface_uuids, surface_collection_uuids)
    add_internal_corner_relations(
        section, builder, corner_uuids, surface_uuids)
    add_internal_line_relations(section, builder, line_uuids, surface_uuids)
    if section.nb_internals(surface_uuids[0]) != len(corner_uuids) + 3:
        raise ValueError(
            "[Test] The Surface should embed all Corners & Lines (that are internal to the Surface)")
    test_boundary_ranges(section, corner_uuids, line_uuids, surface_uuids)
    test_incidence_ranges(section, corner_uuids, line_uuids, surface_uuids)
    test_item_ranges(section, line_uuids, model_boundary_uuids)
    test_corner_item_ranges(section, corner_uuids, corner_collection_uuids)
    test_line_item_ranges(section, line_uuids, line_collection_uuids)
    test_surface_item_ranges(section, surface_uuids, surface_collection_uuids)
    test_clone(section)

    file_io = "test." + section.native_extension()
    model.save_section(section, file_io)

    section2 = model.load_section(file_io)
