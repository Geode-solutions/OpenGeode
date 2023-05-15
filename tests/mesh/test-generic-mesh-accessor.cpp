/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/tests/common.h>

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/tetrahedral_solid_builder.h>
#include <geode/mesh/builder/triangulated_surface_builder.h>
#include <geode/mesh/core/tetrahedral_solid.h>
#include <geode/mesh/core/triangulated_surface.h>

#include <geode/mesh/helpers/generic_solid_accessor.h>
#include <geode/mesh/helpers/generic_surface_accessor.h>

std::unique_ptr< geode::TriangulatedSurface2D > create_surface()
{
    auto surface = geode::TriangulatedSurface2D::create();
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
    builder->create_point( { { 0, 0 } } );
    builder->create_point( { { 1, 0 } } );
    builder->create_point( { { 0, 1 } } );
    builder->create_point( { { 1, -1 } } );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 0, 3, 1 } );
    builder->compute_polygon_adjacencies();
    return surface;
}

std::unique_ptr< geode::TetrahedralSolid3D > create_solid()
{
    auto solid = geode::TetrahedralSolid3D::create();
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_point( { { 0, 0, 0 } } );
    builder->create_point( { { 1, 0, 0 } } );
    builder->create_point( { { 0, 1, 0 } } );
    builder->create_point( { { 0, 0, 1 } } );
    builder->create_point( { { 1, 1, 1 } } );
    builder->create_tetrahedron( { 0, 1, 2, 3 } );
    builder->create_tetrahedron( { 3, 2, 4, 1 } );
    builder->compute_polyhedron_adjacencies();
    return solid;
}

template < typename Mesh >
void test_accessor( const Mesh& mesh )
{
    using Accessor = geode::GenericMeshAccessor< Mesh >;
    const Accessor accessor{ mesh };
    OPENGEODE_EXCEPTION(
        accessor.nb_elements() == 2, "[Test] Wrong number of elements" );
    OPENGEODE_EXCEPTION( accessor.nb_element_vertices( 0 ) == Mesh::dim + 1,
        "[Test] Wrong number of element vertices" );
    OPENGEODE_EXCEPTION( accessor.nb_element_facets( 0 ) == Mesh::dim + 1,
        "[Test] Wrong number of element facets" );
    OPENGEODE_EXCEPTION( accessor.element_vertex( { 0, 0 } ) == 0,
        "[Test] Wrong element vertex { 0, 0 }" );
    OPENGEODE_EXCEPTION( accessor.element_vertex( { 0, 1 } ) == 1,
        "[Test] Wrong element vertex { 0, 1 }" );
    OPENGEODE_EXCEPTION( accessor.element_vertex( { 0, 2 } ) == 2,
        "[Test] Wrong element vertex { 0, 2 }" );
    OPENGEODE_EXCEPTION( accessor.element_vertices( 0 ).size() == Mesh::dim + 1,
        "[Test] Wrong number of element vertices" );
    const typename Accessor::ElementFacet facet{ 0, 0 };
    OPENGEODE_EXCEPTION(
        accessor.element_facet_vertices( facet ).size() == Mesh::dim,
        "[Test] Wrong number of element facet vertices" );
    OPENGEODE_EXCEPTION( !accessor.element_adjacent( { 0, 1 } ),
        "[Test] Wrong element adjacent { 0, 1 }" );
    OPENGEODE_EXCEPTION( accessor.element_adjacent( { 0, 0 } ) == 1,
        "[Test] Wrong element adjacent { 0, 0 }" );
    const typename Accessor::ElementFacet adj{ 0, 0 };
    OPENGEODE_EXCEPTION( accessor.element_adjacent_facet( { 1, 2 } ) == adj,
        "[Test] Wrong element adjacent { 1, 2 }" );
}

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    geode::Logger::info( "Test Surface" );
    const auto surface = create_surface();
    test_accessor( *surface );
    geode::Logger::info( "Test Solid" );
    const auto solid = create_solid();
    test_accessor( *solid );
}

OPENGEODE_TEST( "generic-mesh-accessor" )