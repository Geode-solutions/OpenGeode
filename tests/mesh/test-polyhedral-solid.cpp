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
#include <geode/basic/attribute_manager.h>
#include <geode/basic/logger.h>
#include <geode/basic/point.h>

#include <geode/mesh/builder/geode_polyhedral_solid_builder.h>
#include <geode/mesh/core/geode_polyhedral_solid.h>
#include <geode/mesh/io/polyhedral_solid_input.h>
#include <geode/mesh/io/polyhedral_solid_output.h>

void test_create_vertices( const geode::PolyhedralSolid3D& polyhedral_solid,
    geode::PolyhedralSolidBuilder3D& builder )
{
    builder.create_point( { { 0.1, 0.2, 0.3 } } );
    builder.create_point( { { 2.1, 9.4, 6.7 } } );
    builder.create_point( { { 7.5, 5.2, 6.3 } } );
    builder.create_point( { { 8.1, 1.4, 4.7 } } );
    builder.create_point( { { 4.7, 2.1, 1.3 } } );
    builder.create_point( { { 9.3, 5.3, 6.7 } } );
    builder.create_point( { { 7.5, 4.2, 2.8 } } );
    builder.create_point( { { 2.2, 3.3, 4.4 } } );
    OPENGEODE_EXCEPTION( polyhedral_solid.nb_vertices() == 8,
        "[Test] PolyhedralSolid should have 8 vertices" );
}

void test_create_polyhedra( const geode::PolyhedralSolid3D& polyhedral_solid,
    geode::PolyhedralSolidBuilder3D& builder )
{
    builder.create_polyhedron(
        { 0, 1, 2, 3, 4, 5 }, { { 0, 1, 2 }, { 3, 5, 4 }, { 0, 3, 4, 1 },
                                  { 0, 2, 5, 3 }, { 1, 4, 5, 2 } } );
    builder.create_polyhedron( { 3, 4, 5, 6 },
        { { 1, 3, 2 }, { 0, 2, 3 }, { 3, 1, 0 }, { 0, 1, 2 } } );
    builder.create_polyhedron( { 4, 5, 6, 7 },
        { { 1, 3, 2 }, { 0, 2, 3 }, { 3, 1, 0 }, { 0, 1, 2 } } );
    OPENGEODE_EXCEPTION( polyhedral_solid.nb_polyhedra() == 3,
        "[Test] PolyhedralSolid should have 3 polyhedra" );
    OPENGEODE_EXCEPTION( polyhedral_solid.nb_facets() == 11,
        "[Test] PolyhedralSolid should have 11 facets" );
}

void test_polyhedron_adjacencies(
    const geode::PolyhedralSolid3D& polyhedral_solid,
    geode::PolyhedralSolidBuilder3D& builder )
{
    builder.compute_polyhedron_adjacencies();
    OPENGEODE_EXCEPTION(
        polyhedral_solid.polyhedron_adjacent( { 0, 0 } ) == geode::NO_ID,
        "[Test] PolyhedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( polyhedral_solid.polyhedron_adjacent( { 0, 1 } ) == 1,
        "[Test] PolyhedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION(
        polyhedral_solid.polyhedron_adjacent( { 0, 2 } ) == geode::NO_ID,
        "[Test] PolyhedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( polyhedral_solid.polyhedron_adjacent( { 1, 0 } ) == 2,
        "[Test] PolyhedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( polyhedral_solid.polyhedron_adjacent( { 1, 3 } ) == 0,
        "[Test] PolyhedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( polyhedral_solid.polyhedron_adjacent( { 2, 3 } ) == 1,
        "[Test] PolyhedralSolid adjacent index is not correct" );

    OPENGEODE_EXCEPTION(
        polyhedral_solid.polyhedra_around_vertex( 4 ).size() == 3,
        "[Test] PolyhedralSolid should have 3 polyhedra around this vertex" );

    OPENGEODE_EXCEPTION(
        polyhedral_solid.polyhedron_facets_on_border( 0 ).size() == 4,
        "[Test] First polyhedron of PolyhedralSolid should have 4 facets on "
        "border" );
}

