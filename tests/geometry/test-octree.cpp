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

#include <cmath>
#include <cstdio>
#include <fstream>
#include <functional>
#include <string>

#include <absl/algorithm/container.h>
#include <absl/strings/str_cat.h>

#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/bitsery_archive.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/geometry/bitsery_archive.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/octree.hpp>
#include <geode/geometry/point.hpp>

#include <geode/tests/common.hpp>

namespace
{
    template < geode::index_t dimension >
    geode::Point< dimension > uniform_point( double value )
    {
        geode::Point< dimension > point;
        for( const auto d : geode::LRange{ dimension } )
        {
            point.set_value( d, value );
        }
        return point;
    }

    template < geode::index_t dimension >
    geode::BoundingBox< dimension > uniform_box( double min, double max )
    {
        return { uniform_point< dimension >( min ),
            uniform_point< dimension >( max ) };
    }

    template < geode::index_t dimension >
    constexpr geode::local_index_t expected_bits()
    {
        return dimension == 3 ? 21 : 31;
    }

    template < geode::index_t dimension >
    void check_cell_consistency( const geode::Octree< dimension >& octree )
    {
        for( const auto cell : geode::Range{ octree.nb_cells() } )
        {
            const auto level = octree.level( cell );
            const auto& anchor = octree.cell_anchor( cell );
            const auto found = octree.cell( level, anchor );
            geode::OpenGeodeGeometryException::test(
                found && found.value() == cell,
                "[Test] Wrong cell lookup for cell ", cell );
            const auto box = octree.cell_bounding_box( cell );
            const auto lengths = octree.cell_lengths( cell );
            const auto center = octree.cell_center( cell );
            for( const auto d : geode::LRange{ dimension } )
            {
                const auto side = box.max().value( d ) - box.min().value( d );
                geode::OpenGeodeGeometryException::test(
                    std::fabs( side - lengths[d] ) <= geode::GLOBAL_EPSILON,
                    "[Test] Wrong cell lengths for cell ", cell );
                geode::OpenGeodeGeometryException::test(
                    std::fabs( center.value( d )
                               - ( box.min().value( d ) + side / 2. ) )
                        <= geode::GLOBAL_EPSILON,
                    "[Test] Wrong cell center for cell ", cell );
            }
            if( const auto parent = octree.parent( cell ) )
            {
                geode::OpenGeodeGeometryException::test(
                    octree.level( parent.value() ) + 1 == level,
                    "[Test] Wrong parent level for cell ", cell );
                const auto first_child = octree.children( parent.value() );
                geode::OpenGeodeGeometryException::test(
                    first_child && cell >= first_child.value()
                        && cell < first_child.value()
                                      + geode::Octree< dimension >::NB_CHILDREN,
                    "[Test] Cell ", cell, " not in its parent children" );
                const auto local = cell - first_child.value();
                const auto parent_box =
                    octree.cell_bounding_box( parent.value() );
                const auto parent_center = octree.cell_center( parent.value() );
                geode::OpenGeodeGeometryException::test(
                    parent_box.contains( box ), "[Test] Cell ", cell,
                    " not inside its parent" );
                for( const auto d : geode::LRange{ dimension } )
                {
                    const auto upper = ( ( local >> d ) & 1 ) != 0;
                    geode::OpenGeodeGeometryException::test(
                        upper
                            == ( center.value( d ) > parent_center.value( d ) ),
                        "[Test] Wrong child ordering for cell ", cell );
                }
            }
            else
            {
                geode::OpenGeodeGeometryException::test(
                    level == 0, "[Test] Cell without parent is not a root" );
            }
            if( const auto first_child = octree.children( cell ) )
            {
                geode::OpenGeodeGeometryException::test(
                    !octree.is_leaf( cell ), "[Test] Wrong is_leaf" );
                for( const auto c :
                    geode::Range{ geode::Octree< dimension >::NB_CHILDREN } )
                {
                    geode::OpenGeodeGeometryException::test(
                        octree.parent( first_child.value() + c ) == cell,
                        "[Test] Wrong child parent" );
                }
            }
            else
            {
                geode::OpenGeodeGeometryException::test(
                    octree.is_leaf( cell ), "[Test] Wrong is_leaf" );
            }
        }
    }

