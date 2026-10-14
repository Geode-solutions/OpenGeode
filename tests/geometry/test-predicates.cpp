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
#include <random>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/internal/predicates.hpp>
#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

#include <geode/tests/common.hpp>

namespace
{
    // Smallest representable increment above value
    double next( double value )
    {
        return std::nextafter( value, 2 * value + 1 );
    }

    void test_orient_2d()
    {
        const geode::Point2D a{ { 0, 0 } };
        const geode::Point2D b{ { 1, 0 } };
        const geode::Point2D c{ { 0, 1 } };
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::orient_2d( a, b, c ) == GEO::positive,
            "[Test] Wrong orient_2d on generic points" );
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::orient_2d( a, c, b ) == GEO::negative,
            "[Test] Wrong orient_2d on generic points (inverted)" );

        // Decided by the zero-filter
        const geode::Point2D x1{ { 5, 0 } };
        const geode::Point2D x2{ { -3, 0 } };
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::orient_2d( a, x1, x2 ) == GEO::zero,
            "[Test] Wrong orient_2d on points with same y" );
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::orient_2d( a, a, c ) == GEO::zero,
            "[Test] Wrong orient_2d on duplicated points" );

        // Decided by the exact arithmetic
        const geode::Point2D d1{ { 0.1, 0.3 } };
        const geode::Point2D d2{ { 0.2, 0.6 } };
        const geode::Point2D d3{ { 0.4, 1.2 } };
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::orient_2d( d1, d2, d3 )
                == GEO::PCK::orient_2d( d2, d3, d1 ),
            "[Test] Wrong orient_2d consistency on nearly collinear points" );
        const geode::Point2D e1{ { 1, 1 } };
        const geode::Point2D e2{ { 2, 2 } };
        const geode::Point2D e3{ { 3, next( 3 ) } };
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::orient_2d( e1, e2, geode::Point2D{ { 3, 3 } } )
                == GEO::zero,
            "[Test] Wrong orient_2d on collinear points" );
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::orient_2d( e1, e2, e3 ) == GEO::positive,
            "[Test] Wrong orient_2d on nearly collinear points" );
    }

    void test_orient_3d()
    {
        const geode::Point3D a{ { 0, 0, 0 } };
        const geode::Point3D b{ { 1, 0, 0 } };
        const geode::Point3D c{ { 0, 1, 0 } };
        const geode::Point3D d{ { 0, 0, 1 } };
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::orient_3d( a, b, c, d ) != GEO::zero,
            "[Test] Wrong orient_3d on generic points" );
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::orient_3d( a, b, c, d )
                == -GEO::PCK::orient_3d( a, c, b, d ),
            "[Test] Wrong orient_3d on generic points (inverted)" );

        // Decided by the zero-filter
        const geode::Point3D z1{ { 12, 7, 0 } };
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::orient_3d( a, b, c, z1 ) == GEO::zero,
            "[Test] Wrong orient_3d on points with same z" );
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::orient_3d( a, b, b, d ) == GEO::zero,
            "[Test] Wrong orient_3d on duplicated points" );
        const geode::Point3D y1{ { 3, 0, 5 } };
        const geode::Point3D y2{ { -2, 0, 1 } };
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::orient_3d( a, b, y1, y2 ) == GEO::zero,
            "[Test] Wrong orient_3d on points with same y" );

        // Decided by the exact arithmetic
        const geode::Point3D p0{ { 1, 1, 1 } };
        const geode::Point3D p1{ { 2, 3, 4 } };
        const geode::Point3D p2{ { 4, 1, 2 } };
        const geode::Point3D coplanar{ { 5, 3, 5 } };
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::orient_3d( p0, p1, p2, coplanar ) == GEO::zero,
            "[Test] Wrong orient_3d on coplanar points" );
        const geode::Point3D above{ { 5, 3, next( 5 ) } };
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::orient_3d( p0, p1, p2, above )
                == -GEO::PCK::orient_3d(
                    p0, p1, p2, geode::Point3D{ { 5, 3, 5 - 1e-15 } } ),
            "[Test] Wrong orient_3d on nearly coplanar points" );
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::orient_3d( p0, p1, p2, above ) != GEO::zero,
            "[Test] Wrong orient_3d on nearly coplanar points" );
    }

    void test_det_3d()
    {
        const geode::Vector3D x{ { 1, 0, 0 } };
        const geode::Vector3D y{ { 0, 1, 0 } };
        const geode::Vector3D z{ { 0, 0, 1 } };
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::det_3d( x, y, z ) == GEO::positive,
            "[Test] Wrong det_3d on generic vectors" );

        // Decided by the zero-filter
        const geode::Vector3D xy{ { 3, 5, 0 } };
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::det_3d( x, y, xy ) == GEO::zero,
            "[Test] Wrong det_3d on vectors in plane z=0" );
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::det_3d( x, geode::Vector3D{ { 0, 0, 0 } }, z )
                == GEO::zero,
            "[Test] Wrong det_3d with null vector" );

        // Decided by the exact arithmetic
        const geode::Vector3D u{ { 1, 2, 3 } };
        const geode::Vector3D v{ { 4, 5, 6 } };
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::det_3d( u, v, geode::Vector3D{ { 7, 8, 9 } } )
                == GEO::zero,
            "[Test] Wrong det_3d on coplanar vectors" );
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::det_3d( u, v, geode::Vector3D{ { 7, 8, next( 9 ) } } )
                == GEO::negative,
            "[Test] Wrong det_3d on nearly coplanar vectors" );
    }

    void test_aligned_3d()
    {
        const geode::Point3D a{ { 0, 0, 0 } };
        const geode::Point3D b{ { 1, 0, 0 } };
        const geode::Point3D c{ { 0, 1, 0 } };
        geode::OpenGeodeGeometryException::test(
            !GEO::PCK::aligned_3d( a, b, c ),
            "[Test] Wrong aligned_3d on generic points" );
        geode::OpenGeodeGeometryException::test(
            !GEO::PCK::aligned_3d( a, geode::Point3D{ { 1, 1, 0 } },
                geode::Point3D{ { 2, 2, 1 } } ),
            "[Test] Wrong aligned_3d on points aligned in projection" );

        // Decided by the zero-filter
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::aligned_3d( a, b, geode::Point3D{ { -7, 0, 0 } } ),
            "[Test] Wrong aligned_3d on points along x axis" );
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::aligned_3d( a, a, c ),
            "[Test] Wrong aligned_3d on duplicated points" );

        // Decided by the exact arithmetic
        const geode::Point3D p0{ { 1, 2, 3 } };
        const geode::Point3D p1{ { 2, 4, 6 } };
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::aligned_3d( p0, p1, geode::Point3D{ { 4, 8, 12 } } ),
            "[Test] Wrong aligned_3d on aligned points" );
        geode::OpenGeodeGeometryException::test(
            !GEO::PCK::aligned_3d(
                p0, p1, geode::Point3D{ { 4, 8, next( 12 ) } } ),
            "[Test] Wrong aligned_3d on nearly aligned points" );
        // Too small for the floating-point filter, aligned in projection
        geode::OpenGeodeGeometryException::test(
            !GEO::PCK::aligned_3d( a, b, geode::Point3D{ { 2, 0, 1e-300 } } ),
            "[Test] Wrong aligned_3d on nearly aligned tiny points" );
    }

    void test_dot_3d()
    {
        const geode::Point3D a{ { 0, 0, 0 } };
        const geode::Point3D b{ { 1, 2, 3 } };
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::dot_3d( a, b, geode::Point3D{ { 1, 1, 1 } } )
                == GEO::positive,
            "[Test] Wrong dot_3d on acute angle" );
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::dot_3d( a, b, geode::Point3D{ { -1, -1, -1 } } )
                == GEO::negative,
            "[Test] Wrong dot_3d on obtuse angle" );

        // Decided by the zero-filter
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::dot_3d( a, geode::Point3D{ { 1, 0, 0 } },
                geode::Point3D{ { 0, 5, 2 } } )
                == GEO::zero,
            "[Test] Wrong dot_3d on orthogonal axes" );

        // Decided by the exact arithmetic
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::dot_3d( a, b, geode::Point3D{ { 1, 1, -1 } } )
                == GEO::zero,
            "[Test] Wrong dot_3d on orthogonal vectors" );
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::dot_3d( a, b, geode::Point3D{ { next( 1 ), 1, -1 } } )
                == GEO::positive,
            "[Test] Wrong dot_3d on nearly orthogonal vectors" );
    }

    void test_dot_2d()
    {
        const geode::Point2D a{ { 0, 0 } };
        const geode::Point2D b{ { 1, 2 } };
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::dot_2d( a, b, geode::Point2D{ { 1, 1 } } )
                == GEO::positive,
            "[Test] Wrong dot_2d on acute angle" );
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::dot_2d( a, b, geode::Point2D{ { -1, -1 } } )
                == GEO::negative,
            "[Test] Wrong dot_2d on obtuse angle" );

        // Decided by the zero-filter
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::dot_2d(
                a, geode::Point2D{ { 3, 0 } }, geode::Point2D{ { 0, -4 } } )
                == GEO::zero,
            "[Test] Wrong dot_2d on orthogonal axes" );

        // Decided by the exact arithmetic
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::dot_2d( a, b, geode::Point2D{ { -2, 1 } } ) == GEO::zero,
            "[Test] Wrong dot_2d on orthogonal vectors" );
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::dot_2d( a, b, geode::Point2D{ { -2, next( 1 ) } } )
                == GEO::positive,
            "[Test] Wrong dot_2d on nearly orthogonal vectors" );
    }

    // Exactly degenerate, without shared coordinates: decided once the
    // coordinate differences are known to be exact.
    void test_exact_degenerate()
    {
        const geode::Point3D a{ { 1, 2, 5 } };
        const geode::Point3D b{ { 4, 1, 6 } };
        const geode::Point3D c{ { 2, 7, 16 } };
        const geode::Point3D d{ { 9, 3, 15 } };
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::orient_3d( a, b, c, d ) == GEO::zero,
            "[Test] Wrong orient_3d on points of plane z = x + 2y" );
        const geode::Point2D e{ { 1, 3 } };
        const geode::Point2D f{ { 4, 12 } };
        const geode::Point2D g{ { -7, -21 } };
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::orient_2d( e, f, g ) == GEO::zero,
            "[Test] Wrong orient_2d on points of line y = 3x" );

        // Same configurations, but the coordinate differences with the tiny
        // first point are not representable: rounded differences give a non
        // zero determinant.
        const auto tiny = std::ldexp( 1., -60 );
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::orient_3d( geode::Point3D{ { 3 * tiny, tiny, 5 * tiny } },
                geode::Point3D{ { 1000.5, 3.25, 1007 } },
                geode::Point3D{ { -20.75, 100.5, 180.25 } },
                geode::Point3D{ { 7, 9, 25 } } )
                == GEO::zero,
            "[Test] Wrong orient_3d on points of plane z = x + 2y with "
            "inexact differences" );
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::orient_2d( geode::Point2D{ { tiny, 3 * tiny } },
                geode::Point2D{ { 1000.5, 3001.5 } },
                geode::Point2D{ { -20.25, -60.75 } } )
                == GEO::zero,
            "[Test] Wrong orient_2d on points of line y = 3x with inexact "
            "differences" );

        // Nearly collinear points, exactly coplanar: requires all the
        // roundoff error terms of the coordinate differences, including
        // the products of three of them.
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::orient_3d(
                geode::Point3D{ { 0x1.47d6541ade7cdp+6, 0x1.896798203e2f6p+4,
                    0x1.caf8dc259de1ep+5 } },
                geode::Point3D{ { -0x1.73ff1093c559p+9, -0x1.be654717b99ep+7,
                    -0x1.0465becdd6f18p+9 } },
                geode::Point3D{ { 0x1.22acf2099e883p+9, 0x1.5ccf88d857d6ap+7,
                    0x1.96f21fa711251p+8 } },
                geode::Point3D{ { 0x1.0dfd21cd4d26ap+3, 0x1.43fc8ef65c94cp+1,
                    0x1.79fbfc1f6c02fp+2 } } )
                == GEO::zero,
            "[Test] Wrong orient_3d on nearly collinear coplanar points" );
    }

    // Nearly degenerate configurations, decided by different evaluation
    // stages: the sign must not depend on the order of the points.
    void test_nearly_degenerate_consistency()
    {
        std::mt19937 generator{ 42 };
        std::uniform_real_distribution< double > distribution{ 0, 1000 };
        const auto random = [&] {
            return distribution( generator );
        };
        for( const auto offset : { 0., 5e5 } )
        {
            for( const auto trial : geode::Range{ 10000 } )
            {
                geode_unused( trial );
                std::array< geode::Point3D, 4 > p3;
                for( auto& point : p3 )
                {
                    const auto x = offset + random();
                    const auto y = offset + random();
                    point = geode::Point3D{ { x, y, 0.3 * x + 0.7 * y } };
                }
                const auto sign =
                    GEO::PCK::orient_3d( p3[0], p3[1], p3[2], p3[3] );
                geode::OpenGeodeGeometryException::test(
                    GEO::PCK::orient_3d( p3[1], p3[0], p3[3], p3[2] ) == sign
                        && GEO::PCK::orient_3d( p3[2], p3[3], p3[0], p3[1] )
                               == sign
                        && GEO::PCK::orient_3d( p3[1], p3[0], p3[2], p3[3] )
                               == -sign
                        && GEO::PCK::orient_3d( p3[3], p3[1], p3[2], p3[0] )
                               == -sign,
                    "[Test] Inconsistent orient_3d on nearly coplanar points" );

                std::array< geode::Point2D, 3 > p2;
                for( auto& point : p2 )
                {
                    const auto t = random();
                    point = geode::Point2D{ { offset + t, offset + 0.3 * t } };
                }
                const auto sign2 = GEO::PCK::orient_2d( p2[0], p2[1], p2[2] );
                geode::OpenGeodeGeometryException::test(
                    GEO::PCK::orient_2d( p2[1], p2[2], p2[0] ) == sign2
                        && GEO::PCK::orient_2d( p2[2], p2[0], p2[1] ) == sign2
                        && GEO::PCK::orient_2d( p2[1], p2[0], p2[2] ) == -sign2,
                    "[Test] Inconsistent orient_2d on nearly collinear "
                    "points" );
            }
        }
    }
} // namespace

void test()
{
    test_orient_2d();
    test_orient_3d();
    test_det_3d();
    test_aligned_3d();
    test_dot_3d();
    test_dot_2d();
    test_exact_degenerate();
    test_nearly_degenerate_consistency();

    geode::Logger::info( "TEST SUCCESS" );
}

OPENGEODE_TEST( "predicates" )
