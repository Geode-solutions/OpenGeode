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

#include <geode/geometry/intersection.hpp>

#include <geode/geometry/barycentric_coordinates.hpp>
#include <geode/geometry/basic_objects/circle.hpp>
#include <geode/geometry/basic_objects/cylinder.hpp>
#include <geode/geometry/basic_objects/ellipse.hpp>
#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/sphere.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/distance.hpp>
#include <geode/geometry/perpendicular.hpp>
#include <geode/geometry/projection.hpp>
#include <geode/geometry/square_matrix.hpp>
#include <geode/geometry/vector.hpp>

namespace
{
    using Basis3D = std::array< geode::Vector3D, 3 >;

    // Gram-Schmidt orthonormalization to generate orthonormal vectors from
    // the linearly independent inputs.  The function returns the smallest
    // length of the unnormalized vectors computed during the process.  If
    // this value is nearly zero, it is possible that the inputs are linearly
    // dependent (within numerical round-off errors).
    void orthonormalize( Basis3D& basis )
    {
        for( const auto i : geode::LRange{ 3 } )
        {
            for( const auto j : geode::LRange{ i } )
            {
                const auto dot_ij = basis[i].dot( basis[j] );
                basis[i] -= basis[j] * dot_ij;
            }
            basis[i] = basis[i].normalize();
        }
    }

    // Compute a right-handed orthonormal basis for the orthogonal complement
    // of the input vectors.  The function returns the smallest length of the
    // unnormalized vectors computed during the process.  If this value is
    // nearly zero, it is possible that the inputs are linearly dependent
    // (within numerical round-off errors).
    Basis3D compute_orthogonal_basis( const geode::Vector3D& axis )
    {
        Basis3D basis;
        basis[0] = axis;
        if( std::fabs( axis.value( 0 ) ) > std::fabs( axis.value( 1 ) ) )
        {
            basis[1] =
                geode::Vector3D{ { -axis.value( 2 ), 0, axis.value( 0 ) } };
        }
        else
        {
            basis[1] =
                geode::Vector3D{ { 0, axis.value( 2 ), -axis.value( 1 ) } };
        }
        basis[2] = basis[0].cross( basis[1] );
        orthonormalize( basis );
        return basis;
    }

    struct CylinderLineResult
    {
        CylinderLineResult()
            : intersect( false ), numIntersections( 0 ), parameter{ 0, 0 }
        {
        }

        bool intersect;
        size_t numIntersections;
        std::array< double, 2 > parameter;
    };
} // namespace

namespace geode
{
    IntersectionResult< Point3D > line_plane_intersection(
        const InfiniteLine3D& line, const Plane& plane )
    {
        const auto dot_directions = line.direction().dot( plane.normal() );
        if( std::fabs( dot_directions ) <= GLOBAL_EPSILON )
        {
            // line is parallel to the plane
            return { INTERSECTION_TYPE::parallel };
        }
        const auto signed_distance =
            plane.normal().dot( Vector3D{ line.origin() } )
            + plane.plane_constant();
        auto result =
            line.origin() - line.direction() * signed_distance / dot_directions;
        CorrectnessInfo< Point3D >::Correctness first_correctness{
            point_line_distance( result, line ) <= GLOBAL_EPSILON,
            point_line_projection( result, line )
        };
        const auto plane_distance = point_plane_distance( result, plane );
        CorrectnessInfo< Point3D >::Correctness second_correctness{
            std::get< 0 >( plane_distance ) <= GLOBAL_EPSILON,
            std::get< 1 >( plane_distance )
        };
        return { std::move( result ),
            { first_correctness, second_correctness } };
    }

    template < index_t dimension >
    IntersectionResult< absl::InlinedVector< Point< dimension >, 2 > >
        line_sphere_intersection( const InfiniteLine< dimension >& line,
            const Sphere< dimension >& sphere )
    {
        // The sphere is (X-C)^T*(X-C)-1 = 0 and the line is X = P+t*D.
        // Substitute the line equation into the sphere equation to obtain a
        // quadratic equation Q(t) = t^2 + 2*a1*t + a0 = 0, where a1 =
        // D^T*(P-C),
        // and a0 = (P-C)^T*(P-C)-1.
        const Vector< dimension > diff{ sphere.origin(), line.origin() };
        const auto a0 = diff.dot( diff ) - sphere.radius() * sphere.radius();
        const auto a1 = line.direction().dot( diff );

        // Intersection occurs when Q(t) has real roots.
        const auto discr = a1 * a1 - a0;
        if( discr > GLOBAL_EPSILON ) // positive
        {
            absl::InlinedVector< Point< dimension >, 2 > results;
            const auto root = std::sqrt( discr );
            results.reserve( 2 );
            results.emplace_back(
                line.origin() + line.direction() * ( -a1 - root ) );
            results.emplace_back(
                line.origin() + line.direction() * ( -a1 + root ) );
            typename CorrectnessInfo< absl::InlinedVector< Point< dimension >,
                2 > >::Correctness first_correctness;
            first_correctness.first =
                point_line_distance( results.front(), line ) <= GLOBAL_EPSILON;
            first_correctness.second.push_back(
                point_line_projection( results.front(), line ) );
            first_correctness.first =
                first_correctness.first
                && point_line_distance( results.back(), line )
                       <= GLOBAL_EPSILON;
            first_correctness.second.push_back(
                point_line_projection( results.back(), line ) );
            typename CorrectnessInfo< absl::InlinedVector< Point< dimension >,
                2 > >::Correctness second_correctness;
            const auto front_output =
                point_sphere_distance( results.front(), sphere );
            second_correctness.first =
                std::get< 0 >( front_output ) <= GLOBAL_EPSILON;
            second_correctness.second.push_back(
                std::get< 1 >( front_output ) );
            const auto back_output =
                point_sphere_distance( results.back(), sphere );
            second_correctness.first =
                second_correctness.first
                && std::get< 0 >( back_output ) <= GLOBAL_EPSILON;
            second_correctness.second.push_back( std::get< 1 >( back_output ) );
            return { std::move( results ),
                { first_correctness, second_correctness } };
        }
        else if( discr > -GLOBAL_EPSILON ) // zero
        {
            absl::InlinedVector< Point< dimension >, 2 > results;
            results.reserve( 1 );
            results.emplace_back( line.origin() - line.direction() * a1 );
            typename CorrectnessInfo< absl::InlinedVector< Point< dimension >,
                2 > >::Correctness first_correctness;
            first_correctness.first =
                point_line_distance( results.front(), line ) <= GLOBAL_EPSILON;
            first_correctness.second.push_back(
                point_line_projection( results.front(), line ) );
            const auto output =
                point_sphere_distance( results.front(), sphere );
            typename CorrectnessInfo< absl::InlinedVector< Point< dimension >,
                2 > >::Correctness second_correctness;
            second_correctness.first =
                std::get< 0 >( output ) <= GLOBAL_EPSILON;
            second_correctness.second.push_back( std::get< 1 >( output ) );
            return { std::move( results ),
                { first_correctness, second_correctness } };
        }
        return { INTERSECTION_TYPE::none }; // negative
    }