    template < geode::index_t dimension >
    void test_construction()
    {
        geode::Logger::info( "Test construction ", dimension, "D" );
        const geode::Octree< dimension > octree{ uniform_box< dimension >(
            1., 3. ) };
        geode::OpenGeodeGeometryException::test(
            octree.nb_cells() == 1, "[Test] Wrong number of cells" );
        geode::OpenGeodeGeometryException::test(
            octree.max_depth() == expected_bits< dimension >(),
            "[Test] Wrong max depth" );
        geode::OpenGeodeGeometryException::test(
            octree.is_leaf( 0 ) && octree.level( 0 ) == 0 && !octree.parent( 0 )
                && !octree.children( 0 ),
            "[Test] Wrong root cell" );
        geode::OpenGeodeGeometryException::test(
            octree.domain_bounding_box().min()
                    == uniform_point< dimension >( 1 )
                && octree.domain_bounding_box().max()
                       == uniform_point< dimension >( 3 ),
            "[Test] Wrong domain bounding box" );
        check_cell_consistency( octree );

        try
        {
            geode::Point< dimension > max = uniform_point< dimension >( 1. );
            max.set_value( 0, 0. );
            const geode::Octree< dimension > degenerate{
                { uniform_point< dimension >( 0. ), max }
            };
            geode::OpenGeodeGeometryException::test(
                false, "[Test] Degenerate box should throw" );
        }
        catch( const geode::OpenGeodeGeometryException& )
        {
        }

        geode::Octree< dimension > empty;
        geode::OpenGeodeGeometryException::test(
            empty.nb_cells() == 0 && empty.leaves().empty()
                && !empty.leaf_containing( uniform_point< dimension >( 0. ) ),
            "[Test] Wrong empty octree" );
        empty = geode::Octree< dimension >{ uniform_box< dimension >( 0, 1 ) };
        geode::OpenGeodeGeometryException::test(
            empty.nb_cells() == 1, "[Test] Wrong move assigned octree" );
    }

    void test_non_cubic_3d()
    {
        geode::Logger::info( "Test non cubic 3D" );
        const geode::Octree3D octree{ { geode::Point3D{ { 0., 0., 0. } },
            geode::Point3D{ { 10., 3., 1. } } } };
        geode::OpenGeodeGeometryException::test(
            octree.nb_root_cells()
                == std::array< geode::index_t, 3 >{ 10, 3, 1 },
            "[Test] Wrong 3D root cells" );
        geode::OpenGeodeGeometryException::test(
            octree.nb_cells() == 30, "[Test] Wrong number of 3D root cells" );
        // 10 roots -> 4 root bits
        geode::OpenGeodeGeometryException::test(
            octree.max_depth() == 17, "[Test] Wrong 3D max depth" );
        geode::OpenGeodeGeometryException::test(
            octree.cell_lengths( 0 ) == std::array< double, 3 >{ 1., 1., 1. },
            "[Test] Wrong 3D root lengths" );
        // Row-major, axis 0 fastest
        const auto box = octree.cell_bounding_box( 13 );
        geode::OpenGeodeGeometryException::test(
            box.min() == geode::Point3D{ { 3., 1., 0. } }
                && box.max() == geode::Point3D{ { 4., 2., 1. } },
            "[Test] Wrong 3D root ordering" );
        geode::OpenGeodeGeometryException::test(
            octree.cell_anchor( 13 )
                == std::array< uint32_t, 3 >{ 3u << 17, 1u << 17, 0 },
            "[Test] Wrong 3D root anchor" );
        // Shared face between roots goes to the upper root, domain border to
        // the last root
        geode::OpenGeodeGeometryException::test(
            octree.leaf_containing( geode::Point3D{ { 1., 0.5, 0.5 } } ) == 1,
            "[Test] Wrong leaf on root shared face" );
        geode::OpenGeodeGeometryException::test(
            octree.leaf_containing( geode::Point3D{ { 10., 3., 1. } } ) == 29,
            "[Test] Wrong leaf on domain upper corner" );
        geode::OpenGeodeGeometryException::test(
            octree.leaf_containing( geode::Point3D{ { 4., 2., 0. } } ) == 24,
            "[Test] Wrong leaf on root shared edge" );
        geode::OpenGeodeGeometryException::test(
            !octree.leaf_containing( geode::Point3D{ { 10.1, 1., 0.5 } } ),
            "[Test] Point outside domain should have no leaf" );
        check_cell_consistency( octree );
    }

