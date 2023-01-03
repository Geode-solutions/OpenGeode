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

#include <geode/mesh/common.h>
#include <geode/mesh/core/surface_mesh.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Triangle );
    FORWARD_DECLARATION_DIMENSION_CLASS( TriangulatedSurfaceBuilder );
} // namespace geode

namespace geode
{
    /*!
     * Interface class for Surface of arbitrary dimension made up with triangles
     * only.
     */
    template < index_t dimension >
    class TriangulatedSurface : public SurfaceMesh< dimension >
    {
    public:
        using Builder = TriangulatedSurfaceBuilder< dimension >;

        /*!
         * Create a new TriangulatedSurface using default data structure.
         */
        static std::unique_ptr< TriangulatedSurface< dimension > > create();

        /*!
         * Create a new TriangulatedSurface using a specified data structure.
         * @param[in] impl Data structure implementation
         */
        static std::unique_ptr< TriangulatedSurface< dimension > > create(
            const MeshImpl& impl );

        static MeshType type_name_static()
        {
            return MeshType{ absl::StrCat(
                "TriangulatedSurface", dimension, "D" ) };
        }

        std::unique_ptr< TriangulatedSurface< dimension > > clone() const;

        Triangle< dimension > triangle( index_t triangle_id ) const;

    protected:
        TriangulatedSurface() = default;

    private:
        friend class bitsery::Access;
        template < typename Archive >
        void serialize( Archive& archive );

        local_index_t get_nb_polygon_vertices( index_t /*unused*/ ) const final
        {
            return 3;
        }
    };
    ALIAS_2D_AND_3D( TriangulatedSurface );
} // namespace geode
