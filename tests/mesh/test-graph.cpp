/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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
#include <geode/basic/logger.hpp>
#include <iostream>

#include <geode/mesh/builder/geode/geode_graph_builder.hpp>
#include <geode/mesh/core/geode/geode_graph.hpp>
#include <geode/mesh/io/graph_input.hpp>
#include <geode/mesh/io/graph_output.hpp>

#include <geode/tests/common.hpp>

void test_create_vertices(
    const geode::Graph& graph, geode::GraphBuilder& builder )
{
    builder.create_vertex();
    OPENGEODE_EXCEPTION(
        graph.nb_vertices() == 1, "[Test] Graph should have 1 vertex" );
    builder.create_vertices( 3 );
    OPENGEODE_EXCEPTION(
        graph.nb_vertices() == 4, "[Test] Graph should have 4 vertices" );
}

void test_create_edges(
    const geode::Graph& graph, geode::GraphBuilder& builder )
{
    builder.create_edge( 0, 1 );
    builder.create_edge( 0, 2 );
    builder.create_edge( 3, 2 );
    builder.create_edge( 1, 2 );
    OPENGEODE_EXCEPTION(
        graph.nb_edges() == 4, "[Test] Graph should have 4 edges" );
    std::array< geode::index_t, 2 > answer{ 3, 2 };
    OPENGEODE_EXCEPTION(
        graph.edge_vertices( 2 ) == answer, "[Test] Wrong edge vertices" );

    const auto& edges_around_0 = graph.edges_around_vertex( 0 );
    OPENGEODE_EXCEPTION( edges_around_0.size() == 2,
        "[Test] edges_around_0 should have 2 edges" );
    OPENGEODE_EXCEPTION( edges_around_0[0].edge_id == 0,
        "[Test] edges_around_0 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_0[0].vertex_id == 0,
        "[Test] edges_around_0 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_0[1].edge_id == 1,
        "[Test] edges_around_0 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_0[1].vertex_id == 0,
        "[Test] edges_around_0 has wrong value" );

    const auto& edges_around_2 = graph.edges_around_vertex( 2 );
    OPENGEODE_EXCEPTION( edges_around_2.size() == 3,
        "[Test] edges_around_2 should have 3 edges" );
    OPENGEODE_EXCEPTION( edges_around_2[0].edge_id == 1,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[0].vertex_id == 1,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[1].edge_id == 2,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[1].vertex_id == 1,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[2].edge_id == 3,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[2].vertex_id == 1,
        "[Test] edges_around_2 has wrong value" );
}

void test_delete_edge( const geode::Graph& graph, geode::GraphBuilder& builder )
{
    std::vector< bool > to_delete( graph.nb_edges(), false );
    to_delete.front() = true;
    builder.delete_edges( to_delete );
    OPENGEODE_EXCEPTION(
        graph.nb_edges() == 3, "[Test] Graph should have 3 edges" );
    OPENGEODE_EXCEPTION( graph.edge_vertex( { 0, 0 } ) == 0,
        "[Test] Graph edge vertex index is not correct" );
    OPENGEODE_EXCEPTION( graph.edge_vertex( { 0, 1 } ) == 2,
        "[Test] Graph edge vertex index is not correct" );
    OPENGEODE_EXCEPTION( graph.edge_vertex( { 1, 0 } ) == 3,
        "[Test] Graph edge vertex index is not correct" );
    OPENGEODE_EXCEPTION( graph.edge_vertex( { 1, 1 } ) == 2,
        "[Test] Graph edge vertex index is not correct" );
    OPENGEODE_EXCEPTION( graph.edge_vertex( { 2, 0 } ) == 1,
        "[Test] Graph edge vertex index is not correct" );
    OPENGEODE_EXCEPTION( graph.edge_vertex( { 2, 1 } ) == 2,
        "[Test] Graph edge vertex index is not correct" );

    builder.create_edges( 10 );
    builder.set_edge_vertex( { 1, 0 }, 1 );
    builder.set_edge_vertex( { 1, 1 }, 0 );
    OPENGEODE_EXCEPTION(
        graph.nb_edges() == 13, "[Test] Graph should have 13 edges" );

    to_delete.back() = true;
    to_delete.resize( graph.nb_edges(), true );
    builder.delete_edges( to_delete );
    OPENGEODE_EXCEPTION(
        graph.nb_edges() == 2, "[Test] Graph should have 2 edges" );
    OPENGEODE_EXCEPTION( graph.edge_vertex( { 0, 0 } ) == 1,
        "[Test] Graph edge vertex index is not correct (0, 0)" );
    OPENGEODE_EXCEPTION( graph.edge_vertex( { 0, 1 } ) == 0,
        "[Test] Graph edge vertex index is not correct (0, 1)" );
}

void test_io( const geode::Graph& graph, const std::string& filename )
{
    geode::save_graph( graph, filename );
    const auto graph2 = geode::load_graph(
        geode::OpenGeodeGraph::impl_name_static(), filename );
    geode_unused( graph2 );
}

void test_backward_io( const std::string& filename )
{
    auto graph = geode::load_graph( filename );

    OPENGEODE_EXCEPTION( graph->edges_around_vertex( 2 ).size() == 3,
        "[Test] Backward edges_around should have 3 edges" );
}

void test_clone( const geode::Graph& graph )
{
    const auto graph_clone = graph.clone();
    geode::OpenGeodeGraph graph2{ std::move(
        *dynamic_cast< geode::OpenGeodeGraph* >( graph_clone.get() ) ) };
    OPENGEODE_EXCEPTION(
        graph2.nb_vertices() == 4, "[Test] Graph2 should have 4 vertices" );
    OPENGEODE_EXCEPTION(
        graph2.nb_edges() == 2, "[Test] Graph2 should have2 edge2" );
}

void test_delete_isolated_vertices(
    const geode::Graph& graph, geode::GraphBuilder& builder )
{
    builder.delete_isolated_vertices();
    OPENGEODE_EXCEPTION(
        graph.nb_vertices() == 3, "[Test] Graph should have 3 vertices" );
    OPENGEODE_EXCEPTION(
        graph.nb_edges() == 2, "[Test] Graph should have 2 edges" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    auto graph =
        geode::Graph::create( geode::OpenGeodeGraph::impl_name_static() );
    auto builder = geode::GraphBuilder::create( *graph );

    test_create_vertices( *graph, *builder );
    test_create_edges( *graph, *builder );
    test_io( *graph, absl::StrCat( "test.", graph->native_extension() ) );
    test_backward_io( absl::StrCat(
        geode::DATA_PATH, "test_v7.", graph->native_extension() ) );

    test_delete_edge( *graph, *builder );
    test_clone( *graph );
    test_delete_isolated_vertices( *graph, *builder );

    const auto default_graph = geode::Graph::create();
    OPENGEODE_EXCEPTION(
        default_graph->impl_name() == geode::OpenGeodeGraph::impl_name_static(),
        "[Test] Default type for Graph should be OpenGeodeGraph" );
}

OPENGEODE_TEST( "graph" )