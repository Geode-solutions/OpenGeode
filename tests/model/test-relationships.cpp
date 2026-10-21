/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <vector>

#include <absl/algorithm/container.h>
#include <absl/types/span.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/model/mixin/builder/relationships_builder.hpp>
#include <geode/model/mixin/core/relationships.hpp>

#include <geode/tests/common.hpp>

geode::ComponentID component_id( const geode::uuid& id )
{
    return { geode::ComponentType{ "dummy" }, id };
}

void add_boundary_relations(
    geode::Relationships& relationships, absl::Span< const geode::uuid > uuids )
{
    geode::RelationshipsBuilder builder{ relationships };
    builder.add_boundary_relation(
        component_id( uuids[1] ), component_id( uuids[0] ) );
    builder.add_boundary_relation(
        component_id( uuids[3] ), component_id( uuids[0] ) );
    builder.add_boundary_relation(
        component_id( uuids[5] ), component_id( uuids[0] ) );
    builder.add_boundary_relation(
        component_id( uuids[2] ), component_id( uuids[1] ) );
    builder.add_boundary_relation(
        component_id( uuids[2] ), component_id( uuids[3] ) );
    // Repeat last relation to test duplications
    builder.add_boundary_relation(
        component_id( uuids[2] ), component_id( uuids[3] ) );
}

void add_internal_relations(
    geode::Relationships& relationships, absl::Span< const geode::uuid > uuids )
{
    geode::RelationshipsBuilder builder{ relationships };
    builder.add_internal_relation(
        component_id( uuids[2] ), component_id( uuids[0] ) );
    builder.add_internal_relation(
        component_id( uuids[5] ), component_id( uuids[1] ) );
    // Repeat last relation to test duplications
    builder.add_internal_relation(
        component_id( uuids[5] ), component_id( uuids[1] ) );
}

void add_items_in_collections(
    geode::Relationships& relationships, absl::Span< const geode::uuid > uuids )
{
    geode::RelationshipsBuilder builder{ relationships };
    builder.add_item_in_collection(
        component_id( uuids[0] ), component_id( uuids[4] ) );
    builder.add_item_in_collection(
        component_id( uuids[1] ), component_id( uuids[4] ) );
    builder.add_item_in_collection(
        component_id( uuids[2] ), component_id( uuids[4] ) );
    builder.add_item_in_collection(
        component_id( uuids[3] ), component_id( uuids[4] ) );
    builder.add_item_in_collection(
        component_id( uuids[5] ), component_id( uuids[4] ) );
    // Repeat last relation to test duplications
    builder.add_item_in_collection(
        component_id( uuids[5] ), component_id( uuids[4] ) );
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
    geode::OpenGeodeModelException::test(
        relations.nb_boundaries( uuid ) == nb_boundaries, "", uuid.string(),
        " should have ", nb_boundaries, " boundary(ies)" );

    geode::OpenGeodeModelException::test(
        relations.nb_incidences( uuid ) == nb_incidences, "", uuid.string(),
        " should have ", nb_incidences, " incidence(s)" );

    geode::OpenGeodeModelException::test(
        relations.nb_internals( uuid ) == nb_internals, "", uuid.string(),
        " should have ", nb_internals, " internal component(s)" );

    geode::OpenGeodeModelException::test(
        relations.nb_embeddings( uuid ) == nb_embeddings, "", uuid.string(),
        " should have ", nb_embeddings, " embedding(s)" );

    geode::OpenGeodeModelException::test(
        relations.nb_items( uuid ) == nb_items, "", uuid.string(),
        " should have ", nb_boundaries, " item(s)" );

    geode::OpenGeodeModelException::test(
        relations.nb_collections( uuid ) == nb_collections, "", uuid.string(),
        " should have ", nb_collections, " collection(s)" );

    const auto total = nb_boundaries + nb_incidences + nb_internals
                       + nb_embeddings + nb_items + nb_collections;
    geode::OpenGeodeModelException::test(
        relations.nb_relations( uuid ) == total, "", uuid.string(),
        " should have ", total, " relation(s)" );
}