void test_delete_vertex( const geode::PolyhedralSolid3D& polyhedral_solid,
    geode::PolyhedralSolidBuilder3D& builder )
{
    std::vector< bool > to_delete( polyhedral_solid.nb_vertices(), false );
    to_delete.front() = true;
    builder.delete_vertices( to_delete );
    OPENGEODE_EXCEPTION( polyhedral_solid.nb_vertices() == 7,
        "[Test] PolyhedralSolid should have 7 vertices" );
    geode::Point3D answer{ { 2.1, 9.4, 6.7 } };
    OPENGEODE_EXCEPTION( polyhedral_solid.point( 0 ) == answer,
        "[Test] PolyhedralSolid vertex coordinates are not correct" );
    OPENGEODE_EXCEPTION( polyhedral_solid.nb_polyhedra() == 2,
        "[Test] PolyhedralSolid should have 2 polyhedra" );
    OPENGEODE_EXCEPTION( polyhedral_solid.polyhedron_adjacent( { 1, 3 } ) == 0,
        "[Test] PolyhedralSolid adjacent index is not correct" );
    OPENGEODE_EXCEPTION( polyhedral_solid.nb_facets() == 7,
        "[Test] PolyhedralSolid should have 7 facets" );
}

void test_delete_polyhedra( const geode::PolyhedralSolid3D& polyhedral_solid,
    geode::PolyhedralSolidBuilder3D& builder )
{
    std::vector< bool > to_delete( polyhedral_solid.nb_polyhedra(), false );
    to_delete.front() = true;
    builder.delete_polyhedra( to_delete );
    OPENGEODE_EXCEPTION( polyhedral_solid.nb_polyhedra() == 1,
        "[Test] PolyhedralSolid should have 1 polyhedron" );
    OPENGEODE_EXCEPTION( polyhedral_solid.polyhedron_vertex( { 0, 0 } ) == 3,
        "[Test] PolyhedralSolid vertex index is not correct" );
    OPENGEODE_EXCEPTION( polyhedral_solid.polyhedron_vertex( { 0, 1 } ) == 4,
        "[Test] PolyhedralSolid vertex index is not correct" );
    OPENGEODE_EXCEPTION( polyhedral_solid.polyhedron_vertex( { 0, 2 } ) == 5,
        "[Test] PolyhedralSolid vertex index is not correct" );
    OPENGEODE_EXCEPTION( polyhedral_solid.polyhedron_vertex( { 0, 3 } ) == 6,
        "[Test] PolyhedralSolid vertex index is not correct" );
    OPENGEODE_EXCEPTION( polyhedral_solid.nb_facets() == 4,
        "[Test] PolyhedralSolid should have 4 facets" );
}

void test_io( const geode::PolyhedralSolid3D& polyhedral_solid,
    const std::string& filename )
{
    save_polyhedral_solid( polyhedral_solid, filename );
    auto new_polyhedral_solid = geode::PolyhedralSolid3D::create(
        geode::OpenGeodePolyhedralSolid3D::type_name_static() );
    load_polyhedral_solid( *new_polyhedral_solid, filename );
}

void test_barycenters()
{
    auto polyhedral_solid = geode::PolyhedralSolid3D::create(
        geode::OpenGeodePolyhedralSolid3D::type_name_static() );
    auto builder = geode::PolyhedralSolidBuilder3D::create( *polyhedral_solid );
    double o{ 0.0 };
    double a{ 0.6 };
    double b{ 2.4 };
    double c{ 1.8 };
    builder->create_point( { { o, o, o } } );
    builder->create_point( { { a, o, o } } );
    builder->create_point( { { o, o, c } } );
    builder->create_point( { { o, b, o } } );
    builder->create_point( { { a, b, o } } );
    builder->create_point( { { o, b, c } } );
    builder->create_polyhedron(
        { 0, 1, 2, 3, 4, 5 }, { { 0, 1, 2 }, { 0, 1, 4, 3 }, { 1, 2, 5, 4 },
                                  { 0, 3, 5, 2 }, { 3, 4, 5 } } );
    geode::Point3D answer_facet_barycenter{ { a / 3., 0, c / 3. } };
    OPENGEODE_EXCEPTION(
        polyhedral_solid->polyhedron_facet_barycenter( { 0, 0 } )
            == answer_facet_barycenter,
        "[Test] PolyhedralSolid polyhedron_facet_barycenter is not correct" );
    geode::Point3D answer_polyhedron_barycenter{ { a / 3., 0.5 * b, c / 3. } };
    OPENGEODE_EXCEPTION( polyhedral_solid->polyhedron_barycenter( 0 )
                             == answer_polyhedron_barycenter,
        "[Test] PolyhedralSolid polyhedron barycenter is not correct" );
}

