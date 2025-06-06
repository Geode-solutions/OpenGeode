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

#pragma once

#include <geode/basic/mapping.hpp>
#include <geode/basic/pimpl.hpp>

#include <geode/geometry/basic_objects/ellipse.hpp>
#include <geode/geometry/common.hpp>
#include <geode/geometry/point.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Frame );

    /*!
     * Given a list of points, this class returns neighboring points.
     */
    template < index_t dimension >
    class NNSearch
    {
        OPENGEODE_DISABLE_COPY( NNSearch );
        OPENGEODE_TEMPLATE_ASSERT_2D_OR_3D( dimension );

    public:
        struct ColocatedInfo
        {
            ColocatedInfo() = default;
            [[nodiscard]] index_t nb_unique_points() const
            {
                return unique_points.size();
            }

            [[nodiscard]] index_t nb_colocated_points() const
            {
                return colocated_mapping.size() - nb_unique_points();
            }

            [[nodiscard]] geode::GenericMapping< geode::index_t >
                points_mapping() const
            {
                geode::GenericMapping< geode::index_t > mapping;
                for( const auto& p : geode::Indices{ colocated_mapping } )
                {
                    mapping.map( p, colocated_mapping[p] );
                }
                return mapping;
            }

            [[nodiscard]] geode::GenericMapping< geode::index_t >
                vertices_mapping() const
            {
                geode::GenericMapping< geode::index_t > mapping;
                for( const auto& p : geode::Indices{ colocated_input_points } )
                {
                    mapping.map( p, colocated_input_points[p] );
                }
                return mapping;
            }

            std::vector< Point< dimension > > unique_points;
            /*!
             * This list has the size of the number of points in the tree.
             * Each index is pointing to its new unique point stored in the
             * unique_points vector.
             */
            std::vector< index_t > colocated_mapping;
            /*!
             * This list has the size of the number of points in the tree.
             * Each index is pointing to its old point index as given in input
             * vector.
             */
            std::vector< index_t > colocated_input_points;
        };

    public:
        explicit NNSearch( std::vector< Point< dimension > > points );
        NNSearch( NNSearch&& other ) noexcept;
        ~NNSearch();

        [[nodiscard]] index_t nb_points() const;

        [[nodiscard]] const Point< dimension >& point( index_t index ) const;

        /*!
         * Get the closest neighbor from the given point
         * @param[in] point The requested point
         * @return the index of the closest point
         */
        [[nodiscard]] index_t closest_neighbor(
            const Point< dimension >& point ) const;

        /*!
         * Get the neighbors closer than a given distance from the given point
         * or within a sphere
         * @param[in] point The center of the sphere
         * @param[in] threshold_distance The radius of the sphere
         * @return the list of points inside this distance
         */
        [[nodiscard]] std::vector< index_t > radius_neighbors(
            const Point< dimension >& point, double threshold_distance ) const;

        /*!
         * Get the neighbors within an ellipse described by its frame, centered
         * on the given point
         * @param[in] point The center of the ellipse
         * @param[in] epsilons_frame The frame describing the ellipse
         * @return the list of points inside this distance
         */
        [[nodiscard]] std::vector< index_t > frame_neighbors(
            const Point< dimension >& point,
            const Frame< dimension >& epsilons_frame ) const;

        /*!
         * Get a number of close neighbors from the given point
         * @param[in] point The requested point
         * @param[in] nb_neighbors The number of neighbors to return
         * @return the list of points, it can be smaller than the requested
         * number of neighbors if there is less neighbors than points in the
         * tree
         */
        [[nodiscard]] std::vector< index_t > neighbors(
            const Point< dimension >& point, index_t nb_neighbors ) const;

        /*!
         * Compute a colocation mapping from the list of points
         * @param[in] epsilon The approximation allowed to test if two points
         * are identical
         * @return The information related to this colocated operation
         */
        [[nodiscard]] ColocatedInfo colocated_index_mapping(
            double epsilon ) const;

        [[nodiscard]] ColocatedInfo colocated_index_mapping(
            const Frame< dimension >& epsilon ) const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( NNSearch );
} // namespace geode