    void test_non_cubic_2d()
    {
        geode::Logger::info( "Test non cubic 2D" );
        const geode::Octree2D octree{ { geode::Point2D{ { -1., 0. } },
            geode::Point2D{ { 6., 2. } } } };
        // round( 7 / 2 ) = 4 roots of length 1.75 along x
        geode::OpenGeodeGeometryException::test(
            octree.nb_root_cells() == std::array< geode::index_t, 2 >{ 4, 1 },
            "[Test] Wrong 2D root cells" );
        geode::OpenGeodeGeometryException::test(
            octree.max_depth() == 29, "[Test] Wrong 2D max depth" );
        geode::OpenGeodeGeometryException::test(
            octree.cell_lengths( 0 ) == std::array< double, 2 >{ 1.75, 2. },
            "[Test] Wrong 2D root lengths" );
        geode::OpenGeodeGeometryException::test(
            octree.domain_bounding_box().max() == geode::Point2D{ { 6., 2. } },
            "[Test] Wrong 2D domain" );
        check_cell_consistency( octree );
    }

    template < geode::index_t dimension >
    void test_refine()
    {
        geode::Logger::info( "Test refine ", dimension, "D" );
        constexpr auto nb_children = geode::Octree< dimension >::NB_CHILDREN;
        geode::Octree< dimension > octree{ uniform_box< dimension >( 0., 2. ) };
        const auto first = octree.refine( 0 );
        geode::OpenGeodeGeometryException::test(
            first == 1 && octree.nb_cells() == 1 + nb_children
                && octree.children( 0 ) == 1 && !octree.is_leaf( 0 ),
            "[Test] Wrong first refine" );
        const auto half = uint32_t{ 1 } << ( octree.max_depth() - 1 );
        for( const auto c : geode::Range{ nb_children } )
        {
            const auto child = first + c;
            geode::OpenGeodeGeometryException::test(
                octree.level( child ) == 1 && octree.parent( child ) == 0
                    && octree.is_leaf( child ),
                "[Test] Wrong child ", child );
            std::array< uint32_t, dimension > anchor;
            for( const auto d : geode::LRange{ dimension } )
            {
                anchor[d] = ( ( c >> d ) & 1 ) != 0 ? half : 0;
            }
            geode::OpenGeodeGeometryException::test(
                octree.cell_anchor( child ) == anchor,
                "[Test] Wrong child anchor ", child );
            geode::OpenGeodeGeometryException::test(
                octree.cell_lengths( child )[0] == 1.,
                "[Test] Wrong child length ", child );
        }
        const auto last_child = first + nb_children - 1;
        geode::OpenGeodeGeometryException::test(
            octree.cell_bounding_box( last_child ).min()
                    == uniform_point< dimension >( 1. )
                && octree.cell_bounding_box( last_child ).max()
                       == uniform_point< dimension >( 2. )
                && octree.cell_center( last_child )
                       == uniform_point< dimension >( 1.5 ),
            "[Test] Wrong last child geometry" );

        std::vector< std::array< uint32_t, dimension > > anchors;
        std::vector< geode::local_index_t > levels;
        for( const auto cell : geode::Range{ octree.nb_cells() } )
        {
            anchors.push_back( octree.cell_anchor( cell ) );
            levels.push_back( octree.level( cell ) );
        }
        const auto second = octree.refine( last_child );
        geode::OpenGeodeGeometryException::test(
            second == 1 + nb_children, "[Test] Wrong second refine" );
        const auto third = octree.refine( first );
        geode::OpenGeodeGeometryException::test(
            third == 1 + 2 * nb_children, "[Test] Wrong third refine" );
        octree.refine( second );
        for( const auto cell : geode::Range{ anchors.size() } )
        {
            geode::OpenGeodeGeometryException::test(
                octree.cell_anchor( cell ) == anchors[cell]
                    && octree.level( cell ) == levels[cell],
                "[Test] Cell ", cell, " changed after refine" );
        }
        geode::OpenGeodeGeometryException::test(
            octree.nb_cells() == 1 + 4 * nb_children,
            "[Test] Wrong number of cells" );
        geode::OpenGeodeGeometryException::test(
            octree.leaves().size() == 1 + 4 * ( nb_children - 1 ),
            "[Test] Wrong number of leaves" );
        geode::OpenGeodeGeometryException::test(
            absl::c_is_sorted( octree.leaves() ), "[Test] Unsorted leaves" );
        check_cell_consistency( octree );

        try
        {
            octree.refine( 0 );
            geode::OpenGeodeGeometryException::test(
                false, "[Test] Refining a non-leaf should throw" );
        }
        catch( const geode::OpenGeodeGeometryException& )
        {
        }

        // Lookup of missing cells
        std::array< uint32_t, dimension > zero{};
        geode::OpenGeodeGeometryException::test(
            octree.cell( 0, zero ) == 0 && octree.cell( 1, zero ) == first
                && octree.cell( 2, zero ) == third,
            "[Test] Wrong lookup of existing cells" );
        geode::OpenGeodeGeometryException::test(
            !octree.cell( 3, zero ), "[Test] Missing cell should not exist" );
        auto misaligned = zero;
        misaligned[0] = 1;
        geode::OpenGeodeGeometryException::test(
            !octree.cell( 1, misaligned ), "[Test] Misaligned anchor found" );
        auto outside = zero;
        outside[0] = 2 * half;
        geode::OpenGeodeGeometryException::test(
            !octree.cell( 1, outside ), "[Test] Outside anchor found" );
        geode::OpenGeodeGeometryException::test(
            !octree.cell(
                static_cast< geode::local_index_t >( octree.max_depth() + 1 ),
                zero ),
            "[Test] Too deep cell found" );
    }

