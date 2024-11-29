/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/geometry/basic_objects/polygon.hpp>

#include <optional>

#include <mapbox/earcut.hpp>

#include <geode/basic/logger.hpp>

#include <absl/algorithm/container.h>

#include <geode/geometry/barycentric_coordinates.hpp>
#include <geode/geometry/basic_objects/infinite_line.hpp>
#include <geode/geometry/basic_objects/plane.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/distance.hpp>

namespace mapbox
{
    namespace util
    {
        template < std::size_t coord, geode::index_t dimension >
        struct nth< coord, geode::Point< dimension > >
        {
            inline static auto get( const geode::Point< dimension >& point )
            {
                return point.value( coord );
            };
        };

        template < std::size_t coord, geode::index_t dimension >
        struct nth< coord, geode::RefPoint< dimension > >
        {
            inline static auto get( const geode::RefPoint< dimension >& point )
            {
                return point.get().value( coord );
            };
        };
    } // namespace util
} // namespace mapbox

namespace
{
    template < typename PointType >
    std::array< std::vector< PointType >, 1 > polygon_points(
        const geode::GenericPolygon< PointType, 2 >& polygon )
    {
        return { { polygon.vertices() } };
    }

    template < typename PointType >
    std::array< absl::FixedArray< geode::Point2D >, 1 > polygon_points(
        const geode::GenericPolygon< PointType, 3 >& polygon )
    {
        const auto& vertices = polygon.vertices();
        std::array< absl::FixedArray< geode::Point2D >, 1 > polygons{
            absl::FixedArray< geode::Point2D >( vertices.size() )
        };
        const auto normal =
            polygon.normal().value_or( geode::Vector3D{ { 0, 0, 1 } } );
        const auto axis_to_remove = normal.most_meaningful_axis();
        for( const auto v : geode::LIndices{ vertices } )
        {
            const geode::Point3D& point = vertices[v];
            polygons[0][v] = point.project_point( axis_to_remove );
        }
        return polygons;
    }
} // namespace

namespace geode
{
    template < typename PointType, index_t dimension >
    GenericPolygon< PointType, dimension >::GenericPolygon(
        std::vector< PointType > vertices ) noexcept
        : vertices_{ std::move( vertices ) }
    {
    }

    template < typename PointType, index_t dimension >
    GenericPolygon< PointType, dimension >::GenericPolygon(
        const GenericPolygon< PointType, dimension >& ) noexcept = default;

    template < typename PointType, index_t dimension >
    GenericPolygon< PointType, dimension >&
        GenericPolygon< PointType, dimension >::operator=(
            const GenericPolygon< PointType, dimension >& ) noexcept = default;

    template < typename PointType, index_t dimension >
    GenericPolygon< PointType, dimension >::GenericPolygon(
        GenericPolygon< PointType, dimension >&& ) noexcept = default;

    template < typename PointType, index_t dimension >
    GenericPolygon< PointType, dimension >&
        GenericPolygon< PointType, dimension >::operator=(
            GenericPolygon< PointType, dimension >&& ) noexcept = default;

    template < typename PointType, index_t dimension >
    Point< dimension >
        GenericPolygon< PointType, dimension >::barycenter() const
    {
        Point< dimension > barycenter;
        for( const auto& point : vertices_ )
        {
            barycenter += point;
        }
        return barycenter / nb_vertices();
    }

    template < typename PointType, index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3, std::optional< Vector3D > >::type
        GenericPolygon< PointType, dimension >::normal() const
    {
        Vector3D normal;
        const auto& p0 = vertices_[0];
        for( const auto v : Range{ 2, nb_vertices() } )
        {
            const auto& p1 = vertices_[v - 1];
            const auto& p2 = vertices_[v];
            if( const auto triangle_normal =
                    Triangle< T >{ p0, p1, p2 }.normal() )
            {
                normal += triangle_normal.value();
            }
        }
        try
        {
            return normal.normalize();
        }
        catch( const OpenGeodeException& /*unused*/ )
        {
            return std::nullopt;
        }
    }

    template < typename PointType, index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3, std::optional< Plane > >::type
        GenericPolygon< PointType, dimension >::plane() const
    {
        if( const auto polygon_normal = this->normal() )
        {
            return std::optional< Plane >{ std::in_place,
                polygon_normal.value(), vertices_[0] };
        }
        return std::nullopt;
    }

    template < typename PointType, index_t dimension >
    template < index_t T >
    typename std::enable_if< T == 3, std::optional< OwnerPlane > >::type
        GenericPolygon< PointType, dimension >::owner_plane() const
    {
        if( const auto polygon_normal = this->normal() )
        {
            return std::optional< OwnerPlane >{ std::in_place,
                polygon_normal.value(), vertices_[0] };
        }
        return std::nullopt;
    }

    template < typename PointType, index_t dimension >
    void GenericPolygon< PointType, dimension >::set_point(
        index_t vertex, PointType point )
    {
        vertices_[vertex] = point;
    }

    template < typename PointType, index_t dimension >
    const std::vector< PointType >&
        GenericPolygon< PointType, dimension >::vertices() const
    {
        return vertices_;
    }

    template < typename PointType, index_t dimension >
    BoundingBox< dimension >
        GenericPolygon< PointType, dimension >::bounding_box() const
    {
        BoundingBox< dimension > bbox;
        for( const auto& point : vertices_ )
        {
            bbox.add_point( point );
        }
        return bbox;
    }

    template < typename PointType, index_t dimension >
    index_t GenericPolygon< PointType, dimension >::nb_vertices() const
    {
        return vertices_.size();
    }

