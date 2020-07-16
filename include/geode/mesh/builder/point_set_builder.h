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

#include <vector>

#include <geode/basic/passkey.h>

#include <geode/mesh/builder/vertex_set_builder.h>
#include <geode/mesh/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Point );
    FORWARD_DECLARATION_DIMENSION_CLASS( PointSet );
} // namespace geode

namespace geode
{
    /*!
     * Interface class to represent the builder of a PointSet
     */
    template < index_t dimension >
    class PointSetBuilder : public VertexSetBuilder
    {
        OPENGEODE_TEMPLATE_ASSERT_2D_OR_3D( dimension );
        PASSKEY( PointSet< dimension >, BuilderKey );

    public:
        /*!
         * Create the builder associated with a PointSet.
         * @param[in] mesh The PointSet to build/modify
         */
        static std::unique_ptr< PointSetBuilder< dimension > > create(
            PointSet< dimension >& mesh );

        /*!
         * Set coordinates to a vertex. This vertex should be created before.
         * @param[in] vertex_id The vertex, in [0, nb_vertices()-1].
         * @param[in] point The vertex coordinates
         */
        void set_point( index_t vertex_id, const Point< dimension >& point );

        /*!
         * Create a new point with associated coordinates.
         * @param[in] point The point to create
         * @return the index of the created point
         */
        index_t create_point( const Point< dimension >& point );

        void copy( const PointSet< dimension >& point_set, BuilderKey )
        {
            copy( point_set );
        }

        void set_mesh( PointSet< dimension >& mesh, MeshBuilderFactoryKey key );

    protected:
        PointSetBuilder() = default;

        void copy( const PointSet< dimension >& point_set );

    private:
        virtual void do_set_point(
            index_t vertex_id, const Point< dimension >& point ) = 0;

    private:
        PointSet< dimension >* point_set_;
    };
    ALIAS_2D_AND_3D( PointSetBuilder );
} // namespace geode