    template < index_t dimension >
    IntersectionResult< absl::InlinedVector< Point< dimension >, 2 > >
        segment_sphere_intersection( const Segment< dimension >& segment,
            const Sphere< dimension >& sphere )
    {
        auto line_intersections = line_sphere_intersection(
            InfiniteLine< dimension >{ segment }, sphere );
        if( line_intersections )
        {
            absl::InlinedVector< Point< dimension >, 2 > segment_intersections;
            segment_intersections.reserve(
                line_intersections.result.value().size() );
            for( auto&& point : line_intersections.result.value() )
            {
                if( point_segment_distance( point, segment ) <= GLOBAL_EPSILON )
                {
                    segment_intersections.emplace_back( point );
                }
            }
            if( segment_intersections.empty() )
            {
                return { INTERSECTION_TYPE::none };
            }
            return { std::move( segment_intersections ),
                std::move( line_intersections.correctness.value() ) };
        }
        return line_intersections.type;
    }

    IntersectionResult< Point3D > segment_plane_intersection(
        const Segment3D& segment, const Plane& plane )
    {
        auto line_plane_result =
            line_plane_intersection( InfiniteLine3D{ segment }, plane );
        if( line_plane_result )
        {
            if( point_segment_distance(
                    line_plane_result.result.value(), segment )
                > GLOBAL_EPSILON )
            {
                return { INTERSECTION_TYPE::none };
            }
            const auto lambdas = safe_segment_barycentric_coordinates(
                line_plane_result.result.value(), segment );
            line_plane_result.correctness->first.second =
                segment.vertices()[0].get() * lambdas[0]
                + segment.vertices()[1].get() * lambdas[1];
            return line_plane_result;
        }
        return line_plane_result.type;
    }

    IntersectionResult< Point3D > segment_triangle_intersection(
        const Segment3D& segment, const Triangle3D& triangle )
    {
        // http://www.geometrictools.com/LibMathematics/Intersection/Intersection.html
        // Compute the offset origin, edges, and normal.
        const auto seg_center = segment.barycenter();
        const auto& vertices = triangle.vertices();
        const Vector3D edge1{ vertices[0], vertices[1] };
        const Vector3D edge2{ vertices[0], vertices[2] };
        const auto normal = edge1.cross( edge2 );

        // Solve Q + t*D = b1*E1 + b2*E2 (Q = diff, D = segment direction,
        // E1 = edge1, E2 = edge2, N = Cross(E1,E2)) by
        //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
        //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
        //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
        const auto segment_normalized_direction =
            segment.normalized_direction();
        auto d_dot_n = segment_normalized_direction.dot( normal );
        signed_index_t sign;
        if( d_dot_n > 0. )
        {
            sign = 1;
        }
        else if( d_dot_n < -0. )
        {
            sign = -1;
            d_dot_n = -d_dot_n;
        }
        else
        {
            // Segment and triangle are parallel
            return { INTERSECTION_TYPE::parallel };
        }

        const Vector3D diff{ vertices[0], seg_center };
        const auto d_dot_q_cross_e2 =
            sign * segment_normalized_direction.dot( diff.cross( edge2 ) );
        if( d_dot_q_cross_e2 < -GLOBAL_EPSILON )
        {
            // b1 < 0, no intersection
            return { INTERSECTION_TYPE::none };
        }
        const auto d_dot_e1_cross_q =
            sign * segment_normalized_direction.dot( edge1.cross( diff ) );
        if( d_dot_e1_cross_q < -GLOBAL_EPSILON )
        {
            // b2 < 0, no intersection
            return { INTERSECTION_TYPE::none };
        }

        if( d_dot_q_cross_e2 + d_dot_e1_cross_q - d_dot_n > GLOBAL_EPSILON )
        {
            // b1+b2 > 1, no intersection
            return { INTERSECTION_TYPE::none };
        }

        // InfiniteLine intersects triangle, check if segment does.
        const auto q_dot_n = -sign * diff.dot( normal );
        const auto extd_dot_n = segment.length() * d_dot_n / 2.;

        if( -extd_dot_n > q_dot_n || q_dot_n > extd_dot_n )
        {
            // else: |t| > extent, no intersection
            return { INTERSECTION_TYPE::none };
        }
        // Segment intersects triangle.
        const auto inv = 1. / d_dot_n;
        const auto seg_parameter = q_dot_n * inv;

        auto result = seg_center + segment_normalized_direction * seg_parameter;
        CorrectnessInfo< Point3D >::Correctness first_correctness{
            point_segment_distance( result, segment ) <= GLOBAL_EPSILON,
            point_segment_projection( result, segment )
        };
        const auto point_to_triangle_distance =
            point_triangle_distance( result, triangle );
        CorrectnessInfo< Point3D >::Correctness second_correctness{
            std::get< 0 >( point_to_triangle_distance ) <= GLOBAL_EPSILON,
            std::get< 1 >( point_to_triangle_distance )
        };
        return { std::move( result ),
            { first_correctness, second_correctness } };
    }

