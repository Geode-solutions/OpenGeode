/*
 * Copyright (c) 2019 - 2020 Geode-solutions
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

#include <geode/basic/factory.h>

#include <geode/mesh/common.h>
#include <geode/mesh/core/vertex_set.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( BoundingBox );

    class AttributeManager;
} // namespace geode

namespace geode
{
    /*!
     * Interface class to represent a set of 2D or 3D points.
     * A point is a vertex associated to spatial coordinates
     */
    template < index_t dimension >
    class PointSet : public VertexSet
    {
        OPENGEODE_TEMPLATE_ASSERT_2D_OR_3D( dimension );

    public:
        /*!
         * Create a new PointSet using default data structure.
         */
        static std::unique_ptr< PointSet< dimension > > create();

        /*!
         * Create a new PointSet using a specified data structure.
         * @param[in] impl Data structure implementation
         */
        static std::unique_ptr< PointSet< dimension > > create(
            const MeshImpl& impl );

        static MeshType type_name_static()
        {
            return MeshType{ absl::StrCat( "PointSet", dimension, "D" ) };
        }

        std::unique_ptr< PointSet< dimension > > clone() const;

        const Point< dimension >& point( index_t vertex_id ) const;

        /*!
         * Compute the bounding box from mesh vertices
         */
        BoundingBox< dimension > bounding_box() const;

    protected:
        PointSet() = default;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

        virtual const Point< dimension >& get_point(
            index_t vertex_id ) const = 0;
    };
    ALIAS_2D_AND_3D( PointSet );
} // namespace geode
