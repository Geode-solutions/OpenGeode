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
# all copies and substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import opengeode_py_basic as basic
import opengeode_py_model as model

def find_uuid_in_list( uuids, uuid ):
    for cur_uuid in uuids:
        if cur_uuid.string() != uuid.string():
            return True
    return False

def add_corners( model, builder ):
    nb = 6
    uuids = []
    for i in range( nb ):
        uuids.append( builder.add_corner() )
        builder.set_corner_name( uuids[-1], "corner" + str( len( uuids ) ) )
        
    temp_corner = model.corner( builder.add_corner() )
    builder.remove_corner( temp_corner )
    if model.nb_corners() != nb:
        raise ValueError( "[Test] BRep should have 6 corners")
    if model.corner( uuids[3] ).name() != "corner4":
        raise ValueError( "[Test] Wrong Corner name" )
    return uuids

def add_lines( model, builder ):
    nb = 9
    uuids = []
    for i in range( nb ):
        uuids.append( builder.add_line() )
        builder.set_line_name( uuids[-1], "line" + str( len( uuids ) ) )

    temp_line = model.line( builder.add_line() )
    builder.remove_line( temp_line )
    if model.nb_lines() != nb:
        raise ValueError( "[Test] BRep should have 9 lines" )
    if model.line( uuids[3] ).name() != "line4":
        raise ValueError( "[Test] Wrong Line name" )
    return uuids

def add_surfaces( model, builder ):
    nb = 5
    uuids = []
    for i in range( nb ):
        uuids.append( builder.add_surface() )
        builder.set_surface_name( uuids[-1], "surface" + str( len( uuids ) ) )

    temp_surface = model.surface( builder.add_surface() )
    builder.remove_surface( temp_surface )
    if model.nb_surfaces() != nb:
        raise ValueError( "[Test] BRep should have 5 surfaces" )
    if model.surface( uuids[1] ).name() != "surface2":
        raise ValueError( "[Test] Wrong Surface name" )
    return uuids

def add_blocks( model, builder ):
    nb = 1
    uuids = []
    for i in range( nb ):
        uuids.append( builder.add_block() )
        builder.set_block_name( uuids[-1], "block" + str( len( uuids ) ) )

    temp_block = model.block( builder.add_block() )
    builder.remove_block( temp_block )
    if model.nb_blocks() != nb:
        raise ValueError( "[Test] BRep should have 1 blocks" )
    if model.block( uuids[0] ).name() != "block1":
        raise ValueError( "[Test] Wrong Block name" )
    return uuids

def add_model_boundaries( model, builder ):
    nb = 3
    uuids = []
    for i in range( nb ):
        uuids.append( builder.add_model_boundary() )
        builder.set_model_boundary_name( uuids[-1], "boundary" + str(len( uuids )) )

    temp_boundary = model.model_boundary( builder.add_model_boundary() )
    builder.remove_model_boundary( temp_boundary )
    if model.nb_model_boundaries() != nb:
        raise ValueError( "[Test]BRep should have 3 model boundaries" )
    if model.model_boundary( uuids[0] ).name() != "boundary1":
        raise ValueError( "[Test] Wrong ModelBoundary name" )
    return uuids