    IntersectionResult< Point3D > line_triangle_intersection(
        const InfiniteLine3D& line, const Triangle3D& triangle )
    {
        // http://www.geometrictools.com/LibMathematics/Intersection/Intersection.html
        // Compute the offset origin, edges, and normal.
        const auto& vertices = triangle.vertices();
        const Vector3D edge1{ vertices[0], vertices[1] };
        const Vector3D edge2{ vertices[0], vertices[2] };
        const auto normal = edge1.cross( edge2 );

        // Solve Q + t*D = b1*E1 + b2*E2 (Q = diff, D = segment direction,
        // E1 = edge1, E2 = edge2, N = Cross(E1,E2)) by
        //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
        //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
        //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
        auto d_dot_n = line.direction().dot( normal );
        signed_index_t sign;
        if( d_dot_n > 0. )
        {
            sign = 1;
        }
        else if( d_dot_n < -0. )
        {
            sign = -1;
            d_dot_n = -d_dot_n;
        }
        else
        {
            // Segment and triangle are parallel
            return { INTERSECTION_TYPE::parallel };
        }

        const Vector3D diff{ vertices[0], line.origin() };
        const auto d_dot_q_cross_e2 =
            sign * line.direction().dot( diff.cross( edge2 ) );
        if( d_dot_q_cross_e2 >= -GLOBAL_EPSILON )
        {
            const auto d_dot_e1_cross_q =
                sign * line.direction().dot( edge1.cross( diff ) );
            if( d_dot_e1_cross_q >= -GLOBAL_EPSILON
                && d_dot_q_cross_e2 + d_dot_e1_cross_q <= d_dot_n )
            {
                // InfiniteLine intersects triangle.
                const auto q_dot_n = -sign * diff.dot( normal );
                const auto inv = 1. / d_dot_n;
                const auto seg_parameter = q_dot_n * inv;

                auto result = line.origin() + line.direction() * seg_parameter;
                CorrectnessInfo< Point3D >::Correctness first_correctness{
                    point_line_distance( result, line ) <= GLOBAL_EPSILON,
                    point_line_projection( result, line )
                };
                const auto tri_lambdas =
                    safe_triangle_barycentric_coordinates( result, triangle );
                const auto correctness_point =
                    vertices[0].get() * tri_lambdas[0]
                    + vertices[1].get() * tri_lambdas[1]
                    + vertices[2].get() * tri_lambdas[2];
                CorrectnessInfo< Point3D >::Correctness second_correctness{
                    result.inexact_equal( correctness_point ), correctness_point
                };
                return { std::move( result ),
                    { first_correctness, second_correctness } };
            }
            // else: b1+b2 > 1, no intersection
            // else: b2 < 0, no intersection
        }
        // else: b1 < 0, no intersection
        return { INTERSECTION_TYPE::none };
    }

    IntersectionResult< Point2D > line_line_intersection(
        const InfiniteLine2D& line0, const InfiniteLine2D& line1 )
    {
        // The intersection of two lines is a solution to P0 + s0*D0 = P1 +
        // s1*D1.
        // Rewrite this as s0*D0 - s1*D1 = P1 - P0 = Q.  If DotPerp(D0, D1))
        // = 0,
        // the lines are parallel.  Additionally, if DotPerp(Q, D1)) = 0,
        // the
        // lines are the same.  If Dotperp(D0, D1)) is not zero, then
        //   s0 = DotPerp(Q, D1))/DotPerp(D0, D1))
        // produces the point of intersection.  Also,
        //   s1 = DotPerp(Q, D0))/DotPerp(D0, D1))

        const Vector2D diff{ line0.origin(), line1.origin() };
        const auto D0DotPerpD1 =
            dot_perpendicular( line0.direction(), line1.direction() );
        if( std::fabs( D0DotPerpD1 ) < 0. )
        {
            // The lines are parallel.
            return { INTERSECTION_TYPE::parallel };
        }

        const auto invD0DotPerpD1 = 1.0 / D0DotPerpD1;
        const auto diffDotPerpD1 = dot_perpendicular( diff, line1.direction() );
        const auto s0 = diffDotPerpD1 * invD0DotPerpD1;
        auto result = line0.origin() + line0.direction() * s0;
        CorrectnessInfo< Point2D >::Correctness first_correctness{
            point_line_distance( result, line0 ) <= GLOBAL_EPSILON,
            point_line_projection( result, line0 )
        };
        CorrectnessInfo< Point2D >::Correctness second_correctness{
            point_line_distance( result, line1 ) <= GLOBAL_EPSILON,
            point_line_projection( result, line1 )
        };
        return { std::move( result ),
            { first_correctness, second_correctness } };
    }

