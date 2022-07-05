/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/geometry/intersection.h>

#include <geode/geometry/barycentric_coordinates.h>
#include <geode/geometry/basic_objects/infinite_line.h>
#include <geode/geometry/basic_objects/plane.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/sphere.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/distance.h>
#include <geode/geometry/perpendicular.h>
#include <geode/geometry/vector.h>

namespace geode
{
    IntersectionResult< Point3D > line_plane_intersection(
        const InfiniteLine3D& line, const Plane& plane )
    {
        const auto dot_directions = line.direction().dot( plane.normal() );
        if( std::fabs( dot_directions ) < global_epsilon )
        {
            // line is parallel to the plane
            return { IntersectionType::PARALLEL };
        }
        const auto signed_distance =
            plane.normal().dot( Vector3D{ line.origin() } )
            + plane.plane_constant();
        auto result =
            line.origin() - line.direction() * signed_distance / dot_directions;
        CorrectnessInfo< Point3D > correctness;
        double distance;
        std::tie( distance, correctness.first.second ) =
            point_line_distance( result, line );
        correctness.first.first = distance < global_epsilon;
        std::tie( distance, correctness.second.second ) =
            point_plane_distance( result, plane );
        correctness.second.first = distance < global_epsilon;
        return { std::move( result ), std::move( correctness ) };
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
        if( discr > global_epsilon ) // positive
        {
            absl::InlinedVector< Point< dimension >, 2 > results;
            const auto root = std::sqrt( discr );
            results.reserve( 2 );
            results.emplace_back(
                line.origin() + line.direction() * ( -a1 - root ) );
            results.emplace_back(
                line.origin() + line.direction() * ( -a1 + root ) );
            CorrectnessInfo< absl::InlinedVector< Point< dimension >, 2 > >
                correctness;
            double distance;
            Point< dimension > point;
            std::tie( distance, point ) =
                point_line_distance( results.front(), line );
            correctness.first.first = distance < global_epsilon;
            correctness.first.second.push_back( point );
            std::tie( distance, point ) =
                point_line_distance( results.back(), line );
            correctness.first.first =
                correctness.first.first && distance < global_epsilon;
            correctness.first.second.push_back( point );
            std::tie( distance, point ) =
                point_sphere_distance( results.front(), sphere );
            correctness.second.first = distance < global_epsilon;
            correctness.second.second.push_back( point );
            std::tie( distance, point ) =
                point_sphere_distance( results.back(), sphere );
            correctness.second.first =
                correctness.second.first && distance < global_epsilon;
            correctness.second.second.push_back( point );
            return { std::move( results ), std::move( correctness ) };
        }
        else if( discr > -global_epsilon ) // zero
        {
            absl::InlinedVector< Point< dimension >, 2 > results;
            results.reserve( 1 );
            results.emplace_back( line.origin() - line.direction() * a1 );
            CorrectnessInfo< absl::InlinedVector< Point< dimension >, 2 > >
                correctness;
            double distance;
            Point< dimension > point;
            std::tie( distance, point ) =
                point_line_distance( results.front(), line );
            correctness.first.first = distance < global_epsilon;
            correctness.first.second.push_back( point );
            std::tie( distance, point ) =
                point_sphere_distance( results.front(), sphere );
            correctness.second.first = distance < global_epsilon;
            correctness.second.second.push_back( point );
            return { std::move( results ), std::move( correctness ) };
        }
        return {}; // negative
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
                double distance_to_segment;
                std::tie( distance_to_segment, std::ignore ) =
                    point_segment_distance( point, segment );
                if( distance_to_segment <= global_epsilon )
                {
                    segment_intersections.emplace_back( point );
                }
            }
            if( segment_intersections.empty() )
            {
                return {};
            }
            return { std::move( segment_intersections ),
                std::move( line_intersections.correctness ) };
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
            double distance_to_segment;
            std::tie( distance_to_segment, std::ignore ) =
                point_segment_distance(
                    line_plane_result.result.value(), segment );
            if( distance_to_segment > global_epsilon )
            {
                return {};
            }
            std::array< double, 2 > lambdas;
            try
            {
                lambdas = segment_barycentric_coordinates(
                    line_plane_result.result.value(), segment );
            }
            catch( const OpenGeodeException& )
            {
                lambdas.fill( 0.5 );
            }
            line_plane_result.correctness.first.second =
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
        const Vector3D edge1{ triangle.vertices()[0], triangle.vertices()[1] };
        const Vector3D edge2{ triangle.vertices()[0], triangle.vertices()[2] };
        const auto normal = edge1.cross( edge2 );

