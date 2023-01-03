/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <memory>

#include <geode/mesh/core/mesh_id.h>

#include <geode/model/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Corner );
    FORWARD_DECLARATION_DIMENSION_CLASS( Corners );
    FORWARD_DECLARATION_DIMENSION_CLASS( PointSet );
    FORWARD_DECLARATION_DIMENSION_CLASS( PointSetBuilder );

    struct uuid;
} // namespace geode

namespace geode
{
    template < index_t dimension >
    class CornersBuilder
    {
    public:
        void load_corners( absl::string_view directory );

        /*!
         * Get a pointer to the builder of a Corner mesh
         * @param[in] id Unique index of the Corner
         */
        std::unique_ptr< PointSetBuilder< dimension > > corner_mesh_builder(
            const uuid& id );

        void set_corner_name( const uuid& id, absl::string_view name );

    protected:
        CornersBuilder( Corners< dimension >& corners ) : corners_( corners ) {}

        const uuid& create_corner();

        const uuid& create_corner( const MeshImpl& impl );

        void delete_corner( const Corner< dimension >& corner );

        void set_corner_mesh(
            const uuid& id, std::unique_ptr< PointSet< dimension > > mesh );

    private:
        Corners< dimension >& corners_;
    };
    ALIAS_2D_AND_3D( CornersBuilder );
} // namespace geode