    IntersectionResult< Point2D > segment_segment_intersection(
        const Segment2D& segment0, const Segment2D& segment1 )
    {
        auto line_intersection_result = line_line_intersection(
            InfiniteLine2D{ segment0 }, InfiniteLine2D{ segment1 } );
        if( line_intersection_result )
        {
            // Test whether the line-line intersection is on the segments.
            if( point_segment_distance(
                    line_intersection_result.result.value(), segment0 )
                > GLOBAL_EPSILON )
            {
                return { INTERSECTION_TYPE::none };
            }
            if( point_segment_distance(
                    line_intersection_result.result.value(), segment1 )
                > GLOBAL_EPSILON )
            {
                return { INTERSECTION_TYPE::none };
            }
            std::array< double, 2 > lambdas0;
            try
            {
                lambdas0 = segment_barycentric_coordinates(
                    line_intersection_result.result.value(), segment0 );
            }
            catch( const OpenGeodeException& )
            {
                lambdas0.fill( 0.5 );
            }
            line_intersection_result.correctness->first.second =
                segment0.vertices()[0].get() * lambdas0[0]
                + segment0.vertices()[1].get() * lambdas0[1];
            std::array< double, 2 > lambdas1;
            try
            {
                lambdas1 = segment_barycentric_coordinates(
                    line_intersection_result.result.value(), segment1 );
            }
            catch( const OpenGeodeException& )
            {
                lambdas1.fill( 0.5 );
            }
            line_intersection_result.correctness->second.second =
                segment1.vertices()[0].get() * lambdas1[0]
                + segment1.vertices()[1].get() * lambdas1[1];
            return line_intersection_result;
        }
        return line_intersection_result.type;
    }

    IntersectionResult< Point2D > segment_line_intersection(
        const Segment2D& segment, const InfiniteLine2D& line )
    {
        auto line_intersection_result =
            line_line_intersection( InfiniteLine2D{ segment }, line );
        if( line_intersection_result )
        {
            // Test whether the line-line intersection is on the segment.
            if( point_segment_distance(
                    line_intersection_result.result.value(), segment )
                > GLOBAL_EPSILON )
            {
                return { INTERSECTION_TYPE::none };
            }
            const auto lambdas = safe_segment_barycentric_coordinates(
                line_intersection_result.result.value(), segment );
            line_intersection_result.correctness->first.second =
                segment.vertices()[0].get() * lambdas[0]
                + segment.vertices()[1].get() * lambdas[1];
            return line_intersection_result;
        }
        return line_intersection_result.type;
    }