        // Solve Q + t*D = b1*E1 + b2*E2 (Q = diff, D = segment direction,
        // E1 = edge1, E2 = edge2, N = Cross(E1,E2)) by
        //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
        //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
        //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
        auto DdN = segment.normalized_direction().dot( normal );
        signed_index_t sign;
        if( DdN > 0. )
        {
            sign = 1;
        }
        else if( DdN < -0. )
        {
            sign = -1;
            DdN = -DdN;
        }
        else
        {
            // Segment and triangle are parallel
            return { IntersectionType::PARALLEL };
        }

        const Vector3D diff{ triangle.vertices()[0], seg_center };
        const auto DdQxE2 =
            sign * segment.normalized_direction().dot( diff.cross( edge2 ) );
        if( DdQxE2 >= 0 )
        {
            const auto DdE1xQ =
                sign
                * segment.normalized_direction().dot( edge1.cross( diff ) );
            if( DdE1xQ >= 0 && DdQxE2 + DdE1xQ <= DdN )
            {
                // InfiniteLine intersects triangle, check if segment does.
                const auto QdN = -sign * diff.dot( normal );
                const auto extDdN = segment.length() * DdN / 2.;
                if( -extDdN <= QdN && QdN <= extDdN )
                {
                    // Segment intersects triangle.
                    const auto inv = 1. / DdN;
                    const auto seg_parameter = QdN * inv;

                    auto result =
                        seg_center
                        + segment.normalized_direction() * seg_parameter;
                    CorrectnessInfo< Point3D > correctness;

                    std::array< double, 2 > seg_lambdas;
                    try
                    {
                        seg_lambdas =
                            segment_barycentric_coordinates( result, segment );
                    }
                    catch( const OpenGeodeException& )
                    {
                        seg_lambdas.fill( 0.5 );
                    }
                    correctness.first.second =
                        segment.vertices()[0].get() * seg_lambdas[0]
                        + segment.vertices()[1].get() * seg_lambdas[1];
                    correctness.first.first =
                        std::get< 0 >( point_segment_distance(
                            correctness.first.second, segment ) )
                        < global_epsilon;
                    std::array< double, 3 > tri_lambdas;
                    try
                    {
                        tri_lambdas = triangle_barycentric_coordinates(
                            result, triangle );
                    }
                    catch( const OpenGeodeException& )
                    {
                        tri_lambdas.fill( 1. / 3. );
                    }
                    correctness.second.second =
                        triangle.vertices()[0].get() * tri_lambdas[0]
                        + triangle.vertices()[1].get() * tri_lambdas[1]
                        + triangle.vertices()[2].get() * tri_lambdas[2];
                    correctness.second.first =
                        std::get< 0 >( point_triangle_distance(
                            correctness.second.second, triangle ) )
                        < global_epsilon;
                    return { std::move( result ), std::move( correctness ) };
                }
                // else: |t| > extent, no intersection
            }
            // else: b1+b2 > 1, no intersection
            // else: b2 < 0, no intersection
        }
        // else: b1 < 0, no intersection
        return {};
    }

    IntersectionResult< Point3D > line_triangle_intersection(
        const InfiniteLine3D& line, const Triangle3D& triangle )
    {
        // http://www.geometrictools.com/LibMathematics/Intersection/Intersection.html
        // Compute the offset origin, edges, and normal.
        const Vector3D edge1{ triangle.vertices()[0], triangle.vertices()[1] };
        const Vector3D edge2{ triangle.vertices()[0], triangle.vertices()[2] };
        const auto normal = edge1.cross( edge2 );

        // Solve Q + t*D = b1*E1 + b2*E2 (Q = diff, D = segment direction,
        // E1 = edge1, E2 = edge2, N = Cross(E1,E2)) by
        //   |Dot(D,N)|*b1 = sign(Dot(D,N))*Dot(D,Cross(Q,E2))
        //   |Dot(D,N)|*b2 = sign(Dot(D,N))*Dot(D,Cross(E1,Q))
        //   |Dot(D,N)|*t = -sign(Dot(D,N))*Dot(Q,N)
        auto DdN = line.direction().dot( normal );
        signed_index_t sign;
        if( DdN > 0. )
        {
            sign = 1;
        }
        else if( DdN < -0. )
        {
            sign = -1;
            DdN = -DdN;
        }
        else
        {
            // Segment and triangle are parallel
            return { IntersectionType::PARALLEL };
        }

        const Vector3D diff{ triangle.vertices()[0], line.origin() };
        const auto DdQxE2 = sign * line.direction().dot( diff.cross( edge2 ) );
        if( DdQxE2 >= 0 )
        {
            const auto DdE1xQ =
                sign * line.direction().dot( edge1.cross( diff ) );
            if( DdE1xQ >= 0 && DdQxE2 + DdE1xQ <= DdN )
            {
                // InfiniteLine intersects triangle.
                const auto QdN = -sign * diff.dot( normal );
                const auto inv = 1. / DdN;
                const auto seg_parameter = QdN * inv;

                auto result = line.origin() + line.direction() * seg_parameter;

                CorrectnessInfo< Point3D > correctness;
                double distance;
                std::tie( distance, correctness.first.second ) =
                    point_line_distance( result, line );
                correctness.first.first = distance < global_epsilon;
                std::array< double, 3 > tri_lambdas;
                try
                {
                    tri_lambdas =
                        triangle_barycentric_coordinates( result, triangle );
                }
                catch( const OpenGeodeException& )
                {
                    tri_lambdas.fill( 1. / 3. );
                }
                correctness.second.second =
                    triangle.vertices()[0].get() * tri_lambdas[0]
                    + triangle.vertices()[1].get() * tri_lambdas[1]
                    + triangle.vertices()[2].get() * tri_lambdas[2];
                correctness.second.first =
                    point_point_distance( result, correctness.second.second )
                    < global_epsilon;
                return { std::move( result ), std::move( correctness ) };
            }
            // else: b1+b2 > 1, no intersection
            // else: b2 < 0, no intersection
        }
        // else: b1 < 0, no intersection
        return {};
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
            return { IntersectionType::PARALLEL };
        }

        const auto invD0DotPerpD1 = 1.0 / D0DotPerpD1;
        const auto diffDotPerpD1 = dot_perpendicular( diff, line1.direction() );
        const auto s0 = diffDotPerpD1 * invD0DotPerpD1;
        auto result = line0.origin() + line0.direction() * s0;
        CorrectnessInfo< Point2D > correctness;
        double distance;
        std::tie( distance, correctness.first.second ) =
            point_line_distance( result, line0 );
        correctness.first.first = distance < global_epsilon;
        std::tie( distance, correctness.second.second ) =
            point_line_distance( result, line1 );
        correctness.second.first = distance < global_epsilon;
        return { std::move( result ), std::move( correctness ) };
    }

    IntersectionResult< Point2D > segment_segment_intersection(
        const Segment2D& segment0, const Segment2D& segment1 )
    {
        auto line_intersection_result = line_line_intersection(
            InfiniteLine2D{ segment0 }, InfiniteLine2D{ segment1 } );
        if( line_intersection_result )
        {
            // Test whether the line-line intersection is on the segments.
            double distance_to_segment;
            std::tie( distance_to_segment, std::ignore ) =
                point_segment_distance(
                    line_intersection_result.result.value(), segment0 );
            if( distance_to_segment > global_epsilon )
            {
                return {};
            }
            std::tie( distance_to_segment, std::ignore ) =
                point_segment_distance(
                    line_intersection_result.result.value(), segment1 );
            if( distance_to_segment > global_epsilon )
            {
                return {};
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
            line_intersection_result.correctness.first.second =
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
            line_intersection_result.correctness.second.second =
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
            double distance_to_segment;
            std::tie( distance_to_segment, std::ignore ) =
                point_segment_distance(
                    line_intersection_result.result.value(), segment );
            if( distance_to_segment > global_epsilon )
            {
                return {};
            }
            std::array< double, 2 > lambdas;
            try
            {
                lambdas = segment_barycentric_coordinates(
                    line_intersection_result.result.value(), segment );
            }
            catch( const OpenGeodeException& )
            {
                lambdas.fill( 0.5 );
            }
            line_intersection_result.correctness.first.second =
                segment.vertices()[0].get() * lambdas[0]
                + segment.vertices()[1].get() * lambdas[1];
            return line_intersection_result;
        }
        return line_intersection_result.type;
    }

    template IntersectionResult< absl::InlinedVector< Point2D, 2 > >
        opengeode_geometry_api line_sphere_intersection(
            const InfiniteLine2D& segment, const Sphere2D& sphere );

    template IntersectionResult< absl::InlinedVector< Point2D, 2 > >
        opengeode_geometry_api segment_sphere_intersection(
            const Segment2D& segment, const Sphere2D& sphere );

    template IntersectionResult< absl::InlinedVector< Point3D, 2 > >
        opengeode_geometry_api line_sphere_intersection(
            const InfiniteLine3D& segment, const Sphere3D& sphere );

    template IntersectionResult< absl::InlinedVector< Point3D, 2 > >
        opengeode_geometry_api segment_sphere_intersection(
            const Segment3D& segment, const Sphere3D& sphere );

} // namespace geode