def add_corner_line_boundary_relation( model, builder, corner_uuids, line_uuids ):
    builder.add_corner_line_boundary_relationship( model.corner( corner_uuids[0] ), model.line( line_uuids[0] ) )
    builder.add_corner_line_boundary_relationship( model.corner( corner_uuids[0] ), model.line( line_uuids[5] ) )
    builder.add_corner_line_boundary_relationship( model.corner( corner_uuids[0] ), model.line( line_uuids[2] ) )
    builder.add_corner_line_boundary_relationship( model.corner( corner_uuids[1] ), model.line( line_uuids[0] ) )
    builder.add_corner_line_boundary_relationship( model.corner( corner_uuids[1] ), model.line( line_uuids[1] ) )
    builder.add_corner_line_boundary_relationship( model.corner( corner_uuids[1] ), model.line( line_uuids[3] ) )
    builder.add_corner_line_boundary_relationship( model.corner( corner_uuids[2] ), model.line( line_uuids[1] ) )
    builder.add_corner_line_boundary_relationship( model.corner( corner_uuids[2] ), model.line( line_uuids[2] ) )
    builder.add_corner_line_boundary_relationship( model.corner( corner_uuids[2] ), model.line( line_uuids[4] ) )
    builder.add_corner_line_boundary_relationship( model.corner( corner_uuids[3] ), model.line( line_uuids[5] ) )
    builder.add_corner_line_boundary_relationship( model.corner( corner_uuids[3] ), model.line( line_uuids[6] ) )
    builder.add_corner_line_boundary_relationship( model.corner( corner_uuids[3] ), model.line( line_uuids[8] ) )
    builder.add_corner_line_boundary_relationship( model.corner( corner_uuids[4] ), model.line( line_uuids[3] ) )
    builder.add_corner_line_boundary_relationship( model.corner( corner_uuids[4] ), model.line( line_uuids[6] ) )
    builder.add_corner_line_boundary_relationship( model.corner( corner_uuids[4] ), model.line( line_uuids[7] ) )
    builder.add_corner_line_boundary_relationship( model.corner( corner_uuids[5] ), model.line( line_uuids[4] ) )
    builder.add_corner_line_boundary_relationship( model.corner( corner_uuids[5] ), model.line( line_uuids[7] ) )
    builder.add_corner_line_boundary_relationship( model.corner( corner_uuids[5] ), model.line( line_uuids[8] ) )

    for corner_id in corner_uuids:
        for incidence in model.incident_lines( model.corner( corner_id ) ):
            if not find_uuid_in_list( line_uuids, incidence.id() ):
                raise ValueError( "[Test] All Corners incidences should be Lines" )
        if model.nb_incidences( corner_id ) != 3:
            raise ValueError( "[Test] All Corners should be connected to 3 Lines" )

    for line_id in line_uuids:
        for boundary in model.boundary_corners( model.line( line_id ) ):
            if not find_uuid_in_list( corner_uuids, boundary.id() ):
                raise ValueError( "[Test] All Lines incidences should be Corners" )
        if model.nb_boundaries( line_id ) != 2:
            raise ValueError( "[Test] All Lines should be connected to 2 Corners" )

def add_line_surface_boundary_relation( model, builder, line_uuids, surface_uuids ):
    builder.add_line_surface_boundary_relationship( model.line( line_uuids[0] ), model.surface( surface_uuids[0] ) )
    builder.add_line_surface_boundary_relationship( model.line( line_uuids[0] ), model.surface( surface_uuids[1] ) )
    builder.add_line_surface_boundary_relationship( model.line( line_uuids[1] ), model.surface( surface_uuids[0] ) )
    builder.add_line_surface_boundary_relationship( model.line( line_uuids[1] ), model.surface( surface_uuids[2] ) )
    builder.add_line_surface_boundary_relationship( model.line( line_uuids[2] ), model.surface( surface_uuids[0] ) )
    builder.add_line_surface_boundary_relationship( model.line( line_uuids[2] ), model.surface( surface_uuids[3] ) )
    builder.add_line_surface_boundary_relationship( model.line( line_uuids[3] ), model.surface( surface_uuids[1] ) )
    builder.add_line_surface_boundary_relationship( model.line( line_uuids[3] ), model.surface( surface_uuids[2] ) )
    builder.add_line_surface_boundary_relationship( model.line( line_uuids[4] ), model.surface( surface_uuids[2] ) )
    builder.add_line_surface_boundary_relationship( model.line( line_uuids[4] ), model.surface( surface_uuids[3] ) )
    builder.add_line_surface_boundary_relationship( model.line( line_uuids[5] ), model.surface( surface_uuids[1] ) )
    builder.add_line_surface_boundary_relationship( model.line( line_uuids[5] ), model.surface( surface_uuids[3] ) )
    builder.add_line_surface_boundary_relationship( model.line( line_uuids[6] ), model.surface( surface_uuids[1] ) )
    builder.add_line_surface_boundary_relationship( model.line( line_uuids[6] ), model.surface( surface_uuids[4] ) )
    builder.add_line_surface_boundary_relationship( model.line( line_uuids[7] ), model.surface( surface_uuids[2] ) )
    builder.add_line_surface_boundary_relationship( model.line( line_uuids[7] ), model.surface( surface_uuids[4] ) )
    builder.add_line_surface_boundary_relationship( model.line( line_uuids[8] ), model.surface( surface_uuids[3] ) )
    builder.add_line_surface_boundary_relationship( model.line( line_uuids[8] ), model.surface( surface_uuids[4] ) )

    for line_id in line_uuids:
        for incidence in model.incident_surfaces( model.line( line_id ) ):
            if not find_uuid_in_list( surface_uuids, incidence.id() ):
                raise ValueError( "[Test] All Lines incidences should be Surfaces" )
        if model.nb_incidences( line_id ) != 2:
            raise ValueError( "[Test] All Lines should be connected to 2 Surfaces" )