    IntersectionResult< absl::InlinedVector< Point3D, 2 > >
        opengeode_geometry_api line_cylinder_intersection(
            const InfiniteLine3D& line, const Cylinder& cylinder )
    {
        CylinderLineResult result;
        // Create a coordinate system for the cylinder. In this system,
        // the cylinder segment center C is the origin and the cylinder
        // axis direction W is the z-axis. U and V are the other
        // coordinate axis directions. If P = x*U+y*V+z*W, the cylinder
        // is x^2 + y^2 = r^2, where r is the cylinder radius. The end
        // caps are |z| = h/2, where h is the cylinder height.
        const auto basis =
            compute_orthogonal_basis( cylinder.axis().normalized_direction() );
        const auto& W = basis[0];
        const auto& U = basis[1];
        const auto& V = basis[2];
        const auto halfHeight = 0.5 * cylinder.axis().length();
        const auto rSqr = cylinder.radius() * cylinder.radius();

        const auto cylinder_barycenter = cylinder.axis().barycenter();

        // Convert incoming line origin to capsule coordinates.
        const Vector3D diff{ cylinder_barycenter, line.origin() };
        Vector3D P{ { U.dot( diff ), V.dot( diff ), W.dot( diff ) } };

        // Get the z-value, in cylinder coordinates, of the incoming
        // line's unit-length direction.
        const auto dz = W.dot( line.direction() );
        if( std::fabs( dz ) == 1 )
        {
            // The line is parallel to the cylinder axis. Determine
            // whether the line intersects the cylinder end disks.
            const auto radialSqrDist = rSqr - P.value( 0 ) * P.value( 0 )
                                       - P.value( 1 ) * P.value( 1 );
            if( radialSqrDist >= 0 )
            {
                // The line intersects the cylinder end disks.
                result.intersect = true;
                result.numIntersections = 2;
                if( dz > 0 )
                {
                    result.parameter[0] = -P.value( 2 ) - halfHeight;
                    result.parameter[1] = -P.value( 2 ) + halfHeight;
                }
                else
                {
                    result.parameter[0] = P.value( 2 ) - halfHeight;
                    result.parameter[1] = P.value( 2 ) + halfHeight;
                }
            }
            else
            {
                // else:  The line is outside the cylinder, no intersection.
                return { INTERSECTION_TYPE::none };
            }
        }
        else
        {
            // Convert the incoming line unit-length direction to cylinder
            // coordinates.
            Vector3D D{ { U.dot( line.direction() ), V.dot( line.direction() ),
                dz } };
            if( D.value( 2 ) == 0 )
            {
                // The line is perpendicular to the cylinder axis.
                if( std::fabs( P.value( 2 ) ) <= halfHeight )
                {
                    // Test intersection of line P+t*D with infinite cylinder
                    // x^2+y^2 = r^2. This reduces to computing the roots of a
                    // quadratic equation. If P = (px,py,pz) and
                    // D = (dx,dy,dz), then the quadratic equation is
                    // (dx^2+dy^2)*t^2+2*(px*dx+py*dy)*t+(px^2+py^2-r^2) = 0.
                    const auto a0 = P.value( 0 ) * P.value( 0 )
                                    + P.value( 1 ) * P.value( 1 ) - rSqr;
                    const auto a1 = P.value( 0 ) * D.value( 0 )
                                    + P.value( 1 ) * D.value( 1 );
                    const auto a2 = D.value( 0 ) * D.value( 0 )
                                    + D.value( 1 ) * D.value( 1 );
                    const auto discr = a1 * a1 - a0 * a2;
                    if( discr > 0 )
                    {
                        // The line intersects the cylinder in two places.
                        result.intersect = true;
                        result.numIntersections = 2;
                        const auto root = std::sqrt( discr );
                        result.parameter[0] = ( -a1 - root ) / a2;
                        result.parameter[1] = ( -a1 + root ) / a2;
                    }
                    else if( discr == 0 )
                    {
                        // The line is tangent to the cylinder.
                        result.intersect = true;
                        result.numIntersections = 1;
                        result.parameter[0] = -a1 / a2;
                        result.parameter[1] = result.parameter[0];
                    }
                    // else: The line does not intersect the cylinder.
                }
                else
                { // else: The line is outside the planes of the cylinder end
                    // disks.
                    return { INTERSECTION_TYPE::none };
                }
            }

            // At this time, the line direction is neither parallel nor
            // perpendicular to the cylinder axis. The line must
            // intersect both planes of the end disk, the intersection with
            // the cylinder being a segment. The t-interval of the segment
            // is [t0,t1].

            // Test for intersections with the planes of the end disks.
            const auto t0 = ( -halfHeight - P.value( 2 ) ) / D.value( 2 );
            auto xTmp = P.value( 0 ) + t0 * D.value( 0 );
            auto yTmp = P.value( 1 ) + t0 * D.value( 1 );
            if( xTmp * xTmp + yTmp * yTmp <= rSqr )
            {
                // Plane intersection inside the bottom cylinder end disk.
                result.parameter[result.numIntersections++] = t0;
            }

            const auto t1 = ( halfHeight - P.value( 2 ) ) / D.value( 2 );
            xTmp = P.value( 0 ) + t1 * D.value( 0 );
            yTmp = P.value( 1 ) + t1 * D.value( 1 );
            if( xTmp * xTmp + yTmp * yTmp <= rSqr )
            {
                // Plane intersection inside the top cylinder end disk.
                result.parameter[result.numIntersections++] = t1;
            }

            if( result.numIntersections < 2 )
            {
                // Test for intersection with the cylinder wall.
                const auto a0 = P.value( 0 ) * P.value( 0 )
                                + P.value( 1 ) * P.value( 1 ) - rSqr;
                const auto a1 =
                    P.value( 0 ) * D.value( 0 ) + P.value( 1 ) * D.value( 1 );
                const auto a2 =
                    D.value( 0 ) * D.value( 0 ) + D.value( 1 ) * D.value( 1 );
                const auto discr = a1 * a1 - a0 * a2;
                if( discr > 0 )
                {
                    const auto root = std::sqrt( discr );
                    auto tValue = ( -a1 - root ) / a2;
                    if( t0 <= t1 )
                    {
                        if( t0 <= tValue && tValue <= t1 )
                        {
                            result.parameter[result.numIntersections++] =
                                tValue;
                        }
                    }
                    else
                    {
                        if( t1 <= tValue && tValue <= t0 )
                        {
                            result.parameter[result.numIntersections++] =
                                tValue;
                        }
                    }

                    if( result.numIntersections < 2 )
                    {
                        tValue = ( -a1 + root ) / a2;
                        if( t0 <= t1 )
                        {
                            if( t0 <= tValue && tValue <= t1 )
                            {
                                result.parameter[result.numIntersections++] =
                                    tValue;
                            }
                        }
                        else
                        {
                            if( t1 <= tValue && tValue <= t0 )
                            {
                                result.parameter[result.numIntersections++] =
                                    tValue;
                            }
                        }
                    }
                    // else: Line intersects end disk and cylinder wall.
                }
                else if( discr == 0 )
                {
                    auto tValue = -a1 / a2;
                    if( t0 <= t1 )
                    {
                        if( t0 <= tValue && tValue <= t1 )
                        {
                            result.parameter[result.numIntersections++] =
                                tValue;
                        }
                    }
                    else
                    {
                        if( t1 <= tValue && tValue <= t0 )
                        {
                            result.parameter[result.numIntersections++] =
                                tValue;
                        }
                    }
                }
                // else: Line does not intersect cylinder wall.
            }
            // else: Line intersects both top and bottom cylinder end disks.
        }
        if( result.numIntersections == 2 )
        {
            result.intersect = true;
            if( result.parameter[0] > result.parameter[1] )
            {
                std::swap( result.parameter[0], result.parameter[1] );
            }
        }
        else if( result.numIntersections == 1 )
        {
            result.intersect = true;
            result.parameter[1] = result.parameter[0];
        }

        if( !result.intersect )
        {
            return { INTERSECTION_TYPE::none };
        }
        absl::InlinedVector< Point3D, 2 > results;
        results.reserve( result.numIntersections );
        for( const auto r : LRange{ result.numIntersections } )
        {
            results.emplace_back(
                line.origin() + line.direction() * result.parameter[r] );
        }
        CorrectnessInfo< absl::InlinedVector< Point3D, 2 > >::Correctness
            first_correctness{ true, {} };
        CorrectnessInfo< absl::InlinedVector< Point3D, 2 > >::Correctness
            second_correctness{ true, {} };
        for( const auto r : LRange{ result.numIntersections } )
        {
            // distance to line
            first_correctness.first =
                point_line_distance( results[r], line ) <= GLOBAL_EPSILON;
            first_correctness.second.push_back(
                point_line_projection( results[r], line ) );
            first_correctness.first =
                first_correctness.first
                && point_line_distance( results.back(), line )
                       <= GLOBAL_EPSILON;
            first_correctness.second.push_back(
                point_line_projection( results.back(), line ) );

            // distance to cylinder
            // first check distance and projection orientation to the cylinder
            // limits
            bool cur_correctness{ false };
            for( const auto v : LRange{ 2 } )
            {
                if( !cur_correctness )
                {
                    const auto distance = point_point_distance(
                        results[r], cylinder.axis().vertices()[v].get() );
                    if( distance <= GLOBAL_EPSILON )
                    {
                        cur_correctness = true;
                        second_correctness.second.push_back( results[r] );
                        break;
                    }
                    if( distance < cylinder.radius() )
                    {
                        const geode::Vector3D point_to_vertex{ results[r],
                            cylinder.axis().vertices()[v] };
                        if( cylinder.axis().normalized_direction().dot(
                                point_to_vertex.normalize() )
                            <= GLOBAL_EPSILON )
                        {
                            cur_correctness = true;
                        }
                    }
                }
            }
            // then check distance to the cylinder axis
            if( !cur_correctness )
            {
                const auto distance =
                    point_segment_distance( results[r], cylinder.axis() );
                cur_correctness =
                    std::fabs( distance - cylinder.radius() ) <= GLOBAL_EPSILON;
            }
            second_correctness.first =
                second_correctness.first && cur_correctness;
            second_correctness.second.push_back( results[r] );
        }
        return { std::move( results ),
            { first_correctness, second_correctness } };
    }