void test_relations( const geode::Relationships& relations,
    absl::Span< const geode::uuid > uuids )
{
    test_uuid( relations, uuids[0], 3, 0, 1, 0, 0, 1 );
    test_uuid( relations, uuids[1], 1, 1, 1, 0, 0, 1 );
    test_uuid( relations, uuids[2], 0, 2, 0, 1, 0, 1 );
    test_uuid( relations, uuids[3], 1, 1, 0, 0, 0, 1 );
    test_uuid( relations, uuids[4], 0, 0, 0, 0, 5, 0 );
    test_uuid( relations, uuids[5], 0, 1, 0, 1, 0, 1 );
    geode::OpenGeodeModelException::test(
        relations.is_boundary( uuids[1], uuids[0] ),
        "uuids[0] should be boundary of uuids[1]" );
    geode::OpenGeodeModelException::test(
        !relations.is_boundary( uuids[0], uuids[1] ),
        "uuids[0] should not be boundary of uuids[1]" );
    geode::OpenGeodeModelException::test(
        relations.is_internal( uuids[2], uuids[0] ),
        "uuids[0] should be internal of uuids[1]" );
    geode::OpenGeodeModelException::test(
        relations.is_item( uuids[0], uuids[4] ),
        "uuids[0] should be item of uuids[4]" );
}

void test_attributes( const geode::Relationships& relations,
    absl::Span< const geode::uuid > uuids )
{
    geode::OpenGeodeModelException::test(
        relations.relation_index( uuids[0], uuids[1] ) == 0,
        "Wrong relation index from uuids" );
    const auto output = relations.relation_from_index( 0 );
    geode::OpenGeodeModelException::test(
        std::get< 0 >( output ).id == uuids[1],
        "Wrong relation uuids from index" );
    geode::OpenGeodeModelException::test(
        std::get< 1 >( output ).id == uuids[0],
        "Wrong relation uuids from index" );
    geode::AttributeProperties attribute_properties;
    attribute_properties.assignable = false;
    attribute_properties.interpolable = true;
    attribute_properties.transferable = true;
    geode::AttributeValues< int > attribute_values;
    attribute_values.default_value = 0;
    attribute_values.no_value = 0;
    auto relation_att_id =
        relations.relation_attribute_manager()
            .create_attribute< geode::VariableAttribute, int >(
                "int", attribute_values, attribute_properties );
    const auto relation_att =
        relations.relation_attribute_manager()
            .find_attribute< geode::VariableAttribute, int >( relation_att_id );
    relation_att->set_value( 0, 1 );
    geode::OpenGeodeModelException::test(
        relation_att->value(
            relations.relation_index( uuids[0], uuids[1] ).value() )
            == 1,
        "Wrong relation attribute assignment" );
}

