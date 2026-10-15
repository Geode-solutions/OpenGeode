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

#include <geode/geometry/intersection.hpp>

#include <algorithm>
#include <limits>
#include <optional>
#include <tuple>

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
    /*
     * Triangle target of line_part_intersection
     */
    class TriangleTarget
    {
    public:
        explicit TriangleTarget( const geode::Triangle3D& triangle )
            : triangle_( triangle ), normal_( triangle.normal() )
        {
        }

        const geode::Point3D& origin() const
        {
            return triangle_.vertices()[0];
        }

        // Unit normal, none for a degenerated triangle
        const std::optional< geode::Vector3D >& normal() const
        {
            return normal_;
        }

        double extent() const
        {
            const auto& vertices = triangle_.vertices();
            return std::max( { geode::point_point_distance(
                                   vertices[0].get(), vertices[1].get() ),
                geode::point_point_distance(
                    vertices[1].get(), vertices[2].get() ),
                geode::point_point_distance(
                    vertices[2].get(), vertices[0].get() ) } );
        }

        std::tuple< double, geode::Point3D > closest(
            const geode::Point3D& point ) const
        {
            return geode::point_triangle_distance( point, triangle_ );
        }

    private:
        const geode::Triangle3D& triangle_;
        std::optional< geode::Vector3D > normal_;
    };

    /*
     * 2D segment target of line_part_intersection
     */
    class SegmentTarget
    {
    public:
        explicit SegmentTarget( const geode::Segment2D& segment )
            : segment_( segment ), length_( segment.length() )
        {
            if( length_ > geode::GLOBAL_EPSILON )
            {
                const geode::Vector2D direction{ segment.vertices()[0],
                    segment.vertices()[1] };
                normal_ = geode::Vector2D{ { -direction.value( 1 ) / length_,
                    direction.value( 0 ) / length_ } };
            }
        }

        const geode::Point2D& origin() const
        {
            return segment_.vertices()[0];
        }

        // Unit normal, none for a segment shorter than GLOBAL_EPSILON
        const std::optional< geode::Vector2D >& normal() const
        {
            return normal_;
        }

        double extent() const
        {
            return length_;
        }

        std::tuple< double, geode::Point2D > closest(
            const geode::Point2D& point ) const
        {
            auto projection =
                geode::point_segment_projection( point, segment_ );
            const auto distance =
                geode::point_point_distance( point, projection );
            return { distance, std::move( projection ) };
        }

    private:
        const geode::Segment2D& segment_;
        double length_;
        std::optional< geode::Vector2D > normal_;
    };

    /*
     * Intersection between a target (triangle in 3D, segment in 2D) and the
     * part of the line X = point0 + alpha * (point1 - point0) with alpha in
     * [alpha_min, alpha_max]. Objects closer than GLOBAL_EPSILON are
     * considered intersecting. The line part is parallel when its direction is
     * parallel to the target over the target extent (whatever their
     * distance), or when it is within GLOBAL_EPSILON of the target support
     * near the target (and of the target). The distance fallback returns the
     * distance between the line part and the target, and their closest
     * points.
     */
    template < geode::index_t dimension,
        typename Target,
        typename DistanceFallback >
    geode::IntersectionResult< geode::Point< dimension > >
        line_part_intersection( const geode::Point< dimension >& point0,
            const geode::Point< dimension >& point1,
            double alpha_min,
            double alpha_max,
            const Target& target,
            const DistanceFallback& distance_fallback )
    {
        using Result = geode::IntersectionResult< geode::Point< dimension > >;
        using Correctness = typename geode::CorrectnessInfo<
            geode::Point< dimension > >::Correctness;
        // The point is on the line part, within GLOBAL_EPSILON of its closest
        // point on the target
        const auto build_result = []( geode::Point< dimension > point,
                                      geode::Point< dimension > on_target ) {
            Correctness first_correctness{ true, point };
            Correctness second_correctness{ true, std::move( on_target ) };
            return Result{ std::move( point ),
                { first_correctness, second_correctness } };
        };
        const auto from_fallback = [&]() {
            auto [distance, on_line, on_target] = distance_fallback();
            if( distance > geode::GLOBAL_EPSILON )
            {
                return Result{ geode::INTERSECTION_TYPE::none };
            }
            return build_result( std::move( on_line ), std::move( on_target ) );
        };
        const auto& normal = target.normal();
        if( !normal )
        {
            return from_fallback();
        }
        const auto& origin = target.origin();
        const geode::Vector< dimension > direction{ point0, point1 };
        const auto length = direction.length();
        const auto height0 =
            normal->dot( geode::Vector< dimension >{ origin, point0 } );
        const auto height1 =
            normal->dot( geode::Vector< dimension >{ origin, point1 } );
        const auto height_slope = height1 - height0;
        const auto target_extent = target.extent();
        if( std::fabs( height_slope ) * target_extent
            <= geode::GLOBAL_EPSILON * length )
        {
            // Parallel to the target support over the target extent
            return Result{ geode::INTERSECTION_TYPE::parallel };
        }
        // Restrict the line part to the neighborhood of the target
        const auto extent = ( target_extent + geode::GLOBAL_EPSILON ) / length;
        const auto alpha_center =
            direction.dot( geode::Vector< dimension >{ point0, origin } )
            / ( length * length );
        const auto alpha_begin = std::max( alpha_min, alpha_center - extent );
        const auto alpha_end = std::min( alpha_max, alpha_center + extent );
        if( alpha_begin > alpha_end )
        {
            return Result{ geode::INTERSECTION_TYPE::none };
        }
        const auto height_begin = height0 + height_slope * alpha_begin;
        const auto height_end = height0 + height_slope * alpha_end;
        if( std::fabs( height_begin ) <= geode::GLOBAL_EPSILON
            && std::fabs( height_end ) <= geode::GLOBAL_EPSILON )
        {
            if( std::get< 0 >( distance_fallback() ) > geode::GLOBAL_EPSILON )
            {
                return Result{ geode::INTERSECTION_TYPE::none };
            }
            return Result{ geode::INTERSECTION_TYPE::parallel };
        }
        if( ( height_begin > geode::GLOBAL_EPSILON
                && height_end > geode::GLOBAL_EPSILON )
            || ( height_begin < -geode::GLOBAL_EPSILON
                 && height_end < -geode::GLOBAL_EPSILON ) )
        {
            return Result{ geode::INTERSECTION_TYPE::none };
        }
        // Crossing of the target support
        const auto alpha_support = -height0 / height_slope;
        const auto alpha_crossing =
            std::clamp( alpha_support, alpha_begin, alpha_end );
        auto crossing = point0 + direction * alpha_crossing;
        auto [crossing_distance, on_target] = target.closest( crossing );
        if( crossing_distance <= geode::GLOBAL_EPSILON )
        {
            return build_result(
                std::move( crossing ), std::move( on_target ) );
        }
        // The line part distance to the target is at least
        // crossing_distance * |sin(angle)| / sqrt(2)
        if( alpha_crossing == alpha_support
            && crossing_distance * std::fabs( height_slope )
                   > 2 * geode::GLOBAL_EPSILON * length )
        {
            return Result{ geode::INTERSECTION_TYPE::none };
        }
        return from_fallback();
    }

    /*
     * Intersection result when one of the two objects is a degenerated segment
     * (considered as a point), given the closest points between the two
     * objects and their distance. The result is the point on the first object.
     */
    template < typename Intersection, typename PointType >
    geode::IntersectionResult< Intersection > degenerated_segment_intersection(
        const PointType& point_on_first,
        double distance,
        const PointType& point_on_second )
    {
        if( distance > geode::GLOBAL_EPSILON )
        {
            return { geode::INTERSECTION_TYPE::none };
        }
        typename geode::CorrectnessInfo< Intersection >::Correctness
            first_correctness{ true, Intersection{ point_on_first } };
        typename geode::CorrectnessInfo< Intersection >::Correctness
            second_correctness{ true, Intersection{ point_on_second } };
        return { Intersection{ point_on_first },
            { first_correctness, second_correctness } };
    }

    /*
     * Keep the line intersections located on the segment, with their
     * correctness information.
     */
    template < typename SegmentType, typename PointType >
    geode::IntersectionResult< absl::InlinedVector< PointType, 2 > >
        filter_segment_intersections(
            geode::IntersectionResult< absl::InlinedVector< PointType, 2 > >
                line_intersections,
            const SegmentType& segment )
    {
        using Intersections = absl::InlinedVector< PointType, 2 >;
        const auto& points = line_intersections.result.value();
        const auto& correctness = line_intersections.correctness.value();
        Intersections segment_intersections;
        typename geode::CorrectnessInfo< Intersections >::Correctness
            first_correctness{ correctness.first.first, {} };
        typename geode::CorrectnessInfo< Intersections >::Correctness
            second_correctness{ correctness.second.first, {} };
        for( const auto i : geode::Indices{ points } )
        {
            if( geode::point_segment_distance( points[i], segment )
                > geode::GLOBAL_EPSILON )
            {
                continue;
            }
            segment_intersections.push_back( points[i] );
            first_correctness.second.push_back( correctness.first.second[i] );
            second_correctness.second.push_back( correctness.second.second[i] );
        }
        if( segment_intersections.empty() )
        {
            return { geode::INTERSECTION_TYPE::none };
        }
        return { std::move( segment_intersections ),
            { first_correctness, second_correctness } };
    }
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
        // Compare the distance between the sphere center and the line with
        // the sphere radius
        const auto& center = sphere.origin();
        const auto radius = sphere.radius();
        const auto foot = point_line_projection( center, line );
        const auto height = point_point_distance( center, foot );
        if( height > radius + GLOBAL_EPSILON )
        {
            return { INTERSECTION_TYPE::none };
        }
        const auto half_chord =
            height < radius
                ? std::sqrt( ( radius - height ) * ( radius + height ) )
                : 0.;
        absl::InlinedVector< Point< dimension >, 2 > results;
        if( half_chord <= GLOBAL_EPSILON )
        {
            // Tangent line
            results.emplace_back( foot );
        }
        else
        {
            results.emplace_back( foot - line.direction() * half_chord );
            results.emplace_back( foot + line.direction() * half_chord );
        }
        typename CorrectnessInfo< absl::InlinedVector< Point< dimension >,
            2 > >::Correctness first_correctness;
        typename CorrectnessInfo< absl::InlinedVector< Point< dimension >,
            2 > >::Correctness second_correctness;
        first_correctness.first = true;
        second_correctness.first = true;
        for( const auto& point : results )
        {
            first_correctness.first =
                first_correctness.first
                && point_line_distance( point, line ) <= GLOBAL_EPSILON;
            first_correctness.second.push_back(
                point_line_projection( point, line ) );
            const auto output = point_sphere_distance( point, sphere );
            second_correctness.first =
                second_correctness.first
                && std::get< 0 >( output ) <= GLOBAL_EPSILON;
            second_correctness.second.push_back( std::get< 1 >( output ) );
        }
        return { std::move( results ),
            { first_correctness, second_correctness } };
    }

    template < index_t dimension >
    IntersectionResult< absl::InlinedVector< Point< dimension >, 2 > >
        segment_sphere_intersection( const Segment< dimension >& segment,
            const Sphere< dimension >& sphere )
    {
        if( segment.length() <= GLOBAL_EPSILON )
        {
            // Segment considered as a point
            const auto& vertex = segment.vertices()[0].get();
            const auto output = point_sphere_distance( vertex, sphere );
            if( std::get< 0 >( output ) > GLOBAL_EPSILON )
            {
                return { INTERSECTION_TYPE::none };
            }
            typename CorrectnessInfo< absl::InlinedVector< Point< dimension >,
                2 > >::Correctness first_correctness{ true, { vertex } };
            typename CorrectnessInfo< absl::InlinedVector< Point< dimension >,
                2 > >::Correctness second_correctness{ true,
                { std::get< 1 >( output ) } };
            return { absl::InlinedVector< Point< dimension >, 2 >{ vertex },
                { first_correctness, second_correctness } };
        }
        auto line_intersections = line_sphere_intersection(
            InfiniteLine< dimension >{ segment }, sphere );
        if( !line_intersections.result )
        {
            return line_intersections.type;
        }
        absl::InlinedVector< Point< dimension >, 2 > segment_intersections;
        typename CorrectnessInfo< absl::InlinedVector< Point< dimension >,
            2 > >::Correctness first_correctness{ true, {} };
        typename CorrectnessInfo< absl::InlinedVector< Point< dimension >,
            2 > >::Correctness second_correctness{ true, {} };
        for( auto& point : line_intersections.result.value() )
        {
            // The segment (not only the point) must be within GLOBAL_EPSILON
            // of the sphere
            auto on_segment = point_segment_projection( point, segment );
            if( point_point_distance( point, on_segment ) > GLOBAL_EPSILON
                || std::get< 0 >( point_sphere_distance( on_segment, sphere ) )
                       > GLOBAL_EPSILON )
            {
                continue;
            }
            first_correctness.second.push_back( std::move( on_segment ) );
            second_correctness.second.push_back(
                std::get< 1 >( point_sphere_distance( point, sphere ) ) );
            segment_intersections.push_back( std::move( point ) );
        }
        if( segment_intersections.empty() )
        {
            return { INTERSECTION_TYPE::none };
        }
        return { std::move( segment_intersections ),
            { first_correctness, second_correctness } };
    }

    IntersectionResult< Point3D > segment_plane_intersection(
        const Segment3D& segment, const Plane& plane )
    {
        if( segment.is_degenerated() )
        {
            const auto point = segment.barycenter();
            const auto [distance, projection] =
                point_plane_distance( point, plane );
            return degenerated_segment_intersection< Point3D >(
                point, distance, projection );
        }
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
        const auto& origin = segment.vertices()[0].get();
        const auto length = segment.length();
        if( length <= GLOBAL_EPSILON )
        {
            // Segment considered as a point
            auto [distance, on_triangle] =
                point_triangle_distance( origin, triangle );
            if( distance > GLOBAL_EPSILON )
            {
                return { INTERSECTION_TYPE::none };
            }
            CorrectnessInfo< Point3D >::Correctness first_correctness{ true,
                origin };
            CorrectnessInfo< Point3D >::Correctness second_correctness{ true,
                std::move( on_triangle ) };
            return { origin, { first_correctness, second_correctness } };
        }
        return line_part_intersection( origin, segment.vertices()[1].get(), 0.,
            1., TriangleTarget{ triangle }, [&segment, &triangle] {
                return segment_triangle_distance( segment, triangle );
            } );
    }

    IntersectionResult< Point3D > line_triangle_intersection(
        const InfiniteLine3D& line, const Triangle3D& triangle )
    {
        constexpr auto infinity = std::numeric_limits< double >::infinity();
        return line_part_intersection( line.origin(),
            line.origin() + line.direction(), -infinity, infinity,
            TriangleTarget{ triangle }, [&line, &triangle] {
                return line_triangle_distance( line, triangle );
            } );
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
        if( D0DotPerpD1 == 0. )
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
        const auto& origin = segment0.vertices()[0].get();
        const auto distance_fallback = [&segment0, &segment1] {
            return segment_segment_distance( segment0, segment1 );
        };
        if( segment0.length() <= GLOBAL_EPSILON )
        {
            // Segment considered as a point
            auto [distance, closest0, closest1] = distance_fallback();
            if( distance > GLOBAL_EPSILON )
            {
                return { INTERSECTION_TYPE::none };
            }
            CorrectnessInfo< Point2D >::Correctness first_correctness{ true,
                origin };
            CorrectnessInfo< Point2D >::Correctness second_correctness{ true,
                closest1 };
            return { origin, { first_correctness, second_correctness } };
        }
        return line_part_intersection( origin, segment0.vertices()[1].get(), 0.,
            1., SegmentTarget{ segment1 }, distance_fallback );
    }

    IntersectionResult< Point2D > segment_line_intersection(
        const Segment2D& segment, const InfiniteLine2D& line )
    {
        if( segment.is_degenerated() )
        {
            const auto point = segment.barycenter();
            return degenerated_segment_intersection< Point2D >( point,
                point_line_distance( point, line ),
                point_line_projection( point, line ) );
        }
        // Heights of the segment vertices relative to the line. Same criteria
        // as line_part_intersection with the segment as line part and the
        // line as target: |height1 - height0| is the segment deviation from
        // the line direction over the segment extent.
        const auto& vertex0 = segment.vertices()[0].get();
        const auto& vertex1 = segment.vertices()[1].get();
        const Vector2D normal{ { -line.direction().value( 1 ),
            line.direction().value( 0 ) } };
        const auto height0 = normal.dot( Vector2D{ line.origin(), vertex0 } );
        const auto height1 = normal.dot( Vector2D{ line.origin(), vertex1 } );
        if( std::fabs( height1 - height0 ) <= GLOBAL_EPSILON
            || ( std::fabs( height0 ) <= GLOBAL_EPSILON
                 && std::fabs( height1 ) <= GLOBAL_EPSILON ) )
        {
            // Parallel to the line or included in the line
            return { INTERSECTION_TYPE::parallel };
        }
        if( ( height0 > GLOBAL_EPSILON && height1 > GLOBAL_EPSILON )
            || ( height0 < -GLOBAL_EPSILON && height1 < -GLOBAL_EPSILON ) )
        {
            return { INTERSECTION_TYPE::none };
        }
        const auto alpha =
            std::clamp( height0 / ( height0 - height1 ), 0., 1. );
        auto result = vertex0 + ( vertex1 - vertex0 ) * alpha;
        CorrectnessInfo< Point2D >::Correctness first_correctness{
            point_segment_distance( result, segment ) <= GLOBAL_EPSILON,
            point_segment_projection( result, segment )
        };
        CorrectnessInfo< Point2D >::Correctness second_correctness{
            point_line_distance( result, line ) <= GLOBAL_EPSILON,
            point_line_projection( result, line )
        };
        return { std::move( result ),
            { first_correctness, second_correctness } };
    }

    IntersectionResult< absl::InlinedVector< Point3D, 2 > >
        opengeode_geometry_api line_cylinder_intersection(
            const InfiniteLine3D& line, const Cylinder& cylinder )
    {
        CylinderLineResult result;
        // The line direction is unit: parameters are lengths along the line.
        // Parameters closer than GLOBAL_EPSILON give the same point.
        const auto add_parameter = [&result]( double parameter ) {
            for( const auto i : LRange{ result.numIntersections } )
            {
                if( std::fabs( parameter - result.parameter[i] )
                    <= GLOBAL_EPSILON )
                {
                    return;
                }
            }
            if( result.numIntersections < 2 )
            {
                result.parameter[result.numIntersections++] = parameter;
            }
        };
        // Create a coordinate system for the cylinder. In this system,
        // the cylinder segment center C is the origin and the cylinder
        // axis direction W is the z-axis. U and V are the other
        // coordinate axis directions. If P = x*U+y*V+z*W, the cylinder
        // is x^2 + y^2 = r^2, where r is the cylinder radius. The end
        // caps are |z| = h/2, where h is the cylinder height.
        const auto basis =
            compute_orthogonal_basis( cylinder.axis().direction() );
        const auto& W = basis[0];
        const auto& U = basis[1];
        const auto& V = basis[2];
        const auto halfHeight = 0.5 * cylinder.axis().length();
        const auto radius = cylinder.radius();

        const auto cylinder_barycenter = cylinder.axis().barycenter();

        // Convert incoming line origin to capsule coordinates.
        const Vector3D diff{ cylinder_barycenter, line.origin() };
        Vector3D P{ { U.dot( diff ), V.dot( diff ), W.dot( diff ) } };

        const auto is_inside_radius = [radius]( double x, double y ) {
            return std::sqrt( x * x + y * y ) - radius <= GLOBAL_EPSILON;
        };
        // Intersections of line P+t*D with the infinite cylinder
        // x^2+y^2 = r^2, using the distance h from the line to the cylinder
        // axis: the intersections are at
        // t = t_closest +- sqrt( (r-h)*(r+h) ) / |D_xy|.
        const auto wall_parameters = [&P, &is_inside_radius, radius](
                                         const Vector3D& D ) {
            absl::InlinedVector< double, 2 > parameters;
            const auto a1 =
                P.value( 0 ) * D.value( 0 ) + P.value( 1 ) * D.value( 1 );
            const auto a2 =
                D.value( 0 ) * D.value( 0 ) + D.value( 1 ) * D.value( 1 );
            const auto closest_parameter = -a1 / a2;
            const auto x = P.value( 0 ) + closest_parameter * D.value( 0 );
            const auto y = P.value( 1 ) + closest_parameter * D.value( 1 );
            if( !is_inside_radius( x, y ) )
            {
                return parameters;
            }
            const auto distance_to_axis = std::sqrt( x * x + y * y );
            if( radius - distance_to_axis <= GLOBAL_EPSILON )
            {
                // The line is tangent to the cylinder.
                parameters.push_back( closest_parameter );
                return parameters;
            }
            const auto half_chord =
                std::sqrt( ( radius - distance_to_axis )
                           * ( radius + distance_to_axis ) / a2 );
            parameters.push_back( closest_parameter - half_chord );
            parameters.push_back( closest_parameter + half_chord );
            return parameters;
        };

        // Get the z-value, in cylinder coordinates, of the incoming
        // line's unit-length direction.
        const auto dz = W.dot( line.direction() );
        if( std::fabs( dz ) == 1 )
        {
            // The line is parallel to the cylinder axis. Determine
            // whether the line intersects the cylinder end disks.
            if( !is_inside_radius( P.value( 0 ), P.value( 1 ) ) )
            {
                // The line is outside the cylinder, no intersection.
                return { INTERSECTION_TYPE::none };
            }
            if( dz > 0 )
            {
                add_parameter( -P.value( 2 ) - halfHeight );
                add_parameter( -P.value( 2 ) + halfHeight );
            }
            else
            {
                add_parameter( P.value( 2 ) - halfHeight );
                add_parameter( P.value( 2 ) + halfHeight );
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
                if( std::fabs( P.value( 2 ) ) - halfHeight > GLOBAL_EPSILON )
                {
                    // The line is outside the planes of the cylinder end
                    // disks.
                    return { INTERSECTION_TYPE::none };
                }
                for( const auto parameter : wall_parameters( D ) )
                {
                    add_parameter( parameter );
                }
            }
            else
            {
                // At this time, the line direction is neither parallel nor
                // perpendicular to the cylinder axis. The line must
                // intersect both planes of the end disk, the intersection
                // with the cylinder being a segment. The t-interval of the
                // segment is [t0,t1].

                // Test for intersections with the planes of the end disks.
                const auto t0 = ( -halfHeight - P.value( 2 ) ) / D.value( 2 );
                if( is_inside_radius( P.value( 0 ) + t0 * D.value( 0 ),
                        P.value( 1 ) + t0 * D.value( 1 ) ) )
                {
                    // Plane intersection inside the bottom cylinder end disk.
                    add_parameter( t0 );
                }

                const auto t1 = ( halfHeight - P.value( 2 ) ) / D.value( 2 );
                if( is_inside_radius( P.value( 0 ) + t1 * D.value( 0 ),
                        P.value( 1 ) + t1 * D.value( 1 ) ) )
                {
                    // Plane intersection inside the top cylinder end disk.
                    add_parameter( t1 );
                }

                if( result.numIntersections < 2 )
                {
                    // Test for intersection with the cylinder wall, between
                    // the end disk planes. A parameter shift of
                    // GLOBAL_EPSILON / |dz| moves the point by GLOBAL_EPSILON
                    // along the axis.
                    const auto tolerance = GLOBAL_EPSILON / std::fabs( dz );
                    const auto min_parameter = std::min( t0, t1 ) - tolerance;
                    const auto max_parameter = std::max( t0, t1 ) + tolerance;
                    for( const auto parameter : wall_parameters( D ) )
                    {
                        if( min_parameter <= parameter
                            && parameter <= max_parameter )
                        {
                            add_parameter( parameter );
                        }
                    }
                }
                // else: Line intersects both top and bottom cylinder end
                // disks.
            }
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
        for( const auto& point : results )
        {
            // distance to line
            first_correctness.first =
                first_correctness.first
                && point_line_distance( point, line ) <= GLOBAL_EPSILON;
            first_correctness.second.push_back(
                point_line_projection( point, line ) );

            // distance to cylinder
            // first check distance and projection orientation to the cylinder
            // limits
            bool cur_correctness{ false };
            for( const auto v : LRange{ 2 } )
            {
                const auto distance = point_point_distance(
                    point, cylinder.axis().vertices()[v].get() );
                if( distance <= GLOBAL_EPSILON )
                {
                    cur_correctness = true;
                    break;
                }
                if( distance < cylinder.radius() )
                {
                    const geode::Vector3D point_to_vertex{ point,
                        cylinder.axis().vertices()[v] };
                    if( cylinder.axis().direction().dot(
                            point_to_vertex.normalize() )
                        <= GLOBAL_EPSILON )
                    {
                        cur_correctness = true;
                        break;
                    }
                }
            }
            // then check distance to the cylinder axis
            if( !cur_correctness )
            {
                const auto distance =
                    point_segment_distance( point, cylinder.axis() );
                cur_correctness =
                    std::fabs( distance - cylinder.radius() ) <= GLOBAL_EPSILON;
            }
            second_correctness.first =
                second_correctness.first && cur_correctness;
            second_correctness.second.push_back( point );
        }
        return { std::move( results ),
            { first_correctness, second_correctness } };
    }

    IntersectionResult< absl::InlinedVector< Point3D, 2 > >
        opengeode_geometry_api segment_cylinder_intersection(
            const Segment3D& segment, const Cylinder& cylinder )
    {
        if( segment.is_degenerated() )
        {
            const auto point = segment.barycenter();
            const auto [distance, projection] =
                point_cylinder_distance( point, cylinder );
            return degenerated_segment_intersection<
                absl::InlinedVector< Point3D, 2 > >(
                point, distance, projection );
        }
        auto line_intersections =
            line_cylinder_intersection( InfiniteLine3D{ segment }, cylinder );
        if( line_intersections )
        {
            return filter_segment_intersections(
                std::move( line_intersections ), segment );
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
            second_correctness{ plane_intersection.correctness->second.first,
                {} };
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
        // Heights of the circle points relative to the plane:
        // h(theta) = h_c + radius * ( a * cos(theta) + b * sin(theta) )
        //          = h_c + amplitude * cos(theta - phi)
        const auto& center = circle.plane().origin();
        const auto radius = circle.radius();
        const auto basis = compute_orthogonal_basis( circle.plane().normal() );
        const auto& axis0 = basis[1];
        const auto& axis1 = basis[2];
        const auto center_height =
            plane.normal().dot( Vector3D{ plane.origin(), center } );
        const auto a = plane.normal().dot( axis0 );
        const auto b = plane.normal().dot( axis1 );
        const auto amplitude = radius * std::sqrt( a * a + b * b );
        if( amplitude <= GLOBAL_EPSILON )
        {
            // The circle is parallel to the plane over its extent
            if( std::fabs( center_height ) <= GLOBAL_EPSILON )
            {
                return { INTERSECTION_TYPE::parallel };
            }
            return { INTERSECTION_TYPE::none };
        }
        if( std::fabs( center_height ) > amplitude + GLOBAL_EPSILON )
        {
            return { INTERSECTION_TYPE::none };
        }
        const auto phi = std::atan2( b, a );
        const auto delta =
            std::acos( std::clamp( -center_height / amplitude, -1., 1. ) );
        const auto circle_point = [&]( double theta ) {
            return center
                   + ( axis0 * std::cos( theta ) + axis1 * std::sin( theta ) )
                         * radius;
        };
        absl::InlinedVector< Point3D, 2 > result;
        if( radius * std::sin( delta ) <= GLOBAL_EPSILON )
        {
            // The circle touches the plane
            result.emplace_back( circle_point( phi + delta ) );
        }
        else
        {
            // Ordered along the planes intersection direction
            result.emplace_back( circle_point( phi - delta ) );
            result.emplace_back( circle_point( phi + delta ) );
            const auto direction =
                plane.normal().cross( circle.plane().normal() );
            if( Vector3D{ result[0], result[1] }.dot( direction ) < 0 )
            {
                std::swap( result[0], result[1] );
            }
        }
        CorrectnessInfo< absl::InlinedVector< Point3D, 2 > >::Correctness
            first_correctness{ true, {} };
        CorrectnessInfo< absl::InlinedVector< Point3D, 2 > >::Correctness
            second_correctness{ true, {} };
        for( const auto& intersection : result )
        {
            auto plane_output = point_plane_distance( intersection, plane );
            first_correctness.first =
                first_correctness.first
                && std::get< 0 >( plane_output ) <= GLOBAL_EPSILON;
            first_correctness.second.emplace_back(
                std::move( std::get< 1 >( plane_output ) ) );
            auto circle_output = point_circle_distance( intersection, circle );
            second_correctness.first =
                second_correctness.first
                && std::get< 0 >( circle_output ) <= GLOBAL_EPSILON;
            second_correctness.second.emplace_back(
                std::move( std::get< 1 >( circle_output ) ) );
        }
        return { std::move( result ),
            { first_correctness, second_correctness } };
    }

    IntersectionResult< OwnerInfiniteLine3D > plane_plane_intersection(
        const Plane& plane0, const Plane& plane1 )
    {
        const auto& normal0 = plane0.normal();
        const auto& normal1 = plane1.normal();
        const auto direction = normal0.cross( normal1 );
        const auto sine = direction.length();
        // Height of the plane0 origin relative to plane1
        const auto height =
            normal1.dot( Vector3D{ plane1.origin(), plane0.origin() } );
        if( sine <= GLOBAL_EPSILON )
        {
            // Parallel planes (as in line_plane_intersection)
            if( std::fabs( height ) <= GLOBAL_EPSILON )
            {
                // The planes are coplanar.
                return { INTERSECTION_TYPE::parallel };
            }
            // The planes are parallel but distinct.
            return { INTERSECTION_TYPE::none };
        }
        // Move the plane0 origin in plane0, orthogonally to the line, up to
        // plane1: its height decreases by sine per unit length
        const auto unit_direction = direction / sine;
        const auto in_plane0 = normal0.cross( unit_direction );
        OwnerInfiniteLine3D line{ unit_direction,
            plane0.origin() + in_plane0 * ( height / sine ) };
        const auto compute_corectness = [&line]( const Plane& plane )
            -> CorrectnessInfo< OwnerInfiniteLine3D >::Correctness {
            auto output = point_plane_distance( line.origin(), plane );
            return { std::get< 0 >( output ) <= GLOBAL_EPSILON,
                OwnerInfiniteLine3D{
                    line.direction(), std::move( std::get< 1 >( output ) ) } };
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
        if( segment.is_degenerated() )
        {
            const auto point = segment.barycenter();
            const auto [distance, projection] =
                point_ellipse_distance( point, ellipse );
            return degenerated_segment_intersection<
                absl::InlinedVector< Point< dimension >, 2 > >(
                point, distance, projection );
        }
        auto line_intersections = line_ellipse_intersection< dimension >(
            InfiniteLine< dimension >{ segment }, ellipse );
        if( line_intersections )
        {
            return filter_segment_intersections(
                std::move( line_intersections ), segment );
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