    IntersectionResult< absl::InlinedVector< Point3D, 2 > >
        opengeode_geometry_api segment_cylinder_intersection(
            const Segment3D& segment, const Cylinder& cylinder )
    {
        auto line_intersections =
            line_cylinder_intersection( InfiniteLine3D{ segment }, cylinder );
        if( line_intersections )
        {
            absl::InlinedVector< Point3D, 2 > segment_intersections;
            segment_intersections.reserve(
                line_intersections.result.value().size() );
            for( auto&& point : line_intersections.result.value() )
            {
                if( point_segment_distance( point, segment ) <= GLOBAL_EPSILON )
                {
                    segment_intersections.emplace_back( point );
                }
            }
            if( segment_intersections.empty() )
            {
                return { INTERSECTION_TYPE::none };
            }
            return { std::move( segment_intersections ),
                std::move( line_intersections.correctness.value() ) };
        }
        return line_intersections.type;
    }

    IntersectionResult< absl::InlinedVector< Point3D, 2 > >
        triangle_circle_intersection(
            const Triangle3D& triangle, const Circle& circle )
    {
        const auto plane = triangle.plane();
        if( !plane )
        {
            return { INTERSECTION_TYPE::none };
        }
        const auto plane_intersection =
            plane_circle_intersection( plane.value(), circle );
        if( !plane_intersection.has_intersection() )
        {
            // The planes are parallel or nonintersecting.
            return { plane_intersection.type };
        }
        absl::InlinedVector< Point3D, 2 > result;
        CorrectnessInfo< absl::InlinedVector< Point3D, 2 > >::Correctness
            first_correctness{ true, {} };
        CorrectnessInfo< absl::InlinedVector< Point3D, 2 > >::Correctness
            second_correctness{ true, {} };
        auto& intersections = plane_intersection.result.value();
        for( const auto i : Indices{ intersections } )
        {
            auto& intersection = intersections[i];
            auto triangle_output =
                point_triangle_distance( intersection, triangle );
            if( std::get< 0 >( triangle_output ) <= GLOBAL_EPSILON )
            {
                result.emplace_back( std::move( intersection ) );
                first_correctness.second.emplace_back(
                    std::move( std::get< 1 >( triangle_output ) ) );
                second_correctness.second.emplace_back(
                    plane_intersection.correctness->second.second[i] );
            }
        }
        if( result.empty() )
        {
            return { INTERSECTION_TYPE::none };
        }
        return { std::move( result ),
            { first_correctness, second_correctness } };
    }