    template < typename PointType, index_t dimension >
    std::vector< std::array< index_t, 3 > >
        GenericPolygon< PointType, dimension >::triangulate() const
    {
        const std::array polygons{ vertices_ };
        const auto new_triangles =
            mapbox::earcut< index_t >( polygon_points( *this ) );
        const auto nb_new_triangles = new_triangles.size() / 3;
        std::vector< std::array< index_t, 3 > > result;
        result.reserve( nb_new_triangles );
        for( const auto trgl : Range{ nb_new_triangles } )
        {
            result.emplace_back( std::array< index_t, 3 >{
                new_triangles[3 * trgl], new_triangles[3 * trgl + 1],
                new_triangles[3 * trgl + 2] } );
        }
        return result;
    }

    template < typename PointType, index_t dimension >
    bool GenericPolygon< PointType, dimension >::is_degenerated() const
    {
        double max_length{ 0. };
        index_t max_length_edge{ 0 };
        for( const auto e : Range{ nb_vertices() } )
        {
            const Point< dimension >& point0 = vertices_[e];
            const Point< dimension >& point1 =
                vertices_[e == nb_vertices() - 1 ? 0 : e + 1];
            const auto cur_length = point_point_distance( point0, point1 );
            if( cur_length > max_length )
            {
                max_length = cur_length;
                max_length_edge = e;
            }
        }
        if( max_length < GLOBAL_EPSILON )
        {
            return true;
        }
        const auto next =
            max_length_edge + 1 == nb_vertices() ? 0 : max_length_edge + 1;
        const InfiniteLine< dimension > line{ Segment< dimension >{
            vertices_[max_length_edge], vertices_[next] } };
        for( const auto v : Range{ nb_vertices() } )
        {
            if( v == max_length_edge || v == next )
            {
                continue;
            }
            const Point< dimension >& point = vertices_[v];
            if( point_line_distance( point, line ) > GLOBAL_EPSILON )
            {
                return false;
            }
        }
        return true;
    }

    template < typename PointType, index_t dimension >
    std::string GenericPolygon< PointType, dimension >::string() const
    {
        std::string result{ "[" };
        auto sep = "";
        for( const Point< dimension >& point : vertices_ )
        {
            absl::StrAppend( &result, sep, point.string() );
            sep = ", ";
        }
        result += "]";
        return result;
    }

    template < index_t dimension >
    OwnerPolygon< dimension >::OwnerPolygon(
        std::vector< Point< dimension > > vertices ) noexcept
        : Base{ std::move( vertices ) }
    {
    }
    template < index_t dimension >
    OwnerPolygon< dimension >::OwnerPolygon(
        const OwnerPolygon< dimension >& ) noexcept = default;
    template < index_t dimension >
    OwnerPolygon< dimension >& OwnerPolygon< dimension >::operator=(
        const OwnerPolygon< dimension >& ) noexcept = default;
    template < index_t dimension >
    OwnerPolygon< dimension >::OwnerPolygon(
        OwnerPolygon< dimension >&& ) noexcept = default;
    template < index_t dimension >
    OwnerPolygon< dimension >& OwnerPolygon< dimension >::operator=(
        OwnerPolygon< dimension >&& ) noexcept = default;

    template < index_t dimension >
    Polygon< dimension >::Polygon(
        std::vector< RefPoint< dimension > > vertices ) noexcept
        : Base{ std::move( vertices ) }
    {
    }
    template < index_t dimension >
    Polygon< dimension >::Polygon(
        const Polygon< dimension >& ) noexcept = default;
    template < index_t dimension >
    Polygon< dimension >& Polygon< dimension >::operator=(
        const Polygon< dimension >& ) noexcept = default;
    template < index_t dimension >
    Polygon< dimension >::Polygon( Polygon< dimension >&& ) noexcept = default;
    template < index_t dimension >
    Polygon< dimension >& Polygon< dimension >::operator=(
        Polygon< dimension >&& ) noexcept = default;

    template class opengeode_geometry_api GenericPolygon< Point< 2 >, 2 >;
    template class opengeode_geometry_api GenericPolygon< Point< 3 >, 3 >;
    template class opengeode_geometry_api GenericPolygon< RefPoint< 2 >, 2 >;
    template class opengeode_geometry_api GenericPolygon< RefPoint< 3 >, 3 >;
    template class opengeode_geometry_api OwnerPolygon< 2 >;
    template class opengeode_geometry_api OwnerPolygon< 3 >;
    template class opengeode_geometry_api Polygon< 2 >;
    template class opengeode_geometry_api Polygon< 3 >;

    template opengeode_geometry_api std::optional< Vector3D >
        GenericPolygon< Point< 3 >, 3 >::normal< 3 >() const;
    template opengeode_geometry_api std::optional< Plane >
        GenericPolygon< Point< 3 >, 3 >::plane< 3 >() const;
    template opengeode_geometry_api std::optional< OwnerPlane >
        GenericPolygon< Point< 3 >, 3 >::owner_plane< 3 >() const;

    template opengeode_geometry_api std::optional< Vector3D >
        GenericPolygon< RefPoint< 3 >, 3 >::normal< 3 >() const;
    template opengeode_geometry_api std::optional< Plane >
        GenericPolygon< RefPoint< 3 >, 3 >::plane< 3 >() const;
    template opengeode_geometry_api std::optional< OwnerPlane >
        GenericPolygon< RefPoint< 3 >, 3 >::owner_plane< 3 >() const;
} // namespace geode
