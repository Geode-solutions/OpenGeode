/*
 * Copyright (c) 2019 - 2020 Geode-solutions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <absl/types/span.h>

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>
#include <geode/basic/range.h>
#include <geode/basic/uuid.h>

#include <geode/model/mixin/builder/relationships_builder.h>
#include <geode/model/mixin/core/relationships.h>

#include <geode/tests/common.h>

std::array< geode::uuid, 6 > create_uuids( geode::Relationships& relationships )
{
    std::array< geode::uuid, 6 > uuids;
    geode::RelationshipsBuilder builder{ relationships };
    for( const auto c : geode::Range{ 6 } )
    {
        const geode::uuid cur_uuid;
        builder.register_component(
            geode::ComponentID{ geode::ComponentType{ "dummy" }, cur_uuid } );
        uuids[c] = cur_uuid;
    }
    return uuids;
}

void add_boundary_relations(
    geode::Relationships& relationships, absl::Span< const geode::uuid > uuids )
{
    geode::RelationshipsBuilder builder{ relationships };
    builder.add_boundary_relation( uuids[0], uuids[1] );
    builder.add_boundary_relation( uuids[0], uuids[2] );
    builder.add_boundary_relation( uuids[0], uuids[3] );
    builder.add_boundary_relation( uuids[1], uuids[2] );
    builder.add_boundary_relation( uuids[1], uuids[3] );
    builder.add_boundary_relation( uuids[2], uuids[3] );
    builder.add_boundary_relation( uuids[4], uuids[0] );
    builder.add_boundary_relation( uuids[4], uuids[5] );
    builder.add_boundary_relation( uuids[5], uuids[0] );
    // Repete last relation to test duplications
    builder.add_boundary_relation( uuids[5], uuids[0] );
}

void add_internal_relations(
    geode::Relationships& relationships, absl::Span< const geode::uuid > uuids )
{
    geode::RelationshipsBuilder builder{ relationships };
    builder.add_internal_relation( uuids[0], uuids[1] );
    builder.add_internal_relation( uuids[2], uuids[3] );
    builder.add_internal_relation( uuids[4], uuids[5] );
    builder.add_internal_relation( uuids[4], uuids[0] );
    // Repete last relation to test duplications
    builder.add_internal_relation( uuids[4], uuids[0] );
}

void add_items_in_collections(
    geode::Relationships& relationships, absl::Span< const geode::uuid > uuids )
{
    geode::RelationshipsBuilder builder{ relationships };
    builder.add_item_in_collection( uuids[1], uuids[5] );
    builder.add_item_in_collection( uuids[2], uuids[5] );
    builder.add_item_in_collection( uuids[3], uuids[5] );
    builder.add_item_in_collection( uuids[5], uuids[4] );
    builder.add_item_in_collection( uuids[0], uuids[4] );
    // Repete last relation to test duplications
    builder.add_item_in_collection( uuids[0], uuids[4] );
}

void test_uuid( const geode::Relationships& relations,
    const geode::uuid& uuid,
    geode::index_t nb_boundaries,
    geode::index_t nb_incidences,
    geode::index_t nb_internals,
    geode::index_t nb_embeddings,
    geode::index_t nb_items,
    geode::index_t nb_collections )
{
    OPENGEODE_EXCEPTION( relations.nb_boundaries( uuid ) == nb_boundaries,
        "[Test] ", uuid.string(), " should have ", nb_boundaries,
        " boundary(ies)" );

    OPENGEODE_EXCEPTION( relations.nb_incidences( uuid ) == nb_incidences,
        "[Test] ", uuid.string(), " should have ", nb_incidences,
        " incidence(s)" );

    OPENGEODE_EXCEPTION( relations.nb_internals( uuid ) == nb_internals,
        "[Test] ", uuid.string(), " should have ", nb_internals,
        " internal component(s)" );

    OPENGEODE_EXCEPTION( relations.nb_embeddings( uuid ) == nb_embeddings,
        "[Test] ", uuid.string(), " should have ", nb_embeddings,
        " embedding(s)" );

    OPENGEODE_EXCEPTION( relations.nb_items( uuid ) == nb_items, "[Test] ",
        uuid.string(), " should have ", nb_boundaries, " item(s)" );

    OPENGEODE_EXCEPTION( relations.nb_collections( uuid ) == nb_collections,
        "[Test] ", uuid.string(), " should have ", nb_collections,
        " collection(s)" );
}

void test_relations( const geode::Relationships& relations,
    absl::Span< const geode::uuid > uuids )
{
    test_uuid( relations, uuids[0], 2, 3, 1, 1, 0, 1 );
    test_uuid( relations, uuids[1], 1, 2, 1, 0, 0, 1 );
    test_uuid( relations, uuids[2], 2, 1, 0, 1, 0, 1 );
    test_uuid( relations, uuids[3], 3, 0, 1, 0, 0, 1 );
    test_uuid( relations, uuids[4], 0, 2, 0, 2, 2, 0 );
    test_uuid( relations, uuids[5], 1, 1, 1, 0, 3, 1 );
    OPENGEODE_EXCEPTION( relations.is_boundary( uuids[0], uuids[1] ),
        "[Test] uuids[0] should be boundary of uuids[1]" );
    OPENGEODE_EXCEPTION( relations.is_internal( uuids[0], uuids[1] ),
        "[Test] uuids[0] should be internal of uuids[1]" );
    OPENGEODE_EXCEPTION( relations.is_item( uuids[0], uuids[4] ),
        "[Test] uuids[0] should be item of uuids[4]" );
}

void test()
{
    geode::Relationships relationships;
    const auto uuids = create_uuids( relationships );

    // This Relationships do not represent anything.
    add_boundary_relations( relationships, uuids );
    add_internal_relations( relationships, uuids );
    add_items_in_collections( relationships, uuids );
    test_relations( relationships, uuids );

    relationships.save_relationships( "." );
    geode::Relationships reloaded_relationships;
    geode::RelationshipsBuilder reloader{ reloaded_relationships };
    reloader.load_relationships( "." );
    test_relations( reloaded_relationships, uuids );
}

OPENGEODE_TEST( "relationships" )