    IntersectionResult< absl::InlinedVector< Point3D, 2 > >
        plane_circle_intersection( const Plane& plane, const Circle& circle )
    {
        const auto& circle_plane = circle.plane();
        const auto planes_intersection =
            plane_plane_intersection( plane, circle_plane );
        if( !planes_intersection.has_intersection() )
        {
            // The planes are parallel or nonintersecting.
            return { planes_intersection.type };
        }
        // The planes intersect in a line. Locate one or two points that
        // are on the circle and line. If the line is t*D+P, the circle
        // center is C, and the circle radius is r, then
        //   r^2 = |t*D+P-C|^2 = |D|^2*t^2 + 2*Dot(D,P-C)*t + |P-C|^2
        // This is a quadratic equation of the form
        // a2*t^2 + 2*a1*t + a0 = 0.
        const auto& line = planes_intersection.result.value();
        const Vector3D diff{ circle_plane.origin(), line.origin() };
        const auto a2 = line.direction().dot( line.direction() );
        const auto a1 = diff.dot( line.direction() );
        const auto a0 = diff.dot( diff ) - circle.radius() * circle.radius();

        const auto discr = a1 * a1 - a0 * a2;
        if( discr < 0. )
        {
            // No real roots, the circle does not intersect the plane.
            return { INTERSECTION_TYPE::none };
        }
        absl::InlinedVector< Point3D, 2 > result;
        CorrectnessInfo< absl::InlinedVector< Point3D, 2 > >::Correctness
            first_correctness;
        CorrectnessInfo< absl::InlinedVector< Point3D, 2 > >::Correctness
            second_correctness;
        const auto compute_correctness = [&first_correctness,
                                             &second_correctness, &plane,
                                             &circle](
                                             const Point3D& intersection ) {
            auto plane_output = point_plane_distance( intersection, plane );
            first_correctness.first =
                std::get< 0 >( plane_output ) <= GLOBAL_EPSILON;
            first_correctness.second.emplace_back(
                std::move( std::get< 1 >( plane_output ) ) );
            auto circle_output = point_circle_distance( intersection, circle );
            second_correctness.first =
                std::get< 0 >( circle_output ) <= GLOBAL_EPSILON;
            second_correctness.second.emplace_back(
                std::move( std::get< 1 >( circle_output ) ) );
        };
        if( discr == 0. )
        {
            // The quadratic polynomial has 1 real-valued repeated root.
            // The circle just touches the plane.
            compute_correctness( result.emplace_back(
                line.origin() - line.direction() * ( a1 / a2 ) ) );
        }
        else
        {
            // The quadratic polynomial has 2 distinct, real-valued roots.
            // The circle intersects the plane in two points.
            const auto root = std::sqrt( discr );
            compute_correctness( result.emplace_back(
                line.origin() - line.direction() * ( ( a1 + root ) / a2 ) ) );
            compute_correctness( result.emplace_back(
                line.origin() - line.direction() * ( ( a1 - root ) / a2 ) ) );
        }
        return { std::move( result ),
            { first_correctness, second_correctness } };
    }

    IntersectionResult< OwnerInfiniteLine3D > plane_plane_intersection(
        const Plane& plane0, const Plane& plane1 )
    {
        // If N0 and N1 are parallel, either the planes are parallel and
        // separated or the same plane.
        //  Otherwise, the intersection line is
        //   L(t) = t*Cross(N0,N1)/|Cross(N0,N1)| + c0*N0 + c1*N1
        // for some coefficients c0 and c1 and for t any real number (the
        // line parameter).  Taking dot products with the normals,
        //   d0 = Dot(N0,L) = c0*Dot(N0,N0) + c1*Dot(N0,N1) = c0 + c1*d
        //   d1 = Dot(N1,L) = c0*Dot(N0,N1) + c1*Dot(N1,N1) = c0*d + c1
        // where d = Dot(N0,N1).  These are two equations in two unknowns.
        // The solution is
        //   c0 = (d0 - d*d1)/det
        //   c1 = (d1 - d*d0)/det
        // where det = 1 - d^2.
        const auto dot = plane0.normal().dot( plane1.normal() );
        const auto constant0 = -plane0.plane_constant();
        const auto constant1 = -plane1.plane_constant();
        if( std::fabs( dot ) >= 1. )
        {
            // The planes are parallel.  Check if they are coplanar.
            const auto constant_diff =
                dot >= 0. ? constant0 - constant1 : constant0 + constant1;
            if( std::fabs( constant_diff ) == 0. )
            {
                // The planes are coplanar.
                return { INTERSECTION_TYPE::parallel };
            }
            // The planes are parallel but distinct.
            return { INTERSECTION_TYPE::none };
        }
        const auto invDet = 1. / ( 1. - dot * dot );
        const auto c0 = ( constant0 - dot * constant1 ) * invDet;
        const auto c1 = ( constant1 - dot * constant0 ) * invDet;
        OwnerInfiniteLine3D line{ plane0.normal().cross( plane1.normal() ),
            plane0.normal() * c0 + plane1.normal() * c1 };
        const auto compute_corectness = [&line]( const Plane& plane )
            -> CorrectnessInfo< OwnerInfiniteLine3D >::Correctness {
            auto output = point_plane_distance( line.origin(), plane );
            return std::make_pair( std::get< 0 >( output ) <= GLOBAL_EPSILON,
                OwnerInfiniteLine3D{
                    line.direction(), std::move( std::get< 1 >( output ) ) } );
        };
        auto first_correctness = compute_corectness( plane0 );
        auto second_correctness = compute_corectness( plane1 );
        return { std::move( line ), { first_correctness, second_correctness } };
    }

