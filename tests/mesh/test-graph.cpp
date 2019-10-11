/*
 * Copyright (c) 2019 Geode-solutions
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

#include <geode/basic/attribute.h>
#include <geode/basic/logger.h>

#include <geode/mesh/builder/geode_graph_builder.h>
#include <geode/mesh/core/geode_graph.h>
#include <geode/mesh/io/graph_input.h>
#include <geode/mesh/io/graph_output.h>

void test_create_vertices(
    const geode::Graph& graph, geode::GraphBuilder& builder )
{
    builder.create_vertex();
    OPENGEODE_EXCEPTION(
        graph.nb_vertices() == 1, "[Test] Graph should have 1 vertex" );
    builder.create_vertices( 3 );
    OPENGEODE_EXCEPTION(
        graph.nb_vertices() == 4, "[Test] Graph should have 4 vertices" );
}

void test_delete_vertex(
    const geode::Graph& graph, geode::GraphBuilder& builder )
{
    std::vector< bool > to_delete( graph.nb_vertices(), false );
    to_delete.front() = true;
    builder.delete_vertices( to_delete );
    OPENGEODE_EXCEPTION(
        graph.nb_vertices() == 3, "[Test] Graph should have 3 vertices" );
    OPENGEODE_EXCEPTION(
        graph.nb_edges() == 2, "[Test] Graph should have 2 edges" );

    const auto& edges_around_0 = graph.edges_around_vertex( 0 );
    OPENGEODE_EXCEPTION( edges_around_0.size() == 1,
        "[Test] edges_around_0 should have 1 edge" );
    OPENGEODE_EXCEPTION( edges_around_0[0].edge_id == 1,
        "[Test] edges_around_0 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_0[0].vertex_id == 0,
        "[Test] edges_around_0 has wrong value" );

    const auto& edges_around_2 = graph.edges_around_vertex( 2 );
    OPENGEODE_EXCEPTION( edges_around_2.size() == 1,
        "[Test] edges_around_2 should have 1 edge" );
    OPENGEODE_EXCEPTION( edges_around_2[0].edge_id == 0,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[0].vertex_id == 0,
        "[Test] edges_around_2 has wrong value" );
}

void test_create_edges(
    const geode::Graph& graph, geode::GraphBuilder& builder )
{
    builder.create_edge( 0, 1 );
    builder.create_edge( 0, 2 );
    builder.create_edge( 3, 2 );
    builder.create_edge( 1, 2 );
    OPENGEODE_EXCEPTION(
        graph.nb_edges() == 4, "[Test] Graph should have 4 edges" );

    const auto& edges_around_0 = graph.edges_around_vertex( 0 );
    OPENGEODE_EXCEPTION( edges_around_0.size() == 2,
        "[Test] edges_around_0 should have 2 edges" );
    OPENGEODE_EXCEPTION( edges_around_0[0].edge_id == 0,
        "[Test] edges_around_0 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_0[0].vertex_id == 0,
        "[Test] edges_around_0 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_0[1].edge_id == 1,
        "[Test] edges_around_0 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_0[1].vertex_id == 0,
        "[Test] edges_around_0 has wrong value" );

    const auto& edges_around_2 = graph.edges_around_vertex( 2 );
    OPENGEODE_EXCEPTION( edges_around_2.size() == 3,
        "[Test] edges_around_2 should have 3 edges" );
    OPENGEODE_EXCEPTION( edges_around_2[0].edge_id == 1,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[0].vertex_id == 1,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[1].edge_id == 2,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[1].vertex_id == 1,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[2].edge_id == 3,
        "[Test] edges_around_2 has wrong value" );
    OPENGEODE_EXCEPTION( edges_around_2[2].vertex_id == 1,
        "[Test] edges_around_2 has wrong value" );
}

void test_delete_edge( const geode::Graph& graph, geode::GraphBuilder& builder )
{
    std::vector< bool > to_delete( graph.nb_edges(), false );
    to_delete.front() = true;
    builder.delete_edges( to_delete );
    OPENGEODE_EXCEPTION(
        graph.nb_edges() == 1, "[Test] Graph should have 1 edge" );
    OPENGEODE_EXCEPTION( graph.edge_vertex( { 0, 0 } ) == 0,
        "[Test] Graph edge vertex index is not correct" );
    OPENGEODE_EXCEPTION( graph.edge_vertex( { 0, 1 } ) == 1,
        "[Test] Graph edge vertex index is not correct" );

    builder.create_edges( 10 );
    OPENGEODE_EXCEPTION(
        graph.nb_edges() == 11, "[Test] Graph should have 11 edges" );

    to_delete.resize( graph.nb_edges(), true );
    builder.delete_edges( to_delete );
    OPENGEODE_EXCEPTION(
        graph.nb_edges() == 1, "[Test] Graph should have 1 edge" );
}

void test_io( const geode::Graph& graph, const std::string& filename )
{
    DEBUG( "WRITE" );
    save_graph( graph, filename );
    DEBUG( "READ" );
    auto new_graph =
        geode::Graph::create( geode::OpenGeodeGraph::type_name_static() );
    load_graph( *new_graph, filename );
}

int main()
{
    using namespace geode;

    try
    {
        auto graph = Graph::create( OpenGeodeGraph::type_name_static() );
        auto builder = GraphBuilder::create( *graph );

        test_create_vertices( *graph, *builder );
        test_create_edges( *graph, *builder );
        auto base_file = "test." + graph->native_extension();
        test_io( *graph, base_file );

        test_delete_vertex( *graph, *builder );
        test_delete_edge( *graph, *builder );

        auto default_graph = Graph::create();
        OPENGEODE_EXCEPTION(
            default_graph->type_name() == OpenGeodeGraph::type_name_static(),
            "[Test] Default type for Graph should be OpenGeodeGraph" );

        Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode_lippincott();
    }
}
