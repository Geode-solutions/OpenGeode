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

#pragma once

#include <geode/mesh/common.hpp>
#include <geode/mesh/core/coordinate_reference_system_managers.hpp>
#include <geode/mesh/core/vertex_set.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundingBox );
    FORWARD_DECLARATION_DIMENSION_CLASS( PointSetBuilder );

    class AttributeManager;
} // namespace geode

namespace geode
{
    /*!
     * Interface class to represent a set of 2D or 3D points.
     * A point is a vertex associated to spatial coordinates
     */
    template < index_t dimension >
    class PointSet : public VertexSet,
                     public CoordinateReferenceSystemManagers< dimension >
    {
    public:
        using Builder = PointSetBuilder< dimension >;
        static constexpr auto dim = dimension;

        /*!
         * Create a new PointSet using default data structure.
         */
        [[nodiscard]] static std::unique_ptr< PointSet< dimension > > create();

        /*!
         * Create a new PointSet using a specified data structure.
         * @param[in] impl Data structure implementation
         */
        [[nodiscard]] static std::unique_ptr< PointSet< dimension > > create(
            const MeshImpl& impl );

        [[nodiscard]] static MeshType type_name_static();

        [[nodiscard]] std::unique_ptr< PointSet< dimension > > clone() const;

        /*!
         * Compute the bounding box from mesh vertices
         */
        [[nodiscard]] BoundingBox< dimension > bounding_box() const;

    protected:
        PointSet() = default;
        PointSet( PointSet&& other ) noexcept = default;
        PointSet& operator=( PointSet&& other ) noexcept = default;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );
    };
    ALIAS_1D_AND_2D_AND_3D( PointSet );
} // namespace geode