    template < index_t dimension >
    IntersectionResult< absl::InlinedVector< Point< dimension >, 2 > >
        line_ellipse_intersection( const InfiniteLine< dimension >& line,
            const Ellipse< dimension >& ellipse )
    {
        geode::SquareMatrix< dimension > M( 0 );
        for( const auto i : geode::LRange{ dimension } )
        {
            const auto ratio = ellipse.axes().direction( i ).normalize()
                               * ( 1 / ellipse.axes().direction( i ).length() );
            geode::SquareMatrix< dimension > M_i;
            for( const auto j : geode::LRange{ dimension } )
            {
                for( const auto k : geode::LRange{ dimension } )
                {
                    M_i.set_value( j, k, ratio.value( j ) * ratio.value( k ) );
                }
            }
            M += M_i;
        }
        const Vector< dimension > diff{ ellipse.center(), line.origin() };
        const auto matDir = M * line.direction();
        const auto matDiff = M * diff;
        const auto a0 = diff.dot( matDiff ) - 1;
        const auto a1 = line.direction().dot( matDiff );
        const auto a2 = line.direction().dot( matDir );
        const auto discr = a1 * a1 - a0 * a2;
        if( discr > GLOBAL_EPSILON )
        {
            absl::InlinedVector< Point< dimension >, 2 > results;
            const auto root = std::sqrt( discr );
            results.reserve( 2 );
            results.emplace_back(
                line.origin() + line.direction() * ( -a1 - root ) / a2 );
            results.emplace_back(
                line.origin() + line.direction() * ( -a1 + root ) / a2 );
            typename CorrectnessInfo< absl::InlinedVector< Point< dimension >,
                2 > >::Correctness first_correctness;
            first_correctness.first =
                point_line_distance( results.front(), line ) <= GLOBAL_EPSILON;
            first_correctness.second.push_back(
                point_line_projection( results.front(), line ) );
            first_correctness.first =
                first_correctness.first
                && point_line_distance( results.back(), line )
                       <= GLOBAL_EPSILON;
            first_correctness.second.push_back(
                point_line_projection( results.back(), line ) );
            typename CorrectnessInfo< absl::InlinedVector< Point< dimension >,
                2 > >::Correctness second_correctness;
            const auto front_output =
                point_ellipse_distance( results.front(), ellipse );
            second_correctness.first =
                std::get< 0 >( front_output ) <= GLOBAL_EPSILON;
            second_correctness.second.push_back(
                std::get< 1 >( front_output ) );
            const auto back_output =
                point_ellipse_distance( results.back(), ellipse );
            second_correctness.first =
                second_correctness.first
                && std::get< 0 >( back_output ) <= GLOBAL_EPSILON;
            second_correctness.second.push_back( std::get< 1 >( back_output ) );
            return { std::move( results ),
                { first_correctness, second_correctness } };
        }
        if( discr > -GLOBAL_EPSILON )
        {
            absl::InlinedVector< Point< dimension >, 2 > results;
            results.reserve( 1 );
            results.emplace_back( line.origin() - line.direction() * a1 / a2 );
            typename CorrectnessInfo< absl::InlinedVector< Point< dimension >,
                2 > >::Correctness first_correctness;
            first_correctness.first =
                point_line_distance( results.front(), line ) <= GLOBAL_EPSILON;
            first_correctness.second.push_back(
                point_line_projection( results.front(), line ) );
            const auto output =
                point_ellipse_distance( results.front(), ellipse );
            typename CorrectnessInfo< absl::InlinedVector< Point< dimension >,
                2 > >::Correctness second_correctness;
            second_correctness.first =
                std::get< 0 >( output ) <= GLOBAL_EPSILON;
            second_correctness.second.push_back( std::get< 1 >( output ) );
            return { std::move( results ),
                { first_correctness, second_correctness } };
        }
        return { INTERSECTION_TYPE::none };
    }

    template < index_t dimension >
    IntersectionResult< absl::InlinedVector< Point< dimension >, 2 > >
        segment_ellipse_intersection( const Segment< dimension >& segment,
            const Ellipse< dimension >& ellipse )
    {
        auto line_intersections = line_ellipse_intersection< dimension >(
            InfiniteLine< dimension >{ segment }, ellipse );
        if( line_intersections )
        {
            absl::InlinedVector< Point< dimension >, 2 > segment_intersections;
            segment_intersections.reserve(
                line_intersections.result.value().size() );
            for( auto&& point : line_intersections.result.value() )
            {
                if( point_segment_distance( point, segment ) <= GLOBAL_EPSILON )
                {
                    segment_intersections.emplace_back( point );
                }
            }
            if( segment_intersections.empty() )
            {
                return { INTERSECTION_TYPE::none };
            }
            return { std::move( segment_intersections ),
                std::move( line_intersections.correctness.value() ) };
        }
        return line_intersections.type;
    }

    template IntersectionResult< absl::InlinedVector< Point2D, 2 > >
        opengeode_geometry_api line_sphere_intersection(
            const InfiniteLine2D& segment, const Sphere2D& sphere );

    template IntersectionResult< absl::InlinedVector< Point2D, 2 > >
        opengeode_geometry_api segment_sphere_intersection(
            const Segment2D& segment, const Sphere2D& sphere );

    template IntersectionResult< absl::InlinedVector< Point2D, 2 > >
        opengeode_geometry_api line_ellipse_intersection(
            const InfiniteLine2D& line, const Ellipse2D& sphere );

    template IntersectionResult< absl::InlinedVector< Point2D, 2 > >
        opengeode_geometry_api segment_ellipse_intersection(
            const Segment2D& segment, const Ellipse2D& sphere );

    template IntersectionResult< absl::InlinedVector< Point3D, 2 > >
        opengeode_geometry_api line_sphere_intersection(
            const InfiniteLine3D& segment, const Sphere3D& sphere );

    template IntersectionResult< absl::InlinedVector< Point3D, 2 > >
        opengeode_geometry_api segment_sphere_intersection(
            const Segment3D& segment, const Sphere3D& sphere );

    template IntersectionResult< absl::InlinedVector< Point3D, 2 > >
        opengeode_geometry_api line_ellipse_intersection(
            const InfiniteLine3D& line, const Ellipse3D& sphere );

    template IntersectionResult< absl::InlinedVector< Point3D, 2 > >
        opengeode_geometry_api segment_ellipse_intersection(
            const Segment3D& segment, const Ellipse3D& sphere );

} // namespace geode