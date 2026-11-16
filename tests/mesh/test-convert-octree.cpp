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

#include <geode/tests/common.hpp>

#include <absl/container/flat_hash_set.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/uuid.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/geometry/octree.hpp>
#include <geode/mesh/common.hpp>

#include <geode/mesh/core/hybrid_solid.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>
#include <geode/mesh/helpers/convert_solid_mesh.hpp>
#include <geode/mesh/helpers/convert_surface_mesh.hpp>

namespace
{
    template < geode::index_t dimension >
    geode::BoundingBox< dimension > non_cubic_box();

    template <>
    geode::BoundingBox2D non_cubic_box()
    {
        return { geode::Point2D{ { 0.3, -1.1 } },
            geode::Point2D{ { 7.7, 1.3 } } };
    }

    template <>
    geode::BoundingBox3D non_cubic_box()
    {
        return { geode::Point3D{ { 0.3, -1.1, 2. } },
            geode::Point3D{ { 7.7, 1.3, 3.1 } } };
    }

    template < geode::index_t dimension >
    geode::Octree< dimension > unbalanced_octree()
    {
        geode::Octree< dimension > octree{ non_cubic_box< dimension >() };
        // refine a root, then repeatedly its first child, and another root
        auto leaf = octree.refine( 1 );
        for( [[maybe_unused]] const auto i : geode::Range{ 3 } )
        {
            leaf = octree.refine( leaf );
        }
        octree.refine( octree.nb_cells() - 1 );
        octree.refine( 0 );
        return octree;
    }

    template < geode::index_t dimension >
    geode::uuid add_leaf_attribute( const geode::Octree< dimension >& octree )
    {
        geode::AttributeValues< double > values;
        values.default_value = -1;
        values.no_value = -1;
        const auto attribute_id =
            octree.cell_attribute_manager()
                .template create_attribute< geode::VariableAttribute, double >(
                    "metric", values, geode::AttributeProperties{} );
        auto attribute =
            octree.cell_attribute_manager()
                .template find_attribute< geode::VariableAttribute, double >(
                    attribute_id );
        for( const auto leaf : octree.leaves() )
        {
            attribute->set_value( leaf, 0.5 + leaf );
        }
        return attribute_id;
    }

    template < geode::index_t dimension >
    absl::flat_hash_set< std::array< uint64_t, dimension > >
        brute_force_corners( const geode::Octree< dimension >& octree )
    {
        absl::flat_hash_set< std::array< uint64_t, dimension > > corners;
        for( const auto leaf : octree.leaves() )
        {
            const auto& anchor = octree.cell_anchor( leaf );
            const uint64_t size{ uint64_t{ 1 } << ( octree.max_depth()
                                                    - octree.level( leaf ) ) };
            for( const auto c : geode::Range{ 1u << dimension } )
            {
                std::array< uint64_t, dimension > key;
                for( const auto d : geode::LRange{ dimension } )
                {
                    key[d] = anchor[d] + ( ( ( c >> d ) & 1u ) ? size : 0 );
                }
                corners.insert( key );
            }
        }
        return corners;
    }

    template < geode::index_t dimension >
    void check_octree_mesh( const geode::Octree< dimension >& octree,
        const geode::BoundingBox< dimension >& domain,
        geode::index_t nb_vertices,
        geode::index_t nb_elements,
        double total_measure )
    {
        const auto leaves = octree.leaves();
        geode::OpenGeodeMeshException::test(
            nb_elements == leaves.size(), "[Test] Wrong number of elements" );
        geode::OpenGeodeMeshException::test(
            nb_vertices == brute_force_corners( octree ).size(),
            "[Test] Wrong number of vertices" );
        double domain_measure{ 1 };
        for( const auto d : geode::LRange{ dimension } )
        {
            domain_measure *= domain.max().value( d ) - domain.min().value( d );
        }
        geode::OpenGeodeMeshException::test(
            std::fabs( total_measure - domain_measure )
                <= geode::GLOBAL_EPSILON * domain_measure,
            "[Test] Wrong total measure" );
    }