def add_surface_block_relation( model, builder, surface_uuids, block_uuids ):
    for surface_id in surface_uuids:
        builder.add_surface_block_boundary_relationship( model.surface( surface_id ), model.block( block_uuids[0] ) )

    for surface_id in surface_uuids:
        for incidence in model.incident_blocks( model.surface( surface_id ) ):
            if block_uuids[0].string() != incidence.id().string():
                raise ValueError( "[Test] All Surfaces incidences should be Blocks" )
        if model.nb_incidences( surface_id ) != 1:
            raise ValueError( "[Test] All Surfaces should be connected to 1 Block" )

    if model.nb_boundaries( block_uuids[0] ) != len( surface_uuids ):
        raise ValueError( "[Test] The Block should be connected to all Surfaces" )

def add_surfaces_in_model_boundaries( model, builder, surface_uuids, boundary_uuids ):
    builder.add_surface_in_model_boundary( model.surface( surface_uuids[0] ), model.model_boundary( boundary_uuids[0] ) )
    for i in range( 1, 4 ):
        builder.add_surface_in_model_boundary( model.surface( surface_uuids[i] ), model.model_boundary( boundary_uuids[1] ) )
    builder.add_surface_in_model_boundary( model.surface( surface_uuids[4] ), model.model_boundary( boundary_uuids[2] ) )

    for surface_id in surface_uuids:
        if model.nb_collections( surface_id ) != 1:
            raise ValueError( "[Test] All Surfaces should be in 1 collection (of type Boundary)" )

def add_internal_corner_relations( model, builder, corner_uuids, surface_uuids, block_uuids ):
    for corner_id in corner_uuids:
        builder.add_corner_surface_internal_relationship( model.corner( corner_id ), model.surface( surface_uuids[0] ) )
        builder.add_corner_block_internal_relationship( model.corner( corner_id ), model.block( block_uuids[0] ) )

    for corner_id in corner_uuids:
        for embedding in model.embedding_surfaces_of_corner( model.corner( corner_id ) ):
            if surface_uuids[0].string() != embedding.id().string():
                raise ValueError( "[Test] All Corners embedded surfaces should be Surfaces" )
            if model.nb_internal_corners_of_surface( embedding ) != len( corner_uuids ):
                raise ValueError( "[Test] Surface should embed all Lines" )
        for embedding in model.embedding_blocks_of_corner( model.corner( corner_id ) ):
            if block_uuids[0].string() != embedding.id().string():
                raise ValueError( "[Test] All Corners embedded blocks should be Blocks" )
            if model.nb_internal_corners_of_block( embedding ) != len( corner_uuids ):
                raise ValueError( "[Test] Block should embed all Lines" )
        if model.nb_embeddings( corner_id ) != 2:
            raise ValueError( "[Test] All Corners should be embedded to 1 Block and 1 Surface" )
        if model.nb_embedding_surfaces_of_corner( model.corner( corner_id ) ) != 1:
            raise ValueError( "[Test] All Corners should be embedded to 1 Surface" )
        if model.nb_embedding_blocks_of_corner( model.corner( corner_id ) ) != 1:
            raise ValueError( "[Test] All Corners should be embedded to 1 Block" )

def add_internal_line_relations( model, builder, line_uuids, surface_uuids, block_uuids ):
    for line_id in line_uuids:
        builder.add_line_surface_internal_relationship( model.line( line_id ), model.surface( surface_uuids[0] ) )
        builder.add_line_block_internal_relationship( model.line( line_id ), model.block( block_uuids[0] ) )

    for line_id in line_uuids:
        for embedding in model.embedding_surfaces_of_line( model.line( line_id ) ):
            if surface_uuids[0].string() != embedding.id().string():
                raise ValueError( "[Test] All Line embedded surfaces should be Surfaces" )
            if model.nb_internal_lines_of_surface( embedding ) != len( line_uuids ):
                raise ValueError( "[Test] Surface should embed all Lines" )
        for embedding in model.embedding_blocks_of_line( model.line( line_id ) ):
            if block_uuids[0].string() != embedding.id().string():
                raise ValueError( "[Test] All Lines embedded blocks should be Blocks" )
            if model.nb_internal_lines_of_block( embedding ) != len( line_uuids ):
                raise ValueError( "[Test] Block should embed all Lines" )
        if model.nb_embeddings( line_id ) != 2:
            raise ValueError( "[Test] All Surfaces should be embedded to 1 Block and 1 Surface" )
        if model.nb_embedding_surfaces_of_line( model.line( line_id ) ) != 1:
            raise ValueError( "[Test] All Surfaces should be embedded to 1 Surface" )
        if model.nb_embedding_blocks_of_line( model.line( line_id ) ) != 1:
            raise ValueError( "[Test] All Surfaces should be embedded to 1 Block" )