    template < geode::index_t dimension >
    void test_attributes()
    {
        geode::Logger::info( "Test attributes ", dimension, "D" );
        constexpr auto nb_children = geode::Octree< dimension >::NB_CHILDREN;
        geode::Octree< dimension > octree{ uniform_box< dimension >( 0., 1. ) };
        geode::AttributeProperties properties;
        properties.assignable = false;
        properties.interpolable = false;
        geode::AttributeValues< double > values;
        values.default_value = -1;
        values.no_value = -1;
        const auto attribute_id =
            octree.cell_attribute_manager()
                .template create_attribute< geode::VariableAttribute, double >(
                    "value", values, properties );
        auto attribute =
            octree.cell_attribute_manager()
                .template find_attribute< geode::VariableAttribute, double >(
                    attribute_id );
        attribute->set_value( 0, 5 );
        const auto first = octree.refine( 0 );
        geode::OpenGeodeGeometryException::test(
            octree.cell_attribute_manager().nb_elements() == octree.nb_cells(),
            "[Test] Wrong attribute manager size" );
        for( const auto c : geode::Range{ nb_children } )
        {
            geode::OpenGeodeGeometryException::test(
                attribute->value( first + c ) == 5,
                "[Test] Child did not inherit its parent value" );
        }
        attribute->set_value( first + 1, 7 );
        const auto second = octree.refine( first + 1 );
        for( const auto c : geode::Range{ nb_children } )
        {
            geode::OpenGeodeGeometryException::test(
                attribute->value( second + c ) == 7,
                "[Test] Grandchild did not inherit its parent value" );
        }
        geode::OpenGeodeGeometryException::test(
            attribute->value( 0 ) == 5 && attribute->value( first ) == 5,
            "[Test] Internal cell values changed" );
    }

