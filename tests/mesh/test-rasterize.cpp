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

#include <random>

#include <absl/container/flat_hash_set.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/coordinate_system.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/mesh/builder/regular_grid_solid_builder.hpp>
#include <geode/mesh/builder/regular_grid_surface_builder.hpp>
#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/regular_grid_solid.hpp>
#include <geode/mesh/core/regular_grid_surface.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/helpers/rasterize.hpp>
#include <geode/mesh/io/regular_grid_input.hpp>
#include <geode/mesh/io/triangulated_surface_input.hpp>

void test_rasterize_segment(
    const geode::RegularGrid3D& grid, const geode::Segment3D& segment )
{
    const auto cells = geode::rasterize_segment( grid, segment );
    const absl::flat_hash_set< geode::index_t > answer{ 111, 112, 121, 122, 131,
        132, 211, 212, 221, 222, 231, 232, 242, 243, 252, 253, 342, 343, 352,
        353, 363, 373 };

    geode::OpenGeodeMeshException::test( answer.size() == cells.size(),
        "Wrong number of result cells: ", cells.size(), " instead of ",
        answer.size() );
    for( const auto cell : cells )
    {
        geode::OpenGeodeMeshException::test(
            answer.find( grid.cell_index( cell ) ) != answer.end(),
            "Wrong result cells" );
    }
}

void test_rasterize_triangle(
    const geode::RegularGrid3D& grid, const geode::Triangle3D& triangle )
{
    const auto cells = geode::conservative_rasterize_triangle( grid, triangle );
    const absl::flat_hash_set< geode::index_t > answer{ 111, 112, 121, 122, 211,
        212, 213, 221, 222, 223, 232, 233, 242, 243, 252, 253, 313, 314, 323,
        324, 333, 334, 343, 344, 352, 353, 354, 363, 364, 373, 414, 415, 424,
        425, 434, 435, 444, 445, 454, 464, 515, 516, 525, 526, 535 };
    geode::OpenGeodeMeshException::test( answer.size() == cells.size(),
        "Wrong number of result cells: ", cells.size(), " instead of ",
        answer.size() );
    for( const auto cell : cells )
    {
        geode::OpenGeodeMeshException::test(
            answer.find( grid.cell_index( cell ) ) != answer.end(),
            "Wrong result cells" );
    }
}

void test_rasterize_degenerate_triangle(
    const geode::RegularGrid3D& grid, const geode::Triangle3D& triangle )
{
    const auto cells = geode::conservative_rasterize_triangle( grid, triangle );
    const absl::flat_hash_set< geode::index_t > answer{ 111, 112, 121, 122, 211,
        212, 221, 222, 223, 232, 233, 322, 323, 332, 333, 334, 343, 344, 433,
        434, 443, 444, 445, 454, 455, 544, 545, 554, 555, 556, 565, 566, 655,
        656, 665, 666 };
    geode::OpenGeodeMeshException::test( answer.size() == cells.size(),
        "Wrong number of result cells: ", cells.size(), " instead of ",
        answer.size() );
    for( const auto cell : cells )
    {
        geode::OpenGeodeMeshException::test(
            answer.find( grid.cell_index( cell ) ) != answer.end(),
            "Wrong result cells" );
    }
}

void test_conservative_rasterize_segment()
{
    auto grid = geode::RegularGrid2D::create();
    auto builder = geode::RegularGridBuilder2D::create( *grid );
    builder->initialize_grid( geode::Point2D{ { 0., 0. } }, { 10, 10 }, 1 );
    geode::Point2D pt0{ { 2.5, 2.5 } };
    geode::Point2D pt1{ { 6.5, 6.5 } };
    geode::Segment2D segment{ pt0, pt1 };

    const auto cells = geode::rasterize_segment( *grid, segment );
    const absl::flat_hash_set< geode::index_t > answer{ 22, 33, 44, 55, 66 };
    geode::OpenGeodeMeshException::test( answer.size() == cells.size(),
        "Wrong number of result cells (rasterize_segment): ", cells.size(),
        " instead of ", answer.size() );
    for( const auto cell : cells )
    {
        geode::OpenGeodeMeshException::test(
            answer.find( grid->cell_index( cell ) ) != answer.end(),
            "Wrong result cells (rasterize_segment)" );
    }
    const auto cells2 = geode::conservative_rasterize_segment( *grid, segment );
    const absl::flat_hash_set< geode::index_t > answer2{ 22, 23, 32, 33, 34, 43,
        44, 45, 54, 55, 56, 65, 66 };
    geode::OpenGeodeMeshException::test( answer2.size() == cells2.size(),
        "Wrong number of result cells "
        "(conservative_rasterize_segment): ",
        cells2.size(), " instead of ", answer2.size() );
    for( const auto cell : cells2 )
    {
        geode::OpenGeodeMeshException::test(
            answer2.find( grid->cell_index( cell ) ) != answer.end(),
            "Wrong result cells (conservative_rasterize_segment)" );
    }
}