def add_internal_surface_relations( model, builder, surface_uuids, block_uuids ):
    for surface_id in surface_uuids:
        builder.add_surface_block_internal_relationship( model.surface( surface_id ), model.block( block_uuids[0] ) )

    for surface_id in surface_uuids:
        for embedding in model.embedding_blocks_of_surface( model.surface( surface_id ) ):
            if model.nb_internal_surfaces_of_block( embedding ) != len( surface_uuids ):
                raise ValueError( "[Test] Block should embed all Surfaces" )
            if block_uuids[0].string() != embedding.id().string():
                raise ValueError( "[Test] All Surfaces embeddings should be Blocks" )
        if model.nb_embeddings( surface_id ) != 1:
            raise ValueError( "[Test] All Surfaces should be embedded to 1 Block" )
        if model.nb_embedding_blocks_of_surface( model.surface( surface_id ) ) != 1:
            raise ValueError( "[Test] All Surfaces should be embedded to 1 Block" )

def test_boundary_ranges( model, corner_uuids, line_uuids, surface_uuids, block_uuids ):
    line_boundary_count = 0
    for line_boundary in model.boundary_corners( model.line( line_uuids[0] ) ):
        line_boundary_count += 1
        if line_boundary.id().string() != corner_uuids[0].string() and line_boundary.id().string() != corner_uuids[1].string():
            raise ValueError( "[Test] BoundaryCornerRange iteration result is not correct" )
        if not model.is_line_boundary( line_boundary, model.line( line_uuids[0] ) ):
            raise ValueError( "[Test] Corner should be boundary of Line" )
    if line_boundary_count != 2:
        raise ValueError( "[Test] BoundaryCornerRange should iterates on 2 Corners" )

    surface_boundary_count = 0
    for surface_boundary in model.boundary_lines( model.surface( surface_uuids[0] ) ):
        surface_boundary_count += 1
        if surface_boundary.id().string() != line_uuids[0].string() and surface_boundary.id().string() != line_uuids[1].string() and surface_boundary.id().string() != line_uuids[2].string():
            raise ValueError( "[Test] BoundaryLineRange iteration result is not correct" )
        if not model.is_surface_boundary( surface_boundary, model.surface( surface_uuids[0] ) ):
            raise ValueError( "[Test] Line should be boundary of Surface" )
    if surface_boundary_count != 3:
        raise ValueError( "[Test] BoundaryLineRange should iterates on 3 Lines" )

    block_boundary_count = 0
    for block_boundary in model.boundary_surfaces( model.block( block_uuids[0] ) ):
        block_boundary_count += 1
        if block_boundary.id().string() != surface_uuids[0].string() and block_boundary.id().string() != surface_uuids[1].string() and block_boundary.id().string() != surface_uuids[2].string() and block_boundary.id().string() != surface_uuids[3].string() and block_boundary.id().string() != surface_uuids[4].string():
            raise ValueError( "[Test] BoundarySurfaceRange iteration result is not correct" )
        if not model.is_block_boundary( block_boundary, model.block( block_uuids[0] ) ):
            raise ValueError( "[Test] Surface should be boundary of Block" )
    if block_boundary_count != 5:
        raise ValueError( "[Test] BoundarySurfaceRange should iterates on 5 Surfaces" )

def test_incidence_ranges( model, corner_uuids, line_uuids, surface_uuids, block_uuids ):
    corner_incidence_count = 0
    for corner_incidence in model.incident_lines( model.corner( corner_uuids[0] ) ):
        corner_incidence_count += 1
        if corner_incidence.id().string() != line_uuids[0].string() and corner_incidence.id().string() != line_uuids[2].string() and corner_incidence.id().string() != line_uuids[5].string():
            raise ValueError( "[Test] IncidentLineRange iteration result is not correct" )
    if corner_incidence_count != 3:
        raise ValueError( "[Test] IncidentLineRange should iterates on 3 Lines" )

    line_incidence_count = 0
    for line_incidence in model.incident_surfaces( model.line( line_uuids[0] ) ):
        line_incidence_count += 1
        if line_incidence.id().string() != surface_uuids[0].string() and line_incidence.id().string() != surface_uuids[1].string():
            raise ValueError( "[Test] IncidentSurfaceRange iteration result is not correct" )
    if line_incidence_count != 2:
        raise ValueError( "[Test] IncidentSurfaceRange should iterates on 2 Surfaces" )

    surface_incidence_count = 0
    for surface_incidence in model.incident_blocks( model.surface( surface_uuids[0] ) ):
        surface_incidence_count += 1
        if surface_incidence.id().string() != block_uuids[0].string():
            raise ValueError( "[Test] IncidentBlockRange iteration result is not correct" )
    if surface_incidence_count != 1:
        raise ValueError( "[Test] IncidentBlockRange should iterates on 1 Block" )