    template < geode::index_t dimension >
    void test_leaf_containing()
    {
        geode::Logger::info( "Test leaf containing ", dimension, "D" );
        constexpr auto nb_children = geode::Octree< dimension >::NB_CHILDREN;
        geode::Octree< dimension > octree{ uniform_box< dimension >( 0., 2. ) };
        const auto first = octree.refine( 0 );
        const auto second = octree.refine( first );
        // Interior points
        geode::OpenGeodeGeometryException::test(
            octree.leaf_containing( uniform_point< dimension >( 0.2 ) )
                == second,
            "[Test] Wrong leaf for interior point" );
        geode::OpenGeodeGeometryException::test(
            octree.leaf_containing( uniform_point< dimension >( 1.5 ) )
                == first + nb_children - 1,
            "[Test] Wrong leaf for interior point" );
        // Shared faces go to the upper side
        geode::OpenGeodeGeometryException::test(
            octree.leaf_containing( uniform_point< dimension >( 0.5 ) )
                == second + nb_children - 1,
            "[Test] Wrong leaf on shared corner" );
        geode::OpenGeodeGeometryException::test(
            octree.leaf_containing( uniform_point< dimension >( 1. ) )
                == first + nb_children - 1,
            "[Test] Wrong leaf on shared corner" );
        auto on_face = uniform_point< dimension >( 0.2 );
        on_face.set_value( 0, 1. );
        geode::OpenGeodeGeometryException::test(
            octree.leaf_containing( on_face ) == first + 1,
            "[Test] Wrong leaf on shared face" );
        // Domain borders
        geode::OpenGeodeGeometryException::test(
            octree.leaf_containing( uniform_point< dimension >( 0. ) )
                == second,
            "[Test] Wrong leaf on domain lower corner" );
        geode::OpenGeodeGeometryException::test(
            octree.leaf_containing( uniform_point< dimension >( 2. ) )
                == first + nb_children - 1,
            "[Test] Wrong leaf on domain upper corner" );
        auto on_border = uniform_point< dimension >( 0.2 );
        on_border.set_value( 0, 2. );
        geode::OpenGeodeGeometryException::test(
            octree.leaf_containing( on_border ) == first + 1,
            "[Test] Wrong leaf on domain upper border" );
        // Outside
        geode::OpenGeodeGeometryException::test(
            !octree.leaf_containing( uniform_point< dimension >( 2.001 ) )
                && !octree.leaf_containing(
                    uniform_point< dimension >( -0.001 ) ),
            "[Test] Outside point should have no leaf" );
        // Every leaf center is in its own leaf
        for( const auto leaf : octree.leaves() )
        {
            geode::OpenGeodeGeometryException::test(
                octree.leaf_containing( octree.cell_center( leaf ) ) == leaf,
                "[Test] Wrong leaf for leaf center ", leaf );
        }
    }

    template < geode::index_t dimension >
    void test_traverse()
    {
        geode::Logger::info( "Test traverse ", dimension, "D" );
        geode::Point< dimension > max = uniform_point< dimension >( 4. );
        max.set_value( 0, 8. );
        geode::Octree< dimension > octree{ { uniform_point< dimension >( 0. ),
            max } };
        geode::OpenGeodeGeometryException::test(
            octree.nb_root_cells()[0] == 2, "[Test] Wrong traverse roots" );
        const auto first = octree.refine( 0 );
        const auto second = octree.refine( first + 1 );
        octree.refine( second + 1 );
        const auto query = uniform_box< dimension >( 3.5, 5. );
        std::vector< geode::index_t > visited;
        octree.traverse(
            [&query]( geode::index_t /*cell*/,
                const geode::BoundingBox< dimension >& box ) {
                return box.intersects( query );
            },
            [&visited]( geode::index_t leaf ) {
                visited.push_back( leaf );
            } );
        std::vector< geode::index_t > expected;
        for( const auto leaf : octree.leaves() )
        {
            if( octree.cell_bounding_box( leaf ).intersects( query ) )
            {
                expected.push_back( leaf );
            }
        }
        auto sorted_visited = visited;
        absl::c_sort( sorted_visited );
        geode::OpenGeodeGeometryException::test(
            !expected.empty() && sorted_visited == expected,
            "[Test] Wrong traversed leaves" );

        std::vector< geode::index_t > all;
        octree.traverse(
            []( geode::index_t /*cell*/,
                const geode::BoundingBox< dimension >& /*box*/ ) {
                return true;
            },
            [&all]( geode::index_t leaf ) {
                all.push_back( leaf );
            } );
        // Depth-first, roots and children in index order
        std::vector< geode::index_t > expected_order;
        const std::function< void( geode::index_t ) > collect =
            [&]( geode::index_t cell ) {
                if( const auto first_child = octree.children( cell ) )
                {
                    for( const auto c : geode::Range{
                             geode::Octree< dimension >::NB_CHILDREN } )
                    {
                        collect( first_child.value() + c );
                    }
                    return;
                }
                expected_order.push_back( cell );
            };
        collect( 0 );
        collect( 1 );
        geode::OpenGeodeGeometryException::test(
            all == expected_order && all.back() == 1,
            "[Test] Wrong full traversal order" );
        auto sorted_all = all;
        absl::c_sort( sorted_all );
        geode::OpenGeodeGeometryException::test(
            sorted_all == octree.leaves(), "[Test] Wrong full traversal" );

        std::vector< geode::index_t > none;
        octree.traverse(
            []( geode::index_t /*cell*/,
                const geode::BoundingBox< dimension >& /*box*/ ) {
                return false;
            },
            [&none]( geode::index_t leaf ) {
                none.push_back( leaf );
            } );
        geode::OpenGeodeGeometryException::test(
            none.empty(), "[Test] Rejected roots should not be traversed" );
    }

