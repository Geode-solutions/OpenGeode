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

#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <random>
#include <vector>

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
        // Too small for the floating-point filter, many zero coordinates but
        // non zero determinant: the zero-filter must not conclude.
        const auto tiny = std::ldexp( 1., -330 );
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::det_3d( geode::Vector3D{ { 0, tiny, 0 } },
                geode::Vector3D{ { 0, 0, tiny } },
                geode::Vector3D{ { tiny, 0, 0 } } )
                == GEO::positive,
            "[Test] Wrong det_3d on tiny permutation vectors" );
        geode::OpenGeodeGeometryException::test(
            GEO::PCK::det_3d( geode::Vector3D{ { tiny, 0, 0 } },
                geode::Vector3D{ { 0, 0, tiny } },
                geode::Vector3D{ { 0, tiny, 0 } } )
                == GEO::negative,
            "[Test] Wrong det_3d on tiny permutation vectors (odd)" );

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

    // Exact reference on integer coordinates. The predicates are evaluated
    // on scale * (offset + n) with n integers of absolute value lower than
    // 2^15: the coordinate differences are exact, and the reference 3x3
    // determinants of differences of n are lower than 2^54.
    class IntegerConfigurations
    {
    public:
        std::int64_t random_integer( std::int64_t min, std::int64_t max )
        {
            return std::uniform_int_distribution< std::int64_t >{ min, max }(
                generator_ );
        }

        // Random coordinates, or degenerate configurations:
        // all points on the plane (or line) passing through the first one
        // and spanned by two (or one) random directions, possibly moved by
        // one unit, sharing coordinates or duplicated.
        std::vector< std::array< std::int64_t, 3 > > points(
            geode::index_t nb_points, geode::index_t nb_directions )
        {
            std::vector< std::array< std::int64_t, 3 > > result( nb_points );
            const auto type = random_integer( 0, 4 );
            for( auto& point : result )
            {
                for( auto& value : point )
                {
                    value = random_integer( -10000, 10000 );
                }
            }
            if( type == 0 )
            {
                return result;
            }
            std::array< std::array< std::int64_t, 3 >, 2 > directions;
            for( auto& direction : directions )
            {
                for( auto& value : direction )
                {
                    value = random_integer( -60, 60 );
                }
            }
            for( const auto p : geode::Range{ 1, nb_points } )
            {
                for( const auto d : geode::LRange{ 3 } )
                {
                    result[p][d] = result[0][d];
                    for( const auto i : geode::Range{ nb_directions } )
                    {
                        result[p][d] +=
                            random_integer( -60, 60 ) * directions[i][d];
                    }
                }
            }
            if( type == 2 )
            {
                result.back()[random_integer( 0, 2 )] +=
                    random_integer( 0, 1 ) == 0 ? -1 : 1;
            }
            else if( type == 3 )
            {
                const auto d = random_integer( 0, 2 );
                for( auto& point : result )
                {
                    point[d] = result[0][d];
                }
            }
            else if( type == 4 )
            {
                result.back() = result[random_integer( 0, nb_points - 2 )];
            }
            return result;
        }

        // Frame 0 has usual scales. Frames 1 and 2 have scales so small that
        // the floating-point filters of the degree 3 and degree 2 predicates
        // are uncertain (underflow guard), but large enough to avoid
        // underflow in the exact evaluations.
        void new_frame( geode::index_t frame )
        {
            static constexpr std::array< std::array< int, 2 >, 3 > EXPONENTS{
                { { -40, 40 }, { -335, -326 }, { -505, -490 } }
            };
            scale_ = std::ldexp(
                1., static_cast< int >( random_integer(
                        EXPONENTS[frame][0], EXPONENTS[frame][1] ) ) );
            for( auto& value : offset_ )
            {
                value = random_integer( 0, 1 ) == 0
                            ? 0
                            : random_integer( -( 1 << 30 ), 1 << 30 );
            }
        }

        double coordinate( std::int64_t value, geode::local_index_t d ) const
        {
            return scale_ * static_cast< double >( offset_[d] + value );
        }

        geode::Point3D point3d(
            const std::array< std::int64_t, 3 >& point ) const
        {
            return geode::Point3D{ { coordinate( point[0], 0 ),
                coordinate( point[1], 1 ), coordinate( point[2], 2 ) } };
        }

        geode::Point2D point2d(
            const std::array< std::int64_t, 3 >& point ) const
        {
            return geode::Point2D{ { coordinate( point[0], 0 ),
                coordinate( point[1], 1 ) } };
        }

        geode::Vector3D vector3d(
            const std::array< std::int64_t, 3 >& vector ) const
        {
            return geode::Vector3D{ { scale_
                                          * static_cast< double >( vector[0] ),
                scale_ * static_cast< double >( vector[1] ),
                scale_ * static_cast< double >( vector[2] ) } };
        }

    private:
        std::mt19937_64 generator_{ 42 };
        double scale_{ 1 };
        std::array< std::int64_t, 3 > offset_{};
    };

    int sign( std::int64_t value )
    {
        return ( value > 0 ) - ( value < 0 );
    }

    std::array< std::int64_t, 3 > difference(
        const std::array< std::int64_t, 3 >& a,
        const std::array< std::int64_t, 3 >& b )
    {
        return { a[0] - b[0], a[1] - b[1], a[2] - b[2] };
    }

    std::int64_t determinant( const std::array< std::int64_t, 3 >& a,
        const std::array< std::int64_t, 3 >& b,
        const std::array< std::int64_t, 3 >& c )
    {
        return a[0] * ( b[1] * c[2] - b[2] * c[1] )
               - a[1] * ( b[0] * c[2] - b[2] * c[0] )
               + a[2] * ( b[0] * c[1] - b[1] * c[0] );
    }

    void test_integer_configurations()
    {
        IntegerConfigurations configurations;
        for( const auto trial : geode::Range{ 20000 } )
        {
            const auto frame = trial % 3;
            configurations.new_frame( frame );

            const auto p4 = configurations.points( 4, 2 );
            if( frame != 2 )
            {
                const auto v1 = difference( p4[1], p4[0] );
                const auto v2 = difference( p4[2], p4[0] );
                const auto v3 = difference( p4[3], p4[0] );
                const auto orient_3d = sign( determinant( v1, v2, v3 ) );
                geode::OpenGeodeGeometryException::test(
                    GEO::PCK::orient_3d( configurations.point3d( p4[0] ),
                        configurations.point3d( p4[1] ),
                        configurations.point3d( p4[2] ),
                        configurations.point3d( p4[3] ) )
                        == orient_3d,
                    "[Test] Wrong orient_3d on integer configuration" );
                geode::OpenGeodeGeometryException::test(
                    GEO::PCK::det_3d( configurations.vector3d( v1 ),
                        configurations.vector3d( v2 ),
                        configurations.vector3d( v3 ) )
                        == orient_3d,
                    "[Test] Wrong det_3d on integer configuration" );
            }

            const auto p3 = configurations.points( 3, 1 );
            const auto u = difference( p3[1], p3[0] );
            const auto v = difference( p3[2], p3[0] );
            const auto orient_2d = sign( u[0] * v[1] - u[1] * v[0] );
            geode::OpenGeodeGeometryException::test(
                GEO::PCK::orient_2d( configurations.point2d( p3[0] ),
                    configurations.point2d( p3[1] ),
                    configurations.point2d( p3[2] ) )
                    == orient_2d,
                "[Test] Wrong orient_2d on integer configuration" );
            const auto aligned = u[1] * v[2] - u[2] * v[1] == 0
                                 && u[2] * v[0] - u[0] * v[2] == 0
                                 && orient_2d == 0;
            geode::OpenGeodeGeometryException::test(
                GEO::PCK::aligned_3d( configurations.point3d( p3[0] ),
                    configurations.point3d( p3[1] ),
                    configurations.point3d( p3[2] ) )
                    == aligned,
                "[Test] Wrong aligned_3d on integer configuration" );

            // Second point orthogonal to the first one around the origin
            auto orthogonal = p3;
            const auto w = difference( orthogonal[2], orthogonal[0] );
            const std::array< std::int64_t, 3 > normal{ u[1] * w[2]
                                                            - u[2] * w[1],
                u[2] * w[0] - u[0] * w[2], u[0] * w[1] - u[1] * w[0] };
            if( configurations.random_integer( 0, 1 ) == 0
                && std::abs( normal[0] ) < 10000
                && std::abs( normal[1] ) < 10000
                && std::abs( normal[2] ) < 10000 )
            {
                for( const auto d : geode::LRange{ 3 } )
                {
                    orthogonal[2][d] = orthogonal[0][d] + normal[d];
                }
            }
            const auto a = difference( orthogonal[1], orthogonal[0] );
            const auto b = difference( orthogonal[2], orthogonal[0] );
            geode::OpenGeodeGeometryException::test(
                GEO::PCK::dot_3d( configurations.point3d( orthogonal[0] ),
                    configurations.point3d( orthogonal[1] ),
                    configurations.point3d( orthogonal[2] ) )
                    == sign( a[0] * b[0] + a[1] * b[1] + a[2] * b[2] ),
                "[Test] Wrong dot_3d on integer configuration" );
            auto orthogonal_2d = p3;
            if( configurations.random_integer( 0, 1 ) == 0 )
            {
                orthogonal_2d[2][0] = orthogonal_2d[0][0] - u[1];
                orthogonal_2d[2][1] = orthogonal_2d[0][1] + u[0];
            }
            const auto c = difference( orthogonal_2d[1], orthogonal_2d[0] );
            const auto e = difference( orthogonal_2d[2], orthogonal_2d[0] );
            geode::OpenGeodeGeometryException::test(
                GEO::PCK::dot_2d( configurations.point2d( orthogonal_2d[0] ),
                    configurations.point2d( orthogonal_2d[1] ),
                    configurations.point2d( orthogonal_2d[2] ) )
                    == sign( c[0] * e[0] + c[1] * e[1] ),
                "[Test] Wrong dot_2d on integer configuration" );
        }
    }

    struct RegressionCase
    {
        const char* stage;
        int expected;
        std::vector< double > values;
    };

    // Configurations decided by each evaluation stage of the predicates:
    // floating-point filter, zero-filter, adaptive stages B, C and D for the
    // orientations, exact evaluation for the others. Generated from random
    // coordinates of mixed magnitudes (without subnormal numbers, as
    // expansion arithmetic is not exact on underflow), nearly or exactly
    // degenerate, including configurations where a plain floating-point
    // evaluation gives a wrong sign. Expected signs computed with exact
    // rational arithmetic.
    const std::vector< RegressionCase > ORIENT_2D_CASES{
        { "filter", -1,
            { 0x1.58p-29, 0x1.4afdc553fdedfp-28, -0x1.28p+5,
                0x1.4afdc553fdedfp-28, -0x1.6cp+6, 0x1.2decf20fde86p-8 } },
        { "filter", -1,
            { 0x1.6f5c50890241p-31, -0x1.8000000000001p+6, 0x1.15f0feccb4fep-40,
                -0x1.8p+6, 0x1.18p-35, -0x1.7ffffffffffffp+6 } },
        { "filter", -1,
            { 0x1.8f9008ec5f9c1p-8, 0x1.76dc6863deebdp+11, 0x1.fbp-23,
                -0x1.b071dc6739e8ap+5, 0x1.8f9008ec5f9c1p-8,
                0x1.6ec3de33860ffp+22 } },
        { "filter", 1,
            { 0x1.e81af5dfee914p+18, 0x1.f75c698eee05dp+23,
                -0x1.871ab4cf3e2b3p+12, -0x1.932c8a75b81ccp+17,
                0x1.f75c698eee05dp+23, 0x1.37ffffffffffp+6 } },
        { "filter", 1,
            { -0x1.067ffffe6b8p+11, -0x1.067ffffe6b8p+11, -0x1.4p+6,
                -0x1.a3fffffcd7p+10, 0x1.e7a9ba24781bdp+18,
                0x1.40076227eeeb8p+23 } },
        { "filter", 1,
            { -0x1.9dp-16, 0x1.e8a9622516bf1p+18, 0x1.e8a9622516bf1p+18,
                0x1.d177895166469p+37, 0x1.e8ea1adfe7b4p+18,
                0x1.d2f584be6e0abp+37 } },
        { "zero-filter", 0,
            { -0x1.1p+2, -0x1.2p+5, 0x1.e84858a783c8ap+18, -0x1.2p+5,
                0x1.e7f869e36cd89p+18, -0x1.2p+5 } },
        { "zero-filter", 0,
            { 0x1.e873e5e06bea9p+18, 0x1.600000000001ep+6,
                0x1.e8269855e5f38p+18, 0x1.600000000001ep+6,
                0x1.e83ed7c21a0fdp+18, 0x1.600000000001ep+6 } },
        { "zero-filter", 0,
            { 0x1.e92bc3e4bf7e8p+18, 0x1.9d58cb98b31c2p-8,
                0x1.e86f3ce2e6fd9p+18, 0x1.9d58cb98b31c2p-8, -0x1.e8p-23,
                0x1.9d58cb98b31c2p-8 } },
        { "stage B", -1,
            { 0x1.e8a3087ab5437p+18, -0x1.44cf555e4a497p-15, 0x1.2dp-56,
                -0x1.44cfb0a13d4ebp-15, -0x1.3cp-24, -0x1.44cfb0a13d4ebp-15 } },
        { "stage B", -1,
            { -0x1.93e0393288503p+14, 0x1.c000000024492p+5,
                -0x1.4055d881f63eep+9, 0x1.c000000000e66p+5,
                0x1.e7ea91809638ap+18, 0x1.bffffffd429edp+5 } },
        { "stage B", -1,
            { 0x1.e7e8c99e8d15cp+18, 0x1.d171f803fa15cp+37,
                0x1.e75a1728bb73ep+18, 0x1.d0e9d78bf4822p+37, 0x1p+6,
                0x1.f010d5bd8523fp+24 } },
        { "stage B", 1,
            { -0x1.2000000000001p+5, -0x1.2000000000001p+5,
                -0x1.3d9c8945b5a58p-16, -0x1.2p+5, -0x1.6cp+6, -0x1.2p+5 } },
        { "stage B", 1,
            { 0x1.e7ef323939776p+18, 0x1.d18c32ae0e7e4p+37,
                0x1.e78fa7b892f4ep+18, 0x1.d1310a5900be2p+37,
                -0x1.0d7168262599cp+3, -0x1.0114afd61c6dcp+22 } },
        { "stage B", 1,
            { 0x1.44p+6, 0x1.c00000000004ep+4, 0x1.0f8ee117e7b68p-4, 0x1.cp+4,
                0x1.e7c084ee78a88p+18, 0x1.c000000074d15p+4 } },
        { "stage C exact differences", -1,
            { 0x1.cp+3, -0x1.094p+10, 0x1.8p+6, -0x1.c2bp+12, 0x1.54p+6,
                -0x1.8f20000000002p+12 } },
        { "stage C exact differences", -1,
            { 0x1.74p+6, -0x1.d5b3177ea7faap+7, -0x1.ep+5, 0x1.a57bcd17e83eap+7,
                0x1.88p+6, -0x1.f2d2bdeb50aa8p+7 } },
        { "stage C exact differences", -1,
            { 0x1.38p+6, -0x1.1abffff434p+12, 0x1.e7d3d45a629b6p+18,
                -0x1.ba17f871e89ffp+24, 0x1.e8e5c742fd948p+18,
                -0x1.bb103c94b511dp+24 } },
        { "stage C exact differences", 0,
            { -0x1.ap+5, -0x1.fe2p+11, -0x1.cp+2, -0x1.1d8p+9, -0x1.308p-5,
                -0x1.be638p+4 } },
        { "stage C exact differences", 0,
            { -0x1.6p+5, 0x1.5p+4, 0x1.e85bee3a1b6abp+18, 0x1.e86c2e3a1b6abp+18,
                0x1.078ea0b379fe9p+12, 0x1.0b9ea0b379fe9p+12 } },
        { "stage C exact differences", 0,
            { 0x1.6p+6, 0x1.ee214184c01aep+18, 0x1.5p+4, 0x1.e9390184c01aep+18,
                -0x1.7p-29, 0x1.e7af4184bf434p+18 } },
        { "stage C exact differences", 1,
            { 0x1.e8dfecdb9bf94p+18, -0x1.67049f71468b1p+25, 0x1.ap+5,
                -0x1.373p+12, -0x1.e8p+5, 0x1.60bp+12 } },
        { "stage C exact differences", 1,
            { 0x1.78p+6, 0x1.751000224p-33, 0x1.e775245d490efp+18,
                0x1.e3a63a1490a0dp-21, -0x1.36a3e5965774cp+13,
                -0x1.34369dcae645dp-26 } },
        { "stage C exact differences", 1,
            { -0x1.9f8p-6, 0x1.8de8p+3, 0x1.e7eed47c90e36p+18,
                -0x1.6df31a5d6caa8p+25, -0x1.8cp+6, 0x1.295p+13 } },
        { "stage C", -1,
            { -0x1.0fffffffffffep+5, -0x1.039f11da6df74p+24,
                0x1.18001b6803aadp-59, 0x1.0b41f74dcd001p-40, 0x1.1a8p-15,
                0x1.0da4bd5daf634p+4 } },
        { "stage C", -1,
            { 0x1.b8p-10, 0x1.eeffffc1ep-7, 0x1.4d0d7f1983b18p-36,
                0x1.f8bcbbf2d09ecp-35, 0x1.e74ffd686e29ep+18,
                0x1.121cfe8abdf79p+22 } },
        { "stage C", -1,
            { -0x1.0df3058767eecp-60, -0x1.d800b99713cd1p-39,
                0x1.e856cc9387412p+18, 0x1.4fbbaca56cfccp+23,
                -0x1.05533215806ffp-27, -0x1.67543cdd9099fp-23 } },
        { "stage C", 1,
            { 0x1.e90b3ed0aa80cp+18, 0x1.d2b2fb8f9154p+37, -0x1.898p-6,
                0x1.dd01f89e9d87ep+18, 0x1.112ff8607691fp-11,
                0x1.e8ff4e925331fp+18 } },
        { "stage C", 1,
            { 0x1.f9p-15, -0x1.a476ed4778f4ep-8, 0x1.e8d9563aff343p+18,
                0x1.abbe2b6f796fdp+23, 0x1.04dfc099d9a37p-27,
                -0x1.0975ae1c2b6d3p-7 } },
        { "stage C", 1,
            { 0x1.8fffffffffffdp+5, -0x1.e780000031c99p+11,
                0x1.e8732683a2b8p+18, -0x1.29a62b783728ep+25,
                0x1.e799515b18716p+18, -0x1.29216d9382e5dp+25 } },
        { "stage D", -1,
            { 0x1.e90fbb4bd6729p+18, 0x1.d32b55b467cedp+37,
                0x1.6100000000003p-46, 0x1.5132cc67a5862p-27, -0x1.3dp-35,
                -0x1.2ecf5721933d7p-16 } },
        { "stage D", -1,
            { 0x1.e8ac7657ca17ap+18, 0x1.66dea6e878696p+25, 0x1.28p-32,
                0x1.b2bffffec388ap-26, -0x1.9f91f883aee09p-48,
                -0x1.312fd0bc66d87p-41 } },
        { "stage D", -1,
            { -0x1.6p+3, -0x1.60000000000cdp+4, -0x1.b2p-47,
                -0x1.a6bf457c3fa0bp-41, -0x1.75p-53, -0x1.995de57c3fa0bp-41 } },
        { "stage D", -1,
            { 0x1.e9351ecdf356dp+18, -0x1.31c133408ccc2p+21, -0x1.3p-29,
                0x1.5a5d0b7428083p-14, 0x1.41p-47, 0x1.5a512b7105883p-14 } },
        { "stage D", -1,
            { 0x1.e8abdd5aaf0ebp+18, 0x1.ca211f85041dcp+23, -0x1.ec8p-57,
                -0x1.ec00736ep-34, 0x1.cp-59, -0x1.ebffe5cp-34 } },
        { "stage D", -1,
            { 0x1.e75aadca55d56p+18, 0x1.d0f054a4ad9d6p+37, -0x1.1p-42,
                -0x1.038a8a9526b2ap-23, -0x1.3cp-51, -0x1.47df42b4ced74p-32 } },
        { "stage D", -1,
            { 0x1.e915b062af275p+18, -0x1.9caa4cd343c93p+24, -0x1.2ep-39,
                0x1.fda0000099c97p-34, -0x1.8d8p-38, 0x1.4f64000026726p-32 } },
        { "stage D", -1,
            { 0x1.e8121bc87d3eap+18, 0x1.399fa2da547acp-3, 0x1.a3p-37,
                0x1.0d0f03fe82819p-58, 0x1.3aa885cf379bcp-41,
                0x1.8e9313c79ea96p-63 } },
        { "stage D", 0,
            { 0x1.e7e8de9c0512bp+18, 0x1.e7e8e55a0512bp+23,
                -0x1.a6e5a6abd27b4p-2, -0x1.3b05a6abd27b4p+3, 0x1.8p+3,
                0x1.835fp+8 } },
        { "stage D", 0,
            { 0x1.2169217a56b68p-3, 0x1.2169bf7a56b68p+3, -0x1.1f8p-46,
                0x1.3bffffb82p-14, -0x1.ep+5, -0x1.dfffff62p+11 } },
        { "stage D", 0,
            { -0x1p+6, -0x1.9dp+10, 0x1.18p+6, 0x1.a88p+10, 0x1.08p-39,
                -0x1.9ffffffffe638p+5 } },
        { "stage D", 0,
            { -0x1.5a0a585bedd18p-3, -0x1.2ec9129070175p+3, -0x1.84p+6,
                -0x1.53800002ap+12, 0x1.5ep-20, 0x1.27cp-14 } },
        { "stage D", 0,
            { -0x1.59011db6d266p-58, -0x1.43f48bdb653fap-52, 0x1.7ap-40,
                0x1.625fffdf2p-34, 0x1.998p-55, 0x1.7fd79p-49 } },
        { "stage D", 0,
            { 0x1.4p+4, -0x1.4efffb46p+1, 0x1.9p+4, -0x1.a2bffb46p+1,
                -0x1.7cp-58, 0x1.2e800000018ddp-21 } },
        { "stage D", 0,
            { 0x1.1cp+6, 0x1.6dcp+12, -0x1.7cp-39, -0x1.4800000007b34p+5,
                0x1.9ap-14, -0x1.47ef624p+5 } },
        { "stage D", 0,
            { -0x1.ab8p-38, -0x1.7e8000440cap-2, -0x1.e58p-44,
                -0x1.7e80000135208p-2, 0x1.28p+6, 0x1.78ef40cp+15 } },
        { "stage D", 1,
            { 0x1.8cp+6, -0x1.1357ffffffffep+13, -0x1.a5p-53, 0x1.e44974p-39,
                0x1.a5182c3b99136p-57, 0x1.e1db66a627d1fp-39 } },
        { "stage D", 1,
            { 0x1p+4, 0x1.e869ad1f407fcp+22, 0x1.16d745934d092p-65,
                0x1.0616bbbeec4d7p-46, 0x1.adp-58, 0x1.9934bb8eaf8aep-39 } },
        { "stage D", 1,
            { 0x1.e8a2105940ccdp+18, 0x1.d23e5b05b0b21p+37, 0x1.c8p-59,
                0x1.b89e627fecaedp-40, -0x1.77746c98a1846p-52,
                -0x1.663547f93c8f8p-33 } },
        { "stage D", 1,
            { 0x1.e851ea244b43fp+18, 0x1.d207956b6720ap+37, -0x1.12p-57,
                -0x1.0c8020afc4517p-19, 0x1.ac9ecf3808759p-51,
                -0x1.0c73378f253d6p-19 } },
        { "stage D", 1,
            { 0x1.e8ea82bf9bdbep+18, 0x1.d162e5488285ap+37, 0x0p+0,
                -0x1.3cf830f2042ebp-50, 0x1.3aef4ca2a58cdp-37,
                0x1.2bc731ab6759bp-18 } },
        { "stage D", 1,
            { 0x1.28p+5, -0x1.848p+10, 0x1.a4p-51, -0x1.13b158p-45,
                -0x1.16547fa6d7994p-57, 0x1.64a2e78afaf92p-52 } },
        { "stage D", 1,
            { 0x1.e8bc10c8f7f41p+18, 0x1.50014af9ea77dp+22, -0x1.32p-33,
                -0x1.2080003498p-3, 0x1.dep-31, -0x1.207ffeb76p-3 } },
        { "stage D", 1,
            { 0x1.e889adf679db8p+18, 0x1.d12aeba8eef8cp+37, 0x1.2p-45,
                0x1.1239140fa6393p-26, -0x1.6ea0e8fce5c32p-48,
                -0x1.5d171530f2e5fp-29 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.0fffffffffffep+5, -0x1.039f11da6df74p+24,
                0x1.18001b6803aadp-59, 0x1.0b41f74dcd001p-40, 0x1.1a8p-15,
                0x1.0da4bd5daf634p+4 } },
        { "exact, wrong floating-point sign", -1,
            { 0x1.81bc6db459accp+1, 0x1.5d92c36b72a97p+6, 0x1.c4p-52,
                0x1.64edfa142a51cp-34, 0x1.e7f31ffa9d5e4p+18,
                0x1.ba3454fb1e9d7p+23 } },
        { "exact, wrong floating-point sign", -1,
            { 0x1.e7c462f0b2101p+18, 0x1.20b57b6809466p+18, 0x1.ba8p-43,
                -0x1.2032249dd8d6ep+3, -0x1.bfffffffffffep+2,
                -0x1.a4c90992508b6p+3 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.ep+5, -0x1.c8f8d38476824p+24, 0x1.6bddf7c5210eep+16,
                0x1.5a695a001d6c6p+35, 0x1.e7d82bcc112f6p+18,
                0x1.d071002b3eb5ep+37 } },
        { "exact, wrong floating-point sign", -1,
            { 0x1.e7a5ecce6b37p+18, 0x1.c7bdd18fe9b3p+17, -0x1.7730bf4397d9dp+6,
                -0x1.5ea44ebfeda09p+5, 0x1.1p+5, 0x1.fc67fffff826ep+3 } },
        { "exact, wrong floating-point sign", -1,
            { 0x1.594bb3a1f650dp-54, -0x1.a4d442ed6432ap-49,
                -0x1.83fffffffffffp-46, 0x1.d8dffffffffffp-41, 0x1.2p-28,
                -0x1.5fp-23 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.7006525b4d167p-62, 0x1.4p+3, 0x1.e76bf4b96b441p+18,
                -0x1.b9b9c1c80935bp+23, -0x1.8ae9f9dfe6312p+8,
                0x1.66340a72e89c8p+13 } },
        { "exact, wrong floating-point sign", -1,
            { 0x1.a9p-9, 0x1.7d4211ffffffdp+6, -0x1.73ffffffffffep+6,
                -0x1.16efffffffffep+13, 0x1.e8844d0d1876dp+18,
                0x1.723471e3ec8a1p+25 } },
        { "exact, wrong floating-point sign", 0,
            { -0x1.59011db6d266p-58, -0x1.43f48bdb653fap-52, 0x1.7ap-40,
                0x1.625fffdf2p-34, 0x1.998p-55, 0x1.7fd79p-49 } },
        { "exact, wrong floating-point sign", 0,
            { -0x1.ab8p-38, -0x1.7e8000440cap-2, -0x1.e58p-44,
                -0x1.7e80000135208p-2, 0x1.28p+6, 0x1.78ef40cp+15 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1.e8p-52, 0x1.8600028638p-44, 0x1.d8p-2, 0x1.38840030cp-17,
                -0x1.c8p+5, -0x1.2debffff9e8p-10 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1.56p-9, 0x1.213b03d8p-36, -0x1.75p-11, -0x1.3b7270ap-38,
                -0x1.1e8p-49, 0x1.ebff86da6p-59 } },
        { "exact, wrong floating-point sign", 0,
            { -0x1.7cp+6, -0x1.b8630bddd5b62p-15, -0x1.4cp+6,
                -0x1.80c253226f438p-15, -0x1p+5, -0x1.28ae840bf7b8bp-16 } },
        { "exact, wrong floating-point sign", 0,
            { -0x1.68p-43, -0x1.effffffffef2p+4, 0x1.e781f21e6b44p+18,
                -0x1.6da18516d073p+25, -0x1.3p+5, 0x1.c42p+11 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1.6p+5, 0x1.a0a000002e7p-4, 0x1.c8p-42, 0x1.73a1bb8p-39,
                0x1.78p+6, 0x1.bd0800001738p-3 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1.46p-38, 0x1.0f00003393p-19, 0x0p+0, 0x1.0fp-19, -0x1.3cp+6,
                -0x1.8fef788p-2 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.e90b3ed0aa80cp+18, 0x1.d2b2fb8f9154p+37, -0x1.898p-6,
                0x1.dd01f89e9d87ep+18, 0x1.112ff8607691fp-11,
                0x1.e8ff4e925331fp+18 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.e8fb70c388c39p+18, 0x1.d278dc3e90604p+37, 0x1.56p-42,
                -0x1.399e3570b4cb8p+4, 0x1.1p+4, 0x1.037a5e5a10acbp+23 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.c529f1fdc56eap-14, -0x1.1007699fa556bp+6, 0x1.5p+6,
                -0x1.64p+12, 0x1.08p+6, -0x1.18ap+12 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.7fffffffffffdp+3, -0x1.9b00000164ffdp-19, -0x1.4cp+6,
                0x1.6357ffffd3601p-16, 0x1.e88p-6, -0x1.056c8165p-27 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.e8dfecdb9bf94p+18, -0x1.67049f71468b1p+25, 0x1.ap+5,
                -0x1.373p+12, -0x1.e8p+5, 0x1.60bp+12 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.2cp+6, -0x1.0680000000015p+12, 0x1.e93f6317ad20dp+18,
                0x1.ac1776b4b77cbp+24, 0x1.c57890fd1c852p-8,
                0x1.8cc97edd257ap-2 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.fp+5, -0x1.d19b02046df18p+24, 0x1.e783629e783ebp+18,
                0x1.d12146f667f7bp+37, 0x1.e882d259e2e78p+18,
                0x1.d214fc3be0bc4p+37 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.1220848c94c93p-34, -0x1.40f751ed19209p+6,
                0x1.e795ec4eef81bp+18, 0x1.29915a04634f4p+11,
                0x1.e84a5e5857791p+18, 0x1.2a032ff55265dp+11 } },
    };

    const std::vector< RegressionCase > ORIENT_3D_CASES{
        { "filter", -1,
            { -0x1.48p+6, 0x1.12p-23, -0x1.57fff900dp+5, -0x1.33p-46,
                0x1.9aa7fc4417cc6p-39, -0x1.57ffffffffffdp+5, 0x1.598p-45,
                0x1.dcp-54, -0x1.5800000000001p+5, 0x1.d786101dc84d1p-33,
                -0x1.6p+5, -0x1.57ffffffffff6p+5 } },
        { "filter", -1,
            { 0x1.3cp+6, 0x1.18p+5, 0x1.e82e68dea2621p+18,
                0x1.e82e68dea2621p+18, 0x1.a4p-59, 0x1.e8d93b0a926a8p+18,
                0x1.e82e68dea2621p+18, 0x1.8000000000003p+6,
                0x1.e60293444b5c2p-53, 0x1.e77c92f7d6a7ap+18, -0x1.fcp-3,
                -0x1.4fp-11 } },
        { "filter", -1,
            { 0x1.e757f6e097aeep+18, -0x1.23d8407774a78p-18, -0x1.6c8p-46,
                -0x1.5p-56, -0x1.9225d7e23e18dp+2, -0x1.187fffffffffep-14,
                0x1.c8p+5, 0x1.4e90c92716c87p-54, 0x1.79b3dc470306cp-41,
                -0x1.5p-56, -0x1.23d8407774a78p-18, -0x1.6c8p-46 } },
        { "filter", 1,
            { -0x1.856edb339ab87p-54, -0x1.c7ffffffffffdp+5, -0x1.18p+6,
                0x1.e7be2e932563p+18, 0x1.928p+7, 0x1.56166bf8c7cfap-41,
                -0x1.17ffffffffffep+5, 0x1.e93fdd899b1a3p+18,
                0x1.6200000000001p-17, -0x1.9d1459ef983p-61,
                0x1.fa9603b88efp-51, 0x1.e8cf1652e42f2p+18 } },
        { "filter", 1,
            { -0x1.6p+5, 0x1.e8b7220826db4p+18, -0x1.991b997468855p+14,
                0x1.868bf64a9c8cfp-9, -0x1.6p+5, 0x1.139ca39e9c9efp+1,
                0x1.e8b7220826db4p+18, 0x1.6fffffffffffep-10,
                -0x1.8d76f7d1f2fb7p+22, 0x1.c8p-27, 0x1.138p+1,
                -0x1.d752f5e571934p-4 } },
        { "filter", 1,
            { -0x1.0900000000001p-13, 0x1.99ef178098934p-38, 0x1.5p+6,
                -0x1.6000000000001p-31, -0x1.3ap-47, 0x1.df8p+7,
                -0x1.82dd7fbaf8692p+7, 0x1.7cp+6, 0x1.99ef178098934p-38,
                0x1.99ef178098934p-38, -0x1.dddf32716857ap-11, -0x1.8cp-42 } },
        { "zero-filter", 0,
            { 0x1.90e9fdc1c0a6dp-19, 0x1.ap-53, 0x1.8ap-6,
                -0x1.27ffffffffffep-40, -0x1.9ba9c20e79c39p-21,
                0x1.c64562e3eb6d8p+15, -0x1.7feeb5e84bac5p-18,
                0x1.e7b8c459a5245p+18, 0x1.fbdfeb653bf35p-28,
                0x1.90e9fdc1c0a6dp-19, 0x1.ap-53, 0x1.8ap-6 } },
        { "zero-filter", 0,
            { 0x1.59174a8fcacb8p-64, -0x1.b400000000001p-51,
                0x1.e7b24a096077ep+18, -0x1.b400000000001p-51,
                0x1.40dd970823a4ap+8, 0x1.09ef9a95b7b02p-7,
                -0x1.48ffffffffffep-19, 0x1.e8f531cbd6aefp+18,
                0x1.e78176049d6bp+18, 0x1.59174a8fcacb8p-64,
                -0x1.b400000000001p-51, 0x1.e7b24a096077ep+18 } },
        { "zero-filter", 0,
            { 0x1.72c76e4675043p-45, 0x1.bcef512159383p-47,
                0x1.038b99fe1eb62p-45, -0x1p+4, -0x1.3333333333333p+2,
                -0x1.6666666666666p+3, 0x1.72c76e4675043p-45,
                0x1.24f9726194b5cp+17, 0x1.55cdb01c82d3dp+18,
                0x1.72c76e4675043p-45, 0x1.bcef512159383p-47,
                0x1.038b99fe1eb62p-45 } },
        { "stage B", -1,
            { -0x1.659fdac2e6cd6p-47, 0x1.f1ec2241e0388p+0,
                0x1.f1ec2241e0388p+0, -0x1.77p-17, -0x1.2dp-7,
                0x1.f1ec2241e1923p+0, 0x1.3fp-21, 0x1.b100000000003p-60,
                0x1.f1ec2241e0262p+0, -0x1.659fdac2e6cd6p-47,
                0x1.b100000000003p-60, 0x1.f1ec2241e0388p+0 } },
        { "stage B", -1,
            { -0x1.da00000000001p-42, -0x1.2p-5, 0x1.e77ab05ec953bp+18,
                0x1.e8532a6e4d8c7p+18, 0x1.b8p-35, 0x1.d17a2f9d2358ep+37,
                -0x1.4fa349f7f42a7p-5, -0x1.28724c15c472fp-48,
                0x1.d37bc0c1d265ap+18, 0x1.dbe96d952d05bp-48,
                0x1.e810a4e30f379p+18, 0x1.e77ab05eca393p+18 } },
        { "stage B", -1,
            { -0x1.f798f2e719d8fp-16, -0x1.2e2891bddc4efp-17,
                -0x1.6084aa082bb17p-16, 0x1.f800000000001p-24,
                0x1.2e66666666666p-25, -0x1.6084aa082bb17p-16,
                0x1.e76bd845e27ap+18, 0x1.2473e829ee493p+17,
                0x1.5531e430eb555p+18, -0x1.f798f2e719d8fp-16,
                0x1.2e66666666666p-25, -0x1.6084aa082bb17p-16 } },
        { "stage B", 1,
            { 0x1.e7cc178e1e3d6p+18, 0x1.e859b4d621535p+18,
                0x1.d2743f827c8f3p+37, -0x1.5p+6, 0x1.1fffffffffffep+5,
                0x1.13163c0177eep+24, 0x1.e909b83abf596p+18,
                0x1.e82af72e6068fp+18, 0x1.d2479a5cbf498p+37, -0x1.6p+6,
                0x1.e8020242814a5p+18, 0x1.d2207b8d97443p+37 } },
        { "stage B", 1,
            { 0x1.ap+4, 0x1.bp-13, 0x1.bp-13, 0x1.e8afc68b9d45bp+18,
                -0x1.3bp-41, -0x1.3ce1d23e87fd9p+25, -0x1.ap+5,
                -0x1.91112922ca084p-34, 0x1.12cfffd037b6ep+12,
                0x1.f2704dacaab7p-68, -0x1.99p-50, 0x1.43fffffff3d17p+6 } },
        { "stage B", 1,
            { 0x1.848p-19, 0x1.e7c715b4b4e9ep+18, 0x1.d15b880782fb1p+37,
                0x1.88p-16, -0x1.ecp-17, 0x1.e9316e03920e2p+18,
                0x1.e8e6c99158543p+18, 0x1.848p-19, 0x1.d2926f0291b66p+37,
                0x1.41p-58, 0x1.e7c0a362108ffp+18, 0x1.d15561908da22p+37 } },
        { "stage C exact differences", -1,
            { 0x1.e893b1b90c98fp+18, 0x1.25256aa23ac22p+17,
                0x1.5600fc67ef37dp+18, 0x1.e91866f3c736dp+18,
                0x1.25750a9244541p+17, 0x1.565de1aaa50c9p+18,
                0x1.e8f9cdbbd4cf6p+18, 0x1.2562aea3e616p+17,
                0x1.56487669e1c45p+18, 0x1.e927e8ffd11dep+18,
                0x1.257e58997d787p+17, 0x1.5668bcb31261bp+18 } },
        { "stage C exact differences", -1,
            { 0x1.18p+5, 0x1.5p+3, 0x1.88p+4, -0x1.b8p+5, -0x1.07ffffffffffdp+4,
                -0x1.34p+5, 0x1.ep+4, 0x1.2p+3, 0x1.4ffffffffffffp+4,
                0x1.e767bcf7fefe5p+18, 0x1.2471716198ffp+17,
                0x1.552f0447327edp+18 } },
        { "stage C exact differences", -1,
            { 0x1.b3p-12, 0x1.05p-13, 0x1.308p-12, 0x1.be80000000003p+5,
                0x1.0be6666666666p+4, 0x1.388ccccccccccp+5,
                0x1.e76938b9dc92cp+18, 0x1.2472553c5124dp+17,
                0x1.55300e1bb4002p+18, -0x1.cp+2, -0x1.0cccccccccccdp+1,
                -0x1.3999999999999p+2 } },
        { "stage C exact differences", 0,
            { 0x1.e89089a245f3bp+18, 0x1.252385c7c3923p+17,
                0x1.55fec6be642a9p+18, 0x1.e8efa2ebe4c3bp+18,
                0x1.255c94f3efa8ap+17, 0x1.56415871ecef6p+18,
                0x1.e8977fc208375p+18, 0x1.55fec6be642a9p+18,
                0x1.5603a63b05c05p+18, 0x1.e8977fc208375p+18,
                0x1.55fec6be642a9p+18, 0x1.5603a63b05c05p+18 } },
        { "stage C exact differences", 0,
            { 0x1.d88p+5, 0x1.1b8p+4, 0x1.4acp+5, 0x1.95p-6, 0x1.e6p-8,
                0x1.1b8p-6, 0x1.68p+6, 0x1.affffffffffffp+4,
                0x1.f7fffffffffffp+5, 0x1.ep-26, 0x1.2p-27, 0x1.5p-26 } },
        { "stage C exact differences", 0,
            { -0x1.a58p-56, 0x1.e5p-6, 0x1.e76320f4367e3p+18, -0x1.52p-29,
                -0x1.67afa74c0454ep-4, 0x1.e763a8379d8b9p+18,
                -0x1.d1555baee08ap-40, 0x1.86p-42, 0x1.e763430e367e3p+18,
                -0x1.d1555baee08ap-40, 0x1.86p-42, 0x1.e763430e367e3p+18 } },
        { "stage C exact differences", 1,
            { 0x1.e8d0caad2dd07p+18, 0x1.254a1334b516ap+17,
                0x1.562bc112d3451p+18, 0x0p+0, 0x0p+0, 0x0p+0,
                0x1.e83d734dc485ap+18, 0x1.24f1ab950f836p+17,
                0x1.55c49d833cc3fp+18, 0x1.e820057fe2c0fp+18,
                0x1.24e0034cbb409p+17, 0x1.55b003d98520ap+18 } },
        { "stage C exact differences", 1,
            { 0x0p+0, 0x0p+0, 0x0p+0, 0x1.e7f0e73c2c4bfp+18,
                0x1.24c3bdf0e760cp+17, 0x1.558f0843b89bbp+18,
                0x1.b2d60215ea7a7p-8, 0x1.04e6ce0d26497p-9,
                0x1.30629b0f5755ep-8, 0x1.e7f0e73c2c4bfp+18,
                0x1.24c3bdf0e760cp+17, 0x0p+0 } },
        { "stage C exact differences", 1,
            { 0x0p+0, 0x0p+0, 0x0p+0, 0x1.e7a5f778d7539p+18,
                0x1.2496c7aee7988p+17, 0x1.555a93a163872p+18, 0x1.5ap-46,
                0x1.9f33333333333p-48, 0x0p+0, 0x1.28p-13,
                0x1.6333333333336p-15, 0x1.9e66666666666p-14 } },
        { "stage C", -1,
            { 0x1.a8p+5, -0x1.59183079c12abp-40, 0x1.9c3e299232779p+24,
                -0x1.b000000000001p+4, 0x1.e87035a3820ddp+18,
                0x1.d24511968a1ecp+37, 0x1.58p+6, -0x1.88p+6,
                -0x1.512676b182d02p+22, 0x1.e77378074c3f9p+18,
                -0x1.2a703adf58ee7p-47, 0x1.d12852200dccfp+37 } },
        { "stage C", -1,
            { -0x1.1266666666666p-1, -0x1.d666666666669p-3,
                -0x1.1266666666666p-1, 0x1.b100000000003p-50,
                0x1.03ccccccccccfp-51, 0x1.2f19999999999p-50,
                0x1.e7889b004c62ap+18, 0x1.248529ccfaa17p+17,
                0x1.55460619cf11ep+18, -0x1.497b9f7314552p-17,
                -0x1.8b6125bd4b993p-19, -0x1.cd46ac0782dd9p-18 } },
        { "stage C", -1,
            { 0x1.e893c5db9020cp+18, 0x1.6p+5, 0x1.8cd292e44f0b4p+30,
                0x1.548p-30, 0x1.e7ff7651fca2bp+18, 0x1.d07ecd617ef93p+37,
                0x1.7800000000003p+6, -0x1.2905de1a4c58ep-37,
                0x1.2d5bc32e0e195p+18, -0x1.86p-27, -0x1.379b9b8f901cap+15,
                -0x1.2899b588e0c8ap+34 } },
        { "stage C", 1,
            { 0x1.518p-41, 0x1.95p-43, 0x1.d87ffffffffffp-42, -0x1.38p+5,
                -0x1.7666666666666p+3, -0x1.b4cccccccccccp+4,
                0x1.e81d5af01ca6ap+18, 0x1.24de69c34464p+17,
                0x1.55ae260e7a74bp+18, 0x1.d87ffffffffffp-42,
                0x1.24ba706644b5p+17, 0x1.55842dcca57ddp+18 } },
        { "stage C", 1,
            { -0x1.07119fb168bc4p-69, -0x1.3baebfa1b0e1ep-71,
                -0x1.704bdf91f9078p-70, -0x1.bp+5, -0x1.0333333333331p+4,
                -0x1.2e66666666666p+5, -0x1.07119fb168bc4p-69,
                -0x1.2886837051d8ep-6, -0x1.59f244030a27bp-5, -0x1.1p+5,
                -0x1.4666666666666p+3, -0x1.7ccccccccccccp+4 } },
        { "stage C", 1,
            { 0x1.e8ab49655d035p+18, 0x1.e8bc79d913b37p+18,
                0x1.03a7404d6702ep+23, 0x1.0cbd102950a76p-1, -0x1.a5p-54,
                0x1.ab013abcc5b2ep+5, 0x1.07fffffffffffp+6, -0x1.24p-37,
                -0x1.1200000000c8cp+8, 0x1.4p+3, 0x1.f2eef4045d15ap-46,
                0x1.80000000002bp+2 } },
        { "stage D", -1,
            { -0x1.dep-45, -0x1.1eccccccccccdp-46, -0x1.4e99999999998p-45,
                0x1.e771cc5a15fep+18, 0x1.24777a9c73986p+17,
                0x1.55360f0bdc31cp+18, -0x1.08p+5, -0x1.3cccccccccccdp+3,
                -0x1.7199999999999p+4, 0x1.2bp+3, 0x1.66ccccccccccdp+1,
                0x1.a299999999999p+2 } },
        { "stage D", -1,
            { -0x1.16p+7, -0x1.4d99999999999p+5, -0x1.8533333333333p+6,
                0x1.b78p+7, 0x1.07b3333333333p+6, 0x1.33a6666666664p+7,
                0x1.b900000000003p-50, 0x1.0899999999999p-51,
                0x1.34b3333333333p-50, -0x1.47p-44, -0x1.8866666666664p-46,
                -0x1.c9cccccccccccp-45 } },
        { "stage D", -1,
            { 0x1.614b08257241dp-32, 0x1.a7f3a36022b56p-34,
                0x1.ee9c3e9ad328ep-33, 0x1.e76dde7ee8253p+18,
                0x1.24751f18f1afep+17, 0x1.55334ef26f4d3p+18, -0x1.9e8p-23,
                -0x1.f166666666664p-25, -0x1.2226666666666p-23,
                -0x1.8108a0ee527f3p-54, -0x1.ce0a5ab796326p-56,
                -0x1.0d860a406cf29p-54 } },
        { "stage D", -1,
            { 0x1.e896e69f1f5dcp+18, 0x1.2527572c46051p+17,
                0x1.56033b08fc5b3p+18, -0x1.a8p+5, -0x1.fccccccccccccp+3,
                -0x1.28cccccccccccp+5, -0x1.e6p-34, -0x1.2399999999999p-35,
                -0x1.5433333333333p-34, -0x1.78p+6, -0x1.c333333333333p+4,
                -0x1.0733333333334p+6 } },
        { "stage D", -1,
            { 0x1.1a74d6f7a25dap-7, 0x1.52f29b8f8fa39p-9, 0x1.8b7060277ce97p-8,
                -0x1.be8e2c6ff0ec9p-44, -0x1.0beee7765d5acp-45,
                -0x1.3896b8b4c23f3p-44, 0x1.621116bb5775ap+4,
                0x1.a8e14e7a68f3bp+2, 0x1.efb186397a71ap+3, 0x1.4p+4, 0x1.8p+2,
                0x1.bffffffffffffp+3 } },
        { "stage D", -1,
            { -0x1.ef3d8abb290cep-5, -0x1.2924ecd6b23bp-6,
                -0x1.5aab144fcfef8p-5, 0x1.e7fa40fd5035dp+18,
                0x1.24c95a319686bp+17, 0x1.559593e484f27p+18, 0x1.0cp+6,
                0x1.4199999999999p+4, 0x1.7733333333333p+5, 0x1.b8p+5,
                0x1.08p+4, 0x1.34p+5 } },
        { "stage D", -1,
            { 0x1.08p-12, 0x1.3cccccccccccdp-14, 0x1.7199999999999p-13,
                0x1.1800000000001p+5, 0x1.5000000000003p+3,
                0x1.87fffffffffffp+4, -0x1.5400000000002p-44,
                -0x1.9800000000003p-46, -0x1.dbfffffffffffp-45,
                -0x1.1a0edcf81a414p-50, -0x1.52783c5cec4e5p-52,
                -0x1.8ae19bc1be5b5p-51 } },
        { "stage D", -1,
            { -0x1.bdfd4b3771ee5p+5, -0x1.0b97f9ee11289p+4,
                -0x1.38314e40695ap+5, -0x1.6cp+6, -0x1.b4ccccccccccdp+4,
                -0x1.fd99999999999p+5, -0x1p+6, -0x1.3333333333336p+4,
                -0x1.6666666666663p+5, 0x1.8265139f1f4b5p-64,
                0x1.cfac7df2258d9p-66, 0x1.0e79f42295e7cp-64 } },
        { "stage D", 0,
            { -0x1.eb8p-3, -0x1.26e6666666668p-4, -0x1.26e6666666668p-4,
                -0x1.9bce637a2b8a8p-45, -0x1.ee2addc5cdd96p-47,
                -0x1.2043ac08b8142p-45, 0x1.e80beb0bfedaep+18,
                0x1.24d3f36d98e9bp+17, 0x1.55a1f15532661p+18,
                -0x1.9bce637a2b8a8p-45, -0x1.ee2addc5cdd96p-47,
                -0x1.2043ac08b8142p-45 } },
        { "stage D", 0,
            { 0x1.e8cd41a5f2bdep+18, 0x1.e7b211a0c56bfp+18,
                0x1.d2d1e28419266p+37, -0x1.542abf239265fp-54, -0x1.0cp+6,
                0x1.e8975cc12ec57p+18, 0x1.e8634fe2f170dp+18, 0x1.3p-5,
                0x1.d26cb47ff9a36p+37, 0x1.e8634fe2f170dp+18, 0x1.3p-5,
                0x1.d26cb47ff9a36p+37 } },
        { "stage D", 0,
            { -0x1.e5p-17, 0x1.f2p-6, -0x1.1820000000002p+0,
                0x1.e926b3a714d3dp+18, 0x1.e7c1c0504d47fp+18,
                -0x1.125cfc2d2b787p+24, 0x1.e810094f7f481p+18, -0x1.44p+6,
                0x1.6c8p+11, 0x1.e926b3a714d3dp+18, 0x1.e7c1c0504d47fp+18,
                -0x1.125cfc2d2b787p+24 } },
        { "stage D", 0,
            { -0x1p+6, -0x1.3333333333333p+4, -0x1.6666666666668p+5, 0x1p-52,
                0x1.333333333333p-54, 0x1.6666666666666p-53,
                -0x1.0734791a71418p+2, -0x1.3bd89152ee4ebp+0,
                -0x1.707ca98b6b5bcp+1, 0x1p-52, 0x1.333333333333p-54,
                0x1.6666666666666p-53 } },
        { "stage D", 0,
            { 0x1.e76c88759431ep+18, 0x1.e7cd57e27de35p+18,
                0x1.d09223852a123p+37, 0x1.978f8e130ebb5p-54,
                -0x1.27ffffffffffep+2, -0x1.19e7179bccd03p+21,
                0x1.e76c88759431ep+18, 0x1.0a148850c0f7ap-20,
                -0x1.372c08ce4c9b7p+5, 0x1.978f8e130ebb5p-54,
                -0x1.27ffffffffffep+2, -0x1.19e7179bccd03p+21 } },
        { "stage D", 0,
            { -0x1.70da3ece5d43dp-21, -0x1.ba9f7e913cb7fp-23,
                -0x1.02325f2a0e16p-21, 0x1.001fb8edc2c7dp-40,
                -0x1.70da3ece5d43dp-21, 0x1.6692cfb343e48p-41,
                0x1.773082522d292p-48, 0x1.c23a35fc36315p-50,
                0x1.06a1f4d31f9ccp-48, 0x1.773082522d292p-48,
                0x1.c23a35fc36315p-50, 0x1.06a1f4d31f9ccp-48 } },
        { "stage D", 0,
            { 0x1.e82b5ea000612p+18, 0x1.847681a86d5ap-14, -0x1.58p+5,
                -0x1.1f169da82ce98p-50, 0x1.3p+5, 0x1.64p+6,
                0x1.8840fa6f8ddb1p-31, 0x1.e74e58bfd38efp+18,
                0x1.e82b5ea000612p+18, 0x1.8840fa6f8ddb1p-31,
                0x1.e74e58bfd38efp+18, 0x1.e82b5ea000612p+18 } },
        { "stage D", 0,
            { 0x1.1eccccccccccfp-54, 0x1.1eccccccccccfp-54,
                0x1.4e99999999999p-53, -0x1.0a6b2c3018678p-38,
                -0x1.3fb3cea01d492p-40, -0x1.74fc7110222abp-39, -0x1.6cp-22,
                -0x1.b4ccccccccccdp-24, -0x1.fd99999999999p-23,
                -0x1.0a6b2c3018678p-38, -0x1.3fb3cea01d492p-40,
                -0x1.74fc7110222abp-39 } },
        { "stage D", 1,
            { -0x1.9p+5, -0x1.dffffffffffffp+3, -0x1.18p+5,
                -0x1.67fffffffffffp+6, -0x1.bp+4, -0x1.f7fffffffffffp+5,
                -0x1.c1182267e4d41p-32, -0x1.0d74e171894cp-33,
                -0x1.3a5db1af202ep-32, -0x1.8800000000001p+6,
                -0x1.d666666666666p+4, -0x1.1266666666666p+6 } },
        { "stage D", 1,
            { -0x1.5dp-51, -0x1.a2ccccccccccbp-53, -0x1.e899999999999p-52,
                0x1.3p+6, 0x1.6cccccccccccdp+4, 0x1.a999999999999p+5,
                -0x1.e8p-52, -0x1.24ccccccccccdp-53, -0x1.5599999999999p-52,
                -0x1.4dp-9, -0x1.8f9999999999bp-11, -0x1.d233333333333p-10 } },
        { "stage D", 1,
            { 0x1.e7f6ae2f84e42p+18, 0x1.24c7354fb6227p+17,
                0x1.55931387a9d2ep+18, 0x1.3bffffffffffdp+6,
                0x1.7b33333333333p+4, 0x1.ba66666666666p+5,
                -0x1.3c00000000001p-43, -0x1.7b33333333333p-45,
                -0x1.ba66666666666p-44, -0x1.87ffffffffffdp+5,
                -0x1.d666666666666p+3, -0x1.1266666666666p+5 } },
        { "stage D", 1,
            { -0x1.b28p-26, -0x1.04b3333333333p-27, -0x1.3026666666665p-26,
                0x1.4e981b7123e1p-22, 0x1.918354215e413p-24,
                0x1.d46e8cd198a13p-23, 0x1.e85bbd8a4a731p+18,
                0x1.2503d81fc645p+17, 0x1.55d9d17a67508p+18,
                0x1.f800000000002p+5, 0x1.2e66666666666p+4,
                0x1.60cccccccccccp+5 } },
        { "stage D", 1,
            { 0x1.dc00000000003p+6, 0x1.1d99999999999p+5, 0x1.4d33333333333p+6,
                0x1.baac160da5626p-39, 0x1.099a73a1c9a18p-40,
                0x1.35dedc3cc091bp-39, 0x1.e8ca92a3ed3d8p+18,
                0x1.254657fbf4be8p+17, 0x1.562766a5f2de4p+18, 0x1.73p-19,
                0x1.bd33333333333p-21, 0x1.03b3333333333p-19 } },
        { "stage D", 1,
            { -0x1.1cp+6, -0x1.54ccccccccccdp+4, -0x1.8d99999999999p+5,
                0x1.e7c415edd558fp+18, 0x1.24a8d9f5199bcp+17,
                0x1.556fa8f3488afp+18, 0x1p+3, 0x1.3333333333333p+1,
                0x1.6666666666664p+2, -0x1.1838004e4d246p-68,
                -0x1.50433391295edp-70, -0x1.884e66d405995p-69 } },
        { "stage D", 1,
            { 0x1.2c792b7ba008ap-7, 0x1.689167612670cp-9, 0x1.a4a9a346acd8bp-8,
                -0x1.72b945ede4ce8p-18, -0x1.bcde53ea45c47p-20,
                -0x1.0381b0f3535d5p-18, -0x1.68p+5, -0x1.affffffffffffp+3,
                -0x1.f7fffffffffffp+4, 0x1.7580000000002p-11,
                0x1.c033333333335p-13, 0x1.0573333333332p-11 } },
        { "stage D", 1,
            { 0x1.e759871910d7dp+18, 0x1.2468eaa8a3b4bp+17,
                0x1.552511c4befd7p+18, 0x1.2p+3, 0x1.5999999999999p+1,
                0x1.9333333333334p+2, 0x1.950531a1b3f92p+12,
                0x1.e6063b8ed7f81p+10, 0x1.1b83a2bdfdfb5p+12, 0x0p+0, 0x0p+0,
                0x0p+0 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.1266666666666p-1, -0x1.d666666666669p-3,
                -0x1.1266666666666p-1, 0x1.b100000000003p-50,
                0x1.03ccccccccccfp-51, 0x1.2f19999999999p-50,
                0x1.e7889b004c62ap+18, 0x1.248529ccfaa17p+17,
                0x1.55460619cf11ep+18, -0x1.497b9f7314552p-17,
                -0x1.8b6125bd4b993p-19, -0x1.cd46ac0782dd9p-18 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.4ap-49, 0x1.64p+6, -0x1.0b00000000278p+13,
                0x1.e8a778b6e0defp+18, 0x1.6e8p-39, 0x1.d24a8e3c62f12p+37,
                0x1.e7eb56ac213c9p+18, -0x1.17ffffffffffdp+5,
                0x1.d19708b5b240dp+37, 0x1.ddffffffffffep-61,
                0x1.b417d4c9b35ccp-41, -0x1.4549b2325ca5p-34 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.29e0d5bbdd285p-41, 0x1.e7c39a67e949ep+18,
                0x1.d080b05e9c91ep+37, -0x1.713eb0f1b4e1dp-23, 0x1.32p+5,
                0x1.2b0a208e54d6p+24, 0x1.e77e8424e7b66p+18,
                0x1.e82105025d602p+18, 0x1.d0d9a671a7bd3p+37,
                0x1.6d399883fb49dp+14, 0x1.e2b0d297b39f8p-11,
                0x1.e8f9c3d208987p+18 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.16p+7, -0x1.4d99999999999p+5, -0x1.8533333333333p+6,
                0x1.b78p+7, 0x1.07b3333333333p+6, 0x1.33a6666666664p+7,
                0x1.b900000000003p-50, 0x1.0899999999999p-51,
                0x1.34b3333333333p-50, -0x1.47p-44, -0x1.8866666666664p-46,
                -0x1.c9cccccccccccp-45 } },
        { "exact, wrong floating-point sign", -1,
            { 0x1.e7f2af0a01eefp+18, 0x1.e7f7557f732cep+18,
                0x1.faef0aa9702e7p+17, -0x1.58p+5, 0x1.a6004a886cb4p+2,
                -0x1.656284e8336e5p+4, 0x1.e881dec2ebe92p+18,
                -0x1.da56cca59a6a3p-45, 0x1.fb83cca31809dp+17, -0x1.4cp+6,
                0x1.e8f726cbc65e2p+18, -0x1.58eafd4b43739p+5 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.e5424739d220fp-1, -0x1.048p-2, -0x1.f0ee6102e50f1p+16,
                0x1.e7b817a290fffp+18, 0x1.b28p-38, -0x1.d43238b04de29p+22,
                0x1.2780000000003p-8, 0x1.b98p-9, 0x1.90deb2e2c7a3fp+10,
                -0x1.1p+6, 0x1.6c8p-24, 0x1.e23d6dedc8698p+9 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.82p-24, -0x1.fp+4, 0x1.7d47c7e26b369p+4,
                0x1.e8a3c0e4fe921p+18, 0x1.e7c97d00aef44p+18,
                0x1.d268888727e8fp+37, 0x1.e7978a55bfb07p+18, -0x1.3p+6,
                0x1.d16885dd869fp+37, -0x1.4ep-61, 0x1.e7af934633c69p+18,
                0x1.7dfffffffffbp+4 } },
        { "exact, wrong floating-point sign", -1,
            { 0x1.e896e69f1f5dcp+18, 0x1.2527572c46051p+17,
                0x1.56033b08fc5b3p+18, -0x1.a8p+5, -0x1.fccccccccccccp+3,
                -0x1.28cccccccccccp+5, -0x1.e6p-34, -0x1.2399999999999p-35,
                -0x1.5433333333333p-34, -0x1.78p+6, -0x1.c333333333333p+4,
                -0x1.0733333333334p+6 } },
        { "exact, wrong floating-point sign", 0,
            { -0x1.b8p+5, -0x1.08p+4, -0x1.34p+5, 0x1.18p-26, 0x1.5p-28,
                0x1.88p-27, -0x1.15p-33, -0x1.4c66666666666p-35,
                -0x1.83cccccccccccp-34, -0x1.d07faab9000a6p-41,
                -0x1.16b30008999fdp-42, -0x1.45262ab4b33a7p-41 } },
        { "exact, wrong floating-point sign", 0,
            { -0x1.7d3a7a2b0c7ap+2, -0x1.c9795f66dbc59p+0,
                -0x1.0adc225155889p+2, 0x1.6p+6, 0x1.a666666666666p+4,
                0x1.eccccccccccccp+5, -0x1.b2p-7, -0x1.0466666666666p-8,
                -0x1.2fcccccccccccp-7, -0x1.54d6555d238dep-57,
                -0x1.9901333c9110ap-59, -0x1.dd2c111bfe936p-58 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1.e84683f9ffbb4p+18, 0x1.24f71bfc663d2p+17,
                0x1.55caf5fbcc9cap+18, 0x1.e7945b255ab92p+18,
                0x1.248c36b0033bep+17, 0x1.554e3fcd591b3p+18, -0x1.38p+6,
                -0x1.7666666666666p+4, -0x1.b4cccccccccccp+5, 0x1.7dp+7,
                0x1.c933333333333p+5, 0x1.0ab3333333333p+7 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1.d88p+5, 0x1.1b8p+4, 0x1.4acp+5, 0x1.95p-6, 0x1.e6p-8,
                0x1.1b8p-6, 0x1.68p+6, 0x1.affffffffffffp+4,
                0x1.f7fffffffffffp+5, 0x1.ep-26, 0x1.2p-27, 0x1.5p-26 } },
        { "exact, wrong floating-point sign", 0,
            { -0x1p-62, -0x1.3333333333333p-64, -0x1.6666666666666p-63,
                0x1.e914e6629c41ap+18, 0x1.2572f0a190f43p+17,
                0x1.565b6e11d3c79p+18, 0x1.78p+6, 0x1.c333333333333p+4,
                0x1.0733333333333p+6, 0x1.84p-60, 0x1.d199999999999p-62,
                0x1.0f99999999999p-60 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1.46p-11, 0x1.8733333333333p-13, 0x1.c866666666666p-12,
                0x1.3cf1c3f8c8b83p-41, 0x1.7c555190f0ddp-43,
                0x1.bbb8df291901dp-42, 0x1.78p+6, 0x1.c333333333333p+4,
                0x1.0733333333333p+6, 0x1.09p-36, 0x1.3ep-38, 0x1.73p-37 } },
        { "exact, wrong floating-point sign", 0,
            { -0x1.8cp-58, -0x1.db33333333333p-60, -0x1.1533333333333p-58,
                -0x1.c2065c2ff3ff6p-2, -0x1.0e03d0e992661p-3,
                -0x1.3b0473bb2acc7p-2, 0x1.7p+5, 0x1.b999999999999p+3,
                0x1.0199999999999p+5, 0x1.5cfd6208a7c8cp-37,
                0x1.a2c9a8d72fbdbp-39, 0x1.e895efa5b7b2ap-38 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1.e8b08e7ff8f4ap+18, 0x1.2536bbe6622c6p+17,
                0x1.5615308cc7de7p+18, 0x1.88p+6, 0x1.d666666666666p+4,
                0x1.1266666666666p+6, 0x1.e824fe44d097ep+18,
                0x1.24e2fef616c18p+17, 0x1.55b37ec9c5371p+18, 0x1.1p+5,
                0x1.4666666666666p+3, 0x1.7ccccccccccccp+4 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.5dp-51, -0x1.a2ccccccccccbp-53, -0x1.e899999999999p-52,
                0x1.3p+6, 0x1.6cccccccccccdp+4, 0x1.a999999999999p+5,
                -0x1.e8p-52, -0x1.24ccccccccccdp-53, -0x1.5599999999999p-52,
                -0x1.4dp-9, -0x1.8f9999999999bp-11, -0x1.d233333333333p-10 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.518p-41, 0x1.95p-43, 0x1.d87ffffffffffp-42, -0x1.38p+5,
                -0x1.7666666666666p+3, -0x1.b4cccccccccccp+4,
                0x1.e81d5af01ca6ap+18, 0x1.24de69c34464p+17,
                0x1.55ae260e7a74bp+18, 0x1.d87ffffffffffp-42,
                0x1.24ba706644b5p+17, 0x1.55842dcca57ddp+18 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.b28p-26, -0x1.04b3333333333p-27, -0x1.3026666666665p-26,
                0x1.4e981b7123e1p-22, 0x1.918354215e413p-24,
                0x1.d46e8cd198a13p-23, 0x1.e85bbd8a4a731p+18,
                0x1.2503d81fc645p+17, 0x1.55d9d17a67508p+18,
                0x1.f800000000002p+5, 0x1.2e66666666666p+4,
                0x1.60cccccccccccp+5 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.9p+6, 0x1.ep+4, 0x1.18p+6, -0x1.5dffffffffffep-57,
                -0x1.a4p-59, 0x1.ep+4, 0x1.c88p-54, 0x1.11e6666666666p-55,
                0x1.3f8ccccccccc9p-54, -0x1p+0, -0x1.3333333333334p-2,
                -0x1.6666666666666p-1 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.8p+3, 0x1.e8950465c0aa7p+18, 0x1.d2a77ce8b36dbp+37,
                0x1.0100000000002p+4, 0x1.e81a66537d853p+18, 0x1.d2325fb0a7p+37,
                0x1.380f8cd10533p-67, -0x1.93ef94d17ffc9p-39,
                0x1.3b672e4c3447dp-2, 0x1.b48p-5, -0x1.ea00000000002p-49,
                0x1.5b1678ea299cfp+2 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.0dc2c9c7bd2dfp-60, -0x1.43b68bbc7c9d8p-62,
                -0x1.79aa4db13c0d1p-61, -0x1.0967b20b52cb5p-28,
                -0x1.3e7c6f40c9c0cp-30, -0x1.73912c7640b63p-29, 0x1.ap+3,
                0x1.f333333333333p+1, -0x1.0dc2c9c7bd2dfp-60,
                -0x1.6278f622c571bp+15, -0x1.a95df429b9bbap+13,
                -0x1.f042f230ae058p+14 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.2c792b7ba008ap-7, 0x1.689167612670cp-9, 0x1.a4a9a346acd8bp-8,
                -0x1.72b945ede4ce8p-18, -0x1.bcde53ea45c47p-20,
                -0x1.0381b0f3535d5p-18, -0x1.68p+5, -0x1.affffffffffffp+3,
                -0x1.f7fffffffffffp+4, 0x1.7580000000002p-11,
                0x1.c033333333335p-13, 0x1.0573333333332p-11 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.ffffffffffffdp+0, 0x1.3333333333333p-1, 0x1.6666666666666p+0,
                0x1.7c6ea44fd360dp-21, 0x1.c884c52c9740fp-23,
                0x1.0a4d7304ad908p-21, -0x1.07c72542a016fp+9,
                -0x1.3c88931cc01b9p+7, -0x1.714a00f6e02p+8, -0x1.2bp-22,
                -0x1.66ccccccccccdp-24, -0x1.a299999999999p-23 } },
    };

    const std::vector< RegressionCase > DET_3D_CASES{
        { "filter", -1,
            { 0x1.868bf64a9c8cfp-9, -0x1.6p+5, 0x1.139ca39e9c9efp+1,
                0x1.e8b7220826db4p+18, 0x1.6fffffffffffep-10,
                -0x1.8d76f7d1f2fb7p+22, 0x1.c8p-27, 0x1.138p+1,
                -0x1.d752f5e571934p-4 } },
        { "filter", -1,
            { 0x1.1p+6, 0x1.e74fac642745p+18, 0x1.e902bbb60aaedp+18,
                0x1.5ca0358103241p-14, 0x1.1368c1c3eb0acp-41,
                -0x1.7a8481e0e431p-34, 0x1.6p+5, -0x1.4b7e742389ec9p-64,
                -0x1.95176f0aa877fp-43 } },
        { "filter", -1,
            { 0x1.e8532a6e4d8c7p+18, 0x1.b8p-35, 0x1.d17a2f9d2358ep+37,
                -0x1.4fa349f7f42a7p-5, -0x1.28724c15c472fp-48,
                0x1.d37bc0c1d265ap+18, 0x1.dbe96d952d05bp-48,
                0x1.e810a4e30f379p+18, 0x1.e77ab05eca393p+18 } },
        { "filter", 1,
            { -0x1.b000000000001p+4, 0x1.e87035a3820ddp+18,
                0x1.d24511968a1ecp+37, 0x1.58p+6, -0x1.88p+6,
                -0x1.512676b182d02p+22, 0x1.e77378074c3f9p+18,
                -0x1.2a703adf58ee7p-47, 0x1.d12852200dccfp+37 } },
        { "filter", 1,
            { 0x1.e7be2e932563p+18, 0x1.928p+7, 0x1.56166bf8c7cfap-41,
                -0x1.17ffffffffffep+5, 0x1.e93fdd899b1a3p+18,
                0x1.6200000000001p-17, -0x1.9d1459ef983p-61,
                0x1.fa9603b88efp-51, 0x1.e8cf1652e42f2p+18 } },
        { "filter", 1,
            { -0x1.6000000000001p-31, -0x1.3ap-47, 0x1.df8p+7,
                -0x1.82dd7fbaf8692p+7, 0x1.7cp+6, 0x1.99ef178098934p-38,
                0x1.99ef178098934p-38, -0x1.dddf32716857ap-11, -0x1.8cp-42 } },
        { "zero-filter", 0,
            { 0x1.2p+3, 0x1.5999999999999p+1, 0x1.9333333333334p+2,
                0x1.950531a1b3f92p+12, 0x1.e6063b8ed7f81p+10,
                0x1.1b83a2bdfdfb5p+12, 0x0p+0, 0x0p+0, 0x0p+0 } },
        { "zero-filter", 0,
            { 0x1.e7c0d9abc41a9p+18, 0x1.24a6e900a8dccp+17,
                0x1.556d652b6fac3p+18, 0x0p+0, 0x0p+0, 0x0p+0, 0x1.6p+6,
                -0x1.3de6b040960b6p+10, 0x1.eccccccccccccp+5 } },
        { "zero-filter", 0,
            { 0x0p+0, 0x0p+0, 0x0p+0, 0x1.e83d734dc485ap+18,
                0x1.24f1ab950f836p+17, 0x1.55c49d833cc3fp+18,
                0x1.e820057fe2c0fp+18, 0x1.24e0034cbb409p+17,
                0x1.55b003d98520ap+18 } },
        { "exact", -1,
            { -0x1.67fffffffffffp+6, -0x1.bp+4, -0x1.f7fffffffffffp+5,
                -0x1.c1182267e4d41p-32, -0x1.0d74e171894cp-33,
                -0x1.3a5db1af202ep-32, -0x1.8800000000001p+6,
                -0x1.d666666666666p+4, -0x1.1266666666666p+6 } },
        { "exact", -1,
            { 0x1.b100000000003p-50, 0x1.03ccccccccccfp-51,
                0x1.2f19999999999p-50, 0x1.e7889b004c62ap+18,
                0x1.248529ccfaa17p+17, 0x1.55460619cf11ep+18,
                -0x1.497b9f7314552p-17, -0x1.8b6125bd4b993p-19,
                -0x1.cd46ac0782dd9p-18 } },
        { "exact", -1,
            { 0x1.e81e90526b1f3p+18, 0x1.24df2364a6ac4p+17,
                0x1.55aefea017c8fp+18, 0x1.4642545def031p-22,
                0x1.8782cba3eb9d4p-24, 0x1.c8c342e9e8377p-23,
                0x1.4642545def031p-22, 0x1.24df2364a6ac4p+17,
                0x1.0ab3333333333p-46 } },
        { "exact", 0,
            { -0x1.9bce637a2b8a8p-45, -0x1.ee2addc5cdd96p-47,
                -0x1.2043ac08b8142p-45, 0x1.e80beb0bfedaep+18,
                0x1.24d3f36d98e9bp+17, 0x1.55a1f15532661p+18,
                -0x1.9bce637a2b8a8p-45, -0x1.ee2addc5cdd96p-47,
                -0x1.2043ac08b8142p-45 } },
        { "exact", 0,
            { 0x1.e80602af846a8p+18, 0x1.24d06802e90cbp+17,
                0x1.559dceae0fe42p+18, 0x1.d644e6b5a8de8p-44,
                0x1.1a295739cbb8bp-45, 0x1.49303b18c3022p-44, -0x1.ep-51,
                -0x1.2p-52, -0x1.5p-51 } },
        { "exact", 0,
            { 0x1.e92a021750d96p+18, 0x1.257f9adaca1cp+17,
                0x1.566a34a9ebcb5p+18, -0x1.7p+5, -0x1.b999999999999p+3,
                -0x1.0199999999999p+5, 0x1.3c5392b8cb23dp-38,
                0x1.7b977cddc0916p-40, 0x1.badb6702b5fefp-39 } },
        { "exact", 1,
            { -0x1.5p+6, 0x1.1fffffffffffep+5, 0x1.13163c0177eep+24,
                0x1.e909b83abf596p+18, 0x1.e82af72e6068fp+18,
                0x1.d2479a5cbf498p+37, -0x1.6p+6, 0x1.e8020242814a5p+18,
                0x1.d2207b8d97443p+37 } },
        { "exact", 1,
            { 0x1.3p+6, 0x1.6cccccccccccdp+4, 0x1.a999999999999p+5, -0x1.e8p-52,
                -0x1.24ccccccccccdp-53, -0x1.5599999999999p-52, -0x1.4dp-9,
                -0x1.8f9999999999bp-11, -0x1.d233333333333p-10 } },
        { "exact", 1,
            { 0x1.548p-30, 0x1.e7ff7651fca2bp+18, 0x1.d07ecd617ef93p+37,
                0x1.7800000000003p+6, -0x1.2905de1a4c58ep-37,
                0x1.2d5bc32e0e195p+18, -0x1.86p-27, -0x1.379b9b8f901cap+15,
                -0x1.2899b588e0c8ap+34 } },
        { "exact, wrong floating-point sign", -1,
            { 0x1.b100000000003p-50, 0x1.03ccccccccccfp-51,
                0x1.2f19999999999p-50, 0x1.e7889b004c62ap+18,
                0x1.248529ccfaa17p+17, 0x1.55460619cf11ep+18,
                -0x1.497b9f7314552p-17, -0x1.8b6125bd4b993p-19,
                -0x1.cd46ac0782dd9p-18 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.a8p-53, -0x1.fccccccccccccp-55, -0x1.28cccccccccccp-53,
                0x1.4bfffffffffffp+6, 0x1.8e66666666666p+4,
                0x1.d0cccccccccccp+5, 0x1.3d8p-33, 0x1.7dp-35, 0x1.bc8p-34 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.72b945ede4ce8p-18, -0x1.bcde53ea45c47p-20,
                -0x1.0381b0f3535d5p-18, -0x1.68p+5, -0x1.affffffffffffp+3,
                -0x1.f7fffffffffffp+4, 0x1.7580000000002p-11,
                0x1.c033333333335p-13, 0x1.0573333333332p-11 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.a8p-56, -0x1.fcccccccccccap-58, -0x1.28cccccccccccp-56,
                0x1.1cp+6, 0x1.54ccccccccccdp+4, 0x1.8d99999999999p+5,
                0x1.e79a1df73e606p+18, 0x1.248fab9458a04p+17,
                0x1.5552482d12105p+18 } },
        { "exact, wrong floating-point sign", -1,
            { 0x1.1800000000001p+5, 0x1.5000000000003p+3, 0x1.87fffffffffffp+4,
                -0x1.5400000000002p-44, -0x1.9800000000003p-46,
                -0x1.dbfffffffffffp-45, -0x1.1a0edcf81a414p-50,
                -0x1.52783c5cec4e5p-52, -0x1.8ae19bc1be5b5p-51 } },
        { "exact, wrong floating-point sign", -1,
            { 0x1.e8d4935e52ccp+18, 0x1.254c586bcb473p+17,
                0x1.562e67286d286p+18, 0x1.44989c08ecap+13,
                0x1.8583ee711bf33p+11, 0x1.c66f40d94b466p+12,
                -0x1.a880000000002p-27, 0x1.9819999999999p-26,
                -0x1.2926666666666p-27 } },
        { "exact, wrong floating-point sign", -1,
            { 0x1.d68p-13, 0x1.1a4cccccccccdp-14, 0x1.495999999999bp-13,
                -0x1.16p-50, -0x1.4d99999999999p-52, -0x1.8533333333333p-51,
                0x1.5b37d992ac686p-67, 0x1.a0a96b7ccee3ap-69,
                0x1.e61afd66f15eep-68 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.64p+6, -0x1.ab33333333333p+4, -0x1.f266666666666p+5,
                0x1.e867b179f2aedp+18, 0x1.250b0415f802ap+17,
                0x1.55e22f6ef6ad9p+18, 0x1.368p-44, 0x1.7499999999999p-46,
                0x1.b2b3333333334p-45 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1.e92a021750d96p+18, 0x1.257f9adaca1cp+17,
                0x1.566a34a9ebcb5p+18, -0x1.7p+5, -0x1.b999999999999p+3,
                -0x1.0199999999999p+5, 0x1.3c5392b8cb23dp-38,
                0x1.7b977cddc0916p-40, 0x1.badb6702b5fefp-39 } },
        { "exact, wrong floating-point sign", 0,
            { -0x1.9p+5, -0x1.ep+3, -0x1.18p+5, 0x1.e7cc9c8e5aec2p+18,
                0x1.24adf7889cf41p+17, 0x1.5575a0ca0c721p+18, -0x1.dp+5,
                -0x1.1666666666666p+4, -0x1.44cccccccccccp+5 } },
        { "exact, wrong floating-point sign", 0,
            { -0x1.6p+6, -0x1.a666666666666p+4, 0x1.2333333333333p-4, 0x1.4p+6,
                0x1.8p+4, 0x1.cp+5, 0x1.b8p+5, 0x1.08p+4, 0x1.34p+5 } },
        { "exact, wrong floating-point sign", 0,
            { -0x1.95p+1, -0x1.e6p-1, -0x1.1b8p+1, -0x1.1cb1c32b3814p-61,
                -0x1.55a21d671018p-63, -0x1.8e9277a2e81cp-62,
                0x1.e8c38262eb7adp+18, 0x1.25421b0826e35p+17,
                0x1.562274ded808fp+18 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1.77b86c3e91155p+16, 0x1.c2dd4eb17ae66p+14,
                0x1.07011892325bdp+16, 0x1.832f903e2e4fap-25,
                0x1.d09f79e43792cp-27, 0x1.0f07b1c5206adp-25,
                0x1.e7958a660e749p+18, 0x1.248ceca3a245fp+17,
                0x1.554f14143d519p+18 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1.e847a332f86d6p+18, 0x1.24f7c851c841ap+17,
                0x1.55cbbf0a144c9p+18, 0x1.e8197c672c2cep+18,
                0x1.24dc17711a815p+17, 0x1.55ab70ae9eec3p+18,
                0x1.d4182cf254a1cp-48, 0x1.18db4e2aff944p-49,
                0x1.47aa85dcd4d7ap-48 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1p+4, 0x1.3333333333333p+2, 0x1.6666666666663p+3,
                0x1.e913cf2996126p+18, 0x1.25724918f3a4ap+17,
                0x1.565aaa9d1c401p+18, 0x1.9p-61, 0x1.ep-63, 0x1.18p-61 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1.36p-58, 0x1.74p-60, 0x1.b2p-59, 0x1.ca4f470a2eb6p-2,
                0x1.12fc5dd2e8d39p-3, 0x1.40d11820ba4c2p-2, 0x1.ccp-53,
                0x1.14p-54, 0x1.42p-53 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.3p+6, 0x1.6cccccccccccdp+4, 0x1.a999999999999p+5, -0x1.e8p-52,
                -0x1.24ccccccccccdp-53, -0x1.5599999999999p-52, -0x1.4dp-9,
                -0x1.8f9999999999bp-11, -0x1.d233333333333p-10 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.38p+5, -0x1.7666666666666p+3, -0x1.b4cccccccccccp+4,
                0x1.e81d5af01ca6ap+18, 0x1.24de69c34464p+17,
                0x1.55ae260e7a74bp+18, 0x1.d87ffffffffffp-42,
                0x1.24ba706644b5p+17, 0x1.55842dcca57ddp+18 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.a8p+5, -0x1.fccccccccccccp+3, -0x1.28cccccccccccp+5,
                -0x1.e6p-34, -0x1.2399999999999p-35, -0x1.5433333333333p-34,
                -0x1.78p+6, -0x1.c333333333333p+4, -0x1.0733333333334p+6 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.0967b20b52cb5p-28, -0x1.3e7c6f40c9c0cp-30,
                -0x1.73912c7640b63p-29, 0x1.ap+3, 0x1.f333333333333p+1,
                -0x1.0dc2c9c7bd2dfp-60, -0x1.6278f622c571bp+15,
                -0x1.a95df429b9bbap+13, -0x1.f042f230ae058p+14 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.7c6ea44fd360dp-21, 0x1.c884c52c9740fp-23,
                0x1.0a4d7304ad908p-21, -0x1.07c72542a016fp+9,
                -0x1.3c88931cc01b9p+7, -0x1.714a00f6e02p+8, -0x1.2bp-22,
                -0x1.66ccccccccccdp-24, -0x1.a299999999999p-23 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.05p-45, -0x1.3933333333332p-47, -0x1.6d66666666666p-46,
                -0x1.98f6690d325a7p-40, -0x1.eac14adca2d32p-42,
                -0x1.1e46165609a5dp-40, -0x1.353158dfd5dfcp-47,
                -0x1.7308043fcd72ep-49, -0x1.b0deaf9fc505ep-48 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.ap+5, 0x1.f333333333333p+3, 0x1.2333333333331p+5,
                0x1.e9396289baa4fp+18, 0x1.2588d4b909964p+17,
                0x1.5674f82d35d9fp+18, 0x1.cp+3, 0x1.0cccccccccccdp+2,
                0x1.3999999999999p+3 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.e8cb9e408719ap+18, 0x1.2546f88d1ddc3p+17,
                0x1.562821f9f82b8p+18, 0x1.3cdd0a69c9b49p-42,
                0x1.7c3c72e558724p-44, 0x1.bb9bdb60e72fdp-43, -0x1p-61,
                -0x1.3333333333333p-63, -0x1.6666666666666p-62 } },
    };

    const std::vector< RegressionCase > ALIGNED_3D_CASES{
        { "filter", 0,
            { 0x1.a8p+5, -0x1.59183079c12abp-40, 0x1.9c3e299232779p+24,
                -0x1.b000000000001p+4, 0x1.e87035a3820ddp+18,
                0x1.d24511968a1ecp+37, 0x1.58p+6, -0x1.88p+6,
                -0x1.512676b182d02p+22 } },
        { "filter", 0,
            { -0x1.856edb339ab87p-54, -0x1.c7ffffffffffdp+5, -0x1.18p+6,
                0x1.e7be2e932563p+18, 0x1.928p+7, 0x1.56166bf8c7cfap-41,
                -0x1.17ffffffffffep+5, 0x1.e93fdd899b1a3p+18,
                0x1.6200000000001p-17 } },
        { "filter", 0,
            { -0x1.6p+5, 0x1.e8b7220826db4p+18, -0x1.991b997468855p+14,
                0x1.868bf64a9c8cfp-9, -0x1.6p+5, 0x1.139ca39e9c9efp+1,
                0x1.e8b7220826db4p+18, 0x1.6fffffffffffep-10,
                -0x1.8d76f7d1f2fb7p+22 } },
        { "zero-filter", 1,
            { -0x1.856edb339ab87p-54, -0x1.c7ffffffffffdp+5, -0x1.18p+6,
                -0x1.856edb339ab87p-54, -0x1.c7ffffffffffdp+5, -0x1.18p+6,
                -0x1.7222a2c932b4ep-35, 0x1.d9bfffffffffep+10,
                -0x1.1800000000003p+6 } },
        { "zero-filter", 1,
            { -0x1.0900000000001p-13, 0x1.99ef178098934p-38, 0x1.5p+6,
                -0x1.0900000000001p-13, 0x1.99ef178098934p-38, 0x1.5p+6,
                -0x1.0900000000001p-13, 0x1.9e247570c34bep-38,
                0x1.5000000005b28p+6 } },
        { "zero-filter", 1,
            { 0x1.e893c5db9020cp+18, 0x1.6p+5, 0x1.8cd292e44f0b4p+30,
                0x1.e893c4979020cp+18, 0x1.e897717ee749cp+18,
                0x1.8cd292e44f0b4p+30, 0x1.e893c5db9020cp+18, 0x1.6p+5,
                0x1.8cd292e44f0b4p+30 } },
        { "exact", 0,
            { 0x1.a8p+5, -0x1.59183079c12abp-40, 0x1.9c3e299232779p+24,
                -0x1.e932439f9b65cp+19, 0x1.a7fffffffffaap+6,
                0x1.9c3e259232779p+24, 0x1.d2687f0d0135ep+37,
                -0x1.943a3cdcd4838p+24, 0x1.ab7f27099feb9p+24 } },
        { "exact", 0,
            { -0x1.6p+5, 0x1.e8b7220826db4p+18, -0x1.991b997468855p+14,
                0x1.6e1b8727c9f84p+20, 0x1.e8b4220826db4p+18,
                -0x1.9957997468855p+14, 0x1.d21bfbf9f405fp+37,
                -0x1.6eb2269928741p+20, -0x1.34be2ca3e82bcp+21 } },
        { "exact", 0,
            { -0x1.9p+5, -0x1.dffffffffffffp+3, -0x1.18p+5,
                -0x1.67fffffffffffp+6, -0x1.bp+4, -0x1.f7fffffffffffp+5,
                -0x1.c1182267e4d41p-32, -0x1.0d74e171894cp-33,
                -0x1.3a5db1af202ep-32 } },
        { "exact", 1,
            { 0x1.e8420023b3205p+18, 0x1.24f4667bd1e03p+17,
                0x1.55c7cce5ca303p+18, 0x1.e8538023b3205p+18,
                0x1.24f4d4fbd1e03p+17, 0x1.55c7cce5ca303p+18,
                0x1.e6b84023b3205p+18, 0x1.24eab03bd1e03p+17,
                0x1.55c7cce5ca303p+18 } },
        { "exact", 1,
            { 0x1.e751397690a99p+18, 0x1.2463ef472398fp+17,
                0x1.551f41d2fedd2p+18, 0x1.e7513976c7d99p+18,
                0x1.2463ef472398fp+17, 0x1.552ac1d2fedd2p+18,
                0x1.e751397302119p+18, 0x1.2463ef472398fp+17,
                0x1.546181d2fedd2p+18 } },
        { "exact", 1,
            { 0x1.e899e77c6ee34p+18, 0x1.2529247ddc222p+17,
                0x1.5605553d80d25p+18, 0x1.e899e77c6f03ap+18,
                0x1.24e7247ddc222p+17, 0x1.55f7853d80d25p+18,
                0x1.e899e77c6cbcep+18, 0x1.298b247ddc222p+17,
                0x1.56f0253d80d25p+18 } },
        { "exact, wrong floating-point sign", 0,
            { -0x1.da00000000001p-42, -0x1.2p-5, 0x1.e77ab05ec953bp+18,
                0x1.ad381c83a9b4p-7, 0x1.beep+3, -0x1.ec0bd7aaf1401p+9,
                0x1.4f53d6470a682p+0, 0x1.5dfdcp+10, -0x1.79c919098e7dp+25 } },
        { "exact, wrong floating-point sign", 0,
            { -0x1.dep-45, -0x1.1eccccccccccdp-46, -0x1.4e99999999998p-45,
                0x1.e771cc5a15fep+18, 0x1.24777a9c73986p+17,
                0x1.55360f0bdc31cp+18, -0x1.08p+5, -0x1.3cccccccccccdp+3,
                -0x1.7199999999999p+4 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1.3fc42b4d9a261p-68, 0x1.e7dadd360a0ap+18,
                0x1.e8a6c0473dbe2p+18, -0x1.38bfff601dea6p-43,
                0x1.e7e79d360a0ap+18, 0x1.e8a6c0473dbe2p+18,
                -0x1.4285fffb00ef5p-38, 0x1.e97f9d360a0ap+18,
                0x1.e8a6c0473dbe2p+18 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1.e896e69f1f5dcp+18, 0x1.2527572c46051p+17,
                0x1.56033b08fc5b3p+18, -0x1.a8p+5, -0x1.fccccccccccccp+3,
                -0x1.28cccccccccccp+5, -0x1.e6p-34, -0x1.2399999999999p-35,
                -0x1.5433333333333p-34 } },
        { "exact, wrong floating-point sign", 0,
            { -0x1.64af0ce37a57cp+9, 0x1.4p+6, -0x1.5315df845a20ap+28,
                -0x1.6e0799c8298ap+20, 0x1.3fffc4cp+6, -0x1.5315df845a20ap+28,
                0x1.3818ccec51d2ap+16, 0x1.4000032fecp+6,
                -0x1.5315df845a20ap+28 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1.19298d76cc428p+13, 0x1.51651028284fdp+11,
                0x1.89a092d9845d1p+12, -0x1.05p-45, -0x1.3933333333332p-47,
                -0x1.6d66666666666p-46, -0x1.98f6690d325a7p-40,
                -0x1.eac14adca2d32p-42, -0x1.1e46165609a5dp-40 } },
        { "exact, wrong floating-point sign", 0,
            { -0x1.f226719f9689fp+8, -0x1.2ae3ddc6271f9p+7,
                -0x1.5cb482bc82fa2p+8, 0x1.e92a021750d96p+18,
                0x1.257f9adaca1cp+17, 0x1.566a34a9ebcb5p+18, -0x1.7p+5,
                -0x1.b999999999999p+3, -0x1.0199999999999p+5 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1.237fffffffffdp-25, 0x1.237fffffffffdp-25,
                0x1.9819999999999p-26, 0x1.237fdfefffffdp-25,
                -0x1.e76a549cdcc25p+19, -0x1.e84ff91e2ceep+19,
                0x1.238200ffffffdp-25, 0x1.e76a549cdcd5ap+23,
                0x1.e84ff91e2cfb9p+23 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.4p+3, -0x1.8p+1, -0x1.cp+2, 0x1.d0426ca3c1222p-32,
                0x1.168e412f0d7aep-33, 0x1.44fb4c0c3a64bp-32,
                0x1.7a6a5352872f4p+0, 0x1.c61930c96f058p-2,
                0x1.08e407202b6dep+0 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.e777dd8d5babp+18, 0x1.247b1e8803cdp+17, 0x1.553a4e4959c48p+18,
                0x1.a9p-25, 0x1.fep-27, 0x1.298p-25, -0x1.b58p-27, -0x1.068p-28,
                -0x1.324p-27 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.2a792afadb578p-51, 0x1.662b00603a69p-53,
                0x1.a1dcd5c5997a8p-52, -0x1.9p+3, -0x1.ep+1, -0x1.18p+3,
                0x1.7cp-5, 0x1.c8p-7, 0x1.0ap-5 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.e79c124264023p+18, 0x1.2490d7c16f348p+17,
                0x1.5553a661ac67ep+18, 0x1.e79c124264023p+18,
                0x1.2490d7c08b0e7p+17, 0x1.c40cdd9f56952p+20,
                0x1.e79c124264023p+18, 0x1.2490d7c12327dp+17,
                0x1.9f2475e01ddb6p+19 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.7ep-3, 0x1.078p-43, 0x1.6bd0af8bbabd8p+16,
                0x1.7e00000000b5ep-3, -0x1.a8012e71b6594p-3,
                0x1.6cf9af8bbabd8p+16, 0x1.7e000000071acp-3,
                -0x1.0900bd07128cp+1, 0x1.776aaf8bbabd8p+16 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.9p+6, -0x1.ep+4, -0x1.18p+6, -0x1.9p+5, -0x1.ep+3,
                -0x1.18p+5, 0x1.6e2c82cc348a6p-44, 0x1.b7689cf50bd94p-46,
                0x1.00525b8ef1941p-44 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.84p-7, 0x1.e7865ebbcefb1p+18, 0x1.e7ec56ae751efp+18,
                0x1.e37p-1, -0x1.e8b48df943279p+18, 0x1.e7f9aeae751efp+18,
                0x1.57fd8p+4, -0x1.4f969dbcb9244p+24, 0x1.e91892ae751efp+18 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.10b9908c3edb8p-62, -0x1.474513db7ea1p-64,
                -0x1.7dd0972abe668p-63, -0x1.fa216dcf1316ep-59,
                -0x1.2fada8490b742p-60, -0x1.624a99aa8d5cdp-59, -0x1.4p+6,
                -0x1.8p+4, -0x1.cp+5 } },
    };

    const std::vector< RegressionCase > DOT_2D_CASES{
        { "filter", -1,
            { -0x1.b28p-26, -0x1.04b3333333333p-27, -0x1.b2800000ca5ddp-26,
                -0x1.04b3334873335p-27, -0x1.b27ffff560003p-26,
                -0x1.04b33334c7ee7p-27 } },
        { "filter", -1,
            { 0x1.e865a729a8d94p+18, 0x1.e883f706d5e17p+18,
                0x1.e87be729a8d97p+18, 0x1.e883f7a5d5e17p+18,
                0x1.e865a68aa8d93p+18, 0x1.e89a3706d5e17p+18 } },
        { "filter", -1,
            { -0x1.98p+5, -0x1.2p+4, -0x1.97feefp+5, -0x1.2000000000003p+4,
                -0x1.97fffffffffffp+5, -0x1.1ffddep+4 } },
        { "filter", 1,
            { 0x1.e8ab49655d035p+18, 0x1.e8bc79d913b37p+18,
                0x1.e8b309655d035p+18, 0x1.e8a379d913b37p+18,
                0x1.e8c449655d037p+18, 0x1.e8c439d913b37p+18 } },
        { "filter", 1,
            { 0x1.3fc42b4d9a261p-68, 0x1.e7dadd360a0ap+18, 0x1.3800000000003p+5,
                0x1.e7dadd360a0ap+18, 0x1.9ceb4ee6751fep-34,
                0x1.e7e49d360a0ap+18 } },
        { "filter", 1,
            { 0x1.e8296e4e4cdddp+18, 0x1.e8ca0f62ded75p+18,
                0x1.e8296e4e4cd7bp+18, 0x1.e8d54f62ded75p+18,
                0x1.e81e2e4e4cdddp+18, 0x1.e8ca0f62ded14p+18 } },
        { "zero-filter", 0,
            { -0x1.1266666666666p-1, -0x1.d666666666669p-3,
                -0x1.17d6666666666p-1, -0x1.d666666666669p-3,
                -0x1.1266666666666p-1, -0x1.ec26666666669p-3 } },
        { "zero-filter", 0,
            { 0x1.e82f4ccdfe8c2p+18, 0x1.e82f4ccdfe8c2p+18,
                0x1.e82f4ccdfe8c2p+18, 0x1.e82f4ccdfe96bp+18,
                0x1.e82f4ccdfe819p+18, 0x1.e82f4ccdfe8c2p+18 } },
        { "zero-filter", 0,
            { 0x1.9p+6, 0x1.ep+4, 0x1.e8b0dd41f25ecp+18, 0x1.ep+4, 0x1.9p+6,
                0x1.e89f5d41f25ebp+18 } },
        { "exact", -1,
            { 0x1.a8p+5, -0x1.59183079c12abp-40, 0x1.a800000002422p+5,
                0x1.2ffffffffffaap+6, -0x1.7p+4, 0x1.1bab4641aa1c9p-34 } },
        { "exact", -1,
            { -0x1.856edb339ab87p-54, -0x1.c7ffffffffffdp+5, -0x1.0cp+6,
                -0x1.ff6fffffffffdp+5, 0x1.bb8p+2, -0x1.efffffffffffep+6 } },
        { "exact", -1,
            { -0x1.6p+5, 0x1.e8b7220826db4p+18, 0x1.e74e1abdb21a7p+18,
                0x1.e8c9e20826db3p+18, -0x1.dcp+6, 0x1.e8081e62ec7aep+19 } },
        { "exact", 0,
            { 0x1.253f30d6aabfdp+17, 0x1.253f30d6aabfdp+17,
                0x1.253f30d6a91c5p+17, 0x1.253f3ba1fc1b1p+17,
                0x1.253f260b59649p+17, 0x1.253f30d6a91c5p+17 } },
        { "exact", 0,
            { 0x1.e7f6ae2f84e42p+18, 0x1.24c7354fb6227p+17,
                0x1.e7f6ae33cce42p+18, 0x1.24c7354f3b227p+17,
                0x1.e7f6ae2fc2642p+18, 0x1.24c7355846227p+17 } },
        { "exact", 0,
            { -0x1.58p-28, 0x1.1b8p-23, 0x1.e85e2466c8b8dp+18, 0x1.c580011b8p+1,
                -0x1.c580000acp+1, 0x1.e85e2466c94bfp+18 } },
        { "exact", 1,
            { -0x1.9p+5, -0x1.dffffffffffffp+3, -0x1.5cp+6, -0x1.ccp+6,
                0x1.9p+5, -0x1.ap+5 } },
        { "exact", 1,
            { -0x1.0900000000001p-13, 0x1.99ef178098934p-38, -0x1.3c00212p+6,
                -0x1.8fffffffff998p+4, 0x1.8fff7b7fffffep+4,
                -0x1.3bffffffffe66p+6 } },
        { "exact", 1,
            { -0x1.659fdac2e6cd6p-47, 0x1.f1ec2241e0388p+0,
                0x1.368ffbdf911eap-45, 0x1.e77eefdbe1dap+18,
                -0x1.e77e7360d9497p+18, 0x1.f1ec2241e045p+0 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.856edb339ab87p-54, -0x1.c7ffffffffffdp+5, -0x1.0cp+6,
                -0x1.ff6fffffffffdp+5, 0x1.bb8p+2, -0x1.efffffffffffep+6 } },
        { "exact, wrong floating-point sign", -1,
            { 0x1.8990fdb9327fp-14, 0x1.6f2adcf12ab38p-35, 0x1.580031321fb72p+5,
                -0x1.1fffffffff487p+6, 0x1.200018990fdb9p+6,
                0x1.58000000016f2p+5 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.29e0d5bbdd285p-41, 0x1.e7c39a67e949ep+18,
                0x1.e77056f8407e6p+18, 0x1.e7c39a685449ep+18,
                -0x1.ac000094f06aep-16, 0x1.e799f8b014e42p+19 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.2b42e5f8852e8p+12, 0x1.0800000000003p+6,
                -0x1.2872e5f8852e8p+12, 0x1.e83df65cd15dp+18,
                -0x1.ecda81f4b371cp+18, 0x1.bc00000000003p+6 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.37ffffffffffep-10, -0x1.7666666666665p-12, -0x1.880138p+6,
                0x1.e8c5eaeb5fba3p+18, -0x1.e8c5eb04b953dp+18,
                -0x1.88005d999999ap+6 } },
        { "exact, wrong floating-point sign", -1,
            { 0x1.90e9fdc1c0a6dp-19, 0x1.ap-53, 0x1.90ea0579c0a6dp-19, 0x1.8p+6,
                -0x1.7fffff378b012p+6, 0x1.ee1ap-41 } },
        { "exact, wrong floating-point sign", -1,
            { 0x1.3cp-6, 0x1.bfc87ed643f8fp-53, 0x1.e8bea5ff7e81bp+18,
                -0x1.52p+2, 0x1.533cp+2, 0x1.e8bea4c37e81bp+18 } },
        { "exact, wrong floating-point sign", -1,
            { 0x1.e87e1e821d5bfp+18, 0x1.e8c3a2a21855ep+18,
                0x1.e88f1e821d5bfp+18, 0x1.e88a9d757e0eep+19, 0x1.6431c9ccap+7,
                0x1.e8d4a2a21855ep+18 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.9p+5, -0x1.dffffffffffffp+3, -0x1.5cp+6, -0x1.ccp+6,
                0x1.9p+5, -0x1.ap+5 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.dep-45, -0x1.1eccccccccccdp-46, 0x1.e7fa85c890f16p+18,
                -0x1.1ed42eccccccdp-46, -0x1.ddfc4fp-45,
                0x1.e7fa85c890f16p+18 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.e5424739d220fp-1, -0x1.048p-2, 0x1.e9364a34c9007p+18,
                0x1.6afb8p+6, -0x1.6fca848e73a44p+6, 0x1.e936769511e7bp+18 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.dc00000000003p+6, 0x1.1d99999999999p+5, 0x1.e805cf67d1a7ap+18,
                0x1.ed99999999999p+5, 0x1.7400000000003p+6,
                0x1.e7f0fc349e747p+18 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.1cp+6, -0x1.54ccccccccccdp+4, -0x1.1bff5fp+6,
                -0x1.8933333333333p+6, 0x1.8p+2, -0x1.54ca48ccccccdp+4 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.1f3309ffffeb7p+5, 0x1.598p-58, -0x1.21eb09ffffeb7p+5,
                -0x1.4p+5, 0x1.0667b00000a48p+2, -0x1.5cp-2 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.51f64c27546d3p-60, -0x1.b18p-55, -0x1.e8fdf392ca9ccp-3,
                -0x1.8p+6, 0x1.8p+6, -0x1.e8fdf392ca9cep-3 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.cp-35, 0x1.82p-38, 0x1.e7cf52cbc3b16p+18, 0x1.8a00000608p-8,
                -0x1.8a000038p-8, 0x1.e7cf52cbc3b17p+18 } },
    };

    const std::vector< RegressionCase > DOT_3D_CASES{
        { "filter", -1,
            { 0x1.e7cc178e1e3d6p+18, 0x1.e859b4d621535p+18,
                0x1.d2743f827c8f3p+37, 0x1.e8269b370deb8p+19,
                0x1.e876abf54b1fp+19, 0x1.d2747c8d304e7p+37,
                0x1.e7cc178e34e46p+18, 0x1.d2133725c13dp+37,
                0x1.30a05f94d8p+26 } },
        { "filter", -1,
            { -0x1.48p+6, 0x1.12p-23, -0x1.57fff900dp+5, 0x1.ap+3,
                0x1.e8f040dc8c294p+18, 0x1.e75eb8dbbdb1p+18,
                -0x1.8a34a00c23ac9p+27, 0x1.12p-23, -0x1.5829fda662fa9p+5 } },
        { "filter", -1,
            { 0x1.e81e113ffe3ffp+18, -0x1.951b24fc9e053p-61,
                -0x1.1a2cfebefe0eep+24, 0x1.e80c113ffe3ffp+18,
                0x1.e93676a7f1feep+18, -0x1.1a2cfebefde76p+24,
                0x1.6ee013790b425p+23, -0x1.1a2cfebefe0eep+24,
                -0x1.9f43d7f61f2d1p+25 } },
        { "filter", 1,
            { 0x1.a8p+5, -0x1.59183079c12abp-40, 0x1.9c3e299232779p+24,
                0x1.a800000002422p+5, 0x1.2ffffffffffaap+6,
                0x1.9c42599f3b2b2p+24, -0x1.5b5f39d239fa3p+16,
                -0x1.a79318f9a682p-28, 0x1.9c3e29923277bp+24 } },
        { "filter", 1,
            { 0x1.e893c5db9020cp+18, 0x1.6p+5, 0x1.8cd292e44f0b4p+30,
                0x1.e89385db90209p+18, 0x1.e84fa98e6e9b9p+18,
                0x1.8cd291844f0b4p+30, -0x1.c9bde6131ea5bp+23,
                0x1.8cd292e44f0b4p+30, 0x1.92e6edc0c50bdp+30 } },
        { "filter", 1,
            { -0x1.29e0d5bbdd285p-41, 0x1.e7c39a67e949ep+18,
                0x1.d080b05e9c91ep+37, 0x1.e77056f8407e5p+18,
                0x1.e7c39a685449ep+18, 0x1.d080b05e9c921p+37,
                0x1.e723a3c602196p-14, -0x1.b0d99bbc637bp+20,
                0x1.d089f8a2448c2p+37 } },
        { "zero-filter", 0,
            { 0x1.e79730b3f9a71p+18, 0x1.e928518961dap+18, 0x1.b6602e2b60a6p+25,
                0x1.e79730b3f9a71p+18, 0x1.e928518961dap+18,
                0x1.ba2f76ff22921p+25, -0x1.f01368fdcc978p+20,
                0x1.e9285188f72e1p+18, 0x1.b6602e2b60a6p+25 } },
        { "zero-filter", 0,
            { -0x1.cp+5, -0x1.bp+5, -0x1.042574824591cp+15, -0x1.cp+5,
                0x1.8e7292099b84ap+12, -0x1.042574824591cp+15,
                -0x1.c01ccccfd670dp+5, -0x1.bp+5, 0x1.19f7f11afd135p+19 } },
        { "zero-filter", 0,
            { 0x1.e9159e4067582p+18, 0x1.e9159e4067582p+18,
                0x1.565beec6aebd9p+18, 0x1.e9159e4067582p+18,
                0x1.e89d7a097af1ap+19, 0x1.565beec6aebd9p+18,
                -0x1.f762bf5f6b9c5p+24, 0x1.e9159e4067582p+18,
                -0x1.2de24fc59f765p+32 } },
        { "exact", -1,
            { -0x1.9p+5, -0x1.dffffffffffffp+3, -0x1.18p+5, -0x1.5cp+6,
                -0x1.cbffffffffffdp+6, -0x1.18p+5, -0x1.9p+5,
                -0x1.dfffffffff8c1p+3, 0x1.e06fe177a24e6p+24 } },
        { "exact", -1,
            { -0x1.659fdac2e6cd6p-47, 0x1.f1ec2241e0388p+0,
                0x1.f1ec2241e0388p+0, 0x1.368ffbdf911eap-45,
                0x1.e77eefdbe1d9fp+18, 0x1.f1ec259aa56a2p+0,
                -0x1.faaef580eb99dp-35, 0x1.f1ec54756e20ep+0,
                -0x1.c90684631b2bep+22 } },
        { "exact", -1,
            { -0x1.5dp-51, -0x1.a2ccccccccccbp-53, -0x1.e899999999999p-52,
                -0x1.1p+9, 0x1.e78046bc350dcp+18, -0x1.1c80000000003p-1,
                0x1.d0d1fe1a98df1p+37, 0x1.0358526bf19e9p+28,
                -0x1.0d041fc80bc0cp+28 } },
        { "exact", 0,
            { 0x1.e78ec14fa2f87p+18, 0x1.2488da62fb61ep+17,
                0x1.554a541e25478p+18, 0x1.e78ec14fa2f87p+18,
                0x1.2488e16afb61ep+17, 0x1.5558d41e25478p+18,
                0x1.013462ec596f9p+19, 0x1.227eda62fb61ep+17,
                0x1.554a936625478p+18 } },
        { "exact", 0,
            { -0x1.cp+5, -0x1.48p+5, 0x1.ffffffffdbcep+2, -0x1.bfffc6bp+5,
                -0x1.47f44p+5, 0x1.ffffffffdbcep+2, -0x1.c2e44p+5,
                -0x1.47f1e55p+5, 0x1.ffba2fd6fbce1p+2 } },
        { "exact", 0,
            { 0x1.14p+6, -0x1.d4p-29, -0x1.34p-37, 0x1.14p+6, 0x1.44bp-23,
                0x1.a57ffffffff66p+8, 0x1.1400033899fp+6, -0x1.c323000eap+0,
                0x1.5e88p-31 } },
        { "exact", 1,
            { -0x1.856edb339ab87p-54, -0x1.c7ffffffffffdp+5, -0x1.18p+6,
                -0x1.0cp+6, -0x1.ff6ffffffffffp+5, 0x1.e7fcfa74ea5dbp+18,
                0x1.e80e4304ea5dbp+21, 0x1.960f73dd7b633p+7,
                0x1.d200af686p+8 } },
        { "exact", 1,
            { -0x1.6p+5, 0x1.e8b7220826db4p+18, -0x1.991b997468855p+14,
                0x1.e74e1abdb21a7p+18, 0x1.e8c9e20826db4p+18,
                -0x1.991bb6ec68855p+14, 0x1.e1424ep+8, -0x1.6d571324f6f78p+21,
                0x1.10890371550d3p+22 } },
        { "exact", 1,
            { -0x1.1266666666666p-1, -0x1.d666666666669p-3,
                -0x1.1266666666666p-1, -0x1.17d6666666666p-1,
                -0x1.d666666666669p-3, 0x1.e769e81ed2783p+18,
                -0x1.d0e5d1ddf348bp+37, -0x1.d734650b69dd3p-3,
                -0x1.4bf5157c3fea7p+12 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.659fdac2e6cd6p-47, 0x1.f1ec2241e0388p+0,
                0x1.f1ec2241e0388p+0, 0x1.368ffbdf911eap-45,
                0x1.e77eefdbe1d9fp+18, 0x1.f1ec259aa56a2p+0,
                -0x1.faaef580eb99dp-35, 0x1.f1ec54756e20ep+0,
                -0x1.c90684631b2bep+22 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.2b42e5f8852e8p+12, 0x1.0800000000003p+6,
                0x1.0800000000003p+6, -0x1.2872e5f8852e8p+12,
                0x1.e83df65cd15dp+18, 0x1.e8c4eaf2ebcc8p+18,
                0x1.d10e1a04806eap+37, -0x1.56f297434afa5p+24,
                0x1.0800000007baap+6 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.9a5f26c60cb8fp+8, -0x1.0cp+0, -0x1.bbcc06f7356c6p+4,
                -0x1.4e5f26c60cb8fp+8, -0x1.f0cp+4, -0x1.55e6037b9ab63p+5,
                0x1.83506c9ade856p+9, -0x1.0bd7174062753p+0,
                0x1.75842eda9b65fp+12 } },
        { "exact, wrong floating-point sign", -1,
            { 0x1.1a74d6f7a25dap-7, 0x1.52f29b8f8fa39p-9, 0x1.8b7060277ce97p-8,
                0x1.1a74d706d25dap-7, -0x1.23fd5a1ac8e1p+6,
                0x1.e88d55330741ep+18, 0x1.1a74dfcf00a82p-7,
                0x1.d12e2967c06b1p+37, 0x1.1607f3cc81ff7p+25 } },
        { "exact, wrong floating-point sign", -1,
            { 0x1.c333333333336p-22, 0x1.c333333333336p-22,
                0x1.0733333333332p-20, 0x1.53ee15db06c93p+8,
                0x1.cc6b333333336p-22, -0x1.73ffffbe33333p+6,
                -0x1.b9bffffe3cccdp+10, -0x1.62cf55d463786p+25,
                -0x1.93aacb8095446p+12 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.9p+5, -0x1.ep+3, -0x1.18p+5, -0x1.fp+6,
                -0x1.dffffffffe38fp+3, 0x1.e8bc626a1d3edp+18,
                -0x1.d2c5c898f7f04p+37, -0x1.0b4c9c11bf92ap+25,
                -0x1.1aadb66211e62p+25 } },
        { "exact, wrong floating-point sign", -1,
            { -0x1.4p-51, -0x1.8p-53, -0x1.cp-52, -0x1.70027ffffffffp-36,
                0x1.e7f731b33f8fap+18, -0x1.7918dfb95dde7p-36,
                0x1.62899a1c3c2e6p+25, -0x1.67ad3952130a3p-17,
                -0x1.d1842b539c9bap+37 } },
        { "exact, wrong floating-point sign", -1,
            { 0x1.1c00000000001p+6, 0x1.54ccccccccccdp+4, 0x1.8d99999999999p+5,
                0x1.1bfffffffea3bp+6, 0x1.e7d8817d72fb7p+18,
                0x1.e8d84b0395501p+18, 0x1.891bd8747e5f6p+25,
                0x1.4cb618cd96c8cp+20, -0x1.4c08630ed6b55p+20 } },
        { "exact, wrong floating-point sign", 0,
            { -0x1.ep+4, -0x1.74p+6, -0x1.0cp-28, -0x1.38p+5, -0x1.8ap+6,
                -0x1.0ec2p-28, -0x1.3cp+8, 0x1.76fffffff761cp+8,
                0x1.12ffffffef4p+8 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1.1dp-30, 0x1.56p-32, 0x1.8fp-31, -0x1.bffffffee3p+2,
                -0x1.7fffffffd54p+3, 0x1.8effc67p-31, 0x1.1d96714454e3cp-8,
                -0x1.4d2f7c472a8f9p-9, 0x1.81000000031ep+8 } },
        { "exact, wrong floating-point sign", 0,
            { -0x1.d9p+0, -0x1p+4, -0x1.6d62fe3a07a2ep+11, -0x1.ad9p+4,
                0x1.cp+5, -0x1.62e2fe3a07a2ep+11, -0x1.5f1d90000a5b4p+12,
                -0x1.eb84cddp+10, -0x1.6d60ef2a0dcdp+11 } },
        { "exact, wrong floating-point sign", 0,
            { -0x1.f4p-16, -0x1.2cp-17, -0x1.5ep-16, -0x1.40001f4p+4,
                -0x1.900004bp+5, -0x1.5dffd6ep-16, -0x1.3880003e89b81p+11,
                0x1.f3ffffb5p+9, -0x1.2e80000afp+13 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1.e7795ffaec358p+18, 0x1.247c06635a868p+17,
                0x1.553b5cc93ef24p+18, 0x1.e77961346c358p+18,
                0x1.3dacaf7b30e48p+19, 0x1.553b5cc93ef24p+18,
                -0x1.1339835b42104p+22, 0x1.247c1ee15a868p+17,
                0x1.0fb09ac34a482p+17 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1.e8ebe9ad83f6fp+18, 0x1.7p+4, 0x1.7p+4, 0x1.e8d769ad83f6fp+18,
                0x1.e75906ccd0f3fp+18, 0x1.cp+4, -0x1.3091147a41327p+25,
                -0x1.9efp+12, 0x1.e6ded6ccd0f3fp+20 } },
        { "exact, wrong floating-point sign", 0,
            { 0x1.e77666d238cb4p+18, 0x1.e77666d238cb4p+18, -0x1.18p+7,
                0x1.e761e6d238cb4p+18, 0x1.e78ae6d238cb4p+18,
                0x1.e905af138482p+18, 0x1.5030d3d944b6ap+22,
                -0x1.41097c05d88ep+24, 0x1.06ep+12 } },
        { "exact, wrong floating-point sign", 0,
            { -0x1.6cp+6, 0x1.e805224d11858p+18, -0x1.58011f26c95d3p+25,
                -0x1.9p+6, 0x1.e805221d11858p+18, -0x1.58011f26c4561p+25,
                -0x1.849b47299ae46p+10, 0x1.30e4c7840543ap+22,
                -0x1.58011f26c95d3p+25 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.856edb339ab87p-54, -0x1.c7ffffffffffdp+5, -0x1.18p+6,
                -0x1.0cp+6, -0x1.ff6ffffffffffp+5, 0x1.e7fcfa74ea5dbp+18,
                0x1.e80e4304ea5dbp+21, 0x1.960f73dd7b633p+7,
                0x1.d200af686p+8 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.518p-41, 0x1.95p-43, 0x1.d87ffffffffffp-42,
                0x1.3400000000015p+7, -0x1.57ffffffffff3p+6,
                0x1.c000000000076p+4, 0x1.41ffe941f7dd8p+11,
                0x1.ab42da55be6c3p+23, 0x1.47f7a95f38d58p+25 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.a7f0079a5f538p-68, -0x1.a8p+5, 0x1.aa1f61697967fp-21,
                -0x1.c08p-2, 0x1.48p+5, 0x1.e76537a36d015p+18,
                -0x1.d116cca7c26eep+37, -0x1.a7ffffffffbf2p+5,
                -0x1.abf949b01e5bep+17 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.808p-52, -0x1.51p-59, 0x1.56p-12, -0x1.96a0b89232093p-4,
                0x1.e7639d282d197p+18, 0x1.5618340be28a6p-12,
                0x1.d0dcdb6effdb4p+37, 0x1.83d59df0482a5p+15,
                -0x1.d0e17cb5580b3p+37 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.07119fb168bc4p-69, -0x1.3baebfa1b0e1ep-71,
                -0x1.704bdf91f9078p-70, 0x1.e752ebad36336p+18, -0x1.58p+5,
                -0x1.18p+5, -0x1.5ep+11, 0x1.2520000000002p+11,
                -0x1.309973cc41e02p+25 } },
        { "exact, wrong floating-point sign", 1,
            { -0x1.56p-52, 0x1.e7a6dda937f5ap+18, -0x1.2fffb9a35c646p+5,
                -0x1.bc738e95275f6p+15, 0x1.e7a6dda937f5ap+18,
                -0x1.300479a35c646p+5, -0x1.d47800000055bp-10,
                -0x1.a76adbf4e21f6p+19, 0x1.562809ae03bf4p+15 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.2c792b7ba008ap-7, 0x1.689167612670cp-9, 0x1.a4a9a346acd8bp-8,
                0x1.e84b1a5da13a7p+18, -0x1.4ef4bb74c4f6bp+4,
                -0x1.b4ec86642a94cp-3, 0x1.ad4a2224dac56p+16,
                -0x1.ab1afbabf6ffp+16, 0x1.d1c49265f2b1cp+37 } },
        { "exact, wrong floating-point sign", 1,
            { 0x1.7ap-61, 0x1.c599999999999p-63, 0x1.0899999999999p-61,
                0x1.ce1c5c71a34e5p-31, -0x1.aea6666666666p-59, -0x1.cp+2,
                0x1.aa8f48d0c579ep+21, -0x1.f8f60f973ac25p+11,
                0x1.b7fec40b4e957p-12 } },
    };

    void test_regression_cases()
    {
        for( const auto& test_case : ORIENT_2D_CASES )
        {
            const auto& v = test_case.values;
            geode::OpenGeodeGeometryException::test(
                GEO::PCK::orient_2d( geode::Point2D{ { v[0], v[1] } },
                    geode::Point2D{ { v[2], v[3] } },
                    geode::Point2D{ { v[4], v[5] } } )
                    == test_case.expected,
                "[Test] Wrong orient_2d on case decided by ", test_case.stage );
        }
        for( const auto& test_case : ORIENT_3D_CASES )
        {
            const auto& v = test_case.values;
            geode::OpenGeodeGeometryException::test(
                GEO::PCK::orient_3d( geode::Point3D{ { v[0], v[1], v[2] } },
                    geode::Point3D{ { v[3], v[4], v[5] } },
                    geode::Point3D{ { v[6], v[7], v[8] } },
                    geode::Point3D{ { v[9], v[10], v[11] } } )
                    == test_case.expected,
                "[Test] Wrong orient_3d on case decided by ", test_case.stage );
        }
        for( const auto& test_case : DET_3D_CASES )
        {
            const auto& v = test_case.values;
            geode::OpenGeodeGeometryException::test(
                GEO::PCK::det_3d( geode::Vector3D{ { v[0], v[1], v[2] } },
                    geode::Vector3D{ { v[3], v[4], v[5] } },
                    geode::Vector3D{ { v[6], v[7], v[8] } } )
                    == test_case.expected,
                "[Test] Wrong det_3d on case decided by ", test_case.stage );
        }
        for( const auto& test_case : ALIGNED_3D_CASES )
        {
            const auto& v = test_case.values;
            geode::OpenGeodeGeometryException::test(
                GEO::PCK::aligned_3d( geode::Point3D{ { v[0], v[1], v[2] } },
                    geode::Point3D{ { v[3], v[4], v[5] } },
                    geode::Point3D{ { v[6], v[7], v[8] } } )
                    == ( test_case.expected == 1 ),
                "[Test] Wrong aligned_3d on case decided by ",
                test_case.stage );
        }
        for( const auto& test_case : DOT_2D_CASES )
        {
            const auto& v = test_case.values;
            geode::OpenGeodeGeometryException::test(
                GEO::PCK::dot_2d( geode::Point2D{ { v[0], v[1] } },
                    geode::Point2D{ { v[2], v[3] } },
                    geode::Point2D{ { v[4], v[5] } } )
                    == test_case.expected,
                "[Test] Wrong dot_2d on case decided by ", test_case.stage );
        }
        for( const auto& test_case : DOT_3D_CASES )
        {
            const auto& v = test_case.values;
            geode::OpenGeodeGeometryException::test(
                GEO::PCK::dot_3d( geode::Point3D{ { v[0], v[1], v[2] } },
                    geode::Point3D{ { v[3], v[4], v[5] } },
                    geode::Point3D{ { v[6], v[7], v[8] } } )
                    == test_case.expected,
                "[Test] Wrong dot_3d on case decided by ", test_case.stage );
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
    test_integer_configurations();
    test_regression_cases();

    geode::Logger::info( "TEST SUCCESS" );
}

OPENGEODE_TEST( "predicates" )
