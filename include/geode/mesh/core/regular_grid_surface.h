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

#pragma once

#include <geode/mesh/common.h>
#include <geode/mesh/core/grid.h>
#include <geode/mesh/core/surface_mesh.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( RegularGrid );
    ALIAS_2D( RegularGrid );
} // namespace geode

namespace geode
{
    template <>
    class opengeode_mesh_api RegularGrid< 2 > : public SurfaceMesh< 2 >,
                                                public Grid< 2 >
    {
        OPENGEODE_DISABLE_COPY( RegularGrid );
        friend class bitsery::Access;

    public:
        /*!
         * Create a new RegularGrid using default data structure.
         */
        static std::unique_ptr< RegularGrid2D > create();

        /*!
         * Create a new RegularGrid using a specified data structure.
         * @param[in] impl Data structure implementation
         */
        static std::unique_ptr< RegularGrid2D > create( const MeshImpl& impl );

        absl::string_view native_extension() const
        {
            return native_extension_static();
        }

        static absl::string_view native_extension_static()
        {
            static const auto ext = "og_rgd2d";
            return ext;
        }

        std::unique_ptr< RegularGrid2D > clone() const;

    protected:
        RegularGrid() = default;

        RegularGrid( std::array< index_t, 2 > cells_number,
            std::array< double, 2 > cells_length )
            : Grid< 2 >{ std::move( cells_number ), std::move( cells_length ) }
        {
        }

        RegularGrid(
            std::array< index_t, 2 > cells_number, double cells_length )
            : Grid< 2 >{ std::move( cells_number ), cells_length }
        {
        }

    private:
        template < typename Archive >
        void serialize( Archive& archive );

        local_index_t get_nb_polygon_vertices( index_t /*unused*/ ) const final
        {
            return nb_cell_vertices();
        }
    };
} // namespace geode