    template < geode::index_t dimension >
    void test_max_depth()
    {
        geode::Logger::info( "Test max depth ", dimension, "D" );
        geode::Octree< dimension > octree{ uniform_box< dimension >( 0., 1. ) };
        geode::index_t cell{ 0 };
        while( octree.level( cell ) < octree.max_depth() )
        {
            cell = octree.refine( cell )
                   + geode::Octree< dimension >::NB_CHILDREN - 1;
        }
        geode::OpenGeodeGeometryException::test(
            octree.level( cell ) == expected_bits< dimension >(),
            "[Test] Wrong deepest level" );
        try
        {
            octree.refine( cell );
            geode::OpenGeodeGeometryException::test(
                false, "[Test] Refining at max depth should throw" );
        }
        catch( const geode::OpenGeodeGeometryException& )
        {
        }
        const auto box = octree.cell_bounding_box( cell );
        geode::OpenGeodeGeometryException::test(
            box.max() == uniform_point< dimension >( 1. )
                && box.min().value( 0 ) < 1.
                && octree.cell_lengths( cell )[0]
                       == std::ldexp( 1., -octree.max_depth() ),
            "[Test] Wrong deepest cell geometry" );
        geode::OpenGeodeGeometryException::test(
            octree.leaf_containing( uniform_point< dimension >( 1. ) ) == cell,
            "[Test] Wrong deepest leaf containing" );
        check_cell_consistency( octree );
    }

    template < geode::index_t dimension >
    void test_serialization()
    {
        geode::Logger::info( "Test serialization ", dimension, "D" );
        // Box whose max is not exactly origin + nb * ( side / nb )
        geode::Point< dimension > max = uniform_point< dimension >( 1.2 );
        max.set_value( 0, 0.1 + 1.1 * 3.1 );
        geode::Octree< dimension > octree{ { uniform_point< dimension >( 0.1 ),
            max } };
        geode::AttributeValues< double > values;
        values.default_value = 0;
        values.no_value = 0;
        const auto attribute_id =
            octree.cell_attribute_manager()
                .template create_attribute< geode::VariableAttribute, double >(
                    "value", values, geode::AttributeProperties{} );
        auto attribute =
            octree.cell_attribute_manager()
                .template find_attribute< geode::VariableAttribute, double >(
                    attribute_id );
        attribute->set_value( 1, 3 );
        const auto first = octree.refine( 1 );
        octree.refine( first + 2 );
        attribute->set_value( first, 9 );

        const auto filename =
            absl::StrCat( "octree", dimension, "d.og_octree" );
        {
            std::ofstream file{ filename, std::ofstream::binary };
            geode::TContext context{};
            geode::register_basic_serialize_pcontext(
                std::get< 0 >( context ) );
            geode::register_geometry_serialize_pcontext(
                std::get< 0 >( context ) );
            geode::Serializer archive{ context, file };
            archive.object( octree );
            archive.adapter().flush();
            geode::OpenGeodeGeometryException::test(
                std::get< 1 >( context ).isValid(),
                "[Test] Error while writing file" );
        }
        geode::Octree< dimension > reloaded;
        {
            std::ifstream file{ filename, std::ifstream::binary };
            geode::TContext context{};
            geode::register_basic_deserialize_pcontext(
                std::get< 0 >( context ) );
            geode::register_geometry_deserialize_pcontext(
                std::get< 0 >( context ) );
            geode::Deserializer archive{ context, file };
            archive.object( reloaded );
            const auto& adapter = archive.adapter();
            geode::OpenGeodeGeometryException::test(
                adapter.error() == bitsery::ReaderError::NoError
                    && adapter.isCompletedSuccessfully()
                    && std::get< 1 >( context ).isValid(),
                "[Test] Error while reading file" );
        }
        std::remove( filename.c_str() );
        geode::OpenGeodeGeometryException::test(
            reloaded.nb_cells() == octree.nb_cells()
                && reloaded.max_depth() == octree.max_depth()
                && reloaded.nb_root_cells() == octree.nb_root_cells()
                && reloaded.leaves() == octree.leaves(),
            "[Test] Wrong reloaded octree" );
        geode::OpenGeodeGeometryException::test(
            reloaded.domain_bounding_box().min()
                    == octree.domain_bounding_box().min()
                && reloaded.domain_bounding_box().max()
                       == octree.domain_bounding_box().max(),
            "[Test] Wrong reloaded domain" );
        geode::OpenGeodeGeometryException::test(
            reloaded.domain_bounding_box().max() == max
                && reloaded.leaf_containing( max ).has_value(),
            "[Test] Reloaded domain not snapped to bbox max" );
        for( const auto cell : geode::Range{ octree.nb_cells() } )
        {
            geode::OpenGeodeGeometryException::test(
                reloaded.level( cell ) == octree.level( cell )
                    && reloaded.cell_anchor( cell )
                           == octree.cell_anchor( cell )
                    && reloaded.parent( cell ) == octree.parent( cell )
                    && reloaded.children( cell ) == octree.children( cell )
                    && reloaded.cell_bounding_box( cell ).min()
                           == octree.cell_bounding_box( cell ).min(),
                "[Test] Wrong reloaded cell ", cell );
        }
        check_cell_consistency( reloaded );
        const auto reloaded_attribute =
            reloaded.cell_attribute_manager()
                .template find_attribute< geode::VariableAttribute, double >(
                    attribute_id );
        for( const auto cell : geode::Range{ octree.nb_cells() } )
        {
            geode::OpenGeodeGeometryException::test(
                reloaded_attribute->value( cell ) == attribute->value( cell ),
                "[Test] Wrong reloaded attribute value ", cell );
        }
        for( const auto leaf : octree.leaves() )
        {
            geode::OpenGeodeGeometryException::test(
                reloaded.leaf_containing( octree.cell_center( leaf ) ) == leaf,
                "[Test] Wrong reloaded leaf containing" );
        }
        reloaded.refine( reloaded.leaves().front() );
        geode::OpenGeodeGeometryException::test(
            reloaded.nb_cells()
                == octree.nb_cells() + geode::Octree< dimension >::NB_CHILDREN,
            "[Test] Cannot refine reloaded octree" );
        check_cell_consistency( reloaded );
    }