void test_ranges( const geode::Relationships& relations,
    absl::Span< const geode::uuid > uuids )
{
    const auto boundaries = relations.boundaries( uuids[0] );
    std::vector< geode::uuid > boundary_ids;
    for( const auto& boundary : boundaries )
    {
        boundary_ids.push_back( boundary.id );
    }
    absl::c_sort( boundary_ids );
    std::vector< geode::uuid > expected_boundaries{ uuids[1], uuids[3],
        uuids[5] };
    absl::c_sort( expected_boundaries );
    geode::OpenGeodeModelException::test(
        boundary_ids == expected_boundaries, "Wrong boundaries of uuids[0]" );

    geode::index_t nb_pairs{ 0 };
    for( const auto& boundary0 : boundaries )
    {
        for( const auto& boundary1 : boundaries )
        {
            geode_unused( boundary0 );
            geode_unused( boundary1 );
            nb_pairs++;
        }
    }
    geode::OpenGeodeModelException::test( nb_pairs == 9,
        "Nested iterations on the same range should be "
        "independent" );

    auto copy = boundaries;
    ++copy;
    geode::index_t nb_boundaries{ 0 };
    for( const auto& boundary : boundaries )
    {
        geode_unused( boundary );
        nb_boundaries++;
    }
    geode::OpenGeodeModelException::test( nb_boundaries == 3,
        "Advancing a range copy should not modify the original range" );
    geode::index_t nb_remaining{ 0 };
    for( ; copy != copy; ++copy )
    {
        nb_remaining++;
    }
    geode::OpenGeodeModelException::test(
        nb_remaining == 2, "Range copy should keep its own state" );

    const geode::uuid unknown;
    geode::OpenGeodeModelException::test(
        !( relations.relations( unknown ) != relations.relations( unknown ) ),
        "Range of an unknown component should be empty" );
    geode::OpenGeodeModelException::test(
        relations.nb_boundaries( unknown ) == 0,
        "Unknown component should have no boundary" );

    geode::index_t nb_relations{ 0 };
    for( const auto& relation : relations.relations( uuids[2] ) )
    {
        geode_unused( relation );
        nb_relations++;
    }
    geode::OpenGeodeModelException::test(
        nb_relations == relations.nb_relations( uuids[2] ),
        "Wrong number of relations of uuids[2]" );
    for( const auto& collection : relations.collections( uuids[2] ) )
    {
        geode::OpenGeodeModelException::test(
            collection.id == uuids[4], "Wrong collection of uuids[2]" );
    }
    for( const auto& embedding : relations.embeddings( uuids[2] ) )
    {
        geode::OpenGeodeModelException::test(
            embedding.id == uuids[0], "Wrong embedding of uuids[2]" );
    }
    for( const auto& internal : relations.internals( uuids[1] ) )
    {
        geode::OpenGeodeModelException::test(
            internal.id == uuids[5], "Wrong internal of uuids[1]" );
    }
}

void test_io(
    std::string_view directory, absl::Span< const geode::uuid > uuids )
{
    geode::Relationships reloaded_relationships;
    geode::RelationshipsBuilder reloader{ reloaded_relationships };
    reloader.load_relationships( directory );
    test_relations( reloaded_relationships, uuids );
}

std::vector< std::pair< std::pair< geode::uuid, geode::uuid >, int > >
    relation_values( const geode::Relationships& relations,
        absl::Span< const geode::uuid > uuids )
{
    const auto attribute = relations.relation_attribute_manager()
                               .find_attribute< geode::VariableAttribute, int >(
                                   relations.relation_attribute_manager()
                                       .attribute_ids_matching_name( "value" )
                                       ->front() );
    std::vector< std::pair< std::pair< geode::uuid, geode::uuid >, int > >
        values;
    for( const auto& uuid : uuids )
    {
        for( const auto& relation : relations.relations( uuid ) )
        {
            const auto index = relations.relation_index( uuid, relation.id );
            geode::OpenGeodeModelException::test( index.has_value(),
                "[remove components] Relation should exist" );
            values.push_back(
                { { uuid, relation.id }, attribute->value( index.value() ) } );
        }
    }
    absl::c_sort( values );
    return values;
}

void create_relation_values( const geode::Relationships& relations )
{
    geode::AttributeProperties attribute_properties;
    attribute_properties.assignable = false;
    attribute_properties.interpolable = false;
    attribute_properties.transferable = true;
    geode::AttributeValues< int > attribute_values;
    attribute_values.default_value = -1;
    attribute_values.no_value = -1;
    const auto attribute_id =
        relations.relation_attribute_manager()
            .create_attribute< geode::VariableAttribute, int >(
                "value", attribute_values, attribute_properties );
    const auto attribute =
        relations.relation_attribute_manager()
            .find_attribute< geode::VariableAttribute, int >( attribute_id );
    for( const auto r :
        geode::Range{ relations.relation_attribute_manager().nb_elements() } )
    {
        attribute->set_value( r, static_cast< int >( r ) );
    }
}

