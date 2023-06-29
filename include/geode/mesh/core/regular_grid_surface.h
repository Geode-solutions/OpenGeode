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
#include <geode/mesh/core/grid.h>
#include <geode/mesh/core/surface_mesh.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( RegularGrid );
    FORWARD_DECLARATION_DIMENSION_CLASS( RegularGridBuilder );
    ALIAS_2D( RegularGrid );
} // namespace geode

namespace geode
{
    template <>
    class opengeode_mesh_api RegularGrid< 2 > : public SurfaceMesh< 2 >,
                                                public Grid< 2 >
    {
        friend class bitsery::Access;

    public:
        using Builder = RegularGridBuilder< 2 >;
        static constexpr index_t dim{ 2 };

        /*!
         * Create a new RegularGrid using default data structure.
         */
        static std::unique_ptr< RegularGrid2D > create();

        /*!
         * Create a new RegularGrid using a specified data structure.
         * @param[in] impl Data structure implementation
         */
        static std::unique_ptr< RegularGrid2D > create( const MeshImpl& impl );

        static MeshType type_name_static();

        absl::string_view native_extension() const
        {
            return native_extension_static();
        }

        static absl::string_view native_extension_static()
        {
            static const auto ext = "og_rgd2d";
            return ext;
        }

        AttributeManager& cell_attribute_manager() const override;

        std::unique_ptr< RegularGrid2D > clone() const;

    protected:
        RegularGrid() = default;
        RegularGrid( RegularGrid&& other ) = default;
        RegularGrid& operator=( RegularGrid&& other ) = default;

    private:
        template < typename Archive >
        void serialize( Archive& archive );

        local_index_t get_nb_polygon_vertices( index_t /*unused*/ ) const final
        {
            return nb_cell_vertices();
        }
    };
} // namespace geode