    template < geode::index_t dimension >
    void test_domain_snapping()
    {
        geode::Logger::info( "Test domain snapping ", dimension, "D" );
        const std::array< double, 4 > origins{ 0.1, -3.7, 1234.567, 1e-3 };
        const std::array< double, 5 > sides{ 0.3, 0.7, 1.1, 10.3, 3.33 };
        for( const auto origin : origins )
        {
            for( const auto side : sides )
            {
                for( const auto ratio : { 1., 3.1, 7.2, 10. } )
                {
                    const auto min = uniform_point< dimension >( origin );
                    auto max = uniform_point< dimension >( origin + side );
                    max.set_value( 0, origin + side * ratio );
                    const geode::Octree< dimension > octree{ { min, max } };
                    const auto domain = octree.domain_bounding_box();
                    geode::OpenGeodeGeometryException::test(
                        domain.min() == min && domain.max() == max,
                        "[Test] Domain not snapped to bbox (origin ", origin,
                        ", side ", side, ", ratio ", ratio, ")" );
                    geode::OpenGeodeGeometryException::test(
                        octree.leaf_containing( max ).has_value()
                            && octree.leaf_containing( min ).has_value(),
                        "[Test] bbox corner outside octree (origin ", origin,
                        ", side ", side, ", ratio ", ratio, ")" );
                    const auto last = octree.nb_cells() - 1;
                    geode::OpenGeodeGeometryException::test(
                        octree.cell_bounding_box( last ).max() == max
                            && octree.leaf_containing( max ) == last,
                        "[Test] Last root not snapped to bbox max" );
                }
            }
        }
    }

    template < geode::index_t dimension >
    void do_test()
    {
        test_construction< dimension >();
        test_refine< dimension >();
        test_attributes< dimension >();
        test_leaf_containing< dimension >();
        test_traverse< dimension >();
        test_max_depth< dimension >();
        test_serialization< dimension >();
        test_domain_snapping< dimension >();
    }
} // namespace

void test()
{
    geode::OpenGeodeGeometryLibrary::initialize();
    do_test< 2 >();
    do_test< 3 >();
    test_non_cubic_2d();
    test_non_cubic_3d();
}

OPENGEODE_TEST( "octree" )