void test_conservative_rasterize_triangle()
{
    auto grid = geode::RegularGrid2D::create();
    auto builder = geode::RegularGridBuilder2D::create( *grid );
    builder->initialize_grid( geode::Point2D{ { 0., 0. } }, { 10, 10 }, 1 );
    geode::Point2D pt0{ { 2.5, 2.5 } };
    geode::Point2D pt1{ { 6.5, 6.5 } };
    geode::Point2D pt2{ { 2.5, 6.5 } };
    geode::Triangle2D triangle{ pt0, pt1, pt2 };

    const auto cells =
        geode::conservative_rasterize_triangle( *grid, triangle );
    const absl::flat_hash_set< geode::index_t > answer{ 22, 23, 32, 33, 34, 42,
        43, 44, 45, 52, 53, 54, 55, 56, 62, 63, 64, 65, 66 };
    geode::OpenGeodeMeshException::test( answer.size() == cells.size(),
        "Wrong number of result cells "
        "(conservative_rasterize_triangle): ",
        cells.size(), " instead of ", answer.size() );
    for( const auto cell : cells )
    {
        geode::OpenGeodeMeshException::test(
            answer.find( grid->cell_index( cell ) ) != answer.end(),
            "Wrong result cells (conservative_rasterize_triangle)" );
    }
}

void add_cells( absl::flat_hash_set< geode::Grid3D::CellIndices >& set,
    const geode::RegularGrid3D& grid,
    const geode::Triangle3D& triangle )
{
    for( const auto& cell :
        geode::conservative_rasterize_triangle( grid, triangle ) )
    {
        set.insert( cell );
    }
}

void test_limit()
{
    auto grid = geode::RegularGrid3D::create();
    auto builder = geode::RegularGridBuilder3D::create( *grid );
    builder->initialize_grid(
        geode::Point3D{ { -1, -1, -1 } }, { 3, 3, 3 }, 1 );
    const geode::Point3D pt000{ { 0, 0, 0 } };
    const geode::Point3D pt001{ { 0, 0, 1 } };
    const geode::Point3D pt010{ { 0, 1, 0 } };
    const geode::Point3D pt011{ { 0, 1, 1 } };
    const geode::Point3D pt100{ { 1, 0, 0 } };
    const geode::Point3D pt101{ { 1, 0, 1 } };
    const geode::Point3D pt110{ { 1, 1, 0 } };
    const geode::Point3D pt111{ { 1, 1, 1 } };
    const geode::Triangle3D triangle1{ pt000, pt010, pt101 };
    const geode::Triangle3D triangle1b{ pt000, pt101, pt100 };
    const geode::Triangle3D triangle2{ pt010, pt011, pt111 };
    const geode::Triangle3D triangle2b{ pt010, pt111, pt101 };
    const geode::Triangle3D triangle3{ pt000, pt100, pt110 };
    const geode::Triangle3D triangle3b{ pt000, pt110, pt001 };
    const geode::Triangle3D triangle4{ pt001, pt110, pt111 };
    const geode::Triangle3D triangle4b{ pt001, pt111, pt011 };
    const geode::Triangle3D triangle5{ pt000, pt001, pt011 };
    const geode::Triangle3D triangle5b{ pt000, pt011, pt010 };
    const geode::Triangle3D triangle6{ pt100, pt101, pt111 };
    const geode::Triangle3D triangle6b{ pt100, pt111, pt110 };

    absl::flat_hash_set< geode::Grid3D::CellIndices > all_cells;
    add_cells( all_cells, *grid, triangle1 );
    add_cells( all_cells, *grid, triangle1b );
    add_cells( all_cells, *grid, triangle2 );
    add_cells( all_cells, *grid, triangle2b );
    add_cells( all_cells, *grid, triangle3 );
    add_cells( all_cells, *grid, triangle3b );
    add_cells( all_cells, *grid, triangle4 );
    add_cells( all_cells, *grid, triangle4b );
    add_cells( all_cells, *grid, triangle5 );
    add_cells( all_cells, *grid, triangle5b );
    add_cells( all_cells, *grid, triangle6 );
    add_cells( all_cells, *grid, triangle6b );

    geode::OpenGeodeMeshException::test( all_cells.size() == 27,
        "Wrong number of result cells (rasterize_cube): ", all_cells.size(),
        " instead of 27" );
}

