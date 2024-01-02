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

#include <geode/geometry/radial_sort.h>

#include <absl/algorithm/container.h>

#include <geode/geometry/basic_objects/plane.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/position.h>
#include <geode/geometry/private/position_from_sides.h>
#include <geode/geometry/private/predicates.h>
#include <geode/geometry/projection.h>

namespace
{
    class RadialSort
    {
    public:
        RadialSort( const geode::Segment3D& segment,
            absl::Span< const geode::Point3D > points )
            : segment_( segment ),
              plane_{ segment.direction(), segment.vertices()[0].get() },
              points_( points )
        {
        }

        absl::FixedArray< geode::index_t > sort()
        {
            absl::FixedArray< geode::index_t > result( points_.size() );
            absl::c_iota( result, 0 );
            recursive_sort( result, 0, points_.size() - 1 );
            return result;
        }

    private:
        void recursive_sort( absl::FixedArray< geode::index_t >& result,
            geode::index_t begin,
            geode::index_t end )
        {
            const auto size = end - begin + 1;
            if( size < 2 )
            {
                return;
            }
            geode::index_t left_id{ 0 };
            geode::index_t right_id{ size - 1 };
            absl::FixedArray< geode::index_t > sorted( size );
            std::vector< geode::index_t > same;
            for( const auto i : geode::Range{ size - 1 } )
            {
                const auto id = begin + i + 1;
                const auto side = geode::point_side_to_triangle(
                    points_[result[id]], { segment_.vertices()[0].get(),
                                             segment_.vertices()[1].get(),
                                             points_[result[begin]] } );
                if( side == geode::Side::positive )
                {
                    sorted[left_id++] = result[id];
                }
                else if( side == geode::Side::negative )
                {
                    sorted[right_id--] = result[id];
                }
                else
                {
                    const auto proj_begin = geode::point_plane_projection(
                        points_[result[begin]], plane_ );
                    const auto proj = geode::point_plane_projection(
                        points_[result[id]], plane_ );
                    const auto dot0 =
                        GEO::PCK::dot_3d( proj_begin, plane_.origin(), proj );
                    const auto dot1 =
                        GEO::PCK::dot_3d( proj, plane_.origin(), proj_begin );
                    const auto position = geode::detail::point_segment_position(
                        geode::detail::side( dot0 ),
                        geode::detail::opposite_side( dot1 ) );
                    if( position == geode::Position::inside )
                    {
                        sorted[right_id--] = result[id];
                    }
                    else
                    {
                        same.push_back( result[id] );
                    }
                }
            }
            for( const auto id : same )
            {
                sorted[left_id++] = id;
            }
            OPENGEODE_ASSERT( left_id == right_id,
                "[RadialSort::recursive_sort] left and "
                "right ids are different" );
            sorted[left_id] = result[begin];
            absl::c_copy( sorted, result.begin() + begin );
            const auto left_middle_id = begin + left_id - same.size();
            if( left_middle_id != begin )
            {
                recursive_sort( result, begin, left_middle_id - 1 );
            }
            const auto right_middle_id = begin + left_id;
            if( right_middle_id != end )
            {
                recursive_sort( result, right_middle_id + 1, end );
            }
        }

    private:
        const geode::Segment3D& segment_;
        DEBUG_CONST geode::Plane plane_;
        absl::Span< const geode::Point3D > points_;
    };
} // namespace

namespace geode
{
    absl::FixedArray< index_t > radial_sort(
        const Segment3D& segment, absl::Span< const Point3D > points )
    {
        RadialSort radial_sort{ segment, points };
        return radial_sort.sort();
    }
} // namespace geode