void test_remove_components( absl::Span< const geode::uuid > uuids )
{
    geode::Relationships one_by_one;
    geode::Relationships batch;
    for( auto* relations : { &one_by_one, &batch } )
    {
        add_boundary_relations( *relations, uuids );
        add_internal_relations( *relations, uuids );
        add_items_in_collections( *relations, uuids );
        create_relation_values( *relations );
    }
    const auto values_before = relation_values( one_by_one, uuids );
    geode::RelationshipsBuilder one_by_one_builder{ one_by_one };
    one_by_one_builder.unregister_component( uuids[1] );
    one_by_one_builder.unregister_component( uuids[3] );
    geode::RelationshipsBuilder batch_builder{ batch };
    batch_builder.unregister_components(
        std::vector< geode::uuid >{ uuids[1], uuids[3], uuids[1] } );
    for( const auto* relations : { &one_by_one, &batch } )
    {
        geode::OpenGeodeModelException::test(
            relations->nb_components_with_relations() == 4,
            "[remove components] Wrong number of components with relations" );
        test_uuid( *relations, uuids[0], 1, 0, 1, 0, 0, 1 );
        test_uuid( *relations, uuids[1], 0, 0, 0, 0, 0, 0 );
        test_uuid( *relations, uuids[2], 0, 0, 0, 1, 0, 1 );
        test_uuid( *relations, uuids[3], 0, 0, 0, 0, 0, 0 );
        test_uuid( *relations, uuids[4], 0, 0, 0, 0, 3, 0 );
        test_uuid( *relations, uuids[5], 0, 1, 0, 0, 0, 1 );
        geode::OpenGeodeModelException::test(
            relations->is_boundary( uuids[5], uuids[0] ),
            "[remove components] uuids[5] should be boundary of uuids[0]" );
        geode::OpenGeodeModelException::test(
            relations->is_internal( uuids[2], uuids[0] ),
            "[remove components] uuids[2] should be internal of uuids[0]" );
        geode::OpenGeodeModelException::test(
            relations->is_item( uuids[5], uuids[4] ),
            "[remove components] uuids[5] should be item of uuids[4]" );
        // Relation attribute values are kept for remaining relations
        auto expected_values = values_before;
        expected_values.erase(
            std::remove_if( expected_values.begin(), expected_values.end(),
                [&uuids]( const auto& value ) {
                    const auto& [from, to] = value.first;
                    return from == uuids[1] || from == uuids[3]
                           || to == uuids[1] || to == uuids[3];
                } ),
            expected_values.end() );
        geode::OpenGeodeModelException::test(
            relation_values( *relations, uuids ) == expected_values,
            "[remove components] Wrong relation attribute values" );
    }

    one_by_one_builder.remove_relation( uuids[0], uuids[2] );
    test_uuid( one_by_one, uuids[0], 1, 0, 0, 0, 0, 1 );
    test_uuid( one_by_one, uuids[2], 0, 0, 0, 0, 0, 1 );
    test_uuid( one_by_one, uuids[5], 0, 1, 0, 0, 0, 1 );
    geode::OpenGeodeModelException::test(
        one_by_one.nb_components_with_relations() == 4,
        "[remove relation] Wrong number of components with relations" );
}

void test()
{
    geode::OpenGeodeModelLibrary::initialize();
    geode::Relationships relationships;
    const std::array< geode::uuid, 6 > uuids{
        geode::uuid{ "00000000-a9c9-4d4e-8000-0000d0ecddf1" },
        geode::uuid{ "00000000-f620-4987-8000-00007f488d1c" },
        geode::uuid{ "00000000-006a-4fc8-8000-00002970c6fd" },
        geode::uuid{ "00000000-c823-4e3c-8000-0000d5bbf79b" },
        geode::uuid{ "00000000-cb3b-4476-8000-0000b8510242" },
        geode::uuid{ "00000000-e7cb-4888-8000-0000afce2867" }
    };

    // This Relationships do not represent anything.
    geode::Logger::info( "There should be 3 warnings next, informing that the "
                         "relation already exists." );
    add_boundary_relations( relationships, uuids );
    add_internal_relations( relationships, uuids );
    add_items_in_collections( relationships, uuids );
    test_relations( relationships, uuids );
    test_attributes( relationships, uuids );
    test_ranges( relationships, uuids );
    relationships.save_relationships( "." );
    test_io( absl::StrCat( geode::DATA_PATH, "relationships_v12" ), uuids );
    test_io( ".", uuids );
    test_remove_components( uuids );
}

OPENGEODE_TEST( "relationships" )