namespace
{
    struct FacePlane
    {
        geode::Vector3D normal;
        geode::Point3D origin;
    };

    std::array< FacePlane, 4 > tetrahedron_planes(
        const std::array< geode::Point3D, 4 >& vertices )
    {
        std::array< FacePlane, 4 > planes;
        for( const auto f : geode::LRange{ 4 } )
        {
            const auto& facet = geode::Tetrahedron::tetrahedron_facet_vertex[f];
            const auto& a = vertices[facet[0]];
            const auto& b = vertices[facet[1]];
            const auto& c = vertices[facet[2]];
            auto normal =
                geode::Vector3D{ a, b }.cross( geode::Vector3D{ a, c } );
            if( normal.dot( geode::Vector3D{ a, vertices[f] } ) < 0 )
            {
                normal = normal * -1.;
            }
            planes[f] = { normal.normalize(), a };
        }
        return planes;
    }

    bool is_strictly_inside(
        const std::array< FacePlane, 4 >& planes, const geode::Point3D& point )
    {
        for( const auto& plane : planes )
        {
            if( plane.normal.dot( geode::Vector3D{ plane.origin, point } )
                <= 1e-6 )
            {
                return false;
            }
        }
        return true;
    }

    bool column_intersects_cell( const std::array< FacePlane, 4 >& planes,
        const geode::Grid3D::CellIndices& cell )
    {
        constexpr double TOLERANCE{ 1e-6 };
        auto x_min = -std::numeric_limits< double >::max();
        auto x_max = std::numeric_limits< double >::max();
        const auto y = cell[1] + 0.5;
        const auto z = cell[2] + 0.5;
        for( const auto& plane : planes )
        {
            const auto& normal = plane.normal;
            const auto rest =
                normal.value( 1 ) * ( y - plane.origin.value( 1 ) )
                + normal.value( 2 ) * ( z - plane.origin.value( 2 ) );
            if( std::fabs( normal.value( 0 ) ) < 1e-12 )
            {
                if( rest < -TOLERANCE )
                {
                    return false;
                }
                continue;
            }
            const auto bound =
                plane.origin.value( 0 ) - rest / normal.value( 0 );
            if( normal.value( 0 ) > 0 )
            {
                x_min = std::max( x_min, bound );
            }
            else
            {
                x_max = std::min( x_max, bound );
            }
        }
        return x_min - TOLERANCE <= cell[0] + 1.
               && x_max + TOLERANCE >= cell[0];
    }

    void check_volume_rasterization( const geode::RegularGrid3D& grid,
        const std::array< geode::Point3D, 4 >& vertices,
        absl::Span< const geode::Grid3D::CellIndices > cells,
        std::string_view name )
    {
        std::array< geode::Point3D, 4 > vertices_in_grid;
        for( const auto v : geode::LRange{ 4 } )
        {
            vertices_in_grid[v] =
                grid.grid_coordinate_system().coordinates( vertices[v] );
        }
        const auto planes = tetrahedron_planes( vertices_in_grid );
        absl::flat_hash_set< geode::Grid3D::CellIndices > result;
        for( const auto& cell : cells )
        {
            for( const auto d : geode::LRange{ 3 } )
            {
                geode::OpenGeodeMeshException::test(
                    cell[d] < grid.nb_cells_in_direction( d ), name,
                    ": cell out of the grid" );
            }
            geode::OpenGeodeMeshException::test(
                result.insert( cell ).second, name, ": duplicated cell" );
            geode::OpenGeodeMeshException::test(
                column_intersects_cell( planes, cell ), name, ": cell ",
                cell[0], " ", cell[1], " ", cell[2],
                " does not intersect the tetrahedron" );
        }
        for( const auto c : geode::Range{ grid.nb_cells() } )
        {
            const auto cell = grid.cell_indices( c );
            const geode::Point3D center{ { cell[0] + 0.5, cell[1] + 0.5,
                cell[2] + 0.5 } };
            if( is_strictly_inside( planes, center ) )
            {
                geode::OpenGeodeMeshException::test( result.contains( cell ),
                    name, ": missing cell ", cell[0], " ", cell[1], " ",
                    cell[2] );
            }
        }
    }

