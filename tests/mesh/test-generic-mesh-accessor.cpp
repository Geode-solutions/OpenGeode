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

#include <geode/tests/common.hpp>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/uuid.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/tetrahedral_solid_builder.hpp>
#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>

#include <geode/mesh/helpers/generic_edged_curve_accessor.hpp>
#include <geode/mesh/helpers/generic_solid_accessor.hpp>
#include <geode/mesh/helpers/generic_surface_accessor.hpp>

std::unique_ptr< geode::EdgedCurve3D > create_edged_curve()
{
    auto edged_curve = geode::EdgedCurve3D::create();
    auto builder = geode::EdgedCurveBuilder3D::create( *edged_curve );
    builder->create_point( geode::Point3D{ { 0, 0, 0 } } );
    builder->create_point( geode::Point3D{ { 1, 0, 0 } } );
    builder->create_point( geode::Point3D{ { 0, 1, 0 } } );
    builder->create_edge( 0, 1 );
    builder->create_edge( 0, 2 );
    auto attribute = edged_curve->edge_attribute_manager()
                         .find_or_create_attribute< geode::VariableAttribute,
                             geode::index_t >( "test_attribute", 2 );
    attribute->set_value( 1, 5 );
    return edged_curve;
}

std::unique_ptr< geode::TriangulatedSurface2D > create_surface()
{
    auto surface = geode::TriangulatedSurface2D::create();
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
    builder->create_point( geode::Point2D{ { 0, 0 } } );
    builder->create_point( geode::Point2D{ { 1, 0 } } );
    builder->create_point( geode::Point2D{ { 0, 1 } } );
    builder->create_point( geode::Point2D{ { 1, -1 } } );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 0, 3, 1 } );
    builder->compute_polygon_adjacencies();
    auto attribute = surface->polygon_attribute_manager()
                         .find_or_create_attribute< geode::VariableAttribute,
                             geode::index_t >( "test_attribute", 2 );
    attribute->set_value( 1, 5 );
    return surface;
}

std::unique_ptr< geode::TetrahedralSolid3D > create_solid()
{
    auto solid = geode::TetrahedralSolid3D::create();
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_point( geode::Point3D{ { 0, 0, 0 } } );
    builder->create_point( geode::Point3D{ { 1, 0, 0 } } );
    builder->create_point( geode::Point3D{ { 0, 1, 0 } } );
    builder->create_point( geode::Point3D{ { 0, 0, 1 } } );
    builder->create_point( geode::Point3D{ { 1, 1, 1 } } );
    builder->create_tetrahedron( { 0, 1, 2, 3 } );
    builder->create_tetrahedron( { 3, 2, 4, 1 } );
    builder->compute_polyhedron_adjacencies();
    auto attribute = solid->polyhedron_attribute_manager()
                         .find_or_create_attribute< geode::VariableAttribute,
                             geode::index_t >( "test_attribute", 2 );
    attribute->set_value( 1, 5 );
    return solid;
}

template < typename Mesh >
void test_basic_accessor( const Mesh& mesh, geode::index_t nb_element_vertices )
{
    using Accessor = geode::GenericMeshAccessor< Mesh >;
    const Accessor accessor{ mesh };
    OPENGEODE_EXCEPTION(
        accessor.nb_elements() == 2, "[Test] Wrong number of elements" );
    OPENGEODE_EXCEPTION(
        accessor.nb_element_vertices( 0 ) == nb_element_vertices,
        "[Test] Wrong number of element vertices" );
    OPENGEODE_EXCEPTION( accessor.nb_element_facets( 0 ) == nb_element_vertices,
        "[Test] Wrong number of element facets" );
    OPENGEODE_EXCEPTION( accessor.element_vertex( { 0, 0 } ) == 0,
        "[Test] Wrong element vertex { 0, 0 }" );
    OPENGEODE_EXCEPTION( accessor.element_vertex( { 0, 1 } ) == 1,
        "[Test] Wrong element vertex { 0, 1 }" );
    OPENGEODE_EXCEPTION(
        accessor.element_vertices( 0 ).size() == nb_element_vertices,
        "[Test] Wrong size of element vertices container" );
    const auto attribute =
        accessor.element_attribute_manager()
            .template find_attribute< geode::index_t >( "test_attribute" );
    OPENGEODE_EXCEPTION(
        attribute->value( 0 ) == 2 && attribute->value( 1 ) == 5,
        "[Test] Wrong values of the element attributes." );
    OPENGEODE_EXCEPTION(
        accessor.id() == mesh.id(), "[Test] Wrong uuid value." );
    OPENGEODE_EXCEPTION( accessor.point( 0 ).value( 0 ) == 0
                             && accessor.point( 0 ).value( 1 ) == 0,
        "[Test] Wrong point coordinate value." );
}

template < typename Mesh >
void test_adjacent_accessor( const Mesh& mesh )
{
    using Accessor = geode::GenericMeshAccessor< Mesh >;
    const Accessor accessor{ mesh };
    OPENGEODE_EXCEPTION( accessor.element_vertex( { 0, 2 } ) == 2,
        "[Test] Wrong element vertex { 0, 2 }" );
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
    const auto edged_curve = create_edged_curve();
    test_basic_accessor( *edged_curve, 2 );
    const auto surface = create_surface();
    test_basic_accessor( *surface, 3 );
    test_adjacent_accessor( *surface );
    geode::Logger::info( "Test Solid" );
    const auto solid = create_solid();
    test_basic_accessor( *solid, 4 );
    test_adjacent_accessor( *solid );
}

OPENGEODE_TEST( "generic-mesh-accessor" )