def test_item_ranges( model, surface_uuids, boundary_uuids ):
    boundary_item_count = 0
    for boundary_item in model.model_boundary_items( model.model_boundary( boundary_uuids[1] ) ):
        boundary_item_count += 1
        if boundary_item.id().string() != surface_uuids[1].string() and boundary_item.id().string() != surface_uuids[2].string() and boundary_item.id().string() != surface_uuids[3].string():
            raise ValueError( "[Test] ItemSurfaceRange iteration result is not correct" )
        if not model.is_model_boundary_item( boundary_item, model.model_boundary( boundary_uuids[1] ) ):
            raise ValueError( "[Test] Surface should be item of ModelBoundary" )
    if boundary_item_count != 3:
        raise ValueError( "[Test] IncidentLineRange should iterates "
        "on 3 Surfaces (Boundary 1)" )

def test_reloaded_brep( model ):
    if model.nb_corners() != 6:
        raise ValueError( "[Test] Number of Corners in reloaded BRep should be 6" )
    if model.nb_lines() != 9:
        raise ValueError( "[Test] Number of Lines in reloaded BRep should be 9" )
    if model.nb_surfaces() != 5:
        raise ValueError( "[Test] Number of Surfaces in reloaded BRep should be 5" )
    if model.nb_blocks() != 1:
        raise ValueError( "[Test] Number of Blocks in reloaded BRep should be 1" )
    if model.nb_model_boundaries() != 3:
        raise ValueError( "[Test] Number of Boundaries in reloaded BRep should be 3" )

def test_clone( brep ):
    brep2 = model.BRep()
    builder = model.BRepBuilder( brep2 )
    builder.copy( brep )
    if brep2.nb_corners() != 6:
        raise ValueError( "[Test] BRep should have 6 corners" )
    if brep2.nb_lines() != 9:
        raise ValueError( "[Test] BRep should have 9 lines" )
    if brep2.nb_surfaces() != 5:
        raise ValueError( "[Test] BRep should have 5 surfaces" )
    if brep2.nb_blocks() != 1:
        raise ValueError( "[Test] BRep should have 1 block" )
    if brep2.nb_model_boundaries() != 3:
        raise ValueError( "[Test] BRep should have 3 model boundaries" )

if __name__ == '__main__':
    brep = model.BRep()
    builder = model.BRepBuilder( brep )
    
    # This BRep represents a prism
    corner_uuids = add_corners( brep, builder )
    line_uuids = add_lines( brep, builder )
    surface_uuids = add_surfaces( brep, builder )
    block_uuids = add_blocks( brep, builder )
    model_boundary_uuids = add_model_boundaries( brep, builder )
    
    add_corner_line_boundary_relation( brep, builder, corner_uuids, line_uuids )
    add_line_surface_boundary_relation( brep, builder, line_uuids, surface_uuids )
    add_surface_block_relation(brep, builder, surface_uuids, block_uuids )
    add_surfaces_in_model_boundaries( brep, builder, surface_uuids, model_boundary_uuids )
    add_internal_corner_relations( brep, builder, corner_uuids, surface_uuids, block_uuids )
    add_internal_line_relations( brep, builder, line_uuids, surface_uuids, block_uuids )
    add_internal_surface_relations( brep, builder, surface_uuids, block_uuids )
    if brep.nb_internals( block_uuids[0] ) != len( corner_uuids ) + len( line_uuids ) + len( surface_uuids ):
        raise ValueError( "[Test] The Block should embed all Corners & Lines & Surfaces (that are internal to the Block)" )
    test_boundary_ranges( brep, corner_uuids, line_uuids, surface_uuids, block_uuids )
    test_incidence_ranges( brep, corner_uuids, line_uuids, surface_uuids, block_uuids )
    test_item_ranges( brep, surface_uuids, model_boundary_uuids )
    test_clone( brep )
    
    file_io = "test." + brep.native_extension()
    model.save_brep( brep, file_io )
    
    brep2 = model.BRep()
    model.load_brep( brep2, file_io )
    test_reloaded_brep( brep2 )