    void check_tetrahedron( const geode::RegularGrid3D& grid,
        const std::array< geode::Point3D, 4 >& vertices,
        std::string_view name )
    {
        const geode::Tetrahedron tetrahedron{ vertices[0], vertices[1],
            vertices[2], vertices[3] };
        check_volume_rasterization( grid, vertices,
            geode::rasterize_tetrahedron( grid, tetrahedron ),
            absl::StrCat( name, " (tetrahedron)" ) );

        auto surface = geode::TriangulatedSurface3D::create();
        auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
        for( const auto& vertex : vertices )
        {
            builder->create_point( vertex );
        }
        for( const auto& facet : geode::Tetrahedron::tetrahedron_facet_vertex )
        {
            builder->create_triangle( facet );
        }
        check_volume_rasterization( grid, vertices,
            geode::rasterize_closed_surface( grid, *surface ),
            absl::StrCat( name, " (closed surface)" ) );
    }

    std::array< geode::Point3D, 4 > random_tetrahedron(
        const geode::RegularGrid3D& grid,
        std::mt19937& generator,
        double min_coordinate,
        double max_coordinate )
    {
        std::uniform_real_distribution< double > distribution{ min_coordinate,
            max_coordinate };
        std::array< geode::Point3D, 4 > vertices;
        for( auto& vertex : vertices )
        {
            vertex = grid.grid_coordinate_system().global_coordinates(
                geode::Point3D{ { distribution( generator ),
                    distribution( generator ), distribution( generator ) } } );
        }
        return vertices;
    }

    void test_rasterize_tetrahedron_axis_aligned()
    {
        geode::Logger::info( "Test rasterize tetrahedron axis aligned" );
        auto grid = geode::RegularGrid3D::create();
        auto builder = geode::RegularGridBuilder3D::create( *grid );
        builder->initialize_grid(
            geode::Point3D{ { 0., 0., 0. } }, { 10, 10, 10 }, 1 );
        std::mt19937 generator{ 42 };
        for( const auto trial : geode::Range{ 20 } )
        {
            check_tetrahedron( *grid,
                random_tetrahedron( *grid, generator, 0.1, 9.9 ),
                absl::StrCat( "Axis aligned ", trial ) );
        }
    }

    void test_rasterize_tetrahedron_boundary()
    {
        geode::Logger::info( "Test rasterize tetrahedron on grid boundary" );
        auto grid = geode::RegularGrid3D::create();
        auto builder = geode::RegularGridBuilder3D::create( *grid );
        builder->initialize_grid(
            geode::Point3D{ { 0., 0., 0. } }, { 10, 10, 10 }, 1 );
        check_tetrahedron( *grid,
            { geode::Point3D{ { -1e-16, 2.5, 2.5 } },
                geode::Point3D{ { 3.2, 1.1, 1.3 } },
                geode::Point3D{ { 3.1, 4.7, 1.6 } },
                geode::Point3D{ { 2.9, 2.6, 4.8 } } },
            "Vertex on grid min" );
        check_tetrahedron( *grid,
            { geode::Point3D{ { 2.5, 2.5, -0.8 } },
                geode::Point3D{ { 6.2, 1.1, 1.3 } },
                geode::Point3D{ { 3.1, 7.7, 1.6 } },
                geode::Point3D{ { 2.9, 2.6, 4.8 } } },
            "Below grid min" );
        check_tetrahedron( *grid,
            { geode::Point3D{ { 2.5, 2.5, 2.5 } },
                geode::Point3D{ { 6.2, 11.7, 1.3 } },
                geode::Point3D{ { 3.1, 7.7, 1.6 } },
                geode::Point3D{ { 12.9, 2.6, 4.8 } } },
            "Beyond grid max" );
    }

} // namespace

void test()
{
    geode::OpenGeodeMeshLibrary::initialize();
    auto grid = geode::RegularGrid3D::create();
    auto builder = geode::RegularGridBuilder3D::create( *grid );
    builder->initialize_grid(
        geode::Point3D{ { 0., 0., 0. } }, { 10, 10, 10 }, 1 );
    geode::Point3D pt0{ { 2, 2, 2 } };
    geode::Point3D pt1{ { 3.55, 7.55, 3.55 } };
    geode::Point3D pt2{ { 6.55, 1.55, 5.55 } };
    geode::Point3D pt3{ { 6, 6, 6 } };
    geode::Point3D pt4{ { 4, 4, 4 } };

    test_rasterize_segment( *grid, geode::Segment3D{ pt0, pt1 } );
    test_conservative_rasterize_segment();
    test_conservative_rasterize_triangle();
    test_rasterize_triangle( *grid, geode::Triangle3D{ pt0, pt1, pt2 } );
    test_rasterize_degenerate_triangle(
        *grid, geode::Triangle3D{ pt0, pt3, pt4 } );

    test_limit();
    test_rasterize_tetrahedron_axis_aligned();
    test_rasterize_tetrahedron_boundary();
}

OPENGEODE_TEST( "rasterize" )