    void test_2d()
    {
        geode::Logger::info( "Test octree into polygonal surface" );
        const auto octree = unbalanced_octree< 2 >();
        const auto attribute_id = add_leaf_attribute( octree );
        const auto surface =
            geode::convert_octree_into_polygonal_surface( octree );
        double area{ 0 };
        for( const auto p : geode::Range{ surface->nb_polygons() } )
        {
            geode::OpenGeodeMeshException::test(
                surface->nb_polygon_vertices( p ) == 4,
                "[Test] Polygon should be a quadrangle" );
            area += surface->polygon_area( p );
        }
        check_octree_mesh( octree, non_cubic_box< 2 >(), surface->nb_vertices(),
            surface->nb_polygons(), area );
        const auto leaves = octree.leaves();
        auto attribute =
            surface->polygon_attribute_manager()
                .find_read_only_attribute< double >( attribute_id );
        for( const auto p : geode::Range{ surface->nb_polygons() } )
        {
            const auto box = octree.cell_bounding_box( leaves[p] );
            geode::BoundingBox2D polygon_box;
            for( const auto v : geode::LRange{ 4 } )
            {
                polygon_box.add_point(
                    surface->point( surface->polygon_vertex( { p, v } ) ) );
            }
            geode::OpenGeodeMeshException::test(
                polygon_box.min() == box.min()
                    && polygon_box.max() == box.max(),
                "[Test] Polygon box differs from leaf box" );
            geode::OpenGeodeMeshException::test(
                attribute->value( p ) == 0.5 + leaves[p],
                "[Test] Wrong attribute value" );
        }
        const auto domain = surface->bounding_box();
        geode::OpenGeodeMeshException::test(
            domain.min() == non_cubic_box< 2 >().min()
                && domain.max() == non_cubic_box< 2 >().max(),
            "[Test] Mesh bounding box should be the octree domain" );
        // refined leaf 0 has 4 children: conforming interior adjacencies
        geode::index_t nb_adjacencies{ 0 };
        for( const auto p : geode::Range{ surface->nb_polygons() } )
        {
            for( const auto e : geode::LRange{ 4 } )
            {
                nb_adjacencies += surface->polygon_adjacent( { p, e } ) ? 1 : 0;
            }
        }
        geode::OpenGeodeMeshException::test(
            nb_adjacencies > 0, "[Test] No adjacency" );
    }

    void test_3d()
    {
        geode::Logger::info( "Test octree into hybrid solid" );
        const auto octree = unbalanced_octree< 3 >();
        const auto attribute_id = add_leaf_attribute( octree );
        const auto solid = geode::convert_octree_into_hybrid_solid( octree );
        double volume{ 0 };
        for( const auto p : geode::Range{ solid->nb_polyhedra() } )
        {
            geode::OpenGeodeMeshException::test(
                solid->nb_polyhedron_vertices( p ) == 8,
                "[Test] Polyhedron should be a hexahedron" );
            volume += solid->polyhedron_volume( p );
        }
        check_octree_mesh( octree, non_cubic_box< 3 >(), solid->nb_vertices(),
            solid->nb_polyhedra(), volume );
        const auto leaves = octree.leaves();
        auto attribute =
            solid->polyhedron_attribute_manager()
                .find_read_only_attribute< double >( attribute_id );
        for( const auto p : geode::Range{ solid->nb_polyhedra() } )
        {
            const auto box = octree.cell_bounding_box( leaves[p] );
            geode::BoundingBox3D polyhedron_box;
            for( const auto v : geode::LRange{ 8 } )
            {
                polyhedron_box.add_point(
                    solid->point( solid->polyhedron_vertex( { p, v } ) ) );
            }
            geode::OpenGeodeMeshException::test(
                polyhedron_box.min() == box.min()
                    && polyhedron_box.max() == box.max(),
                "[Test] Polyhedron box differs from leaf box" );
            geode::OpenGeodeMeshException::test(
                attribute->value( p ) == 0.5 + leaves[p],
                "[Test] Wrong attribute value" );
        }
        const auto domain = solid->bounding_box();
        geode::OpenGeodeMeshException::test(
            domain.min() == non_cubic_box< 3 >().min()
                && domain.max() == non_cubic_box< 3 >().max(),
            "[Test] Mesh bounding box should be the octree domain" );
        geode::index_t nb_adjacencies{ 0 };
        for( const auto p : geode::Range{ solid->nb_polyhedra() } )
        {
            for( const auto f : geode::LRange{ 6 } )
            {
                nb_adjacencies +=
                    solid->polyhedron_adjacent( { p, f } ) ? 1 : 0;
            }
        }
        geode::OpenGeodeMeshException::test(
            nb_adjacencies > 0, "[Test] No adjacency" );
    }

} // namespace

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    geode::Logger::set_level( geode::Logger::LEVEL::debug );
    test_2d();
    test_3d();
}

OPENGEODE_TEST( "convert-octree" )