void test_create_vertex_attribute(
    const geode::PolyhedralSolid3D& polyhedral_solid )
{
    auto attribute =
        polyhedral_solid.vertex_attribute_manager()
            .find_or_create_attribute< geode::VariableAttribute, double >(
                "test" );
    for( auto v : geode::Range{ polyhedral_solid.nb_vertices() } )
    {
        attribute->value( v ) = v;
    }
}

void test_clone( const geode::PolyhedralSolid3D& polyhedral_solid )
{
    auto polyhedral_solid2 = polyhedral_solid.clone();
    OPENGEODE_EXCEPTION( polyhedral_solid2->nb_vertices() == 7,
        "[Test] PolyhedralSolid2 should have 7 vertices" );
    OPENGEODE_EXCEPTION( polyhedral_solid2->nb_facets() == 4,
        "[Test]PolyhedralSolid2 should have 4 facets" );
    OPENGEODE_EXCEPTION( polyhedral_solid2->nb_polyhedra() == 1,
        "[Test] PolyhedralSolid2 should have 1 polyhedron" );

    auto attribute2 =
        polyhedral_solid2->vertex_attribute_manager().find_attribute< double >(
            "test" );
    for( auto v : geode::Range{ polyhedral_solid2->nb_vertices() } )
    {
        OPENGEODE_EXCEPTION( attribute2->value( v ) == v + 1,
            "[Test]PolyhedralSolid2 attribute should be "
                + std::to_string( v + 1 ) );
    }
}

void test_set_polyhedron_vertex(
    const geode::PolyhedralSolid3D& polyhedral_solid,
    geode::PolyhedralSolidBuilder3D& builder )
{
    auto facet_id = polyhedral_solid.polyhedron_facet({0,1});
    builder.set_polyhedron_vertex( { 0, 2 }, 1 );

    OPENGEODE_EXCEPTION( polyhedral_solid.polyhedron_vertex( { 0, 2 } ) == 1,
        "[Test] PolyhedronVertex after set_polyhedron_vertex is wrong" );
    OPENGEODE_EXCEPTION( polyhedral_solid.polyhedron_facet_vertex({{0,1},1}) == 1,
        "[Test] PolyhedronFacetVertex after set_polyhedron_vertex is wrong" );
    OPENGEODE_EXCEPTION( polyhedral_solid.polyhedron_facet({0,1}) == facet_id,
        "[Test] Polyhedron facet id after set_polyhedron_vertex is wrong" );
}

void test_delete_all( const geode::PolyhedralSolid3D& polyhedral_solid,
    geode::PolyhedralSolidBuilder3D& builder )
{
    std::vector< bool > to_delete( polyhedral_solid.nb_polyhedra(), true );
    builder.delete_polyhedra( to_delete );
    
    OPENGEODE_EXCEPTION( polyhedral_solid.nb_vertices() == 7,
        "[Test] PolyhedralSolid should have 7 vertices" );
    OPENGEODE_EXCEPTION( polyhedral_solid.nb_facets() == 0,
        "[Test] PolyhedralSolid should have 0 facet" );
    OPENGEODE_EXCEPTION( polyhedral_solid.nb_polyhedra() == 0,
        "[Test] PolyhedralSolid should have 0 polyhedron" );
    OPENGEODE_EXCEPTION( polyhedral_solid.polyhedra_around_vertex( 0 ).empty(),
        "[Test] No more polyhedra around vertices" );
}

int main()
{
    using namespace geode;

    try
    {
        auto polyhedral_solid = PolyhedralSolid3D::create(
            OpenGeodePolyhedralSolid3D::type_name_static() );
        auto builder = PolyhedralSolidBuilder3D::create( *polyhedral_solid );

        test_create_vertices( *polyhedral_solid, *builder );
        test_create_vertex_attribute( *polyhedral_solid );
        test_create_polyhedra( *polyhedral_solid, *builder );
        test_polyhedron_adjacencies( *polyhedral_solid, *builder );
        auto base_file = "test." + polyhedral_solid->native_extension();
        test_io( *polyhedral_solid, base_file );

        test_delete_vertex( *polyhedral_solid, *builder );
        test_delete_polyhedra( *polyhedral_solid, *builder );
        test_clone( *polyhedral_solid );
        test_set_polyhedron_vertex( *polyhedral_solid, *builder );
        test_delete_all( *polyhedral_solid, *builder );

        test_barycenters();

        Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode_lippincott();
    }
}
